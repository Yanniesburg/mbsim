#include "system.h"
#include "mbsim/integrators/integrators.h"

using namespace std;
using namespace MBSim;

int main (int argc, char* argv[])
{
  System *sys = new System("TS");
  sys->initialize();

  HETS2Integrator integrator;
  integrator.setEndTime(0.1);
  integrator.setStepSize(1e-4);
  integrator.setPlotStepSize(1e-4);

  integrator.integrate(*sys);

  delete sys;

  return 0;
}

