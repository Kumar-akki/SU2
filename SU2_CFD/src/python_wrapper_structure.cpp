/*!
 * \file python_wrapper_structure.cpp
 * \brief Driver subroutines that are used by the Python wrapper. Those routines are usually called from an external Python environment.
 * \author D. Thomas
 * \version 7.5.1 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2023, SU2 Contributors (cf. AUTHORS.md)
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

#include "../../Common/include/toolboxes/geometry_toolbox.hpp"
#include "../include/drivers/CDriver.hpp"
#include "../include/drivers/CSinglezoneDriver.hpp"

void CDriver::PythonInterfacePreprocessing(CConfig** config, CGeometry**** geometry, CSolver***** solver) {
  int rank = MASTER_NODE;
  SU2_MPI::Comm_rank(SU2_MPI::GetComm(), &rank);

  /*--- Initialize boundary conditions customization, this is achieved through the Python wrapper. --- */
  for (iZone = 0; iZone < nZone; iZone++) {
    if (config[iZone]->GetnMarker_PyCustom() > 0) {
      if (rank == MASTER_NODE) cout << "----------------- Python Interface Preprocessing ( Zone " << iZone << " ) -----------------" << endl;

      if (rank == MASTER_NODE) cout << "Setting customized boundary conditions for zone " << iZone << endl;
      for (iMesh = 0; iMesh <= config[iZone]->GetnMGLevels(); iMesh++) {
        geometry[iZone][INST_0][iMesh]->SetCustomBoundary(config[iZone]);
      }
      geometry[iZone][INST_0][MESH_0]->UpdateCustomBoundaryConditions(geometry[iZone][INST_0], config[iZone]);

      if ((config[iZone]->GetKind_Solver() == MAIN_SOLVER::EULER) ||
          (config[iZone]->GetKind_Solver() == MAIN_SOLVER::NAVIER_STOKES) ||
          (config[iZone]->GetKind_Solver() == MAIN_SOLVER::RANS) ||
          (config[iZone]->GetKind_Solver() == MAIN_SOLVER::INC_EULER) ||
          (config[iZone]->GetKind_Solver() == MAIN_SOLVER::INC_NAVIER_STOKES) ||
          (config[iZone]->GetKind_Solver() == MAIN_SOLVER::INC_RANS) ||
          (config[iZone]->GetKind_Solver() == MAIN_SOLVER::NEMO_EULER) ||
          (config[iZone]->GetKind_Solver() == MAIN_SOLVER::NEMO_NAVIER_STOKES)) {
        solver[iZone][INST_0][MESH_0][FLOW_SOL]->UpdateCustomBoundaryConditions(geometry[iZone][INST_0], config[iZone]);
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
/* Functions related to the global performance indices (Lift, Drag, etc.)  */
/////////////////////////////////////////////////////////////////////////////

passivedouble CDriver::Get_Drag() const {
  unsigned short val_iZone = ZONE_0;
  unsigned short FinestMesh = config_container[val_iZone]->GetFinestMesh();
  su2double CDrag, factor, val_Drag;

  /*--- Calculate drag force based on drag coefficient ---*/
  factor = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetAeroCoeffsReferenceForce();
  CDrag = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetTotal_CD();

  val_Drag = CDrag * factor;

  return SU2_TYPE::GetValue(val_Drag);
}

passivedouble CDriver::Get_Lift() const {
  unsigned short val_iZone = ZONE_0;
  unsigned short FinestMesh = config_container[val_iZone]->GetFinestMesh();
  su2double CLift, factor, val_Lift;

  /*--- Calculate drag force based on drag coefficient ---*/
  factor = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetAeroCoeffsReferenceForce();
  CLift = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetTotal_CL();

  val_Lift = CLift * factor;

  return SU2_TYPE::GetValue(val_Lift);
}

passivedouble CDriver::Get_Mx() const {
  unsigned short val_iZone = ZONE_0;
  unsigned short FinestMesh = config_container[val_iZone]->GetFinestMesh();
  su2double CMx, RefLengthCoeff, factor, val_Mx;

  RefLengthCoeff = config_container[val_iZone]->GetRefLength();

  /*--- Calculate moment around x-axis based on coefficients ---*/
  factor = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetAeroCoeffsReferenceForce();
  CMx = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetTotal_CMx();

  val_Mx = CMx * factor * RefLengthCoeff;

  return SU2_TYPE::GetValue(val_Mx);
}

passivedouble CDriver::Get_My() const {
  unsigned short val_iZone = ZONE_0;
  unsigned short FinestMesh = config_container[val_iZone]->GetFinestMesh();
  su2double CMy, RefLengthCoeff, factor, val_My;

  RefLengthCoeff = config_container[val_iZone]->GetRefLength();

  /*--- Calculate moment around x-axis based on coefficients ---*/
  factor = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetAeroCoeffsReferenceForce();
  CMy = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetTotal_CMy();

  val_My = CMy * factor * RefLengthCoeff;

  return SU2_TYPE::GetValue(val_My);
}

passivedouble CDriver::Get_Mz() const {
  unsigned short val_iZone = ZONE_0;
  unsigned short FinestMesh = config_container[val_iZone]->GetFinestMesh();
  su2double CMz, RefLengthCoeff, factor, val_Mz;

  RefLengthCoeff = config_container[val_iZone]->GetRefLength();

  /*--- Calculate moment around z-axis based on coefficients ---*/
  factor = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetAeroCoeffsReferenceForce();
  CMz = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetTotal_CMz();

  val_Mz = CMz * factor * RefLengthCoeff;

  return SU2_TYPE::GetValue(val_Mz);
}

passivedouble CDriver::Get_DragCoeff() const {
  unsigned short val_iZone = ZONE_0;
  unsigned short FinestMesh = config_container[val_iZone]->GetFinestMesh();
  su2double CDrag;

  CDrag = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetTotal_CD();

  return SU2_TYPE::GetValue(CDrag);
}

passivedouble CDriver::Get_LiftCoeff() const {
  unsigned short val_iZone = ZONE_0;
  unsigned short FinestMesh = config_container[val_iZone]->GetFinestMesh();
  su2double CLift;

  CLift = solver_container[val_iZone][INST_0][FinestMesh][FLOW_SOL]->GetTotal_CL();

  return SU2_TYPE::GetValue(CLift);
}

//////////////////////////////////////////////////////////////////////////////////
/* Functions to obtain global parameters from SU2 (time steps, delta t, etc.)   */
//////////////////////////////////////////////////////////////////////////////////

unsigned long CDriver::GetNumberTimeIter() const { return config_container[ZONE_0]->GetnTime_Iter(); }

unsigned long CDriver::GetTimeIter() const { return TimeIter; }

passivedouble CDriver::GetUnsteadyTimeStep() const {
  return SU2_TYPE::GetValue(config_container[ZONE_0]->GetTime_Step());
}

string CDriver::GetSurfaceFileName() const { return config_container[ZONE_0]->GetSurfCoeff_FileName(); }

////////////////////////////////////////////////////////////////////////////////
/* Functions related to the management of markers                             */
////////////////////////////////////////////////////////////////////////////////

vector<string> CDriver::GetCHTMarkerTags() const {
  vector<string> tags;
  const auto nMarker = config_container[ZONE_0]->GetnMarker_All();

  // The CHT markers can be identified as the markers that are customizable with a BC type HEAT_FLUX or ISOTHERMAL.
  for (auto iMarker = 0u; iMarker < nMarker; iMarker++) {
    if ((config_container[ZONE_0]->GetMarker_All_KindBC(iMarker) == HEAT_FLUX ||
         config_container[ZONE_0]->GetMarker_All_KindBC(iMarker) == ISOTHERMAL) &&
        config_container[ZONE_0]->GetMarker_All_PyCustom(iMarker)) {
      tags.push_back(config_container[ZONE_0]->GetMarker_All_TagBound(iMarker));
    }
  }
  return tags;
}

vector<string> CDriver::GetInletMarkerTags() const {
  vector<string> tags;
  const auto nMarker = config_container[ZONE_0]->GetnMarker_All();

  for (auto iMarker = 0u; iMarker < nMarker; iMarker++) {
    bool isCustomizable = config_container[ZONE_0]->GetMarker_All_PyCustom(iMarker);
    bool isInlet = (config_container[ZONE_0]->GetMarker_All_KindBC(iMarker) == INLET_FLOW);

    if (isCustomizable && isInlet) {
      tags.push_back(config_container[ZONE_0]->GetMarker_All_TagBound(iMarker));
    }
  }
  return tags;
}

void CDriver::SetHeatSource_Position(passivedouble alpha, passivedouble pos_x, passivedouble pos_y,
                                     passivedouble pos_z) {
  CSolver* solver = solver_container[ZONE_0][INST_0][MESH_0][RAD_SOL];

  config_container[ZONE_0]->SetHeatSource_Rot_Z(alpha);
  config_container[ZONE_0]->SetHeatSource_Center(pos_x, pos_y, pos_z);

  solver->SetVolumetricHeatSource(geometry_container[ZONE_0][INST_0][MESH_0], config_container[ZONE_0]);
}

void CDriver::SetInlet_Angle(unsigned short iMarker, passivedouble alpha) {
  su2double alpha_rad = alpha * PI_NUMBER / 180.0;

  unsigned long iVertex;

  for (iVertex = 0; iVertex < geometry_container[ZONE_0][INST_0][MESH_0]->nVertex[iMarker]; iVertex++) {
    solver_container[ZONE_0][INST_0][MESH_0][FLOW_SOL]->SetInlet_FlowDir(iMarker, iVertex, 0, cos(alpha_rad));
    solver_container[ZONE_0][INST_0][MESH_0][FLOW_SOL]->SetInlet_FlowDir(iMarker, iVertex, 1, sin(alpha_rad));
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Functions related to simulation control, high level functions (reset convergence, set initial mesh, etc.)   */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CDriver::ResetConvergence() {
  for (auto iZone = 0u; iZone < nZone; iZone++) {
    switch (main_config->GetKind_Solver()) {
      case MAIN_SOLVER::EULER:
      case MAIN_SOLVER::NAVIER_STOKES:
      case MAIN_SOLVER::RANS:
      case MAIN_SOLVER::INC_EULER:
      case MAIN_SOLVER::INC_NAVIER_STOKES:
      case MAIN_SOLVER::INC_RANS:
      case MAIN_SOLVER::NEMO_EULER:
      case MAIN_SOLVER::NEMO_NAVIER_STOKES:
        integration_container[iZone][INST_0][FLOW_SOL]->SetConvergence(false);
        if (config_container[iZone]->GetKind_Solver() == MAIN_SOLVER::RANS)
          integration_container[iZone][INST_0][TURB_SOL]->SetConvergence(false);
        if (config_container[iZone]->GetKind_Trans_Model() == TURB_TRANS_MODEL::LM)
          integration_container[iZone][INST_0][TRANS_SOL]->SetConvergence(false);
        break;

      case MAIN_SOLVER::FEM_ELASTICITY:
        integration_container[iZone][INST_0][FEA_SOL]->SetConvergence(false);
        break;

      case MAIN_SOLVER::ADJ_EULER:
      case MAIN_SOLVER::ADJ_NAVIER_STOKES:
      case MAIN_SOLVER::ADJ_RANS:
      case MAIN_SOLVER::DISC_ADJ_EULER:
      case MAIN_SOLVER::DISC_ADJ_NAVIER_STOKES:
      case MAIN_SOLVER::DISC_ADJ_RANS:
      case MAIN_SOLVER::DISC_ADJ_INC_EULER:
      case MAIN_SOLVER::DISC_ADJ_INC_NAVIER_STOKES:
      case MAIN_SOLVER::DISC_ADJ_INC_RANS:
        integration_container[iZone][INST_0][ADJFLOW_SOL]->SetConvergence(false);
        if ((config_container[iZone]->GetKind_Solver() == MAIN_SOLVER::ADJ_RANS) ||
            (config_container[iZone]->GetKind_Solver() == MAIN_SOLVER::DISC_ADJ_RANS))
          integration_container[iZone][INST_0][ADJTURB_SOL]->SetConvergence(false);
        break;

      default:
        break;
    }
  }
}

void CSinglezoneDriver::SetInitialMesh() {
  DynamicMeshUpdate(0);

  SU2_OMP_PARALLEL {
    for (iMesh = 0u; iMesh <= main_config->GetnMGLevels(); iMesh++) {
      SU2_OMP_FOR_STAT(roundUpDiv(geometry_container[ZONE_0][INST_0][iMesh]->GetnPoint(), omp_get_max_threads()))
      for (auto iPoint = 0ul; iPoint < geometry_container[ZONE_0][INST_0][iMesh]->GetnPoint(); iPoint++) {
        /*--- Overwrite fictitious velocities. ---*/
        su2double Grid_Vel[3] = {0.0, 0.0, 0.0};

        /*--- Set the grid velocity for this coarse node. ---*/
        geometry_container[ZONE_0][INST_0][iMesh]->nodes->SetGridVel(iPoint, Grid_Vel);
      }
      END_SU2_OMP_FOR
      /*--- Push back the volume. ---*/
      geometry_container[ZONE_0][INST_0][iMesh]->nodes->SetVolume_n();
      geometry_container[ZONE_0][INST_0][iMesh]->nodes->SetVolume_nM1();
    }
    /*--- Push back the solution so that there is no fictitious velocity at the next step. ---*/
    solver_container[ZONE_0][INST_0][MESH_0][MESH_SOL]->GetNodes()->Set_Solution_time_n();
    solver_container[ZONE_0][INST_0][MESH_0][MESH_SOL]->GetNodes()->Set_Solution_time_n1();
  }
  END_SU2_OMP_PARALLEL
}

void CDriver::BoundaryConditionsUpdate() {
  int rank = MASTER_NODE;

  SU2_MPI::Comm_rank(SU2_MPI::GetComm(), &rank);

  if (rank == MASTER_NODE) cout << "Updating boundary conditions." << endl;
  for (auto iZone = 0u; iZone < nZone; iZone++) {
    geometry_container[iZone][INST_0][MESH_0]->UpdateCustomBoundaryConditions(geometry_container[iZone][INST_0],config_container[iZone]);
  }
}
