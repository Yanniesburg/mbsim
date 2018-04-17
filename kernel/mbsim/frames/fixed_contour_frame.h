/* Copyright (C) 2004-2014 MBSim Development Team
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

#ifndef _FIXED_CONTOUR_FRAME_H__
#define _FIXED_CONTOUR_FRAME_H__

#include "mbsim/frames/contour_frame.h"

namespace MBSim {

  class Contour;

  class FixedContourFrame : public ContourFrame {

    public:
      FixedContourFrame(const std::string &name = "dummy", const fmatvec::Vec2 &zeta = fmatvec::Vec2(), Contour *contour_=nullptr) : ContourFrame(name,zeta), contour(contour_) { updZeta = false; }

      void resetUpToDate() override { Frame::resetUpToDate(); }

      void updatePositions() override;
      void updateVelocities() override;
      void updateAccelerations() override;
      void updateJacobians(int j=0) override;
      void updateGyroscopicAccelerations() override;

      void setContourOfReference(Contour *contour_) { contour = contour_; }

    protected:
      Contour *contour;
  };

}

#endif
