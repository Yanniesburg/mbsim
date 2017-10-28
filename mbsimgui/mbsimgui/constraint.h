/*
    MBSimGUI - A fronted for MBSim.
    Copyright (C) 2012-2016 Martin Förg

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

#ifndef _CONSTRAINT__H_
#define _CONSTRAINT__H_

#include "object.h"

namespace MBSimGUI {

  class Constraint : public Element {
    public:
      QMenu* createContextMenu() { return new ConstraintContextMenu(this); }
  };

  class MechanicalConstraint : public Constraint {
  };

  class GeneralizedConstraint : public MechanicalConstraint {
  };

  class GeneralizedGearConstraint : public GeneralizedConstraint {
    public:
      QString getType() const { return "GeneralizedGearConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedGearConstraintPropertyDialog(this);}
  };

  class GeneralizedDualConstraint : public GeneralizedConstraint {
    public:
      QString getType() const { return "GeneralizedDualConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedDualConstraintPropertyDialog(this);}
  };

  class GeneralizedPositionConstraint : public GeneralizedDualConstraint {
    public:
      QString getType() const { return "GeneralizedPositionConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedPositionConstraintPropertyDialog(this);}
  };

  class GeneralizedVelocityConstraint : public GeneralizedDualConstraint {
    public:
      QString getType() const { return "GeneralizedVelocityConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedVelocityConstraintPropertyDialog(this);}
  };

  class GeneralizedAccelerationConstraint : public GeneralizedDualConstraint {
    public:
      QString getType() const { return "GeneralizedAccelerationConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedAccelerationConstraintPropertyDialog(this);}
  };

  class JointConstraint : public MechanicalConstraint {
    public:
      QString getType() const { return "JointConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new JointConstraintPropertyDialog(this);}
  };

  class GeneralizedConnectionConstraint : public GeneralizedDualConstraint {
    public:
      QString getType() const { return "GeneralizedConnectionConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedConnectionConstraintPropertyDialog(this);}
  };

}

#endif
