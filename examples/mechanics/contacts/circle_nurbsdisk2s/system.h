#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "mbsim/dynamic_system_solver.h"
#include <string>

class System : public MBSim::DynamicSystemSolver {
  public:
    System(const std::string &projectName);
    void init(InitStage stage, const MBSim::InitConfigSet &config);
};

#endif /* _SYSTEM_H */

