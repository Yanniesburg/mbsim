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
 * Contact: martin.o.foerg@gmail.com
 */

#ifndef _GEARING_H_
#define _GEARING_H_

#include "mbsim/mechanical_link.h"
#include "mbsim/rigid_body.h"
#include "mbsim/frames/frame.h"

namespace MBSim {

  class Gearing : public MechanicalLink {
    protected:
      Function<double(double,double)> *func;
      double r0, r1;
      bool reverse;
      Frame Z0, Z1;
      Frame *P0, *P1;
      fmatvec::Vec3 WrP0Z, WrP1Z;
      fmatvec::Vec3 Wt;
    public:
      Gearing(const std::string &name);
      void updateh(int i=0);
      void updateW(int i=0);
      void updateJacobians(int j=0);
      void updateg();
      void updategd();
      void updatewb();
      void updatehRef(const fmatvec::Vec &hParent, int j=0);
      void updateWRef(const fmatvec::Mat &WParent, int j=0);
      void connect(double r1, Frame* frame1_, double r2, Frame* frame2_);
      virtual void updatexd();
      virtual void calcxSize();

      bool isActive() const { return true; }
      bool gActiveChanged() { return false; }
      void init(InitStage stage, const InitConfigSet &config);
      bool isSetValued() const;
      virtual void calclaSize(int j);
      virtual void calcgSize(int j);
      virtual void calcgdSize(int j);

      void setForceFunction(Function<double(double,double)> *func_) {
        func=func_;
        func->setParent(this);
        func->setName("Force");
      }
      void setReverse(bool reverse_) { reverse = reverse_; }
  };

}

#endif
