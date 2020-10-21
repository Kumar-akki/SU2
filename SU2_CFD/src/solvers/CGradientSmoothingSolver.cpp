﻿/*!
 * \file CSolverGradientSmoothing.cpp
 * \brief Main solver routines for the gradient smoothing problem.
 * \author T. Dick
 * \version 7.0.5 "Blackbird"
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

#include "../../include/solvers/CGradientSmoothingSolver.hpp"
#include "../../include/variables/CSobolevSmoothingVariable.hpp"
#include <algorithm>


CGradientSmoothingSolver::CGradientSmoothingSolver(CGeometry *geometry, CConfig *config) : CSolver(false,true) {

  unsigned short iDim, jDim;
  unsigned int marker_count=0;
  unsigned long iPoint;

  /*--- general geometric settings ---*/
  nDim         = geometry->GetnDim();
  nPoint       = geometry->GetnPoint();
  nPointDomain = geometry->GetnPointDomain();
  nElement     = geometry->GetnElem();

  /*--- Here is where we assign the kind of each element ---*/

  /*--- First level: different possible terms of the equations  ---*/
  element_container = new CElement** [MAX_TERMS]();
  for (unsigned short iTerm = 0; iTerm < MAX_TERMS; iTerm++)
    element_container[iTerm] = new CElement* [MAX_FE_KINDS]();

  /*--- Initialize all subsequent levels ---*/
  for (unsigned short iKind = 0; iKind < MAX_FE_KINDS; iKind++) {
    element_container[GRAD_TERM][iKind] = nullptr;
  }

  if (nDim == 2) {
    element_container[GRAD_TERM][EL_TRIA] = new CTRIA1();
    element_container[GRAD_TERM][EL_QUAD] = new CQUAD4();
    if (config->GetSecOrdQuad()) {
      element_container[GRAD_TERM][EL_TRIA2] = new CTRIA3();
    }
  }
  else if (nDim == 3) {
    element_container[GRAD_TERM][EL_TETRA] = new CTETRA1();
    element_container[GRAD_TERM][EL_HEXA]  = new CHEXA8();
    element_container[GRAD_TERM][EL_PYRAM] = new CPYRAM5();
    element_container[GRAD_TERM][EL_PRISM] = new CPRISM6();
    if (config->GetSecOrdQuad()) {
      element_container[GRAD_TERM][EL_TETRA2] = new CTETRA4();
      element_container[GRAD_TERM][EL_PYRAM2] = new CPYRAM6();
    }
  }

  /*--- for operations on surfaces we initalize the structures for nDim-1 ---*/
  if (config->GetSmoothOnSurface()) {
    if (nDim == 2) {
      element_container[GRAD_TERM][EL_LINE] = new CLINE();
    }
    else if (nDim == 3) {
      element_container[GRAD_TERM][EL_TRIA] = new CTRIA1();
      element_container[GRAD_TERM][EL_QUAD] = new CQUAD4();
      if (config->GetSecOrdQuad()) {
        element_container[GRAD_TERM][EL_TRIA2] = new CTRIA3();
      }
    }
  }

  Residual = new su2double[nDim];   for (iDim = 0; iDim < nDim; iDim++) Residual[iDim] = 0.0;
  Solution = new su2double[nDim];   for (iDim = 0; iDim < nDim; iDim++) Solution[iDim] = 0.0;
  mZeros_Aux = new su2double *[nDim];
  mId_Aux    = new su2double *[nDim];
  for(iDim = 0; iDim < nDim; iDim++){
    mZeros_Aux[iDim] = new su2double[nDim];
    mId_Aux[iDim]    = new su2double[nDim];
  }
  for(iDim = 0; iDim < nDim; iDim++){
    for (jDim = 0; jDim < nDim; jDim++){
      mZeros_Aux[iDim][jDim] = 0.0;
      mId_Aux[iDim][jDim]    = 0.0;
    }
    mId_Aux[iDim][iDim] = 1.0;
  }

  /*--- linear system ---*/
  if ( !config->GetSmoothOnSurface() ) {
    if ( config->GetSepDim() ) {
      LinSysSol.Initialize(nPoint, nPointDomain, 1, 0.0);
      LinSysRes.Initialize(nPoint, nPointDomain, 1, 0.0);
      Jacobian.Initialize(nPoint, nPointDomain, 1, 1, false, geometry, config, false, true);
    } else {
      LinSysSol.Initialize(nPoint, nPointDomain, nDim, 0.0);
      LinSysRes.Initialize(nPoint, nPointDomain, nDim, 0.0);
      Jacobian.Initialize(nPoint, nPointDomain, nDim, nDim, false, geometry, config, false, true);
    }

    // initialize auxiliar helper vectors
    auxVecInp.Initialize(nPoint, nPointDomain, nDim, 0.0);
  }

  activeCoord.Initialize(nPoint, nPointDomain, nDim, 0.0);

  /*--- passive vectors needed for projections ---*/
  /*--- they always need full size ---*/
  helperVecIn.Initialize(nPoint, nPointDomain, nDim, 0.0);
  helperVecOut.Initialize(nPoint, nPointDomain, nDim, 0.0);

  /*--- Initialize the CVariable structure holding solution data ---*/
  nodes = new CSobolevSmoothingVariable(nPoint, nDim,  config);
  SetBaseClassPointerToNodes();

  /*--- Initialize the boundary of the boundary ---*/
  if (config->GetSmoothOnSurface()) {

    /*--- check which points are in more than one physical boundary ---*/
    for (iPoint = 0; iPoint < nPoint; iPoint++) {


      for (unsigned short iMarker = 0; iMarker < config->GetnMarker_All(); iMarker++) {
        long iVertex = geometry->nodes->GetVertex(iPoint, iMarker);
        if (iVertex >= 0) {
          marker_count++;
        }
      }
      if (marker_count>=2) {
        nodes->MarkAsBoundaryPoint(iPoint);
      }
      marker_count = 0;
    }
  }

  /*--- Term ij of the Jacobian ---*/
  Jacobian_block = new su2double*[nDim];
  for (iDim = 0; iDim < nDim; iDim++) {
    Jacobian_block[iDim] = new su2double [nDim];
    for (jDim = 0; jDim < nDim; jDim++) {
      Jacobian_block[iDim][jDim] = 0.0;
    }
  }

  /*--- vector for the parameter gradient ---*/
  for (auto iDV=0; iDV<config->GetnDV_Total(); iDV++) {
    deltaP.push_back(0.0);
  }

}


