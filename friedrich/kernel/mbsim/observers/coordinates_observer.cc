/* Copyright (C) 2004-2011 MBSim Development Team
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
 * Contact: martin.o.foerg@gmail.com
 */

#include <config.h>
#include "mbsim/observers/coordinates_observer.h"
#include "mbsim/frame.h"
#include "mbsim/utils/rotarymatrices.h"
#include "mbsim/utils/eps.h"
#ifdef HAVE_OPENMBVCPPINTERFACE
#include <openmbvcppinterface/frame.h>
#endif

using namespace std;
using namespace MBXMLUtils;
using namespace fmatvec;

namespace MBSim {

  CoordinatesObserver::CoordinatesObserver(const std::string &name) : Observer(name), frame(0) {
#ifdef HAVE_OPENMBVCPPINTERFACE
    openMBVPosition=0;
    openMBVVelocity=0;
    openMBVAcceleration=0;
    openMBVFrame=0;
#endif
  }

  void CoordinatesObserver::init(InitStage stage) {
    if(stage==resolveXMLPath) {
      if(saved_frame!="")
        setFrame(getByPath<Frame>(saved_frame));
      Observer::init(stage);
    }
    else if(stage==MBSim::plot) {
      updatePlotFeatures();

      Observer::init(stage);
      if(getPlotFeature(plotRecursive)==enabled) {
#ifdef HAVE_OPENMBVCPPINTERFACE
        if(getPlotFeature(openMBV)==enabled) {
          if(openMBVPosition) {
            openMBVPosGrp=new OpenMBV::Group();
            openMBVPosGrp->setName("Position_Group");
            openMBVPosGrp->setExpand(false);
            getOpenMBVGrp()->addObject(openMBVPosGrp);
            openMBVPosition->setName("Position");
            openMBVPosGrp->addObject(openMBVPosition);
          }
          if(openMBVVelocity) {
            openMBVVelGrp=new OpenMBV::Group();
            openMBVVelGrp->setName("Velocity_Group");
            openMBVVelGrp->setExpand(false);
            getOpenMBVGrp()->addObject(openMBVVelGrp);
            openMBVVelocity->setName("Velocity");
            openMBVVelGrp->addObject(openMBVVelocity);
          }
          if(openMBVAcceleration) {
            openMBVAccGrp=new OpenMBV::Group();
            openMBVAccGrp->setName("Acceleration_Group");
            openMBVAccGrp->setExpand(false);
            getOpenMBVGrp()->addObject(openMBVAccGrp);
            openMBVAcceleration->setName("Acceleration");
            openMBVAccGrp->addObject(openMBVAcceleration);
          }
          if(openMBVFrame) {
            openMBVFrame->setName("Frame");
            getOpenMBVGrp()->addObject(openMBVFrame);
          }
        }
#endif
      }
    }
    else
      Observer::init(stage);
  }

  void CoordinatesObserver::plot(double t, double dt) {
    if(getPlotFeature(plotRecursive)==enabled) {
#ifdef HAVE_OPENMBVCPPINTERFACE
      if(getPlotFeature(openMBV)==enabled) {
        Vec3 r = frame->getPosition();
        Vec3 v = frame->getVelocity();
        Vec3 a = frame->getAcceleration();

        if(openMBVPosition && !openMBVPosition->isHDF5Link()) {
          vector<double> data;
          data.push_back(t);
          data.push_back(0);
          data.push_back(0);
          data.push_back(0);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(0.5);
          openMBVPosition->append(data);
        }

        if(openMBVVelocity && !openMBVVelocity->isHDF5Link()) {
          vector<double> data;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(v(0));
          data.push_back(v(1));
          data.push_back(v(2));
          data.push_back(0.5);
          openMBVVelocity->append(data);
        }

        if(openMBVAcceleration && !openMBVAcceleration->isHDF5Link()) {
          vector<double> data;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(a(0));
          data.push_back(a(1));
          data.push_back(a(2));
          data.push_back(0.5);
          openMBVAcceleration->append(data);
        }
      }
#endif

      Observer::plot(t,dt);
    }
  }

