/* Copyright (C) 2004-2006  Martin Förg
 
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
 * Contact:
 *   mfoerg@users.berlios.de
 *
 */
#include <config.h>
#include "body_rigid_rel.h"
#include "port.h"
#include "contour.h"
#include "link.h"
#include "tree.h"
#include "multi_body_system.h"
#include "crigidbody.h"

namespace MBSim {

  BodyRigidRel::BodyRigidRel(const string &name) : BodyRigid(name), APK(3), APK0(3), lSize(6), lInd(0), tree(0), PrPK(3), PrPK0(3), KrOK(3), KvK(3), e(6), precessor(0), C(6) {

    APK0(0,0)=1.0;
    APK0(1,1)=1.0;
    APK0(2,2)=1.0;
  }

  void BodyRigidRel::calcSize() {
    BodyRigid::calcSize();

    setqInd(tree->getqSize());
    setuInd(tree->getuSize());
    setxInd(tree->getxSize());
    setlInd(tree->getlSize());
    tree->setqSize(tree->getqSize()+qSize);
    tree->setuSize(tree->getuSize()+uSize);
    tree->setxSize(tree->getxSize()+xSize);
    tree->setlSize(tree->getlSize()+lSize);
    for(int i=0; i<successor.size(); i++) {
      successor[i]->calcSize();
    }
  }

  void BodyRigidRel::setMbs(MultiBodySystem* mbs_) {
    Element::setMbs(mbs_);
    for(int i=0; i<successor.size(); i++) {
      successor[i]->setMbs(mbs);
    }
  }

  void BodyRigidRel::setTree(Tree* tree_) {
    tree = tree_;
    for(int i=0; i<successor.size(); i++) {
      successor[i]->setTree(tree);
    }
  }

