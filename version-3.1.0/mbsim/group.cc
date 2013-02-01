/* Copyright (C) 2004-2009 MBSim Development Team
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

#include <config.h>
#include "mbsim/group.h"
#include "mbsim/object.h"
#include "mbsim/link.h"
#include "mbsim/extra_dynamic.h"
#include "mbsim/frame.h"
#include "mbsim/contour.h"
#include "mbsim/dynamic_system_solver.h"
#include "hdf5serie/simpleattribute.h"
#include "mbsim/objectfactory.h"

#ifdef HAVE_OPENMBVCPPINTERFACE
#include <openmbvcppinterface/frame.h>
#endif

//#ifdef _OPENMP
//#include <omp.h>
//#endif

using namespace std;
using namespace fmatvec;

namespace MBSim {

  Group::Group(const string &name) : DynamicSystem(name) {}

  Group::~Group() {}

  void Group::facLLM(int j) {
    for(vector<DynamicSystem*>::iterator i = dynamicsystem.begin(); i != dynamicsystem.end(); ++i)
      (*i)->facLLM(j);

    for(vector<Object*>::iterator i = object.begin(); i != object.end(); ++i) 
      (*i)->facLLM(j);
  }

  void Group::updateStateDependentVariables(double t) {
    for(int i=0; i<(int)dynamicsystem.size(); i++) {
      try { dynamicsystem[i]->updateStateDependentVariables(t); }
      catch(MBSimError error) { error.printExceptionMessage(); throw; }
    }

    for(int i=0; i<(int)object.size(); i++) {
      try { object[i]->updateStateDependentVariables(t); }
      catch(MBSimError error) { error.printExceptionMessage(); throw; }
    }
  }

  void Group::updateJacobians(double t, int j) {
    for(int i=0; i<(int)dynamicsystem.size(); i++) {
      try { dynamicsystem[i]->updateJacobians(t,j); }
      catch(MBSimError error) { error.printExceptionMessage(); throw; }
    }

    for(int i=0; i<(int)object.size(); i++) {
      try { object[i]->updateJacobians(t,j); }
      catch(MBSimError error) { error.printExceptionMessage(); throw; }
    }

    for(int i=0; i<(int)link.size(); i++) {
      try { link[i]->updateJacobians(t,j); }
      catch(MBSimError error) { error.printExceptionMessage(); throw; }
    }
  }

  void Group::updatedu(double t, double dt) {
    for(vector<DynamicSystem*>::iterator i = dynamicsystem.begin(); i != dynamicsystem.end(); ++i) 
      (*i)->updatedu(t,dt);

    for(vector<Object*>::iterator i = object.begin(); i != object.end(); ++i)
      (*i)->updatedu(t,dt);
  }

  void Group::updateud(double t, int j) {
    for(vector<DynamicSystem*>::iterator i = dynamicsystem.begin(); i != dynamicsystem.end(); ++i) 
      (*i)->updateud(t,j);

    for(vector<Object*>::iterator i = object.begin(); i != object.end(); ++i)
      (*i)->updateud(t,j);
  }

  void Group::updatezd(double t) {
    for(vector<DynamicSystem*>::iterator i = dynamicsystem.begin(); i != dynamicsystem.end(); ++i) 
      (*i)->updatezd(t);

    for(vector<Object*>::iterator i = object.begin(); i != object.end(); ++i) 
      (*i)->updatezd(t);

    for(vector<Link*>::iterator i = link.begin(); i != link.end(); ++i)
      (*i)->updatexd(t);

    for(vector<ExtraDynamic*>::iterator i = extraDynamic.begin(); i!= extraDynamic.end(); ++i) 
      (*i)->updatexd(t);
  }

  void Group::initializeUsingXML(TiXmlElement *element) {
    TiXmlElement *e;
    Element::initializeUsingXML(element);
    e=element->FirstChildElement();

    // search first element known by Group
    while(e && e->ValueStr()!=MBSIMNS"frameOfReference" &&
        e->ValueStr()!=MBSIMNS"position" &&
        e->ValueStr()!=MBSIMNS"orientation" &&
        e->ValueStr()!=MBSIMNS"frames")
      e=e->NextSiblingElement();

    if(e && e->ValueStr()==MBSIMNS"frameOfReference") {
      string ref=e->Attribute("ref");
      setFrameOfReference(getByPath<Frame>(ref)); // must be a Frame of the parent, so it allready exists (no need to resolve path later)
      e=e->NextSiblingElement();
    }

    if(e && e->ValueStr()==MBSIMNS"position") {
      setPosition(getVec3(e));
      e=e->NextSiblingElement();
    }

    if(e && e->ValueStr()==MBSIMNS"orientation") {
      setOrientation(getSqrMat3(e));
      e=e->NextSiblingElement();
    }

    // frames
    TiXmlElement *E=e->FirstChildElement();
    while(E && E->ValueStr()==MBSIMNS"frame") {
      TiXmlElement *ec=E->FirstChildElement();
      Frame *f=new Frame(ec->Attribute("name"));
      f->initializeUsingXML(ec);
      ec=ec->NextSiblingElement();
      string refF="I";
      if(ec->ValueStr()==MBSIMNS"frameOfReference") {
        refF=ec->Attribute("ref");
        refF=refF.substr(6, refF.length()-7); // reference frame is allways "Frame[X]"
        ec=ec->NextSiblingElement();
      }
      Vec3 RrRF=getVec3(ec);
      ec=ec->NextSiblingElement();
      SqrMat3 ARF=getSqrMat3(ec);
      addFrame(f, RrRF, ARF, refF);
      E=E->NextSiblingElement();
    }
    e=e->NextSiblingElement();

    // contours
    E=e->FirstChildElement();
    while(E && E->ValueStr()==MBSIMNS"contour") {
      TiXmlElement *ec=E->FirstChildElement();
      Contour *c=ObjectFactory::getInstance()->createContour(ec);
      TiXmlElement *contourElement=ec; // save for later initialization
      ec=ec->NextSiblingElement();
      string refF="I";
      if(ec->ValueStr()==MBSIMNS"frameOfReference") {
        refF=ec->Attribute("ref");
        refF=refF.substr(6, refF.length()-7); // reference frame is allways "Frame[X]"
        ec=ec->NextSiblingElement();
      }
      Vec3 RrRC=getVec3(ec);
      ec=ec->NextSiblingElement();
      SqrMat3 ARC=getSqrMat3(ec);
      addContour(c, RrRC, ARC, refF);
      c->initializeUsingXML(contourElement);
      E=E->NextSiblingElement();
    }
    e=e->NextSiblingElement();

    // groups
    E=e->FirstChildElement();
    Group *g;
    while(E) {
      g=ObjectFactory::getInstance()->createGroup(E);
      addGroup(g);
      g->initializeUsingXML(E);
      E=E->NextSiblingElement();
    }
    e=e->NextSiblingElement();

    // objects
    E=e->FirstChildElement();
    Object *o;
    while(E) {
      o=ObjectFactory::getInstance()->createObject(E);
      addObject(o);
      o->initializeUsingXML(E);
      E=E->NextSiblingElement();
    }
    e=e->NextSiblingElement();

    // extraDynamics
    if (e->ValueStr()==MBSIMNS"extraDynamics") {
      E=e->FirstChildElement();
      ExtraDynamic *ed;
      while(E) {
        ed=ObjectFactory::getInstance()->createExtraDynamic(E);
        addExtraDynamic(ed);
        ed->initializeUsingXML(E);
        E=E->NextSiblingElement();
      }
      e=e->NextSiblingElement();
    }

    // links
    E=e->FirstChildElement();
    Link *l;
    while(E) {
      l=ObjectFactory::getInstance()->createLink(E);
      addLink(l);
      l->initializeUsingXML(E);
      E=E->NextSiblingElement();
    }
#ifdef HAVE_OPENMBVCPPINTERFACE

    e=element->FirstChildElement(MBSIMNS"enableOpenMBVFrameI");
    if(e) {
      //if(!openMBVBody)
        //setOpenMBVRigidBody(new OpenMBV::InvisibleBody);
      I->enableOpenMBV(getDouble(e->FirstChildElement(MBSIMNS"size")),
          getDouble(e->FirstChildElement(MBSIMNS"offset")));

      // pass a OPENMBV_ID processing instruction to the OpenMBV Frame object
      for(TiXmlNode *child=e->FirstChild(); child; child=child->NextSibling()) {
        TiXmlUnknown *unknown=child->ToUnknown();
        const size_t length=strlen("?OPENMBV_ID ");
        if(unknown && unknown->ValueStr().substr(0, length)=="?OPENMBV_ID ")
          I->getOpenMBVFrame()->setID(unknown->ValueStr().substr(length, unknown->ValueStr().length()-length-1));
      }
    }
#endif
  }

  TiXmlElement* Group::writeXMLFile(TiXmlNode *parent) {
    TiXmlElement *ele0 = DynamicSystem::writeXMLFile(parent);

    TiXmlElement *ele1;

    if(getFrameOfReference()) {
      ele1 = new TiXmlElement( MBSIMNS"frameOfReference" );
      ele1->SetAttribute("ref", frameParent->getXMLPath(this,true));
      ele0->LinkEndChild(ele1);
    }

    addElementText(ele0,MBSIMNS"position",getPosition());
    addElementText(ele0,MBSIMNS"orientation", getOrientation());

    ele1 = new TiXmlElement( MBSIMNS"frames" );
    for(unsigned int i=1; i<frame.size(); i++) {
      TiXmlElement* ele2 = new TiXmlElement( MBSIMNS"frame" );
      ele1->LinkEndChild( ele2 );
      frame[i]->writeXMLFile(ele2);
      if(saved_refFrameF[i-1] != "I") {
        TiXmlElement *ele3 = new TiXmlElement( MBSIMNS"frameOfReference" );
        string str = string("Frame[") + saved_refFrameF[i-1] + "]";
        ele3->SetAttribute("ref", str);
        ele2->LinkEndChild(ele3);
      }

      addElementText(ele2,MBSIMNS"position",saved_RrRF[i-1]);
      addElementText(ele2,MBSIMNS"orientation",saved_ARF[i-1]);
    }
    ele0->LinkEndChild( ele1 );

    ele1 = new TiXmlElement( MBSIMNS"contours" );
    for(vector<Contour*>::iterator i = contour.begin(); i != contour.end(); ++i) 
      (*i)->writeXMLFile(ele1);
    ele0->LinkEndChild( ele1 );

    ele1 = new TiXmlElement( MBSIMNS"groups" );
    for(vector<DynamicSystem*>::iterator i = dynamicsystem.begin(); i != dynamicsystem.end(); ++i) 
      (*i)->writeXMLFile(ele1);
    ele0->LinkEndChild( ele1 );

    ele1 = new TiXmlElement( MBSIMNS"objects" );
    for(vector<Object*>::iterator i = object.begin(); i != object.end(); ++i) 
      (*i)->writeXMLFile(ele1);
    ele0->LinkEndChild( ele1 );

    ele1 = new TiXmlElement( MBSIMNS"extraDynamics" );
    for(vector<ExtraDynamic*>::iterator i = extraDynamic.begin(); i != extraDynamic.end(); ++i) 
      (*i)->writeXMLFile(ele1);
    ele0->LinkEndChild( ele1 );

    ele1 = new TiXmlElement( MBSIMNS"links" );
    for(vector<Link*>::iterator i = link.begin(); i != link.end(); ++i) 
      (*i)->writeXMLFile(ele1);
    ele0->LinkEndChild( ele1 );

    if(I->getOpenMBVFrame()) {
      ele1 = new TiXmlElement( MBSIMNS"enableOpenMBVFrameI" );
      addElementText(ele1,MBSIMNS"size",I->getOpenMBVFrame()->getSize());
      addElementText(ele1,MBSIMNS"offset",I->getOpenMBVFrame()->getOffset());
      ele0->LinkEndChild(ele1);
    }

    return ele0;
  }

}

