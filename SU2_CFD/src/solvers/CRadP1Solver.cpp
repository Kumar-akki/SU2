/*!
 * \file CRadP1Solver.cpp
 * \brief Main subroutines for solving P1 radiation problems.
 * \author Ruben Sanchez
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
 * Copyright 2012-2018, Francisco D. Palacios, Thomas D. Economon,
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

#include "../../include/solvers/CRadP1Solver.hpp"
#include "../../include/variables/CRadP1Variable.hpp"

CRadP1Solver::CRadP1Solver(void) : CRadSolver() {

  FlowPrimVar_i = NULL;
  FlowPrimVar_j = NULL;

}

CRadP1Solver::CRadP1Solver(CGeometry* geometry, CConfig *config) : CRadSolver(geometry, config) {

  unsigned long iPoint;
  unsigned short iVar, iDim;
  unsigned short direct_diff = config->GetDirectDiff();

  nDim =          geometry->GetnDim();
  nPoint =        geometry->GetnPoint();
  nPointDomain =  geometry->GetnPointDomain();
  nVar =          1;
  node =          new CVariable*[nPoint];

  /*--- Initialize nVarGrad for deallocation ---*/

  nVarGrad = nVar;

  Residual = new su2double[nVar]; Residual_RMS = new su2double[nVar];
  Solution = new su2double[nVar]; Residual_Max = new su2double[nVar];

  Res_Visc = new su2double[nVar];

  /*--- Define some structures for locating max residuals ---*/

  Point_Max = new unsigned long[nVar];
  for (iVar = 0; iVar < nVar; iVar++) Point_Max[iVar] = 0;
  Point_Max_Coord = new su2double*[nVar];
  for (iVar = 0; iVar < nVar; iVar++) {
    Point_Max_Coord[iVar] = new su2double[nDim];
    for (iDim = 0; iDim < nDim; iDim++) Point_Max_Coord[iVar][iDim] = 0.0;
  }

  /*--- Jacobians and vector structures for implicit computations ---*/

  if (config->GetKind_TimeIntScheme_Radiation() == EULER_IMPLICIT) {

    Jacobian_i = new su2double* [nVar];
    Jacobian_j = new su2double* [nVar];
    for (iVar = 0; iVar < nVar; iVar++) {
      Jacobian_i[iVar] = new su2double [nVar];
      Jacobian_j[iVar] = new su2double [nVar];
    }

    if (rank == MASTER_NODE) cout << "Initialize Jacobian structure (P1 radiation equation)." << endl;
    Jacobian.Initialize(nPoint, nPointDomain, nVar, nVar, true, geometry, config);

  }

  /*--- Solution and residual vectors ---*/

  LinSysSol.Initialize(nPoint, nPointDomain, nVar, 0.0);
  LinSysRes.Initialize(nPoint, nPointDomain, nVar, 0.0);
  LinSysAux.Initialize(nPoint, nPointDomain, nVar, 0.0);

  /*--- Read farfield conditions from config ---*/
  Temperature_Inf = config->GetTemperature_FreeStreamND();

  /*--- Initialize the secondary values for direct derivative approxiations ---*/

  switch(direct_diff){
    case NO_DERIVATIVE: case D_DENSITY:
    case D_PRESSURE: case D_VISCOSITY:
    case D_MACH: case D_AOA:
    case D_SIDESLIP: case D_REYNOLDS:
    case D_TURB2LAM: case D_DESIGN:
      /*--- Not necessary here ---*/
      break;
    case D_TEMPERATURE:
      SU2_TYPE::SetDerivative(Temperature_Inf, 1.0);
      break;
    default:
      break;
  }

  SetTemperature_Inf(Temperature_Inf);

  /*--- Define some auxiliary vectors for computing flow variable
   gradients by least squares, S matrix := inv(R)*traspose(inv(R)),
   c vector := transpose(WA)*(Wb) ---*/

  if (config->GetKind_Gradient_Method() == WEIGHTED_LEAST_SQUARES) {

    Smatrix = new su2double* [nDim];
    for (iDim = 0; iDim < nDim; iDim++)
      Smatrix[iDim] = new su2double [nDim];

  }

  /*--- Always instantiate and initialize the variable to a zero value. ---*/
  su2double init_val;
  switch(config->GetKind_P1_Init()){
    case P1_INIT_ZERO: init_val = 0.0; break;
    case P1_INIT_TEMP: init_val = 4.0*STEFAN_BOLTZMANN*pow(config->GetInc_Temperature_Init(),4.0); break;
    default: init_val = 0.0; break;
  }

  for (iPoint = 0; iPoint < nPoint; iPoint++)
    node[iPoint] = new CRadP1Variable(init_val, nDim, nVar, config);

}

