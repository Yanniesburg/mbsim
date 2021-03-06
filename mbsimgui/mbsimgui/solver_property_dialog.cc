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

#include <config.h>
#include "solver_property_dialog.h"
#include "solver.h"
#include "basic_widgets.h"
#include "variable_widgets.h"
#include "extended_widgets.h"
#include <QDialogButtonBox>
#include <QPushButton>

using namespace std;
using namespace MBXMLUtils;
using namespace xercesc;

namespace MBSimGUI {

  ToleranceWidgetFactory::ToleranceWidgetFactory(const QString &type_) : type(type_) {
    name.emplace_back("Scalar");
    name.emplace_back("Vector");
    xmlName.push_back(MBSIM%(type.toStdString()+"Scalar"));
    xmlName.push_back(MBSIM%(type.toStdString()));
  }

  QWidget* ToleranceWidgetFactory::createWidget(int i) {
    if(i==0)
      return new ChoiceWidget2(new ScalarWidgetFactory("1e-6"),QBoxLayout::RightToLeft,5);
    if(i==1)
      return new ChoiceWidget2(new VecSizeVarWidgetFactory(1,1,100,1,vector<QStringList>(3,QStringList()),vector<int>(3,0),false,false,true,"1e-6"),QBoxLayout::RightToLeft,5);
    return nullptr;
  }

  SolverPropertyDialog::SolverPropertyDialog(Solver *solver) : EmbedItemPropertyDialog(solver) {
  }

  DOMElement* SolverPropertyDialog::initializeUsingXML(DOMElement *parent) {
    return parent;
  }