CGradientSmoothingSolver::~CGradientSmoothingSolver(void) {

  unsigned short iDim;

  if (element_container != nullptr) {
    for (unsigned short iVar = 0; iVar < MAX_TERMS; iVar++) {
      for (unsigned short jVar = 0; jVar < MAX_FE_KINDS; jVar++) {
        delete element_container[iVar][jVar];
      }
      delete [] element_container[iVar];
    }
    delete [] element_container;
  }

  if (Jacobian_block != nullptr) {
    for (iDim = 0; iDim < nDim; ++iDim) {
      delete [] Jacobian_block[iDim];
    }
    delete [] Jacobian_block;
  }

  for (iDim = 0; iDim < nDim; iDim++) {
    if (mZeros_Aux[iDim] != nullptr) delete [] mZeros_Aux[iDim];
    if (mId_Aux[iDim] != nullptr) delete [] mId_Aux[iDim];
  }
  if (mZeros_Aux != nullptr) delete [] mZeros_Aux;
  if (mId_Aux != nullptr) delete [] mId_Aux;

  delete nodes;

}


void CGradientSmoothingSolver::ApplyGradientSmoothingVolume(CGeometry *geometry, CSolver *solver, CNumerics **numerics, CConfig *config) {

  /*--- current dimension if we run consecutive on each dimension ---*/
  dir = 0;

  /*--- Initialize vector and sparse matrix ---*/
  LinSysSol.SetValZero();
  LinSysRes.SetValZero();
  Jacobian.SetValZero();

  Compute_StiffMatrix(geometry, numerics, config);

  if ( config->GetSepDim() ) {

    for (dir = 0; dir < nDim ; dir++) {

      for (unsigned long iPoint =0; iPoint<geometry->GetnPoint(); iPoint++)  {
        auxVecInp.SetBlock(iPoint, dir, 1.0);
      }

      ofstream input ("input.txt");
      auxVecInp.printVec(input);
      input.close();

      Compute_Residual(geometry, solver, config);

      Impose_BC(geometry, numerics, config);

      Solve_Linear_System(geometry, config);

      WriteSensitivities(geometry, solver, config);

      ofstream result ("result.txt");
      LinSysSol.printVec(result);
      result.close();

      LinSysSol.SetValZero();
      LinSysRes.SetValZero();
    }

  } else {

    for (unsigned long iPoint =0; iPoint<geometry->GetnPoint(); iPoint++)  {
      for (auto iDim = 0; iDim < nDim ; iDim++) {
        auxVecInp.SetBlock(iPoint, iDim, 1.0);
      }
    }

    ofstream input ("input.txt");
    auxVecInp.printVec(input);
    input.close();

    Compute_Residual(geometry, solver, config);

    Impose_BC(geometry, numerics, config);

    Solve_Linear_System(geometry, config);

    ofstream result ("result.txt");
    LinSysSol.printVec(result);
    result.close();

    WriteSensitivities(geometry, solver, config);

  }


}


void CGradientSmoothingSolver::Compute_StiffMatrix(CGeometry *geometry, CNumerics **numerics, CConfig *config){

  unsigned long iElem;
  unsigned short iNode, iDim, nNodes = 0;
  unsigned long indexNode[8]={0,0,0,0,0,0,0,0};
  su2double val_Coord;
  int EL_KIND = 0;

  su2activematrix DHiDHj;
  su2double HiHj = 0.0;

  unsigned short NelNodes, jNode;

  /*--- Loops over all the elements ---*/

  for (iElem = 0; iElem < geometry->GetnElem(); iElem++) {

    if (geometry->elem[iElem]->GetVTK_Type() == TRIANGLE)      {nNodes = 3; EL_KIND = EL_TRIA;}
    if (geometry->elem[iElem]->GetVTK_Type() == QUADRILATERAL) {nNodes = 4; EL_KIND = EL_QUAD;}
    if (geometry->elem[iElem]->GetVTK_Type() == TETRAHEDRON)   {nNodes = 4; EL_KIND = EL_TETRA;}
    if (geometry->elem[iElem]->GetVTK_Type() == PYRAMID)       {nNodes = 5; EL_KIND = EL_PYRAM;}
    if (geometry->elem[iElem]->GetVTK_Type() == PRISM)         {nNodes = 6; EL_KIND = EL_PRISM;}
    if (geometry->elem[iElem]->GetVTK_Type() == HEXAHEDRON)    {nNodes = 8; EL_KIND = EL_HEXA;}

    // if we need higher order quadrature rules overide some of the element kinds
    if (config->GetSecOrdQuad()) {
      if (geometry->elem[iElem]->GetVTK_Type() == TRIANGLE)    {nNodes = 3; EL_KIND = EL_TRIA2;}
      if (geometry->elem[iElem]->GetVTK_Type() == TETRAHEDRON) {nNodes = 4; EL_KIND = EL_TETRA2;}
      if (geometry->elem[iElem]->GetVTK_Type() == PYRAMID)     {nNodes = 6; EL_KIND = EL_PYRAM2;}
    }

    for (iNode = 0; iNode < nNodes; iNode++) {

      indexNode[iNode] = geometry->elem[iElem]->GetNode(iNode);

      for (iDim = 0; iDim < nDim; iDim++) {
        val_Coord = Get_ValCoord(geometry, indexNode[iNode], iDim);
        element_container[GRAD_TERM][EL_KIND]->SetRef_Coord(iNode, iDim, val_Coord);
      }

    }

    /*--- compute the contributions of the single elements inside the numerics container ---*/

    numerics[GRAD_TERM]->Compute_Tangent_Matrix(element_container[GRAD_TERM][EL_KIND], config);

    NelNodes = element_container[GRAD_TERM][EL_KIND]->GetnNodes();

    /*--- for all nodes add the contribution to the system Jacobian ---*/

    for (iNode = 0; iNode < NelNodes; iNode++) {

      for (jNode = 0; jNode < NelNodes; jNode++) {

        DHiDHj = element_container[GRAD_TERM][EL_KIND]->Get_DHiDHj(iNode, jNode);
        HiHj = element_container[GRAD_TERM][EL_KIND]->Get_HiHj(iNode, jNode);

        if ( config->GetSepDim() ) {

          Jacobian_block[0][0] = DHiDHj[dir][dir] + HiHj;
          Jacobian.AddBlock(indexNode[iNode], indexNode[jNode], Jacobian_block);

        } else {

          for (iDim = 0; iDim < nDim; iDim++) {
            Jacobian_block[iDim][iDim] = DHiDHj[iDim][iDim] + HiHj;
          }
          Jacobian.AddBlock(indexNode[iNode], indexNode[jNode], Jacobian_block);

        }
      }
    }
  }
}


