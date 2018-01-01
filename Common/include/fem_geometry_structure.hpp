/*!
 * \file fem_geometry_structure.hpp
 * \brief Headers of the main subroutines for creating the geometrical structure for the FEM solver.
 *        The subroutines and functions are in the <i>fem_geometry_structure.cpp</i> file.
 * \author E. van der Weide
 * \version 4.1.0 "Cardinal"
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

#include "./geometry_structure.hpp"

using namespace std;

/*!
 * \class long3T
 * \brief Help class used to store three longs as one entity.
 * \version 5.0.0 "Cardinal"
 */
class long3T {
public:
  long long0;  /*!< \brief First long to store in this class. */
  long long1;  /*!< \brief Second long to store in this class. */
  long long2;  /*!< \brief Third long to store in this class. */

  /* Constructors and destructors. */
  long3T();
  ~long3T();

  long3T(const long a, const long b, const long c);

  long3T(const long3T &other);

  /* Operators. */
  long3T& operator=(const long3T &other);

  bool operator<(const long3T &other) const;

private:
  /* Copy function. */
  void Copy(const long3T &other);
};

/*!
 * \class CReorderElementClass
 * \brief Class, used to reorder the owned elements after the partitioning.
 * \author E. van der Weide
 * \version 4.1.0 "Cardinal"
 */
class CReorderElementClass {
public:
  /*!
   * \brief Constructor of the class, set the member variables to the arguments.
   */
  CReorderElementClass(const unsigned long  val_GlobalElemID,
                       const unsigned short val_TimeLevel,
                       const bool           val_CommSolution,
                       const unsigned short val_VTK_Type,
                       const unsigned short val_nPolySol,
                       const bool           val_JacConstant);

  /*!
   * \brief Destructor of the class. Nothing to be done.
   */
  ~CReorderElementClass(void);

  /*!
   * \brief Copy constructor of the class.
   */
  CReorderElementClass(const CReorderElementClass &other);

  /*!
   * \brief Assignment operator of the class.
   */
  CReorderElementClass& operator=(const CReorderElementClass &other);

  /*!
   * \brief Less than operator of the class. Needed for the sorting.
   */
  bool operator<(const CReorderElementClass &other) const;

  /*!
   * \brief Function to make available the variable commSolution.
   * \return Whether or not the solution of the element must be communicated.
   */
  bool GetCommSolution(void);

  /*!
   * \brief Function to make available the element type of the element.
   * \return The value of elemType, which stores the VTK type, polynomial degree
             and whether or not the Jacobian is constant.
   */
  unsigned short GetElemType(void);

  /*!
   * \brief Function to make available the global element ID.
   * \return The global element ID of the element.
   */
  unsigned long GetGlobalElemID(void);

  /*!
   * \brief Function to make available the time level.
   * \return The time level of the element.
   */
  unsigned short GetTimeLevel(void);

  /*!
   * \brief Function, which sets the value of commSolution.
   * \param[in] val_CommSolution  - value to which commSolution must be set.
   */
  void SetCommSolution(const bool val_CommSolution);

private:
  unsigned long  globalElemID; /*!< \brief Global element ID of the element. */
  unsigned short timeLevel;    /*!< \brief Time level of the element. Only relevant
                                           for time accurate local time stepping. */
  bool           commSolution; /*!< \brief Whether or not the solution must be
                                           communicated to other ranks. */
  unsigned short elemType;     /*!< \brief Short hand for the element type, Which
                                           stored info of the VTK_Type, polynomial
                                           degree of the solution and whether or
                                           not the Jacobian is constant. */

  /*!
   * \brief Copy function. Needed for the copy constructor and assignment operator.
   */
  void Copy(const CReorderElementClass &other);

  /*!
   * \brief Default constructor of the class. Disabled.
   */
   CReorderElementClass(void);
};

/*!
 * \class SortFacesClass
 * \brief Functor, used for a different sorting of the faces than the < operator
 *        of FaceOfElementClass.
 * \author E. van der Weide
 * \version 4.1.0 "Cardinal"
 */
class CVolumeElementFEM;   // Forward declaration to avoid problems.
class SortFacesClass {
public:
  /*!
   * \brief Constructor of the class. Set the value of nVolElemTot.
   */
   SortFacesClass(unsigned long            val_nVolElemOwned,
                  unsigned long            val_nVolElemTot,
                  const CVolumeElementFEM *val_volElem);

 /*!
  * \brief Destructor of the class. Nothing to be done.
  */
  ~SortFacesClass(void);

 /*!
  * \brief Operator used for the comparison.
  * \param[in] f0 - First face in the comparison.
  * \param[in] f1 - Second face in the comparison.
  */
  bool operator()(const FaceOfElementClass &f0,
                  const FaceOfElementClass &f1);
private:
  unsigned long nVolElemOwned; /*!< \brief Number of locally owned volume elements. */
  unsigned long nVolElemTot;   /*!< \brief Total number of local volume elements . */

  const CVolumeElementFEM *volElem; /*!< \brief The locally stored volume elements. */

  /*!
   * \brief Default constructor of the class. Disabled.
   */
   SortFacesClass(void);
};

/*!
 * \class CVolumeElementFEM
 * \brief Class to store a volume element for the FEM solver.
 * \author E. van der Weide
 * \version 4.1.0 "Cardinal"
 */
class CVolumeElementFEM {
public:
  bool elemIsOwned;             /*!< \brief Whether or not this is an owned element. */
  bool JacIsConsideredConstant; /*!< \brief Whether or not the Jacobian of the transformation
                                     to the standard element is considered constant. */

  int rankOriginal;            /*!< \brief The rank where the original volume is stored. For
                                    the owned volumes, this is simply the current rank. */

  short periodIndexToDonor;    /*!< \brief The index of the periodic transformation to the donor
                                    element. Only for halo elements. A -1 indicates no periodic
                                    transformation. */

  unsigned short VTK_Type;     /*!< \brief Element type using the VTK convention. */
  unsigned short nPolyGrid;    /*!< \brief Polynomial degree for the geometry of the element. */
  unsigned short nPolySol;     /*!< \brief Polynomial degree for the solution of the element. */
  unsigned short nDOFsGrid;    /*!< \brief Number of DOFs for the geometry of the element. */
  unsigned short nDOFsSol;     /*!< \brief Number of DOFs for the solution of the element. */
  unsigned short nFaces;       /*!< \brief Number of faces of the element. */
  unsigned short timeLevel;    /*!< \brief Time level of the element when time accurate local
                                           time stepping is employed. */

