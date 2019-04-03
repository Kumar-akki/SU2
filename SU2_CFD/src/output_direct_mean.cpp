/*!
 * \file output_direct_mean.cpp
 * \brief Main subroutines for compressible flow output
 * \author R. Sanchez
 * \version 6.0.1 "Falcon"
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


#include "../include/output_structure.hpp"

CFlowOutput::CFlowOutput(CConfig *config, CGeometry *geometry, CSolver **solver, unsigned short val_iZone) : COutput(config) {

  nDim = geometry->GetnDim();  
  
  turb_model = config->GetKind_Turb_Model();
  
  grid_movement = config->GetGrid_Movement(); 
  
  su2double Gas_Constant, Mach2Vel, Mach_Motion;
  unsigned short iDim;
  su2double Gamma = config->GetGamma();
      
  /*--- Set the non-dimensionalization for coefficients. ---*/
  
  RefArea = config->GetRefArea();
  
  if (grid_movement) {
    Gas_Constant = config->GetGas_ConstantND();
    Mach2Vel = sqrt(Gamma*Gas_Constant*config->GetTemperature_FreeStreamND());
    Mach_Motion = config->GetMach_Motion();
    RefVel2 = (Mach_Motion*Mach2Vel)*(Mach_Motion*Mach2Vel);
  }
  else {
    RefVel2 = 0.0;
    for (iDim = 0; iDim < nDim; iDim++)
      RefVel2  += solver[FLOW_SOL]->GetVelocity_Inf(iDim)*solver[FLOW_SOL]->GetVelocity_Inf(iDim);
  }
  RefDensity  = solver[FLOW_SOL]->GetDensity_Inf();
  RefPressure = solver[FLOW_SOL]->GetPressure_Inf();
  factor = 1.0 / (0.5*RefDensity*RefArea*RefVel2);
  
  /*--- Set the default history fields if nothing is set in the config file ---*/
  
  if (nRequestedHistoryFields == 0){
    RequestedHistoryFields.push_back("ITER");
    RequestedHistoryFields.push_back("RMS_RES");
    nRequestedHistoryFields = RequestedHistoryFields.size();
  }
  if (nRequestedScreenFields == 0){
    if (config->GetTime_Domain()) RequestedScreenFields.push_back("TIME_ITER");
    if (multizone) RequestedScreenFields.push_back("OUTER_ITER");
    RequestedScreenFields.push_back("INNER_ITER");
    RequestedScreenFields.push_back("RMS_DENSITY");
    RequestedScreenFields.push_back("RMS_MOMENTUM-X");
    RequestedScreenFields.push_back("RMS_MOMENTUM-Y");
    RequestedScreenFields.push_back("RMS_ENERGY");
    nRequestedScreenFields = RequestedScreenFields.size();
  }
  if (nRequestedVolumeFields == 0){
    RequestedVolumeFields.push_back("COORDINATES");
    RequestedVolumeFields.push_back("SOLUTION");
    RequestedVolumeFields.push_back("PRIMITIVE");
    nRequestedVolumeFields = RequestedVolumeFields.size();
  }
  
  stringstream ss;
  ss << "Zone " << config->GetiZone() << " (Comp. Fluid)";
  MultiZoneHeaderString = ss.str();
  
}

CFlowOutput::~CFlowOutput(void) {

  if (rank == MASTER_NODE){
    HistFile.close();

  }


}



