#include <mbsim/numerics/nonlinear_algebra/multi_dimensional_newton_method.h>
#include <mbsim/utils/nonlinear_algebra.h>
#include <mbsim/utils/stopwatch.h>
#include <fmatvec/fmatvec.h>

#include <iostream>

using namespace fmatvec;
using namespace MBSim;
using namespace std;


class TestFunction : public MBSim::Function<fmatvec::Vec(fmatvec::Vec)> {

  public:
    /**
     * \brief Constructor
     */
  TestFunction(){
  }

    /**
     * \brief Destructor
     */
    virtual ~TestFunction() {
    }

    virtual fmatvec::Vec operator ()(const fmatvec::Vec & vector) {
      Vec result(vector.size(), INIT, 0.);

      for(int i=0; i< result.size(); i++) {
        result(i) = pow(sin(i*2*M_PI/vector.size()) + vector(i), 2);
      }

      return result;
    }

};

int main (int argc, char* argv[]) {

  int dimension = 1000;

  TestFunction * function = new TestFunction();
  NewtonJacobianFunction * jac = new NumericalNewtonJacobianFunction();

  MultiDimensionalNewtonMethod newton;
  newton.setFunction(function);
  newton.setJacobianFunction(jac);

  map<RangeV, double> tolerances;
  tolerances.insert(pair<RangeV, double>(RangeV(0,dimension/2-1), 1e-10));
  tolerances.insert(pair<RangeV, double>(RangeV(dimension/2,dimension-1), 1e-8));



  Vec initialSolution(dimension,INIT,0.0);
  for(int i =0; i< dimension; i++) {
    initialSolution(i) = 5*i;
  }
  Vec test1 = initialSolution.copy();
  Vec test2 = initialSolution.copy();

  StopWatch sw;

  cout << "Solving system of dimension " << dimension << endl;

  /*New Newton*/
  for(int i =0 ; i < 2; i++) {
    CriteriaFunction *cfunc;
    if(i==0) {
      cfunc = new GlobalResidualCriteriaFunction(1e-10);
      cout << "Solving with GlobalResidualCriteriaFunction with tolerance of 1e-10 ... " << endl;
    }
    else {
      cout << "Solving with LocalResidualCriteriaFunction with tolerance for first half of solution vector of 1e-10 and second half of 1e-8 ... " << endl;
      cfunc = new LocalResidualCriteriaFunction(tolerances);
    }
    newton.setCriteriaFunction(cfunc);
    sw.start();
    test1 = newton.solve(initialSolution);

    cout << "Time = " << sw.stop(true) << endl;
    cout << "Iterations = " << newton.getNumberOfIterations() << endl << endl;

    delete cfunc;
  }

  /*Old newton*/
  cout << "Solving system with old newton algorithm as reference with tolerance of 1e-10 ... " << endl;
  TestFunction * function2 = new TestFunction();
  MultiDimNewtonMethod newton2(function2);
  newton2.setTolerance(1e-10);

  sw.start();

  test2 = newton2.solve(test2);

  cout << "Time = " << sw.stop(true) << endl;
  cout << "Iterations = " << newton2.getNumberOfIterations() << endl;
  cout << "REAMARK: The iterations are counted differently (minus one) with the old newton..." << endl;

  delete function;
  delete jac;
  delete function2;

  return 0;

}