  unsigned short indStandardElement; /*!< \brief Index in the vector of standard elements. */

  unsigned int factTimeLevel;        /*!< \brief Number of local time steps for this element
                                                 compared to the largest time step when time
                                                 accurate local time stepping is employed. */

  unsigned long elemIDGlobal;        /*!< \brief Global element ID of this element. */
  unsigned long offsetDOFsSolGlobal; /*!< \brief Global offset of the solution DOFs of this element. */
  unsigned long offsetDOFsSolLocal;  /*!< \brief Local offset of the solution DOFs of this element. */

  unsigned long offsetDOFsSolThisTimeLevel; /*!< \brief Local offset of the solution DOFs of this element
                                                        in the working vector of the time level of the element.
                                                        Needed for time accurate local time stepping. */
  unsigned long offsetDOFsSolPrevTimeLevel; /*!< \brief Local offset of the solution DOFs of this element
                                                        in the working vector of the previous time level. */

  vector<bool> JacFacesIsConsideredConstant; /*!< \brief Vector with the booleans whether the Jacobian of the
                                                  transformation to the standard element is constant for the faces. */
  vector<bool> ElementOwnsFaces;             /*!< \brief Vector with the booleans whether the element is the
                                                         owner of the faces. */

  vector<unsigned long> nodeIDsGrid; /*!< \brief Vector with the node IDs of the grid for this element. */

  su2double lenScale;                /*!< \brief Length scale of the element. */

  su2double shockSensorValue;               /*!< \brief Value for sensing a shock */
  su2double shockArtificialViscosity;       /*!< \brief Artificial viscosity for a shock */


  vector<su2double> metricTerms;            /*!< \brief Vector of the metric terms in the
                                                        integration points of this element. */
  vector<su2double> metricTermsSolDOFs;     /*!< \brief Vector of the metric terms in the
                                                        solution DOFs of this element. */
  vector<su2double> metricTerms2ndDer;      /*!< \brief Vector of the metric terms needed for the
                                                        computation of the 2nd derivatives in the
                                                        integration points. Only determined when
                                                        needed (ADER-DG with non-aliased predictor
                                                        for the Navier-Stokes equations). */
  vector<su2double> gridVelocities;         /*!< \brief Vector of the grid velocities in the
                                                        integration points of this element. */
  vector<su2double> gridVelocitiesSolDOFs;  /*!< \brief Vector of the grid velocities in the
                                                        solution DOFs of this element. */
  vector<su2double> massMatrix;             /*!< \brief Mass matrix for this element. */
  vector<su2double> invMassMatrix;          /*!< \brief Inverse mass matrix for this element. */
  vector<su2double> lumpedMassMatrix;       /*!< \brief Lumped mass matrix for this element. */

  vector<su2double> ADERIterationMatrix;    /*!< \brief Iteration matrix used in the predictor step of
                                                        the ADER-DG scheme. */

  vector<su2double> coorIntegrationPoints;  /*!< \brief The coordinates of the integration points of this element. */
  vector<su2double> coorSolDOFs;            /*!< \brief The coordinates of the solution DOFs of this element. */
  vector<su2double> wallDistance;           /*!< \brief The wall distance to the viscous walls for
                                                        the integration points of this element. */
  vector<su2double> wallDistanceSolDOFs;    /*!< \brief The wall distance to the viscous walls for
                                                        the solution DOFs of this element. */

  /*!
   * \brief Constructor of the class. Initialize the pointers to NULL.
   */
  CVolumeElementFEM(void);

  /*!
   * \brief Destructor of the class. Nothing to be done.
   */
  ~CVolumeElementFEM(void);

  /*!
   * \brief Get all the corner points of all the faces of this element. It must be made sure
            that the numbering of the faces is identical to the numbering used for the
            standard elements.
   * \param[out] nFaces         - Number of faces of this element.
   * \param[out] nPointsPerFace - Number of corner points for each of the faces.
   * \param[out] faceConn       - Global IDs of the corner points of the faces.
   */
  void GetCornerPointsAllFaces(unsigned short &numFaces,
                               unsigned short nPointsPerFace[],
                               unsigned long  faceConn[6][4]);
};

/*!
 * \class CPointFEM
 * \brief Class to a point for the FEM solver.
 * \author E. van der Weide
 * \version 4.1.0 "Cardinal"
 */
class CPointFEM {
public:
  unsigned long globalID;    /*!< \brief The global ID of this point in the grid. */
  short periodIndexToDonor;  /*!< \brief The index of the periodic transformation to the donor
                                  element. Only for halo elements. A -1 indicates no periodic
                                  transformation. */
  su2double coor[3];         /*!< \brief Array with the coordinates of the node. */

  /*!
   * \brief Default constructor of the class. Initialize the coordinates to zero
            to avoid a valgrind warning in two space dimensions.
   */
  CPointFEM(void);

  /*!
   * \brief Destructor of the class. Nothing to be done.
   */
  ~CPointFEM(void);

  /*!
   * \brief Copy constructor of the class.
   */
  CPointFEM(const CPointFEM &other);

  /*!
   * \brief Assignment operator of the class.
   */
  CPointFEM& operator=(const CPointFEM &other);

  /*!
   * \brief Less than operator of the class. Needed for the sorting.
   */
  bool operator<(const CPointFEM &other) const;

  /*!
   * \brief Equal operator of the class. Needed for the removal of double entities.
   */
  bool operator==(const CPointFEM &other) const;

private:
  /*!
   * \brief Copy function. Needed for the copy constructor and assignment operator.
   */
  void Copy(const CPointFEM &other);
};

/*!
 * \class CInternalFaceElementFEM
 * \brief Class to store an internal face for the FEM solver.
 * \author E. van der Weide
 * \version 4.1.0 "Cardinal"
 */
class CInternalFaceElementFEM {
public:
  unsigned short VTK_Type;     /*!< \brief Element type using the VTK convention. */

  unsigned short indStandardElement; /*!< \brief Index in the vector of standard face elements. */

  unsigned long elemID0;              /*!< \brief Element ID adjacent to side 0 of the face. */
  unsigned long elemID1;              /*!< \brief Element ID adjacent to side 1 of the face. */

  vector<unsigned long> DOFsGridFaceSide0;   /*!< \brief Vector of the grid DOFs of side 0 of the face. */
  vector<unsigned long> DOFsGridFaceSide1;   /*!< \brief Vector of the grid DOFs of side 1 of the face. */
  vector<unsigned long> DOFsSolFaceSide0;    /*!< \brief Vector of the solution DOFs of side 0 of the face. */
  vector<unsigned long> DOFsSolFaceSide1;    /*!< \brief Vector of the solution DOFs of side 1 of the face. */

