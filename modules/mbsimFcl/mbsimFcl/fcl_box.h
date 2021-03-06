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

#ifndef _MBSIMFCL_FCL_BOX_H_
#define _MBSIMFCL_FCL_BOX_H_

#include "mbsimFcl/fcl_contour.h"
#include <mbsim/utils/boost_parameters.h>
#include <mbsim/utils/openmbv_utils.h>

namespace MBSimFcl {

  /**
   * \brief FclBox
   */
  class FclBox : public FclContour {
    public:
      /**
       * \brief constructor
       * \param name of box
       * \param length of box
       * \param R frame of reference
       */
      FclBox(const std::string &name="", const fmatvec::Vec3 &length=fmatvec::Vec3(fmatvec::INIT,1), MBSim::Frame *R=nullptr) : FclContour(name,R), lx(length(0)), ly(length(1)), lz(length(2)) { }

      /* INHERITED INTERFACE OF ELEMENT */
      /***************************************************/
      void init(InitStage stage, const MBSim::InitConfigSet &config) override;
      void initializeUsingXML(xercesc::DOMElement *element) override;
      /***************************************************/

      /* GETTER / SETTER */
      void setLength(const fmatvec::Vec3 &length) { lx = length(0); ly = length(1); lz = length(2); }
      void setLength(double lx_, double ly_, double lz_) { lx = lx_; ly = ly_; lz = lz_; }
      /***************************************************/

      BOOST_PARAMETER_MEMBER_FUNCTION( (void), enableOpenMBV, MBSim::tag, (optional (diffuseColor,(const fmatvec::Vec3&),"[-1;1;1]")(transparency,(double),0)(pointSize,(double),0)(lineWidth,(double),0))) {
        MBSim::OpenMBVCuboid ombv(fmatvec::Vec3(),diffuseColor,transparency,pointSize,lineWidth);
        openMBVRigidBody=ombv.createOpenMBV(); 
      }

    private:
      /**
       * \brief x-, y- and z-length of cuboid
       */
      double lx{1};
      double ly{1};
      double lz{1};
  };
}

#endif
