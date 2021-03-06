/*
    MBSimGUI - A fronted for MBSim.
    Copyright (C) 2012 Martin Förg

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _JOINT__H_
#define _JOINT__H_

#include "link.h"

namespace MBSimGUI {

  class Joint : public FloatingFrameLink {
    public:
      QString getType() const override { return "Joint"; }
      ElementPropertyDialog* createPropertyDialog() override {return new JointPropertyDialog(this);}
  };

  class ElasticJoint : public FloatingFrameLink {
    public:
      QString getType() const override { return "ElasticJoint"; }
      ElementPropertyDialog* createPropertyDialog() override {return new ElasticJointPropertyDialog(this);}
  };

}

#endif
