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

#ifndef _FCL_CONTOUR_H_
#define _FCL_CONTOUR_H_

#include "mbsim/contours/rigid_contour.h"
#include "fcl/geometry/collision_geometry.h"

namespace MBSim {

  /**
   * \brief FclContour
   */
  class FclContour : public RigidContour {
    public:
      /**
       * \brief constructor
       * \param name of contour
       */
      FclContour(const std::string &name="", Frame *R=nullptr) : RigidContour(name,R) { }

      void init(InitStage stage, const InitConfigSet &config) override;
      void initializeUsingXML(xercesc::DOMElement *element) override;

      std::shared_ptr<fcl::CollisionGeometry<double> > getCollisionGeometry() const { return cg; }

      void setComputeLocalAABB(bool computeLocalAABB_) { computeLocalAABB = computeLocalAABB_; }

    protected:
      std::shared_ptr<fcl::CollisionGeometry<double> > cg;

      /**
       * \brief compute local AABB
       */
      bool computeLocalAABB{true};
  };
}

#endif