  void BodyRigidRel::updateqRef() {
    q>>(tree->getq()(qInd,qInd+qSize-1));
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateqRef();
    }
  }

  void BodyRigidRel::updatezdRef() {
    qd>>(tree->getqd()(qInd,qInd+qSize-1));
    ud>>(tree->getud()(uInd,uInd+uSize-1));
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updatezdRef();
    }
  }

  void BodyRigidRel::updateqdRef() {
    qd>>(tree->getqd()(qInd,qInd+qSize-1));
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateqdRef();
    }
  }

  void BodyRigidRel::updateuRef() {
    u>>(tree->getu()(uInd,uInd+uSize-1));
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateuRef();
    }
  }

  void BodyRigidRel::updatehRef() {
    h>>(tree->geth()(uInd,uInd+uSize-1));
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updatehRef();
    }
  }

  void BodyRigidRel::updaterRef() {
    r>>(tree->getr()(uInd,uInd+uSize-1));
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updaterRef();
    }
  }

  void BodyRigidRel::updateMhRef() {
    Index I = getlIndex();
    Mh>>tree->getMh()(I);
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateMhRef();
    }
  }

  void BodyRigidRel::updatelRef() {
    l>>(tree->getl()(Il));
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updatelRef();
    }
  }

  void BodyRigidRel::updateJRef() {
    J>>(tree->getJ()(Il,Index(0,uInd+uSize-1)));
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateJRef();
    }
  }

  void BodyRigidRel::updateTRef() {
    Index Iu = getuIndex();
    Index Iq = Index(qInd,qInd+qSize-1);
    T>>(tree->getT()(Iq,Iu));
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateTRef();
    }
  }

  void BodyRigidRel::initStage1() {
    BodyRigid::init();

    if(JT.rows()==0)
      JT.resize(3,0);
    if(JR.rows()==0)
      JR.resize(3,0);

    IuT = Index(uInd+iT.start(),uInd+iT.end());
    IuR = Index(uInd+iR.start(),uInd+iR.end());

    Mh(0,0) = m;
    Mh(1,1) = m;
    Mh(2,2) = m;

    Mat tKrKS = tilde(KrKS);
    I += m*JTJ(tKrKS);
    Mh(Index(3,5)) = I;
    Mh(Index(0,2),Index(3,5)) = m*trans(tKrKS);


    vector<LinkPortData>::iterator it1=linkSetValuedPortData.begin(); 
    for(int i=0; i<linkSetValuedPortData.size(); i++) {
      int portID = it1->ID;
      int objectID = it1->objectID;
      bool addLink = true;
      for(int j=0; j<tree->linkSetValuedPortData.size(); j++)
	if(tree->linkSetValuedPortData[j].link == it1->link)
	  addLink = false;
      if(addLink)
	tree->addLink(it1->link,port[portID],objectID);
      it1++;
    }

    vector<LinkContourData>::iterator it2=linkSetValuedContourData.begin(); 
    for(int i=0; i<linkSetValuedContourData.size(); i++) {
      int contourID = it2->ID;
      int objectID = it2->objectID;
      bool addLink = true;
      for(int j=0; j<tree->linkSetValuedContourData.size(); j++)
	if(tree->linkSetValuedContourData[j].link == it2->link)
	  addLink = false;
      if(addLink) 
	tree->addLink(it2->link,contour[contourID],objectID);

      it2++;
    }

    for(int i=0; i<successor.size(); i++) {
      successor[i]->initStage1();
    }
    iI = Index(0,uInd+uSize-1);
  }

  void BodyRigidRel::initStage2() {
    int k=0;
    for(int i=0; i<linkSetValued.size(); i++) {
      int linkID = -1;
      for(int j=0; j<tree->linkSetValued.size(); j++)
	if(tree->linkSetValued[j] == linkSetValued[i])
	  linkID = j;
      Index iJ(0,linkSetValued[i]->getlaSize()-1);
      if(linkID > -1) {
	W[i].resize() >> tree->W[linkID](iI,iJ);
      } else {
	W[i].resize() >> tree->W[k](iI,iJ);
	k++;
      }
    }
    for(int i=0; i<successor.size(); i++) {
      successor[i]->initStage2();
    }
  }


  void BodyRigidRel::initz() {
    Body::initz();
    for(int i=0; i<successor.size(); i++) {
      successor[i]->initz();
    }
  }

  void BodyRigidRel::initPlotFiles() {

    BodyRigid::initPlotFiles();

    for(int i=0; i<successor.size(); i++) {
      successor[i]->initPlotFiles();
    }
  }

  void BodyRigidRel::plot(double t, double dt) {

    BodyRigid::plot(t); 

    for(int i=0; i<successor.size(); i++) {
      successor[i]->plot(t,dt);
    }
  }

  void BodyRigidRel::updateFullName() {
    Element::setFullName(tree->getFullName()+"."+getName());
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateFullName();
    }
  } 

  void BodyRigidRel::addChild(BodyRigidRel *body) {

    successor.push_back(body);
    body->setMbs(mbs);
    body->setTree(tree);
    body->updateFullName();
    body->setPrecessor(this);
  }

  void BodyRigidRel::setPrecessor(BodyRigidRel *precessor_) {
    precessor = precessor_;
  }

  void BodyRigidRel::updateh(double t) {
    sumUpForceElements(t);

    Vec KF = trans(AWK)*WF;
    Vec KM = trans(AWK)*WM;
    l(0,2) = KF - m*crossProduct(KomegaK,crossProduct(KomegaK,KrKS));
    l(3,5) = KM + crossProduct(I*KomegaK,KomegaK);

    if(precessor) {
      Vec f(6,NONINIT);
      f(0,2) = trans(APK)*(crossProduct(precessor->getKomegaK(), 2*(JT*u(iT))+crossProduct(precessor->getKomegaK(),PrPK)));
      f(3,5) = crossProduct(KomegaK,JR*u(iR));

      C(Index(0,2),Index(0,2)) = trans(APK);
      C(Index(3,5),Index(3,5)) = trans(APK);
      C(Index(0,2),Index(3,5)) = -trans(APK)*tilde(PrPK);

      e = C*precessor->gete()+f;


      J(Index(0,2),IuT) = trans(APK)*JT;
      J(Index(3,5),IuR) = JR;
      BodyRigidRel* nextBody = precessor;
      while(nextBody) {
	J(Index(0,5),Index(nextBody->getIuT().start(),nextBody->getIuR().end())) = C*precessor->getJ()(Index(0,5),Index(nextBody->getIuT().start(),nextBody->getIuR().end()));
	nextBody = nextBody->getPrecessor();
      }

      l -= Mh*e;

      Index I(0,uInd+uSize-1);
      tree->geth()(I) += trans(J)*l;
      tree->getM()(I) += JTMJ(Mh,J);
    } else {

      J(Index(0,2),IuT) = trans(APK)*JT;
      J(Index(3,5),IuR) = JR;

      Index I(0,uInd+uSize-1);
      tree->getM().init(0);
      tree->geth().init(0);
      tree->geth()(I) = trans(J)*l;
      tree->getM()(I) = JTMJ(Mh,J);
    }

    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateh(t);
    }
  }

  void BodyRigidRel::updateCenterOfGravity(double t) {

    (this->*updateAK0K)();

    PrPK = JT*q(iT) + PrPK0;
    APK = APK0*AK0K;
    if(precessor) {
      AWK = precessor->getAWK()*APK;
      KomegaK = trans(APK)*precessor->getKomegaK() + JR*u(iR);
      WomegaK = AWK * KomegaK;
      KrOK = trans(APK)*(PrPK + precessor->getKrOK());
      KvK = trans(APK)*(precessor->getKvK() + JT*u(iT) + crossProduct(precessor->getKomegaK(),PrPK));
    } else {
      AWK = APK;
      KomegaK = JR*u(iR);
      WomegaK = AWK * KomegaK;
      KrOK = trans(APK)*(PrPK);
      KvK = trans(APK)*JT*u(iT);
    }

    WrOK = AWK * KrOK;
    WvK = AWK * KvK;
  }

  void BodyRigidRel::updateKinematics(double t) {

    BodyRigid::updateKinematics(t);

    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateKinematics(t);
    }
  }

  void BodyRigidRel::updateW(double t) {
    Index IF(0,2);
    Index IM(3,5);

    vector<LinkPortData>::iterator it1=linkSetValuedPortData.begin(); 
    vector<LinkContourData>::iterator it2=linkSetValuedContourData.begin(); 
    vector<Mat>::iterator itW=W.begin(); 
    for(int i=0; i<linkSetValuedPortData.size(); i++) {
      int portID = it1->ID;
      int objectID = it1->objectID;
      Mat ld = it1->link->getLoadDirections(objectID);
      Index iJ(0,ld.cols()-1);
      Mat Kl(6,ld.cols(),NONINIT);
      Kl(IF,iJ) = trans(AWK)*ld(IF,iJ);
      Kl(IM,iJ) = trans(AWK)*ld(IM,iJ) + tilde(KrKP[portID])*Kl(IF,iJ) ;
      (*itW) += trans(J)*Kl;
      //(*itW)(iI,iJ) += trans(J)*Kl;
      //    (*itw) = trans(fd)*crossProduct(WomegaK,crossProduct(WomegaK,WrSP[portID]));
      it1++; itW++; 
    }

    for(int i=0; i<linkSetValuedContourData.size(); i++) {
      if(it2->link->isActive()) {
	int objectID = it2->objectID;
	Mat ld = it2->link->getLoadDirections(objectID);
	Index iJ(0,ld.cols()-1);
	Mat Kl(6,ld.cols(),NONINIT);
	Vec KrKC = trans(AWK)*(it2->link->getWrOC(objectID)-WrOK);
	Kl(IF,iJ) = trans(AWK)*ld(IF,iJ);
	Kl(IM,iJ) = trans(AWK)*ld(IM,iJ) + tilde(KrKC)*Kl(IF,iJ) ;
	(*itW) += trans(J)*Kl;
	//(*itW)(iI,iJ) += trans(J)*Kl;
      }
      it2++; itW++; 
    }
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateW(t);
    }
  }

  void BodyRigidRel::updater(double t) {
    for(vector<LinkPortData>::iterator i=linkSetValuedPortData.begin(); i!=linkSetValuedPortData.end(); i++) {
      Index I = i->link->getlaIndex();
      tree->getr() += mbs->getW()(tree->getuIndex(),I)*i->link->getla();
    }
    for(vector<LinkContourData>::iterator i=linkSetValuedContourData.begin(); i!=linkSetValuedContourData.end(); i++) {
      if(i->link->isActive()) {
	Index I = i->link->getlaIndex();
	tree->getr() += mbs->getW()(tree->getuIndex(),I)*i->link->getla();
      }
    }
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updater(t);
    }
  }

  double BodyRigidRel::computeKineticEnergy() {
    return 0.5 * (m*trans(KvK)*(KvK + 2*crossProduct(KomegaK,KrKS)) + trans(KomegaK)*I*KomegaK);
  }

  void BodyRigidRel::updatedq(double t, double dt) {

    BodyRigid::updatedq(t,dt);

    for(int i=0; i<successor.size(); i++) {
      successor[i]->updatedq(t,dt);
    }
  }

  void BodyRigidRel::updateqd(double t) {
    (this->*updateT)();
    qd = T*u;
    for(int i=0; i<successor.size(); i++) {
      successor[i]->updateqd(t);
    }
  }

  Port* BodyRigidRel::getPort(const string &pName) {
  cout << "Port* BodyRigidRel::getPort(const string &name): name = " << name << endl;
     // Auf sich selber suchen
    for(int i=0;i<port.size();i++) {
      cout << "port[" << i << "]->name = " << port[i]->getName() << endl;
      if(port[i]->getName() == pName)
        return port[i];
}
    for(int i=0;i<successor.size();i++) {
      Port* p = successor[i]->getPort(pName);
      if(p) return p;
    }
  }

}
