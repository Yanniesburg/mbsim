/* Copyright (C) 2004-2012 MBSim Development Team
 *
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
 * Contact: martin.o.foerg@googlemail.com
 */

#include <config.h>

#include <ctime>
#include <iostream>

#include "linear_complementarity_problem.h"

#include <mbsim/utils/nonsmooth_algebra.h>
#include <mbsim/utils/stopwatch.h>
#include <mbsim/mbsim_event.h>

using namespace std;
using namespace fmatvec;

//TODO: get into fmatvec
namespace fmatvec {
  SqrMat Sym2Sqr(const SymMat & M_) {
    SqrMat M(M_.size(), NONINIT);
    for (int i = 0; i < M_.size(); i++)
      for (int j = i; j < M_.size(); j++) {
        M(i, j) = M(j, i) = M_(i, j);
      }
    return M;
  }
}

namespace MBSim {

  std::ostream& operator <<(std::ostream & o, const LinearComplementarityProblem::LCPSolvingStrategy &strategy) {
    switch (strategy) {
      case LinearComplementarityProblem::Standard:
        return o << "Standard";
      case LinearComplementarityProblem::ReformulatedStandard:
        return o << "ReformulatedStandard";
      case LinearComplementarityProblem::ReformulatedFixpointOnly:
        return o << "ReformulatedFixpointOnly";
      case LinearComplementarityProblem::ReformulatedNewtonOnly:
        return o << "ReformulatedNewtonOnly";
      case LinearComplementarityProblem::LemkeOnly:
        return o << "LemkeOnly";
      default:
        return o << "ERROR: Unknown LCP solving strategy";
    }
  }

  std::ostream& operator <<(std::ostream & o, const LinearComplementarityProblem::JacobianType &jacobianType) {
    switch (jacobianType) {
      case LinearComplementarityProblem::Numerical:
        return o << "Numerical";
      case LinearComplementarityProblem::LCPSpecial:
        return o << "LCPSpecial";
      default:
        return o << "ERROR: Unknown jacobian type";
    }
  }

  LinearComplementarityProblem::LinearComplementarityProblem(const SymMat & M_, const Vec & q_, const LCPSolvingStrategy & strategy_ /*= Standard*/, const JacobianType & jacobianType_ /*= LCPSpecial*/) :
      strategy(strategy_), mediumEigenValue(0.0), jacobianType(jacobianType_), lemkeSolver(), newtonSolver(nullptr), newtonFunction(nullptr), jacobianFunction(nullptr), criteriaNewton(nullptr), fixpointSolver(nullptr), fixpointFunction(nullptr), criteriaFixedpoint(nullptr) {

    //set properties

    newtonSolver = new MultiDimensionalNewtonMethod();

    fixpointSolver = new MultiDimensionalFixpointSolver();

    setSystem(M_, q_);
  }

  LinearComplementarityProblem::~LinearComplementarityProblem() {
    delete newtonSolver;
    delete newtonFunction;
    delete jacobianFunction;
    delete fixpointSolver;
    delete fixpointFunction;
    delete criteriaNewton;
    delete criteriaFixedpoint;
  }

  void LinearComplementarityProblem::setSystem(const SymMat & M_, const Vec & q_) {
    q.resize() = q_;
    M.resize() = Sym2Sqr(M_);

    /*set different solvers*/
    //Lemke
    lemkeSolver.setSystem(M, q);

    //Newton
    if (!jacobianFunction) {
      if (jacobianType == LCPSpecial)
        jacobianFunction = new LinearComplementarityJacobianFunction();
      else
        jacobianFunction = new NumericalNewtonJacobianFunction();
    }

    newtonSolver->setJacobianFunction(jacobianFunction);

    if (!newtonFunction) {
      newtonFunction = new LCPNewtonReformulationFunction();

      newtonSolver->setFunction(newtonFunction);

      if (!criteriaNewton) {
        criteriaNewton = new GlobalResidualCriteriaFunction();

        newtonSolver->setCriteriaFunction(criteriaNewton);
      }
    }

    newtonFunction->setSystem(M, q);

    //Fixpoint
    if (!fixpointFunction) {
      fixpointFunction = new LCPFixpointReformulationFunction();

      fixpointSolver->setFunction(fixpointFunction);

      if (!criteriaFixedpoint) {
        criteriaFixedpoint = new GlobalShiftCriteriaFunction();

        fixpointSolver->setCriteriaFunction(criteriaFixedpoint);
      }
    }

    fixpointFunction->setSystem(M, q);

  }