CRadP1Solver::~CRadP1Solver(void) {

  if (FlowPrimVar_i != NULL) delete [] FlowPrimVar_i;
  if (FlowPrimVar_j != NULL) delete [] FlowPrimVar_j;

}

void CRadP1Solver::Preprocessing(CGeometry *geometry, CSolver **solver_container, CConfig *config, unsigned short iMesh, unsigned short iRKStep, unsigned short RunTime_EqSystem, bool Output) {

  unsigned long iPoint;

  /*--- Initialize the residual vector ---*/
  for (iPoint = 0; iPoint < nPoint; iPoint ++) {
    LinSysRes.SetBlock_Zero(iPoint);
  }

  /*--- Initialize the Jacobian matrix ---*/
  Jacobian.SetValZero();

  /*--- Compute the Solution gradients ---*/
  if (config->GetKind_Gradient_Method() == GREEN_GAUSS) SetSolution_Gradient_GG(geometry, config);
  if (config->GetKind_Gradient_Method() == WEIGHTED_LEAST_SQUARES) SetSolution_Gradient_LS(geometry, config);

}

void CRadP1Solver::Postprocessing(CGeometry *geometry, CSolver **solver_container, CConfig *config, unsigned short iMesh) {

  unsigned long iPoint;
  su2double Energy, Temperature;
  su2double SourceTerm, SourceTerm_Derivative;

  for (iPoint = 0; iPoint < nPointDomain; iPoint++) {

    /*--- Retrieve the radiative energy ---*/
    Energy = node[iPoint]->GetSolution(0);

    /*--- Retrieve temperature from the flow solver ---*/
    Temperature = solver_container[FLOW_SOL]->node[iPoint]->GetPrimitive()[nDim+1];

    /*--- Compute the divergence of the radiative flux ---*/
    SourceTerm = Absorption_Coeff*(Energy - 4.0*STEFAN_BOLTZMANN*pow(Temperature,4.0));

    /*--- Compute the derivative of the source term with respect to the temperature ---*/
    SourceTerm_Derivative =  - 16.0*Absorption_Coeff*STEFAN_BOLTZMANN*pow(Temperature,3.0);

    /*--- Store the source term and its derivative ---*/
    node[iPoint]->SetRadiative_SourceTerm(0, SourceTerm);
    node[iPoint]->SetRadiative_SourceTerm(1, SourceTerm_Derivative);

  }

}