  vector<unsigned long> DOFsGridElementSide0;   /*!< \brief Vector of the grid DOFs of the element of side 0. */
  vector<unsigned long> DOFsGridElementSide1;   /*!< \brief Vector of the grid DOFs of the element of side 1. */
  vector<unsigned long> DOFsSolElementSide0;    /*!< \brief Vector of the solution DOFs of the element of side 0. */
  vector<unsigned long> DOFsSolElementSide1;    /*!< \brief Vector of the solution DOFs of the element of side 1. */

  vector<su2double> metricNormalsFace;     /*!< \brief The normals in the integration points of the face.
                                                       The normals point from side 0 to side 1. */
  vector<su2double> metricCoorDerivFace0;  /*!< \brief The terms drdx, dsdx, etc. of side 0 in the
                                                       integration points of the face. */
  vector<su2double> metricCoorDerivFace1;  /*!< \brief The terms dxdr, dydr, etc. of side 1 in the
                                                       integration points of the face. */

  vector<su2double> coorIntegrationPoints;  /*!< \brief Coordinates for the integration points of this face. */
  vector<su2double> gridVelocities;         /*!< \brief Grid velocities in the integration points of this face. */
  vector<su2double> wallDistance;           /*!< \brief The wall distance to the viscous walls for
                                                        the integration points of this face. */

  /*!
   * \brief Constructor of the class. Initialize some pointers to NULL.
   */
  CInternalFaceElementFEM(void);

  /*!
   * \brief Destructor of the class. Nothing to be done.
   */
  ~CInternalFaceElementFEM(void);
};

/*!
 * \class CSurfaceElementFEM
 * \brief Class to store a surface element for the FEM solver.
 * \author E. van der Weide
 * \version 4.1.0 "Cardinal"
 */
class CSurfaceElementFEM {
public:
  unsigned short VTK_Type;     /*!< \brief Element type using the VTK convention. */
  unsigned short nPolyGrid;    /*!< \brief Polynomial degree for the geometry of the element. */
  unsigned short nDOFsGrid;    /*!< \brief Number of DOFs for the geometry of the element. */

  unsigned short indStandardElement; /*!< \brief Index in the vector of standard elements. */

  unsigned long volElemID;         /*!< \brief ID of the corresponding volume element. */
  unsigned long boundElemIDGlobal; /*!< \brief Global ID of this surface element inside
                                        the boundary to which it belongs. */

  vector<unsigned long> nodeIDsGrid; /*!< \brief Vector with the node IDs of the grid for this element.
                                                 In this vector the original sequence of the grid file
                                                 is stored. */

  vector<unsigned long> DOFsGridFace;   /*!< \brief Vector of the grid DOFs of the face. In principle
                                                    the same information as nodeIDsGrid, but the sequence
                                                    could be different. */
  vector<unsigned long> DOFsSolFace;    /*!< \brief Vector of the solution DOFs of the face. */

  vector<unsigned long> DOFsGridElement;   /*!< \brief Vector of the grid DOFs of the adjacent element. */
  vector<unsigned long> DOFsSolElement;    /*!< \brief Vector of the solution DOFs of the adjacent element. */

  vector<su2double> metricNormalsFace;     /*!< \brief The normals in the integration points of the face.
                                                       The normals point out of the adjacent element. */
  vector<su2double> metricCoorDerivFace;   /*!< \brief The terms drdx, dsdx, etc. in the integration
                                                       points of the face. */
  vector<su2double> coorIntegrationPoints; /*!< \brief The coordinates of the integration points of the face. */
  vector<su2double> gridVelocities;         /*!< \brief Grid velocities in the integration points of this face. */
  vector<su2double> wallDistance;          /*!< \brief The wall distances of the integration points
                                                       of the face. */

  /*!
   * \brief Constructor of the class. Initialize some variables.
   */
  CSurfaceElementFEM(void);

  /*!
   * \brief Destructor of the class. Nothing to be done.
   */
  ~CSurfaceElementFEM(void);

  /*!
   * \brief Copy constructor of the class.
   */
  CSurfaceElementFEM(const CSurfaceElementFEM &other);

  /*!
   * \brief Assignment operator of the class.
   */
  CSurfaceElementFEM& operator=(const CSurfaceElementFEM &other);

  /*!
   * \brief Less than operator of the class. Needed for the sorting.
            The criterion for comparison is the corresponding (local) volume ID.
   */
  bool operator<(const CSurfaceElementFEM &other) const;

  /*!
   *  \brief Function, which determines the corner points of this surface element.
   *  \param[out] nPointsPerFace - Number of corner points of the face.
   *  \param[out] faceConn       - The corner points of the face.
   */
  void GetCornerPointsFace(unsigned short &nPointsPerFace,
                           unsigned long  faceConn[]);

private:
  /*!
   * \brief Copy function. Needed for the copy constructor and assignment operator.
   */
  void Copy(const CSurfaceElementFEM &other);
};

/*!
 * \class CBoundaryFEM
 * \brief Class to store a boundary for the FEM solver.
 * \author E. van der Weide
 * \version 4.1.0 "Cardinal"
 */
class CBoundaryFEM {
public:
  string markerTag;  /*!< \brief Marker tag of this boundary. */

  bool periodicBoundary; /*!< \brief Whether or not this boundary is a periodic boundary. */

  vector<unsigned long> nSurfElem; /*!< \brief Number of surface elements per time level,
                                               cumulative storage format. */

  vector<CSurfaceElementFEM> surfElem; /*!< \brief Vector of the local surface elements. */

  /*!
   * \brief Constructor of the class. Nothing to be done.
   */
  CBoundaryFEM(void);

  /*!
   * \brief Destructor of the class. Nothing to be done.
   */
  ~CBoundaryFEM(void);
};

/*!
 * \class CMeshFEM
 * \brief Base class for the FEM solver.
 * \author E. van der Weide
 * \version 4.1.0 "Cardinal"
 */
class CMeshFEM: public CGeometry {
protected:
  unsigned long nVolElemTot;    /*!< \brief Total number of local volume elements, including halos. */
  unsigned long nVolElemOwned;  /*!< \brief Number of owned local volume elements. */

