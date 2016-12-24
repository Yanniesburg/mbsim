/* Copyright (C) 2004-2014 MBSim Development Team
 * 
 * This library is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU Lesser General Public 
 * License as published by the Free Software Foundation; either 
 * version 2.1 of the License, or (at your option) any later version. 
 * 
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * Contact: martin.o.foerg@googlemail.com
 */

#include <config.h>
#include "mbsim/links/floating_frame_link.h"

using namespace std;
using namespace fmatvec;
using namespace MBXMLUtils;
using namespace xercesc;

namespace MBSim {

  FloatingFrameLink::FloatingFrameLink(const std::string &name) : FrameLink(name), refFrame(NULL), refFrameID(0), C("F") {
    C.setParent(this);
  }

  void FloatingFrameLink::resetUpToDate() {
    FrameLink::resetUpToDate();
    C.resetUpToDate();  
  }

  void FloatingFrameLink::calclaSize(int j) {
    FrameLink::calclaSize(j);
    laSize = forceDir.cols() + momentDir.cols();
  }

  void FloatingFrameLink::calcgSize(int j) {
    FrameLink::calcgSize(j);
    gSize = forceDir.cols() + momentDir.cols();
  }

  void FloatingFrameLink::calcgdSize(int j) {
    FrameLink::calcgdSize(j);
    gdSize = forceDir.cols() + momentDir.cols();
  }

  void FloatingFrameLink::calcrFactorSize(int j) {
    FrameLink::calcrFactorSize(j);
    rFactorSize = isSetValued() ? forceDir.cols() + momentDir.cols() : 0;
  }

  void FloatingFrameLink::calccorrSize(int j) {
    FrameLink::calccorrSize(j);
    corrSize = forceDir.cols() + momentDir.cols();
  }

  void FloatingFrameLink::updatedhdz() {
    THROW_MBSIMERROR("Internal error");
  }

  void FloatingFrameLink::updateW(int j) {
    W[j][0] -= C.evalJacobianOfTranslation(j).T() * evalRF() + C.evalJacobianOfRotation(j).T() * evalRM();
    W[j][1] += frame[1]->evalJacobianOfTranslation(j).T() * evalRF() + frame[1]->evalJacobianOfRotation(j).T() * evalRM();
  }

  void FloatingFrameLink::updateh(int j) {
    h[j][0] -= C.evalJacobianOfTranslation(j).T() * evalForce() + C.evalJacobianOfRotation(j).T() * evalMoment();
    h[j][1] += frame[1]->evalJacobianOfTranslation(j).T() * evalForce() + frame[1]->evalJacobianOfRotation(j).T() * evalMoment();
  }

  void FloatingFrameLink::updatePositions() {
    WrP0P1 = frame[1]->evalPosition() - frame[0]->evalPosition();
    updPos = false;
  }

  void FloatingFrameLink::updatePositions(Frame *frame_) {
    frame_->setPosition(frame[1]->evalPosition());
    frame_->setOrientation(frame[0]->evalOrientation());
  }

  void FloatingFrameLink::updateVelocities() {
    WvP0P1 = frame[1]->evalVelocity() - C.evalVelocity();
    WomP0P1 = frame[1]->evalAngularVelocity() - C.evalAngularVelocity();
    updVel = false;
  }

  void FloatingFrameLink::updateGeneralizedPositions() {
    rrel.set(iF, evalGlobalForceDirection().T() * evalGlobalRelativePosition());
    rrel.set(iM, x);
    updrrel = false;
  }

  void FloatingFrameLink::updateGeneralizedVelocities() {
    vrel.set(iF, evalGlobalForceDirection().T() * evalGlobalRelativeVelocity());
    vrel.set(iM, evalGlobalMomentDirection().T() * evalGlobalRelativeAngularVelocity());
    updvrel = false;
  }

  void FloatingFrameLink::updateGeneralizedForces() {
    lambda.set(iF, evallaF());
    lambda.set(iM, evallaM());
    updla = false;
  }

  void FloatingFrameLink::updateForceDirections() {
    DF = refFrame->evalOrientation() * forceDir;
    DM = refFrame->evalOrientation() * momentDir;
    updFD = false;
  }

  void FloatingFrameLink::updateR() {
    RF.set(RangeV(0,2), RangeV(iF), evalGlobalForceDirection());
    RM.set(RangeV(0,2), RangeV(iM), evalGlobalMomentDirection());
    updRMV = false;
  }

  void FloatingFrameLink::updateg() {
    g(iF) = evalGeneralizedRelativePosition()(iF);
    g(iM) = rrel(iM);;
  }

  void FloatingFrameLink::updategd() {
    gd(iF) = evalGeneralizedRelativeVelocity()(iF);
    gd(iM) = vrel(iM);
  }

  void FloatingFrameLink::init(InitStage stage) {
    if(stage==resize) {
      FrameLink::init(stage);
      int size = forceDir.cols() + momentDir.cols();
      iF = RangeV(0, forceDir.cols() - 1);
      iM = RangeV(forceDir.cols(), forceDir.cols() + momentDir.cols() - 1);
      rrel.resize(size);
      vrel.resize(size);
      if(isSetValued()) {
        g.resize(size);
        gd.resize(size);
        RF.resize(size);
        RM.resize(size);
        la.resize(size);
      }
      lambda.resize(size);
      lambdaF.resize(forceDir.cols());
      lambdaM.resize(momentDir.cols());
      for(unsigned int i=0; i<2; i++) {
        W[i].resize(2);
        V[i].resize(2);
        h[i].resize(2);
        r[i].resize(2);
      }
    }
    else if(stage==unknownStage) {
      FrameLink::init(stage);

      if (forceDir.cols()) DF = forceDir;
      if (momentDir.cols()) DM = momentDir;

      refFrame = refFrameID ? frame[1] : frame[0];
      C.setFrameOfReference(frame[0]);
    }
    else
      FrameLink::init(stage);
  }

  void FloatingFrameLink::initializeUsingXML(DOMElement *element) {
    FrameLink::initializeUsingXML(element);
    DOMElement *e = E(element)->getFirstElementChildNamed(MBSIM%"frameOfReferenceID");
    if (e) refFrameID = getInt(e);
  }

}
