/* Copyright (C) 2004-2016 MBSim Development Team
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
#include "mbsim/constraints/generalized_constraint.h"
#include "mbsim/frames/frame.h"

using namespace std;
using namespace fmatvec;
using namespace MBXMLUtils;
using namespace xercesc;

namespace MBSim {

  void GeneralizedConstraint::init(InitStage stage, const InitConfigSet &config) {
    if(stage==resolveStringRef) {
      if(!saved_supportFrame.empty())
        setSupportFrame(getByPath<Frame>(saved_supportFrame));
    }
    MechanicalConstraint::init(stage, config);
  }

  void GeneralizedConstraint::initializeUsingXML(DOMElement* element) {
    MechanicalConstraint::initializeUsingXML(element);
    DOMElement *e=E(element)->getFirstElementChildNamed(MBSIM%"supportFrame");
    if(e) saved_supportFrame=E(e)->getAttribute("ref");
  }

}