  vector<unsigned long> nVolElemOwnedPerTimeLevel;    /*!< \brief Number of owned local volume elements
                                                                  per time level. Cumulative storage. */
  vector<unsigned long> nVolElemInternalPerTimeLevel; /*!< \brief Number of internal local volume elements per
                                                                  time level. Internal means that the solution
                                                                  data does not need to be communicated. */
  vector<unsigned long> nVolElemHaloPerTimeLevel;    /*!< \brief Number of local halo volume elements
                                                                 per time level. Cumulative storage. */

  vector<vector<unsigned long> > ownedElemAdjLowTimeLevel; /*!< \brief List of owned elements per time level that are
                                                                       adjacent to elements of the lower time level. */
  vector<vector<unsigned long> > haloElemAdjLowTimeLevel; /*!< \brief List of halo elements per time level that are
                                                                      adjacent to elements of the lower time level. */

  vector<CVolumeElementFEM> volElem; /*!< \brief Vector of the local volume elements, including halos. */

  vector<CPointFEM> meshPoints;      /*!< \brief Vector of the points of the FEM mesh. */

  vector<CBoundaryFEM> boundaries;   /*!< \brief Vector of the boundaries of the FEM mesh. */

  vector<unsigned short> rotPerMarkers; /*!< \brief Vector, which contains the indices of the rotational
                                                    periodic markers. */
  vector<vector<unsigned long> > rotPerHalos; /*!< \brief Vector of vector, which contains the indices of
                                                          the halo elements for which a rotationally periodic
                                                          correction must be applied. */

  vector<int> ranksRecv;             /*!< \brief Vector of ranks, from which this rank will receive halo
                                                 information. Self communication is included. */
  vector<int> ranksSend;             /*!< \brief Vector of ranks, to which this rank will send halo
                                                 information. Self communication is included. */

  vector<vector<unsigned long> > entitiesSend;    /*!< \brief Vector of vector, which contains the entities that
                                                              must be sent. Self communication is included. For DG
                                                              an entitity is an element, for regular FEM an entity
                                                              is a DOF. */
  vector<vector<unsigned long> > entitiesRecv; /*!< \brief Vector of vector, which contains the entities that
                                                           must be received. Self communication is included. For DG
                                                           an entity is an element, for regular FEM an entity
                                                           is a DOF. */

  vector<FEMStandardBoundaryFaceClass> standardBoundaryFacesSol;  /*!< \brief Vector that contains the standard boundary
                                                                              faces used for the solution of the DG solver. */
  vector<FEMStandardBoundaryFaceClass> standardBoundaryFacesGrid; /*!< \brief Vector that contains the standard boundary
                                                                              faces used for the geometry of the DG solver. */
public:
  /*!
  * \brief Constructor of the class.
  */
 CMeshFEM(void);

  /*!
  * \overload
  * \brief Redistributes the grid over the ranks and creates the halo layer.
  * \param[in] geometry - The linear distributed grid that must be redistributed.
  * \param[in] config   - Definition of the particular problem.
  */
  CMeshFEM(CGeometry *geometry, CConfig *config);

  /*!
  * \brief Destructor of the class.
  */
  virtual ~CMeshFEM(void);

  /*!
  * \brief Function, which makes available the boundaries of the local FEM mesh.
  * \return  Pointer to the boundaries of the local FEM mesh.
  */
  CBoundaryFEM *GetBoundaries(void);

  /*!
  * \brief Function, which makes available the mesh points of the local FEM mesh.
  * \return  Pointer to the mesh points of the local FEM mesh.
  */
  CPointFEM *GetMeshPoints(void);

  /*!
  * \brief Function, which makes available the number of mesh points of the local FEM mesh.
  * \return  Number of mesh points of the local FEM mesh.
  */
  unsigned long GetNMeshPoints(void);

  /*!
  * \brief Function, which makes available the number of owned volume elements in the local FEM mesh.
  * \return  Number of owned volume elements of the local FEM mesh.
  */
  unsigned long GetNVolElemOwned(void);

  /*!
  * \brief Function, which makes available the total number of volume elements in the local FEM mesh.
  * \return  Total number of volume elements of the local FEM mesh.
  */
  unsigned long GetNVolElemTot(void);

  /*!
  * \brief Function, which makes available the volume elements in the local FEM mesh.
  * \return  Pointer to the volume elements of the local FEM mesh.
  */
  CVolumeElementFEM *GetVolElem(void);

  /*!
  * \brief Function, which makes available the number of owned volume elements per time level.
  * \return  The pointer to the data of nVolElemOwnedPerTimeLevel.
  */
  unsigned long *GetNVolElemOwnedPerTimeLevel(void);

  /*!
  * \brief Function, which makes available the number of internal volume elements per time level.
  * \return  The pointer to the data of nVolElemInternalPerTimeLevel.
  */
  unsigned long *GetNVolElemInternalPerTimeLevel(void);

  /*!
  * \brief Function, which makes available the number of halo volume elements per time level.
  * \return  The pointer to the data of nVolElemHaloPerTimeLevel.
  */
  unsigned long *GetNVolElemHaloPerTimeLevel(void);

  /*!
  * \brief Function, which makes available the vector of vectors containing the owned element
           IDs adjacent to elements of a lower time level. Note that a copy is made.
  * \return  Copy of ownedElemAdjLowTimeLevel.
  */
  vector<vector<unsigned long> > GetOwnedElemAdjLowTimeLevel(void);

  /*!
  * \brief Function, which makes available the vector of vectors containing the halo element
           IDs adjacent to elements of a lower time level. Note that a copy is made.
  * \return  Copy of haloElemAdjLowTimeLevel.
  */
  vector<vector<unsigned long> > GetHaloElemAdjLowTimeLevel(void);

  /*!
  * \brief Function, which makes available the number of standard boundary faces of the solution.
  * \return  Number of standard boundary faces of the solution.
  */
  unsigned short GetNStandardBoundaryFacesSol(void);

  /*!
  * \brief Function, which makes available the standard boundary faces of the solution.
  * \return  Pointer to the standard boundary faces of the solution.
  */
  FEMStandardBoundaryFaceClass *GetStandardBoundaryFacesSol(void);

  /*!
  * \brief Function, which makes available the vector of receive ranks as
           a const reference.
  * \return  Const reference to the vector of ranks.
  */
  const vector<int> &GetRanksRecv(void) const;

  /*!
  * \brief Function, which makes available the vector of send ranks as
           a const reference.
  * \return  Const reference to the vector of ranks.
  */
  const vector<int> &GetRanksSend(void) const;

  /*!
  * \brief Function, which makes available the vector of vectors containing the receive
           entities as a const reference.
  * \return  Const reference to the vector of vectors of receive entities.
  */
  const vector<vector<unsigned long> > &GetEntitiesRecv(void) const;

