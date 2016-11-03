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

#ifndef _LINK__H_
#define _LINK__H_

#include "element.h"

namespace MBSimGUI {

  class Link : public Element {
    public:
      Link(const std::string &str, Element *parent) : Element(str,parent) { }
      static Link* readXMLFile(const std::string &filename, Element *parent);
      virtual int getxSize() {return 0;}
  };

//  class FrameLink : public Link {
//    friend class FrameLinkPropertyDialog;
//    public:
//      FrameLink(const std::string &str, Element *parent);
//      void initialize();
//      xercesc::DOMElement* initializeUsingXML(xercesc::DOMElement *element);
//      xercesc::DOMElement* writeXMLFile(xercesc::DOMNode *element);
//    protected:
//      ExtProperty connections, forceArrow;
// };

  class FloatingFrameLink : public Link {
    friend class FloatingFrameLinkPropertyDialog;
    public:
      FloatingFrameLink(const std::string &str, Element *parent);
      void initialize();
      xercesc::DOMElement* initializeUsingXML(xercesc::DOMElement *element);
      xercesc::DOMElement* writeXMLFile(xercesc::DOMNode *element);
    protected:
      ExtProperty connections, refFrameID, forceArrow, momentArrow;
 };

  class RigidBodyLink : public Link {
    friend class RigidBodyLinkPropertyDialog;
    public:
      RigidBodyLink(const std::string &str, Element *parent);
      void initialize();
      xercesc::DOMElement* initializeUsingXML(xercesc::DOMElement *element);
      xercesc::DOMElement* writeXMLFile(xercesc::DOMNode *element);
    protected:
      ExtProperty support;
 };

}

#endif
