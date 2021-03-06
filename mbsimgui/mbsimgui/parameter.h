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

#ifndef _PARAMETER__H_
#define _PARAMETER__H_

#include "treeitemdata.h"
#include "parameter_property_dialog.h"
#include "parameter_context_menu.h"

namespace XERCES_CPP_NAMESPACE {
  class DOMElement;
  class DOMNode;
}

namespace MBSimGUI {

  class EmbedItemData;
  class PropertyWidget;
  class PropertyDialog;
  class ExtWidget;
  class TextWidget;

  class Parameter : public TreeItemData {
    public:
      Parameter()  = default;
      QString getName() const override { return QString::fromStdString(MBXMLUtils::E(element)->getAttribute("name")); }
      QString getValue() const override { return MBXMLUtils::E(element)->getFirstTextChild()?QString::fromStdString(MBXMLUtils::X()%MBXMLUtils::E(element)->getFirstTextChild()->getData()):""; }
      xercesc::DOMElement* createXMLElement(xercesc::DOMNode *parent);
      virtual void initializeUsingXML(xercesc::DOMElement *element);
      virtual ParameterPropertyDialog* createPropertyDialog() { return new ParameterPropertyDialog(this); }
      ParameterContextMenu* createContextMenu() override { return new ParameterContextMenu(this); }
      xercesc::DOMElement* getXMLElement() { return element; }
      void setXMLElement(xercesc::DOMElement *element_) { element = element_; }
      virtual void removeXMLElements();
      EmbedItemData* getParent() { return parent; }
      void setParent(EmbedItemData* parent_) { parent = parent_; }
      bool getConfig() { return config; }
      void setConfig(bool config_) { config = config_; }
      static std::vector<Parameter*> initializeParametersUsingXML(xercesc::DOMElement *element);
    protected:
      EmbedItemData *parent{nullptr};
      xercesc::DOMElement *element;
      bool config{false};
  };

  class StringParameter : public Parameter {
    public:
      StringParameter() = default;
      QString getType() const override { return "stringParameter"; }
      void initializeUsingXML(xercesc::DOMElement *element) override;
      ParameterPropertyDialog* createPropertyDialog() override {return new StringParameterPropertyDialog(this);}
  };

  class ScalarParameter : public Parameter {
    public:
      ScalarParameter() = default;
      QString getType() const override { return "scalarParameter"; }
      void initializeUsingXML(xercesc::DOMElement *element) override;
      ParameterPropertyDialog* createPropertyDialog() override {return new ScalarParameterPropertyDialog(this);}
  };

  class VectorParameter : public Parameter {
    public:
      VectorParameter() = default;
      QString getType() const override { return "vectorParameter"; }
      void initializeUsingXML(xercesc::DOMElement *element) override;
      ParameterPropertyDialog* createPropertyDialog() override {return new VectorParameterPropertyDialog(this);}
  };

  class MatrixParameter : public Parameter {
    public:
      MatrixParameter() = default;
      QString getType() const override { return "matrixParameter"; }
      void initializeUsingXML(xercesc::DOMElement *element) override;
      ParameterPropertyDialog* createPropertyDialog() override {return new MatrixParameterPropertyDialog(this);}
  };

  class ImportParameter : public Parameter {
    public:
      ImportParameter() = default;
      QString getType() const override { return "import"; }
      ParameterPropertyDialog* createPropertyDialog() override {return new ImportParameterPropertyDialog(this);}
  };

}

#endif