void CRadP1Solver::Viscous_Residual(CGeometry *geometry, CSolver **solver_container, CNumerics *numerics,
                                   CConfig *config, unsigned short iMesh, unsigned short iRKStep) {
  unsigned long iEdge, iPoint, jPoint;

  for (iEdge = 0; iEdge < geometry->GetnEdge(); iEdge++) {

    /*--- Points in edge ---*/

    iPoint = geometry->edge[iEdge]->GetNode(0);
    jPoint = geometry->edge[iEdge]->GetNode(1);

    /*--- Points coordinates, and normal vector ---*/

    numerics->SetCoord(geometry->node[iPoint]->GetCoord(),
                       geometry->node[jPoint]->GetCoord());
    numerics->SetNormal(geometry->edge[iEdge]->GetNormal());

    /*--- Radiation variables w/o reconstruction, and its gradients ---*/

    numerics->SetRadVar(node[iPoint]->GetSolution(), node[jPoint]->GetSolution());
    numerics->SetRadVarGradient(node[iPoint]->GetGradient(), node[jPoint]->GetGradient());

    /*--- Compute residual, and Jacobians ---*/

    numerics->ComputeResidual(Residual, Jacobian_i, Jacobian_j, config);

    /*--- Add and subtract residual, and update Jacobians ---*/

    LinSysRes.SubtractBlock(iPoint, Residual);
    LinSysRes.AddBlock(jPoint, Residual);

    Jacobian.SubtractBlock(iPoint, iPoint, Jacobian_i);
    Jacobian.SubtractBlock(iPoint, jPoint, Jacobian_j);
    Jacobian.AddBlock(jPoint, iPoint, Jacobian_i);
    Jacobian.AddBlock(jPoint, jPoint, Jacobian_j);

  }

}

void CRadP1Solver::Source_Residual(CGeometry *geometry, CSolver **solver_container, CNumerics *numerics, CNumerics *second_numerics,
                                    CConfig *config, unsigned short iMesh) {
  unsigned long iPoint;

  for (iPoint = 0; iPoint < nPointDomain; iPoint++) {

    /*--- Conservative variables w/o reconstruction ---*/

    numerics->SetPrimitive(solver_container[FLOW_SOL]->node[iPoint]->GetPrimitive(), NULL);

    /*--- Radiation variables w/o reconstruction ---*/

    numerics->SetRadVar(node[iPoint]->GetSolution(), NULL);

    /*--- Set volume ---*/

    numerics->SetVolume(geometry->node[iPoint]->GetVolume());

    /*--- Compute the source term ---*/

    numerics->ComputeResidual(Residual, Jacobian_i, config);

    /*--- Subtract residual and the Jacobian ---*/

    LinSysRes.SubtractBlock(iPoint, Residual);
    Jacobian.SubtractBlock(iPoint, iPoint, Jacobian_i);

  }

}

void CRadP1Solver::BC_HeatFlux_Wall(CGeometry *geometry, CSolver **solver_container, CNumerics *conv_numerics, CNumerics *visc_numerics, CConfig *config, unsigned short val_marker) {

  unsigned short iDim, iVar, jVar;
  unsigned long iVertex, iPoint;

  su2double Theta, Ib_w, Temperature, Radiative_Energy;
  su2double *Normal, Area, Wall_Emissivity;
  su2double Radiative_Heat_Flux;
  su2double *Unit_Normal;

  Unit_Normal = new su2double[nDim];

  bool implicit      = (config->GetKind_TimeIntScheme_Flow() == EULER_IMPLICIT);
  /*--- Identify the boundary by string name ---*/
  string Marker_Tag = config->GetMarker_All_TagBound(val_marker);

  /*--- Get the specified wall emissivity from config ---*/
  Wall_Emissivity = config->GetWall_Emissivity(Marker_Tag);

  /*--- Compute the constant for the wall theta ---*/
  Theta = Wall_Emissivity / (2.0*(2.0 - Wall_Emissivity));

  /*--- Loop over all of the vertices on this boundary marker ---*/

  for (iVertex = 0; iVertex < geometry->nVertex[val_marker]; iVertex++) {
    iPoint = geometry->vertex[val_marker][iVertex]->GetNode();

    /*--- Check if the node belongs to the domain (i.e, not a halo node) ---*/

    if (geometry->node[iPoint]->GetDomain()) {

      /*--- Compute dual-grid area and boundary normal ---*/
      Normal = geometry->vertex[val_marker][iVertex]->GetNormal();

      Area = 0.0;
      for (iDim = 0; iDim < nDim; iDim++)
        Area += Normal[iDim]*Normal[iDim];
      Area = sqrt (Area);

      // Weak application of the boundary condition

      /*--- Initialize the viscous residuals to zero ---*/
      for (iVar = 0; iVar < nVar; iVar++) {
        Res_Visc[iVar] = 0.0;
        if (implicit) {
          for (jVar = 0; jVar < nVar; jVar++)
            Jacobian_i[iVar][jVar] = 0.0;
        }
      }

      /*--- Apply a weak boundary condition for the radiative transfer equation. ---*/

      /*--- Retrieve temperature from the flow solver ---*/
      Temperature = solver_container[FLOW_SOL]->node[iPoint]->GetPrimitive()[nDim+1];

      /*--- Compute the blackbody intensity at the wall. ---*/
      Ib_w = 4.0*STEFAN_BOLTZMANN*pow(Temperature,4.0);

      /*--- Compute the radiative heat flux. ---*/
      Radiative_Energy = node[iPoint]->GetSolution(0);
      Radiative_Heat_Flux = Theta*(Ib_w - Radiative_Energy);

      /*--- Compute the Viscous contribution to the residual ---*/
      Res_Visc[0] = Radiative_Heat_Flux*Area;

      /*--- Apply to the residual vector ---*/
      LinSysRes.SubtractBlock(iPoint, Res_Visc);

      /*--- Compute the Jacobian contribution. ---*/
      if (implicit) {
        Jacobian_i[0][0] = - Theta;
        Jacobian.SubtractBlock(iPoint, iPoint, Jacobian_i);
      }

    }
  }

}

