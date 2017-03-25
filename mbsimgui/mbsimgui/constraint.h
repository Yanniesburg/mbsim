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
#include "extended_properties.h"

namespace MBSimGUI {

  class RigidBody;

  //  class RigidBodyOfReferencePropertyFactory : public PropertyFactory {
  //    public:
  //      RigidBodyOfReferencePropertyFactory(Element *element_, const MBXMLUtils::FQN &xmlName_) : element(element_), xmlName(xmlName_) { }
  //      Property* createProperty(int i=0);
  //    protected:
  //      Element *element;
  //      MBXMLUtils::FQN xmlName;
  //  };
  //
  //  class GeneralizedGearConstraintPropertyFactory : public PropertyFactory {
  //    public:
  //      GeneralizedGearConstraintPropertyFactory(Element *element_, const MBXMLUtils::FQN &xmlName_="") : element(element_), xmlName(xmlName_) { }
  //      Property* createProperty(int i=0);
  //    protected:
  //      Element *element;
  //      MBXMLUtils::FQN xmlName;
  //  };

  class Constraint : public Element {
    public:
      Constraint(const QString &str="") : Element(str) { }
  };

  class MechanicalConstraint : public Constraint {
    public:
      MechanicalConstraint(const QString &str="") : Constraint(str) { }
  };

  class GeneralizedConstraint : public MechanicalConstraint {
    public:
      GeneralizedConstraint(const QString &str="");
  };

  class GeneralizedGearConstraint : public GeneralizedConstraint {
    public:
      GeneralizedGearConstraint(const QString &str="");
      QString getType() const { return "GeneralizedGearConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedGearConstraintPropertyDialog(this);}
  };

  class GeneralizedDualConstraint : public GeneralizedConstraint {
    public:
      GeneralizedDualConstraint(const QString &str="");
      QString getType() const { return "GeneralizedDualConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedDualConstraintPropertyDialog(this);}
  };

  class GeneralizedPositionConstraint : public GeneralizedDualConstraint {
    public:
      GeneralizedPositionConstraint(const QString &str="");
      QString getType() const { return "GeneralizedPositionConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedPositionConstraintPropertyDialog(this);}
  };

  class GeneralizedVelocityConstraint : public GeneralizedDualConstraint {
    public:
      GeneralizedVelocityConstraint(const QString &str="");
      QString getType() const { return "GeneralizedVelocityConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedVelocityConstraintPropertyDialog(this);}
  };

  class GeneralizedAccelerationConstraint : public GeneralizedDualConstraint {
    public:
      GeneralizedAccelerationConstraint(const QString &str="");
      QString getType() const { return "GeneralizedAccelerationConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedAccelerationConstraintPropertyDialog(this);}
  };

  class JointConstraint : public MechanicalConstraint {
    public:
      JointConstraint(const QString &str="");
      QString getType() const { return "JointConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new JointConstraintPropertyDialog(this);}
  };

  class GeneralizedConnectionConstraint : public GeneralizedDualConstraint {
    public:
      GeneralizedConnectionConstraint(const QString &str="") : GeneralizedDualConstraint(str) { }
      QString getType() const { return "GeneralizedConnectionConstraint"; }
      ElementPropertyDialog* createPropertyDialog() {return new GeneralizedConnectionConstraintPropertyDialog(this);}
  };

}

#endif
