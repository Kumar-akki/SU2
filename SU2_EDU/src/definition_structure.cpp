/*!
 * \file definition_structure.cpp
 * \brief Main subroutines used by SU2_CFD.
 * \author Aerospace Design Laboratory (Stanford University) <http://su2.stanford.edu>.
 * \version 2.0.9
 *
 * Stanford University Unstructured (SU2).
 * Copyright (C) 2012-2013 Aerospace Design Laboratory (ADL).
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

#include "../include/definition_structure.hpp"


unsigned short GetnZone(string val_mesh_filename, unsigned short val_format, CConfig *config) {
  string text_line, Marker_Tag;
  ifstream mesh_file;
  short nZone = 1;
  bool isFound = false;
  char cstr[200];
  string::size_type position;
  int rank = MASTER_NODE;
  
#ifndef NO_MPI
  rank = MPI::COMM_WORLD.Get_rank();
  if (MPI::COMM_WORLD.Get_size() != 1) {
    unsigned short lastindex = val_mesh_filename.find_last_of(".");
    val_mesh_filename = val_mesh_filename.substr(0, lastindex);
    val_mesh_filename = val_mesh_filename + "_1.su2";
  }
#endif
  
  /*--- Search the mesh file for the 'NZONE' keyword. ---*/
  switch (val_format) {
    case SU2:
      
      /*--- Open grid file ---*/
      strcpy (cstr, val_mesh_filename.c_str());
      mesh_file.open(cstr, ios::in);
      if (mesh_file.fail()) {
        cout << "cstr=" << cstr << endl;
        cout << "There is no geometry file (GetnZone))!" << endl;
#ifdef NO_MPI
        exit(1);
#else
        MPI::COMM_WORLD.Abort(1);
        MPI::Finalize();
#endif
      }
      
      /*--- Open the SU2 mesh file ---*/
      while (getline (mesh_file,text_line)) {
        
        /*--- Search for the "NZONE" keyword to see if there are multiple Zones ---*/
        position = text_line.find ("NZONE=",0);
        if (position != string::npos) {
          text_line.erase (0,6); nZone = atoi(text_line.c_str()); isFound = true;
          if (rank == MASTER_NODE) {
            //					if (nZone == 1) cout << "SU2 mesh file format with a single zone." << endl;
            //					else if (nZone >  1) cout << "SU2 mesh file format with " << nZone << " zones." << endl;
            //					else
            if (nZone <= 0) {
              cout << "Error: Number of mesh zones is less than 1 !!!" << endl;
#ifdef NO_MPI
              exit(1);
#else
              MPI::COMM_WORLD.Abort(1);
              MPI::Finalize();
#endif
            }
          }
        }
      }
      /*--- If the "NZONE" keyword was not found, assume this is an ordinary
       simulation on a single Zone ---*/
      if (!isFound) {
        nZone = 1;
        //			if (rank == MASTER_NODE) cout << "SU2 mesh file format with a single zone." << endl;
      }
      break;
      
    case CGNS:
      
      nZone = 1;
      //		if (rank == MASTER_NODE) cout << "CGNS mesh file format with a single zone." << endl;
      break;
      
    case NETCDF_ASCII:
      
      nZone = 1;
      //		if (rank == MASTER_NODE) cout << "NETCDF mesh file format with a single zone." << endl;
      break;
      
  }
  
  /*--- For time spectral integration, nZones = nTimeInstances. ---*/
  if (config->GetUnsteady_Simulation() == TIME_SPECTRAL) {
    nZone = config->GetnTimeInstances();
  }
  
  return (unsigned short) nZone;
}