  void CoordinatesObserver::initializeUsingXML(TiXmlElement *element) {
    Observer::initializeUsingXML(element);
    TiXmlElement *e=element->FirstChildElement(MBSIMNS"frame");
    if(e) saved_frame=e->Attribute("ref");
    e=element->FirstChildElement(MBSIMNS"enableOpenMBVPosition");
    if(e) {
        OpenMBVArrow ombv;
        openMBVPosition=ombv.createOpenMBV(e); 
    }
    e=element->FirstChildElement(MBSIMNS"enableOpenMBVVelocity");
    if(e) {
        OpenMBVArrow ombv;
        openMBVVelocity=ombv.createOpenMBV(e); 
    }
    e=element->FirstChildElement(MBSIMNS"enableOpenMBVAcceleration");
    if(e) {
        OpenMBVArrow ombv;
        openMBVAcceleration=ombv.createOpenMBV(e); 
    }
    e=element->FirstChildElement(MBSIMNS"enableOpenMBVFrame");
    if(e) {
        OpenMBVFrame ombv;
        openMBVFrame=ombv.createOpenMBV(e); 
    }
  }

  MBSIM_OBJECTFACTORY_REGISTERXMLNAME(Element, CartesianCoordinatesObserver, MBSIMNS"CartesianCoordinatesObserver")

  CartesianCoordinatesObserver::CartesianCoordinatesObserver(const std::string &name) : CoordinatesObserver(name), A(EYE) {
#ifdef HAVE_OPENMBVCPPINTERFACE
    openMBVXPosition=0;
    openMBVYPosition=0;
    openMBVZPosition=0;
    openMBVXVelocity=0;
    openMBVYVelocity=0;
    openMBVZVelocity=0;
    openMBVXAcceleration=0;
    openMBVYAcceleration=0;
    openMBVZAcceleration=0;
#endif
  }

  void CartesianCoordinatesObserver::init(InitStage stage) {
    if(stage==MBSim::plot) {
      updatePlotFeatures();

      CoordinatesObserver::init(stage);
      if(getPlotFeature(plotRecursive)==enabled) {
#ifdef HAVE_OPENMBVCPPINTERFACE
	ex = A.col(0);
	ey = A.col(1);
	ez = A.col(2);
        if(getPlotFeature(openMBV)==enabled) {
          if(openMBVPosition) {
            openMBVXPosition = new OpenMBV::Arrow(*openMBVPosition);
            openMBVYPosition = new OpenMBV::Arrow(*openMBVPosition);
            openMBVZPosition = new OpenMBV::Arrow(*openMBVPosition);
            openMBVXPosition->setName("XPosition");
            openMBVPosGrp->addObject(openMBVXPosition);
            openMBVYPosition->setName("YPosition");
            openMBVPosGrp->addObject(openMBVYPosition);
            openMBVZPosition->setName("ZPosition");
            openMBVPosGrp->addObject(openMBVZPosition);
          }
          if(openMBVVelocity) {
            openMBVXVelocity = new OpenMBV::Arrow(*openMBVVelocity);
            openMBVYVelocity = new OpenMBV::Arrow(*openMBVVelocity);
            openMBVZVelocity = new OpenMBV::Arrow(*openMBVVelocity);
            openMBVXVelocity->setName("XVelocity");
            openMBVVelGrp->addObject(openMBVXVelocity);
            openMBVYVelocity->setName("YVelocity");
            openMBVVelGrp->addObject(openMBVYVelocity);
            openMBVZVelocity->setName("ZVelocity");
            openMBVVelGrp->addObject(openMBVZVelocity);
          }
          if(openMBVAcceleration) {
            openMBVXAcceleration = new OpenMBV::Arrow(*openMBVAcceleration);
            openMBVYAcceleration = new OpenMBV::Arrow(*openMBVAcceleration);
            openMBVZAcceleration = new OpenMBV::Arrow(*openMBVAcceleration);
            openMBVXAcceleration->setName("XAcceleration");
            openMBVAccGrp->addObject(openMBVXAcceleration);
            openMBVYAcceleration->setName("YAcceleration");
            openMBVAccGrp->addObject(openMBVYAcceleration);
            openMBVZAcceleration->setName("ZAcceleration");
            openMBVAccGrp->addObject(openMBVZAcceleration);
          }
        }
#endif
      }
    }
    else
      CoordinatesObserver::init(stage);
  }