  /*!
  * \brief Function, which makes available the vector of vectors containing the send
           entities as a const reference.
  * \return  Const reference to the vector of vectors of send entities.
  */
  const vector<vector<unsigned long> > &GetEntitiesSend(void) const;

  /*!
  * \brief Function, which makes available the vector of rotational periodic markers
           as a const reference.
  * \return  Const reference to the vector with rotational periodic markers.
  */
  const vector<unsigned short> &GetRotPerMarkers(void) const;

  /*!
  * \brief Function, which makes available the vector of vectors containing the rotational
           periodic halos as a const reference.
  * \return  Const reference to the vector of vectors with rotational periodic halos.
  */
  const vector<vector<unsigned long> > &GetRotPerHalos(void) const;

  /*!
  * \brief Compute surface area (positive z-direction) for force coefficient non-dimensionalization.
  * \param[in] config - Definition of the particular problem.
  */
  void SetPositive_ZArea(CConfig *config);

protected:
  /*!
  * \brief Function, which computes the gradients of the parametric coordinates
           w.r.t. the Cartesian coordinates in the integration points of a face,
           i.e. drdx, drdy, dsdx, etc.
  * \param[in]  nIntegration   - Number of integration points on the face.
  * \param[in]  nDOFs          - Number of DOFs of the grid associated with the
                                 neighboring element.
  * \param[in]  matDerBasisInt - Matrix, which contains the derivatives of the
                                 basis functions w.r.t. the parametric
                                 coordinates r, s and t in the integration points.
  * \param[in]  DOFs           - The DOFs of the grid associated with the element.
  * \param[out] derivCoor      - Storage for the derivatives of the coordinates.
  */
  void ComputeGradientsCoordinatesFace(const unsigned short nIntegration,
                                       const unsigned short nDOFs,
                                       const su2double      *matDerBasisInt,
                                       const unsigned long  *DOFs,
                                       su2double            *derivCoor);
  /*!
  * \brief Function, which computes the gradients of the Cartesian coordinates
           w.r.t. the parametric coordinates in the given set of integration
           points, i.e. dxdr, dydr, etc.
  * \param[in]  nIntegration   - Number of integration points.
  * \param[in]  nDOFs          - Number of DOFs of the grid associated with the
                                 element.
  * \param[in]  matDerBasisInt - Matrix, which contains the derivatives of the
                                 basis functions w.r.t. the parametric
                                 coordinates r, s and t in the integration points.
  * \param[in]  DOFs           - The DOFs of the grid associated with the element.
  * \param[out] derivCoor    - Storage for the derivatives of the coordinates.
  */
  void ComputeGradientsCoorWRTParam(const unsigned short nIntegration,
                                    const unsigned short nDOFs,
                                    const su2double      *matDerBasisInt,
                                    const unsigned long  *DOFs,
                                    su2double            *derivCoor);
  /*!
  * \brief Function, which computes the information of the normals in the
           integration points of a face.
  * \param[in]  nIntegration - Number of integration points on the face.
  * \param[in]  nDOFs        - Number of DOFs of the grid associated with the face.
  * \param[in]  dr           - r-derivatives of the basis functions of the face.
  * \param[in]  ds           - s-derivatives of the basis functions of the face.
                               Only for 3D computations.
  * \param[in]  DOFs         - The DOFs of the grid associated with the face.
  * \param[out] normals      - Storage for the normal information to be computed.
  */
  void ComputeNormalsFace(const unsigned short nIntegration,
                          const unsigned short nDOFs,
                          const su2double      *dr,
                          const su2double      *ds,
                          const unsigned long  *DOFs,
                          su2double            *normals);

  /*!
  * \brief Function, which computes the metric terms of the faces of a
           physical boundary.
  * \param[inout] boundary - Boundary for whose faces the boundary metric
                             terms must be computed.
  * \param[in]    config   - Definition of the particular problem.
  */
  void MetricTermsBoundaryFaces(CBoundaryFEM *boundary,
                                CConfig      *config);
};

/*!
 * \class CMeshFEM_DG
 * \brief Class which contains all the variables for the DG FEM solver.
 * \author E. van der Weide
 * \version 4.1.0 "Cardinal"
 */
class CMeshFEM_DG: public CMeshFEM {
private:
  vector<FEMStandardElementClass> standardElementsSol;  /*!< \brief Vector that contains the standard volume elements
                                                                    used for the solution of the DG solver. */
  vector<FEMStandardElementClass> standardElementsGrid; /*!< \brief Vector that contains the standard volume elements
                                                                    used for the geometry of the DG solver. */

  vector<FEMStandardInternalFaceClass> standardMatchingFacesSol;  /*!< \brief Vector that contains the standard matching
                                                                              internal faces used for the solution of
                                                                              the DG solver. */
  vector<FEMStandardInternalFaceClass> standardMatchingFacesGrid; /*!< \brief Vector that contains the standard matching
                                                                              internal faces used for the geometry of
                                                                              the DG solver. */

  vector<su2double> LagrangianBeginTimeIntervalADER_DG; /*!< \brief Values of the Lagrangian interpolation functions of
                                                                    the time DOFs at the beginning of the time interval,
                                                                    i.e. r == -1. */

  vector<su2double> timeInterpolDOFToIntegrationADER_DG;    /*!< \brief The interpolation matrix between the time DOFs and
                                                                        the time integration points for ADER-DG. */
  vector<su2double> timeInterpolAdjDOFToIntegrationADER_DG; /*!< \brief The interpolation matrix between the time DOFs of adjacent
                                                                        elements of a higher time level and the time integration
                                                                        points for ADER-DG. */

  vector<unsigned long> nMatchingFacesInternal;          /*!< \brief Number of matching faces between between two owned elements
                                                                     per time level. Cumulative storage format. */
  vector<unsigned long> nMatchingFacesWithHaloElem;      /*!< \brief Number of matching faces between an owned element and a halo
                                                                     element per time level. Cumulative storage format. */
  vector<CInternalFaceElementFEM> matchingFaces; /*!< \brief Vector of the local matching internal faces. */

  map<unsigned long, unsigned long> Global_to_Local_Point; /*!< \brief Global-local mapping for the DOFs. */

public:

 /*!
  * \brief Constructor of the class.
  */
 CMeshFEM_DG(void);

  /*!
  * \overload
  * \brief Redistributes the grid over the ranks and creates the halo layer.
  * \param[in] geometry - The linear distributed grid that must be redistributed.
  * \param[in] config   - Definition of the particular problem.
  */
  CMeshFEM_DG(CGeometry *geometry, CConfig *config);