unsigned short GetnDim(string val_mesh_filename, unsigned short val_format) {
  
  string text_line, Marker_Tag;
  ifstream mesh_file;
  short nDim = 3;
  bool isFound = false;
  char cstr[200];
  string::size_type position;
  
#ifndef NO_MPI
  if (MPI::COMM_WORLD.Get_size() != 1) {
    unsigned short lastindex = val_mesh_filename.find_last_of(".");
    val_mesh_filename = val_mesh_filename.substr(0, lastindex);
    val_mesh_filename = val_mesh_filename + "_1.su2";
  }
#endif
  
  switch (val_format) {
    case SU2:
      
      /*--- Open grid file ---*/
      strcpy (cstr, val_mesh_filename.c_str());
      mesh_file.open(cstr, ios::in);
      
      /*--- Read SU2 mesh file ---*/
      while (getline (mesh_file,text_line)) {
        /*--- Search for the "NDIM" keyword to see if there are multiple Zones ---*/
        position = text_line.find ("NDIME=",0);
        if (position != string::npos) {
          text_line.erase (0,6); nDim = atoi(text_line.c_str()); isFound = true;
        }
      }
      break;
      
    case CGNS:
      nDim = 3;
      break;
      
    case NETCDF_ASCII:
      nDim = 3;
      break;
  }
  return (unsigned short) nDim;
}



void Geometrical_Preprocessing(CGeometry ***geometry, CConfig **config, unsigned short val_nZone) {
  
  
  unsigned short iMGlevel, iZone;
  unsigned long iPoint;
  int rank = MASTER_NODE;
#ifndef NO_MPI
  rank = MPI::COMM_WORLD.Get_rank();
#endif
  
  for (iZone = 0; iZone < val_nZone; iZone++) {
    
    /*--- Compute elements surrounding points, points surrounding points,
     and elements surrounding elements ---*/
    if (rank == MASTER_NODE) cout << "Setting local point and element connectivity." << endl;
    geometry[iZone][MESH_0]->SetEsuP();
    geometry[iZone][MESH_0]->SetPsuP();
    geometry[iZone][MESH_0]->SetEsuE();
    
    /*--- Check the orientation before computing geometrical quantities ---*/
    if (rank == MASTER_NODE) cout << "Checking the numerical grid orientation." << endl;
    geometry[iZone][MESH_0]->SetBoundVolume();
    geometry[iZone][MESH_0]->Check_Orientation(config[iZone]);
    
    /*--- Create the edge structure ---*/
    if (rank == MASTER_NODE) cout << "Identifying edges and vertices." << endl;
    geometry[iZone][MESH_0]->SetEdges();
    geometry[iZone][MESH_0]->SetVertex(config[iZone]);
    
    /*--- Compute center of gravity ---*/
    if (rank == MASTER_NODE) cout << "Computing centers of gravity." << endl;
    geometry[iZone][MESH_0]->SetCG();
    
    /*--- Create the control volume structures ---*/
    if (rank == MASTER_NODE) cout << "Setting the control volume structure." << endl;
    geometry[iZone][MESH_0]->SetControlVolume(config[iZone], ALLOCATE);
    geometry[iZone][MESH_0]->SetBoundControlVolume(config[iZone], ALLOCATE);
    
    /*--- Identify closest normal neighbor ---*/
    if (rank == MASTER_NODE) cout << "Searching for the closest normal neighbors to the surfaces." << endl;
    geometry[iZone][MESH_0]->FindNormal_Neighbor(config[iZone]);
    
    /*--- Compute the surface curvature ---*/
    if (rank == MASTER_NODE) cout << "Compute the surface curvature." << endl;
    geometry[iZone][MESH_0]->ComputeSurf_Curvature(config[iZone]);
    
    if ((config[iZone]->GetMGLevels() != 0) && (rank == MASTER_NODE))
      cout << "Setting the multigrid structure." <<endl;
    
  }
  
#ifndef NO_MPI
  /*--- Synchronization point after the multigrid stuff ---*/
  MPI::COMM_WORLD.Barrier();
#endif
  
  /*--- Loop over all the new grid ---*/
  for (iMGlevel = 1; iMGlevel <= config[ZONE_0]->GetMGLevels(); iMGlevel++) {
    
    /*--- Loop over all zones at each grid level. ---*/
    for (iZone = 0; iZone < val_nZone; iZone++) {
      
      /*--- Create main agglomeration structure (including MPI calls) ---*/
      geometry[iZone][iMGlevel] = new CMultiGridGeometry(geometry, config, iMGlevel, iZone);
      
      /*--- Compute points surrounding points. ---*/
      geometry[iZone][iMGlevel]->SetPsuP(geometry[iZone][iMGlevel-1]);
      
      /*--- Create the edge structure ---*/
      geometry[iZone][iMGlevel]->SetEdges();
      geometry[iZone][iMGlevel]->SetVertex(geometry[iZone][iMGlevel-1], config[iZone]);
      
      /*--- Create the control volume structures ---*/
      geometry[iZone][iMGlevel]->SetControlVolume(config[iZone],geometry[iZone][iMGlevel-1], ALLOCATE);
      geometry[iZone][iMGlevel]->SetBoundControlVolume(config[iZone],geometry[iZone][iMGlevel-1], ALLOCATE);
      geometry[iZone][iMGlevel]->SetCoord(geometry[iZone][iMGlevel-1]);
      
      /*--- Find closest neighbor to a surface point ---*/
      geometry[iZone][iMGlevel]->FindNormal_Neighbor(config[iZone]);
      
    }
  }
  
  /*--- For unsteady simulations, initialize the grid volumes
   and coordinates for previous solutions. Loop over all zones/grids. Is this
   the best place for this? ---*/
  for (iZone = 0; iZone < val_nZone; iZone++) {
    if (config[iZone]->GetUnsteady_Simulation() && config[iZone]->GetGrid_Movement()) {
      for (iMGlevel = 0; iMGlevel <= config[iZone]->GetMGLevels(); iMGlevel++) {
        for (iPoint = 0; iPoint < geometry[iZone][iMGlevel]->GetnPoint(); iPoint++) {
          geometry[iZone][iMGlevel]->node[iPoint]->SetVolume_n();
          geometry[iZone][iMGlevel]->node[iPoint]->SetVolume_nM1();
          
          geometry[iZone][iMGlevel]->node[iPoint]->SetCoord_n();
          geometry[iZone][iMGlevel]->node[iPoint]->SetCoord_n1();
        }
      }
    }
  }
  
}

