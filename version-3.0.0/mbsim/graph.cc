/* Copyright (C) 2004-2009 MBSim Development Team
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
 * Contact: mfoerg@users.berlios.de
 */

#include <config.h>
#include "mbsim/graph.h"
#include "mbsim/object.h"
#include "mbsim/frame.h"
#include "mbsim/extra_dynamic.h"

using namespace fmatvec; 
using namespace std;

namespace MBSim {

  Graph::Graph(const string &projectName) : DynamicSystem(projectName) {
    calcuSize_[0] = &Graph::calcuSize0;
    calcuSize_[1] = &Graph::calcuSize1;
    sethSize_[0] = &Graph::sethSize0;
    sethSize_[1] = &Graph::sethSize1;
  }

  Graph::~Graph() {}

  void Graph::updateStateDependentVariables(double t) {
    for(unsigned int i=0; i<obj.size(); i++) 
      for(unsigned int j=0; j<obj[i].size(); j++) 
	obj[i][j]->updateStateDependentVariables(t);
  }

  void Graph::updateJacobians(double t, int k) {
    for(unsigned int i=0; i<obj.size(); i++) 
      for(unsigned int j=0; j<obj[i].size(); j++) 
	obj[i][j]->updateJacobians(t,k);

  }

  void Graph::updatedu(double t, double dt) {
    ud[0] = slvLLFac(LLM[0], h[0]*dt+r[0]);
  }

  void Graph::updateud(double t, int j) {
    ud[j] =  slvLLFac(LLM[j], h[j]+r[j]);
  }

  void Graph::updatezd(double t) {
    qd = T*u;
    ud[0] = slvLLFac(LLM[0], h[0]+r[0]);

    for(vector<DynamicSystem*>::iterator i = dynamicsystem.begin(); i != dynamicsystem.end(); ++i) 
      (*i)->updatexd(t);

    for(vector<ExtraDynamic*>::iterator i = extraDynamic.begin(); i!= extraDynamic.end(); ++i) 
      (**i).updatexd(t);
  }

  void Graph::sethSize0(int hSize_) {
    hSize[0] = hSize_;
    for(vector<Object*>::iterator i = object.begin(); i != object.end(); ++i) {
      (*i)->sethSize((*i)->getuSize(0)+(*i)->getuInd(0),0);
      (*i)->sethInd(0,0);
    }
  } 

  void Graph::sethSize1(int hSize_) {
    DynamicSystem::sethSize(hSize_,1);
  } 

  void Graph::calcqSize() {
    qSize = 0;
    for(unsigned int i=0; i<obj.size(); i++) 
      for(unsigned int j=0; j<obj[i].size(); j++) {
	obj[i][j]->calcqSize();
	obj[i][j]->setqInd(qSize);
	qSize += obj[i][j]->getqSize();
      }
  }

  void Graph::calcuSize0() {
    uSize[0] = 0;
    for(unsigned int i=0; i<obj.size(); i++) 
      for(unsigned int j=0; j<obj[i].size(); j++) {
	obj[i][j]->calcuSize(0);
	obj[i][j]->setuInd(uSize[0],0);
	uSize[0] += obj[i][j]->getuSize(0);
      }
  }

  void Graph::calcuSize1() {
    DynamicSystem::calcuSize(1);
  }

  void Graph::facLLM(int i) {
    LLM[i] = facLL(M[i]); 
  }

  void Graph::addObject(int level, Object* object) {
    DynamicSystem::addObject(object);

    for(int i=obj.size(); i<=level; i++) {
      vector<Object*> vec;
      obj.push_back(vec);
    }

    obj[level].push_back(object);
  }

}