  void CartesianCoordinatesObserver::plot(double t, double dt) {
    if(getPlotFeature(plotRecursive)==enabled) {
#ifdef HAVE_OPENMBVCPPINTERFACE
      if(getPlotFeature(openMBV)==enabled) {
        Vec3 r = frame->getPosition();
        Vec3 v = frame->getVelocity();
        Vec3 a = frame->getAcceleration();

        if(openMBVPosition && !openMBVPosition->isHDF5Link()) {
          vector<double> data;
          Vec3 rx =  (r.T()*ex)*ex;
          data.push_back(t);
          data.push_back(0);
          data.push_back(0);
          data.push_back(0);
          data.push_back(rx(0));
          data.push_back(rx(1));
          data.push_back(rx(2));
          data.push_back(0.5);
          openMBVXPosition->append(data);
          data.clear();
          Vec3 ry =  (r.T()*ey)*ey;
          data.push_back(t);
          data.push_back(0);
          data.push_back(0);
          data.push_back(0);
          data.push_back(ry(0));
          data.push_back(ry(1));
          data.push_back(ry(2));
          data.push_back(0.5);
          openMBVYPosition->append(data);
          data.clear();
          Vec3 rz =  (r.T()*ez)*ez;
          data.push_back(t);
          data.push_back(0);
          data.push_back(0);
          data.push_back(0);
          data.push_back(rz(0));
          data.push_back(rz(1));
          data.push_back(rz(2));
          data.push_back(0.5);
          openMBVZPosition->append(data);
        }

        if(openMBVVelocity && !openMBVVelocity->isHDF5Link()) {
          vector<double> data;
          Vec3 vx =  (v.T()*ex)*ex;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(vx(0));
          data.push_back(vx(1));
          data.push_back(vx(2));
          data.push_back(0.5);
          openMBVXVelocity->append(data);
          data.clear();
          Vec3 vy =  (v.T()*ey)*ey;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(vy(0));
          data.push_back(vy(1));
          data.push_back(vy(2));
          data.push_back(0.5);
          openMBVYVelocity->append(data);
          data.clear();
          Vec3 vz =  (v.T()*ez)*ez;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(vz(0));
          data.push_back(vz(1));
          data.push_back(vz(2));
          data.push_back(0.5);
          openMBVZVelocity->append(data);
        }

        if(openMBVAcceleration && !openMBVAcceleration->isHDF5Link()) {
          vector<double> data;
          Vec3 ax =  (a.T()*ex)*ex;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(ax(0));
          data.push_back(ax(1));
          data.push_back(ax(2));
          data.push_back(0.5);
          openMBVXAcceleration->append(data);
          data.clear();
          Vec3 ay =  (a.T()*ey)*ey;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(ay(0));
          data.push_back(ay(1));
          data.push_back(ay(2));
          data.push_back(0.5);
          openMBVYAcceleration->append(data);
          data.clear();
          Vec3 az =  (a.T()*ez)*ez;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(az(0));
          data.push_back(az(1));
          data.push_back(az(2));
          data.push_back(0.5);
          openMBVZAcceleration->append(data);
        }

        if(openMBVFrame && !openMBVFrame->isHDF5Link()) {
          vector<double> data;
          SqrMat3 AWP;
          AWP.set(0, ex);
          AWP.set(1, ey);
          AWP.set(2, ez);
          data.push_back(t);
          data.push_back(0);
          data.push_back(0);
          data.push_back(0);
          Vec3 cardan=AIK2Cardan(AWP);
          data.push_back(cardan(0));
          data.push_back(cardan(1));
          data.push_back(cardan(2));
          data.push_back(0);
          openMBVFrame->append(data);
        }
      }
#endif

      CoordinatesObserver::plot(t,dt);
    }
  }

  MBSIM_OBJECTFACTORY_REGISTERXMLNAME(Element, CylinderCoordinatesObserver, MBSIMNS"CylinderCoordinatesObserver")

  CylinderCoordinatesObserver::CylinderCoordinatesObserver(const std::string &name) : CoordinatesObserver(name) {
#ifdef HAVE_OPENMBVCPPINTERFACE
    openMBVRadialPosition=0;
    openMBVZPosition=0;
    openMBVRadialVelocity=0;
    openMBVCircularVelocity=0;
    openMBVZVelocity=0;
    openMBVRadialAcceleration=0;
    openMBVCircularAcceleration=0;
    openMBVZAcceleration=0;
#endif
    ez(2) = 1;
  }

