#include "system.h"
#include <mbsim/integrators/integrators.h>

using namespace MBSim;
using namespace std;

int main (int argc, char* argv[]) {

  System *sys = new System("MBS");

  sys->setStopIfNoConvergence(true,true);
  double dt = 5e-5;
  sys->setGeneralizedImpulseTolerance(1e-2*dt);
  sys->setGeneralizedRelativeVelocityTolerance(1e-8);
  sys->initialize();

  TimeSteppingIntegrator integrator;

  integrator.setEndTime(0.5);
  integrator.setStepSize(dt);
  integrator.setPlotStepSize(5e-3);

  integrator.integrate(*sys);

  cout << "finished"<<endl;

  delete sys;

  return 0;

}

