#include "system.h"
#include <mbsim/integrators/integrators.h>

using namespace std;
using namespace MBSim;

bool rigidContact;

int main (int argc, char* argv[]) {

  char dummy[10000];
  double tEnd, dt, dtPlot;
  bool eventDriven;

  // Beginn input
  ifstream is("input.asc");
  is >> rigidContact;
  is.getline(dummy,10000);
  is >> eventDriven;
  is.getline(dummy,10000);
  is >> tEnd;
  is.getline(dummy,10000);
  is >> dtPlot;
  is.getline(dummy,10000);
  is >> dt;
  is.getline(dummy,10000);
  is.close();

  System *sys = new System("TS");
  sys->setStopIfNoConvergence(true);
  sys->initialize();

  Integrator* integrator;

  if(!rigidContact) {
    integrator = new RADAU5Integrator;
  } 
  else if(eventDriven) { // Event driven time integration
    sys->setProjectionTolerance(1e-15);
    sys->setGeneralizedRelativePositionTolerance(1e-6);
    sys->setGeneralizedRelativeVelocityTolerance(1e-6);
    sys->setGeneralizedImpulseTolerance(1e-6);
    sys->setGeneralizedForceTolerance(1e-8);
    sys->setGeneralizedRelativeAccelerationTolerance(1e-8);
    integrator = new LSODEIntegrator;
    static_cast<LSODEIntegrator*>(integrator)->setInitialStepSize(1e-8);
    static_cast<LSODEIntegrator*>(integrator)->setAbsoluteTolerance(1e-7);
    static_cast<LSODEIntegrator*>(integrator)->setRelativeTolerance(1e-7);
    static_cast<LSODEIntegrator*>(integrator)->setToleranceForPositionConstraints(1e-5);
    static_cast<LSODEIntegrator*>(integrator)->setToleranceForVelocityConstraints(1e-5);
  } 
  else { // time stepping integration
    sys->setGeneralizedImpulseTolerance(1e-2*dt);
    sys->setGeneralizedRelativeVelocityTolerance(1e-8);
    integrator = new TimeSteppingIntegrator;
    static_cast<TimeSteppingIntegrator*>(integrator)->setStepSize(dt);
  }

  integrator->setEndTime(tEnd);
  integrator->setPlotStepSize(dtPlot);
  integrator->integrate(*sys);

  cout << "finished"<<endl;

  delete integrator;
  delete sys;

  return 0;

}