  void CylinderCoordinatesObserver::init(InitStage stage) {
    if(stage==MBSim::plot) {
      updatePlotFeatures();

      CoordinatesObserver::init(stage);
      if(getPlotFeature(plotRecursive)==enabled) {
#ifdef HAVE_OPENMBVCPPINTERFACE
        if(getPlotFeature(openMBV)==enabled) {
          if(openMBVPosition) {
            openMBVRadialPosition = new OpenMBV::Arrow(*openMBVPosition);
            openMBVZPosition = new OpenMBV::Arrow(*openMBVPosition);
            openMBVRadialPosition->setName("RadialPosition");
            openMBVPosGrp->addObject(openMBVRadialPosition);
            openMBVZPosition->setName("ZPosition");
            openMBVPosGrp->addObject(openMBVZPosition);
          }
          if(openMBVVelocity) {
            openMBVRadialVelocity = new OpenMBV::Arrow(*openMBVVelocity);
            openMBVCircularVelocity = new OpenMBV::Arrow(*openMBVVelocity);
            openMBVZVelocity = new OpenMBV::Arrow(*openMBVVelocity);
            openMBVRadialVelocity->setName("RadialVelocity");
            openMBVVelGrp->addObject(openMBVRadialVelocity);
            openMBVCircularVelocity->setName("CircularVelocity");
            openMBVVelGrp->addObject(openMBVCircularVelocity);
            openMBVZVelocity->setName("ZVelocity");
            openMBVVelGrp->addObject(openMBVZVelocity);
          }
          if(openMBVAcceleration) {
            openMBVRadialAcceleration = new OpenMBV::Arrow(*openMBVAcceleration);
            openMBVCircularAcceleration = new OpenMBV::Arrow(*openMBVAcceleration);
            openMBVZAcceleration = new OpenMBV::Arrow(*openMBVAcceleration);
            openMBVRadialAcceleration->setName("RadialAcceleration");
            openMBVAccGrp->addObject(openMBVRadialAcceleration);
            openMBVCircularAcceleration->setName("CircularAcceleration");
            openMBVAccGrp->addObject(openMBVCircularAcceleration);
            openMBVZAcceleration->setName("ZAcceleration");
            openMBVAccGrp->addObject(openMBVZAcceleration);
          }
        }
#endif
      }
    }
    else
      CoordinatesObserver::init(stage);
  }

  void CylinderCoordinatesObserver::plot(double t, double dt) {
    if(getPlotFeature(plotRecursive)==enabled) {
#ifdef HAVE_OPENMBVCPPINTERFACE
      if(getPlotFeature(openMBV)==enabled) {
        Vec3 r = frame->getPosition();
        Vec3 v = frame->getVelocity();
        Vec3 a = frame->getAcceleration();
        Vec3 ep = crossProduct(ez,r);
        double nrmep = nrm2(ep);
        if(nrmep<epsroot()) {
          if(fabs(ez(0))<epsroot() && fabs(ez(1))<epsroot()) {
            ep(0) = 1.;
            ep(1) = 0.;
            ep(2) = 0.;
          }
          else {
            ep(0) = -ez(1);
            ep(1) = ez(0);
            ep(2) = 0.0;
          }
        }
        else
          ep = ep/nrmep;
        Vec3 er = crossProduct(ep,ez);

        if(openMBVPosition && !openMBVPosition->isHDF5Link()) {
          vector<double> data;
          Vec3 rr =  (r.T()*er)*er;
          data.push_back(t);
          data.push_back(0);
          data.push_back(0);
          data.push_back(0);
          data.push_back(rr(0));
          data.push_back(rr(1));
          data.push_back(rr(2));
          data.push_back(0.5);
          openMBVRadialPosition->append(data);
          data.clear();
          Vec3 rz =  (r.T()*ez)*ez;
          data.push_back(t);
          data.push_back(0);
          data.push_back(0);
          data.push_back(0);
          data.push_back(rz(0));
          data.push_back(rz(1));
          data.push_back(rz(2));
          data.push_back(0.5);
          openMBVZPosition->append(data);
        }

        if(openMBVVelocity && !openMBVVelocity->isHDF5Link()) {
          vector<double> data;
          Vec3 vr =  (v.T()*er)*er;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(vr(0));
          data.push_back(vr(1));
          data.push_back(vr(2));
          data.push_back(0.5);
          openMBVRadialVelocity->append(data);
          data.clear();
          Vec3 vp =  (v.T()*ep)*ep;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(vp(0));
          data.push_back(vp(1));
          data.push_back(vp(2));
          data.push_back(0.5);
          openMBVCircularVelocity->append(data);
          data.clear();
          Vec3 vz =  (v.T()*ez)*ez;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(vz(0));
          data.push_back(vz(1));
          data.push_back(vz(2));
          data.push_back(0.5);
          openMBVZVelocity->append(data);
        }

        if(openMBVAcceleration && !openMBVAcceleration->isHDF5Link()) {
          vector<double> data;
          Vec3 ar =  (a.T()*er)*er;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(ar(0));
          data.push_back(ar(1));
          data.push_back(ar(2));
          data.push_back(0.5);
          openMBVRadialAcceleration->append(data);
          data.clear();
          Vec3 ap =  (a.T()*ep)*ep;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(ap(0));
          data.push_back(ap(1));
          data.push_back(ap(2));
          data.push_back(0.5);
          openMBVCircularAcceleration->append(data);
          data.clear();
          Vec3 az =  (a.T()*ez)*ez;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(az(0));
          data.push_back(az(1));
          data.push_back(az(2));
          data.push_back(0.5);
          openMBVZAcceleration->append(data);
        }

        if(openMBVFrame && !openMBVFrame->isHDF5Link()) {
          vector<double> data;
          SqrMat3 AWP;
          AWP.set(0, er);
          AWP.set(1, ep);
          AWP.set(2, ez);
          data.push_back(t);
          data.push_back(0);
          data.push_back(0);
          data.push_back(0);
          Vec3 cardan=AIK2Cardan(AWP);
          data.push_back(cardan(0));
          data.push_back(cardan(1));
          data.push_back(cardan(2));
          data.push_back(0);
          openMBVFrame->append(data);
        }
      }
#endif

      CoordinatesObserver::plot(t,dt);
    }
  }

