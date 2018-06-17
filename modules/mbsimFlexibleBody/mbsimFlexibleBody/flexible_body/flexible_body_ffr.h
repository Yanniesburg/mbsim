/* Copyright (C) 2004-2015 MBSim Development Team
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

#ifndef _FLEXIBLE_BODY_FFR_H_
#define _FLEXIBLE_BODY_FFR_H_

#include "mbsimFlexibleBody/node_based_body.h"
#include "mbsim/functions/time_dependent_function.h"
#include "mbsim/functions/state_dependent_function.h"
#include "mbsim/utils/boost_parameters.h"
#include "mbsim/utils/index.h"

namespace OpenMBV {
  class FlexibleBody;
}

namespace MBSim {
  class Frame;
}

namespace MBSimFlexibleBody {

  template<typename Dep>
    struct BaseType;

  template<int N>
    struct BaseType<fmatvec::Vector<fmatvec::Fixed<N>, double>> {
      typedef fmatvec::VecV type;
    };

  template<>
    struct BaseType<fmatvec::SqrMat3> {
      typedef fmatvec::MatVx3 type;
    };

  template<>
    struct BaseType<fmatvec::SqrMatV> {
      typedef fmatvec::MatV type;
    };

  template<int N>
    struct BaseType<fmatvec::Matrix<fmatvec::General, fmatvec::Fixed<N>, fmatvec::Var, double>> {
      typedef fmatvec::MatV type;
    };

  /*!
   *  \brief Flexible body using a floating frame of reference formulation
   *
   * */
  class FlexibleBodyFFR : public NodeBasedBody {
    public:

      enum OMBVColorRepresentation {
        none=0,
        xDisplacement,
        yDisplacement,
        zDisplacement,
        totalDisplacement,
        xxStress,
        yyStress,
        zzStress,
        xyStress,
        yzStress,
        zxStress,
        equivalentStress,
        unknown
      };

      FlexibleBodyFFR(const std::string &name=""); 
      /**
       * \brief destructor
       */
      ~FlexibleBodyFFR() override;

      void updateqd() override;
      void updateT() override;
      void updateh(int j=0) override;
      void updateM() override { (this->*updateM_)(); }
      void updateGeneralizedPositions() override;
      void updateGeneralizedVelocities() override;
      void updateDerivativeOfGeneralizedPositions() override;
      void updateGeneralizedAccelerations() override;
      void updatePositions();
      void updateVelocities();
      void updateAccelerations();
      void updateJacobians() override;
      void updateGyroscopicAccelerations();
      void updatePositions(MBSim::Frame *frame) override;
      void updateVelocities(MBSim::Frame *frame) override;
      void updateAccelerations(MBSim::Frame *frame) override;
      void updateJacobians(MBSim::Frame *frame, int j=0) override { (this->*updateJacobians_[j])(frame); }
      void updateGyroscopicAccelerations(MBSim::Frame *frame) override;
      void updateJacobians0(MBSim::Frame *frame);
      void updateJacobians1(MBSim::Frame *frame) { }
      void updateMb();
      void updatehb();
      void updateKJ(int j=0) { (this->*updateKJ_[j])(); }
      void updateKJ0();
      void updateKJ1();
      void (FlexibleBodyFFR::*updateKJ_[2])();
      void calcSize() override;
      void calcqSize() override;
      void calcuSize(int j=0) override;

      /* INHERITED INTERFACE OF OBJECT */
      void init(InitStage stage, const MBSim::InitConfigSet &config) override;
      void updateLLM() override { (this->*updateLLM_)(); }
      void setUpInverseKinetics() override;
      /*****************************************************/

      /* INHERITED INTERFACE OF ELEMENT */
      void plot() override;
      /*****************************************************/

      /* GETTER / SETTER */
      int getNumberOfModeShapes() { if(updSize) calcSize(); return ne; }

      // NOTE: we can not use a overloaded setTranslation here due to restrictions in XML but define them for convinience in c++
      /*!
       * \brief set Kinematic for genral translational motion
       * \param fPrPK translational kinematic description
       */
      void setGeneralTranslation(MBSim::Function<fmatvec::Vec3(fmatvec::VecV, double)> *fPrPK_) {
        fPrPK = fPrPK_;
        fPrPK->setParent(this);
        fPrPK->setName("GeneralTranslation");
      }
      /*!
       * \brief set Kinematic for only time dependent translational motion
       * \param fPrPK translational kinematic description
       */
      void setTimeDependentTranslation(MBSim::Function<fmatvec::Vec3(double)> *fPrPK_) {
        setGeneralTranslation(new MBSim::TimeDependentFunction<fmatvec::Vec3>(fPrPK_));
      }
      /*!
       * \brief set Kinematic for only state dependent translational motion
       * \param fPrPK translational kinematic description
       */
      void setStateDependentTranslation(MBSim::Function<fmatvec::Vec3(fmatvec::VecV)> *fPrPK_) {
        setGeneralTranslation(new MBSim::StateDependentFunction<fmatvec::Vec3>(fPrPK_));
      }
      void setTranslation(MBSim::Function<fmatvec::Vec3(fmatvec::VecV, double)> *fPrPK_) { setGeneralTranslation(fPrPK_); }
      void setTranslation(MBSim::Function<fmatvec::Vec3(double)> *fPrPK_) { setTimeDependentTranslation(fPrPK_); }
      void setTranslation(MBSim::Function<fmatvec::Vec3(fmatvec::VecV)> *fPrPK_) { setStateDependentTranslation(fPrPK_); }

      // NOTE: we can not use a overloaded setRotation here due to restrictions in XML but define them for convinience in c++
      /*!
       * \brief set Kinematic for general rotational motion
       * \param fAPK rotational kinematic description
       */
      void setGeneralRotation(MBSim::Function<fmatvec::RotMat3(fmatvec::VecV, double)>* fAPK_) {
        fAPK = fAPK_;
        fAPK->setParent(this);
        fAPK->setName("GeneralRotation");
      }
      /*!
       * \brief set Kinematic for only time dependent rotational motion
       * \param fAPK rotational kinematic description
       */
      void setTimeDependentRotation(MBSim::Function<fmatvec::RotMat3(double)>* fAPK_) { setGeneralRotation(new MBSim::TimeDependentFunction<fmatvec::RotMat3>(fAPK_)); }
      /*!
       * \brief set Kinematic for only state dependent rotational motion
       * \param fAPK rotational kinematic description
       */
      void setStateDependentRotation(MBSim::Function<fmatvec::RotMat3(fmatvec::VecV)>* fAPK_) { setGeneralRotation(new MBSim::StateDependentFunction<fmatvec::RotMat3>(fAPK_)); }
      void setRotation(MBSim::Function<fmatvec::RotMat3(fmatvec::VecV, double)>* fAPK_) { setGeneralRotation(fAPK_); }
      void setRotation(MBSim::Function<fmatvec::RotMat3(double)>* fAPK_) { setTimeDependentRotation(fAPK_); }
      void setRotation(MBSim::Function<fmatvec::RotMat3(fmatvec::VecV)>* fAPK_) { setStateDependentRotation(fAPK_); }

      void setTranslationDependentRotation(bool dep) { translationDependentRotation = dep; }
      void setCoordinateTransformationForRotation(bool ct) { coordinateTransformation = ct; }
      void setBodyFixedRepresentationOfAngularVelocity(bool bf) { bodyFixedRepresentationOfAngularVelocity = bf; }

      /*!
       * \brief get Kinematic for translational motion
       * \return translational kinematic description
       */
      MBSim::Function<fmatvec::Vec3(fmatvec::VecV, double)>* getTranslation() { return fPrPK; }
      /*!
       * \brief get Kinematic for rotational motion
       * \return rotational kinematic description
       */
      MBSim::Function<fmatvec::RotMat3(fmatvec::VecV, double)>* getRotation() { return fAPK; }

      double getMass() const { return m; }
      MBSim::Frame* getFrameK() { return K; };

      // Interface for basic data
      /*! \brief Set mass
       *
       * Set the mass of the flexible body.
       * \param m The mass of the body
       * */
      void setMass(double m_) { m = m_; }
      void setPositionIntegral(const fmatvec::Vec3 &rdm_) { rdm = rdm_; }
      void setPositionPositionIntegral(const fmatvec::SymMat3& rrdm_) { rrdm = rrdm_; }
      void setShapeFunctionIntegral(const fmatvec::Mat3xV &Pdm_) { Pdm = Pdm_; }

      void setPositionShapeFunctionIntegral(const std::vector<fmatvec::Mat3xV> &rPdm) { setPositionShapeFunctionIntegralArray(rPdm); }
      void setPositionShapeFunctionIntegralArray(const std::vector<fmatvec::Mat3xV> &rPdm_) { rPdm = rPdm_; }
      void setPositionShapeFunctionIntegral(const fmatvec::MatV &rPdm_) { rPdm = getCellArray1D<fmatvec::Mat3xV>(3,rPdm_); }

      void setShapeFunctionShapeFunctionIntegral(const std::vector<std::vector<fmatvec::SqrMatV> > &PPdm) { setShapeFunctionShapeFunctionIntegralArray(PPdm); }
      void setShapeFunctionShapeFunctionIntegralArray(const std::vector<std::vector<fmatvec::SqrMatV> > &PPdm_) { PPdm = PPdm_; }
      void setShapeFunctionShapeFunctionIntegral(const fmatvec::MatV &PPdm_) { PPdm = getCellArray2D<fmatvec::SqrMatV>(PPdm_.cols(),3,PPdm_); }

      void setStiffnessMatrix(const fmatvec::SymMatV &Ke0_) { Ke0 = Ke0_; }
      void setDampingMatrix(const fmatvec::SymMatV &De0_) { De0 = De0_; }
      void setProportionalDamping(const fmatvec::Vec2 &beta_) { beta = beta_; }
      // End of interface

      // Interface for nonlinear stiffness matrices
      void setNonlinearStiffnessMatrixOfFirstOrder(const std::vector<fmatvec::SqrMatV> &Knl1) { setNonlinearStiffnessMatrixOfFirstOrderArray(Knl1); }
      void setNonlinearStiffnessMatrixOfFirstOrderArray(const std::vector<fmatvec::SqrMatV> &Knl1_) { Knl1 = Knl1_; }
      void setNonlinearStiffnessMatrixOfFirstOrder(const fmatvec::MatV &Knl1_) { Knl1 = getCellArray1D<fmatvec::SqrMatV>(Knl1_.cols(),Knl1_); }

      void setNonlinearStiffnessMatrixOfSecondOrder(const std::vector<std::vector<fmatvec::SqrMatV> > &Knl2) { setNonlinearStiffnessMatrixOfSecondOrderArray(Knl2); }
      void setNonlinearStiffnessMatrixOfSecondOrderArray(const std::vector<std::vector<fmatvec::SqrMatV> > &Knl2_) { Knl2 = Knl2_; }
      void setNonlinearStiffnessMatrixOfSecondOrder(const fmatvec::MatV &Knl2_) { Knl2 = getCellArray2D<fmatvec::SqrMatV>(Knl2_.cols(),Knl2_.cols(),Knl2_); }
      // End of interface

      // Interface for reference stresses 
      void setInitialStressIntegral(const fmatvec::VecV &ksigma0_) { ksigma0 = ksigma0_; }
      void setNonlinearInitialStressIntegral(const fmatvec::SqrMatV &ksigma1_) { ksigma1 = ksigma1_; }
      // End of interface

      // Interface for geometric stiffness matrices
      void setGeometricStiffnessMatrixDueToAcceleration(const std::vector<fmatvec::SqrMatV> &K0t) { setGeometricStiffnessMatrixDueToAccelerationArray(K0t); }
      void setGeometricStiffnessMatrixDueToAccelerationArray(const std::vector<fmatvec::SqrMatV> &K0t_) { K0t = K0t_; }
      void setGeometricStiffnessMatrixDueToAcceleration(const fmatvec::MatV &K0t_) { K0t = getCellArray1D<fmatvec::SqrMatV>(K0t_.cols(),K0t_); }

      void setGeometricStiffnessMatrixDueToAngularAcceleration(const std::vector<fmatvec::SqrMatV> &K0r) { setGeometricStiffnessMatrixDueToAngularAccelerationArray(K0r); }
      void setGeometricStiffnessMatrixDueToAngularAccelerationArray(const std::vector<fmatvec::SqrMatV> &K0r_) { K0r = K0r_; }
      void setGeometricStiffnessMatrixDueToAngularAcceleration(const fmatvec::MatV &K0r_) { K0r = getCellArray1D<fmatvec::SqrMatV>(K0r_.cols(),K0r_); }

      void setGeometricStiffnessMatrixDueToAngularVelocity(const std::vector<fmatvec::SqrMatV> &K0om) { setGeometricStiffnessMatrixDueToAngularVelocityArray(K0om); }
      void setGeometricStiffnessMatrixDueToAngularVelocityArray(const std::vector<fmatvec::SqrMatV> &K0om_) { K0om = K0om_; }
      void setGeometricStiffnessMatrixDueToAngularVelocity(const fmatvec::MatV &K0om_) { K0om = getCellArray1D<fmatvec::SqrMatV>(K0om_.cols(),K0om_); }
      // End of interface

      void setNodalRelativePosition(const std::vector<fmatvec::Vec3> &r) { setNodalRelativePositionArray(r); }
      void setNodalRelativePositionArray(const std::vector<fmatvec::Vec3> &r) { KrKP = r; }
      void setNodalRelativePosition(const fmatvec::VecV &r) { KrKP = getCellArray1D<fmatvec::Vec3>(3,r); }

      void setNodalRelativeOrientation(const std::vector<fmatvec::SqrMat3> &A) { setNodalRelativeOrientationArray(A); }
      void setNodalRelativeOrientationArray(const std::vector<fmatvec::SqrMat3> &A) { ARP = A; }
      void setNodalRelativeOrientation(const fmatvec::MatVx3 &A) { ARP = getCellArray1D<fmatvec::SqrMat3>(3,A); }

      void setNodalShapeMatrixOfTranslation(const std::vector<fmatvec::Mat3xV> &Phi) { setNodalShapeMatrixOfTranslationArray(Phi); }
      void setNodalShapeMatrixOfTranslationArray(const std::vector<fmatvec::Mat3xV> &Phi_) { Phi = Phi_; }
      void setNodalShapeMatrixOfTranslation(const fmatvec::MatV &Phi_) { Phi = getCellArray1D<fmatvec::Mat3xV>(3,Phi_); }

      void setNodalShapeMatrixOfRotation(const std::vector<fmatvec::Mat3xV> &Psi) { setNodalShapeMatrixOfRotationArray(Psi); }
      void setNodalShapeMatrixOfRotationArray(const std::vector<fmatvec::Mat3xV> &Psi_) { Psi = Psi_; }
      void setNodalShapeMatrixOfRotation(const fmatvec::MatV &Psi_) { Psi = getCellArray1D<fmatvec::Mat3xV>(3,Psi_); }

      void setNodalStressMatrix(const std::vector<fmatvec::Matrix<fmatvec::General, fmatvec::Fixed<6>, fmatvec::Var, double> > &sigmahel) { setNodalStressMatrixArray(sigmahel); }
      void setNodalStressMatrixArray(const std::vector<fmatvec::Matrix<fmatvec::General, fmatvec::Fixed<6>, fmatvec::Var, double> > &sigmahel_) { sigmahel = sigmahel_; }
      void setNodalStressMatrix(const fmatvec::MatV &sigmahel_) { sigmahel = getCellArray1D<fmatvec::Matrix<fmatvec::General, fmatvec::Fixed<6>, fmatvec::Var, double> >(6,sigmahel_); }

      void setNodalNonlinearStressMatrix(const std::vector<std::vector<fmatvec::Matrix<fmatvec::General, fmatvec::Fixed<6>, fmatvec::Var, double> > > &sigmahen) { setNodalNonlinearStressMatrixArray(sigmahen); }
      void setNodalNonlinearStressMatrixArray(const std::vector<std::vector<fmatvec::Matrix<fmatvec::General, fmatvec::Fixed<6>, fmatvec::Var, double> > > &sigmahen_) { sigmahen = sigmahen_; }
      void setNodalNonlinearStressMatrix(const fmatvec::MatV &sigmahen_) { sigmahen = getCellArray2D<fmatvec::Matrix<fmatvec::General, fmatvec::Fixed<6>, fmatvec::Var, double> >(6,sigmahen_.cols(),sigmahen_); }

      void setNodalInitialStress(const std::vector<fmatvec::Vector<fmatvec::Fixed<6>, double> > &sigma0) { setNodalInitialStressArray(sigma0); }
      void setNodalInitialStressArray(const std::vector<fmatvec::Vector<fmatvec::Fixed<6>, double> > &sigma0_) { sigma0 = sigma0_; }
      void setNodalInitialStress(const fmatvec::VecV &sigma0_) { sigma0 = getCellArray1D<fmatvec::Vector<fmatvec::Fixed<6>, double> >(6,sigma0_); }

      void setNodalGeometricStiffnessMatrixDueToForce(const std::vector<std::vector<fmatvec::SqrMatV> > &K0F) { setNodalGeometricStiffnessMatrixDueToForceArray(K0F); }
      void setNodalGeometricStiffnessMatrixDueToForceArray(const std::vector<std::vector<fmatvec::SqrMatV> > &K0F_) { K0F = K0F_; }
      void setNodalGeometricStiffnessMatrixDueToForce(const fmatvec::MatV &K0F_) { K0F = getCellArray2D<fmatvec::SqrMatV>(K0F_.cols(),K0F_.cols(),K0F_); }

      void setNodalGeometricStiffnessMatrixDueToMoment(const std::vector<std::vector<fmatvec::SqrMatV> > &K0M) { setNodalGeometricStiffnessMatrixDueToMomentArray(K0M); }
      void setNodalGeometricStiffnessMatrixDueToMomentArray(const std::vector<std::vector<fmatvec::SqrMatV> > &K0M_) { K0M = K0M_; }
      void setNodalGeometricStiffnessMatrixDueToMoment(const fmatvec::MatV &K0M_) { K0M = getCellArray2D<fmatvec::SqrMatV>(K0M_.cols(),K0M_.cols(),K0M_); }

      const fmatvec::Vec3& getNodalRelativePosition(int i) const { return KrKP[i]; }
      const fmatvec::Mat3xV& getNodalShapeMatrixOfTranslation(int i) const { return Phi[i]; }

      using NodeBasedBody::addFrame;
      using NodeBasedBody::addContour;

      void setOpenMBVFlexibleBody(const std::shared_ptr<OpenMBV::FlexibleBody> &body);
      void setOpenMBVNodes(const std::vector<MBSim::Index> &ombvNodes_) { ombvNodes = ombvNodes_; }
      void setOpenMBVColorRepresentation(OMBVColorRepresentation ombvColorRepresentation_) { ombvColorRepresentation = ombvColorRepresentation_; }

      void initializeUsingXML(xercesc::DOMElement *element) override;

      void setqRel(const fmatvec::VecV &q);
      void setuRel(const fmatvec::VecV &u);

      bool transformCoordinates() const {return fTR!=nullptr;}

      void resetUpToDate() override;
      const fmatvec::VecV& evalqTRel() { if(updq) updateGeneralizedPositions(); return qTRel; }
      const fmatvec::VecV& evalqRRel() { if(updq) updateGeneralizedPositions(); return qRRel; }
      const fmatvec::VecV& evalqERel() { if(updq) updateGeneralizedPositions(); return qERel; }
      const fmatvec::VecV& evaluTRel() { if(updu) updateGeneralizedVelocities(); return uTRel; }
      const fmatvec::VecV& evaluRRel() { if(updu) updateGeneralizedVelocities(); return uRRel; }
      const fmatvec::VecV& evaluERel() { if(updu) updateGeneralizedVelocities(); return uERel; }
      const fmatvec::VecV& evalqdTRel() { if(updqd) updateDerivativeOfGeneralizedPositions(); return qdTRel; }
      const fmatvec::VecV& evalqdRRel() { if(updqd) updateDerivativeOfGeneralizedPositions(); return qdRRel; }
      const fmatvec::VecV& evalqdERel() { if(updqd) updateDerivativeOfGeneralizedPositions(); return qdERel; }
      const fmatvec::VecV& evaludERel() { if(updud) updateGeneralizedAccelerations(); return udERel; }
      const fmatvec::Vec3& evalGlobalRelativePosition() { if(updPos) updatePositions(); return WrPK; }
      const fmatvec::Vec3& evalGlobalRelativeVelocity() { if(updVel) updateVelocities(); return WvPKrel; }
      const fmatvec::Vec3& evalGlobalRelativeAngularVelocity() { if(updVel) updateVelocities(); return WomPK; }
      const fmatvec::Mat3xV& evalPJTT() { if(updPJ) updateJacobians(); return PJTT; }
      const fmatvec::Mat3xV& evalPJRR() { if(updPJ) updateJacobians(); return PJRR; }
      const fmatvec::Vec3& evalPjhT() { if(updPJ) updateJacobians(); return PjhT; }
      const fmatvec::Vec3& evalPjhR() { if(updPJ) updateJacobians(); return PjhR; }
      const fmatvec::Vec3& evalPjbT() { if(updPjb) updateGyroscopicAccelerations(); return PjbT; }
      const fmatvec::Vec3& evalPjbR() { if(updPjb) updateGyroscopicAccelerations(); return PjbR; }
      const fmatvec::SymMatV& evalMb() { if(updMb) updateMb(); return M_; }
      const fmatvec::VecV& evalhb() { if(updMb) updateMb(); return h_; }
      const fmatvec::MatV& evalKJ(int j=0) { if(updKJ[j]) updateKJ(j); return KJ[j]; }
      const fmatvec::VecV& evalKi() { if(updKJ[0]) updateKJ(0); return Ki; }
      const fmatvec::Vec3& evalGlobalRelativePosition(int i) { if(updNodalPos[i]) updatePositions(i); return WrRP[i]; }
      const fmatvec::Vec3& evalGlobalRelativeAngularVelocity(int i) { if(updNodalVel[i]) updateVelocities(i); return Womrel[i]; }

      fmatvec::Vec3& getGlobalRelativeVelocity(bool check=true) { assert((not check) or (not updVel)); return WvPKrel; }
      fmatvec::SqrMat3& getRelativeOrientation(bool check=true) { assert((not check) or (not updPos)); return APK; }
      fmatvec::Vec3& getPjbT(bool check=true) { assert((not check) or (not updPjb)); return PjbT; }
      fmatvec::Vec3& getPjbR(bool check=true) { assert((not check) or (not updPjb)); return PjbR; }

      fmatvec::VecV& getqTRel(bool check=true) { assert((not check) or (not updq)); return qTRel; }
      fmatvec::VecV& getqRRel(bool check=true) { assert((not check) or (not updq)); return qRRel; }
      fmatvec::VecV& getqERel(bool check=true) { assert((not check) or (not updq)); return qERel; }
      fmatvec::VecV& getuTRel(bool check=true) { assert((not check) or (not updu)); return uTRel; }
      fmatvec::VecV& getuRRel(bool check=true) { assert((not check) or (not updu)); return uRRel; }
      fmatvec::VecV& getuERel(bool check=true) { assert((not check) or (not updu)); return uERel; }
      fmatvec::VecV& getqdTRel(bool check=true) { assert((not check) or (not updqd)); return qdTRel; }
      fmatvec::VecV& getqdRRel(bool check=true) { assert((not check) or (not updqd)); return qdRRel; }
      fmatvec::VecV& getqdERel(bool check=true) { assert((not check) or (not updqd)); return qdERel; }
      fmatvec::VecV& getudERel(bool check=true) { assert((not check) or (not updud)); return udERel; }

      fmatvec::Vec3& getNodalRelativeVelocity(int i, bool check=true) { assert((not check) or (not updNodalVel[i])); return Wvrel[i]; }

      void updateStresses(int i) override;
      void updatePositions(int i) override;
      void updateVelocities(int i) override;
      void updateAccelerations(int i) override;
      void updateJacobians(int i, int j=0) override;
      void updateGyroscopicAccelerations(int i) override;

      void updatePositions(NodeFrame* frame) override;
      void updateVelocities(NodeFrame* frame) override;
      void updateAccelerations(NodeFrame* frame) override;
      void updateJacobians(NodeFrame* frame, int j=0) override;
      void updateGyroscopicAccelerations(NodeFrame* frame) override;

      template <class T>
      static std::vector<T> getCellArray1D(xercesc::DOMElement *element) {
        std::vector<T> array;
        xercesc::DOMElement* e=element->getFirstElementChild();
        if(MBXMLUtils::E(e)->getTagName()==MBSIMFLEX%"ele") {
          while(e) {
            array.push_back(MBXMLUtils::E(e)->getText<T>());
            e=e->getNextElementSibling();
          }
        }
        return array;
      }

      template <class T>
      static std::vector<T> getCellArray1D(int m, const typename BaseType<T>::type &A) {
        std::vector<T> array;
        int M = A.rows()/m;
        int n = A.cols();
        for(int i=0; i<M; i++)
          array.push_back(T(A(fmatvec::RangeV(i*m,i*m+m-1),fmatvec::RangeV(0,n-1))));
        return array;
      }

      template <class T>
      static std::vector<std::vector<T> > getCellArray2D(xercesc::DOMElement *element) {
        std::vector<std::vector<T> > array;
        xercesc::DOMElement *e=element->getFirstElementChild();
        if(MBXMLUtils::E(e)->getTagName()==MBSIMFLEX%"row") {
          while(e) {
            array.push_back(std::vector<T>());
            xercesc::DOMElement *ee=e->getFirstElementChild();
            while(ee) {
              array[array.size()-1].push_back(MBXMLUtils::E(ee)->getText<T>());
              ee=ee->getNextElementSibling();
            }
            e=e->getNextElementSibling();
          }
        }
        return array;
      }

      template <class T>
      static std::vector<std::vector<T> > getCellArray2D(int m, int N, const typename BaseType<T>::type &A) {
        int M = A.rows()/(m*N);
        int n = A.cols();
        std::vector<std::vector<T> > array(M);
        for(int i=0, k=0; i<M; i++) {
          for(int j=0; j<N; j++) {
            array[i].push_back(T(A(fmatvec::RangeV(k*m,k*m+m-1),fmatvec::RangeV(0,n-1))));
            k++;
          }
        }
        return array;
      }

    protected:
      double m{0};
      fmatvec::Vec3 rdm;
      fmatvec::SymMat3 rrdm, mmi0;
      fmatvec::Mat3xV Pdm;
      std::vector<std::vector<fmatvec::SqrMatV> > PPdm, Knl2;
      std::vector<std::vector<fmatvec::SqrMatV> > Ke2;
      std::vector<fmatvec::Mat3xV> rPdm;
      std::vector<std::vector<fmatvec::SqrMat3> > mmi2, Gr1;
      std::vector<fmatvec::SqrMatV> Knl1, K0t, K0r, K0om, Ct1, Cr1, Ge, Oe1, Ke1, De1;
      fmatvec::Vec2 beta;
      fmatvec::VecV ksigma0;
      fmatvec::SqrMatV ksigma1;
      std::vector<fmatvec::SymMat3> mmi1;
      fmatvec::MatVx3 Ct0, Cr0;
      fmatvec::SymMatV Me, Ke0, De0;
      std::vector<fmatvec::SqrMat3> Gr0;
      fmatvec::Matrix<fmatvec::General,fmatvec::Var,fmatvec::Fixed<6>,double> Oe0;

      fmatvec::SqrMat3 Id;
      std::vector<fmatvec::Vec3> KrKP, WrRP, Wvrel, Womrel;
      std::vector<fmatvec::SqrMat3> ARP;
      std::vector<fmatvec::Mat3xV> Phi, Psi;
      std::vector<std::vector<fmatvec::SqrMatV> > K0F, K0M;
      std::vector<fmatvec::Vector<fmatvec::Fixed<6>, double> > sigma0;
      std::vector<fmatvec::Matrix<fmatvec::General, fmatvec::Fixed<6>, fmatvec::Var, double> > sigmahel;
      std::vector<std::vector<fmatvec::Matrix<fmatvec::General, fmatvec::Fixed<6>, fmatvec::Var, double> > > sigmahen;

      // Number of mode shapes 
      int ne{0};

      MBSim::Frame *K;

      /**
       * \brief TODO
       */
      fmatvec::SymMat Mbuf;

      /**
       * \brief boolean to use body fixed Frame for rotation
       */
      bool coordinateTransformation{true};

      fmatvec::Vec3 PjhT, PjhR, PjbT, PjbR;

      /**
       * \brief rotation matrix from kinematic Frame to parent Frame
       */
      fmatvec::SqrMat3 APK;

      /**
       * \brief translation from parent to kinematic Frame in parent and world system
       */
      fmatvec::Vec3 PrPK, WrPK;

      /**
       * \brief translational and angular velocity from parent to kinematic Frame in world system
       */
      fmatvec::Vec3 WvPKrel, WomPK;

      MBSim::Function<fmatvec::MatV(fmatvec::VecV)> *fTR{0};

      /**
       * \brief translation from parent Frame to kinematic Frame in parent system
       */
      MBSim::Function<fmatvec::Vec3(fmatvec::VecV, double)> *fPrPK{0};

      /**
       * \brief rotation from kinematic Frame to parent Frame
       */
      MBSim::Function<fmatvec::RotMat3(fmatvec::VecV, double)> *fAPK{0};

      /**
       * \brief function pointer to update mass matrix
       */
      void (FlexibleBodyFFR::*updateM_)();

      /**
       * \brief update constant mass matrix
       */
      void updateMConst();

      /**
       * \brief update time dependend mass matrix
       */
      void updateMNotConst();

      /**
       * \brief function pointer for Cholesky decomposition of mass matrix
       */
      void (FlexibleBodyFFR::*updateLLM_)();

      /**
       * \brief Cholesky decomposition of constant mass matrix
       */
      void updateLLMConst() { }

      /**
       * \brief Cholesky decomposition of time dependent mass matrix
       */
      void updateLLMNotConst() { Object::updateLLM(); }

      void (FlexibleBodyFFR::*updateJacobians_[2])(MBSim::Frame *frame);

      fmatvec::Vec aT, aR;

      fmatvec::VecV qTRel, qRRel, qERel, uTRel, uRRel, uERel, qdTRel, qdRRel, qdERel, udERel;
      fmatvec::Mat3xV WJTrel, WJRrel, PJTT, PJRR;

      MBSim::Frame *frameForJacobianOfRotation{0};

      fmatvec::Range<fmatvec::Var,fmatvec::Var> iqT, iqR, iqE, iuT, iuR, iuE;

      bool translationDependentRotation{false}, constJT{false}, constJR{false}, constjT{false}, constjR{false};

      bool updPjb{true}, updGC{true}, updMb{true}, updKJ[2];

      fmatvec::SymMatV M_;
      fmatvec::VecV h_;
      fmatvec::MatV KJ[2];
      fmatvec::VecV Ki;

      void determineSID();
      void prefillMassMatrix();

      bool bodyFixedRepresentationOfAngularVelocity{false};

    private:
      std::vector<MBSim::Index> ombvNodes;
      OMBVColorRepresentation ombvColorRepresentation{none};
      double (FlexibleBodyFFR::*evalOMBVColorRepresentation[12])(int i);
      double evalNone(int i) { return 0; }
      double evalXDisplacement(int i) { return evalNodalDisplacement(i)(0); }
      double evalYDisplacement(int i) { return evalNodalDisplacement(i)(1); }
      double evalZDisplacement(int i) { return evalNodalDisplacement(i)(2); }
      double evalTotalDisplacement(int i) { return fmatvec::nrm2(evalNodalDisplacement(i)); }
      double evalXXStress(int i) { return evalNodalStress(i)(0); }
      double evalYYStress(int i) { return evalNodalStress(i)(1); }
      double evalZZStress(int i) { return evalNodalStress(i)(2); }
      double evalXYStress(int i) { return evalNodalStress(i)(3); }
      double evalYZStress(int i) { return evalNodalStress(i)(4); }
      double evalZXStress(int i) { return evalNodalStress(i)(5); }
      double evalEquivalentStress(int i);
  };

}

#endif