 /*!
  * \brief Destructor of the class.
  */
  ~CMeshFEM_DG(void);

 /*!
  * \brief Function to compute the coordinates of the integration points.
  */
  void CoordinatesIntegrationPoints(void);

 /*!
  * \brief Function to compute the coordinates of solution DOFs.
  */
  void CoordinatesSolDOFs(void);

 /*!
  * \brief Function to compute the distance to the nearest viscous wall.
  * \param[in] config - Definition of the particular problem.
  */
  void ComputeWall_Distance(CConfig *config);

 /*!
  * \brief Function to create the faces used in the DG formulation.
  * \param[in] config - Definition of the particular problem.
  */
  void CreateFaces(CConfig *config);

 /*!
  * \brief Function to create the standard volume elements.
  * \param[in] config - Definition of the particular problem.
  */
  void CreateStandardVolumeElements(CConfig *config);

 /*!
  * \brief Function, which makes available the Lagrangian interpolation functions
           at the begin of the time interval for the time DOFs of ADER-DG.
  * \return  The Lagrangian interpolation functions at the begin of the interval.
  */
  su2double *GetLagrangianBeginTimeIntervalADER_DG(void);

 /*!
  * \brief Function, which makes available the time interpolation matrix between
           the time DOFs and time integration points for ADER-DG.
  * \return  The time interpolation matrix for ADER-DG.
  */
  su2double *GetTimeInterpolDOFToIntegrationADER_DG(void);

 /*!
  * \brief Function, which makes available the time interpolation matrix between
           the adjacent time DOFs of the next time level and the time
           integration points for ADER-DG.
  * \return  The time interpolation matrix of adjacent time DOFs for ADER-DG.
  */
  su2double *GetTimeInterpolAdjDOFToIntegrationADER_DG(void);

 /*!
  * \brief Function, which makes available the number of matching internal faces
           between an owned element and a halo element per time level.
  * \return  The number of matching internal faces between these elements per time level.
  */
  unsigned long *GetNMatchingFacesWithHaloElem(void);

 /*!
  * \brief Function, which makes available the number of matching internal faces
           between two owned elements per time level.
  * \return  The number of matching internal faces per time level.
  */
  unsigned long *GetNMatchingFacesInternal(void);

 /*!
  * \brief Function, which makes available the matching internal faces.
  * \return  Pointer to the matching internal faces.
  */
  CInternalFaceElementFEM *GetMatchingFaces(void);

 /*!
  * \brief Function, which makes available the number of standard volume elements of the solution.
  * \return  Number of standard volume elements of the solution.
  */
  unsigned short GetNStandardElementsSol(void);

 /*!
  * \brief Function, which makes available the standard volume elements of the solution.
  * \return  Pointer to the standard volume elements of the solution.
  */
  FEMStandardElementClass *GetStandardElementsSol(void);

 /*!
  * \brief Function, which makes available the number of standard internal matching faces of the solution.
  * \return  Number of standard internal matching faces of the solution.
  */
  unsigned short GetNStandardMatchingFacesSol(void);

 /*!
  * \brief Function, which makes available the standard internal matching faces of the solution.
  * \return  Pointer to the standard internal matching faces of the solution.
  */
  FEMStandardInternalFaceClass *GetStandardMatchingFacesSol(void);

 /*!
  * \brief Function, which computes a length scale of the volume elements.
  * \param[in] config - Definition of the particular problem.
  */
  void LengthScaleVolumeElements(void);

 /*!
  * \brief Function, which computes the metric terms of the surface
           elements, both internal faces and physical boundary faces.
  * \param[in] config - Definition of the particular problem.
  */
  void MetricTermsSurfaceElements(CConfig *config);

 /*!
  * \brief Function, which computes the metric terms of the
           volume elements.
  * \param[in] config - Definition of the particular problem.
  */
  void MetricTermsVolumeElements(CConfig *config);

 /*!
  * \brief Set the send receive boundaries of the grid.
  * \param[in] config - Definition of the particular problem.
  */
  void SetSendReceive(CConfig *config);

  /*!
   * \brief Set the value of the total number of points globally in the simulation.
   * \param[in] val_global_npoint - Global number of points in the mesh (excluding halos).
   */
  void SetGlobal_nPointDomain(unsigned long val_global_npoint);

  /*!
   * \brief Retrieve total number of nodes in a simulation across all processors (excluding halos).
   * \returns Total number of nodes in a simulation across all processors (excluding halos).
   */
  unsigned long GetGlobal_nPointDomain();

  /*!
   * \brief Set the local index that correspond with the global numbering index.
   */
  void SetGlobal_to_Local_Point();

  /*!
   * \brief Get the local index that correspond with the global numbering index.
   * \param[in] val_ipoint - Global point.
   * \returns Local index that correspond with the global index, -1 if not found on the current rank.
   */
  long GetGlobal_to_Local_Point(unsigned long val_ipoint);

private:
 /*!
  * \brief Function, which computes the correct sequence of the connectivities
           of a face, such that it matches the sequence of the given corner points.
  * \param[in]  VTK_TypeFace       - Type of the face using the VTK convention.
  * \param[in]  cornerPointsFace   - The corner points of the face in the desired
                                     sequence.
  * \param[in]  VTK_TypeElem       - Type of the element using the VTK convention.
  * \param[in]  nPolyGrid          - Polynomial degree used in the grid definition
                                     for the face and the element.
  * \param[in]  elemNodeIDsGrid    - The node IDs of the grid DOFs of the element,
                                     i.e. the element connectivity.
  * \param[in]  nPolyConn          - Polynomial degree of the connectivities to
                                     be modified.
  * \param[in]  connElem           - Connectivity of the adjacent volume element.
  * \param[out] swapFaceInElement  - Whether or not the connectivity of the face must
                                     be swapped compared to the face of the corresponding
                                     standard element. Only relevant for triangular faces
                                     of a pyramid and quadrilateral faces of a prism.
                                     corresponds to the top point of the adjacent pyramid.
  * \param[out] modConnFace        - Connectivity of the face after the renumbering.
  * \param[out] modConnElem        - Connectivity of the element after the renumbering.
                                     This renumbering is such that the face corresponds
                                     to the appropriate face of the element used in the
                                     standard faces and also the corner points match.
  */
  void CreateConnectivitiesFace(const unsigned short        VTK_TypeFace,
                                const unsigned long         *cornerPointsFace,
                                const unsigned short        VTK_TypeElem,
                                const unsigned short        nPolyGrid,
                                const vector<unsigned long> &elemNodeIDsGrid,
                                const unsigned short        nPolyConn,
                                const unsigned long         *connElem,
                                bool                        &swapFaceInElement,
                                unsigned long               *modConnFace,
                                unsigned long               *modConnElem);