void Solver_Preprocessing(CSolver ***solver_container, CGeometry **geometry,
                          CConfig *config, unsigned short iZone) {
  
  unsigned short iMGlevel;
  bool euler, ns, turbulent, spalart_allmaras, menter_sst;
  
  /*--- Initialize some useful booleans ---*/
  euler = false;  ns = false;  turbulent = false;
  menter_sst = false;  spalart_allmaras = false;

  /*--- Assign booleans ---*/
  switch (config->GetKind_Solver()) {
    case EULER : euler = true; break;
    case NAVIER_STOKES: ns = true; break;
    case RANS : ns = true; turbulent = true; break;
  }
  /*--- Assign turbulence model booleans --- */
  if (turbulent) {
    switch (config->GetKind_Turb_Model()) {
      case SA: spalart_allmaras = true; break;
      case SST: menter_sst = true; break;
        
      default: cout << "Specified turbulence model unavailable or none selected" << endl; exit(1); break;
    }
  }
  
  /*--- Definition of the Class for the solution: solver_container[DOMAIN][MESH_LEVEL][EQUATION]. Note that euler, ns
   and potential are incompatible, they use the same position in sol container ---*/
  for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
    
    /*--- Allocate solution for direct problem, and run the preprocessing and postprocessing ---*/
    if (euler) {
      solver_container[iMGlevel][FLOW_SOL] = new CEulerSolver(geometry[iMGlevel], config, iMGlevel);
    }
    if (ns) {
      solver_container[iMGlevel][FLOW_SOL] = new CNSSolver(geometry[iMGlevel], config, iMGlevel);
    }
    if (turbulent) {
      if (spalart_allmaras) {
        solver_container[iMGlevel][TURB_SOL] = new CTurbSASolver(geometry[iMGlevel], config, iMGlevel);
        solver_container[iMGlevel][FLOW_SOL]->Preprocessing(geometry[iMGlevel], solver_container[iMGlevel], config, iMGlevel, NO_RK_ITER, RUNTIME_FLOW_SYS);
        solver_container[iMGlevel][TURB_SOL]->Postprocessing(geometry[iMGlevel], solver_container[iMGlevel], config, iMGlevel);
      }
      else if (menter_sst) {
        solver_container[iMGlevel][TURB_SOL] = new CTurbSSTSolver(geometry[iMGlevel], config, iMGlevel);
        solver_container[iMGlevel][FLOW_SOL]->Preprocessing(geometry[iMGlevel], solver_container[iMGlevel], config, iMGlevel, NO_RK_ITER, RUNTIME_FLOW_SYS);
        solver_container[iMGlevel][TURB_SOL]->Postprocessing(geometry[iMGlevel], solver_container[iMGlevel], config, iMGlevel);
      }
    }
    
  }
  
}

