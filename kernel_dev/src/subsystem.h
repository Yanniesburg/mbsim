/* Copyright (C) 2004-2008  Martin Förg
 
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

#ifndef _SUBSYSTEM_H_
#define _SUBSYSTEM_H_

#include "element.h"
#include "interfaces.h"

namespace H5 {
  class Group;
}

namespace MBSim {
  class Frame;
  class Contour;
  class ExtraDynamicInterface;
  class DataInterfaceBase;
  class Object;
  class Link;

  // Compatibility classes
  class TreeRigid;
  class BodyRigid;

  class Subsystem : public Element, public ObjectInterface, public LinkInterface {

    friend class HitSphereLink;

    protected:
      Subsystem *parent;

      vector<Object*> object;
      vector<Link*> link;
      vector<ExtraDynamicInterface*> EDI;
      vector<DataInterfaceBase*> DIB;
      vector<Subsystem*> subsystem;
      vector<Link*> linkSingleValued;
      vector<Link*> linkSetValued;
      vector<Link*> linkSetValuedActive;

      vector<Link*> specialLink;

      vector<Vec> IrOK, IrOC, IrOS;
      vector<SqrMat> AIK, AIC, AIS;

      int qSize, qInd;
      int uSize[2], uInd[2];
      int xSize, xInd;
      int hSize[2], hInd[2];
      int gSize, gInd;
      int gdSize, gdInd;
      int laSize, laInd;
      int rFactorSize, rFactorInd;
      int svSize, svInd;

      /** linear relation matrix \f$\boldsymbol{T}\f$ of position and velocity parameters */
      Mat T;
      /** mass matrix \f$\boldsymbol{M}\f$*/
      SymMat M;
      /** LU-decomposition of mass matrix \f$\boldsymbol{M}\f$*/
      SymMat LLM;
      Vec q, qd, q0;
      Vec u, ud, u0;
      Vec x, xd, x0;
      Vec h, r, f;
      Mat W, V;
      Vec la;
      Vec dla;
      Vec g, gd;
      Vec wb;
      Vec res;
      Vec rFactor;

      Vec sv;
      Vector<int> jsv;

      vector<Frame*> port;
      vector<Contour*> contour;

      void addSubsystem(Subsystem *subsystem);
      void addObject(Object *object);

   public:
      /*! Constructor */
      Subsystem(const string &name);
      /*! Destructor */
      ~Subsystem();

      virtual void init();
      virtual void preinit();
      void initz();
      void calcqSize();
      void calcuSize(int j=0);
      void calcxSize();
      void calcsvSize();
      //void calchSize();
      void calclaSize();
      void calclaSizeForActiveg();
      void calcgSize();
      void calcgSizeActive();
      void calcgdSize();
      void calcgdSizeActive();
      void calcrFactorSize();
      void checkActiveLinks();
      void checkActiveg();
      void checkActivegd();
      void checkActivegdn();
      void checkActivegdd();
      void checkAllgd();

      void updateCondition();

      Subsystem* getParent() {return parent;}
      void setParent(Subsystem* sys) {parent = sys;}

      const Mat& getW() const {return W;}
      Mat& getW() {return W;}
      const Mat& getV() const {return V;}
      Mat& getV() {return V;}

      Vec& getsv() {return sv;}
      const Vec& getsv() const {return sv;}
      Vector<int>& getjsv() {return jsv;}
      const Vector<int>& getjsv() const {return jsv;}
      const Vec& getla() const {return la;}
      Vec& getla() {return la;}
      const Vec& getres() const {return res;}
      Vec& getres() {return res;}
      const Vec& getg() const {return g;}
      Vec& getg() {return g;}
      const Vec& getgd() const {return gd;}
      Vec& getgd() {return gd;}
      const Vec& getrFactor() const {return rFactor;}
      Vec& getrFactor() {return rFactor;}

      void setrFactorInd(int ind) {rFactorInd = ind;}
      void setlaInd(int ind) {laInd = ind;}
      void setgInd(int ind) {gInd = ind;}
      void setgdInd(int ind) {gdInd = ind;}

      void setsvInd(int svInd_) {svInd = svInd_;};
      int getgSize() const {return gSize;} 
      int getgdSize() const {return gdSize;} 
      int getlaSize() const {return laSize;} 
      int getlaInd() const {return laInd;} 
      int getrFactorSize() const {return rFactorSize;} 
      int getsvSize() const {return svSize;}

      int getqSize() const {return qSize;}
      int getuSize(int i=0) const {return uSize[i];}
      int getxSize() const {return xSize;}
      int getzSize() const { return qSize + uSize[0] + xSize; }
      int gethSize(int i=0) const { return hSize[i]; }

      /*! Get mass matrix */
      const SymMat& getM() const {return M;};
      /*! Get T-matrix */
      const Mat& getT() const {return T;};
      /*! Get Cholesky decomposition of the mass matrix */
      const SymMat& getLLM() const {return LLM;};
      const Vec& geth() const {return h;};

      const Vec& getq() const {return q;};
      const Vec& getu() const {return u;};

      void updateT(double t); 
      void updateh(double t); 
      void updateM(double t); 
      void updatedq(double t, double dt); 
      void updatedu(double t, double dt) = 0;
      void updateqd(double t) {throw 5;}
      void updateud(double t) {throw 5;}
      void updateJacobians(double t) = 0; 
      void updateSecondJacobians(double t) = 0; 

      void resizeJacobians(int j); 
      void checkForConstraints();

      void updater(double t); 
      void updatewb(double t); 
      void updateW(double t); 
      void updateV(double t); 
      //void updateh(double t); see above
      void updateg(double t);
      void updategd(double t);
      void updatedx(double t, double dt); 
      void updatexd(double t);
      void updateStopVector(double t); 

      virtual void facLLM() = 0;

      void updateqRef(const Vec &ref); 
      void updateqdRef(const Vec &ref); 
      void updateuRef(const Vec &ref); 
      void updateudRef(const Vec &ref); 
      void updatexRef(const Vec &ref); 
      void updatexdRef(const Vec &ref); 
      //void updatezRef(const Vec &ref); 
      //void updatezdRef(const Vec &ref); 
      void updatehRef(const Vec &ref, int i=0);
      void updatefRef(const Vec &ref);
      void updaterRef(const Vec &ref);
      void updateTRef(const Mat &ref);
      void updateMRef(const SymMat &ref, int i=0);
      void updateLLMRef(const SymMat &ref, int i=0);
      void updatesvRef(const Vec& ref);
      void updatejsvRef(const Vector<int> &ref);
      void updategRef(const Vec &ref);
      void updategdRef(const Vec &ref);
      void updateWRef(const Mat &ref, int i=0);
      void updateVRef(const Mat &ref, int i=0);
      void updatelaRef(const Vec &ref);
      void updatewbRef(const Vec &ref);
      void updateresRef(const Vec &ref);
      void updaterFactorRef(const Vec &ref);

      /*! set scale factor for Flow Quantity Tolerances tolQ=tol*scaleTolQ */
      void setScaleTolQ(double scaleTolQ);
      /*! set scale factor for Pressure Quantity Tolerances tolp=tol*scaleTolp */
      void setScaleTolp(double scaleTolp);
      void setgdTol(double tol);
      void setgddTol(double tol);
      void setlaTol(double tol);
      void setLaTol(double tol);
      void setrMax(double rMax);

      void addFrame(Frame * port);
      void addContour(Contour* contour);

      int portIndex(const Frame *port_) const;

      virtual Frame* getFrame(const string &name, bool check=true);
      virtual Contour* getContour(const string &name, bool check=true);

      void addFrame(Frame *port_, const Vec &RrRK, const SqrMat &ARK, const Frame* refFrame=0); 

      void addFrame(const string &str, const Vec &SrSK, const SqrMat &ASK, const Frame* refFrame=0);

      void addContour(Contour* contour, const Vec &RrRC, const SqrMat &ARC, const Frame* refFrame=0);
      void addContour(Contour* contour, const Vec &RrRC, const Frame* refFrame=0) {addContour(contour,RrRC,SqrMat(3,EYE));}

      void addLink(Link *link);
      
      Subsystem* getSubsystem(const string &name,bool check=true);
      Object* getObject(const string &name,bool check=true);
      Link* getLink(const string &name,bool check=true);
      /*! Returns an extra dynamic interface */
      ExtraDynamicInterface* getEDI(const string &name,bool check=true);
      DataInterfaceBase* getDataInterfaceBase(const string &name, bool check=true);
      /*! Adds an \param object to multibody system */
      void addEDI(ExtraDynamicInterface *edi_);
      /* Add a data_interface_base \param dib_ to the DataInterfaceBase-vector */
      void addDataInterfaceBase(DataInterfaceBase* dib_);

      void setUpLinks();

      const Vec& getx() const {return x;};
      Vec& getx() {return x;};
      void setx(const Vec& x_) { x = x_; }
      void setx0(const Vec &x0_) { x0 = x0_; }
      void setx0(double x0_) { x0 = Vec(1,INIT,x0_); }
      Vec& getxd() {return xd;};
      Vec& getx0() {return x0;};
      const Vec& getx0() const {return x0;};
      const Vec& getxd() const {return xd;};

      int  getxInd() { return xInd; }
      const Vec& getf() const {return f;};
      Vec& getf() {return f;};


      void setqSize(int qSize_) { qSize = qSize_; }
      void setqInd(int qInd_) { qInd = qInd_; }
      void setuSize(int uSize_, int i=0) { uSize[i] = uSize_; }
      void setuInd(int uInd_, int i=0) { uInd[i] = uInd_; }
      void setxSize(int xSize_) { xSize = xSize_; }
      void setxInd(int xInd_) { xInd = xInd_; }
      //void sethSize(int hSize_) { hSize = hSize_; }
      void sethSize(int hSize_, int i=0);
      void sethInd(int hInd_, int i=0) { hInd[i] = hInd_; }
      int  getqInd() { return qInd; }
      int  getuInd(int i=0) { return uInd[i]; }
      int  gethInd(int i=0) { return hInd[i]; }

      void sethsSize(int hsSize_);

      int gethInd(Subsystem* sys, int i=0); 

      bool gActiveChanged();

      virtual int solveConstraintsFixpointSingle();
      virtual int solveImpactsFixpointSingle();
      virtual int solveConstraintsGaussSeidel();
      virtual int solveImpactsGaussSeidel();
      virtual int solveConstraintsRootFinding();
      virtual int solveImpactsRootFinding();
      virtual int jacobianConstraints();
      virtual int jacobianImpacts();

      virtual void updaterFactors();

      virtual void checkConstraintsForTermination();
      virtual void checkImpactsForTermination();

      void setMultiBodySystem(MultiBodySystem* sys);
      void setFullName(const string &str);
      void setlaIndMBS(int laIndParent);

      virtual string getType() const {return "Subsystem";}

      virtual void plot(double t, double dt, bool top=true);
      virtual void initPlot(bool top=true);
      virtual void closePlot();
      virtual H5::Group *getPlotGroup() { return plotGroup; }
      PlotFeatureStatus getPlotFeature(PlotFeature fp) { return Element::getPlotFeature(fp); };
      PlotFeatureStatus getPlotFeatureForChildren(PlotFeature fp) { return Element::getPlotFeatureForChildren(fp); };
  };
}

#endif

