/*
    MBSimGUI - A fronted for MBSim.
    Copyright (C) 2017 Martin Förg

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

#ifndef _PROJECT__H_
#define _PROJECT__H_

#include "embeditemdata.h"
#include "project_property_dialog.h"
#include "namespace.h"

namespace XERCES_CPP_NAMESPACE {
  class DOMElement;
  class DOMNode;
}

namespace MBSimGUI {

  class DynamicSystemSolver;
  class Solver;

  class Project : public EmbedItemData {
    public:
      Project()  = default;
      ~Project() override;
      virtual void removeXMLElements();
      virtual xercesc::DOMElement* createXMLElement(xercesc::DOMNode *parent);
      virtual void initializeUsingXML(xercesc::DOMElement *element);
      QString getType() const override { return "MBSimProject"; }
      virtual MBXMLUtils::NamespaceURI getNameSpace() const { return MBSIMXML; }
      virtual ProjectPropertyDialog* createPropertyDialog() { return new ProjectPropertyDialog(this); }
      QMenu* createContextMenu() override { return nullptr; }
      EmbeddingPropertyDialog* createEmbeddingPropertyDialog() override { return new EmbeddingPropertyDialog(this,false); }
      xercesc::DOMElement* processIDAndHref(xercesc::DOMElement* element) override;
      xercesc::DOMElement* processHref(xercesc::DOMElement* element) override;
      void setDynamicSystemSolver(DynamicSystemSolver *dss_);
      void setSolver(Solver *solver_);
      DynamicSystemSolver* getDynamicSystemSolver() const { return dss; }
      Solver* getSolver() const { return solver; }
      xercesc::DOMElement* createEmbedXMLElement() override;
      void setEmbeded(bool embeded_) override;
      void maybeRemoveEmbedXMLElement() override;
    private:
      DynamicSystemSolver *dss{nullptr};
      Solver *solver{nullptr};
  };

}

#endif