void CRadP1Solver::BC_Isothermal_Wall(CGeometry *geometry, CSolver **solver_container, CNumerics *conv_numerics, CNumerics *visc_numerics, CConfig *config,
                                       unsigned short val_marker) {

  unsigned short iDim, iVar, jVar;
  unsigned long iVertex, iPoint;

  su2double Theta, Ib_w, Radiative_Energy;
  su2double *Normal, *Unit_Normal, Area, Wall_Emissivity;
  su2double Radiative_Heat_Flux;
  su2double Twall;

  Unit_Normal = new su2double[nDim];

  bool implicit      = (config->GetKind_TimeIntScheme_Flow() == EULER_IMPLICIT);

  /*--- Identify the boundary by string name ---*/
  string Marker_Tag = config->GetMarker_All_TagBound(val_marker);

  /*--- Get the specified wall emissivity from config ---*/
  Wall_Emissivity = config->GetWall_Emissivity(Marker_Tag);

  /*--- Compute the constant for the wall theta ---*/
  Theta = Wall_Emissivity / (2.0*(2.0 - Wall_Emissivity));

    /*--- Retrieve the specified wall temperature ---*/
  Twall = config->GetIsothermal_Temperature(Marker_Tag)/config->GetTemperature_Ref();

  /*--- Loop over all of the vertices on this boundary marker ---*/

  for (iVertex = 0; iVertex < geometry->nVertex[val_marker]; iVertex++) {
    iPoint = geometry->vertex[val_marker][iVertex]->GetNode();

    /*--- Check if the node belongs to the domain (i.e, not a halo node) ---*/

    if (geometry->node[iPoint]->GetDomain()) {

      /*--- Compute dual-grid area and boundary normal ---*/
      Normal = geometry->vertex[val_marker][iVertex]->GetNormal();

      Area = 0.0;
      for (iDim = 0; iDim < nDim; iDim++)
        Area += Normal[iDim]*Normal[iDim];
      Area = sqrt (Area);

      // Weak application of the boundary condition

      /*--- Initialize the viscous residuals to zero ---*/
      for (iVar = 0; iVar < nVar; iVar++) {
        Res_Visc[iVar] = 0.0;
        if (implicit) {
          for (jVar = 0; jVar < nVar; jVar++)
            Jacobian_i[iVar][jVar] = 0.0;
        }
      }

      /*--- Apply a weak boundary condition for the radiative transfer equation. ---*/

      /*--- Compute the blackbody intensity at the wall. ---*/
      Ib_w = 4.0*STEFAN_BOLTZMANN*pow(Twall,4.0);

      /*--- Compute the radiative heat flux. ---*/
      Radiative_Energy = node[iPoint]->GetSolution(0);
      Radiative_Heat_Flux = 1.0*Theta*(Ib_w - Radiative_Energy);

      /*--- Compute the Viscous contribution to the residual ---*/
      Res_Visc[0] = Radiative_Heat_Flux*Area;

      /*--- Apply to the residual vector ---*/
      LinSysRes.SubtractBlock(iPoint, Res_Visc);

      /*--- Compute the Jacobian contribution. ---*/
      if (implicit) {
        Jacobian_i[0][0] = - Theta;
        Jacobian.SubtractBlock(iPoint, iPoint, Jacobian_i);
      }
    }
  }

}