  Vec LinearComplementarityProblem::solve(const Vec & initialSolution /*= Vec(0,NONINIT)*/) {

    /*dimension of the system*/
    int dimension = q.size();

    //flag (is system solved?)
    bool solved = false;

    //solution vector
    Vec solution(2 * dimension, NONINIT);

    Vec w;
    w >> solution(0, dimension - 1);
    Vec z;
    z >> solution(dimension, 2 * dimension - 1);

    StopWatch mainwatch;
    mainwatch.start();

    if (msgAct(Debug)) {
      msg(Debug) << "*****" << __func__ << "*****" << endl;
      msg(Debug) << "Solving-strategy is: " << strategy << endl;
      msg(Debug) << "Jacobian Type is: " << jacobianType << endl;
      msg(Debug) << "dimension: " << dimension << endl;
    }

    //NOTE: Fortran-NewtonSolver seems to work worse than the "native" NewtonSolver of the utils in mbsim
    //      MultiDimNewtonMethodFortran NewtonSolverFortran;
    //      NewtonSolverFortran.setRootFunction(&func);
    //      Vec gapLambdaFortran = NewtonSolverFortran.solve(gapLambda0);
    if (strategy != LemkeOnly) {
      if (strategy == Standard) {
        clock_t t_start_Lemke1 = clock();
        solution = lemkeSolver.solve(dimension);

        if (lemkeSolver.getInfo() == 0) {
          solved = true;

          if (msgAct(Debug)) {
            msg(Debug) << "LemkerSolver found solution in " << lemkeSolver.getSteps() << " step(s)." << endl;
            if (msgAct(Debug)) {
              double cpuTime = double(clock() - t_start_Lemke1) / CLOCKS_PER_SEC;
              msg(Debug) << "... in: " << cpuTime << "s = " << cpuTime / 3600 << "h" << endl;
              if (msgAct(Debug)) { //Lemke-Solver Info
                msg(Debug) << "solution: " << solution << endl;
                msg(Debug) << "gaps       forces   " << endl;
                for (int i = 0; i < solution.size() / 2; i++) {
                  msg(Debug) << solution(i) << " | " << solution(i + solution.size() / 2) << endl;
                }
              }
            }
          }
        }

        else {
          if (msgAct(Debug)) {
            msg(Debug) << "No Solution for Lemke-Algorithm. Now Trying combination of Newton + FixpointIteration" << endl;
          }
        }
      } //End Lemke-Solver in standard strategy

      //clock_t t_start_Iterative = clock();

      /*Get initial solution for the reformulated system to apply recursive schemes*/
      if (not solved) {
        if (initialSolution.size() != 2 * dimension)
          solution = createInitialSolution(M, q);
        else {
          solution = initialSolution;
        }
      }

      /*Try to solve the reformulated system with iterative schemes*/
      for (int loop = 0; loop < 5 and not solved; loop++) {
        if (strategy == ReformulatedNewtonOnly) {
          useNewton(solution, solved);
        }
        else if (strategy == ReformulatedFixpointOnly) {
          useFixpoint(solution, solved);
        }
        else {
          useNewton(solution, solved);

          switch (newtonSolver->getInfo()) {
            case 0:

            break;
            case -1: {
              useFixpoint(solution, solved);
              break;
            }
          }
        }
      } /*End reformulated system*/

      /*If no solution is found with the other algorithms use the Lemke-Algorithm with a maxmimal number of steps as fallback*/
      if (!solved) {
        if (msgAct(Debug)) {
          msg(Debug) << "No convergence during calculation of contact forces with reformulated system scheme!" << endl;
          msg(Debug) << "Using Lemke Algorithm with maximal number of steps as fallback." << endl;
        }

//        if (strategy == ReformulatedFixpointOnly or strategy == ReformulatedNewtonOnly or strategy == ReformulatedStandard) {
//          // If reformulated routines fails: assume that they will fail in the future and switch back to standard solution
//          strategy = Standard;
//          if (msgAct(Debug)) {
//            msg(Debug) << "Switching back to standard solution strategy!" << endl;
//          }
//        }
      }
      else if(lemkeSolver.getInfo() != 0) {
        //Switch back strategy in case shortened Lemke has failed but reformulated were succesfull
        strategy = oldStrategy;
      }
    }

    if (not solved) {

      clock_t t_start_Lemke1 = clock();
      lemkeSolver.setSystem(M, q);
      solution = lemkeSolver.solve();

      if (lemkeSolver.getInfo() == 0) {
        if (msgAct(Debug)) {
          msg(Debug) << "LemkerSolver found solution (in fallback case): " << lemkeSolver.getSteps() << " step(s)." << endl;
          if (msgAct(Debug)) {
            double cpuTime = double(clock() - t_start_Lemke1) / CLOCKS_PER_SEC;
            msg(Debug) << "... in: " << cpuTime << "s = " << cpuTime / 3600 << "h" << endl;
            if (msgAct(Debug)) { //Newton-Solver Info
              msg(Debug) << "solution: " << solution << endl;
              msg(Debug) << "gaps       forces   " << endl;
              for (int i = 0; i < solution.size() / 2; i++) {
                msg(Debug) << solution(i) << " | " << solution(i + solution.size() / 2) << endl;
              }
            }
          }
        }
      }
      else {
        throw runtime_error("MaxwellContact::solveLCP(): No Solution found for this LCP");
      }
    }

    if (msgAct(Debug)) {
      msg(Debug) << "The complete algorithm needed: " << mainwatch.stop(false) << "s = " << mainwatch.stop(false) / 3600 << "h" << endl;
    }

    return solution;
  }