  MBSIM_OBJECTFACTORY_REGISTERXMLNAME(Element, NaturalCoordinatesObserver, MBSIMNS"NaturalCoordinatesObserver")

  NaturalCoordinatesObserver::NaturalCoordinatesObserver(const std::string &name) : CoordinatesObserver(name) {
#ifdef HAVE_OPENMBVCPPINTERFACE
    openMBVTangentialAcceleration=0;
    openMBVNormalAcceleration=0;
#endif
  }

  void NaturalCoordinatesObserver::init(InitStage stage) {
    if(stage==MBSim::plot) {
      updatePlotFeatures();

      CoordinatesObserver::init(stage);
      if(getPlotFeature(plotRecursive)==enabled) {
#ifdef HAVE_OPENMBVCPPINTERFACE
        if(getPlotFeature(openMBV)==enabled) {
          if(openMBVAcceleration) {
            openMBVTangentialAcceleration = new OpenMBV::Arrow(*openMBVAcceleration);
            openMBVNormalAcceleration = new OpenMBV::Arrow(*openMBVAcceleration);
            openMBVTangentialAcceleration->setName("TangentialAcceleration");
            openMBVAccGrp->addObject(openMBVTangentialAcceleration);
            openMBVNormalAcceleration->setName("NormalAcceleration");
            openMBVAccGrp->addObject(openMBVNormalAcceleration);
          }
        }
#endif
      }
    }
    else
      CoordinatesObserver::init(stage);
  }

  void NaturalCoordinatesObserver::plot(double t, double dt) {
    if(getPlotFeature(plotRecursive)==enabled) {
#ifdef HAVE_OPENMBVCPPINTERFACE
      if(getPlotFeature(openMBV)==enabled) {
        Vec3 r = frame->getPosition();
        Vec3 v = frame->getVelocity();
        Vec3 a = frame->getAcceleration();
        double nrmv = nrm2(v);
        Vec3 et;
        if(nrmv<epsroot())
          et(0) = 1;
        else
          et = v/nrmv;
        Vec3 eb = crossProduct(et,a);
        double nrmeb = nrm2(eb);
        if(nrmeb<epsroot()) {
          if(fabs(et(0))<epsroot() && fabs(et(1))<epsroot()) {
            eb(0) = 1.;
            eb(1) = 0.;
            eb(2) = 0.;
          }
          else {
            eb(0) = -et(1);
            eb(1) = et(0);
            eb(2) = 0.0;
          }
        }
        else
          eb = eb/nrmeb;
        Vec3 en = -crossProduct(et,eb);

        if(openMBVAcceleration && !openMBVAcceleration->isHDF5Link()) {
          vector<double> data;
          Vec3 at =  (a.T()*et)*et;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(at(0));
          data.push_back(at(1));
          data.push_back(at(2));
          data.push_back(0.5);
          openMBVTangentialAcceleration->append(data);
          data.clear();
          Vec3 an =  (a.T()*en)*en;
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          data.push_back(an(0));
          data.push_back(an(1));
          data.push_back(an(2));
          data.push_back(0.5);
          openMBVNormalAcceleration->append(data);
        }

        if(openMBVFrame && !openMBVFrame->isHDF5Link()) {
          vector<double> data;
          SqrMat3 AWP;
          AWP.set(0, et);
          AWP.set(1, en);
          AWP.set(2, eb);
          data.push_back(t);
          data.push_back(r(0));
          data.push_back(r(1));
          data.push_back(r(2));
          Vec3 cardan=AIK2Cardan(AWP);
          data.push_back(cardan(0));
          data.push_back(cardan(1));
          data.push_back(cardan(2));
          data.push_back(0);
          openMBVFrame->append(data);
        }
      }
#endif

      CoordinatesObserver::plot(t,dt);
    }
  }

}