void CRadP1Solver::BC_Far_Field(CGeometry *geometry, CSolver **solver_container, CNumerics *conv_numerics, CNumerics *visc_numerics, CConfig *config, unsigned short val_marker) {

  unsigned short iDim, iVar, jVar;
  unsigned long iVertex, iPoint;

  su2double Theta, Ib_w, Radiative_Energy;
  su2double *Normal, *Unit_Normal, Area, Wall_Emissivity;
  su2double Radiative_Heat_Flux;
  su2double Twall;

  Unit_Normal = new su2double[nDim];

  bool implicit      = (config->GetKind_TimeIntScheme_Flow() == EULER_IMPLICIT);

  /*--- Identify the boundary by string name ---*/
  string Marker_Tag = config->GetMarker_All_TagBound(val_marker);

  /*--- Get the specified wall emissivity from config ---*/
  Wall_Emissivity = config->GetWall_Emissivity(Marker_Tag);

  /*--- Compute the constant for the wall theta ---*/
  Theta = Wall_Emissivity / (2.0*(2.0 - Wall_Emissivity));

  /*--- Retrieve the specified wall temperature ---*/
  Twall = GetTemperature_Inf();

  /*--- Loop over all of the vertices on this boundary marker ---*/

  for (iVertex = 0; iVertex < geometry->nVertex[val_marker]; iVertex++) {
    iPoint = geometry->vertex[val_marker][iVertex]->GetNode();

    /*--- Check if the node belongs to the domain (i.e, not a halo node) ---*/

    if (geometry->node[iPoint]->GetDomain()) {

      /*--- Compute dual-grid area and boundary normal ---*/
      Normal = geometry->vertex[val_marker][iVertex]->GetNormal();

      Area = 0.0;
      for (iDim = 0; iDim < nDim; iDim++)
        Area += Normal[iDim]*Normal[iDim];
      Area = sqrt (Area);

      // Weak application of the boundary condition

      /*--- Initialize the viscous residuals to zero ---*/
      for (iVar = 0; iVar < nVar; iVar++) {
        Res_Visc[iVar] = 0.0;
        if (implicit) {
          for (jVar = 0; jVar < nVar; jVar++)
            Jacobian_i[iVar][jVar] = 0.0;
        }
      }

      /*--- Apply a weak boundary condition for the radiative transfer equation. ---*/

      /*--- Compute the blackbody intensity at the wall. ---*/
      Ib_w = 4.0*STEFAN_BOLTZMANN*pow(Twall,4.0);

      /*--- Compute the radiative heat flux. ---*/
      Radiative_Energy = node[iPoint]->GetSolution(0);
      Radiative_Heat_Flux = 1.0*Theta*(Ib_w - Radiative_Energy);

      /*--- Compute the Viscous contribution to the residual ---*/
      Res_Visc[0] = Radiative_Heat_Flux*Area;

      /*--- Apply to the residual vector ---*/
      LinSysRes.SubtractBlock(iPoint, Res_Visc);

      /*--- Compute the Jacobian contribution. ---*/
      if (implicit) {
        Jacobian_i[0][0] = - Theta;
        Jacobian.SubtractBlock(iPoint, iPoint, Jacobian_i);
      }
    }
  }

}