  /*!
  * \brief Function, which computes the correct sequence of the connectivities
           of a line adjacent to a quadrilateral, such that the line is face 0
           of the quadrilateral and it matches the sequence of the given corner points.
  * \param[in]  cornerPointsLine - The corner points of the line in the desired
                                   sequence.
  * \param[in]  nPolyGrid        - Polynomial degree used in the grid definition
                                   for the line and the quadrilateral.
  * \param[in]  quadNodeIDsGrid  - The node IDs of the grid DOFs of the quadrilateral,
                                   i.e. the element connectivity.
  * \param[in]  nPolyConn        - Polynomial degree of the connectivities to
                                   be modified.
  * \param[in]  connQuad         - Connectivity of the adjacent quadrilateral.
  * \param[out] modConnLine      - Connectivity of the line after the renumbering.
  * \param[out] modConnQuad      - Connectivity of the quadrilateral after the
                                   renumbering. This renumbering is such that the
                                   line corresponds to face 0 of the quadrilateral.
  */
  void CreateConnectivitiesLineAdjacentQuadrilateral(
                                  const unsigned long         *cornerPointsLine,
                                  const unsigned short        nPolyGrid,
                                  const vector<unsigned long> &quadNodeIDsGrid,
                                  const unsigned short        nPolyConn,
                                  const unsigned long         *connQuad,
                                  unsigned long               *modConnLine,
                                  unsigned long               *modConnQuad);

  /*!
  * \brief Function, which computes the correct sequence of the connectivities
           of a line adjacent to a triangle, such that the line is face 0
           of the triangle and it matches the sequence of the given corner points.
  * \param[in]  cornerPointsLine - The corner points of the line in the desired
                                   sequence.
  * \param[in]  nPolyGrid        - Polynomial degree used in the grid definition
                                   for the line and the triangle.
  * \param[in]  triaNodeIDsGrid  - The node IDs of the grid DOFs of the triangle,
                                   i.e. the element connectivity.
  * \param[in]  nPolyConn        - Polynomial degree of the connectivities to
                                   be modified.
  * \param[in]  connTria         - Connectivity of the adjacent triangle.
  * \param[out] modConnLine      - Connectivity of the line after the renumbering.
  * \param[out] modConnTria      - Connectivity of the triangle after the
                                   renumbering. This renumbering is such that the
                                   line corresponds to face 0 of the triangle.
  */
  void CreateConnectivitiesLineAdjacentTriangle(
                                  const unsigned long         *cornerPointsLine,
                                  const unsigned short        nPolyGrid,
                                  const vector<unsigned long> &triaNodeIDsGrid,
                                  const unsigned short        nPolyConn,
                                  const unsigned long         *connTria,
                                  unsigned long               *modConnLine,
                                  unsigned long               *modConnTria);

  /*!
  * \brief Function, which computes the correct sequence of the connectivities
           of a quad adjacent to a hexahedron, such that the quad is face 0
           of the hexahedron and it matches the sequence of the given corner points.
  * \param[in]  cornerPointsQuad - The corner points of the quad in the desired
                                   sequence.
  * \param[in]  nPolyGrid        - Polynomial degree used in the grid definition
                                   for the quadrilateral and the hexahedron.
  * \param[in]  hexaNodeIDsGrid  - The node IDs of the grid DOFs of the
                                   hexahedron, i.e. the element connectivity.
  * \param[in]  nPolyConn        - Polynomial degree of the connectivities to
                                   be modified.
  * \param[in]  connHexa         - Connectivity of the adjacent hexahedron.
  * \param[out] modConnQuad      - Connectivity of the quad after the renumbering.
  * \param[out] modConnHexa      - Connectivity of the hexahedron after the
                                   renumbering. This renumbering is such that the
                                   quad corresponds to face 0 of the hexahedron.
  */
  void CreateConnectivitiesQuadrilateralAdjacentHexahedron(
                                  const unsigned long         *cornerPointsQuad,
                                  const unsigned short        nPolyGrid,
                                  const vector<unsigned long> &hexaNodeIDsGrid,
                                  const unsigned short        nPolyConn,
                                  const unsigned long         *connHexa,
                                  unsigned long               *modConnQuad,
                                  unsigned long               *modConnHexa);

  /*!
  * \brief Function, which computes the correct sequence of the connectivities
           of a quad adjacent to a prism, such that the quad is face 2
           of the prism and it matches the sequence of the given corner points.
  * \param[in]  cornerPointsQuad  - The corner points of the quad in the desired
                                    sequence.
  * \param[in]  nPolyGrid         - Polynomial degree used in the grid definition
                                    for the quadrilateral and the prism.
  * \param[in]  prismNodeIDsGrid  - The node IDs of the grid DOFs of the prism,
                                    i.e. the element connectivity.
  * \param[in]  nPolyConn         - Polynomial degree of the connectivities to
                                    be modified.
  * \param[in]  connPrism         - Connectivity of the adjacent prism.
  * \param[out] swapFaceInElement - Whether or not the connectivity of the face must
                                    be swapped compared to the face of the corresponding
                                    standard element.
  * \param[out] modConnQuad       - Connectivity of the quad after the renumbering.
  * \param[out] modConnPrism      - Connectivity of the prism after the
                                    renumbering. This renumbering is such that the
                                    quad corresponds to face 3 of the prism.
  */
  void CreateConnectivitiesQuadrilateralAdjacentPrism(
                                  const unsigned long         *cornerPointsQuad,
                                  const unsigned short        nPolyGrid,
                                  const vector<unsigned long> &prismNodeIDsGrid,
                                  const unsigned short        nPolyConn,
                                  const unsigned long         *connPrism,
                                  bool                        &swapFaceInElement,
                                  unsigned long               *modConnQuad,
                                  unsigned long               *modConnPrism);

