/*!
 * \file dual_grid_structure.inl
 * \brief In-Line subroutines of the <i>dual_grid_structure.hpp</i> file.
 * \author F. Palacios, T. Economon
 * \version 4.0.0 "Cardinal"
 *
 * SU2 Lead Developers: Dr. Francisco Palacios (Francisco.D.Palacios@boeing.com).
 *                      Dr. Thomas D. Economon (economon@stanford.edu).
 *
 * SU2 Developers: Prof. Juan J. Alonso's group at Stanford University.
 *                 Prof. Piero Colonna's group at Delft University of Technology.
 *                 Prof. Nicolas R. Gauger's group at Kaiserslautern University of Technology.
 *                 Prof. Alberto Guardone's group at Polytechnic University of Milan.
 *                 Prof. Rafael Palacios' group at Imperial College London.
 *
 * Copyright (C) 2012-2015 SU2, the open-source CFD code.
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

#pragma once

inline void CPoint::SetElem(unsigned long val_elem) { Elem.push_back(val_elem); nElem = Elem.size(); }

inline void CPoint::ResetBoundary(void) { if (vertex != NULL) delete [] vertex; Boundary = false; }

inline void CPoint::ResetElem(void) { Elem.clear(); nElem = 0; }

inline void CPoint::ResetPoint(void) { Point.clear(); Edge.clear(); nPoint = 0; }

inline su2double CPoint::GetCoord(unsigned short val_dim) { return coord[val_dim]; }

inline su2double *CPoint::GetCoord(void) { return coord; }

inline bool CPoint::GetFlip_Orientation(void) { return Flip_Orientation; }

inline void CPoint::SetCoord(unsigned short val_dim, su2double val_coord) { coord[val_dim] = val_coord; }

inline void CPoint::SetFlip_Orientation(void) { Flip_Orientation = true; }

inline void CPoint::AddCoord(unsigned short val_dim, su2double val_coord) { coord[val_dim] += val_coord; }

inline void CPoint::SetCoord(su2double *val_coord) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++) 
		coord[iDim]=val_coord[iDim]; 
}

inline void CPoint::SetnElem(unsigned short val_nElem) { nElem = val_nElem; }

inline unsigned short CPoint::GetnElem(void) { return nElem; }

inline void CPoint::SetEdge(long val_edge, unsigned short val_nedge) { Edge[val_nedge] = val_edge; }

inline unsigned long CPoint::GetElem(unsigned short val_elem) {	return Elem[val_elem]; }

inline long CPoint::GetEdge(unsigned short val_edge) { return Edge[val_edge]; }

inline void CPoint::SetnPoint(unsigned short val_nPoint) { nPoint = val_nPoint; }

inline unsigned short CPoint::GetnPoint(void) {	return nPoint; }

inline unsigned long CPoint::GetPoint(unsigned short val_point) { return Point[val_point]; }

inline su2double CPoint::GetVolume (void) { return Volume[0]; }

inline bool CPoint::GetMove (void) { return Move; }

inline bool CPoint::GetBoundary(void) { return Boundary; }

inline void CPoint::SetBoundary(bool val_boundary) { Boundary = val_boundary; }

inline void CPoint::SetPhysicalBoundary(bool val_boundary) { PhysicalBoundary = val_boundary; }

inline bool CPoint::GetPhysicalBoundary(void) { return PhysicalBoundary; }

inline void CPoint::SetSolidBoundary(bool val_boundary) { SolidBoundary = val_boundary; }

inline bool CPoint::GetSolidBoundary(void) { return SolidBoundary; }

inline void CPoint::AddVolume (su2double val_Volume) { Volume[0] += val_Volume; }

inline void CPoint::SetVolume (su2double val_Volume) { Volume[0] = val_Volume; }

inline void CPoint::SetMove(bool val_move) { Move = val_move; }

inline su2double *CPoint::GetCoord_Old(void) { return Coord_old; }

inline su2double *CPoint::GetCoord_Sum(void) { return Coord_sum; }

inline su2double *CPoint::GetGridVel(void) { return GridVel; }

inline su2double **CPoint::GetGridVel_Grad(void) { return GridVel_Grad; }

inline void CPoint::SetCoord_Old(su2double *val_coord_old) {
	for (unsigned short iDim = 0; iDim < nDim; iDim++)
		Coord_old[iDim] = val_coord_old[iDim]; 
}

inline void CPoint::SetCoord_SumZero(void) {
	for (unsigned short iDim = 0; iDim < nDim; iDim++)
	Coord_sum[iDim] = 0.0; 
}

inline void CPoint::AddCoord_Sum(su2double *val_coord_sum) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++) 
		Coord_sum[iDim] += val_coord_sum[iDim]; 
}

inline void CPoint::SetGridVel(unsigned short val_dim, su2double val_gridvel) { GridVel[val_dim] = val_gridvel; }

inline void CPoint::SetGridVel_Grad(unsigned short val_var, unsigned short val_dim, su2double val_value) { GridVel_Grad[val_var][val_dim] = val_value; }

inline void CPoint::SetChildren_CV (unsigned short val_nchildren_CV, unsigned long val_children_CV) {
	if (Children_CV.size() <= val_nchildren_CV) Children_CV.resize(val_nchildren_CV+1);
	Children_CV[val_nchildren_CV] = val_children_CV; 
}

inline unsigned short CPoint::GetnNodes() { return 0; }

inline unsigned long CPoint::GetParent_CV (void) { return Parent_CV; }

inline unsigned long CPoint::GetChildren_CV (unsigned short val_nchildren_CV) {	return Children_CV[val_nchildren_CV]; }

inline bool CPoint::GetAgglomerate (void) { return Agglomerate; }

inline bool CPoint::GetAgglomerate_Indirect (void) { return Agglomerate_Indirect; }

inline void CPoint::SetAgglomerate_Indirect(bool val_agglomerate) { Agglomerate_Indirect = val_agglomerate; };

inline void CPoint::SetVertex(long val_vertex, unsigned short val_nmarker) { 
	if (Boundary) vertex[val_nmarker] = val_vertex; 
}

inline unsigned short CPoint::GetnChildren_CV (void) { return nChildren_CV; }

inline long CPoint::GetVertex(unsigned short val_marker) { 
	if (Boundary) return vertex[val_marker]; 
	else return -1; 
}

inline void CPoint::SetnChildren_CV (unsigned short val_nchildren_CV) {	nChildren_CV = val_nchildren_CV; }

inline void CPoint::SetParent_CV (unsigned long val_parent_CV) { Parent_CV = val_parent_CV; Agglomerate = true; }

inline void CPoint::SetGridVel(su2double *val_gridvel) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++)
		GridVel[iDim] = val_gridvel[iDim];
}

inline void CPoint::SetVolume_n (void) { Volume[1] = Volume[0]; }

inline void CPoint::SetVolume_nM1 (void) { Volume[2] = Volume[1]; }

inline su2double CPoint::GetVolume_n (void) { return Volume[1]; }

inline su2double CPoint::GetVolume_nM1 (void) { return Volume[2]; }

inline void CPoint::SetCoord_n (void) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++)
		Coord_n[iDim] = coord[iDim]; 
}

inline void CPoint::SetCoord_n1 (void) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++)
		Coord_n1[iDim] = Coord_n[iDim]; 
}

inline void CPoint::SetCoord_p1(su2double *val_coord) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++)
		Coord_p1[iDim] = val_coord[iDim]; 
}

inline su2double *CPoint::GetCoord_n (void) { return Coord_n; }

inline su2double *CPoint::GetCoord_n1 (void) { return Coord_n1; }

inline su2double *CPoint::GetCoord_p1 (void) { return Coord_p1; }

inline void CPoint::SetColor(unsigned short val_color) { color = val_color; }

inline void CPoint::SetnNeighbor(unsigned short val_nneighbor) { nNeighbor = val_nneighbor; }

inline unsigned short CPoint::GetnNeighbor(void) { return nNeighbor; }

inline unsigned short CPoint::GetColor(void) { return color; }

inline unsigned long CPoint::GetGlobalIndex(void) { return GlobalIndex; }

inline void CPoint::SetGlobalIndex(unsigned long val_globalindex) { GlobalIndex = val_globalindex; }

inline void CPoint::SetDomain(bool val_domain) { Domain = val_domain; }

inline bool CPoint::GetDomain(void) { return Domain; }

inline void CPoint::SetWall_Distance(su2double val_distance) { Wall_Distance = val_distance; }

inline void CPoint::SetCurvature(su2double val_curvature) { Curvature = val_curvature; }

inline void CPoint::SetSharpEdge_Distance(su2double val_distance) { SharpEdge_Distance = val_distance; }

inline su2double CPoint::GetWall_Distance(void) { return Wall_Distance; }

inline su2double CPoint::GetCurvature(void) { return Curvature; }

inline su2double CPoint::GetSharpEdge_Distance(void) { return SharpEdge_Distance; }

inline void CPoint::SetNodes_Coord(su2double *val_coord_Edge_CG, su2double *val_coord_FaceElem_CG, su2double *val_coord_Elem_CG) { }

inline void CPoint::SetNodes_Coord(su2double *val_coord_Edge_CG, su2double *val_coord_Elem_CG) { }

inline void  CPoint::GetNormal(su2double *val_normal) { }

inline su2double *CPoint::GetNormal(void) { return 0; }

inline void CPoint::SetNormal(su2double *val_face_normal) { }

inline void CPoint::SetZeroValues(void) { }

inline void CPoint::AddNormal(su2double *val_face_normal) { }

inline unsigned short CEdge::GetnNodes() { return 2; }

inline unsigned long CEdge::GetNode(unsigned short val_node) { return Nodes[val_node]; }

inline su2double CEdge::GetCG(unsigned short val_dim) { return Coord_CG[val_dim]; }

inline su2double *CEdge::GetNormal(void) {	return Normal; }

inline void CEdge::GetNormal(su2double *val_normal) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++) 
		val_normal[iDim] = Normal[iDim]; 
}

inline void CEdge::SetNormal(su2double *val_face_normal) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++) 
		Normal[iDim]=val_face_normal[iDim]; 
}

inline void CEdge::AddNormal(su2double *val_face_normal) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++) 
		Normal[iDim] += val_face_normal[iDim];
}

inline void CEdge::SetZeroValues(void) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim ++) 
		Normal[iDim] = 0.0;
}

inline su2double *CEdge::GetCoord(void) { return NULL; }

inline void CEdge::SetCoord(su2double *val_coord) { }

inline unsigned short CVertex::GetnNodes() { return 1; }

inline unsigned long CVertex::GetNode() { return Nodes[0]; }

inline su2double *CVertex::GetNormal(void) { return Normal; }

inline su2double *CVertex::GetVarCoord(void) { return VarCoord; }

inline su2double *CVertex::GetCoord(void) { return CartCoord; }

inline su2double CVertex::GetCoord(unsigned short val_dim) { return CartCoord[val_dim]; }

inline void CVertex::SetAuxVar(su2double val_auxvar) { Aux_Var = val_auxvar; }

inline void CVertex::AddAuxVar(su2double val_auxvar) { Aux_Var += val_auxvar; }

inline su2double CVertex::GetAuxVar(void) { return Aux_Var; }

inline void CVertex::GetNormal(su2double *val_normal) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++) 
		val_normal[iDim] = Normal[iDim]; 
}

inline void CVertex::SetNormal(su2double *val_face_normal) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++) 
		Normal[iDim]=val_face_normal[iDim];
}

inline void CVertex::SetVarCoord(su2double *val_varcoord) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++) 
		VarCoord[iDim] = val_varcoord[iDim];
}

inline void CVertex::AddVarCoord(su2double *val_varcoord) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++) 
		VarCoord[iDim] += val_varcoord[iDim];
}

inline void CVertex::SetCoord(su2double *val_coord) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim++) 
		CartCoord[iDim] = val_coord[iDim];
}

inline void CVertex::SetRotation_Type(short val_rotation_type) { Rotation_Type = val_rotation_type; }

inline short CVertex::GetRotation_Type(void) { return Rotation_Type; }

inline void CVertex::SetDonorPoint(long val_periodicpoint, long val_processor) { 
	PeriodicPoint[0] = val_periodicpoint; 
	PeriodicPoint[1] = val_processor; 
}

inline void CVertex::SetDonorElem(long val_donorelem) { Donor_Elem = val_donorelem; }

inline long CVertex::GetDonorElem(void) { return Donor_Elem; }

inline long CVertex::GetDonorPoint(void) { return PeriodicPoint[0]; }

inline long CVertex::GetDonorProcessor(void) { return PeriodicPoint[1]; }

inline void CVertex::SetBasisFunction(unsigned short val_node, su2double val_basis) { Basis_Function[val_node] = val_basis; }

inline su2double CVertex::GetBasisFunction(unsigned short val_node) { return Basis_Function[val_node]; }

inline long *CVertex::GetPeriodicPointDomain(void) { return PeriodicPoint; }

inline void CVertex::SetZeroValues(void) { 
	for (unsigned short iDim = 0; iDim < nDim; iDim ++) 
		Normal[iDim] = 0.0; 
}

inline unsigned long CVertex::GetNormal_Neighbor(void) { return Normal_Neighbor; }

inline void CVertex::SetNormal_Neighbor(unsigned long val_Normal_Neighbor) { Normal_Neighbor = val_Normal_Neighbor; }

inline void CVertex::IncrementnDonor(void){nDonor_Points++;}

inline void CVertex::SetDonorInfo(unsigned short iDonor, unsigned long *val){
  for ( unsigned short it =0; it<4; it++) Donor_Info[iDonor][it]=val[it];
}

inline void CVertex::SetDonorCoeff(unsigned short iDonor, su2double val){ Donor_Coeff[iDonor] = val; }

inline unsigned long CVertex::GetDonorInfo(unsigned short iDonor, unsigned short it){ return Donor_Info[iDonor][it];}

inline su2double CVertex::GetDonorCoeff(unsigned short iDonor){ return Donor_Coeff[iDonor];}

inline unsigned short CVertex::GetnDonorPoints(void){ return nDonor_Points;}

inline void CVertex::SetnDonorPoints(unsigned short nDonor) {nDonor_Points = nDonor;}

inline su2double *CVertex::GetVarRot(void) {return VarRot;}

inline void CVertex::SetVarRot(su2double* val) {
  for (unsigned short iDim = 0; iDim < nDim; iDim++)
    VarRot[iDim] = val[iDim];
}
