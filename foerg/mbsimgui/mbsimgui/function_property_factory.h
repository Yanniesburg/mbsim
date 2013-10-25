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

#ifndef _FUNCTION_PROPERTY_FACTORY_H_
#define _FUNCTION_PROPERTY_FACTORY_H_

#include "property.h"
#include <vector>

class Element;

//class FunctionPropertyFactory : public PropertyFactory {
//  public:
//    FunctionPropertyFactory(const std::string &omit_, int n_, const std::string &xmlName_="", int mode_=1) : omit(omit_), n(n_), xmlName(xmlName_), mode(mode_) { }
//    Property* createProperty(int i=0);
//  protected:
//    std::string omit, xmlName;
//    int n, mode;
//};
//
//class TranslationPropertyFactory : public PropertyFactory {
//  public:
//    TranslationPropertyFactory(const std::string &omit_, int n_, const std::string &xmlName_="", int mode_=1) : omit(omit_), n(n_), xmlName(xmlName_), mode(mode_) { }
//    Property* createProperty(int i=0);
//  protected:
//    std::string omit, xmlName;
//    int n, mode;
//};
//
//class RotationPropertyFactory : public PropertyFactory {
//  public:
//    RotationPropertyFactory(const std::string &omit_, int n_, const std::string &xmlName_="", int mode_=1) : omit(omit_), n(n_), xmlName(xmlName_), mode(mode_) { }
//    Property* createProperty(int i=0);
//  protected:
//    std::string omit, xmlName;
//    int n, mode;
//};

class FunctionPropertyFactory2 : public PropertyFactory {
  public:
    FunctionPropertyFactory2() : name(FunctionPropertyFactory2::getNames()) { }
    Property* createProperty(int i=0);
    static std::vector<std::string> getNames();
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
};

class TranslationPropertyFactory2 : public PropertyFactory {
  public:
    TranslationPropertyFactory2() : name(TranslationPropertyFactory2::getNames()) { }
    Property* createProperty(int i=0);
    static std::vector<std::string> getNames();
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
};

class TranslationPropertyFactory3 : public PropertyFactory {
  public:
    TranslationPropertyFactory3() : name(TranslationPropertyFactory3::getNames()) { }
    Property* createProperty(int i=0);
    static std::vector<std::string> getNames();
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
};

class RotationPropertyFactory2 : public PropertyFactory {
  public:
    RotationPropertyFactory2() : name(RotationPropertyFactory2::getNames()) { }
    Property* createProperty(int i=0);
    static std::vector<std::string> getNames();
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
};

class RotationPropertyFactory3 : public PropertyFactory {
  public:
    RotationPropertyFactory3() : name(RotationPropertyFactory3::getNames()) { }
    Property* createProperty(int i=0);
    static std::vector<std::string> getNames();
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
};

class SymbolicFunctionPropertyFactory2 : public PropertyFactory {
  public:
    SymbolicFunctionPropertyFactory2(const std::string &ext_, const std::vector<std::string> &var_) : name(SymbolicFunctionPropertyFactory2::getNames()), ext(ext_), var(var_) { }
    Property* createProperty(int i=0);
    static std::vector<std::string> getNames();
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
    std::string ext;
    std::vector<std::string> var;
};

class TranslationPropertyFactory4 : public PropertyFactory {
  public:
    TranslationPropertyFactory4();
    Property* createProperty(int i=0);
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
};

class RotationPropertyFactory4 : public PropertyFactory {
  public:
    RotationPropertyFactory4();
    Property* createProperty(int i=0);
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
};

class TabularFunctionPropertyFactory : public PropertyFactory {
  public:
    TabularFunctionPropertyFactory();
    Property* createProperty(int i=0);
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
};

class ConstraintPropertyFactory : public PropertyFactory {
  public:
    ConstraintPropertyFactory();
    Property* createProperty(int i=0);
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
};

class ConnectFramesPropertyFactory : public PropertyFactory {
  public:
    ConnectFramesPropertyFactory(Element *element);
    Property* createProperty(int i=0);
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
    Element *element;
};

class SpringDamperPropertyFactory: public PropertyFactory {
  public:
    SpringDamperPropertyFactory();
    Property* createProperty(int i=0);
    std::string getName(int i=0) const { return name[i]; }
    int getSize() const { return name.size(); }
  protected:
    std::vector<std::string> name;
};

#endif