void CGradientSmoothingSolver::Compute_Surface_StiffMatrix(CGeometry *geometry, CNumerics **numerics, CConfig *config, unsigned long val_marker, unsigned short nSurfDim){

  unsigned long iElem, iPoint, iVertex, iSurfDim;
  unsigned short iNode, jNode, nNodes = 0, NelNodes;
  std::vector<unsigned long> indexNode(8, 0.0);
  std::vector<unsigned long> indexVertex(8, 0.0);
  int EL_KIND = 0;

  su2activematrix DHiDHj;
  su2double HiHj = 0.0;

  su2activematrix Coord;

  /*--- Loops over all the elements ---*/

  for (iElem = 0; iElem < geometry->GetnElem_Bound(val_marker); iElem++) {

    /*--- Identify the kind of boundary element ---*/
    if (geometry->bound[val_marker][iElem]->GetVTK_Type() == LINE)           {nNodes = 2; EL_KIND = EL_LINE;}
    if (geometry->bound[val_marker][iElem]->GetVTK_Type() == TRIANGLE)       {nNodes = 3; EL_KIND = EL_TRIA;}
    if (geometry->bound[val_marker][iElem]->GetVTK_Type() == QUADRILATERAL)  {nNodes = 4; EL_KIND = EL_QUAD;}

    if (config->GetSecOrdQuad()) {
      if (geometry->bound[val_marker][iElem]->GetVTK_Type() == TRIANGLE)       {nNodes = 3; EL_KIND = EL_TRIA2;}
    }

    /*--- Retrieve the boundary reference and current coordinates ---*/

    for (iNode = 0; iNode < nNodes; iNode++) {
      indexNode[iNode] = geometry->bound[val_marker][iElem]->GetNode(iNode);
    }

    Coord = GetElementCoordinates(geometry, indexNode, EL_KIND);

    /*--- We need the indices of the vertices, which are "Dual Grid Info" ---*/
    for (iVertex = 0; iVertex < geometry->nVertex[val_marker]; iVertex++) {
      iPoint = geometry->vertex[val_marker][iVertex]->GetNode();
      for (iNode = 0; iNode < nNodes; iNode++) {
        if (iPoint == indexNode[iNode]) indexVertex[iNode] = iVertex;
      }
    }

    /*--- compute the contributions of the single elements inside the numerics container ---*/
    numerics[GRAD_TERM]->SetCoord(Coord);
    numerics[GRAD_TERM]->Compute_Tangent_Matrix(element_container[GRAD_TERM][EL_KIND], config);

    NelNodes = element_container[GRAD_TERM][EL_KIND]->GetnNodes();

    /*--- for all nodes add the contribution to the system Jacobian ---*/

    for (iNode = 0; iNode < NelNodes; iNode++) {
      for (jNode = 0; jNode < NelNodes; jNode++) {

        DHiDHj = element_container[GRAD_TERM][EL_KIND]->Get_DHiDHj(iNode, jNode);
        HiHj = element_container[GRAD_TERM][EL_KIND]->Get_HiHj(iNode, jNode);
        for (iSurfDim=0; iSurfDim<nSurfDim; iSurfDim++) {
          Jacobian_block[iSurfDim][iSurfDim] = DHiDHj[iSurfDim][iSurfDim] + HiHj;
        }
        Jacobian.AddBlock(indexVertex[iNode], indexVertex[jNode], Jacobian_block);

      }
    }
  }
}


void CGradientSmoothingSolver::Compute_Residual(CGeometry *geometry, CSolver *solver, CConfig *config){

  unsigned long iElem;
  unsigned short iDim, iNode, nNodes = 0;
  int EL_KIND = 0;
  std::vector<unsigned long> indexNode(8, 0.0);
  su2double Weight, Jac_X;

  for (iElem = 0; iElem < geometry->GetnElem(); iElem++) {

    if (geometry->elem[iElem]->GetVTK_Type() == TRIANGLE)      {nNodes = 3; EL_KIND = EL_TRIA;}
    if (geometry->elem[iElem]->GetVTK_Type() == QUADRILATERAL) {nNodes = 4; EL_KIND = EL_QUAD;}
    if (geometry->elem[iElem]->GetVTK_Type() == TETRAHEDRON)   {nNodes = 4; EL_KIND = EL_TETRA;}
    if (geometry->elem[iElem]->GetVTK_Type() == PYRAMID)       {nNodes = 5; EL_KIND = EL_PYRAM;}
    if (geometry->elem[iElem]->GetVTK_Type() == PRISM)         {nNodes = 6; EL_KIND = EL_PRISM;}
    if (geometry->elem[iElem]->GetVTK_Type() == HEXAHEDRON)    {nNodes = 8; EL_KIND = EL_HEXA;}

    // if we need higher order quadrature rules overide some of the element kinds
    if (config->GetSecOrdQuad()) {
      if (geometry->elem[iElem]->GetVTK_Type() == TRIANGLE)    {nNodes = 3; EL_KIND = EL_TRIA2;}
      if (geometry->elem[iElem]->GetVTK_Type() == TETRAHEDRON) {nNodes = 4; EL_KIND = EL_TETRA2;}
      if (geometry->elem[iElem]->GetVTK_Type() == PYRAMID)     {nNodes = 6; EL_KIND = EL_PYRAM2;}
    }

    for (iNode = 0; iNode < nNodes; iNode++) {

      indexNode[iNode] = geometry->elem[iElem]->GetNode(iNode);

      for (iDim = 0; iDim < nDim; iDim++) {
        auto val_Coord = Get_ValCoord(geometry, indexNode[iNode], iDim);
        element_container[GRAD_TERM][EL_KIND]->SetRef_Coord(iNode, iDim, val_Coord);
      }

    }

    element_container[GRAD_TERM][EL_KIND]->ClearElement();       /*--- Restarts the element: avoids adding over previous results in other elements --*/
    element_container[GRAD_TERM][EL_KIND]->ComputeGrad_Linear();
    unsigned short nGauss = element_container[GRAD_TERM][EL_KIND]->GetnGaussPoints();

    for (unsigned short iGauss = 0; iGauss < nGauss; iGauss++) {

      for (iNode = 0; iNode < nNodes; iNode++) {
        indexNode[iNode] = geometry->elem[iElem]->GetNode(iNode);
      }

      Weight = element_container[GRAD_TERM][EL_KIND]->GetWeight(iGauss);
      Jac_X = element_container[GRAD_TERM][EL_KIND]->GetJ_X(iGauss);

      for (unsigned short iNode = 0; iNode < nNodes; iNode++) {

        if ( config->GetSepDim() ) {

          if (config->GetSobMode()==DEBUG) {
            Residual[dir] += Weight * Jac_X * element_container[GRAD_TERM][EL_KIND]->GetNi(iNode,iGauss) * (auxVecInp.GetBlock(indexNode[iNode]))[dir];
            LinSysRes.AddBlock(indexNode[iNode], &Residual[dir]);
          } else {
            Residual[dir] += Weight * Jac_X * element_container[GRAD_TERM][EL_KIND]->GetNi(iNode,iGauss) * nodes->GetSensitivity(indexNode[iNode], dir);
            LinSysRes.AddBlock(indexNode[iNode], &Residual[dir]);
          }

        } else {

          for (iDim = 0; iDim < nDim; iDim++) {

            if (config->GetSobMode()==DEBUG) {
              Residual[iDim] += Weight * Jac_X * element_container[GRAD_TERM][EL_KIND]->GetNi(iNode,iGauss) * (auxVecInp.GetBlock(indexNode[iNode]))[iDim];
            } else {
              Residual[iDim] += Weight * Jac_X * element_container[GRAD_TERM][EL_KIND]->GetNi(iNode,iGauss) * nodes->GetSensitivity(indexNode[iNode], iDim);
            }
          }
          LinSysRes.AddBlock(indexNode[iNode], Residual);

        }

        for (iDim = 0; iDim < nDim; iDim++) {
          Residual[iDim] = 0;
        }

      }
    }
  }
}


