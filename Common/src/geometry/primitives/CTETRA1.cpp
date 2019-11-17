/*!
 * \file CTETRA1.cpp
 * \brief Definition of 4-node tetra element with 1 Gauss point.
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


CTETRA1::CTETRA1() : CElementWithKnownSizes<NGAUSS,NNODE,NDIM>() {

  /*--- Gauss coordinates and weights ---*/

  GaussCoord[0][0] = 0.25;  GaussCoord[0][1] = 0.25; GaussCoord[0][2] = 0.25;  GaussWeight(0) = 1.0/6.0;

  /*--- Store the values of the shape functions and their derivatives ---*/

  unsigned short iGauss;
  su2double Xi, Eta, Zeta, val_Ni;

  for (iGauss = 0; iGauss < NGAUSS; iGauss++) {

    Xi = GaussCoord[iGauss][0];
    Eta = GaussCoord[iGauss][1];
    Zeta = GaussCoord[iGauss][2];

    val_Ni = Xi;						  GaussPoint[iGauss].SetNi(val_Ni,0);
    val_Ni = Eta;						  GaussPoint[iGauss].SetNi(val_Ni,1);
    val_Ni = 1.0-Xi-Eta-Zeta;	GaussPoint[iGauss].SetNi(val_Ni,2);
    val_Ni = Zeta;					  GaussPoint[iGauss].SetNi(val_Ni,3);

    /*--- dN/d xi, dN/d eta, dN/d zeta ---*/

    dNiXj[iGauss][0][0] =  1.0;  dNiXj[iGauss][0][1] =  0.0;  dNiXj[iGauss][0][2] =  0.0;
    dNiXj[iGauss][1][0] =  0.0;  dNiXj[iGauss][1][1] =  1.0;  dNiXj[iGauss][1][2] =  0.0;
    dNiXj[iGauss][2][0] = -1.0;  dNiXj[iGauss][2][1] = -1.0;  dNiXj[iGauss][2][2] = -1.0;
    dNiXj[iGauss][3][0] =  0.0;  dNiXj[iGauss][3][1] =  0.0;  dNiXj[iGauss][3][2] =  1.0;

  }

  /*--- Shape functions evaluated at the nodes for extrapolation of the stresses at the Gaussian Points ---*/
  /*--- The stress is constant at a TETRA1 element ---*/

  NodalExtrap[0][0] = 1.0;
  NodalExtrap[1][0] = 1.0;
  NodalExtrap[2][0] = 1.0;
  NodalExtrap[3][0] = 1.0;

}

su2double CTETRA1::ComputeVolume(const FrameType mode) const {

  unsigned short iDim;
  su2double r1[3] = {0.0,0.0,0.0}, r2[3] = {0.0,0.0,0.0}, r3[3] = {0.0,0.0,0.0}, CrossProduct[3] = {0.0,0.0,0.0};
  su2double Volume = 0.0;

  /*--- Select the appropriate source for the nodal coordinates depending on the frame requested
        for the gradient computation, REFERENCE (undeformed) or CURRENT (deformed)---*/
  const su2activematrix& Coord = (mode==REFERENCE) ? RefCoord : CurrentCoord;

  for (iDim = 0; iDim < NDIM; iDim++) {
    r1[iDim] = Coord[1][iDim] - Coord[0][iDim];
    r2[iDim] = Coord[2][iDim] - Coord[0][iDim];
    r3[iDim] = Coord[3][iDim] - Coord[0][iDim];
  }

  CrossProduct[0] = (r1[1]*r2[2] - r1[2]*r2[1])*r3[0];
  CrossProduct[1] = (r1[2]*r2[0] - r1[0]*r2[2])*r3[1];
  CrossProduct[2] = (r1[0]*r2[1] - r1[1]*r2[0])*r3[2];

  Volume = fabs(CrossProduct[0] + CrossProduct[1] + CrossProduct[2])/6.0;

  return Volume;

}