void CFlowOutput::SetHistoryOutputFields(CConfig *config){
  
  /// BEGIN_GROUP: ITERATION, DESCRIPTION: Iteration identifier.
  /// DESCRIPTION: The time iteration index.
  AddHistoryOutput("TIME_ITER",     "Time_Iter",  FORMAT_INTEGER, "ITER"); 
  /// DESCRIPTION: The outer iteration index.
  AddHistoryOutput("OUTER_ITER",   "Outer_Iter",  FORMAT_INTEGER, "ITER"); 
  /// DESCRIPTION: The inner iteration index.
  AddHistoryOutput("INNER_ITER",   "Inner_Iter", FORMAT_INTEGER,  "ITER"); 
  /// END_GROUP
  
  /// DESCRIPTION: Currently used wall-clock time.
  AddHistoryOutput("PHYS_TIME",   "Time(min)", FORMAT_SCIENTIFIC, "PHYS_TIME"); 

  /// BEGIN_GROUP: RMS_RES, DESCRIPTION: The root-mean-square residuals of the SOLUTION variables. 
  /// DESCRIPTION: Root-mean square residual of the density.
  AddHistoryOutput("RMS_DENSITY",    "rms[Rho]",  FORMAT_FIXED,   "RMS_RES", TYPE_RESIDUAL);
  /// DESCRIPTION: Root-mean square residual of the momentum x-component.
  AddHistoryOutput("RMS_MOMENTUM-X", "rms[RhoU]", FORMAT_FIXED,   "RMS_RES", TYPE_RESIDUAL);
  /// DESCRIPTION: Root-mean square residual of the momentum y-component.
  AddHistoryOutput("RMS_MOMENTUM-Y", "rms[RhoV]", FORMAT_FIXED,   "RMS_RES", TYPE_RESIDUAL);
  /// DESCRIPTION: Root-mean square residual of the momentum z-component.
  if (nDim == 3) AddHistoryOutput("RMS_MOMENTUM-Z", "rms[RhoW]", FORMAT_FIXED,   "RMS_RES", TYPE_RESIDUAL);
  /// DESCRIPTION: Root-mean square residual of the energy.
  AddHistoryOutput("RMS_ENERGY",     "rms[RhoE]", FORMAT_FIXED,   "RMS_RES", TYPE_RESIDUAL);
  
  switch(turb_model){
  case SA: case SA_NEG: case SA_E: case SA_COMP: case SA_E_COMP:
    /// DESCRIPTION: Root-mean square residual of nu tilde (SA model).  
    AddHistoryOutput("RMS_NU_TILDE",       "rms[nu]", FORMAT_FIXED, "RMS_RES", TYPE_RESIDUAL);
    break;  
  case SST:
    /// DESCRIPTION: Root-mean square residual of kinetic energy (SST model).    
    AddHistoryOutput("RMS_KINETIC_ENERGY", "rms[k]",  FORMAT_FIXED, "RMS_RES", TYPE_RESIDUAL);
    /// DESCRIPTION: Root-mean square residual of the dissipation (SST model).    
    AddHistoryOutput("RMS_DISSIPATION",    "rms[w]",  FORMAT_FIXED, "RMS_RES", TYPE_RESIDUAL);
    break;
  default: break;
  }
  /// END_GROUP
   
  /// BEGIN_GROUP: MAX_RES, DESCRIPTION: The maximum residuals of the SOLUTION variables. 
  /// DESCRIPTION: Maximum residual of the density.
  AddHistoryOutput("MAX_DENSITY",    "max[Rho]",  FORMAT_FIXED,   "MAX_RES", TYPE_RESIDUAL);
  /// DESCRIPTION: Maximum residual of the momentum x-component. 
  AddHistoryOutput("MAX_MOMENTUM-X", "max[RhoU]", FORMAT_FIXED,   "MAX_RES", TYPE_RESIDUAL);
  /// DESCRIPTION: Maximum residual of the momentum y-component. 
  AddHistoryOutput("MAX_MOMENTUM-Y", "max[RhoV]", FORMAT_FIXED,   "MAX_RES", TYPE_RESIDUAL);
  /// DESCRIPTION: Maximum residual of the momentum z-component. 
  if (nDim == 3) AddHistoryOutput("MAX_MOMENTUM-Z", "max[RhoW]", FORMAT_FIXED,   "MAX_RES", TYPE_RESIDUAL);
  /// DESCRIPTION: Maximum residual of the energy.  
  AddHistoryOutput("MAX_ENERGY",     "max[RhoE]", FORMAT_FIXED,   "MAX_RES", TYPE_RESIDUAL);
  
  switch(turb_model){
  case SA: case SA_NEG: case SA_E: case SA_COMP: case SA_E_COMP:
    /// DESCRIPTION: Maximum residual of nu tilde (SA model).
    AddHistoryOutput("MAX_NU_TILDE",       "max[nu]", FORMAT_FIXED, "MAX_RES", TYPE_RESIDUAL);
    break;  
  case SST:
    /// DESCRIPTION: Maximum residual of kinetic energy (SST model). 
    AddHistoryOutput("MAX_KINETIC_ENERGY", "max[k]",  FORMAT_FIXED, "MAX_RES", TYPE_RESIDUAL);
    /// DESCRIPTION: Maximum residual of the dissipation (SST model).   
    AddHistoryOutput("MAX_DISSIPATION",    "max[w]",  FORMAT_FIXED, "MAX_RES", TYPE_RESIDUAL); 
    break;
  default: break;
  }
  /// END_GROUP
  
  /// BEGIN_GROUP: AERO_COEFF, DESCRIPTION: Sum of the aerodynamic coefficients and forces on all surfaces (markers) set with MARKER_MONITORING.
  /// DESCRIPTION: Drag coefficient 
  AddHistoryOutput("DRAG",       "CD",   FORMAT_FIXED, "AERO_COEFF", TYPE_COEFFICIENT);
  /// DESCRIPTION: Lift coefficient 
  AddHistoryOutput("LIFT",       "CL",   FORMAT_FIXED, "AERO_COEFF", TYPE_COEFFICIENT);
  /// DESCRIPTION: Sideforce coefficient   
  AddHistoryOutput("SIDEFORCE",  "CSF",  FORMAT_FIXED, "AERO_COEFF", TYPE_COEFFICIENT);
  /// DESCRIPTION: Moment around the x-axis    
  AddHistoryOutput("MOMENT-X",   "CMx",  FORMAT_FIXED, "AERO_COEFF", TYPE_COEFFICIENT);
  /// DESCRIPTION: Moment around the y-axis    
  AddHistoryOutput("MOMENT-Y",   "CMy",  FORMAT_FIXED, "AERO_COEFF", TYPE_COEFFICIENT);
  /// DESCRIPTION: Moment around the z-axis      
  AddHistoryOutput("MOMENT-Z",   "CMz",  FORMAT_FIXED, "AERO_COEFF", TYPE_COEFFICIENT);
  /// DESCRIPTION: Force in x direction    
  AddHistoryOutput("FORCE-X",    "CFx",  FORMAT_FIXED, "AERO_COEFF", TYPE_COEFFICIENT);
  /// DESCRIPTION: Force in y direction    
  AddHistoryOutput("FORCE-Y",    "CFy",  FORMAT_FIXED, "AERO_COEFF", TYPE_COEFFICIENT);
  /// DESCRIPTION: Force in z direction      
  AddHistoryOutput("FORCE-Z",    "CFz",  FORMAT_FIXED, "AERO_COEFF", TYPE_COEFFICIENT);
  /// DESCRIPTION: Lift-to-drag ratio
  AddHistoryOutput("EFFICIENCY", "CEff", FORMAT_FIXED, "AERO_COEFF", TYPE_COEFFICIENT);
  /// END_GROUP
  
  /// BEGIN_GROUP: AERO_COEFF_SURF, DESCRIPTION: Aerodynamic coefficients and forces per surface.
  vector<string> Marker_Monitoring;
  for (unsigned short iMarker_Monitoring = 0; iMarker_Monitoring < config->GetnMarker_Monitoring(); iMarker_Monitoring++){
    Marker_Monitoring.push_back(config->GetMarker_Monitoring_TagBound(iMarker_Monitoring));
  }  
  /// DESCRIPTION: Drag coefficient   
  AddHistoryOutputPerSurface("DRAG_ON_SURFACE",       "CD",   FORMAT_FIXED, "AERO_COEFF_SURF", Marker_Monitoring, TYPE_COEFFICIENT);
  /// DESCRIPTION: Lift coefficient   
  AddHistoryOutputPerSurface("LIFT_ON_SURFACE",       "CL",   FORMAT_FIXED, "AERO_COEFF_SURF", Marker_Monitoring, TYPE_COEFFICIENT);
  /// DESCRIPTION: Sideforce coefficient     
  AddHistoryOutputPerSurface("SIDEFORCE_ON_SURFACE",  "CSF",  FORMAT_FIXED, "AERO_COEFF_SURF", Marker_Monitoring, TYPE_COEFFICIENT);
  /// DESCRIPTION: Moment around the x-axis      
  AddHistoryOutputPerSurface("MOMENT-X_ON_SURFACE",   "CMx",  FORMAT_FIXED, "AERO_COEFF_SURF", Marker_Monitoring, TYPE_COEFFICIENT);
  /// DESCRIPTION: Moment around the y-axis      
  AddHistoryOutputPerSurface("MOMENT-Y_ON_SURFACE",   "CMy",  FORMAT_FIXED, "AERO_COEFF_SURF", Marker_Monitoring, TYPE_COEFFICIENT);
  /// DESCRIPTION: Moment around the z-axis        
  AddHistoryOutputPerSurface("MOMENT-Z_ON_SURFACE",   "CMz",  FORMAT_FIXED, "AERO_COEFF_SURF", Marker_Monitoring, TYPE_COEFFICIENT);
  /// DESCRIPTION: Force in x direction      
  AddHistoryOutputPerSurface("FORCE-X_ON_SURFACE",    "CFx",  FORMAT_FIXED, "AERO_COEFF_SURF", Marker_Monitoring, TYPE_COEFFICIENT);
  /// DESCRIPTION: Force in y direction      
  AddHistoryOutputPerSurface("FORCE-Y_ON_SURFACE",    "CFy",  FORMAT_FIXED, "AERO_COEFF_SURF", Marker_Monitoring, TYPE_COEFFICIENT);
  /// DESCRIPTION: Force in z direction        
  AddHistoryOutputPerSurface("FORCE-Z_ON_SURFACE",    "CFz",  FORMAT_FIXED, "AERO_COEFF_SURF", Marker_Monitoring, TYPE_COEFFICIENT);
  /// DESCRIPTION: Lift-to-drag ratio  
  AddHistoryOutputPerSurface("EFFICIENCY_ON_SURFACE", "CEff", FORMAT_FIXED, "AERO_COEFF_SURF", Marker_Monitoring, TYPE_COEFFICIENT);
  /// END_GROUP 
  
  /// DESCRIPTION: Angle of attack  
  AddHistoryOutput("AOA",         "AoA",                      FORMAT_SCIENTIFIC, "AOA");
  /// DESCRIPTION: Linear solver iterations   
  AddHistoryOutput("LINSOL_ITER", "Linear_Solver_Iterations", FORMAT_INTEGER,    "LINSOL_ITER");
  
  /// BEGIN_GROUP: AERO_COEFF_SURF, DESCRIPTION: Surface values on non-solid markers.
  vector<string> Marker_Analyze;
  for (unsigned short iMarker_Analyze = 0; iMarker_Analyze < config->GetnMarker_Analyze(); iMarker_Analyze++){
    Marker_Analyze.push_back(config->GetMarker_Analyze_TagBound(iMarker_Analyze));
  }  
  /// DESCRIPTION: Average mass flow    
  AddHistoryOutputPerSurface("AVG_MASSFLOW",             "Avg_Massflow",              FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Average Mach number      
  AddHistoryOutputPerSurface("AVG_MACH",                 "Avg_Mach",                  FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Average Temperature        
  AddHistoryOutputPerSurface("AVG_TEMP",                 "Avg_Temp",                  FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Average Pressure  
  AddHistoryOutputPerSurface("AVG_PRESS",                "Avg_Press",                 FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Average Density  
  AddHistoryOutputPerSurface("AVG_DENSITY",              "Avg_Density",               FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Average Enthalpy  
  AddHistoryOutputPerSurface("AVG_ENTHALPY",             "Avg_Enthalpy",              FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Average velocity in normal direction of the surface
  AddHistoryOutputPerSurface("AVG_NORMALVEL",            "Avg_NormalVel",             FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Flow uniformity 
  AddHistoryOutputPerSurface("UNIFORMITY",               "Uniformity",                FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Secondary strength
  AddHistoryOutputPerSurface("SECONDARY_STRENGTH",       "Secondary_Strength",        FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Momentum distortion  
  AddHistoryOutputPerSurface("MOMENTUM_DISTORTION",      "Momentum_Distortion",       FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Secondary over uniformity 
  AddHistoryOutputPerSurface("SECONDARY_OVER_UNIFORMITY", "Secondary_Over_Uniformity", FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Average total temperature  
  AddHistoryOutputPerSurface("AVG_TOTALTEMP",            "Avg_TotalTemp",             FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Average total pressure   
  AddHistoryOutputPerSurface("AVG_TOTALPRESS",           "Avg_TotalPress",            FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// DESCRIPTION: Pressure drop    
  AddHistoryOutputPerSurface("PRESSURE_DROP",            "Pressure_Drop",             FORMAT_SCIENTIFIC, "SURFACE_OUTPUT", Marker_Analyze, TYPE_COEFFICIENT);
  /// END_GROUP
  
  /// BEGIN_GROUP: ENGINE_OUTPUT, DESCRIPTION: Engine output
  /// DESCRIPTION: Aero CD drag
  AddHistoryOutput("AEROCDRAG",                  "AeroCDrag",                  FORMAT_SCIENTIFIC, "ENGINE_OUTPUT", TYPE_COEFFICIENT);
  /// DESCRIPTION: Solid CD drag  
  AddHistoryOutput("SOLIDCDRAG",                 "SolidCDrag",                 FORMAT_SCIENTIFIC, "ENGINE_OUTPUT", TYPE_COEFFICIENT);
  /// DESCRIPTION: Radial distortion 
  AddHistoryOutput("RADIAL_DISTORTION",          "Radial_Distortion",          FORMAT_SCIENTIFIC, "ENGINE_OUTPUT", TYPE_COEFFICIENT);
  /// DESCRIPTION: Circumferential distortion
  AddHistoryOutput("CIRCUMFERENTIAL_DISTORTION", "Circumferential_Distortion", FORMAT_SCIENTIFIC, "ENGINE_OUTPUT", TYPE_COEFFICIENT);
  /// END_GROUP
  
  /// BEGIN_GROUP: ROTATING_FRAME, DESCRIPTION: Coefficients related to a rotating frame of reference.
  /// DESCRIPTION: Merit  
  AddHistoryOutput("MERIT", "CMerit", FORMAT_SCIENTIFIC, "ROTATING_FRAME", TYPE_COEFFICIENT);
  /// DESCRIPTION: CT 
  AddHistoryOutput("CT",    "CT",     FORMAT_SCIENTIFIC, "ROTATING_FRAME", TYPE_COEFFICIENT);
  /// DESCRIPTION: CQ  
  AddHistoryOutput("CQ",    "CQ",     FORMAT_SCIENTIFIC, "ROTATING_FRAME", TYPE_COEFFICIENT);
  /// END_GROUP
  
  /// BEGIN_GROUP: EQUIVALENT_AREA, DESCRIPTION: Equivalent area.  
  /// DESCRIPTION: Equivalent area    
  AddHistoryOutput("EQUIV_AREA",   "CEquiv_Area",  FORMAT_SCIENTIFIC, "EQUIVALENT_AREA", TYPE_COEFFICIENT);
  /// DESCRIPTION: Nearfield obj. function      
  AddHistoryOutput("NEARFIELD_OF", "CNearFieldOF", FORMAT_SCIENTIFIC, "EQUIVALENT_AREA", TYPE_COEFFICIENT);
  /// END_GROUP

}

void CFlowOutput::SetVolumeOutputFields(CConfig *config){
  
  // Grid coordinates
  AddVolumeOutput("COORD-X", "x", "COORDINATES");
  AddVolumeOutput("COORD-Y", "y", "COORDINATES");
  if (nDim == 3)
    AddVolumeOutput("COORD-Z", "z", "COORDINATES");
  
  // Solution variables
  AddVolumeOutput("DENSITY",    "Density",    "SOLUTION");
  AddVolumeOutput("MOMENTUM-X", "Momentum_x", "SOLUTION");
  AddVolumeOutput("MOMENTUM-Y", "Momentum_y", "SOLUTION");
  if (nDim == 3)
    AddVolumeOutput("MOMENTUM-Z", "Momentum_z", "SOLUTION");
  AddVolumeOutput("ENERGY",     "Energy",     "SOLUTION");  
  
  // Turbulent Residuals
  switch(config->GetKind_Turb_Model()){
  case SST:
    AddVolumeOutput("TKE", "TKE", "SOLUTION");
    AddVolumeOutput("OMEGA", "Omega", "SOLUTION");
    break;
  case SA: case SA_COMP: case SA_E: 
  case SA_E_COMP: case SA_NEG: 
    AddVolumeOutput("NU_TILDE", "Nu_Tilde", "SOLUTION");
    break;
  case NONE:
    break;
  }
  
  // Primitive variables
  AddVolumeOutput("PRESSURE",    "Pressure",                "PRIMITIVE");
  AddVolumeOutput("TEMPERATURE", "Temperature",             "PRIMITIVE");
  AddVolumeOutput("MACH",        "Mach",                    "PRIMITIVE");
  AddVolumeOutput("PRESSURE_COEFF", "Pressure_Coefficient", "PRIMITIVE");
  
  if (config->GetKind_Solver() == RANS || config->GetKind_Solver() == NAVIER_STOKES){
    AddVolumeOutput("LAMINAR_VISCOSITY", "Laminar_Viscosity", "PRIMITIVE");
    
    AddVolumeOutput("SKIN_FRICTION-X", "Skin_Friction_Coefficient_x", "PRIMITIVE");
    AddVolumeOutput("SKIN_FRICTION-Y", "Skin_Friction_Coefficient_y", "PRIMITIVE");
    if (nDim == 3)
      AddVolumeOutput("SKIN_FRICTION-Z", "Skin_Friction_Coefficient_z", "PRIMITIVE");
    
    AddVolumeOutput("HEAT_FLUX", "Heat_Flux", "PRIMITIVE");
    AddVolumeOutput("Y_PLUS", "Y_Plus", "PRIMITIVE");
    
  }
  
  if (config->GetKind_Solver() == RANS) {
    AddVolumeOutput("EDDY_VISCOSITY", "Eddy_Viscosity", "PRIMITIVE");
  }
  
  if (config->GetKind_Trans_Model() == BC){
    AddVolumeOutput("INTERMITTENCY", "gamma_BC", "INTERMITTENCY");
  }

  //Residuals
  AddVolumeOutput("RESIDUAL_DENSITY", "Residual_Density", "RESIDUAL");
  AddVolumeOutput("RESIDUAL_MOMENTUM-X", "Residual_Momentum_x", "RESIDUAL");
  AddVolumeOutput("RESIDUAL_MOMENTUM-Y", "Residual_Momentum_y", "RESIDUAL");
  if (nDim == 3)
    AddVolumeOutput("RESIDUAL_MOMENTUM-Z", "Residual_Momentum_z", "RESIDUAL");
  AddVolumeOutput("RESIDUAL_ENERGY", "Residual_Energy", "RESIDUAL");
  
  switch(config->GetKind_Turb_Model()){
  case SST:
    AddVolumeOutput("RESIDUAL_TKE", "Residual_TKE", "RESIDUAL");
    AddVolumeOutput("RESIDUAL_OMEGA", "Residual_Omega", "RESIDUAL");
    break;
  case SA: case SA_COMP: case SA_E: 
  case SA_E_COMP: case SA_NEG: 
    AddVolumeOutput("RESIDUAL_NU_TILDE", "Residual_Nu_Tilde", "RESIDUAL");
    break;
  case NONE:
    break;
  }
  
  // Limiter values
  AddVolumeOutput("LIMITER_DENSITY", "Limiter_Density", "LIMITER");
  AddVolumeOutput("LIMITER_MOMENTUM-X", "Limiter_Momentum_x", "LIMITER");
  AddVolumeOutput("LIMITER_MOMENTUM-Y", "Limiter_Momentum_y", "LIMITER");
  if (nDim == 3)
    AddVolumeOutput("LIMITER_MOMENTUM-Z", "Limiter_Momentum_z", "LIMITER");
  AddVolumeOutput("LIMITER_ENERGY", "Limiter_Energy", "LIMITER");
  
  switch(config->GetKind_Turb_Model()){
  case SST:
    AddVolumeOutput("LIMITER_TKE", "Limiter_TKE", "RESIDUAL");
    AddVolumeOutput("LIMITER_OMEGA", "Limiter_Omega", "RESIDUAL");
    break;
  case SA: case SA_COMP: case SA_E: 
  case SA_E_COMP: case SA_NEG: 
    AddVolumeOutput("LIMITER_NU_TILDE", "Limiter_Nu_Tilde", "RESIDUAL");
    break;
  case NONE:
    break;
  }
  
  // Hybrid RANS-LES
  if (config->GetKind_HybridRANSLES() != NO_HYBRIDRANSLES){
    AddVolumeOutput("DES_LENGTHSCALE", "DES_LengthScale", "DDES");
    AddVolumeOutput("WALL_DISTANCE", "Wall_Distance", "DDES");
  }
  
  // Roe Low Dissipation
  if (config->GetKind_RoeLowDiss() != NO_ROELOWDISS){
    AddVolumeOutput("ROE_DISSIPATION", "Roe_Dissipation", "ROE_DISSIPATION");
  }
  
  if(config->GetKind_Solver() == RANS || config->GetKind_Solver() == NAVIER_STOKES){
    if (nDim == 3){
      AddVolumeOutput("VORTICITY_X", "Vorticity_x", "VORTEX_IDENTIFICATION");
      AddVolumeOutput("VORTICITY_Y", "Vorticity_y", "VORTEX_IDENTIFICATION");
    }
    AddVolumeOutput("VORTICITY_Z", "Vorticity_z", "VORTEX_IDENTIFICATION");
    AddVolumeOutput("Q_CRITERION", "Q_Criterion", "VORTEX_IDENTIFICATION");  
  }
}

void CFlowOutput::LoadVolumeData(CConfig *config, CGeometry *geometry, CSolver **solver, unsigned long iPoint){
  
  CVariable* Node_Flow = solver[FLOW_SOL]->node[iPoint]; 
  CVariable* Node_Turb = NULL;
  
  if (config->GetKind_Turb_Model() != NONE){
    Node_Turb = solver[TURB_SOL]->node[iPoint]; 
  }
  
  CPoint*    Node_Geo  = geometry->node[iPoint];
          
  SetVolumeOutputValue("COORD-X", iPoint,  Node_Geo->GetCoord(0));  
  SetVolumeOutputValue("COORD-Y", iPoint,  Node_Geo->GetCoord(1));
  if (nDim == 3)
    SetVolumeOutputValue("COORD-Z", iPoint, Node_Geo->GetCoord(2));
  
  SetVolumeOutputValue("DENSITY",    iPoint, Node_Flow->GetSolution(0));
  SetVolumeOutputValue("MOMENTUM-X", iPoint, Node_Flow->GetSolution(1));
  SetVolumeOutputValue("MOMENTUM-Y", iPoint, Node_Flow->GetSolution(2));
  if (nDim == 3){
    SetVolumeOutputValue("MOMENTUM-Z", iPoint, Node_Flow->GetSolution(3));
    SetVolumeOutputValue("ENERGY",     iPoint, Node_Flow->GetSolution(4));
  } else {
    SetVolumeOutputValue("ENERGY",     iPoint, Node_Flow->GetSolution(3));    
  }
  
  // Turbulent Residuals
  switch(config->GetKind_Turb_Model()){
  case SST:
    SetVolumeOutputValue("TKE", iPoint, Node_Turb->GetSolution(0));
    SetVolumeOutputValue("OMEGA", iPoint, Node_Turb->GetSolution(1));
    break;
  case SA: case SA_COMP: case SA_E: 
  case SA_E_COMP: case SA_NEG: 
    SetVolumeOutputValue("NU_TILDE", iPoint, Node_Turb->GetSolution(0));
    break;
  case NONE:
    break;
  }
  
  SetVolumeOutputValue("PRESSURE", iPoint, Node_Flow->GetPressure());
  SetVolumeOutputValue("TEMPERATURE", iPoint, Node_Flow->GetTemperature());
  SetVolumeOutputValue("MACH", iPoint, sqrt(Node_Flow->GetVelocity2())/Node_Flow->GetSoundSpeed());
  SetVolumeOutputValue("PRESSURE_COEFF", iPoint, (Node_Flow->GetPressure() - RefPressure)*factor*RefArea);
  
  if (config->GetKind_Solver() == RANS || config->GetKind_Solver() == NAVIER_STOKES){
    SetVolumeOutputValue("LAMINAR_VISCOSITY", iPoint, Node_Flow->GetLaminarViscosity());
  }
  
  if (config->GetKind_Solver() == RANS) {
    SetVolumeOutputValue("EDDY_VISCOSITY", iPoint, Node_Flow->GetEddyViscosity());
  }
  
  if (config->GetKind_Trans_Model() == BC){
    SetVolumeOutputValue("INTERMITTENCY", iPoint, Node_Turb->GetGammaBC());
  }
  
  SetVolumeOutputValue("RESIDUAL_DENSITY", iPoint, solver[FLOW_SOL]->LinSysRes.GetBlock(iPoint, 0));
  SetVolumeOutputValue("RESIDUAL_MOMENTUM-X", iPoint, solver[FLOW_SOL]->LinSysRes.GetBlock(iPoint, 1));
  SetVolumeOutputValue("RESIDUAL_MOMENTUM-Y", iPoint, solver[FLOW_SOL]->LinSysRes.GetBlock(iPoint, 2));
  if (nDim == 3){
    SetVolumeOutputValue("RESIDUAL_MOMENTUM-Z", iPoint, solver[FLOW_SOL]->LinSysRes.GetBlock(iPoint, 3));
    SetVolumeOutputValue("RESIDUAL_ENERGY", iPoint, solver[FLOW_SOL]->LinSysRes.GetBlock(iPoint, 4));
  } else {
    SetVolumeOutputValue("RESIDUAL_ENERGY", iPoint, solver[FLOW_SOL]->LinSysRes.GetBlock(iPoint, 3));   
  }
  
  switch(config->GetKind_Turb_Model()){
  case SST:
    SetVolumeOutputValue("RESIDUAL_TKE", iPoint, solver[TURB_SOL]->LinSysRes.GetBlock(iPoint, 0));
    SetVolumeOutputValue("RESIDUAL_OMEGA", iPoint, solver[TURB_SOL]->LinSysRes.GetBlock(iPoint, 1));
    break;
  case SA: case SA_COMP: case SA_E: 
  case SA_E_COMP: case SA_NEG: 
    SetVolumeOutputValue("RESIDUAL_NU_TILDE", iPoint, solver[TURB_SOL]->LinSysRes.GetBlock(iPoint, 0));
    break;
  case NONE:
    break;
  }
  
  SetVolumeOutputValue("LIMITER_DENSITY", iPoint, Node_Flow->GetLimiter_Primitive(0));
  SetVolumeOutputValue("LIMITER_MOMENTUM-X", iPoint, Node_Flow->GetLimiter_Primitive(1));
  SetVolumeOutputValue("LIMITER_MOMENTUM-Y", iPoint, Node_Flow->GetLimiter_Primitive(2));
  if (nDim == 3){
    SetVolumeOutputValue("LIMITER_MOMENTUM-Z", iPoint, Node_Flow->GetLimiter_Primitive(3));
    SetVolumeOutputValue("LIMITER_ENERGY", iPoint, Node_Flow->GetLimiter_Primitive(4));
  } else {
    SetVolumeOutputValue("LIMITER_ENERGY", iPoint, Node_Flow->GetLimiter_Primitive(3));   
  }
  
  switch(config->GetKind_Turb_Model()){
  case SST:
    SetVolumeOutputValue("LIMITER_TKE", iPoint, Node_Turb->GetLimiter_Primitive(0));
    SetVolumeOutputValue("LIMITER_OMEGA", iPoint, Node_Turb->GetLimiter_Primitive(1));
    break;
  case SA: case SA_COMP: case SA_E: 
  case SA_E_COMP: case SA_NEG: 
    SetVolumeOutputValue("LIMITER_NU_TILDE", iPoint, Node_Turb->GetLimiter_Primitive(0));
    break;
  case NONE:
    break;
  }
  
  if (config->GetKind_HybridRANSLES() != NO_HYBRIDRANSLES){
    SetVolumeOutputValue("DES_LENGTHSCALE", iPoint, Node_Flow->GetDES_LengthScale());
    SetVolumeOutputValue("WALL_DISTANCE", iPoint, Node_Geo->GetWall_Distance());
  }
  
  if (config->GetKind_RoeLowDiss() != NO_ROELOWDISS){
    SetVolumeOutputValue("ROE_DISSIPATION", iPoint, Node_Flow->GetRoe_Dissipation());
  }
  
  if(config->GetKind_Solver() == RANS || config->GetKind_Solver() == NAVIER_STOKES){
    if (nDim == 3){
      SetVolumeOutputValue("VORTICITY_X", iPoint, Node_Flow->GetVorticity()[0]);
      SetVolumeOutputValue("VORTICITY_Y", iPoint, Node_Flow->GetVorticity()[1]);      
    } 
    SetVolumeOutputValue("VORTICITY_Z", iPoint, Node_Flow->GetVorticity()[2]);      
    SetVolumeOutputValue("Q_CRITERION", iPoint, GetQ_Criterion(config, geometry, Node_Flow));      
  }
  
}

void CFlowOutput::LoadSurfaceData(CConfig *config, CGeometry *geometry, CSolver **solver, unsigned long iPoint, unsigned short iMarker, unsigned long iVertex){
  
  if ((config->GetKind_Solver() == NAVIER_STOKES) || (config->GetKind_Solver()  == RANS)) {  
    SetVolumeOutputValue("SKIN_FRICTION-X", iPoint, solver[FLOW_SOL]->GetCSkinFriction(iMarker, iVertex, 0));
    SetVolumeOutputValue("SKIN_FRICTION-Y", iPoint, solver[FLOW_SOL]->GetCSkinFriction(iMarker, iVertex, 1));
    if (nDim == 3)
      SetVolumeOutputValue("SKIN_FRICTION-Z", iPoint, solver[FLOW_SOL]->GetCSkinFriction(iMarker, iVertex, 2));
  
    SetVolumeOutputValue("HEAT_FLUX", iPoint, solver[FLOW_SOL]->GetHeatFlux(iMarker, iVertex));
    SetVolumeOutputValue("Y_PLUS", iPoint, solver[FLOW_SOL]->GetYPlus(iMarker, iVertex));
  }
}

void CFlowOutput::LoadHistoryData(CGeometry ****geometry, CSolver *****solver_container, CConfig **config,
      CIntegration ****integration, bool DualTime, su2double timeused, unsigned short val_iZone, unsigned short val_iInst) {
  
  CSolver* flow_solver = solver_container[val_iZone][val_iInst][MESH_0][FLOW_SOL];
  CSolver* turb_solver = solver_container[val_iZone][val_iInst][MESH_0][TURB_SOL];
  
  SetHistoryOutputValue("TIME_ITER", config[val_iZone]->GetTimeIter());  
  SetHistoryOutputValue("INNER_ITER", config[val_iZone]->GetInnerIter());
  SetHistoryOutputValue("OUTER_ITER", config[val_iZone]->GetOuterIter());  
  SetHistoryOutputValue("PHYS_TIME", timeused);
  
  SetHistoryOutputValue("RMS_DENSITY", log10(flow_solver->GetRes_RMS(0)));
  SetHistoryOutputValue("RMS_MOMENTUM-X", log10(flow_solver->GetRes_RMS(1)));
  SetHistoryOutputValue("RMS_MOMENTUM-Y", log10(flow_solver->GetRes_RMS(2)));
  if (nDim == 2)
    SetHistoryOutputValue("RMS_ENERGY", log10(flow_solver->GetRes_RMS(3)));
  else {
    SetHistoryOutputValue("RMS_MOMENTUM-Z", log10(flow_solver->GetRes_RMS(3)));
    SetHistoryOutputValue("RMS_ENERGY", log10(flow_solver->GetRes_RMS(4)));
  }
  
  switch(turb_model){
  case SA: case SA_NEG: case SA_E: case SA_COMP: case SA_E_COMP:
    SetHistoryOutputValue("RMS_NU_TILDE", log10(turb_solver->GetRes_RMS(0)));
    break;  
  case SST:
    SetHistoryOutputValue("RMS_KINETIC_ENERGY", log10(turb_solver->GetRes_RMS(0)));
    SetHistoryOutputValue("RMS_DISSIPATION",    log10(turb_solver->GetRes_RMS(1)));
    break;
  default: break;
  }
  
  SetHistoryOutputValue("MAX_DENSITY", log10(flow_solver->GetRes_Max(0)));
  SetHistoryOutputValue("MAX_MOMENTUM-X", log10(flow_solver->GetRes_Max(1)));
  SetHistoryOutputValue("MAX_MOMENTUM-Y", log10(flow_solver->GetRes_Max(2)));
  if (nDim == 2)
    SetHistoryOutputValue("MAX_ENERGY", log10(flow_solver->GetRes_Max(3)));
  else {
    SetHistoryOutputValue("MAX_MOMENTUM-Z", log10(flow_solver->GetRes_Max(3)));
    SetHistoryOutputValue("MAX_ENERGY", log10(flow_solver->GetRes_Max(4)));
  }
  
  switch(turb_model){
  case SA: case SA_NEG: case SA_E: case SA_COMP: case SA_E_COMP:
    SetHistoryOutputValue("MAX_NU_TILDE", log10(turb_solver->GetRes_Max(0)));
    break;  
  case SST:
    SetHistoryOutputValue("MAX_KINETIC_ENERGY", log10(turb_solver->GetRes_Max(0)));
    SetHistoryOutputValue("MAX_DISSIPATION",    log10(turb_solver->GetRes_Max(1)));
    break;
  default: break;
  }
  
  SetHistoryOutputValue("DRAG", flow_solver->GetTotal_CD());
  SetHistoryOutputValue("LIFT", flow_solver->GetTotal_CL());
  if (nDim == 3)
    SetHistoryOutputValue("SIDEFORCE", flow_solver->GetTotal_CSF());
  if (nDim == 3){
    SetHistoryOutputValue("MOMENT-X", flow_solver->GetTotal_CMx());
    SetHistoryOutputValue("MOMENT-Y", flow_solver->GetTotal_CMy());
  }
  SetHistoryOutputValue("MOMENT-Z", flow_solver->GetTotal_CMz());
  SetHistoryOutputValue("FORCE-X", flow_solver->GetTotal_CFx());
  SetHistoryOutputValue("FORCE-Y", flow_solver->GetTotal_CFy());
  if (nDim == 3)
    SetHistoryOutputValue("FORCE-Z", flow_solver->GetTotal_CFz());
  SetHistoryOutputValue("EFFICIENCY", flow_solver->GetTotal_CEff());
  
  for (unsigned short iMarker_Monitoring = 0; iMarker_Monitoring < config[val_iZone]->GetnMarker_Monitoring(); iMarker_Monitoring++) {
    SetHistoryOutputPerSurfaceValue("DRAG_ON_SURFACE", flow_solver->GetSurface_CD(iMarker_Monitoring), iMarker_Monitoring);
    SetHistoryOutputPerSurfaceValue("LIFT_ON_SURFACE", flow_solver->GetSurface_CL(iMarker_Monitoring), iMarker_Monitoring);
    if (nDim == 3)
      SetHistoryOutputPerSurfaceValue("SIDEFORCE_ON_SURFACE", flow_solver->GetSurface_CSF(iMarker_Monitoring), iMarker_Monitoring);
    if (nDim == 3){
      SetHistoryOutputPerSurfaceValue("MOMENT-X_ON_SURFACE", flow_solver->GetSurface_CMx(iMarker_Monitoring), iMarker_Monitoring);
      SetHistoryOutputPerSurfaceValue("MOMENT-Y_ON_SURFACE", flow_solver->GetSurface_CMy(iMarker_Monitoring), iMarker_Monitoring);
    }
    SetHistoryOutputPerSurfaceValue("MOMENT-Z_ON_SURFACE", flow_solver->GetSurface_CMz(iMarker_Monitoring), iMarker_Monitoring);
    SetHistoryOutputPerSurfaceValue("FORCE-X_ON_SURFACE", flow_solver->GetSurface_CFx(iMarker_Monitoring), iMarker_Monitoring);
    SetHistoryOutputPerSurfaceValue("FORCE-Y_ON_SURFACE", flow_solver->GetSurface_CFy(iMarker_Monitoring), iMarker_Monitoring);
    if (nDim == 3)
      SetHistoryOutputPerSurfaceValue("FORCE-Z_ON_SURFACE", flow_solver->GetSurface_CFz(iMarker_Monitoring), iMarker_Monitoring);   
    
    SetHistoryOutputPerSurfaceValue("EFFICIENCY_ON_SURFACE", flow_solver->GetSurface_CEff(iMarker_Monitoring), iMarker_Monitoring);
  }
  
  SetHistoryOutputValue("AOA", config[val_iZone]->GetAoA());
  SetHistoryOutputValue("LINSOL_ITER", flow_solver->GetIterLinSolver());
  
  for (unsigned short iMarker_Analyze = 0; iMarker_Analyze < config[val_iZone]->GetnMarker_Analyze(); iMarker_Analyze++) {  
    SetHistoryOutputPerSurfaceValue("AVG_MASSFLOW",               config[val_iZone]->GetSurface_MassFlow(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("AVG_MACH",                   config[val_iZone]->GetSurface_Mach(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("AVG_TEMP",                   config[val_iZone]->GetSurface_Temperature(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("AVG_PRESS",                  config[val_iZone]->GetSurface_Pressure(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("AVG_DENSITY",                config[val_iZone]->GetSurface_Density(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("AVG_ENTHALPY",               config[val_iZone]->GetSurface_Enthalpy(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("AVG_NORMALVEL",              config[val_iZone]->GetSurface_NormalVelocity(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("UNIFORMITY",                 config[val_iZone]->GetSurface_Uniformity(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("SECONDARY_STRENGTH",         config[val_iZone]->GetSurface_SecondaryStrength(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("MOMENTUM_DISTORTION",        config[val_iZone]->GetSurface_MomentumDistortion(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("SECONDARY_OVER_UNIFORMITY",  config[val_iZone]->GetSurface_SecondOverUniform(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("AVG_TOTALTEMP",              config[val_iZone]->GetSurface_TotalTemperature(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("AVG_TOTALPRESS",             config[val_iZone]->GetSurface_TotalPressure(iMarker_Analyze), iMarker_Analyze);
    SetHistoryOutputPerSurfaceValue("PRESSURE_DROP",              config[val_iZone]->GetSurface_PressureDrop(iMarker_Analyze), iMarker_Analyze);
  }
}

su2double CFlowOutput::GetQ_Criterion(CConfig *config, CGeometry *geometry, CVariable* node_flow){
  
  unsigned short iDim, jDim;
  su2double Grad_Vel[3][3] = {{0.0, 0.0, 0.0},{0.0, 0.0, 0.0},{0.0, 0.0, 0.0}};
  su2double Omega[3][3]    = {{0.0, 0.0, 0.0},{0.0, 0.0, 0.0},{0.0, 0.0, 0.0}};
  su2double Strain[3][3]   = {{0.0, 0.0, 0.0},{0.0, 0.0, 0.0},{0.0, 0.0, 0.0}};
  for (iDim = 0; iDim < nDim; iDim++) {
    for (unsigned short jDim = 0 ; jDim < nDim; jDim++) {
      Grad_Vel[iDim][jDim] = node_flow->GetGradient_Primitive(iDim+1, jDim);
      Strain[iDim][jDim]   = 0.5*(Grad_Vel[iDim][jDim] + Grad_Vel[jDim][iDim]);
      Omega[iDim][jDim]    = 0.5*(Grad_Vel[iDim][jDim] - Grad_Vel[jDim][iDim]);
    }
  }
  
  su2double OmegaMag = 0.0, StrainMag = 0.0;
  for (iDim = 0; iDim < nDim; iDim++) {
    for (jDim = 0 ; jDim < nDim; jDim++) {
      StrainMag += Strain[iDim][jDim]*Strain[iDim][jDim];
      OmegaMag  += Omega[iDim][jDim]*Omega[iDim][jDim];
    }
  }
  StrainMag = sqrt(StrainMag); OmegaMag = sqrt(OmegaMag);
  
  su2double Q = 0.5*(OmegaMag - StrainMag);
  
  return Q;
}


bool CFlowOutput::SetInit_Residuals(CConfig *config){
  
  return (config->GetUnsteady_Simulation() != STEADY && (config->GetIntIter() == 0))|| 
        (config->GetUnsteady_Simulation() == STEADY && (config->GetExtIter() < 2)); 
  
}

bool CFlowOutput::SetUpdate_Averages(CConfig *config, bool dualtime){
  
  return (config->GetUnsteady_Simulation() != STEADY && !dualtime);
      
}