void CGradientSmoothingSolver::Compute_Surface_Residual(CGeometry *geometry, CSolver *solver, CConfig *config, unsigned long val_marker){

  unsigned long iElem, iPoint, iVertex;
  unsigned short iDim, iNode, nNodes = 0;
  int EL_KIND = 0;
  std::vector<unsigned long> indexNode(8, 0.0);
  std::vector<unsigned long> indexVertex(8, 0.0);
  su2double Weight, Jac_X, normalSens = 0.0, norm;
  su2double* normal = NULL;
  su2activematrix Coord;

  for (iElem = 0; iElem < geometry->GetnElem_Bound(val_marker); iElem++) {

    /*--- Identify the kind of boundary element ---*/
    if (geometry->bound[val_marker][iElem]->GetVTK_Type() == LINE)           {nNodes = 2; EL_KIND = EL_LINE;}
    if (geometry->bound[val_marker][iElem]->GetVTK_Type() == TRIANGLE)       {nNodes = 3; EL_KIND = EL_TRIA;}
    if (geometry->bound[val_marker][iElem]->GetVTK_Type() == QUADRILATERAL)  {nNodes = 4; EL_KIND = EL_QUAD;}

    if (config->GetSecOrdQuad()) {
      if (geometry->bound[val_marker][iElem]->GetVTK_Type() == TRIANGLE)       {nNodes = 3; EL_KIND = EL_TRIA2;}
    }

    /*--- Retrieve the boundary reference and current coordinates ---*/
    for (iNode = 0; iNode < nNodes; iNode++) {
      indexNode[iNode] = geometry->bound[val_marker][iElem]->GetNode(iNode);
    }

    Coord = GetElementCoordinates(geometry, indexNode, EL_KIND);

    /*--- We need the indices of the vertices, which are "Dual Grid Info" ---*/
    for (iVertex = 0; iVertex < geometry->nVertex[val_marker]; iVertex++) {
      iPoint = geometry->vertex[val_marker][iVertex]->GetNode();
      for (iNode = 0; iNode < nNodes; iNode++) {
        if (iPoint == indexNode[iNode]) indexVertex[iNode] = iVertex;
      }
    }

    element_container[GRAD_TERM][EL_KIND]->ClearElement();       /*--- Restarts the element: avoids adding over previous results in other elements --*/
    element_container[GRAD_TERM][EL_KIND]->ComputeGrad_Linear(Coord);
    unsigned short nGauss = element_container[GRAD_TERM][EL_KIND]->GetnGaussPoints();

    for (unsigned short iGauss = 0; iGauss < nGauss; iGauss++) {

      Weight = element_container[GRAD_TERM][EL_KIND]->GetWeight(iGauss);
      Jac_X = element_container[GRAD_TERM][EL_KIND]->GetJ_X(iGauss);

      for (unsigned short iNode = 0; iNode < nNodes; iNode++) {

        normal = geometry->vertex[val_marker][indexVertex[iNode]]->GetNormal();
        norm = 0.0;
        for (iDim = 0; iDim < nDim; iDim++) {
          norm += normal[iDim]*normal[iDim];
        }
        norm = sqrt(norm);
        for (iDim = 0; iDim < nDim; iDim++) {
          normal[iDim] = normal[iDim] / norm;
        }

        for (iDim = 0; iDim < nDim; iDim++) {
          if (config->GetSobMode()==DEBUG) {
            normalSens += normal[iDim] * (auxVecInp.GetBlock(indexVertex[iNode]))[iDim];
          } else {
            normalSens += normal[iDim] * nodes->GetSensitivity(indexNode[iNode], iDim);
          }
        }      

        Residual[0] += Weight * Jac_X * element_container[GRAD_TERM][EL_KIND]->GetNi(iNode,iGauss) * normalSens;
        LinSysRes.AddBlock(indexVertex[iNode], Residual);

        Residual[0] = 0;
        normalSens = 0;

      }
    }
  }
}



void CGradientSmoothingSolver::Impose_BC(CGeometry *geometry, CNumerics **numerics, CConfig *config) {

  unsigned short iMarker;

  /*--- Get the boundary markers and iterate over them ---------------------------------*/
  /* Notice that for no marker we automatically impose Zero Neumann boundary conditions */

  for (iMarker = 0; iMarker < config->GetnMarker_All(); iMarker++) {
    if (config->GetMarker_All_SobolevBC(iMarker) == YES) {
      BC_Dirichlet(geometry, NULL, numerics, config, iMarker);
    }
  }

}