void CRadP1Solver::BC_Inlet(CGeometry *geometry, CSolver **solver_container, CNumerics *conv_numerics, CNumerics *visc_numerics, CConfig *config, unsigned short val_marker) {

}

void CRadP1Solver::BC_Outlet(CGeometry *geometry, CSolver **solver_container, CNumerics *conv_numerics, CNumerics *visc_numerics,
                              CConfig *config, unsigned short val_marker) {

}

void CRadP1Solver::BC_Euler_Wall(CGeometry *geometry, CSolver **solver_container,
                                CNumerics *numerics, CConfig *config, unsigned short val_marker) {

  /*--- Convective fluxes across euler wall are equal to zero. ---*/

}

void CRadP1Solver::ImplicitEuler_Iteration(CGeometry *geometry, CSolver **solver_container, CConfig *config) {

  unsigned short iVar;
  unsigned long iPoint, total_index, IterLinSol = 0;
  su2double Vol;
  su2double Delta_time = 0.01, Delta;

  /*--- Set maximum residual to zero ---*/

  for (iVar = 0; iVar < nVar; iVar++) {
    SetRes_RMS(iVar, 0.0);
    SetRes_Max(iVar, 0.0, 0);
  }
  /*--- Build implicit system ---*/

  for (iPoint = 0; iPoint < nPointDomain; iPoint++) {

    /*--- Read the volume ---*/

    Vol = geometry->node[iPoint]->GetVolume();

    /*--- Modify matrix diagonal to assure diagonal dominance ---*/

    if (node[iPoint]->GetDelta_Time() != 0.0) {
      Delta = Vol / node[iPoint]->GetDelta_Time();
      Jacobian.AddVal2Diag(iPoint, Delta);
    }
    else {
      Jacobian.SetVal2Diag(iPoint, 1.0);
      for (iVar = 0; iVar < nVar; iVar++) {
        total_index = iPoint*nVar + iVar;
        LinSysRes[total_index] = 0.0;
      }
    }

    /*--- Right hand side of the system (-Residual) and initial guess (x = 0) ---*/

    for (iVar = 0; iVar < nVar; iVar++) {
      total_index = iPoint*nVar+iVar;
      LinSysRes[total_index] = - (LinSysRes[total_index]);
      LinSysSol[total_index] = 0.0;
      AddRes_RMS(iVar, LinSysRes[total_index]*LinSysRes[total_index]);
      AddRes_Max(iVar, fabs(LinSysRes[total_index]), geometry->node[iPoint]->GetGlobalIndex(), geometry->node[iPoint]->GetCoord());
    }
  }

  /*--- Initialize residual and solution at the ghost points ---*/

  for (iPoint = nPointDomain; iPoint < nPoint; iPoint++) {
    for (iVar = 0; iVar < nVar; iVar++) {
      total_index = iPoint*nVar + iVar;
      LinSysRes[total_index] = 0.0;
      LinSysSol[total_index] = 0.0;
    }
  }

  /*--- Solve or smooth the linear system ---*/

  IterLinSol = System.Solve(Jacobian, LinSysRes, LinSysSol, geometry, config);

  for (iPoint = 0; iPoint < nPointDomain; iPoint++) {
    for (iVar = 0; iVar < nVar; iVar++) {
      node[iPoint]->AddSolution(iVar, LinSysSol[iPoint*nVar+iVar]);
    }
  }

  /*--- The the number of iterations of the linear solver ---*/

  SetIterLinSolver(IterLinSol);

  /*--- MPI solution ---*/

  InitiateComms(geometry, config, SOLUTION);
  CompleteComms(geometry, config, SOLUTION);

  /*--- Compute the root mean square residual ---*/

  SetResidual_RMS(geometry, config);

}

