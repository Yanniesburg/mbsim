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

#ifndef _CONTACT_RIGID_BILATERAL_H_
#define _CONTACT_RIGID_BILATERAL_H_

#include "contact_rigid.h"

namespace MBSim {

  /*! \brief Class for bilateral rigid contacts 
   *
   */
  class ContactRigidBilateral: public ContactRigid {

    public: 
      ContactRigidBilateral(const string &name);

      void projectGS(double dt);
      void solveGS(double dt);
      void checkForTermination(double dt);
      void checkActive() {}

      void residualProj(double dt);
      void residualProjJac(double dt);
  };

}

#endif