  DOMElement* SolverPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    item->removeXMLElements();
    return nullptr;
  }

  IntegratorPropertyDialog::IntegratorPropertyDialog(Solver *solver) : SolverPropertyDialog(solver) {
    addTab("General");
    addTab("Initial conditions");

    startTime = new ExtWidget("Start time",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),false,false,MBSIM%"startTime");
    addToTab("General", startTime);

    endTime = new ExtWidget("End time",new ChoiceWidget2(new ScalarWidgetFactory("1",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),false,false,MBSIM%"endTime");
    addToTab("General", endTime);

    plotStepSize = new ExtWidget("Plot step size",new ChoiceWidget2(new ScalarWidgetFactory("1e-2",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),false,false,MBSIM%"plotStepSize");
    addToTab("General", plotStepSize);

    initialState = new ExtWidget("Initial state",new ChoiceWidget2(new VecSizeVarWidgetFactory(1),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialState");
    addToTab("Initial conditions", initialState);
  }

  DOMElement* IntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    SolverPropertyDialog::initializeUsingXML(item->getXMLElement());
    startTime->initializeUsingXML(item->getXMLElement());
    endTime->initializeUsingXML(item->getXMLElement());
    plotStepSize->initializeUsingXML(item->getXMLElement());
    initialState->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* IntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    SolverPropertyDialog::writeXMLFile(item->getXMLElement());
    startTime->writeXMLFile(item->getXMLElement());
    endTime->writeXMLFile(item->getXMLElement());
    plotStepSize->writeXMLFile(item->getXMLElement());
    initialState->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  RootFindingIntegratorPropertyDialog::RootFindingIntegratorPropertyDialog(Solver *solver) : IntegratorPropertyDialog(solver) {
    addTab("Tolerances");
    addTab("Root-finding");

    gMax = new ExtWidget("Tolerance for position constraint",new ChoiceWidget2(new ScalarWidgetFactory("1e-5"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"toleranceForPositionConstraints");
    addToTab("Tolerances", gMax);

    gdMax = new ExtWidget("Tolerance for velocity constraint",new ChoiceWidget2(new ScalarWidgetFactory("1e-5"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"toleranceForVelocityConstraints");
    addToTab("Tolerances", gdMax);

    dtRoot = new ExtWidget("Root finding accuracy",new ChoiceWidget2(new ScalarWidgetFactory("1e-10",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"rootFindingAccuracy");
    addToTab("Root-finding", dtRoot);

    plotOnRoot = new ExtWidget("Plot on root",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"plotOnRoot");
    addToTab("Root-finding", plotOnRoot);
  }

  DOMElement* RootFindingIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    IntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    gMax->initializeUsingXML(item->getXMLElement());
    gdMax->initializeUsingXML(item->getXMLElement());
    dtRoot->initializeUsingXML(item->getXMLElement());
    plotOnRoot->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* RootFindingIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    IntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    gMax->writeXMLFile(item->getXMLElement());
    gdMax->writeXMLFile(item->getXMLElement());
    dtRoot->writeXMLFile(item->getXMLElement());
    plotOnRoot->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  DOPRI5IntegratorPropertyDialog::DOPRI5IntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);
  }

  DOMElement* DOPRI5IntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* DOPRI5IntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  DOP853IntegratorPropertyDialog::DOP853IntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("1e-3",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);
  }

  DOMElement* DOP853IntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* DOP853IntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  ODEXIntegratorPropertyDialog::ODEXIntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);
  }

  DOMElement* ODEXIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* ODEXIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  RADAU5IntegratorPropertyDialog::RADAU5IntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");
    addTab("Extra");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);

    vector<QString> list;
    list.emplace_back("\"ODE\"");
    list.emplace_back("\"DAE1\"");
    list.emplace_back("\"DAE2\"");
    list.emplace_back("\"DAE3\"");
    list.emplace_back("\"GGL\"");
    formalism = new ExtWidget("Formalism",new TextChoiceWidget(list,0,true),true,false,MBSIM%"formalism");
    addToTab("General", formalism);

    reducedForm = new ExtWidget("Reduced form",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"reducedForm");
    addToTab("Extra", reducedForm);
  }

  DOMElement* RADAU5IntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    formalism->initializeUsingXML(item->getXMLElement());
    reducedForm->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* RADAU5IntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    formalism->writeXMLFile(item->getXMLElement());
    reducedForm->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  RADAUIntegratorPropertyDialog::RADAUIntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");
    addTab("Extra");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);

    vector<QString> list;
    list.emplace_back("\"ODE\"");
    list.emplace_back("\"DAE1\"");
    list.emplace_back("\"DAE2\"");
    list.emplace_back("\"DAE3\"");
    list.emplace_back("\"GGL\"");
    formalism = new ExtWidget("Formalism",new TextChoiceWidget(list,0,true),true,false,MBSIM%"formalism");
    addToTab("General", formalism);

    reducedForm = new ExtWidget("Reduced form",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"reducedForm");
    addToTab("Extra", reducedForm);
  }

  DOMElement* RADAUIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    formalism->initializeUsingXML(item->getXMLElement());
    reducedForm->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* RADAUIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    formalism->writeXMLFile(item->getXMLElement());
    reducedForm->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  RODASIntegratorPropertyDialog::RODASIntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");
    addTab("Extra");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);

    vector<QString> list;
    list.emplace_back("\"ODE\"");
    list.emplace_back("\"DAE1\"");
    formalism = new ExtWidget("Formalism",new TextChoiceWidget(list,0,true),true,false,MBSIM%"formalism");
    addToTab("General", formalism);

    reducedForm = new ExtWidget("Reduced form",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"reducedForm");
    addToTab("Extra", reducedForm);

    autonomousSystem = new ExtWidget("Autonomous system",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"autonomousSystem");
    addToTab("Extra", autonomousSystem);
  }

  DOMElement* RODASIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    formalism->initializeUsingXML(item->getXMLElement());
    reducedForm->initializeUsingXML(item->getXMLElement());
    autonomousSystem->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* RODASIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    formalism->writeXMLFile(item->getXMLElement());
    reducedForm->writeXMLFile(item->getXMLElement());
    autonomousSystem->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  SEULEXIntegratorPropertyDialog::SEULEXIntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");
    addTab("Extra");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);

    vector<QString> list;
    list.emplace_back("\"ODE\"");
    list.emplace_back("\"DAE1\"");
    formalism = new ExtWidget("Formalism",new TextChoiceWidget(list,0,true),true,false,MBSIM%"formalism");
    addToTab("General", formalism);

    reducedForm = new ExtWidget("Reduced form",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"reducedForm");
    addToTab("Extra", reducedForm);

    autonomousSystem = new ExtWidget("Autonomous system",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"autonomousSystem");
    addToTab("Extra", autonomousSystem);
  }

  DOMElement* SEULEXIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    formalism->initializeUsingXML(item->getXMLElement());
    reducedForm->initializeUsingXML(item->getXMLElement());
    autonomousSystem->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* SEULEXIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    formalism->writeXMLFile(item->getXMLElement());
    reducedForm->writeXMLFile(item->getXMLElement());
    autonomousSystem->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  PHEM56IntegratorPropertyDialog::PHEM56IntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");
    addTab("Extra");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);

    vector<QString> list;
    list.emplace_back("\"DEC\"");
    list.emplace_back("\"DGETRF\"");
    linearAlgebra = new ExtWidget("Linear algebra",new TextChoiceWidget(list,1,true),true,false,MBSIM%"linearAlgebra");
    addToTab("Extra", linearAlgebra);

    generalVMatrix = new ExtWidget("General V matrix",new ChoiceWidget2(new BoolWidgetFactory("1"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"generalVMatrix");
    addToTab("Extra", generalVMatrix);

    initialProjection = new ExtWidget("Initial projection",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialProjection");
    addToTab("Extra", initialProjection);

    numberOfStepsBetweenProjections = new ExtWidget("Number of steps between projections",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"numberOfStepsBetweenProjections");
    addToTab("Extra", numberOfStepsBetweenProjections);

    projectOntoIndex1ConstraintManifold = new ExtWidget("Project onto index 1 constraint manifold",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"projectOntoIndex1ConstraintManifold");
    addToTab("Extra", projectOntoIndex1ConstraintManifold);
  }

  DOMElement* PHEM56IntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    linearAlgebra->initializeUsingXML(item->getXMLElement());
    generalVMatrix->initializeUsingXML(item->getXMLElement());
    initialProjection->initializeUsingXML(item->getXMLElement());
    numberOfStepsBetweenProjections->initializeUsingXML(item->getXMLElement());
    projectOntoIndex1ConstraintManifold->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* PHEM56IntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    linearAlgebra->writeXMLFile(item->getXMLElement());
    generalVMatrix->writeXMLFile(item->getXMLElement());
    initialProjection->writeXMLFile(item->getXMLElement());
    numberOfStepsBetweenProjections->writeXMLFile(item->getXMLElement());
    projectOntoIndex1ConstraintManifold->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  LSODEIntegratorPropertyDialog::LSODEIntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");

    vector<QString> list;
    list.emplace_back("\"nonstiff\"");
    list.emplace_back("\"Adams\"");
    list.emplace_back("\"stiff\"");
    list.emplace_back("\"BDF\"");
    method = new ExtWidget("Method",new TextChoiceWidget(list,1,true),true,false,MBSIM%"method");
    addToTab("General", method);

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    minimumStepSize = new ExtWidget("Minimum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"minimumStepSize");
    addToTab("Step size", minimumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);
  }

  DOMElement* LSODEIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    method->initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    minimumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* LSODEIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    method->writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    minimumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  LSODAIntegratorPropertyDialog::LSODAIntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    minimumStepSize = new ExtWidget("Minimum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"minimumStepSize");
    addToTab("Step size", minimumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);
  }

  DOMElement* LSODAIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    minimumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* LSODAIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    minimumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  LSODIIntegratorPropertyDialog::LSODIIntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    minimumStepSize = new ExtWidget("Minimum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"minimumStepSize");
    addToTab("Step size", minimumStepSize);

    maxSteps = new ExtWidget("Step limit",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepLimit");
    addToTab("Step size", maxSteps);

    vector<QString> list;
    list.emplace_back("\"ODE\"");
    list.emplace_back("\"DAE2\"");
    list.emplace_back("\"GGL\"");
    formalism = new ExtWidget("Formalism",new TextChoiceWidget(list,1,true),true,false,MBSIM%"formalism");
    addToTab("General", formalism);
  }

  DOMElement* LSODIIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    minimumStepSize->initializeUsingXML(item->getXMLElement());
    maxSteps->initializeUsingXML(item->getXMLElement());
    formalism->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* LSODIIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    minimumStepSize->writeXMLFile(item->getXMLElement());
    maxSteps->writeXMLFile(item->getXMLElement());
    formalism->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  DASPKIntegratorPropertyDialog::DASPKIntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    vector<QString> list;
    list.emplace_back("\"ODE\"");
    list.emplace_back("\"DAE1\"");
    list.emplace_back("\"DAE2\"");
    list.emplace_back("\"GGL\"");
    formalism = new ExtWidget("Formalism",new TextChoiceWidget(list,1,true),true,false,MBSIM%"formalism");
    addToTab("General", formalism);
  }

  DOMElement* DASPKIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    formalism->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* DASPKIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    formalism->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  TimeSteppingIntegratorPropertyDialog::TimeSteppingIntegratorPropertyDialog(Solver *solver) : IntegratorPropertyDialog(solver) {
    addTab("Step size");
    addTab("Tolerances");

    stepSize = new ExtWidget("Step size",new ChoiceWidget2(new ScalarWidgetFactory("1e-3",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepSize");
    addToTab("Step size", stepSize);

    gMax = new ExtWidget("Tolerance for position constraint",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"toleranceForPositionConstraints");
    addToTab("Tolerances", gMax);
  }

  DOMElement* TimeSteppingIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    IntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    stepSize->initializeUsingXML(item->getXMLElement());
    gMax->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* TimeSteppingIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    IntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    stepSize->writeXMLFile(item->getXMLElement());
    gMax->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  ThetaTimeSteppingIntegratorPropertyDialog::ThetaTimeSteppingIntegratorPropertyDialog(Solver *solver) : IntegratorPropertyDialog(solver) {
    addTab("Step size");
    addTab("Tolerances");

    stepSize = new ExtWidget("Step size",new ChoiceWidget2(new ScalarWidgetFactory("1e-3",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepSize");
    addToTab("Step size", stepSize);

    theta = new ExtWidget("Theta",new ChoiceWidget2(new ScalarWidgetFactory("0.5",vector<QStringList>(2,noUnitUnits()),vector<int>(2,0)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"theta");
    addToTab("General", theta);

    gMax = new ExtWidget("Tolerance for position constraint",new ChoiceWidget2(new ScalarWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"toleranceForPositionConstraints");
    addToTab("Tolerances", gMax);
  }

  DOMElement* ThetaTimeSteppingIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    IntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    stepSize->initializeUsingXML(item->getXMLElement());
    theta->initializeUsingXML(item->getXMLElement());
    gMax->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* ThetaTimeSteppingIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    IntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    stepSize->writeXMLFile(item->getXMLElement());
    theta->writeXMLFile(item->getXMLElement());
    gMax->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  TimeSteppingSSCIntegratorPropertyDialog::TimeSteppingSSCIntegratorPropertyDialog(Solver *solver) : IntegratorPropertyDialog(solver) {
    addTab("Step size");
    addTab("Tolerances");
    addTab("Extra");

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);

    minimumStepSize = new ExtWidget("Minimum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"minimumStepSize");
    addToTab("Step size", minimumStepSize);

    outputInterpolation = new ExtWidget("Output interpolation",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"outputInterpolation");
    addToTab("Extra", outputInterpolation);

    vector<QString> list;
    list.emplace_back("\"noGapControl\"");
    list.emplace_back("\"biggestRoot\"");
    list.emplace_back("\"scoring\"");
    list.emplace_back("\"smallestRoot\"");
    list.emplace_back("\"gapTolerance\"");
    gapControl = new ExtWidget("Gap control",new TextChoiceWidget(list,1,true),true,false,MBSIM%"gapControl");
    addToTab("Extra", gapControl);

    maximumOrder = new ExtWidget("Maximum order",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumOrder");
    addToTab("Extra", maximumOrder);

    list.clear();
    list.emplace_back("\"extrapolation\"");
    list.emplace_back("\"embedded\"");
    list.emplace_back("\"embeddedHigherOrder\"");
    method = new ExtWidget("Method",new TextChoiceWidget(list,0,true),true,false,MBSIM%"method");
    addToTab("General", method);

    list.clear();
    list.emplace_back("\"all\"");
    list.emplace_back("\"scale\"");
    list.emplace_back("\"exclude\"");
    errorTest = new ExtWidget("Error test",new TextChoiceWidget(list,0,true),true,false,MBSIM%"errorTest");
    addToTab("Extra", errorTest);

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("absoluteTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ToleranceWidgetFactory("relativeTolerance"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", relTol);

    stepSizeControl = new ExtWidget("Step size control",new ChoiceWidget2(new BoolWidgetFactory("1"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepSizeControl");
    addToTab("Step size", stepSizeControl);

    gapTolerance = new ExtWidget("Gap tolerance",new ChoiceWidget2(new ScalarWidgetFactory("1e-6",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"gapTolerance");
    addToTab("Tolerances", gapTolerance);

    maximumGain = new ExtWidget("Maximum gain",new ChoiceWidget2(new ScalarWidgetFactory("2.2",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumGain");
    addToTab("Extra", maximumGain);

    safetyFactor = new ExtWidget("Safety factor",new ChoiceWidget2(new ScalarWidgetFactory("0.7",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"safetyFactor");
    addToTab("Extra", safetyFactor);
  }

  DOMElement* TimeSteppingSSCIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    IntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    minimumStepSize->initializeUsingXML(item->getXMLElement());
    outputInterpolation->initializeUsingXML(item->getXMLElement());
    gapControl->initializeUsingXML(item->getXMLElement());
    maximumOrder->initializeUsingXML(item->getXMLElement());
    method->initializeUsingXML(item->getXMLElement());
    errorTest->initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    stepSizeControl->initializeUsingXML(item->getXMLElement());
    gapTolerance->initializeUsingXML(item->getXMLElement());
    maximumGain->initializeUsingXML(item->getXMLElement());
    safetyFactor->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* TimeSteppingSSCIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    IntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    minimumStepSize->writeXMLFile(item->getXMLElement());
    outputInterpolation->writeXMLFile(item->getXMLElement());
    gapControl->writeXMLFile(item->getXMLElement());
    maximumOrder->writeXMLFile(item->getXMLElement());
    method->writeXMLFile(item->getXMLElement());
    errorTest->writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    stepSizeControl->writeXMLFile(item->getXMLElement());
    gapTolerance->writeXMLFile(item->getXMLElement());
    maximumGain->writeXMLFile(item->getXMLElement());
    safetyFactor->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  HETS2IntegratorPropertyDialog::HETS2IntegratorPropertyDialog(Solver *solver) : IntegratorPropertyDialog(solver) {
    addTab("Step size");

    stepSize = new ExtWidget("Step size",new ChoiceWidget2(new ScalarWidgetFactory("1e-3",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepSize");
    addToTab("Step size", stepSize);
  }

  DOMElement* HETS2IntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    IntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    stepSize->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* HETS2IntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    IntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    stepSize->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  ExplicitEulerIntegratorPropertyDialog::ExplicitEulerIntegratorPropertyDialog(Solver *solver) : IntegratorPropertyDialog(solver) {
    addTab("Step size");

    stepSize = new ExtWidget("Step size",new ChoiceWidget2(new ScalarWidgetFactory("1e-3",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepSize");
    addToTab("Step size", stepSize);
  }

  DOMElement* ExplicitEulerIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    IntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    stepSize->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* ExplicitEulerIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    IntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    stepSize->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  ImplicitEulerIntegratorPropertyDialog::ImplicitEulerIntegratorPropertyDialog(Solver *solver) : IntegratorPropertyDialog(solver) {
    addTab("Step size");
    addTab("Extra");

    stepSize = new ExtWidget("Step size",new ChoiceWidget2(new ScalarWidgetFactory("1e-3",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepSize");
    addToTab("Step size", stepSize);

    reducedForm = new ExtWidget("Reduced form",new ChoiceWidget2(new BoolWidgetFactory("0"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"reducedForm");
    addToTab("Extra", reducedForm);
  }

  DOMElement* ImplicitEulerIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    IntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    stepSize->initializeUsingXML(item->getXMLElement());
    reducedForm->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* ImplicitEulerIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    IntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    stepSize->writeXMLFile(item->getXMLElement());
    reducedForm->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  RKSuiteIntegratorPropertyDialog::RKSuiteIntegratorPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");

    vector<QString> list;
    list.emplace_back("\"RK23\"");
    list.emplace_back("\"RK45\"");
    list.emplace_back("\"RK78\"");
    method = new ExtWidget("Method",new TextChoiceWidget(list,1,true),true,false,MBSIM%"method");
    addToTab("General", method);

    list.clear();
    list.emplace_back("\"usual\"");
    list.emplace_back("\"complex\"");
    task = new ExtWidget("Task",new TextChoiceWidget(list,1,true),true,false,MBSIM%"task");
    addToTab("General", task);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ScalarWidgetFactory("1e-6"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"relativeToleranceScalar");
    addToTab("Tolerances", relTol);

    threshold = new ExtWidget("Threshold",new ChoiceWidget2(new ToleranceWidgetFactory("threshold"),QBoxLayout::RightToLeft,3),true,false);
    addToTab("Tolerances", threshold);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);
  }

  DOMElement* RKSuiteIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    method->initializeUsingXML(item->getXMLElement());
    task->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    threshold->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* RKSuiteIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    method->writeXMLFile(item->getXMLElement());
    task->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    threshold->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  BoostOdeintDOSPropertyDialog::BoostOdeintDOSPropertyDialog(Solver *solver) : RootFindingIntegratorPropertyDialog(solver) {
    addTab("Step size");

    absTol = new ExtWidget("Absolute tolerance",new ChoiceWidget2(new ScalarWidgetFactory("1e-6",vector<QStringList>(2),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"absoluteToleranceScalar");
    addToTab("Tolerances", absTol);

    relTol = new ExtWidget("Relative tolerance",new ChoiceWidget2(new ScalarWidgetFactory("1e-6",vector<QStringList>(2),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"relativeToleranceScalar");
    addToTab("Tolerances", relTol);

    initialStepSize = new ExtWidget("Initial step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialStepSize");
    addToTab("Step size", initialStepSize);

    maximumStepSize = new ExtWidget("Maximum step size",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"maximumStepSize");
    addToTab("Step size", maximumStepSize);
  }

  DOMElement* BoostOdeintDOSPropertyDialog::initializeUsingXML(DOMElement *parent) {
    RootFindingIntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    absTol->initializeUsingXML(item->getXMLElement());
    relTol->initializeUsingXML(item->getXMLElement());
    initialStepSize->initializeUsingXML(item->getXMLElement());
    maximumStepSize->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* BoostOdeintDOSPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    RootFindingIntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    absTol->writeXMLFile(item->getXMLElement());
    relTol->writeXMLFile(item->getXMLElement());
    initialStepSize->writeXMLFile(item->getXMLElement());
    maximumStepSize->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  QuasiStaticIntegratorPropertyDialog::QuasiStaticIntegratorPropertyDialog(Solver *solver) : IntegratorPropertyDialog(solver) {
    addTab("Step size");

    stepSize = new ExtWidget("Step size",new ChoiceWidget2(new ScalarWidgetFactory("1e-3",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"stepSize");
    addToTab("Step size", stepSize);
  }

  DOMElement* QuasiStaticIntegratorPropertyDialog::initializeUsingXML(DOMElement *parent) {
    IntegratorPropertyDialog::initializeUsingXML(item->getXMLElement());
    stepSize->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* QuasiStaticIntegratorPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    IntegratorPropertyDialog::writeXMLFile(item->getXMLElement());
    stepSize->writeXMLFile(item->getXMLElement());
    return nullptr;
  }


  EigenanalyzerPropertyDialog::EigenanalyzerPropertyDialog(Solver *solver) : SolverPropertyDialog(solver) {
    addTab("General");
    addTab("Initial conditions");

    startTime = new ExtWidget("Start time",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"startTime");
    addToTab("General", startTime);

    endTime = new ExtWidget("End time",new ChoiceWidget2(new ScalarWidgetFactory("1",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"endTime");
    addToTab("General", endTime);

    plotStepSize = new ExtWidget("Plot step size",new ChoiceWidget2(new ScalarWidgetFactory("1e-2",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"plotStepSize");
    addToTab("General", plotStepSize);

    initialState = new ExtWidget("Initial state",new ChoiceWidget2(new VecSizeVarWidgetFactory(1),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialState");
    addToTab("Initial conditions", initialState);

    vector<QString> list;
    list.emplace_back("\"eigenmodes\"");
    list.emplace_back("\"eigenmotion\"");
    task = new ExtWidget("Task",new TextChoiceWidget(list,1,true),true,false,MBSIM%"task");
    addToTab("General",task);

    initialDeviation = new ExtWidget("Initial deviation",new ChoiceWidget2(new VecSizeVarWidgetFactory(1),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialDeviation");
    addToTab("General",initialDeviation);

    amplitude = new ExtWidget("Amplitude",new ChoiceWidget2(new ScalarWidgetFactory("1"),QBoxLayout::RightToLeft,5),true,false,MBSIM%"amplitude");
    addToTab("General",amplitude);

    modeAmplitudeTable = new ExtWidget("Mode amplitude table",new ChoiceWidget2(new MatRowsVarWidgetFactory(1,2),QBoxLayout::RightToLeft,5),true,false,MBSIM%"modeAmplitudeTable");
    addToTab("General",modeAmplitudeTable);

    loops = new ExtWidget("Loops",new SpinBoxWidget(5),true,false,MBSIM%"loops");
    addToTab("General",loops);
  }

  DOMElement* EigenanalyzerPropertyDialog::initializeUsingXML(DOMElement *parent) {
    SolverPropertyDialog::initializeUsingXML(item->getXMLElement());
    startTime->initializeUsingXML(item->getXMLElement());
    endTime->initializeUsingXML(item->getXMLElement());
    plotStepSize->initializeUsingXML(item->getXMLElement());
    initialState->initializeUsingXML(item->getXMLElement());
    task->initializeUsingXML(item->getXMLElement());
    initialDeviation->initializeUsingXML(item->getXMLElement());
    amplitude->initializeUsingXML(item->getXMLElement());
    modeAmplitudeTable->initializeUsingXML(item->getXMLElement());
    loops->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* EigenanalyzerPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    SolverPropertyDialog::writeXMLFile(item->getXMLElement());
    startTime->writeXMLFile(item->getXMLElement());
    endTime->writeXMLFile(item->getXMLElement());
    plotStepSize->writeXMLFile(item->getXMLElement());
    initialState->writeXMLFile(item->getXMLElement());
    task->writeXMLFile(item->getXMLElement());
    initialDeviation->writeXMLFile(item->getXMLElement());
    amplitude->writeXMLFile(item->getXMLElement());
    modeAmplitudeTable->writeXMLFile(item->getXMLElement());
    loops->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

  HarmonicResponseAnalyzerPropertyDialog::HarmonicResponseAnalyzerPropertyDialog(Solver *solver) : SolverPropertyDialog(solver) {
    addTab("General");
    addTab("Initial conditions");

    startTime = new ExtWidget("Start time",new ChoiceWidget2(new ScalarWidgetFactory("0",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"startTime");
    addToTab("General", startTime);

    excitationFrequencies = new ExtWidget("Excitation frequencies",new ChoiceWidget2(new VecSizeVarWidgetFactory(1),QBoxLayout::RightToLeft,5),true,false,MBSIM%"excitationFrequencies");
    addToTab("General", excitationFrequencies);

    systemFrequencies = new ExtWidget("System frequencies",new ChoiceWidget2(new VecSizeVarWidgetFactory(1),QBoxLayout::RightToLeft,5),true,false,MBSIM%"systemFrequencies");
    addToTab("General", systemFrequencies);

    plotStepSize = new ExtWidget("Plot step size",new ChoiceWidget2(new ScalarWidgetFactory("1e-2",vector<QStringList>(2,timeUnits()),vector<int>(2,2)),QBoxLayout::RightToLeft,5),true,false,MBSIM%"plotStepSize");
    addToTab("General", plotStepSize);

    initialState = new ExtWidget("Initial state",new ChoiceWidget2(new VecSizeVarWidgetFactory(1),QBoxLayout::RightToLeft,5),true,false,MBSIM%"initialState");
    addToTab("Initial conditions", initialState);

    vector<QString> list;
    list.emplace_back("\"frequencyResponse\"");
    task = new ExtWidget("Task",new TextChoiceWidget(list,1,true),true,false,MBSIM%"task");
    addToTab("General",task);
  }

  DOMElement* HarmonicResponseAnalyzerPropertyDialog::initializeUsingXML(DOMElement *parent) {
    SolverPropertyDialog::initializeUsingXML(item->getXMLElement());
    startTime->initializeUsingXML(item->getXMLElement());
    excitationFrequencies->initializeUsingXML(item->getXMLElement());
    systemFrequencies->initializeUsingXML(item->getXMLElement());
    plotStepSize->initializeUsingXML(item->getXMLElement());
    initialState->initializeUsingXML(item->getXMLElement());
    task->initializeUsingXML(item->getXMLElement());
    return parent;
  }

  DOMElement* HarmonicResponseAnalyzerPropertyDialog::writeXMLFile(DOMNode *parent, DOMNode *ref) {
    SolverPropertyDialog::writeXMLFile(item->getXMLElement());
    startTime->writeXMLFile(item->getXMLElement());
    excitationFrequencies->writeXMLFile(item->getXMLElement());
    systemFrequencies->writeXMLFile(item->getXMLElement());
    plotStepSize->writeXMLFile(item->getXMLElement());
    initialState->writeXMLFile(item->getXMLElement());
    task->writeXMLFile(item->getXMLElement());
    return nullptr;
  }

}
