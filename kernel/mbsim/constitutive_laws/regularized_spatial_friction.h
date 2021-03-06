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

#ifndef _REGULARIZED_SPATIAL_FRICTION_H_
#define _REGULARIZED_SPATIAL_FRICTION_H_

#include <mbsim/constitutive_laws/regularized_friction.h>

namespace MBSim {

  class RegularizedSpatialFriction : public RegularizedFriction {
    public:
      RegularizedSpatialFriction(Function<fmatvec::Vec(fmatvec::Vec,double)> *frictionForceFunc_=nullptr) : RegularizedFriction(frictionForceFunc_) { }
      ~RegularizedSpatialFriction() override = default;
      int getFrictionDirections() override { return 2; }
      bool isSticking(const fmatvec::Vec& s, double sTol) override { return nrm2(s) <= sTol; }
  };

}

#endif
