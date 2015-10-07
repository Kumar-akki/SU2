/*!
 * \file interpolation_structure.cpp
 * \brief Main subroutines used by SU2_FSI
 * \author H. Kline
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

#include "../include/interpolation_structure.hpp"

CInterpolator::CInterpolator(void){

	nZone = 0;
	Geometry = NULL;

	donor_geometry  = NULL;
	target_geometry = NULL;

	donorZone  = 0;
	targetZone = 0;

}

CInterpolator::~CInterpolator(void){}


CInterpolator::CInterpolator(CGeometry ***geometry_container, CConfig **config, unsigned int iZone, unsigned int jZone){

  /* Store pointers*/
	Geometry = geometry_container;

	nZone = 2;

	donorZone  = iZone;
	targetZone = jZone;

	donor_geometry  = geometry_container[donorZone][MESH_0];
	target_geometry = geometry_container[targetZone][MESH_0];

  /*--- Initialize transfer coefficients between the zones ---*/
	/* Since this is a virtual function, call it in the child class constructor  */
  //Set_TransferCoeff(targetZone,donorZone,config);
  /*--- Initialize transfer coefficients between the zones ---*/
  //Set_TransferCoeff(Zones,config);

}

inline void CInterpolator::Set_TransferCoeff(CConfig **config) { }

/*
void CInterpolator::InitializeData(unsigned int* Zones, unsigned short val_nVar){
  nVar=val_nVar;
  unsigned int iZone;
  unsigned short it;
  if (nVar>0){
    //--- Initialize Data vectors to 0 ---//
    Data = new su2double**[nZone];
    for (it=0; it<nZone; it++){
      iZone = Zones[it];
      Data[iZone] = new su2double*[Geometry[iZone][MESH_0]->GetnPoint()];
      for (unsigned long iPoint =0; iPoint< Geometry[iZone][MESH_0]->GetnPoint(); iPoint++){
        Data[iZone][iPoint] = new su2double[nVar];
        for (unsigned short iVar=0; iVar<nVar; iVar++){
          Data[iZone][iPoint][iVar]=0.0;
        }
      }
    }
  }else{
    Data = NULL;
  }

}
*/

//void CInterpolator::Interpolate_Data(unsigned int iZone, CConfig **config){
//  unsigned long iPoint, jPoint, jVertex, iMarker, iVertex;
//  unsigned short jMarker;
//  unsigned int donorZone;
//  su2double weight=0.0;
//
//  /*--- Loop through points, increment Data in the input zone by the weight in the transfer matrix ---*/
//
//  /*Loop by i then by j to more efficiently call memory*/
//  for (iMarker = 0; iMarker < config[iZone]->GetnMarker_All(); iMarker++){
//    if (config[iZone]->GetMarker_All_FSIinterface(iMarker) == YES){
//      for (iVertex = 0; iVertex<Geometry[iZone][MESH_0]->GetnVertex(iMarker); iVertex++) {
//        iPoint =Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetNode();
//        /*--- Set Data to 0 before interpolation ---*/
//        for (unsigned short iVar=0; iVar<nVar; iVar++){
//          Data[iZone][iPoint][iVar]=0;
//        }
//        /*--- Interpolate ---*/
//        for (unsigned short jDonor = 0; jDonor< Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetnDonorPoints(); jDonor++){
//          /* Unpack info about the donor point */
//          donorZone = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetDonorInfo(jDonor,0);
//          jPoint = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetDonorInfo(jDonor,1);
//          jMarker = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetDonorInfo(jDonor,2);
//          jVertex = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetDonorInfo(jDonor,3);
//          weight = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetDonorCoeff(jDonor);
//          /*--- Increment the value of the data ---*/
//          for (unsigned short iVar=0; iVar<nVar; iVar++){
//            Data[iZone][iPoint][iVar]+=Data[donorZone][jPoint][iVar]*weight;
//          }
//        }
//      }
//    }
//  }
//}
//
//void CInterpolator::Interpolate_Deformation(unsigned int iZone, CConfig **config){
//
//  unsigned long GlobalIndex, iPoint, i2Point, jPoint, j2Point, iVertex, jVertex;
//  unsigned short iMarker, jMarker, iDim;
//  unsigned int donorZone;
//  su2double *NewVarCoord = NULL, *VarCoord, *VarRot, *dist = NULL;
//  su2double weight;
//  unsigned short nDim = Geometry[iZone][MESH_0]->GetnDim();
//  /*--- Loop over vertices in the interface marker (zone 0) ---*/
//  for (iMarker = 0; iMarker < config[iZone]->GetnMarker_All(); iMarker++){
//    if (config[iZone]->GetMarker_All_FSIinterface(iMarker) == YES){
//      for (iVertex = 0; iVertex<Geometry[iZone][MESH_0]->GetnVertex(iMarker); iVertex++) {
//        iPoint =Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetNode();
//        /*--- Set NewCoord to 0 ---*/
//        for (iDim=0; iDim<nDim; iDim++) NewVarCoord[iDim]=0.0;
//        /*--- Loop over vertices in the interface marker (zone 1) --*/
//        for (unsigned short jDonor = 0; jDonor< Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetnDonorPoints(); jDonor++){
//          donorZone = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetDonorInfo(jDonor,0);
//          jPoint = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetDonorInfo(jDonor,1);
//          jMarker = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetDonorInfo(jDonor,2);
//          jVertex = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetDonorInfo(jDonor,3);
//          weight = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetDonorCoeff(jDonor);
//          /* Get translation and rotation from the solution */
//          VarCoord = donor_geometry->vertex[jMarker][jVertex]->GetVarCoord();
//          VarRot =   donor_geometry->vertex[jMarker][jVertex]->GetVarRot();
//
//          for (iDim=0; iDim<nDim; iDim++) dist[iDim]=0.0;
//
//          for (iDim=0; iDim<nDim; iDim++){
//            NewVarCoord[iDim]+=VarCoord[iDim]*weight;
//            dist[iDim] = Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->GetCoord(iDim)-donor_geometry->node[jPoint]->GetCoord(iDim);
//          }
//          /*--- Add contribution of rotation (cross product of donor point rotation and dist to donor point) ---*/
//          if (nDim==2){
//            NewVarCoord[0]+=weight*(-dist[1]*VarRot[2]);
//            NewVarCoord[1]+=weight*(dist[0]*VarRot[2]);
//          }
//          if (nDim==3){
//            NewVarCoord[0]+=weight*(dist[2]*VarRot[1]-dist[1]*VarRot[2]);
//            NewVarCoord[1]+=weight*(dist[0]*VarRot[2]-dist[2]*VarRot[0]);
//            NewVarCoord[2]+=weight*(dist[1]*VarRot[0]-dist[0]*VarRot[1]);
//          }
//        }
//        /*--- Set the varcoord information ---*/
//        Geometry[iZone][MESH_0]->vertex[iMarker][iVertex]->SetVarCoord(NewVarCoord);
//      }
//    }
//  }
//
//  // must be called later:
//  //flow_grid_movement->SetVolume_Deformation(Geometry[ZONE_0][MESH_0], config[ZONE_0], true);
//
//}
//
//su2double CInterpolator::GetData(unsigned int iZone, unsigned long iPoint, unsigned short iVar){
//  if (Data !=NULL)
//    return Data[iZone][iPoint][iVar];
//  else
//    return 0.0; // Check this.
//}
//
//su2double* CInterpolator::GetData(unsigned int iZone, unsigned long iPoint){
//  if (Data !=NULL)
//    return Data[iZone][iPoint];
//  else
//    return NULL;
//}
//
//void CInterpolator::SetData(unsigned int iZone, unsigned long iPoint, unsigned short iVar, su2double val){
//  if (Data !=NULL)
//    Data[iZone][iPoint][iVar]=val;
//  else
//    cout <<" CInterpolator object has not been initialized"<<endl;
//}

