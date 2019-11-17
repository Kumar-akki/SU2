/*!
 * \file CQUAD4.cpp
 * \brief Definition of the 4-node quadrilateral element with 4 Gauss points.
 * \author R. Sanchez
 * \version 6.2.0 "Falcon"
 *
 * The current SU2 release has been coordinated by the
 * SU2 International Developers Society <www.su2devsociety.org>
 * with selected contributions from the open-source community.
 *
 * The main research teams contributing to the current release are:
 *  - Prof. Juan J. Alonso's group at Stanford University.
 *  - Prof. Piero Colonna's group at Delft University of Technology.
 *  - Prof. Nicolas R. Gauger's group at Kaiserslautern University of Technology.
 *  - Prof. Alberto Guardone's group at Polytechnic University of Milan.
 *  - Prof. Rafael Palacios' group at Imperial College London.
 *  - Prof. Vincent Terrapon's group at the University of Liege.
 *  - Prof. Edwin van der Weide's group at the University of Twente.
 *  - Lab. of New Concepts in Aeronautics at Tech. Institute of Aeronautics.
 *
 * Copyright 2012-2019, Francisco D. Palacios, Thomas D. Economon,
 *                      Tim Albring, and the SU2 contributors.
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../../include/geometry/primitives/CElement.hpp"


CQUAD4::CQUAD4() : CElementWithKnownSizes<NGAUSS,NNODE,NDIM>() {

  /*--- Gauss coordinates and weights ---*/

  su2double oneOnSqrt3 = 0.577350269189626;

  GaussCoord[0][0] = -oneOnSqrt3;  GaussCoord[0][1] = -oneOnSqrt3;  GaussWeight(0) = 1.0;
  GaussCoord[1][0] =  oneOnSqrt3;  GaussCoord[1][1] = -oneOnSqrt3;  GaussWeight(1) = 1.0;
  GaussCoord[2][0] =  oneOnSqrt3;  GaussCoord[2][1] =  oneOnSqrt3;  GaussWeight(2) = 1.0;
  GaussCoord[3][0] = -oneOnSqrt3;  GaussCoord[3][1] =  oneOnSqrt3;  GaussWeight(3) = 1.0;

  /*--- Store the values of the shape functions and their derivatives ---*/

  unsigned short iNode, iGauss;
  su2double Xi, Eta, val_Ni;

  for (iGauss = 0; iGauss < NGAUSS; iGauss++) {

    Xi = GaussCoord[iGauss][0];
    Eta = GaussCoord[iGauss][1];

    val_Ni = 0.25*(1.0-Xi)*(1.0-Eta);		GaussPoint[iGauss].SetNi(val_Ni,0);
    val_Ni = 0.25*(1.0+Xi)*(1.0-Eta);		GaussPoint[iGauss].SetNi(val_Ni,1);
    val_Ni = 0.25*(1.0+Xi)*(1.0+Eta);		GaussPoint[iGauss].SetNi(val_Ni,2);
    val_Ni = 0.25*(1.0-Xi)*(1.0+Eta);		GaussPoint[iGauss].SetNi(val_Ni,3);

    /*--- dN/d xi, dN/d eta ---*/

    dNiXj[iGauss][0][0] = -0.25*(1.0-Eta);  dNiXj[iGauss][0][1] = -0.25*(1.0-Xi);
    dNiXj[iGauss][1][0] =  0.25*(1.0-Eta);  dNiXj[iGauss][1][1] = -0.25*(1.0+Xi);
    dNiXj[iGauss][2][0] =  0.25*(1.0+Eta);  dNiXj[iGauss][2][1] =  0.25*(1.0+Xi);
    dNiXj[iGauss][3][0] = -0.25*(1.0+Eta);  dNiXj[iGauss][3][1] =  0.25*(1.0-Xi);

  }

  /*--- Store the extrapolation functions (used to compute nodal stresses) ---*/

  su2double ExtrapCoord[4][2], sqrt3 = 1.732050807568877;;

  ExtrapCoord[0][0] = -sqrt3;  ExtrapCoord[0][1] = -sqrt3;
  ExtrapCoord[1][0] =  sqrt3;  ExtrapCoord[1][1] = -sqrt3;
  ExtrapCoord[2][0] =  sqrt3;  ExtrapCoord[2][1] =  sqrt3;
  ExtrapCoord[3][0] = -sqrt3;  ExtrapCoord[3][1] =  sqrt3;

  for (iNode = 0; iNode < NNODE; iNode++) {

    Xi = ExtrapCoord[iNode][0];
    Eta = ExtrapCoord[iNode][1];

    NodalExtrap[iNode][0] = 0.25*(1.0-Xi)*(1.0-Eta);
    NodalExtrap[iNode][1] = 0.25*(1.0+Xi)*(1.0-Eta);
    NodalExtrap[iNode][2] = 0.25*(1.0+Xi)*(1.0+Eta);
    NodalExtrap[iNode][3] = 0.25*(1.0-Xi)*(1.0+Eta);

  }

}

su2double CQUAD4::ComputeArea(const FrameType mode) const {

  unsigned short iDim;
  su2double a[2] = {0.0,0.0}, b[2] = {0.0,0.0};
  su2double Area = 0.0;

  /*--- Select the appropriate source for the nodal coordinates depending on the frame requested
        for the gradient computation, REFERENCE (undeformed) or CURRENT (deformed)---*/
  const su2activematrix& Coord = (mode==REFERENCE) ? RefCoord : CurrentCoord;

  for (iDim = 0; iDim < NDIM; iDim++) {
    a[iDim] = Coord[0][iDim]-Coord[2][iDim];
    b[iDim] = Coord[1][iDim]-Coord[2][iDim];
  }

  Area = 0.5*fabs(a[0]*b[1]-a[1]*b[0]);

  for (iDim = 0; iDim < NDIM; iDim++) {
    a[iDim] = Coord[0][iDim]-Coord[3][iDim];
    b[iDim] = Coord[2][iDim]-Coord[3][iDim];
  }

  Area += 0.5*fabs(a[0]*b[1]-a[1]*b[0]);

  return Area;

}