void CGradientSmoothingSolver::BC_Dirichlet(CGeometry *geometry, CSolver **solver_container, CNumerics **numerics, CConfig *config, unsigned short val_marker) {


  unsigned long iPoint, iVertex;
  unsigned long iVar, jVar;

  for (iVertex = 0; iVertex < geometry->nVertex[val_marker]; iVertex++) {

    /*--- Get node index ---*/

    iPoint = geometry->vertex[val_marker][iVertex]->GetNode();

    if ( config->GetSepDim() ) {

      su2double one = 1.0;
      su2double zero = 0.0;

      if (geometry->nodes->GetDomain(iPoint)) {

        LinSysRes.SetBlock(iPoint, &zero);
        LinSysSol.SetBlock(iPoint, &zero);

        for (iVar = 0; iVar < nPoint; iVar++) {
          if (iVar==iPoint) {
            Jacobian.SetBlock(iVar,iPoint, &one);
          }
          else {
            Jacobian.SetBlock(iVar,iPoint, &zero);
          }
        }
        /*--- Delete the rows for a particular node ---*/
        for (jVar = 0; jVar < nPoint; jVar++) {
          if (iPoint!=jVar) {
            Jacobian.SetBlock(iPoint,jVar, &zero);
          }
        }

      } else {
        /*--- Delete the column (iPoint is halo so Send/Recv does the rest) ---*/
        for (iVar = 0; iVar < nPoint; iVar++) {
          Jacobian.SetBlock(iVar,iPoint, &zero);
        }
      }

    } else {

      if (geometry->nodes->GetDomain(iPoint)) {

       if (nDim == 2) {
          Solution[0] = 0.0;  Solution[1] = 0.0;
          Residual[0] = 0.0;  Residual[1] = 0.0;
        }
        else {
          Solution[0] = 0.0;  Solution[1] = 0.0;  Solution[2] = 0.0;
          Residual[0] = 0.0;  Residual[1] = 0.0;  Residual[2] = 0.0;
        }

        LinSysRes.SetBlock(iPoint, Residual);
        LinSysSol.SetBlock(iPoint, Solution);

        /*--- STRONG ENFORCEMENT OF THE DIRICHLET BOUNDARY CONDITION ---*/
        /*--- Delete the columns for a particular node ---*/

        for (iVar = 0; iVar < nPoint; iVar++) {
          if (iVar==iPoint) {
            Jacobian.SetBlock(iVar,iPoint,mId_Aux);
          }
          else {
            Jacobian.SetBlock(iVar,iPoint,mZeros_Aux);
          }
        }

        /*--- Delete the rows for a particular node ---*/
        for (jVar = 0; jVar < nPoint; jVar++) {
          if (iPoint!=jVar) {
            Jacobian.SetBlock(iPoint,jVar,mZeros_Aux);
          }
        }

      } else {
        /*--- Delete the column (iPoint is halo so Send/Recv does the rest) ---*/
        for (iVar = 0; iVar < nPoint; iVar++) Jacobian.SetBlock(iVar,iPoint,mZeros_Aux);
      }

    }

  }

}


void CGradientSmoothingSolver::BC_Surface_Dirichlet(CGeometry *geometry, CConfig *config, unsigned short val_marker) {


  unsigned long iPoint, iVertex;
  unsigned long iVar, jVar;

  for (iVertex = 0; iVertex < geometry->nVertex[val_marker]; iVertex++) {

    /*--- Get node index ---*/

    iPoint = geometry->vertex[val_marker][iVertex]->GetNode();

    if ( nodes->IsBoundaryPoint(iPoint) ) {

      //std::cout << "Node " << iPoint << " at ("<< geometry->node[iPoint]->GetCoord(0) << ", " << geometry->node[iPoint]->GetCoord(1) << ") is a bound of a bound." << std::endl;

      su2double one = 1.0;
      su2double zero = 0.0;

      if (geometry->nodes->GetDomain(iPoint)) {

        LinSysRes.SetBlock(iVertex, &zero);
        LinSysSol.SetBlock(iVertex, &zero);

        for (iVar = 0; iVar < geometry->nVertex[val_marker]; iVar++) {
          if (iVar==iVertex) {
            //std::cout << "Setting block " << iVar << ", " << iVertex << std::endl;
            Jacobian.SetBlock(iVar,iVertex, &one);
          }
          else {
            //std::cout << "Setting block " << iVar << ", " << iVertex << std::endl;
            Jacobian.SetBlock(iVar,iVertex, &zero);
          }
        }
        /*--- Delete the rows for a particular node ---*/
        for (jVar = 0; jVar < geometry->nVertex[val_marker]; jVar++) {
          if (iVertex!=jVar) {
            //std::cout << "Setting block " << iVertex << ", " << jVar << std::endl;
            Jacobian.SetBlock(iVertex,jVar, &zero);
          }
        }

      } else {
        /*--- Delete the column (iPoint is halo so Send/Recv does the rest) ---*/
        for (iVar = 0; iVar < geometry->nVertex[val_marker]; iVar++) {
          //std::cout << "Setting block " << iVar<< ", " << iVertex << std::endl;
          Jacobian.SetBlock(iVar,iVertex, &zero);
        }
      }
    }
  }

}


// For now: left empty since there is no calculation necessary for zero Neumann boundaries.
void CGradientSmoothingSolver::BC_Neumann(CGeometry *geometry, CSolver **solver_container, CNumerics **numerics, CConfig *config, unsigned short val_marker) {

}


void CGradientSmoothingSolver::Solve_Linear_System(CGeometry *geometry, CConfig *config){

  unsigned long IterLinSol = 0;

  IterLinSol = System.Solve(Jacobian, LinSysRes, LinSysSol, geometry, config);

  SetIterLinSolver(IterLinSol);

}


void CGradientSmoothingSolver::WriteSensitivities(CGeometry *geometry, CSolver *solver, CConfig *config, unsigned long val_marker){

  unsigned long iPoint, total_index;
  unsigned short iDim;
  su2double* normal;
  su2double norm;

  if ( config->GetSmoothOnSurface() ) {

    for (unsigned long iVertex =0; iVertex<geometry->nVertex[val_marker]; iVertex++)  {

      iPoint = geometry->vertex[val_marker][iVertex]->GetNode();
      normal = geometry->vertex[val_marker][iVertex]->GetNormal();
      norm = 0.0;
      for (iDim = 0; iDim < nDim; iDim++) {
        norm += normal[iDim]*normal[iDim];
      }
      norm = sqrt(norm);
      for (iDim = 0; iDim < nDim; iDim++) {
        normal[iDim] = normal[iDim] / norm;
      }

      for (iDim = 0; iDim < nDim; iDim++) {
        this->GetNodes()->SetSensitivity(iPoint, iDim, normal[iDim]*LinSysSol[iVertex]);
      }
    }

  } else if ( config->GetSepDim() ) {

    for (iPoint = 0; iPoint < nPoint; iPoint++) {
      this->GetNodes()->SetSensitivity(iPoint, dir, LinSysSol[iPoint]);
    }

  } else {

    for (iPoint = 0; iPoint < nPoint; iPoint++) {
      for (iDim = 0; iDim < nDim; iDim++) {
        total_index = iPoint*nDim + iDim;
        this->GetNodes()->SetSensitivity(iPoint, iDim, LinSysSol[total_index]);
      }
    }

  }
}


