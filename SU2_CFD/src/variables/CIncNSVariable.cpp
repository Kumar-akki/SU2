/*!
 * \file CIncNSVariable.cpp
 * \brief Definition of the variable classes for incompressible flow.
 * \author F. Palacios, T. Economon
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

#include "../../include/variables/CIncNSVariable.hpp"


CIncNSVariable::CIncNSVariable(su2double pressure, const su2double *velocity, su2double temperature,
                               Idx_t npoint, Idx_t ndim, Idx_t nvar, CConfig *config) :
                               CIncEulerVariable(pressure, velocity, temperature, npoint, ndim, nvar, config) {
  Vorticity.resize(nPoint,3);
  StrainMag.resize(nPoint);
  DES_LengthScale.resize(nPoint) = su2double(0.0);
}

bool CIncNSVariable::SetVorticity(Idx_t iPoint) {

  Vorticity(iPoint,0) = 0.0; Vorticity(iPoint,1) = 0.0;

  Vorticity(iPoint,2) = Gradient_Primitive(iPoint,2,0)-Gradient_Primitive(iPoint,1,1);

  if (nDim == 3) {
    Vorticity(iPoint,0) = Gradient_Primitive(iPoint,3,1)-Gradient_Primitive(iPoint,2,2);
    Vorticity(iPoint,1) = -(Gradient_Primitive(iPoint,3,0)-Gradient_Primitive(iPoint,1,2));
  }
  return false;
}

bool CIncNSVariable::SetStrainMag(Idx_t iPoint) {

  AD::StartPreacc();
  AD::SetPreaccIn(Gradient_Primitive[iPoint], nDim+1, nDim);

  su2double Div = 0.0;
  for (Idx_t iDim = 0; iDim < nDim; iDim++)
    Div += Gradient_Primitive(iPoint,iDim+1,iDim);

  StrainMag(iPoint) = 0.0;

  /*--- Add diagonal part ---*/

  for (Idx_t iDim = 0; iDim < nDim; iDim++) {
    StrainMag(iPoint) += pow(Gradient_Primitive(iPoint,iDim+1,iDim) - 1.0/3.0*Div, 2.0);
  }
  if (nDim == 2) {
    StrainMag(iPoint) += pow(1.0/3.0*Div, 2.0);
  }

  /*--- Add off diagonals ---*/

  StrainMag(iPoint) += 2.0*pow(0.5*(Gradient_Primitive(iPoint,1,1) + Gradient_Primitive(iPoint,2,0)), 2);

  if (nDim == 3) {
    StrainMag(iPoint) += 2.0*pow(0.5*(Gradient_Primitive(iPoint,1,2) + Gradient_Primitive(iPoint,3,0)), 2);
    StrainMag(iPoint) += 2.0*pow(0.5*(Gradient_Primitive(iPoint,2,2) + Gradient_Primitive(iPoint,3,1)), 2);
  }

  StrainMag(iPoint) = sqrt(2.0*StrainMag(iPoint));

  AD::SetPreaccOut(StrainMag(iPoint));
  AD::EndPreacc();

  return false;
}


bool CIncNSVariable::SetPrimVar(Idx_t iPoint, su2double eddy_visc, su2double turb_ke, CFluidModel *FluidModel) {

  unsigned short iVar;
  bool check_dens = false, check_temp = false, physical = true;

  /*--- Store the density from the previous iteration. ---*/

  Density_Old(iPoint) = GetDensity(iPoint);

  /*--- Set the value of the pressure ---*/

  SetPressure(iPoint);

  /*--- Set the value of the temperature directly ---*/

  su2double Temperature = Solution(iPoint,nDim+1);
  check_temp = SetTemperature(iPoint,Temperature);

  /*--- Use the fluid model to compute the new value of density.
  Note that the thermodynamic pressure is constant and decoupled
  from the dynamic pressure being iterated. ---*/

  /*--- Use the fluid model to compute the new value of density. ---*/

  FluidModel->SetTDState_T(Temperature);

  /*--- Set the value of the density ---*/

  check_dens = SetDensity(iPoint, FluidModel->GetDensity());

  /*--- Non-physical solution found. Revert to old values. ---*/

  if (check_dens || check_temp) {

    /*--- Copy the old solution ---*/

    for (iVar = 0; iVar < nVar; iVar++)
      Solution(iPoint,iVar) = Solution_Old(iPoint,iVar);

    /*--- Recompute the primitive variables ---*/

    Temperature = Solution(iPoint,nDim+1);
    SetTemperature(iPoint, Temperature);
    FluidModel->SetTDState_T(Temperature);
    SetDensity(iPoint, FluidModel->GetDensity());

    /*--- Flag this point as non-physical. ---*/

    physical = false;

  }

  /*--- Set the value of the velocity and velocity^2 (requires density) ---*/

  SetVelocity(iPoint);

  /*--- Set laminar viscosity ---*/

  SetLaminarViscosity(iPoint, FluidModel->GetLaminarViscosity());

  /*--- Set eddy viscosity locally and in the fluid model. ---*/

  SetEddyViscosity(iPoint, eddy_visc);
  FluidModel->SetEddyViscosity(eddy_visc);

  /*--- Set thermal conductivity (effective value if RANS). ---*/

  SetThermalConductivity(iPoint, FluidModel->GetThermalConductivity());

  /*--- Set specific heats ---*/

  SetSpecificHeatCp(iPoint, FluidModel->GetCp());
  SetSpecificHeatCv(iPoint, FluidModel->GetCv());

  return physical;

}
