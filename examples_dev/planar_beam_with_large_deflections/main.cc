#include "system.h"
#include <mbsim/integrators/integrators.h>

using namespace MBSim;
using namespace std;

int main (int argc, char* argv[]) {

  DynamicSystemSolver *sys = new System("MBS");

  sys->setImpactSolver(RootFinding);
  sys->setStopIfNoConvergence(true,true);
  sys->init();

  ThetaTimeSteppingIntegrator integrator;

  integrator.setTheta(0.5);
  integrator.setEndTime(0.4);
  integrator.setStepSize(1e-5);
  integrator.setPlotStepSize(1e-4);

  sys->closePlot();

  cout << "finished"<<endl;

  delete sys;

  return 0;

}