CNearestNeighbor::CNearestNeighbor(void):  CInterpolator(){ }

/* Nearest Neighbor Interpolator */
CNearestNeighbor::CNearestNeighbor(CGeometry ***geometry_container, CConfig **config,  unsigned int iZone, unsigned int jZone) :  CInterpolator(geometry_container, config, iZone, jZone){

  /*--- Initialize transfer coefficients between the zones ---*/
  Set_TransferCoeff(config);

  /*--- For fluid-structure interaction data interpolated with have nDim dimensions ---*/
  //InitializeData(Zones,nDim);

}

CNearestNeighbor::~CNearestNeighbor(){}

void CNearestNeighbor::Set_TransferCoeff(CConfig **config){

  unsigned long iVertex, jVertex;
  unsigned short iDim;
  unsigned short nDim = donor_geometry->GetnDim();

  unsigned short nMarkerInt, nMarkerDonor, nMarkerTarget;		// Number of markers on the interface, donor and target side
  unsigned short iMarkerInt, iMarkerDonor, iMarkerTarget;		// Variables for iteration over markers
  int markDonor = -1, markTarget = -1;

  unsigned long nVertexDonor = 0, nVertexTarget= 0;
  unsigned long Point_Donor = 0, Point_Target = 0;
  unsigned long Vertex_Donor = 0, Vertex_Target = 0;

  unsigned long iVertexDonor, iPointDonor = 0;
  unsigned long iVertexTarget, iPointTarget = 0;
  unsigned long pPoint = 0, Global_Point = 0;
  unsigned long jGlobalPoint = 0, pGlobalPoint = 0;
  int iProcessor, pProcessor = 0;

  unsigned long nLocalVertex_Donor = 0, nLocalVertex_Target = 0;
  unsigned long nGlobalVertex_Donor = 0, MaxLocalVertex_Donor = 0;

  unsigned long Global_Point_Donor;
  int Donor_Processor;
  unsigned short int donorindex = 0;

  /*--- Number of markers on the FSI interface ---*/
  nMarkerInt     = (config[donorZone]->GetMarker_n_FSIinterface())/2;
  nMarkerTarget  = target_geometry->GetnMarker();
  nMarkerDonor   = donor_geometry->GetnMarker();

  su2double *Coord_i, Coord_j[3], dist = 0.0, mindist, maxdist;

  int rank = MASTER_NODE;
  int nProcessor = SINGLE_NODE;

#ifdef HAVE_MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcessor);
#endif

  // For the markers on the interface
  for (iMarkerInt = 1; iMarkerInt <= nMarkerInt; iMarkerInt++) {

	  markDonor = -1;
	  markTarget = -1;

	  /*--- On the donor side ---*/

	  for (iMarkerDonor = 0; iMarkerDonor < nMarkerDonor; iMarkerDonor++){
		  /*--- If the tag GetMarker_All_FSIinterface(iMarkerDonor) equals the index we are looping at ---*/
		  if (config[donorZone]->GetMarker_All_FSIinterface(iMarkerDonor) == iMarkerInt ){
			  /*--- We have identified the identifier for the structural marker ---*/
			  markDonor = iMarkerDonor;
			  /*--- Store the number of local points that belong to markDonor ---*/
			  nVertexDonor = donor_geometry->GetnVertex(iMarkerDonor);
			  break;
		  }
		  else {
			  /*--- If the tag hasn't matched any tag within the donor markers ---*/
			  markDonor = -1;
			  nVertexDonor = 0;
		  }
	  }

	  /*--- On the target side ---*/
	  for (iMarkerTarget = 0; iMarkerTarget < nMarkerTarget; iMarkerTarget++){
		  /*--- If the tag GetMarker_All_FSIinterface(iMarkerFlow) equals the index we are looping at ---*/
		  if (config[targetZone]->GetMarker_All_FSIinterface(iMarkerTarget) == iMarkerInt ){
			  /*--- We have identified the identifier for the target marker ---*/
			  markTarget = iMarkerTarget;
			  /*--- Store the number of local points that belong to markTarget ---*/
			  nVertexTarget = target_geometry->GetnVertex(iMarkerTarget);
			  break;
		  }
		  else {
			  /*--- If the tag hasn't matched any tag within the Flow markers ---*/
			  nVertexTarget = 0;
			  markTarget = -1;
		  }
	  }

	  unsigned long *Buffer_Send_nVertex_Donor = new unsigned long [1];
	  unsigned long *Buffer_Receive_nVertex_Donor = new unsigned long [nProcessor];

	  unsigned long *Buffer_Send_nVertex_Target = new unsigned long [1];
	  unsigned long *Buffer_Receive_nVertex_Target = new unsigned long [nProcessor];

	  nLocalVertex_Donor = 0;
	  for (iVertex = 0; iVertex < nVertexDonor; iVertex++) {
		iPointDonor = donor_geometry->vertex[markDonor][iVertex]->GetNode();
		if (donor_geometry->node[iPointDonor]->GetDomain()) nLocalVertex_Donor++;
	  }

	  nLocalVertex_Target = 0;
	  for (iVertex = 0; iVertex < nVertexTarget; iVertex++) {
		iPointTarget = target_geometry->vertex[markTarget][iVertex]->GetNode();
		if (target_geometry->node[iPointTarget]->GetDomain()) nLocalVertex_Target++;
	  }

	  Buffer_Send_nVertex_Donor[0] = nLocalVertex_Donor;
	  Buffer_Send_nVertex_Target[0] = nLocalVertex_Target;

	  /*--- Send Interface vertex information --*/
#ifdef HAVE_MPI
	  SU2_MPI::Allreduce(&nLocalVertex_Donor, &nGlobalVertex_Donor, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
	  SU2_MPI::Allreduce(&nLocalVertex_Donor, &MaxLocalVertex_Donor, 1, MPI_UNSIGNED_LONG, MPI_MAX, MPI_COMM_WORLD);
	  SU2_MPI::Allgather(Buffer_Send_nVertex_Donor, 1, MPI_UNSIGNED_LONG, Buffer_Receive_nVertex_Donor, 1, MPI_UNSIGNED_LONG, MPI_COMM_WORLD);
	  SU2_MPI::Allgather(Buffer_Send_nVertex_Target, 1, MPI_UNSIGNED_LONG, Buffer_Receive_nVertex_Target, 1, MPI_UNSIGNED_LONG, MPI_COMM_WORLD);
#else
	  nGlobalVertex_Donor 		= nLocalVertex_Donor;
	  MaxLocalVertex_Donor 		= nLocalVertex_Donor;
	  Buffer_Receive_nVertex_Donor[0] = Buffer_Send_nVertex_Donor[0];
	  Buffer_Receive_nVertex_Target[0] = Buffer_Send_nVertex_Target[0];
#endif

	  su2double *Buffer_Send_Coord = new su2double [MaxLocalVertex_Donor*nDim];
	  unsigned long *Buffer_Send_Point = new unsigned long [MaxLocalVertex_Donor];
	  unsigned long *Buffer_Send_GlobalPoint = new unsigned long [MaxLocalVertex_Donor];

	  su2double *Buffer_Receive_Coord = new su2double [nProcessor*MaxLocalVertex_Donor*nDim];
	  unsigned long *Buffer_Receive_Point = new unsigned long [nProcessor*MaxLocalVertex_Donor];
	  unsigned long *Buffer_Receive_GlobalPoint = new unsigned long [nProcessor*MaxLocalVertex_Donor];

	  unsigned long nBuffer_Coord = MaxLocalVertex_Donor*nDim;
	  unsigned long nBuffer_Point = MaxLocalVertex_Donor;

	  for (iVertex = 0; iVertex < MaxLocalVertex_Donor; iVertex++) {
		Buffer_Send_Point[iVertex] = 0;
		Buffer_Send_GlobalPoint[iVertex] = 0;
		for (iDim = 0; iDim < nDim; iDim++)
		  Buffer_Send_Coord[iVertex*nDim+iDim] = 0.0;
	  }

	  /*--- Copy coordinates and point to the auxiliar vector --*/
	  nLocalVertex_Donor = 0;
		for (iVertexDonor = 0; iVertexDonor < nVertexDonor; iVertexDonor++) {
		  iPointDonor = donor_geometry->vertex[markDonor][iVertexDonor]->GetNode();
		  if (donor_geometry->node[iPointDonor]->GetDomain()) {
			Buffer_Send_Point[nLocalVertex_Donor] = iPointDonor;
			Buffer_Send_GlobalPoint[nLocalVertex_Donor] = donor_geometry->node[iPointDonor]->GetGlobalIndex();
			for (iDim = 0; iDim < nDim; iDim++)
			  Buffer_Send_Coord[nLocalVertex_Donor*nDim+iDim] = donor_geometry->node[iPointDonor]->GetCoord(iDim);
			nLocalVertex_Donor++;
		  }
		}

#ifdef HAVE_MPI
	  SU2_MPI::Allgather(Buffer_Send_Coord, nBuffer_Coord, MPI_DOUBLE, Buffer_Receive_Coord, nBuffer_Coord, MPI_DOUBLE, MPI_COMM_WORLD);
	  SU2_MPI::Allgather(Buffer_Send_Point, nBuffer_Point, MPI_UNSIGNED_LONG, Buffer_Receive_Point, nBuffer_Point, MPI_UNSIGNED_LONG, MPI_COMM_WORLD);
	  SU2_MPI::Allgather(Buffer_Send_GlobalPoint, nBuffer_Point, MPI_UNSIGNED_LONG, Buffer_Receive_GlobalPoint, nBuffer_Point, MPI_UNSIGNED_LONG, MPI_COMM_WORLD);
#else
	  for (iVertex = 0; iVertex < nBuffer_Coord; iVertex++){
		  Buffer_Receive_Coord[iVertex] = Buffer_Send_Coord[iVertex];
	  }
	  for (iVertex = 0; iVertex < nBuffer_Point; iVertex++){
		  Buffer_Receive_Point[iVertex] = Buffer_Send_Point[iVertex];
		  Buffer_Receive_GlobalPoint[iVertex] = Buffer_Send_GlobalPoint[iVertex];
	  }
#endif

	  /*--- Compute the closest point to a Near-Field boundary point ---*/
	  maxdist = 0.0;
	  for (iVertexTarget = 0; iVertexTarget < nVertexTarget; iVertexTarget++) {

		  Point_Target = target_geometry->vertex[markTarget][iVertexTarget]->GetNode();

		  if (target_geometry->node[Point_Target]->GetDomain()) {

			  target_geometry->vertex[markTarget][iVertexTarget]->SetnDonorPoints(1);
			  target_geometry->vertex[markTarget][iVertexTarget]->Allocate_DonorInfo();

			  /*--- Coordinates of the boundary point ---*/
			  Coord_i = target_geometry->node[Point_Target]->GetCoord();
			  mindist = 1E6; pProcessor = 0; pPoint = 0;

			  /*--- Loop over all the boundaries to find the pair ---*/
			  for (iProcessor = 0; iProcessor < nProcessor; iProcessor++){
				  for (jVertex = 0; jVertex < Buffer_Receive_nVertex_Donor[iProcessor]; jVertex++) {
					  Point_Donor = Buffer_Receive_Point[iProcessor*MaxLocalVertex_Donor+jVertex];
					  Global_Point_Donor = Buffer_Receive_GlobalPoint[iProcessor*MaxLocalVertex_Donor+jVertex];

					  /*--- Compute the dist ---*/
					  dist = 0.0; for (iDim = 0; iDim < nDim; iDim++) {
						  Coord_j[iDim] = Buffer_Receive_Coord[(iProcessor*MaxLocalVertex_Donor+jVertex)*nDim+iDim];
						  dist += pow(Coord_j[iDim]-Coord_i[iDim],2.0);
					  }

//					  if (((dist < mindist) && (iProcessor != rank)) ||
//						  ((dist < mindist) && (iProcessor == rank) && (Point_Donor != Point_Target))) {
					  if (dist < mindist) {
						  mindist = dist; pProcessor = iProcessor; pGlobalPoint = Global_Point_Donor;
					  }

					  if (dist == 0.0) break;
				  }
			  }

			  /*--- Store the value of the pair ---*/
			  maxdist = max(maxdist, mindist);
			  target_geometry->vertex[markTarget][iVertexTarget]->SetInterpDonorPoint(donorindex, pGlobalPoint);
			  target_geometry->vertex[markTarget][iVertexTarget]->SetInterpDonorProcessor(donorindex, pProcessor);
			  target_geometry->vertex[markTarget][iVertexTarget]->SetDonorCoeff(donorindex,1.0);

//	      unsigned long gpoint = target_geometry->vertex[markTarget][iVertexTarget]->GetNode();
//	      cout <<" Nearest Neighbor for target g.i " << target_geometry->node[gpoint]->GetGlobalIndex() <<" is "<< pGlobalPoint << "; d = " << mindist<< endl;
		  }
	  }

	  delete[] Buffer_Send_Coord;
	  delete[] Buffer_Send_Point;
	  delete[] Buffer_Send_GlobalPoint;

	  delete[] Buffer_Receive_Coord;
	  delete[] Buffer_Receive_Point;
	  delete[] Buffer_Receive_GlobalPoint;

	  delete[] Buffer_Send_nVertex_Donor;
	  delete[] Buffer_Receive_nVertex_Donor;
	  delete[] Buffer_Send_nVertex_Target;
	  delete[] Buffer_Receive_nVertex_Target;

  }

}


CIsoparametric::CIsoparametric(CGeometry ***geometry_container, CConfig **config, unsigned int iZone, unsigned int jZone)  :  CInterpolator(geometry_container, config, iZone, jZone){

  /*--- Initialize transfer coefficients between the zones ---*/
  Set_TransferCoeff(config);

  /*--- For fluid-structure interaction data interpolated with have nDim dimensions ---*/
 // InitializeData(Zones,nDim);
}

CIsoparametric::~CIsoparametric(){}

void CIsoparametric::Set_TransferCoeff(CConfig **config){
  unsigned long jPoint, iVertex, jVertex, inode, jElem, iNearestNode=0, iNearestVertex=0;
  long donor_elem=0, temp_donor=0;
  unsigned short iDim, donorindex;
  unsigned short nDim = target_geometry->GetnDim();
  unsigned short nMarkerInt, nMarkerDonor, nMarkerTarget;
  unsigned short iMarkerInt, iMarkerDonor, iMarkerTarget;
  unsigned short markDonor=0, markTarget=0, iFace;
  unsigned long pGlobalPoint = 0;
  unsigned int nNodes=0;
  /*--- Restricted to 2-zone for now ---*/
  unsigned int nFaces=1; //For 2D cases, we want to look at edges, not faces, as the 'interface'
  bool face_on_marker=true;
  su2double dist = 0.0, mindist=1E6, *Coord, *Coord_i, *Coord_j;
  su2double myCoeff[10]; // Maximum # of donor points
  su2double  *Normal;
  su2double projected_point[nDim];
  su2double tmp, tmp2;

  /*--- Number of markers on the interface ---*/
  nMarkerInt = (config[targetZone]->GetMarker_n_FSIinterface())/2;
  nMarkerDonor  =  config[donorZone]->GetnMarker_All();
  nMarkerTarget =  config[targetZone]->GetnMarker_All();
  /*--- For the number of markers on the interface... ---*/
  for (iMarkerInt=1; iMarkerInt <= nMarkerInt; iMarkerInt++){
    /*--- Procedure:
     * -Loop through vertices of the aero grid
     * -Find nearest element and allocate enough space in the aero grid donor point info
     *    -set the transfer coefficient values
     */

    /*--- ... the marker markDonor ... ---*/
    for (iMarkerDonor=0; iMarkerDonor < nMarkerDonor; iMarkerDonor++){
      if ( config[donorZone]->GetMarker_All_FSIinterface(iMarkerDonor) == (iMarkerInt)){
        markDonor=iMarkerDonor;
      }
    }

    /*--- ... the marker markTarget. ---*/
    for (iMarkerTarget=0; iMarkerTarget < nMarkerTarget; iMarkerTarget++){
      if (config[targetZone]->GetMarker_All_FSIinterface(iMarkerTarget) == (iMarkerInt)){
        markTarget=iMarkerTarget;
      }
    }

    /*--- Loop over the vertices on the target Marker ---*/
    for (iVertex = 0; iVertex<target_geometry->GetnVertex(markTarget); iVertex++) {
      Coord_i = target_geometry->vertex[markTarget][iVertex]->GetCoord();

      /*--- Find the Nearest Neighbor Point ---*/
      // Parallel: replace w/ loop over processors, recieve Coord_j
      mindist=1E6;
      for (jVertex = 0; jVertex<donor_geometry->GetnVertex(markDonor); jVertex++) {
        jPoint =donor_geometry->vertex[markDonor][jVertex]->GetNode(); // Local index of jVertex
        Coord_j = donor_geometry->node[jPoint]->GetCoord();
        dist = 0.0;
        for (iDim=0; iDim<nDim; iDim++)
          dist+=pow(Coord_i[iDim]-Coord_j[iDim],2.0);

        //cout <<" dist, gi " << dist << " " << donor_geometry->node[jPoint]->GetGlobalIndex()<<endl;
        if (dist<mindist){
          mindist=dist;
          /*--- Info stored at the node: zone, point, marker, vertex ---*/
          iNearestNode = jPoint;/* local index of the nearest neighbor */
          iNearestVertex = jVertex;
        }
      }
      //cout <<" dist, gi " << mindist << " " << donor_geometry->node[iNearestNode]->GetGlobalIndex()<<endl;
      /*--- Now that we know the closest vertex,
       * the closest element (or edge) must be one of the ones connected to that vertex:
       * Loop over --*/
      donor_elem=-1;
      mindist=1E6;

      for (jElem=0; jElem<donor_geometry->node[iNearestNode]->GetnElem(); jElem++){


        /*--- Loop over all the faces of this element to find ones on the interface boundary
         * If a face is on markDonor, then find the dist and check against previous to find closest
         * face. ---*/
        if (nDim==3){
          temp_donor = donor_geometry->node[iNearestNode]->GetElem(jElem);
          nFaces = donor_geometry->elem[temp_donor]->GetnFaces();
        }
        else{
          temp_donor = iNearestNode;
          nFaces = donor_geometry->node[iNearestNode]->GetnPoint();
        }
        /*--- Loop over faces (edges) attached to iNearestNode/vertex ---*/
        for (iFace=0; iFace<nFaces; iFace++){
          face_on_marker=true;

          /*--- If 3D loop over a face. if 2D loop over an element ---*/
          if (nDim==3){
            nNodes = donor_geometry->elem[temp_donor]->GetnNodesFace(iFace);
            /*-- Check if on marker of interface---*/
            for (unsigned int ifacenode=0; ifacenode<nNodes; ifacenode++){
              /*--- Local index of the node on face --*/
              inode = donor_geometry->elem[temp_donor]->GetFaces(iFace, ifacenode);
              jPoint = donor_geometry->elem[temp_donor]->GetNode(inode);
              face_on_marker = (face_on_marker and (donor_geometry->node[jPoint]->GetVertex(markDonor) !=-1));
            }
          }
          else{
            /*-- 2D: 'face' is an edge connected to the nearest node ---*/
            nNodes = 2; // edges have two nodes
            for (unsigned int ifacenode=0; ifacenode<nNodes; ifacenode++){
              inode = donor_geometry->node[iNearestNode]->GetEdge(iFace);
              jPoint = donor_geometry->edge[inode]->GetNode(ifacenode);
              face_on_marker = (face_on_marker and (donor_geometry->node[jPoint]->GetVertex(markDonor) !=-1));
            }
          }


          /*--- face_on_marker is true iff all nodes on face iFace are in marker markDonor (or 2D) ---*/
          //Coord = target_geometry->vertex[markTarget][iVertex]->GetCoord();
          /*--- if iFace is part of markDonor, calculate the isoparametric coefficients ---*/
          if (face_on_marker ){
            /*--- Find projected dist ---*/
            //Coord = target_geometry->vertex[markTarget][iVertex]->GetCoord();
            Normal = donor_geometry->vertex[markDonor][iNearestVertex]->GetNormal();
            Coord_j = donor_geometry->vertex[markDonor][iNearestVertex]->GetCoord();
            /*--- Project point xj onto surface --*/

            tmp = 0;
            tmp2=0;
            for (iDim=0; iDim<nDim; iDim++){
              tmp+=Normal[iDim]*Normal[iDim];
              tmp2+=Normal[iDim]*(Coord_i[iDim]-Coord_j[iDim]);
            }
            tmp = 1/tmp;
            tmp2 = tmp2*sqrt(tmp);
            for (iDim=0; iDim<nDim; iDim++){
              // projection of \vec{q} onto plane defined by \vec{n} and \vec{p}:
              // \vec{q} - \vec{n} ( (\vec{q}-\vec{p} ) \cdot \vec{n})
              // tmp2 = ( (\vec{q}-\vec{p} ) \cdot \vec{N})
              // \vec{n} = \vec{N}/(|N|), tmp = 1/|N|^2
              projected_point[iDim]=Coord_i[iDim] + Normal[iDim]*tmp2*tmp;
            }

            /*--- find isoparametric representation. if the point is outside the face (or edge, in 2D), the method will return
             * coefficients associated with setting the nearest neighbor ---*/
            Isoparameters( myCoeff, nDim, temp_donor, iFace, nNodes, projected_point);
            /*--- Recalculate the dist using the isoparametric representation ---*/
            dist = 0.0;
            Coord = target_geometry->vertex[markTarget][iVertex]->GetCoord();
            for(donorindex=0; donorindex< nNodes; donorindex++){
              if (nDim==3){
                jPoint = donor_geometry->elem[temp_donor]->GetNode(donor_geometry->elem[temp_donor]->GetFaces(iFace,donorindex));
              }
              else{
                inode = donor_geometry->node[iNearestNode]->GetEdge(iFace);
                jPoint = donor_geometry->edge[inode]->GetNode(donorindex);
              }
              Coord_j = donor_geometry->node[jPoint]->GetCoord();
              for (iDim=0; iDim<nDim; iDim++){
                Coord[iDim]-=myCoeff[donorindex]*Coord_j[iDim];
              }
            }
            for (iDim=0; iDim<nDim; iDim++){
              dist+=pow(Coord[iDim],2.0);
            }

            /*--- If the dist is shorter than last closest (and nonzero nodes are on the boundary), update ---*/
            if (dist<mindist ){
              /*--- update last dist ---*/
              mindist = dist;
              /*--- Store info ---*/
              donor_elem = temp_donor;

              // Debugging check
              /*
              cout <<"debug check" <<endl;
              Coord = target_geometry->vertex[markTarget][iVertex]->GetCoord();
              for (iDim=0; iDim<nDim; iDim++)
                Coord[iDim]=0;

              for (donorindex=0; donorindex< nNodes; donorindex++){
                //--- If 3D loop over a face. if 2D loop over an element ---
                if (nDim==3){
                  jPoint = donor_geometry->elem[temp_donor]->GetNode(donor_geometry->elem[temp_donor]->GetFaces(iFace,donorindex));
                }
                else{
                  inode = donor_geometry->node[iNearestNode]->GetEdge(iFace);
                  jPoint = donor_geometry->edge[inode]->GetNode(donorindex);
                }
                Coord_j = donor_geometry->node[jPoint]->GetCoord();
                cout <<" isoparam: " << myCoeff[donorindex] <<" Coord: ";
                for (iDim=0; iDim<nDim; iDim++){
                  Coord[iDim]+=myCoeff[donorindex]*Coord_j[iDim];
                  cout << Coord_j[iDim]<< " ";
                }
                cout << endl;
              }
              Coord_j = donor_geometry->node[iNearestNode]->GetCoord();
              cout << endl;
              for (iDim=0; iDim<nDim; iDim++){
                cout << " iso " << Coord[iDim] <<" proj " << projected_point[iDim] <<" NN " <<  Coord_j[iDim] << endl;
              }
             */
              target_geometry->vertex[markTarget][iVertex]->SetDonorElem(donor_elem); // in 2D is nearest neighbor
              target_geometry->vertex[markTarget][iVertex]->SetDonorFace(iFace); // in 2D is the edge
              target_geometry->vertex[markTarget][iVertex]->SetnDonorPoints(nNodes);
            }
          }
        }
      }

      /*--- If nDonorPoints ==0, no match was found, set nearest neighbor ---*/
      //unsigned long gpoint = target_geometry->vertex[markTarget][iVertex]->GetNode();
      //cout <<" Nearest Neighbor for target g.i " << target_geometry->node[gpoint]->GetGlobalIndex() <<" is "<< donor_geometry->node[iNearestNode]->GetGlobalIndex() << "; d = " << mindist<< endl;

      if (target_geometry->vertex[markTarget][iVertex]->GetnDonorPoints()==0){
        nNodes=1;
        target_geometry->vertex[markTarget][iVertex]->SetnDonorPoints(nNodes);
        donor_elem = -1;
        myCoeff[0] = 1;
      }


      /*--- Set the appropriate amount of memory ---*/
      target_geometry->vertex[markTarget][iVertex]->Allocate_DonorInfo();
      /*--- Recal the closest face/edge ---*/
      iFace = target_geometry->vertex[markTarget][iVertex]->GetDonorFace();

      /*--- Loop over vertices of the element to set the values at the vertex ---*/
      for (donorindex=0; donorindex< target_geometry->vertex[markTarget][iVertex]->GetnDonorPoints(); donorindex++){
        if (donor_elem!=-1){
          if (nDim==3){
            jPoint = donor_geometry->elem[temp_donor]->GetNode(donor_geometry->elem[temp_donor]->GetFaces(iFace,donorindex));
          }
          else{
            inode = donor_geometry->node[iNearestNode]->GetEdge(iFace);
            jPoint = donor_geometry->edge[inode]->GetNode(donorindex);
          }
        }
        else
          jPoint = iNearestNode; // If no matching element is found, revert to Nearest Neighbor

        /*--- jPoint must be converted to global index ---*/
        //ivtx = donor_geometry->node[jPoint]->GetVertex(markDonor);
        //donor_geometry->vertex[markDonor][ivtx]->IncrementnDonor();

        pGlobalPoint = donor_geometry->node[jPoint]->GetGlobalIndex();
        target_geometry->vertex[markTarget][iVertex]->SetInterpDonorPoint(donorindex,pGlobalPoint);
        target_geometry->vertex[markTarget][iVertex]->SetDonorCoeff(donorindex,myCoeff[donorindex]);
        target_geometry->vertex[markTarget][iVertex]->SetInterpDonorProcessor(donorindex, MASTER_NODE);

        /* From NN
        target_geometry->vertex[markTarget][iVertexTarget]->SetInterpDonorPoint(donorindex, pGlobalPoint);
        target_geometry->vertex[markTarget][iVertexTarget]->SetInterpDonorProcessor(donorindex, pProcessor);
        target_geometry->vertex[markTarget][iVertexTarget]->SetDonorCoeff(donorindex,1.0);
         */

        // FOR PARALELL:
        //target_geometry->vertex[markTarget][iVertex]->SetInterpDonorProc(donorindex,proc);
        //cout <<" myCoeff  " << myCoeff[donorindex] << " ";
      }
      //cout << endl;
    }

  }

}

void CIsoparametric::Isoparameters(su2double* isoparams,
    unsigned short nDim, long donor_elem,  unsigned short iFace,
    unsigned int nDonorPoints,  su2double* xj){
  short i,j,k, iedge;
  short n0 = nDim+1, n, iDim;
  short m =  nDonorPoints, m0;
  unsigned long jPoint, jPoint2;
  su2double tmp, tmp2;
  su2double x[m], x_tmp[m];
  su2double Q[m*m], R[m*m], A[n0*m];
  su2double x2[n0];

  /*--- n0: # of dimensions + 1. n: n0 less any degenerate rows ---*/
  n=n0;
  m0=m;

  bool test[n0], testi[n0];

  /*--- 2D: line, no need to go through computation --*/
  if (nDim==2){

    iedge = donor_geometry->node[donor_elem]->GetEdge(iFace);
    jPoint = donor_geometry->edge[iedge]->GetNode(0);
    jPoint2= donor_geometry->edge[iedge]->GetNode(1);
    tmp =  pow(donor_geometry->node[jPoint]->GetCoord(0)- donor_geometry->node[jPoint2]->GetCoord(0),2.0);
    tmp += pow(donor_geometry->node[jPoint]->GetCoord(1)- donor_geometry->node[jPoint2]->GetCoord(1),2.0);
    tmp = sqrt(tmp);

    tmp2 = pow(donor_geometry->node[jPoint]->GetCoord(0) - xj[0],2.0);
    tmp2 += pow(donor_geometry->node[jPoint]->GetCoord(1) - xj[1],2.0);
    tmp2 = sqrt(tmp2);
    isoparams[1] = tmp2/tmp;

    tmp2 = pow(donor_geometry->node[jPoint2]->GetCoord(0) - xj[0],2.0);
    tmp2 += pow(donor_geometry->node[jPoint2]->GetCoord(1) - xj[1],2.0);
    tmp2 = sqrt(tmp2);
    isoparams[0] = tmp2/tmp;

    //cout << "iso " << isoparams[0] << " " << isoparams[1] << endl;
  }
  else{

    /*--- Create Matrix A: 1st row all 1's, 2nd row x coordinates, 3rd row y coordinates, etc ---*/
    /*--- Right hand side is [1, \vec{x}']'---*/
    for (i=0; i<m*n; i++)
      A[i]=0;
    for (i=0; i<m; i++)
      A[i]=1.0;
    /*j,n: dimension. i,m: # neighbor point*/
    x[0]=1.0;
    for (iDim=0; iDim<nDim; iDim++)
      x[iDim+1]=xj[iDim];

    /*--- temp2 contains node #s that are on the surface (aka, we are excluding interior points) ---*/
    for (k=0; k<m0; k++){
      isoparams[k]=0;
      /*--- If 3D loop over a face. if 2D loop over an element ---*/
      jPoint = donor_geometry->elem[donor_elem]->GetNode(donor_geometry->elem[donor_elem]->GetFaces(iFace,k));
      // Neglect donor points that are not members of the matching marker.
      // jth coordinate of the ith donor vertex
      for (j=1; j<n0; j++){
        A[j*m+i] = donor_geometry->node[jPoint]->GetCoord(j-1);
      }
      i++;

    }
    /*--- Eliminate degenerate rows:
     * for example, if z constant including the z values will make the system degenerate
     * TODO: improve efficiency of this loop---*/
    test[0]=true; // always keep the 1st row
    for (i=1; i<n0; i++){
      // Test this row against all previous
      test[i]=true; // Assume that it is not degenerate
      for (k=0; k<i; k++){
        tmp=0; tmp2=0;
        for (j=0;j<m;j++){
          tmp+= A[i*m+j]*A[i*m+j];
          tmp2+=A[k*m+j]*A[k*m+j];
        }
        tmp  = pow(tmp,0.5);
        tmp2 = pow(tmp2,0.5);
        testi[k]=false;
        for (j=0; j<m; j++){
          // If at least one ratio is non-matching row i is not degenerate w/ row k
          if (A[i*m+j]/tmp != A[k*m+j]/tmp2)
            testi[k]=true;
        }
        // If any of testi (k<i) are false, row i is degenerate
        test[i]=(test[i] and testi[k]);
      }
      if (!test[i]) n--;
    }
    /*--- Initialize A2 now that we might have a smaller system --*/
    su2double A2[n*m];
    j=0;
    /*--- Copy only the rows that are non-degenerate ---*/
    for (i=0; i<n0; i++){
      if (test[i]){
        for (k=0;k<m;k++ ){
          A2[m*j+k]=A[m*i+k];
        }
        x2[j]=x[i];
        j++;
      }
    }
    /*--- Initialize Q,R to 0 --*/
    for (i=0; i<m*m; i++){
      Q[i]=0;
      R[i]=0;
    }
    /*--- TODO: make this loop more efficient ---*/
    /*--- Solve for rectangular Q1 R1 ---*/
    for (i=0; i<m; i++){
      tmp=0;
      for (j=0; j<n; j++)
        tmp += (A2[j*m+i])*(A2[j*m+i]);

      R[i*m+i]= pow(tmp,0.5);
      if (tmp>eps && i<n){
        for (j=0; j<n; j++)
          Q[j*m+i]=A2[j*m+i]/R[i*m+i];
      }
      else if (tmp!=0){
        for (j=0; j<n; j++)
          Q[j*m+i]=A2[j*m+i]/tmp;
      }
      for (j=i+1; j<m; j++){
        tmp=0;
        for (k=0; k<n; k++)
          tmp+=A2[k*m+j]*Q[k*m+i];

        R[i*m+j]=tmp;

        for (k=0; k<n; k++)
          A2[k*m+j]=A2[k*m+j]-Q[k*m+i]*R[i*m+j];
      }
    }
    /*--- x_tmp = Q^T * x2 ---*/
    for (i=0; i<m; i++)
      x_tmp[i]=0.0;
    for (i=0; i<m; i++){
      for (j=0; j<n; j++)
        x_tmp[i]+=Q[j*m+i]*x2[j];
    }
    /*--- solve x_tmp = R*isoparams for isoparams: upper triangular system ---*/
    for (i=n-1; i>=0; i--){
      if (R[i*m+i]>eps)
        isoparams[i]=x_tmp[i]/R[i*m+i];
      else
        isoparams[i]=0;
      for (j=0; j<i; j++)
        x_tmp[j]=x_tmp[j]-R[j*m+i]*isoparams[i];
    }

  }
  /*--- Isoparametric coefficients have been calculated. Run checks to eliminate  ---*/

  su2double tol = 1e-13; // hardcoded tolerance
  /*--- Check 0: normalize to 1: corrects for points not in face---*/
//  tmp=0;
//  for (i=0; i<m; i++){
//    if (isoparams[i]>1) isoparams[i]=1;
//    if (isoparams[i]<0) isoparams[i]=0;
//    tmp+=isoparams[i]*isoparams[i];
//  }
//  tmp = pow(tmp,0.5);
//  for (i=0; i<m; i++){
//    isoparams[i] /= tmp;
//  }
  /*--- Check 1: if close to 0, replace with 0 ---*/
  for (i=0; i<m; i++){
    if (abs(isoparams[i])< tol )
      isoparams[i]=0;
  }
  /*--- Check 2: if > 1, point is ouside face, not really represented accurately ---*/
  bool inside_face = true;
  for (i=0; i<m; i++){
    if (isoparams[i]> 1.1 or  isoparams[i]<-0.1 )
      inside_face = false;
  }
  if (!inside_face){
    //cout <<"Reverted to nearest neighbor " << endl;
    /*--- Revert to nearest neighbor ---*/
    tmp=1E6; tmp2=0.0; k=0;
    for (i=0; i<m0; i++){
      /*--- If 3D loop over a face. if 2D loop over an element ---*/
      if (nDim==3)
        jPoint = donor_geometry->elem[donor_elem]->GetNode(donor_geometry->elem[donor_elem]->GetFaces(iFace,i));
      else{
        iedge = donor_geometry->node[donor_elem]->GetEdge(iFace);
        jPoint = donor_geometry->edge[iedge]->GetNode(i);
      }

      for (j=0;j<nDim;j++)
        tmp2+=pow((donor_geometry->node[jPoint]->GetCoord(j)-xj[j]),2.0);
      if (tmp2<tmp){
        tmp=tmp2;
        k=i;
      }
      isoparams[i]=0;
    }
    isoparams[k]=1;
  }
  /*--- Check 4: print the result ---
  if (nDim==2)
    n0=2;
  for (k=0; k<m0; k++){
    cout <<" iDim " << k <<" Coord " << xj[k] <<" ";
    tmp =0;
    for (i=0; i<n0; i++){
      if (nDim==3)
        jPoint = donor_geometry->elem[donor_elem]->GetFaces(iFace,i);
      else{
        iedge = donor_geometry->node[donor_elem]->GetEdge(iFace);
        jPoint = donor_geometry->edge[iedge]->GetNode(i);
      }
      tmp+=isoparams[i]*(donor_geometry->node[jPoint]->GetCoord(k));
    }
    cout << tmp << endl;

  }
   */
}