void CRadP1Solver::SetTime_Step(CGeometry *geometry, CSolver **solver_container, CConfig *config,
                               unsigned short iMesh, unsigned long Iteration) {

  unsigned short iDim, iMarker;
  unsigned long iEdge, iVertex, iPoint = 0, jPoint = 0;
  su2double *Normal, Area, Vol, Lambda;
  su2double Global_Delta_Time = 1E6, Global_Delta_UnstTimeND = 0.0, Local_Delta_Time = 0.0, K_v = 0.25;
  su2double CFL = config->GetCFL_Rad();
  su2double GammaP1 = 1.0 / (3.0*(Absorption_Coeff + Scattering_Coeff));

  bool dual_time = ((config->GetTime_Marching() == DT_STEPPING_1ST) ||
                    (config->GetTime_Marching() == DT_STEPPING_2ND));

  bool implicit = (config->GetKind_TimeIntScheme_Flow() == EULER_IMPLICIT);

  /*--- Compute spectral radius based on thermal conductivity ---*/

  Min_Delta_Time = 1.E6; Max_Delta_Time = 0.0;

  for (iPoint = 0; iPoint < nPointDomain; iPoint++) {
    node[iPoint]->SetMax_Lambda_Visc(0.0);
  }

  /*--- Loop interior edges ---*/

  for (iEdge = 0; iEdge < geometry->GetnEdge(); iEdge++) {

    iPoint = geometry->edge[iEdge]->GetNode(0);
    jPoint = geometry->edge[iEdge]->GetNode(1);

    /*--- Get the edge's normal vector to compute the edge's area ---*/
    Normal = geometry->edge[iEdge]->GetNormal();
    Area = 0; for (iDim = 0; iDim < nDim; iDim++) Area += Normal[iDim]*Normal[iDim]; Area = sqrt(Area);

    /*--- Viscous contribution ---*/

    Lambda = GammaP1*Area*Area;
    if (geometry->node[iPoint]->GetDomain()) node[iPoint]->AddMax_Lambda_Visc(Lambda);
    if (geometry->node[jPoint]->GetDomain()) node[jPoint]->AddMax_Lambda_Visc(Lambda);

  }

  /*--- Loop boundary edges ---*/

  for (iMarker = 0; iMarker < geometry->GetnMarker(); iMarker++) {
    for (iVertex = 0; iVertex < geometry->GetnVertex(iMarker); iVertex++) {

      /*--- Point identification, Normal vector and area ---*/

      iPoint = geometry->vertex[iMarker][iVertex]->GetNode();
      Normal = geometry->vertex[iMarker][iVertex]->GetNormal();
      Area = 0.0; for (iDim = 0; iDim < nDim; iDim++) Area += Normal[iDim]*Normal[iDim]; Area = sqrt(Area);

      /*--- Viscous contribution ---*/

      Lambda = GammaP1*Area*Area;
      if (geometry->node[iPoint]->GetDomain()) node[iPoint]->AddMax_Lambda_Visc(Lambda);

    }
  }

  /*--- Each element uses their own speed, steady state simulation ---*/

  for (iPoint = 0; iPoint < nPointDomain; iPoint++) {

    Vol = geometry->node[iPoint]->GetVolume();

    if (Vol != 0.0) {

      /*--- Time step setting method ---*/

       Local_Delta_Time = CFL*K_v*Vol*Vol/ node[iPoint]->GetMax_Lambda_Visc();

      /*--- Min-Max-Logic ---*/

      Global_Delta_Time = min(Global_Delta_Time, Local_Delta_Time);
      Min_Delta_Time = min(Min_Delta_Time, Local_Delta_Time);
      Max_Delta_Time = max(Max_Delta_Time, Local_Delta_Time);
      if (Local_Delta_Time > config->GetMax_DeltaTime())
        Local_Delta_Time = config->GetMax_DeltaTime();

      node[iPoint]->SetDelta_Time(Local_Delta_Time);
    }
    else {
      node[iPoint]->SetDelta_Time(0.0);
    }
  }

  /*--- Compute the max and the min dt (in parallel) ---*/
  if (config->GetComm_Level() == COMM_FULL) {
#ifdef HAVE_MPI
    su2double rbuf_time, sbuf_time;
    sbuf_time = Min_Delta_Time;
    SU2_MPI::Reduce(&sbuf_time, &rbuf_time, 1, MPI_DOUBLE, MPI_MIN, MASTER_NODE, MPI_COMM_WORLD);
    SU2_MPI::Bcast(&rbuf_time, 1, MPI_DOUBLE, MASTER_NODE, MPI_COMM_WORLD);
    Min_Delta_Time = rbuf_time;

    sbuf_time = Max_Delta_Time;
    SU2_MPI::Reduce(&sbuf_time, &rbuf_time, 1, MPI_DOUBLE, MPI_MAX, MASTER_NODE, MPI_COMM_WORLD);
    SU2_MPI::Bcast(&rbuf_time, 1, MPI_DOUBLE, MASTER_NODE, MPI_COMM_WORLD);
    Max_Delta_Time = rbuf_time;
#endif
  }

  /*--- For exact time solution use the minimum delta time of the whole mesh ---*/
  if (config->GetTime_Marching() == TIME_STEPPING) {
#ifdef HAVE_MPI
    su2double rbuf_time, sbuf_time;
    sbuf_time = Global_Delta_Time;
    SU2_MPI::Reduce(&sbuf_time, &rbuf_time, 1, MPI_DOUBLE, MPI_MIN, MASTER_NODE, MPI_COMM_WORLD);
    SU2_MPI::Bcast(&rbuf_time, 1, MPI_DOUBLE, MASTER_NODE, MPI_COMM_WORLD);
    Global_Delta_Time = rbuf_time;
#endif
    for (iPoint = 0; iPoint < nPointDomain; iPoint++)
      node[iPoint]->SetDelta_Time(Global_Delta_Time);
  }

  /*--- Recompute the unsteady time step for the dual time strategy
   if the unsteady CFL is diferent from 0 ---*/
  if ((dual_time) && (Iteration == 0) && (config->GetUnst_CFL() != 0.0) && (iMesh == MESH_0)) {
    Global_Delta_UnstTimeND = config->GetUnst_CFL()*Global_Delta_Time/config->GetCFL(iMesh);

#ifdef HAVE_MPI
    su2double rbuf_time, sbuf_time;
    sbuf_time = Global_Delta_UnstTimeND;
    SU2_MPI::Reduce(&sbuf_time, &rbuf_time, 1, MPI_DOUBLE, MPI_MIN, MASTER_NODE, MPI_COMM_WORLD);
    SU2_MPI::Bcast(&rbuf_time, 1, MPI_DOUBLE, MASTER_NODE, MPI_COMM_WORLD);
    Global_Delta_UnstTimeND = rbuf_time;
#endif
    config->SetDelta_UnstTimeND(Global_Delta_UnstTimeND);
  }

  /*--- The pseudo local time (explicit integration) cannot be greater than the physical time ---*/
  if (dual_time)
    for (iPoint = 0; iPoint < nPointDomain; iPoint++) {
      if (!implicit) {
        cout << "Using unsteady time: " << config->GetDelta_UnstTimeND() << endl;
        Local_Delta_Time = min((2.0/3.0)*config->GetDelta_UnstTimeND(), node[iPoint]->GetDelta_Time());
        node[iPoint]->SetDelta_Time(Local_Delta_Time);
      }
  }
}