  double LinearComplementarityProblem::computeMediumEigVal(const SqrMat & M) {
    /*update Material constant*/
    double eigvalSum = 0;
    for (int i = 0; i < M.size(); i++) {
      eigvalSum += M(i, i);
    }
    return eigvalSum / M.size();
  }

  Vec LinearComplementarityProblem::createInitialSolution(const SymMat & M, const Vec & q, double mediumEigVal /*= 0*/) {
    return createInitialSolution(Sym2Sqr(M), q, mediumEigVal);
  }

  Vec LinearComplementarityProblem::createInitialSolution(const SqrMat & M, const Vec & q, double mediumEigVal /*= 0*/) {

    if (mediumEigVal <= 0)
      mediumEigVal = computeMediumEigVal(M);

    /*Initialize solution*/
    Vec solution(q.size() * 2, INIT, 0.);
    for (int i = 0; i < q.size(); i++) {
      if (q(i) > 0)
        solution(i) = q(i);
      else
        solution(i) = 0;
    }

    for (int i = q.size(); i < 2 * q.size(); i++) {
      if (q(i - q.size()) > 0)
        solution(i) = 0;
      else
        solution(i) = -q(i - q.size()) / mediumEigVal;
    }

    return solution;
  }

  void LinearComplementarityProblem::useNewton(Vec & solution, bool & solved) {
    if (msgAct(Debug))
      msg(Debug) << "Trying Newton Solver ... " << endl;

    int i = 0;
    do {
      solution = newtonSolver->solve(solution);

      if (newtonSolver->getInfo() == 1)
        if (msgAct(Debug)) {
          msg(Debug) << "Newton scheme seems to converge but has not finished --> Going on ..." << endl;
        }
    } while (i++ < 3 and newtonSolver->getInfo() == 1);

    if (msgAct(Debug)) {
      msg(Debug) << "Iterations = " << newtonSolver->getNumberOfIterations() << endl;
      if (msgAct(Debug)) {
        msg(Debug) << "Info about NewtonSolver" << endl;
        msg(Debug) << "nrm2(f(solution)):  " << nrm2((*newtonFunction)(solution)) << endl;
        if (msgAct(Debug)) { //Newton-Solver Info
          msg(Debug) << "solution: " << solution << endl;
          msg(Debug) << "gaps       forces   " << endl;
          for (int i = 0; i < solution.size() / 2; i++) {
            msg(Debug) << solution(i) << " | " << solution(i + solution.size() / 2) << endl;
          }
        }
      }
    }

    if (newtonSolver->getInfo() == 0) {
      solved = true;

      if (msgAct(Debug)) {
        msg(Debug) << "Newton-Solver found solution" << endl;

        if (msgAct(Debug)) {
          //double cpuTime = double(clock() - t_start_Iterative) / CLOCKS_PER_SEC;
          //msg(Debug) << "... in: " << cpuTime << "s = " << cpuTime / 3600 << "h" << endl;
          if (msgAct(Debug)) { //Newton-Solver Info
            msg(Debug) << "solution: " << solution << endl;
            msg(Debug) << "gaps       forces   " << endl;
            for (int i = 0; i < solution.size() / 2; i++) {
              msg(Debug) << solution(i) << " | " << solution(i + solution.size() / 2) << endl;
            }
          }
        }
      }
    }
  }