void Integration_Preprocessing(CIntegration **integration_container,
                               CGeometry **geometry, CConfig *config,
                               unsigned short iZone) {
  
  bool
  euler, ns, turbulent, spalart_allmaras, menter_sst;
  
  /*--- Initialize some useful booleans ---*/
  euler            = false;   ns               = false;  turbulent        = false;
  spalart_allmaras = false;   menter_sst       = false;
  
  /*--- Assign booleans ---*/
  switch (config->GetKind_Solver()) {
    case EULER : euler = true; break;
    case NAVIER_STOKES: ns = true; break;
    case RANS : ns = true; turbulent = true; break;
  }
  
  /*--- Assign turbulence model booleans --- */
  if (turbulent) {
    switch (config->GetKind_Turb_Model()) {
      case SA: spalart_allmaras = true; break;
      case SST: menter_sst = true; break;
      default: cout << "Specified turbulence model unavailable or none selected" << endl; exit(1); break;
    }
  }
  
  /*--- Allocate solution for direct problem ---*/
  if (euler) integration_container[FLOW_SOL] = new CMultiGridIntegration(config);
  if (ns) integration_container[FLOW_SOL] = new CMultiGridIntegration(config);
  if (turbulent) integration_container[TURB_SOL] = new CSingleGridIntegration(config);
  
}