su2activematrix CGradientSmoothingSolver::GetElementCoordinates(CGeometry *geometry, std::vector<unsigned long>& indexNode, int EL_KIND) {

  su2activematrix Coord;

  switch (EL_KIND) {

  case EL_LINE:

    Coord.resize(2,2);
    for(auto iNode=0; iNode<2; iNode++) {
      for(auto iDim=0; iDim<2; iDim++) {
        Coord[iNode][iDim] = Get_ValCoord(geometry, indexNode[iNode], iDim);
      }
    }
    break;

  case EL_TRIA:

    Coord.resize(3,3);
    for(auto iNode=0; iNode<3; iNode++) {
      for(auto iDim=0; iDim<3; iDim++) {
        Coord[iNode][iDim] = Get_ValCoord(geometry, indexNode[iNode], iDim);
      }
    }
    break;

  case EL_TRIA2:

    Coord.resize(3,3);
    for(auto iNode=0; iNode<3; iNode++) {
      for(auto iDim=0; iDim<3; iDim++) {
        Coord[iNode][iDim] = Get_ValCoord(geometry, indexNode[iNode], iDim);
      }
    }
    break;

  case EL_QUAD:

    Coord.resize(4,3);
    for(auto iNode=0; iNode<4; iNode++) {
      for(auto iDim=0; iDim<3; iDim++) {
        Coord[iNode][iDim] = Get_ValCoord(geometry, indexNode[iNode], iDim);
      }
    }
    break;

  default:
    std::cout << "Type of element is not supported. " <<std::endl;

  }

  return Coord;

}


void CGradientSmoothingSolver::ApplyGradientSmoothingSurface(CGeometry *geometry, CSolver *solver, CNumerics **numerics, CConfig *config, unsigned long val_marker) {

  /*--- Initialize vector and sparse matrix ---*/
  LinSysSol.Initialize(geometry->nVertex[val_marker], geometry->nVertex[val_marker], 1, 0.0);
  LinSysRes.Initialize(geometry->nVertex[val_marker], geometry->nVertex[val_marker], 1, 0.0);
  Jacobian.InitOwnConnectivity(geometry->nVertex[val_marker], 1, 1, val_marker, geometry, config);
  //LinSysSol.SetValZero();
  //LinSysRes.SetValZero();

  auxVecInp.Initialize(geometry->nVertex[val_marker], geometry->nVertex[val_marker], nDim, 1.0);
  //auxVecInp.SetValZero();


  ofstream input ("input.txt");
  auxVecInp.printVec(input);
  input.close();


  Compute_Surface_StiffMatrix(geometry, numerics, config, val_marker);

  Compute_Surface_Residual(geometry, solver, config, val_marker);

  if ( config->GetDirichletSurfaceBound() ) {
    BC_Surface_Dirichlet(geometry, config, val_marker);
  }

  Solve_Linear_System(geometry, config);

  ofstream result ("result.txt");
  LinSysSol.printVec(result);
  result.close();

  WriteSensitivities(geometry, solver, config, val_marker);

}


void CGradientSmoothingSolver::SetSensitivity(CGeometry *geometry, CSolver **solver, CConfig *config) {
  unsigned long iPoint;
  unsigned short iDim;
  for (iPoint = 0; iPoint < nPoint; iPoint++) {
    for (iDim = 0; iDim < nDim; iDim++) {
      nodes->SetSensitivity(iPoint,iDim, solver[ADJFLOW_SOL]->GetNodes()->GetSensitivity(iPoint,iDim));
    }
  }
}


void CGradientSmoothingSolver::OutputSensitivity(CGeometry *geometry, CSolver **solver, CConfig *config) {
  unsigned long iPoint;
  unsigned short iDim;
  for (iPoint = 0; iPoint < nPoint; iPoint++) {
    for (iDim = 0; iDim < nDim; iDim++) {
      solver[ADJFLOW_SOL]->GetNodes()->SetSensitivity(iPoint,iDim, nodes->GetSensitivity(iPoint,iDim));
    }
  }
}


void CGradientSmoothingSolver::WriteSens2Geometry(CGeometry *geometry, CConfig *config) {
  unsigned long iPoint;
  unsigned short iDim;
  for (iPoint = 0; iPoint < nPoint; iPoint++) {
    for (iDim = 0; iDim < nDim; iDim++) {
      geometry->SetSensitivity(iPoint,iDim, nodes->GetSensitivity(iPoint,iDim));
    }
  }
}


void CGradientSmoothingSolver::ReadSens2Geometry(CGeometry *geometry, CConfig *config) {
  unsigned long iPoint;
  unsigned short iDim;
  for (iPoint = 0; iPoint < nPoint; iPoint++) {
    for (iDim = 0; iDim < nDim; iDim++) {
      nodes->SetSensitivity(iPoint, iDim, geometry->GetSensitivity(iPoint,iDim));
    }
  }
}

void CGradientSmoothingSolver::WriteSens2Vector(CGeometry *geometry, CConfig *config, CSysVector<su2mixedfloat>& vector) {
  unsigned long iPoint;
  unsigned short iDim;
  for (iPoint = 0; iPoint < nPoint; iPoint++) {
    for (iDim = 0; iDim < nDim; iDim++) {
      vector[iPoint*nDim+iDim] = SU2_TYPE::GetValue(nodes->GetSensitivity(iPoint,iDim));
    }
  }
}

void CGradientSmoothingSolver::MultiplyParameterJacobian(su2double *Jacobian, bool transposed){

  unsigned iDV, iPoint, iDim,  total_index;
  su2double Sens;

  if (!transposed) {
    for (iDV=0; iDV<deltaP.size(); iDV++) {
      deltaP[iDV] = 0.0;
      for (iPoint = 0; iPoint < nPoint; iPoint++) {
        for (iDim = 0; iDim < nDim; iDim++) {
          total_index = iPoint*nDim+iDim;
          deltaP[iDV] += Jacobian[iDV * nPoint*nDim + total_index] * nodes->GetSensitivity(iPoint ,iDim);
        }
      }
    }
  } else if (transposed) {
    for (iPoint = 0; iPoint < nPoint; iPoint++) {
      for (iDim = 0; iDim < nDim; iDim++) {
        Sens = 0.0;
        total_index = iPoint*nDim+iDim;
        for (iDV=0; iDV<deltaP.size(); iDV++) {
          Sens += Jacobian[iDV * nPoint*nDim + total_index] * deltaP[iDV];
        }
        nodes->SetSensitivity(iPoint ,iDim, Sens);
      }
    }
  }

}