  void LinearComplementarityProblem::useFixpoint(Vec & solution, bool & solved) {
    if (msgAct(Debug))
      msg(Debug) << "Trying Fixpoint Solver ... " << endl;

    int i = 0;
    do {
      solution = fixpointSolver->solve(solution);

      if (fixpointSolver->getInfo() == 1)
        if (msgAct(Debug)) {
          msg(Debug) << "Fixpoint scheme seems to converge but has not finished --> Going on ..." << endl;
        }
    } while (i++ < 3 and fixpointSolver->getInfo() == 1);

    if (msgAct(Debug)) {
      msg(Debug) << "Iterations = " << fixpointSolver->getNumberOfIterations() << endl;
      if (msgAct(Debug)) {
        msg(Debug) << "Info about FixpointSolver" << endl;
        msg(Debug) << "nrm2(f(solution)):  " << nrm2((*newtonFunction)(solution)) << endl;
        if (msgAct(Debug)) {
          msg(Debug) << "solution: " << solution << endl;
          msg(Debug) << "gaps       forces   " << endl;
          for (int i = 0; i < solution.size() / 2; i++) {
            msg(Debug) << solution(i) << " | " << solution(i + solution.size() / 2) << endl;
          }
        }
      }
    }

    if (fixpointSolver->getInfo() == 0) {
      solved = true;

      if (msgAct(Debug)) {
        msg(Debug) << "Fixpoint-Solver found solution" << endl;
        if (msgAct(Debug)) {
          //double cpuTime = double(clock() - t_start_Iterative) / CLOCKS_PER_SEC;
          //msg(Debug) << "... in: " << cpuTime << "s = " << cpuTime / 3600 << "h" << endl;
          if (msgAct(Debug)) {
            msg(Debug) << "solution: " << solution << endl;
            msg(Debug) << "gaps       forces   " << endl;
            for (int i = 0; i < solution.size() / 2; i++) {
              msg(Debug) << solution(i) << " | " << solution(i + solution.size() / 2) << endl;
            }
          }
        }
      }
    }
  }

}