  /*!
  * \brief Function, which computes the correct sequence of the connectivities
           of a quad adjacent to a pyramid, such that the quad is face 0
           of the pyramid and it matches the sequence of the given corner points.
  * \param[in]  cornerPointsQuad - The corner points of the quad in the desired
                                   sequence.
  * \param[in]  nPolyGrid        - Polynomial degree used in the grid definition
                                   for the quadrilateral and the pyramid.
  * \param[in]  pyraNodeIDsGrid  - The node IDs of the grid DOFs of the pyramid,
                                   i.e. the element connectivity.
  * \param[in]  nPolyConn        - Polynomial degree of the connectivities to
                                   be modified.
  * \param[in]  connPyra         - Connectivity of the adjacent pyramid.
  * \param[out] modConnQuad      - Connectivity of the quad after the renumbering.
  * \param[out] modConnPyra      - Connectivity of the pyramid after the
                                   renumbering. This renumbering is such that the
                                   quad corresponds to face 0 of the pyramid.
  */
  void CreateConnectivitiesQuadrilateralAdjacentPyramid(
                                  const unsigned long         *cornerPointsQuad,
                                  const unsigned short        nPolyGrid,
                                  const vector<unsigned long> &pyraNodeIDsGrid,
                                  const unsigned short        nPolyConn,
                                  const unsigned long         *connPyra,
                                  unsigned long               *modConnQuad,
                                  unsigned long               *modConnPyra);

  /*!
  * \brief Function, which computes the correct sequence of the connectivities
           of a triangle adjacent to a prism, such that the triangle is face 0
           of the prism and it matches the sequence of the given corner points.
  * \param[in]  cornerPointsTria - The corner points of the triangle in the
                                   desired sequence.
  * \param[in]  nPolyGrid        - Polynomial degree used in the grid definition
                                   for the triangle and the prism.
  * \param[in]  prismNodeIDsGrid - The node IDs of the grid DOFs of the prism,
                                   i.e. the element connectivity.
  * \param[in]  nPolyConn        - Polynomial degree of the connectivities to
                                   be modified.
  * \param[in]  connPrism        - Connectivity of the adjacent prism.
  * \param[out] modConnTria      - Connectivity of the triangle after the renumbering.
  * \param[out] modConnPrism     - Connectivity of the prism after the
                                   renumbering. This renumbering is such that the
                                   triangle corresponds to face 0 of the prism.
  */
  void CreateConnectivitiesTriangleAdjacentPrism(
                                  const unsigned long         *cornerPointsTria,
                                  const unsigned short        nPolyGrid,
                                  const vector<unsigned long> &prismNodeIDsGrid,
                                  const unsigned short        nPolyConn,
                                  const unsigned long         *connPrism,
                                  unsigned long               *modConnTria,
                                  unsigned long               *modConnPrism);

  /*!
  * \brief Function, which computes the correct sequence of the connectivities
           of a triangle adjacent to a pyramid, such that the triangle is face 1
           of the pyramid and it matches the sequence of the given corner points.
  * \param[in]  cornerPointsTria   - The corner points of the triangle in the
                                     desired sequence.
  * \param[in]  nPolyGrid          - Polynomial degree used in the grid definition
                                     for the triangle and the pyramid.
  * \param[in]  pyraNodeIDsGrid    - The node IDs of the grid DOFs of the pyramid,
                                     i.e. the element connectivity.
  * \param[in]  nPolyConn          - Polynomial degree of the connectivities to
                                     be modified.
  * \param[in]  connPyra           - Connectivity of the adjacent pyramid.
  * \param[out] swapFaceInElement  - Whether or not the connectivity of the face must
                                     be swapped compared to the face of the corresponding
                                     standard element.
  * \param[out] modConnTria        - Connectivity of the triangle after the renumbering.
  * \param[out] modConnPyra        - Connectivity of the pyramid after the
                                     renumbering. This renumbering is such that the
                                     triangle corresponds to face 3 of the pyramid.
  */
  void CreateConnectivitiesTriangleAdjacentPyramid(
                                  const unsigned long         *cornerPointsTria,
                                  const unsigned short        nPolyGrid,
                                  const vector<unsigned long> &pyraNodeIDsGrid,
                                  const unsigned short        nPolyConn,
                                  const unsigned long         *connPyra,
                                  bool                        &swapFaceInElement,
                                  unsigned long               *modConnTria,
                                  unsigned long               *modConnPyra);

  /*!
  * \brief Function, which computes the correct sequence of the connectivities
           of a triangle adjacent to a tetrahedron, such that the triangle is face 0
           of the tetrahedron and it matches the sequence of the given corner points.
  * \param[in]  cornerPointsTria - The corner points of the triangle in the
                                   desired sequence.
  * \param[in]  nPolyGrid        - Polynomial degree used in the grid definition
                                   for the triangle and the tetrahedron.
  * \param[in]  tetNodeIDsGrid   - The node IDs of the grid DOFs of the
                                   tetrahedron, i.e. the element connectivity.
  * \param[in]  nPolyConn        - Polynomial degree of the connectivities to
                                   be modified.
  * \param[in]  connTet          - Connectivity of the adjacent tetrahedron.
  * \param[out] modConnTria      - Connectivity of the triangle after the renumbering.
  * \param[out] modConnTet       - Connectivity of the tetrahedron after the
                                   renumbering. This renumbering is such that the
                                   triangle corresponds to face 0 of the tetrahedron.
  */
  void CreateConnectivitiesTriangleAdjacentTetrahedron(
                                  const unsigned long         *cornerPointsTria,
                                  const unsigned short        nPolyGrid,
                                  const vector<unsigned long> &tetNodeIDsGrid,
                                  const unsigned short        nPolyConn,
                                  const unsigned long         *connTet,
                                  unsigned long               *modConnTria,
                                  unsigned long               *modConnTet);

  /*!
  * \brief Function, which computes the metric terms for internal
           matching faces.
  * \param[in] config - Definition of the particular problem.
  */
  void MetricTermsMatchingFaces(CConfig *config);

  /*!
  * \brief Function, which computes the time coefficients in the iteration matrix
           of the ADER-DG predictor step.
  * \param[in]  config       - Definition of the particular problem.
  * \param[out] timeCoefAder - The coefficients to be computed.
  */
  void TimeCoefficientsPredictorADER_DG(CConfig           *config,
                                        vector<su2double> &timeCoefAder);

  /*!
  * \brief Function, which computes the volume metric terms for the given
           entities from the gradients of the coordinates.
  * \param[in]  nEntities    - Number of entities for which the metric terms
                               must be computed.
  * \param[in]  gradCoor     - The gradients of the coordinates (w.r.t. the
                               parametric coordinates) from which the metric
                               terms must be computed.
  * \param[out] metricTerms  - Vector in which the metric terms must be stored.
  */
  void VolumeMetricTermsFromCoorGradients(const unsigned short nEntities,
                                          const su2double      *gradCoor,
                                          vector<su2double>    &metricTerms);
};

#include "fem_geometry_structure.inl"
