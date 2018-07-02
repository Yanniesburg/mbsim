/* Copyright (C) 2004-2016 MBSim Development Team
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

#ifndef _RELATIVE_KINEMATICS_OBSERVER_H__
#define _RELATIVE_KINEMATICS_OBSERVER_H__

#include "mbsim/observers/observer.h"

#include <mbsim/utils/boost_parameters.h>
#include <mbsim/utils/openmbv_utils.h>

namespace MBSim {

  class Frame;

  class RelativeKinematicsObserver : public Observer {
    private:
      Frame *frame, *refFrame;
      std::string saved_frame, saved_frameOfReference;
      std::shared_ptr<OpenMBV::Group> openMBVPosGrp, openMBVVelGrp, openMBVAngVelGrp, openMBVAccGrp, openMBVAngAccGrp;
      std::shared_ptr<OpenMBV::Arrow> openMBVPosition, openMBVVelocity, openMBVAngularVelocity, openMBVAcceleration, openMBVAngularAcceleration, openMBVrTrans, openMBVrRel, openMBVvTrans, openMBVvRot, openMBVvRel, openMBVvF, openMBVaTrans, openMBVaRot, openMBVaZp, openMBVaCor, openMBVaRel, openMBVaF, openMBVomTrans, openMBVomRel, openMBVpsiTrans, openMBVpsiRot, openMBVpsiRel;

    public:
      RelativeKinematicsObserver(const std::string &name="");

      void setFrame(Frame *frame_) { frame = frame_; }
      void setFrameOfReference(Frame *frame_) { refFrame = frame_; }

      void init(InitStage stage, const InitConfigSet &config) override;
      void plot() override;
      void initializeUsingXML(xercesc::DOMElement *element) override;

      BOOST_PARAMETER_MEMBER_FUNCTION( (void), enableOpenMBVPosition, tag, (optional (scaleLength,(double),1)(scaleSize,(double),1)(referencePoint,(OpenMBV::Arrow::ReferencePoint),OpenMBV::Arrow::fromPoint)(minimalColorValue,(double),0)(maximalColorValue,(double),1)(diffuseColor,(const fmatvec::Vec3&),"[-1;1;1]")(transparency,(double),0))) {
        OpenMBVArrow ombv(scaleLength,scaleSize,OpenMBV::Arrow::toHead,referencePoint,0,minimalColorValue,maximalColorValue,diffuseColor,transparency);
        openMBVPosition=ombv.createOpenMBV();
      }
      BOOST_PARAMETER_MEMBER_FUNCTION( (void), enableOpenMBVVelocity, tag, (optional (scaleLength,(double),1)(scaleSize,(double),1)(referencePoint,(OpenMBV::Arrow::ReferencePoint),OpenMBV::Arrow::fromPoint)(minimalColorValue,(double),0)(maximalColorValue,(double),1)(diffuseColor,(const fmatvec::Vec3&),"[-1;1;1]")(transparency,(double),0))) {
        OpenMBVArrow ombv(scaleLength,scaleSize,OpenMBV::Arrow::toHead,referencePoint,0,minimalColorValue,maximalColorValue,diffuseColor,transparency);
        openMBVVelocity=ombv.createOpenMBV();
      }
      BOOST_PARAMETER_MEMBER_FUNCTION( (void), enableOpenMBVAngularVelocity, tag, (optional (scaleLength,(double),1)(scaleSize,(double),1)(referencePoint,(OpenMBV::Arrow::ReferencePoint),OpenMBV::Arrow::fromPoint)(minimalColorValue,(double),0)(maximalColorValue,(double),1)(diffuseColor,(const fmatvec::Vec3&),"[-1;1;1]")(transparency,(double),0))) {
        OpenMBVArrow ombv(scaleLength,scaleSize,OpenMBV::Arrow::toDoubleHead,referencePoint,0,minimalColorValue,maximalColorValue,diffuseColor,transparency);
        openMBVAngularVelocity=ombv.createOpenMBV();
      }
      BOOST_PARAMETER_MEMBER_FUNCTION( (void), enableOpenMBVAcceleration, tag, (optional (scaleLength,(double),1)(scaleSize,(double),1)(referencePoint,(OpenMBV::Arrow::ReferencePoint),OpenMBV::Arrow::fromPoint)(minimalColorValue,(double),0)(maximalColorValue,(double),1)(diffuseColor,(const fmatvec::Vec3&),"[-1;1;1]")(transparency,(double),0))) {
        OpenMBVArrow ombv(scaleLength,scaleSize,OpenMBV::Arrow::toHead,referencePoint,0,minimalColorValue,maximalColorValue,diffuseColor,transparency);
        openMBVAcceleration=ombv.createOpenMBV();
      }
      BOOST_PARAMETER_MEMBER_FUNCTION( (void), enableOpenMBVAngularAcceleration, tag, (optional (scaleLength,(double),1)(scaleSize,(double),1)(referencePoint,(OpenMBV::Arrow::ReferencePoint),OpenMBV::Arrow::fromPoint)(minimalColorValue,(double),0)(maximalColorValue,(double),1)(diffuseColor,(const fmatvec::Vec3&),"[-1;1;1]")(transparency,(double),0))) {
        OpenMBVArrow ombv(scaleLength,scaleSize,OpenMBV::Arrow::toDoubleHead,referencePoint,0,minimalColorValue,maximalColorValue,diffuseColor,transparency);
        openMBVAngularAcceleration=ombv.createOpenMBV();
      }
  };

}  

#endif
