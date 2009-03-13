/* Copyright (C) 2007  Martin Förg, Roland Zander
 
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
 *   rzander@users.berlios.de
 *
 */

#include <config.h> 
#include "circlesolid_line.h"
#include "contact.h"

namespace MBSim {

  double computeAngleOnUnitSphere(const Vec& r) {
    return r(0)>=0 ? (r(1)>=0 ? acos(r(0)) : 2*M_PI-acos(r(0))) : (r(1)>=0 ? acos(r(0)) : 2*M_PI-acos(r(0)));
  }

  void ContactKinematicsCircleSolidLine::assignContours(const vector<Contour*> &contour) {
    if(dynamic_cast<CircleSolid*>(contour[0])) {
      icircle = 0; iline = 1;
      circlesolid = static_cast<CircleSolid*>(contour[0]);
      line = static_cast<Line*>(contour[1]);
    } else {
      icircle = 1; iline = 0;
      circlesolid = static_cast<CircleSolid*>(contour[1]);
      line = static_cast<Line*>(contour[0]);
    }
  }

  void ContactKinematicsCircleSolidLine::updateg(Vec &g, ContourPointData *cpData) {

    cpData[iline].cosy.setOrientation(line->getFrame()->getOrientation());
    cpData[icircle].cosy.getOrientation().col(0) = -line->getFrame()->getOrientation().col(0);
    cpData[icircle].cosy.getOrientation().col(1) = -line->getFrame()->getOrientation().col(1);
    cpData[icircle].cosy.getOrientation().col(2) = line->getFrame()->getOrientation().col(2);

    Vec Wn = cpData[iline].cosy.getOrientation().col(0);

    Vec Wd = circlesolid->getFrame()->getPosition() - line->getFrame()->getPosition();

    g(0) = trans(Wn)*Wd - circlesolid->getRadius();

    cpData[icircle].cosy.setPosition(circlesolid->getFrame()->getPosition() - Wn*circlesolid->getRadius());
    cpData[iline].cosy.setPosition(cpData[icircle].cosy.getPosition() - Wn*g(0));
  }

  void ContactKinematicsCircleSolidLine::updategd(const Vec &g, Vec &gd, ContourPointData *cpData) {}

  void ContactKinematicsCircleSolidLine::updatewb(Vec &wb, const Vec &g, ContourPointData *cpData) {

    Vec v2 = cpData[icircle].cosy.getOrientation().col(2);
    Vec n1 = cpData[iline].cosy.getOrientation().col(0);
    Vec n2 = cpData[icircle].cosy.getOrientation().col(0);
    Vec u1 = cpData[iline].cosy.getOrientation().col(1);
    Vec u2 = cpData[icircle].cosy.getOrientation().col(1);
    Vec vC1 = cpData[iline].cosy.getVelocity();
    Vec vC2 = cpData[icircle].cosy.getVelocity();
    Vec Om1 = cpData[iline].cosy.getAngularVelocity();
    Vec Om2 = cpData[icircle].cosy.getAngularVelocity();
    double r = circlesolid->getRadius();

    double ad2 = -trans(v2)*(Om2-Om1);
    double ad1 = trans(u1)*(vC2-vC1) - r*ad2;
    Vec s2 = u2*r;

    wb(0) += trans(n1)*(-crossProduct(Om1,vC2-vC1) - crossProduct(Om1,u1)*ad1 + crossProduct(Om2,s2)*ad2);
    
    if(wb.size() > 1) 
      wb(1) += trans(u1)*(-crossProduct(Om1,vC2-vC1) - crossProduct(Om1,u1)*ad1 + crossProduct(Om2,s2)*ad2);
  }

}