void CGradientSmoothingSolver::OutputDVGradient(string out_file) {

  unsigned iDV;

  ofstream delta_p (out_file);
  delta_p.precision(17);
  for (iDV = 0; iDV < deltaP.size(); iDV++) {
    delta_p << deltaP[iDV] << ",";
  }
  delta_p.close();

}


void CGradientSmoothingSolver::CalculateOriginalGradient(CGeometry *geometry, CVolumetricMovement *grid_movement, CConfig *config) {

  if (rank == MASTER_NODE) cout << endl << "Calculating the original DV gradient." << endl;

  WriteSens2Geometry(geometry,config);

  grid_movement->SetVolume_Deformation(geometry, config, false, true);

  ReadSens2Geometry(geometry,config);

  WriteSens2Vector(geometry, config, helperVecOut);

  ProjectMeshToDV(geometry, config, helperVecOut, deltaP);

  OutputDVGradient("orig_grad.dat");
}


void CGradientSmoothingSolver::WriteReadSurfaceSensitivities(CGeometry *geometry, CConfig *config, VectorType& x, bool write) {

  unsigned total_index, iPoint;

  for (auto iMarker = 0; iMarker < geometry->GetnMarker(); iMarker++) {
    if (config->GetMarker_All_DV(iMarker) == YES) {
      for (auto iVertex = 0; iVertex <geometry->nVertex[iMarker]; iVertex++) {
        iPoint = geometry->vertex[iMarker][iVertex]->GetNode();
        for (auto iDim = 0; iDim < nDim; iDim++){
          total_index = iPoint*nDim+iDim;
          if (write) {
            nodes->SetSensitivity(iPoint,iDim,x[total_index]);
          } else {
            x[total_index]=nodes->GetSensitivity(iPoint,iDim);
          }
        }
      }
    }
  }

}


MatrixType CGradientSmoothingSolver::GetStiffnessMatrix(CGeometry *geometry, CNumerics **numerics, CConfig *config) {

  Compute_StiffMatrix(geometry, numerics, config);

  if (config->GetSepDim()) {
    MatrixType largeMat = MatrixType::Zero(nDim*nPoint, nDim*nPoint);
    for (auto i=0; i<nPoint; i++) {
      for (auto j=0; j<nPoint; j++) {
        for (auto iDim=0; iDim<nDim; iDim++) {
          largeMat(nDim*i+iDim, nDim*j+iDim) = Jacobian.GetBlock(i,j,0,0);
        }
      }
    }
    return largeMat;
  } else {
    return (Jacobian.ConvertToEigen()).cast<su2double>();
  }
}


MatrixType CGradientSmoothingSolver::GetSurfaceStiffnessMatrix(CGeometry *geometry, CNumerics **numerics, CConfig *config, unsigned long val_marker) {

  /*--- Initialize the sparse matrix ---*/
  Jacobian.InitOwnConnectivity(geometry->nVertex[val_marker], 1, 1, val_marker, geometry, config);

  Compute_Surface_StiffMatrix(geometry, numerics, config, val_marker);

  // blow up the matrix to the fitting dimension
  auto mat = Jacobian.ConvertToEigen();
  MatrixType largeMat = MatrixType::Zero(nDim*mat.rows(), nDim*mat.cols());
  for (auto i=0; i<mat.rows(); i++) {
    for (auto j=0; j<mat.cols(); j++) {
      largeMat(2*i,2*j) = mat(i,j);
      largeMat(2*i+1,2*j+1) = mat(i,j);
    }
  }

  return largeMat;
}


void CGradientSmoothingSolver::SmoothCompleteSystem(CGeometry *geometry, CSolver *solver, CNumerics **numerics, CConfig *config, CVolumetricMovement *grid_movement, su2double *param_jacobi) {

  cout << endl << "Applying Sobolev Smoothing by assembling the whole system matrix." << endl;

  bool twoD = config->GetSmoothOnSurface();

  unsigned short nDV_Total= config->GetnDV_Total(), nVertex, nDim = geometry->GetnDim();
  unsigned long nPoint  = geometry->GetnPoint();

  // SysMat can be sized here
  // param_jacobi_eigen needs dynamic resize depending on volume or surface?
  // stiffness is overwritten (resized) by return
  MatrixType SysMat(nDV_Total, nDV_Total), stiffness, param_jacobi_eigen;

  if (twoD) {

    for (unsigned short iMarker = 0; iMarker < config->GetnMarker_All(); iMarker++) {
      if ( config->GetMarker_All_DV(iMarker) == YES ) {

        nVertex = geometry->nVertex[iMarker];

        param_jacobi_eigen.resize(nDV_Total, nVertex*nDim);

        // get the reduced parameterization jacobian
        for (auto iDVindex=0; iDVindex<nDV_Total; iDVindex++) {
          for (auto iVertex = 0; iVertex <nVertex; iVertex++) {
            auto iPoint = geometry->vertex[iMarker][iVertex]->GetNode();
            for (auto iDim = 0; iDim < nDim; iDim++){
              auto total_index = iPoint*nDim+iDim;
              param_jacobi_eigen(iDVindex, iVertex*nDim+iDim) = param_jacobi[iDVindex*nPoint*nDim + total_index];
            }
          }
        }

        // get the stiffness matrix for the 2D case
        stiffness = GetSurfaceStiffnessMatrix(geometry, numerics, config, iMarker);

      }
    }

  } else {

    // get the parameterization jacobian
    param_jacobi_eigen.resize(nDV_Total, nPoint*nDim);
    for (auto iDVindex=0; iDVindex<nDV_Total; iDVindex++) {
      for (auto iPoint = 0; iPoint <nPoint; iPoint++) {
        for (auto iDim = 0; iDim < nDim; iDim++){
          param_jacobi_eigen(iDVindex, iPoint*nDim+iDim) = param_jacobi[iDVindex*nPoint*nDim + iPoint*nDim + iDim];
        }
      }
    }

    // get the inverse stiffness matrix for the mesh movement
    CSysMatrix<su2mixedfloat>& linear_elasticity_stiffness = grid_movement->GetStiffnessMatrix(geometry, config, true);
    MatrixType surf2vol = ((linear_elasticity_stiffness.ConvertToEigen().cast<su2double>()).inverse()).cast<su2double>();

    // get the inner stiffness matrix
    stiffness = GetStiffnessMatrix(geometry, numerics, config);
    stiffness = stiffness * surf2vol;
    stiffness = surf2vol.transpose() * stiffness;
  }

  // calculate the overall system
  SysMat = param_jacobi_eigen * stiffness * param_jacobi_eigen.transpose();

  ofstream SysMatrix(config->GetObjFunc_Hess_FileName());
  SysMatrix << SysMat.format(CSVFormat);
  SysMatrix.close();

  // solve the system
  QRdecomposition QR(SysMat);
  VectorType b = Eigen::Map<VectorType, Eigen::Unaligned>(deltaP.data(), deltaP.size());
  VectorType x = QR.solve(b);

  deltaP = std::vector<su2double>(x.data(), x.data() + x.size());

  OutputDVGradient();

  //some lines for comparison and debugging only
  VectorType unit = VectorType::Zero(nDV_Total, 1);
  unit(0) = 1.0;
  ofstream firstReduced("firstReduced.dat");
  firstReduced << (param_jacobi_eigen.transpose()*unit).format(CSVFormat);
  firstReduced.close();

  ofstream firstVector("firstVector.dat");
  for (auto iPoint = 0; iPoint <nPoint; iPoint++) {
    for (auto iDim = 0; iDim < nDim; iDim++){
      firstVector << param_jacobi[iPoint*nDim+iDim] <<endl;
    }
  }
  firstVector.close();

}


