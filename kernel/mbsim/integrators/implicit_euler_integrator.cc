/* Copyright (C) 2004-2009 MBSim Development Team
 *
 * This library is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU Lesser General Public 
 * License as published by the Free Software Foundation; either 
 * version 2.1 of the License, or (at your option) any later version. 
 *  
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * Lesser General Public License for more details. 
 *  
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * Contact: martin.o.foerg@googlemail.com
 */

#include <config.h>
#include <mbsim/dynamic_system_solver.h>
#include "implicit_euler_integrator.h"
#include "mbsim/utils/nonlinear_algebra.h"
#include <ctime>

#ifndef NO_ISO_14882
using namespace std;
#endif

using namespace fmatvec;
using namespace MBSim;
using namespace MBXMLUtils;
using namespace xercesc;

namespace MBSim {

  MBSIM_OBJECTFACTORY_REGISTERCLASS(MBSIM, ImplicitEulerIntegrator)

  Vec ImplicitEulerIntegrator::ResiduumFull::operator()(const Vec &z) {
    Vec res;
    sys->setState(z);
    sys->resetUpToDate();
    res =  z - zk - sys->evalzd()*dt;
    return res;
  } 

  Vec ImplicitEulerIntegrator::ResiduumReduced::operator()(const Vec &ux) {
    Vec res;
    Vec z(sys->getzSize(),NONINIT);
    z(0,sys->getqSize()-1) = zk(0,sys->getqSize()-1) + ux(0,sys->getuSize()-1)*dt;
    z(sys->getqSize(),sys->getzSize()-1) = ux;
    sys->setState(z);
    sys->resetUpToDate();
    res =  ux - zk(sys->getqSize(),sys->getzSize()-1) - sys->evalzd()(sys->getqSize(),sys->getzSize()-1)*dt;
    return res;
  }

  void ImplicitEulerIntegrator::preIntegrate() {
    debugInit();
    assert(dtPlot >= dt);

    if(reduced)
      res = new ResiduumReduced(system,dt);
    else
      res = new ResiduumFull(system,dt);

    system->setTime(tStart);

    if(z0.size()) {
      if(z0.size() != system->getzSize())
        throwError("(ImplicitEulerIntegrator::integrate): size of z0 does not match, must be " + to_string(system->getzSize()));
      system->setState(z0);
    }
    else
      system->evalz0();

    // Perform a projection of generalized positions and velocities at time t=0
    if(system->getInitialProjection()) {
      system->projectGeneralizedPositions(2,true);
      system->projectGeneralizedVelocities(2);
    }

    tPlot = 0.;
    
    stepPlot =(int) (dtPlot/dt + 0.5);
    assert(fabs(stepPlot*dt - dtPlot) < dt*dt);
    
    step = 0;
    integrationSteps = 0;
    
    s0 = clock();
    time = 0;
  }

  void ImplicitEulerIntegrator::subIntegrate(double tStop) {
    MultiDimNewtonMethod newton(res);
    // newton.setLinearAlgebra(1);
    while(system->getTime()<tStop) { // time loop
      res->setState(system->getState());
      integrationSteps++;
      if((step*stepPlot - integrationSteps) < 0) {
        step++;
        system->resetUpToDate();
        system->plot();
        double s1 = clock();
        time += (s1-s0)/CLOCKS_PER_SEC;
        s0 = s1;
        if(msgAct(Status)) msg(Status) << "   t = " << system->getTime() << ",\tdt = "<< dt << flush;
        tPlot += dtPlot;
      }

      system->getTime() += dt;
      if(reduced) {
        Vec qOld;
        qOld = system->getq();
        system->getState()(system->getqSize(),system->getzSize()-1) = newton.solve(system->getState()(system->getqSize(),system->getzSize()-1));
        system->getq() = qOld + system->getu()*dt;
      }
      else
        system->getState() = newton.solve(system->getState());
      if(newton.getInfo() != 0)
        throwError("(ImplicitEulerIntegrator::subIntegrate): computation of new state failed!");
    }
  }

  void ImplicitEulerIntegrator::postIntegrate() {
    delete res;
  }

  void ImplicitEulerIntegrator::integrate() {
    preIntegrate();
    subIntegrate(tEnd);
    postIntegrate();
  }

  void ImplicitEulerIntegrator::initializeUsingXML(DOMElement *element) {
    Integrator::initializeUsingXML(element);
    DOMElement *e;
    e=E(element)->getFirstElementChildNamed(MBSIM%"stepSize");
    if(e) setStepSize(E(e)->getText<double>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"reducedForm");
    if(e) setReducedForm((E(e)->getText<bool>()));
  }

}
