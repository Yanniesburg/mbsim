#include "woodpecker.h"
#include <mbsim/integrators/integrators.h>

using namespace std;
using namespace MBSim;

int main (int argc, char* argv[]) {

  Woodpecker sys("MBS");
  sys.setConstraintSolver(DynamicSystemSolver::GaussSeidel);
  sys.setImpactSolver(DynamicSystemSolver::GaussSeidel);
//  sys.setFlushEvery(100);
  sys.initialize();

  Integrator* integrator;
  //integrator.setDriftCompensation(true);

  sys.setStopIfNoConvergence(true, true);
  bool eventDriven = true;
  if(eventDriven) { // Event driven time integration
    sys.setProjectionTolerance(1e-15);
    sys.setGeneralizedRelativePositionTolerance(1e-6);
    sys.setGeneralizedRelativeVelocityTolerance(1e-8);
    sys.setGeneralizedImpulseTolerance(1e-8);
    sys.setGeneralizedRelativeAccelerationTolerance(1e-10);
    sys.setGeneralizedForceTolerance(1e-10);
    integrator = new LSODEIntegrator;
    static_cast<LSODEIntegrator*>(integrator)->setInitialStepSize(1e-10);
    static_cast<LSODEIntegrator*>(integrator)->setMaximumStepSize(1e-2);
    static_cast<LSODEIntegrator*>(integrator)->setAbsoluteTolerance(1e-7);
    static_cast<LSODEIntegrator*>(integrator)->setRelativeTolerance(1e-7);
    static_cast<LSODEIntegrator*>(integrator)->setRootFindingAccuracy(1e-14);
    static_cast<LSODEIntegrator*>(integrator)->setToleranceForPositionConstraints(1e-5);
    static_cast<LSODEIntegrator*>(integrator)->setToleranceForVelocityConstraints(1e-5);
  }
  else { // time stepping integration
    double dt = 1e-5;
    sys.setGeneralizedImpulseTolerance(1e-2*dt);
    sys.setGeneralizedRelativeVelocityTolerance(1e-8);
    integrator = new TimeSteppingIntegrator;
    //integrator = new ThetaTimeSteppingIntegrator;
    static_cast<TimeSteppingIntegrator*>(integrator)->setStepSize(dt);
    //static_cast<ThetaTimeSteppingIntegrator*>(integrator)->setTheta(0.75);
  }

  integrator->setEndTime(5.001);
  integrator->setPlotStepSize(1e-3);
  integrator->integrate(sys);

  delete integrator;

  return 0;
}