void CGradientSmoothingSolver::ApplyGradientSmoothingDV(CGeometry *geometry, CSolver *solver, CNumerics **numerics, CConfig *config, CSurfaceMovement *surface_movement, CVolumetricMovement *grid_movement) {

  /// record the parameterization
  if (rank == MASTER_NODE)  cout << " calculate the original gradient" << endl;
  RecordParameterizationJacobian(geometry, config, surface_movement, activeCoord);

  /// calculate the original gradinet
  //CalculateOriginalGradient(geometry, grid_movement, config);

  /// compute the Hessian column by column
  if (rank == MASTER_NODE)  cout << " computing the system matrix line by line" << endl;

  /// variable declarations
  unsigned nDVtotal=config->GetnDV_Total();
  unsigned column;
  vector<su2double> seedvector(nDVtotal, 0.0);
  MatrixType hessian = MatrixType::Zero(nDVtotal, nDVtotal);

  /// get matrix vector product for this
  auto mat_vec = GetStiffnessMatrixVectorProduct(geometry, numerics, config);

  for (column=0; column<nDVtotal; column++) {

    if (rank == MASTER_NODE)  cout << "    working in column " << column << endl;

    /// create seeding
    std::fill(seedvector.begin(), seedvector.end(), 0.0);
    seedvector[column] = 1.0;

    // necessary for surface case!
    helperVecIn.SetValZero();
    helperVecOut.SetValZero();

    /// forward projection
    ProjectDVtoMesh(geometry, config, seedvector, helperVecIn);

    ofstream helperVecInStream("helperVecIn.dat");
    helperVecIn.printVec(helperVecInStream);
    helperVecInStream.close();

    /// matrix vector product in the middle
    if (config->GetSmoothOnSurface()) {

      /// for surface we need extra work
      for (unsigned short iMarker = 0; iMarker < config->GetnMarker_All(); iMarker++) {
        if ( config->GetMarker_All_DV(iMarker) == YES ) {
          /// reduce the vector
          for (auto iVertex = 0; iVertex <geometry->nVertex[iMarker]; iVertex++) {
            auto iPoint = geometry->vertex[iMarker][iVertex]->GetNode();
            for (auto iDim = 0; iDim < nDim; iDim++){
              matVecIn(iVertex,iDim) = helperVecIn(iPoint,iDim);
            }
          }

          ofstream matVecInStream("matVecIn.dat");
          matVecIn.printVec(matVecInStream);
          matVecInStream.close();

          mat_vec(matVecIn, matVecOut);

          ofstream matVecOutStream("matVecOut.dat");
          matVecOut.printVec(matVecOutStream);
          matVecOutStream.close();

          /// get full vector back
          for (auto iVertex = 0; iVertex <geometry->nVertex[iMarker]; iVertex++) {
            auto iPoint = geometry->vertex[iMarker][iVertex]->GetNode();
            for (auto iDim = 0; iDim < nDim; iDim++){
              helperVecOut(iPoint*nDim,iDim) = matVecOut(iVertex,iDim);
            }
          }
        }
      }
    } else {

      ///straight forward for volume case.
      mat_vec(helperVecIn, helperVecOut);
    }

    ofstream helperVecOutStream("helperVecOut.dat");
    helperVecOut.printVec(helperVecOutStream);
    helperVecOutStream.close();

    /// reverse projection
    ProjectMeshToDV(geometry, config, helperVecOut, seedvector);

    /// extract projected direction
    hessian.col(column) = Eigen::Map<VectorType, Eigen::Unaligned>(seedvector.data(), seedvector.size());
  }

  /// output the matrix
  ofstream SysMatrix(config->GetObjFunc_Hess_FileName());
  SysMatrix << hessian.format(CSVFormat);
  SysMatrix.close();

  /// calculate and output the treated gradient
  QRdecomposition QR(hessian);
  VectorType b = Eigen::Map<VectorType, Eigen::Unaligned>(deltaP.data(), deltaP.size());
  VectorType x = QR.solve(b);

  deltaP = std::vector<su2double>(x.data(), x.data() + x.size());

  OutputDVGradient();
}


CSysMatrixVectorProduct<su2mixedfloat> CGradientSmoothingSolver::GetStiffnessMatrixVectorProduct(CGeometry *geometry, CNumerics **numerics, CConfig *config) {

  bool twoD = config->GetSmoothOnSurface();
  if (twoD) {
    for (unsigned short iMarker = 0; iMarker < config->GetnMarker_All(); iMarker++) {
      if ( config->GetMarker_All_DV(iMarker) == YES ) {

        /*--- Initialize the sparse matrix ---*/
        Jacobian.InitOwnConnectivity(geometry->nVertex[iMarker], nDim-1, nDim-1, iMarker, geometry, config);
        Compute_Surface_StiffMatrix(geometry, numerics, config, iMarker, nDim-1);

        // don't forget to initialize the vectors to the correct size.
        matVecIn.Initialize(geometry->nVertex[iMarker], geometry->nVertex[iMarker], nDim-1, 0.0);
        matVecOut.Initialize(geometry->nVertex[iMarker], geometry->nVertex[iMarker], nDim-1, 0.0);
      }
    }
  } else {
      Compute_StiffMatrix(geometry, numerics, config);
  }

  return CSysMatrixVectorProduct<su2mixedfloat>(Jacobian, geometry, config);
}
