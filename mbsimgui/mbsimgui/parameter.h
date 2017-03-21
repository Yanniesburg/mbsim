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
#include "basic_properties.h"
#include "extended_properties.h"
#include "parameter_property_dialog.h"
#include "parameter_context_menu.h"

namespace XERCES_CPP_NAMESPACE {
  class DOMElement;
  class DOMNode;
}

namespace MBSimGUI {

  class PropertyWidget;
  class PropertyDialog;
  class ExtWidget;
  class TextWidget;

  class Parameter : public TreeItemData {
    friend class ParameterPropertyDialog;
    public:
      Parameter(const std::string &name="");
      virtual ~Parameter() {}
      virtual std::string getValue() const {return valuestr;}
      void setValue(const std::string &value) {valuestr = value;}
      virtual xercesc::DOMElement* createXMLElement(xercesc::DOMNode *parent);
      virtual void initializeUsingXML(xercesc::DOMElement *element);
      virtual xercesc::DOMElement* writeXMLFile(xercesc::DOMNode *element);
      virtual std::string getType() const { return "Parameter"; }
      const std::string& getName() const { return name; }
      void setName(const std::string &str) { name = str; }
      virtual ParameterPropertyDialog* createPropertyDialog() {return new ParameterPropertyDialog(this);}
      virtual ParameterContextMenu* createContextMenu() {return new ParameterContextMenu;}
      xercesc::DOMElement* getXMLElement() { return element; }
      virtual void removeXMLElements();
      Element* getParent() { return parent; }
      void setParent(Element* parent_) { parent = parent_; }
      bool getConfig() { return config; }
      void setConfig(bool config_) { config = config_; }
    protected:
      Element *parent;
      std::string name, valuestr;
      xercesc::DOMElement *element;
      bool config;
  };

  class StringParameter : public Parameter {
    friend class StringParameterPropertyDialog;
    public:
      StringParameter(const std::string &name="");
      virtual ~StringParameter() {}
      virtual std::string getType() const { return "stringParameter"; }
      virtual ParameterPropertyDialog* createPropertyDialog() {return new StringParameterPropertyDialog(this);}
  };

  class ScalarParameter : public Parameter {
    friend class ScalarParameterPropertyDialog;
    public:
      ScalarParameter(const std::string &name="", const std::string &value="0");
      virtual ~ScalarParameter() {}
      virtual std::string getType() const { return "scalarParameter"; }
      virtual ParameterPropertyDialog* createPropertyDialog() {return new ScalarParameterPropertyDialog(this);}
  };

  class VectorParameter : public Parameter {
    friend class VectorParameterPropertyDialog;
    public:
      VectorParameter(const std::string &name="");
      virtual ~VectorParameter() {}
      virtual std::string getType() const { return "vectorParameter"; }
      virtual ParameterPropertyDialog* createPropertyDialog() {return new VectorParameterPropertyDialog(this);}
  };

  class MatrixParameter : public Parameter {
    friend class MatrixParameterPropertyDialog;
    public:
      MatrixParameter(const std::string &name="");
      virtual ~MatrixParameter() {}
      virtual std::string getType() const { return "matrixParameter"; }
      virtual ParameterPropertyDialog* createPropertyDialog() {return new MatrixParameterPropertyDialog(this);}
  };

  class ImportParameter : public Parameter {
    friend class ImportParameterPropertyDialog;
    public:
      ImportParameter();
      virtual ~ImportParameter() {}
      virtual std::string getType() const { return "import"; }
      virtual ParameterPropertyDialog* createPropertyDialog() {return new ImportParameterPropertyDialog(this);}
  };

  class Parameters {
    protected:
      std::vector<Parameter*> parameter;
    public:
      Parameters() { }
      void addParameter(Parameter *param);
      void addParameters(const Parameters &list); 
      void removeParameter(Parameter *param);
      void removeParameters();
      Parameter *getParameter(int i) const { return parameter[i]; }
      int getNumberOfParameters() const { return parameter.size(); }
      Element* getParent() { return parent; }
      void setParent(Element* parent_) { parent = parent_; }
      void initializeUsingXML(xercesc::DOMElement *element);
      xercesc::DOMElement* writeXMLFile(xercesc::DOMNode *element);
      static Parameters readXMLFile(const std::string &filename);
      virtual void writeXMLFile(const std::string &name);
    private:
      Element *parent;
  };

}

#endif
