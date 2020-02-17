/*!
 * \file CSourceVolumetricHeat.cpp
 * \brief Numerical methods for volumetric heat source term integration.
 * \author Ruben Sanchez
 * \version 7.0.1 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2019, SU2 Contributors (cf. AUTHORS.md)
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

#include "../../../include/numerics/source/CSourceVolumetricHeat.hpp"

CSourceVolumetricHeat::CSourceVolumetricHeat(unsigned short val_nDim, unsigned short val_nVar, CConfig *config) : CNumerics(val_nDim, val_nVar, config) {

  implicit = (config->GetKind_TimeIntScheme_Flow() == EULER_IMPLICIT);

}

void CSourceVolumetricHeat::ComputeResidual(su2double *val_residual, su2double **val_Jacobian_i, CConfig *config) {

  unsigned short iDim;

  /*--- Zero the continuity contribution ---*/

  val_residual[0] = 0.0;

  /*--- Zero the momentum contribution. ---*/

  for (iDim = 0; iDim < nDim; iDim++)
    val_residual[iDim+1] = 0.0;

  /*--- Set the energy contribution ---*/

  val_residual[nDim+1] = -1.0*config->GetHeatSource_Val()*Volume;

  /*--- Jacobian contribution is 0 as the heat source is constant ---*/

  if (implicit) {

    for (unsigned short i = 0; i < 4; i++)
      for (unsigned short j = 0; j < 4; j++)
        val_Jacobian_i[i][j] = 0.0;

  }

}
