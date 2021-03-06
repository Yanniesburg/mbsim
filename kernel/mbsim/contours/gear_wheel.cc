/* Copyright (C) 2004-2018 MBSim Development Team
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
#include "mbsim/contours/gear_wheel.h"
#include "mbsim/utils/utils.h"

using namespace std;
using namespace fmatvec;
using namespace MBXMLUtils;
using namespace xercesc;

namespace MBSim {

  MBSIM_OBJECTFACTORY_REGISTERCLASS(MBSIM, GearWheel)

  void GearWheel::init(InitStage stage, const InitConfigSet &config) {
    if(stage==plotting) {
      if(plotFeature[openMBV] && openMBVRigidBody) {
        static_pointer_cast<OpenMBV::GearWheel>(openMBVRigidBody)->setNumberOfTeeth(N);
        static_pointer_cast<OpenMBV::GearWheel>(openMBVRigidBody)->setWidth(w);
        static_pointer_cast<OpenMBV::GearWheel>(openMBVRigidBody)->setHelixAngle(be);
        static_pointer_cast<OpenMBV::GearWheel>(openMBVRigidBody)->setPitchAngle(ga);
        static_pointer_cast<OpenMBV::GearWheel>(openMBVRigidBody)->setModule(m);
        static_pointer_cast<OpenMBV::GearWheel>(openMBVRigidBody)->setPressureAngle(al);
        static_pointer_cast<OpenMBV::GearWheel>(openMBVRigidBody)->setBacklash(b);
        static_pointer_cast<OpenMBV::GearWheel>(openMBVRigidBody)->setSolid(solid);
      }
    }
    RigidContour::init(stage, config);
  }

  void GearWheel::initializeUsingXML(DOMElement *element) {
    RigidContour::initializeUsingXML(element);
    DOMElement* e;
    e=E(element)->getFirstElementChildNamed(MBSIM%"numberOfTeeth");
    setNumberOfTeeth(E(e)->getText<int>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"width");
    setWidth(E(e)->getText<double>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"helixAngle");
    if(e) setHelixAngle(E(e)->getText<double>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"pitchAngle");
    if(e) setPitchAngle(E(e)->getText<double>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"module");
    if(e) setModule(E(e)->getText<double>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"pressureAngle");
    if(e) setPressureAngle(E(e)->getText<double>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"backlash");
    if(e) setBacklash(E(e)->getText<double>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"solid");
    if(e) setSolid(E(e)->getText<bool>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"enableOpenMBV");
    if(e) {
      OpenMBVGearWheel ombv;
      ombv.initializeUsingXML(e);
      openMBVRigidBody=ombv.createOpenMBV(); 
    }
  }

}