void Numerics_Preprocessing(CNumerics ****numerics_container,
                            CSolver ***solver_container, CGeometry **geometry,
                            CConfig *config, unsigned short iZone) {
  
  unsigned short iMGlevel, iSol, nDim,
  
  nVar_Flow         = 0,
  nVar_Turb         = 0;
  
  double *constants = NULL;
  
  bool
  euler, ns, turbulent,
  spalart_allmaras, menter_sst;
  
  bool compressible = (config->GetKind_Regime() == COMPRESSIBLE);
  bool incompressible = (config->GetKind_Regime() == INCOMPRESSIBLE);
  bool freesurface = (config->GetKind_Regime() == FREESURFACE);
  
  /*--- Initialize some useful booleans ---*/
  euler            = false;   ns               = false;   turbulent        = false;
  spalart_allmaras = false; menter_sst       = false;
  
  /*--- Assign booleans ---*/
  switch (config->GetKind_Solver()) {
    case EULER : euler = true; break;
    case NAVIER_STOKES: ns = true; break;
    case RANS : ns = true; turbulent = true; break;
  }
  
  /*--- Assign turbulence model booleans --- */
  if (turbulent) {
    switch (config->GetKind_Turb_Model()){
      case SA: spalart_allmaras = true; break;
      case SST: menter_sst = true; constants = solver_container[MESH_0][TURB_SOL]->GetConstants(); break;
      default: cout << "Specified turbulence model unavailable or none selected" << endl; exit(1); break;
    }
  }
  
  /*--- Number of variables for direct problem ---*/
  if (euler)				nVar_Flow = solver_container[MESH_0][FLOW_SOL]->GetnVar();
  if (ns)	          nVar_Flow = solver_container[MESH_0][FLOW_SOL]->GetnVar();
  if (turbulent)		nVar_Turb = solver_container[MESH_0][TURB_SOL]->GetnVar();
  
  /*--- Number of dimensions ---*/
  nDim = geometry[MESH_0]->GetnDim();
  
  /*--- Definition of the Class for the numerical method: numerics_container[MESH_LEVEL][EQUATION][EQ_TERM] ---*/
  for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
    numerics_container[iMGlevel] = new CNumerics** [MAX_SOLS];
    for (iSol = 0; iSol < MAX_SOLS; iSol++)
      numerics_container[iMGlevel][iSol] = new CNumerics* [MAX_TERMS];
  }
  
  /*--- Solver definition for the Potential, Euler, Navier-Stokes problems ---*/
  if ((euler) || (ns)) {
    
    /*--- Definition of the convective scheme for each equation and mesh level ---*/
    switch (config->GetKind_ConvNumScheme_Flow()) {
      case NO_CONVECTIVE :
        cout << "No convective scheme." << endl; exit(1);
        break;
        
      case SPACE_CENTERED :
        if (compressible) {
          /*--- Compressible flow ---*/
          switch (config->GetKind_Centered_Flow()) {
            case NO_CENTERED : cout << "No centered scheme." << endl; break;
            case LAX : numerics_container[MESH_0][FLOW_SOL][CONV_TERM] = new CCentLax_Flow(nDim,nVar_Flow, config); break;
            case JST : numerics_container[MESH_0][FLOW_SOL][CONV_TERM] = new CCentJST_Flow(nDim,nVar_Flow, config); break;
            default : cout << "Centered scheme not implemented." << endl; exit(1); break;
          }
          
          if (!config->GetLowFidelitySim()) {
            for (iMGlevel = 1; iMGlevel <= config->GetMGLevels(); iMGlevel++)
              numerics_container[iMGlevel][FLOW_SOL][CONV_TERM] = new CCentLax_Flow(nDim, nVar_Flow, config);
          }
          else {
            numerics_container[MESH_1][FLOW_SOL][CONV_TERM] = new CCentJST_Flow(nDim, nVar_Flow, config);
            for (iMGlevel = 2; iMGlevel <= config->GetMGLevels(); iMGlevel++)
              numerics_container[iMGlevel][FLOW_SOL][CONV_TERM] = new CCentLax_Flow(nDim, nVar_Flow, config);
          }
          
          /*--- Definition of the boundary condition method ---*/
          for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++)
            numerics_container[iMGlevel][FLOW_SOL][CONV_BOUND_TERM] = new CUpwRoe_Flow(nDim, nVar_Flow, config);
          
        }
        if (incompressible) {
          /*--- Incompressible flow, use artificial compressibility method ---*/
          switch (config->GetKind_Centered_Flow()) {
            case NO_CENTERED : cout << "No centered scheme." << endl; break;
            case LAX : numerics_container[MESH_0][FLOW_SOL][CONV_TERM] = new CCentLaxArtComp_Flow(nDim, nVar_Flow, config); break;
            case JST : numerics_container[MESH_0][FLOW_SOL][CONV_TERM] = new CCentJSTArtComp_Flow(nDim, nVar_Flow, config); break;
            default : cout << "Centered scheme not implemented." << endl; exit(1); break;
          }
          for (iMGlevel = 1; iMGlevel <= config->GetMGLevels(); iMGlevel++)
            numerics_container[iMGlevel][FLOW_SOL][CONV_TERM] = new CCentLaxArtComp_Flow(nDim,nVar_Flow, config);
          
          /*--- Definition of the boundary condition method ---*/
          for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++)
            numerics_container[iMGlevel][FLOW_SOL][CONV_BOUND_TERM] = new CUpwRoeArtComp_Flow(nDim, nVar_Flow, config);
          
        }
        if (freesurface) {
          /*--- FreeSurface flow, use artificial compressibility method ---*/
          cout << "Centered scheme not implemented." << endl; exit(1);
        }
        break;
      case SPACE_UPWIND :
        if (compressible) {
          /*--- Compressible flow ---*/
          switch (config->GetKind_Upwind_Flow()) {
            case NO_UPWIND : cout << "No upwind scheme." << endl; break;
            case ROE_1ST : case ROE_2ND :
              for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
                numerics_container[iMGlevel][FLOW_SOL][CONV_TERM] = new CUpwRoe_Flow(nDim, nVar_Flow, config);
                numerics_container[iMGlevel][FLOW_SOL][CONV_BOUND_TERM] = new CUpwRoe_Flow(nDim, nVar_Flow, config);
              }
              break;
              
            case AUSM_1ST : case AUSM_2ND :
              for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
                numerics_container[iMGlevel][FLOW_SOL][CONV_TERM] = new CUpwAUSM_Flow(nDim, nVar_Flow, config);
                numerics_container[iMGlevel][FLOW_SOL][CONV_BOUND_TERM] = new CUpwAUSM_Flow(nDim, nVar_Flow, config);
              }
              break;
              
            case ROE_TURKEL_1ST : case ROE_TURKEL_2ND :
              for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
                numerics_container[iMGlevel][FLOW_SOL][CONV_TERM] = new CUpwRoe_Turkel_Flow(nDim, nVar_Flow, config);
                numerics_container[iMGlevel][FLOW_SOL][CONV_BOUND_TERM] = new CUpwRoe_Turkel_Flow(nDim, nVar_Flow, config);
              }
              break;
              
            case HLLC_1ST : case HLLC_2ND :
              for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
                numerics_container[iMGlevel][FLOW_SOL][CONV_TERM] = new CUpwHLLC_Flow(nDim, nVar_Flow, config);
                numerics_container[iMGlevel][FLOW_SOL][CONV_BOUND_TERM] = new CUpwHLLC_Flow(nDim, nVar_Flow, config);
              }
              break;
              
            default : cout << "Upwind scheme not implemented." << endl; exit(1); break;
          }
          
        }
        if (incompressible) {
          /*--- Incompressible flow, use artificial compressibility method ---*/
          switch (config->GetKind_Upwind_Flow()) {
            case NO_UPWIND : cout << "No upwind scheme." << endl; break;
            case ROE_1ST : case ROE_2ND :
              for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
                numerics_container[iMGlevel][FLOW_SOL][CONV_TERM] = new CUpwRoeArtComp_Flow(nDim, nVar_Flow, config);
                numerics_container[iMGlevel][FLOW_SOL][CONV_BOUND_TERM] = new CUpwRoeArtComp_Flow(nDim, nVar_Flow, config);
              }
              break;
            default : cout << "Upwind scheme not implemented." << endl; exit(1); break;
          }
        }
        if (freesurface) {
          /*--- Incompressible flow, use artificial compressibility method ---*/
          switch (config->GetKind_Upwind_Flow()) {
            case NO_UPWIND : cout << "No upwind scheme." << endl; break;
            case ROE_1ST : case ROE_2ND :
              for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
                numerics_container[iMGlevel][FLOW_SOL][CONV_TERM] = new CUpwRoeArtComp_FreeSurf_Flow(nDim, nVar_Flow, config);
                numerics_container[iMGlevel][FLOW_SOL][CONV_BOUND_TERM] = new CUpwRoeArtComp_FreeSurf_Flow(nDim, nVar_Flow, config);
              }
              break;
            default : cout << "Upwind scheme not implemented." << endl; exit(1); break;
          }
        }
        
        break;
        
      default :
        cout << "Convective scheme not implemented (euler and ns)." << endl; exit(1);
        break;
    }
    
    /*--- Definition of the viscous scheme for each equation and mesh level ---*/
    switch (config->GetKind_ViscNumScheme_Flow()) {
      case NONE :
        break;
      case AVG_GRAD :
        if (compressible) {
          /*--- Compressible flow ---*/
          for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
            numerics_container[iMGlevel][FLOW_SOL][VISC_TERM] = new CAvgGrad_Flow(nDim, nVar_Flow, config);
            numerics_container[iMGlevel][FLOW_SOL][VISC_BOUND_TERM] = new CAvgGrad_Flow(nDim, nVar_Flow, config);
          }
        }
        if (incompressible) {
          /*--- Incompressible flow, use artificial compressibility method ---*/
          for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
            numerics_container[iMGlevel][FLOW_SOL][VISC_TERM] = new CAvgGradArtComp_Flow(nDim, nVar_Flow, config);
            numerics_container[iMGlevel][FLOW_SOL][VISC_BOUND_TERM] = new CAvgGradArtComp_Flow(nDim, nVar_Flow, config);
          }
        }
        if (freesurface) {
          /*--- Freesurface flow, use artificial compressibility method ---*/
          for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
            numerics_container[iMGlevel][FLOW_SOL][VISC_TERM] = new CAvgGradArtComp_Flow(nDim, nVar_Flow, config);
            numerics_container[iMGlevel][FLOW_SOL][VISC_BOUND_TERM] = new CAvgGradArtComp_Flow(nDim, nVar_Flow, config);
          }
        }
        break;
      case AVG_GRAD_CORRECTED :
        if (compressible) {
          /*--- Compressible flow ---*/
          numerics_container[MESH_0][FLOW_SOL][VISC_TERM] = new CAvgGradCorrected_Flow(nDim, nVar_Flow, config);
          for (iMGlevel = 1; iMGlevel <= config->GetMGLevels(); iMGlevel++)
            numerics_container[iMGlevel][FLOW_SOL][VISC_TERM] = new CAvgGrad_Flow(nDim, nVar_Flow, config);
          
          /*--- Definition of the boundary condition method ---*/
          for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++)
            numerics_container[iMGlevel][FLOW_SOL][VISC_BOUND_TERM] = new CAvgGrad_Flow(nDim, nVar_Flow, config);
        }
        if (incompressible) {
          /*--- Incompressible flow, use artificial compressibility method ---*/
          numerics_container[MESH_0][FLOW_SOL][VISC_TERM] = new CAvgGradCorrectedArtComp_Flow(nDim, nVar_Flow, config);
          for (iMGlevel = 1; iMGlevel <= config->GetMGLevels(); iMGlevel++)
            numerics_container[iMGlevel][FLOW_SOL][VISC_TERM] = new CAvgGradArtComp_Flow(nDim, nVar_Flow, config);
          
          /*--- Definition of the boundary condition method ---*/
          for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++)
            numerics_container[iMGlevel][FLOW_SOL][VISC_BOUND_TERM] = new CAvgGradArtComp_Flow(nDim, nVar_Flow, config);
        }
        if (freesurface) {
          /*--- Freesurface flow, use artificial compressibility method ---*/
          numerics_container[MESH_0][FLOW_SOL][VISC_TERM] = new CAvgGradCorrectedArtComp_Flow(nDim, nVar_Flow, config);
          for (iMGlevel = 1; iMGlevel <= config->GetMGLevels(); iMGlevel++)
            numerics_container[iMGlevel][FLOW_SOL][VISC_TERM] = new CAvgGradArtComp_Flow(nDim, nVar_Flow, config);
          
          /*--- Definition of the boundary condition method ---*/
          for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++)
            numerics_container[iMGlevel][FLOW_SOL][VISC_BOUND_TERM] = new CAvgGradArtComp_Flow(nDim, nVar_Flow, config);
        }
        break;
      case GALERKIN :
        cout << "Galerkin viscous scheme not implemented." << endl; exit(1); exit(1);
        break;
      default :
        cout << "Numerical viscous scheme not recognized." << endl; exit(1); exit(1);
        break;
    }
    
    /*--- Definition of the source term integration scheme for each equation and mesh level ---*/
    switch (config->GetKind_SourNumScheme_Flow()) {
      case NONE :
        break;
      case PIECEWISE_CONSTANT :
        
        for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
          
          if (config->GetRotating_Frame() == YES)
            numerics_container[iMGlevel][FLOW_SOL][SOURCE_FIRST_TERM] = new CSourceRotatingFrame_Flow(nDim, nVar_Flow, config);
          else if (config->GetAxisymmetric() == YES)
            numerics_container[iMGlevel][FLOW_SOL][SOURCE_FIRST_TERM] = new CSourceAxisymmetric_Flow(nDim,nVar_Flow, config);
          else if (config->GetGravityForce() == YES)
            numerics_container[iMGlevel][FLOW_SOL][SOURCE_FIRST_TERM] = new CSourceGravity(nDim, nVar_Flow, config);
          else
            numerics_container[iMGlevel][FLOW_SOL][SOURCE_FIRST_TERM] = new CSourceNothing(nDim, nVar_Flow, config);
          
          numerics_container[iMGlevel][FLOW_SOL][SOURCE_SECOND_TERM] = new CSourceNothing(nDim, nVar_Flow, config);
        }
        
        break;
      default :
        cout << "Source term not implemented." << endl; exit(1);
        break;
    }
    
  }
  
  /*--- Solver definition for the turbulent model problem ---*/
  if (turbulent) {
    
    /*--- Definition of the convective scheme for each equation and mesh level ---*/
    switch (config->GetKind_ConvNumScheme_Turb()) {
      case NONE :
        break;
      case SPACE_UPWIND :
        for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++){
          if (spalart_allmaras) numerics_container[iMGlevel][TURB_SOL][CONV_TERM] = new CUpwSca_TurbSA(nDim, nVar_Turb, config);
          else if (menter_sst) numerics_container[iMGlevel][TURB_SOL][CONV_TERM] = new CUpwSca_TurbSST(nDim, nVar_Turb, config);
        }
        break;
      default :
        cout << "Convective scheme not implemented (turbulent)." << endl; exit(1);
        break;
    }
    
    /*--- Definition of the viscous scheme for each equation and mesh level ---*/
    switch (config->GetKind_ViscNumScheme_Turb()) {
      case NONE :
        break;
      case AVG_GRAD :
        for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++){
          if (spalart_allmaras) numerics_container[iMGlevel][TURB_SOL][VISC_TERM] = new CAvgGrad_TurbSA(nDim, nVar_Turb, config);
          else if (menter_sst) numerics_container[iMGlevel][TURB_SOL][VISC_TERM] = new CAvgGrad_TurbSST(nDim, nVar_Turb, constants, config);
        }
        break;
      case AVG_GRAD_CORRECTED :
        for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++){
          if (spalart_allmaras) numerics_container[iMGlevel][TURB_SOL][VISC_TERM] = new CAvgGradCorrected_TurbSA(nDim, nVar_Turb, config);
          else if (menter_sst) numerics_container[iMGlevel][TURB_SOL][VISC_TERM] = new CAvgGradCorrected_TurbSST(nDim, nVar_Turb, constants, config);
        }
        break;
      case GALERKIN :
        cout << "Viscous scheme not implemented." << endl;
        exit(1); break;
      default :
        cout << "Viscous scheme not implemented." << endl; exit(1);
        break;
    }
    
    /*--- Definition of the source term integration scheme for each equation and mesh level ---*/
    switch (config->GetKind_SourNumScheme_Turb()) {
      case NONE :
        break;
      case PIECEWISE_CONSTANT :
        for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++) {
          if (spalart_allmaras) numerics_container[iMGlevel][TURB_SOL][SOURCE_FIRST_TERM] = new CSourcePieceWise_TurbSA(nDim, nVar_Turb, config);
          else if (menter_sst) numerics_container[iMGlevel][TURB_SOL][SOURCE_FIRST_TERM] = new CSourcePieceWise_TurbSST(nDim, nVar_Turb, constants, config);
          numerics_container[iMGlevel][TURB_SOL][SOURCE_SECOND_TERM] = new CSourceNothing(nDim, nVar_Turb, config);
        }
        break;
      default :
        cout << "Source term not implemented." << endl; exit(1);
        break;
    }
    
    /*--- Definition of the boundary condition method ---*/
    for (iMGlevel = 0; iMGlevel <= config->GetMGLevels(); iMGlevel++){
      if (spalart_allmaras) {
        numerics_container[iMGlevel][TURB_SOL][CONV_BOUND_TERM] = new CUpwSca_TurbSA(nDim, nVar_Turb, config);
        numerics_container[iMGlevel][TURB_SOL][VISC_BOUND_TERM] = new CAvgGrad_TurbSA(nDim, nVar_Turb, config);
      }
      else if (menter_sst) {
        numerics_container[iMGlevel][TURB_SOL][CONV_BOUND_TERM] = new CUpwSca_TurbSST(nDim, nVar_Turb, config);
        numerics_container[iMGlevel][TURB_SOL][VISC_BOUND_TERM] = new CAvgGrad_TurbSST(nDim, nVar_Turb, constants, config);
      }
    }
  }
  
}
