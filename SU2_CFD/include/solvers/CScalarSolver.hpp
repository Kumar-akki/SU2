/*!
 * \file CScalarSolver.hpp
 * \brief Headers of the CScalarSolver class
 * \author A. Bueno.
 * \version 7.2.0 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2021, SU2 Contributors (cf. AUTHORS.md)
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

#include <vector>
#include "../../../Common/include/parallelization/omp_structure.hpp"
#include "../variables/CScalarVariable.hpp"
#include "CSolver.hpp"

/*!
 * \class CScalarSolver
 * \brief Main class for defining the turbulence model solver.
 * \ingroup Turbulence_Model
 * \author A. Bueno.
 */
class CScalarSolver : public CSolver {
 protected:
  enum : size_t { MAXNDIM = 3 };      /*!< \brief Max number of space dimensions, used in some static arrays. */
  enum : size_t { MAXNVAR = 2 };      /*!< \brief Max number of variables, used in some static arrays. */
  enum : size_t { MAXNVARFLOW = 12 }; /*!< \brief Max number of flow variables, used in some static arrays. */

  enum : size_t { OMP_MAX_SIZE = 512 }; /*!< \brief Max chunk size for light point loops. */
  enum : size_t { OMP_MIN_SIZE = 32 };  /*!< \brief Min chunk size for edge loops (max is color group size). */

  unsigned long omp_chunk_size; /*!< \brief Chunk size used in light point loops. */

  su2double lowerlimit[MAXNVAR] = {0.0}; /*!< \brief contains lower limits for turbulence variables. Note that ::min() returns the smallest positive value for floats. */
  su2double upperlimit[MAXNVAR] = {0.0}; /*!< \brief contains upper limits for turbulence variables. */

  su2double Solution_Inf[MAXNVAR] = {0.0}; /*!< \brief Far-field solution. */

  const bool Conservative; /*!< \brief Transported Variable is conservative. Solution has to be multiplied with rho. */

  /*--- Shallow copy of grid coloring for OpenMP parallelization. ---*/

#ifdef HAVE_OMP
  vector<GridColor<> > EdgeColoring; /*!< \brief Edge colors. */
  bool ReducerStrategy = false;      /*!< \brief If the reducer strategy is in use. */
#else
  array<DummyGridColor<>, 1> EdgeColoring;
  /*--- Never use the reducer strategy if compiling for MPI-only. ---*/
  static constexpr bool ReducerStrategy = false;
#endif

  /*--- Edge fluxes for reducer strategy (see the notes in CEulerSolver.hpp). ---*/
  CSysVector<su2double> EdgeFluxes; /*!< \brief Flux across each edge. */

  /*!
   * \brief The highest level in the variable hierarchy this solver can safely use.
   */
  CScalarVariable* nodes = nullptr;

  /*!
   * \brief Return nodes to allow CSolver::base_nodes to be set.
   */
  inline CVariable* GetBaseClassPointerToNodes() final { return nodes; }

 private:  // changed from private in CTurbSolver.hpp
  /*!
   * \brief Compute the viscous flux for the turbulent equation at a particular edge.
   * \param[in] iEdge - Edge for which we want to compute the flux
   * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] solver_container - Container vector with all the solutions.
   * \param[in] numerics - Description of the numerical method.
   * \param[in] config - Definition of the particular problem.
   */
  void Viscous_Residual(unsigned long iEdge, CGeometry* geometry, CSolver** solver_container, CNumerics* numerics,
                        CConfig* config);
  using CSolver::Viscous_Residual; /*--- Silence warning ---*/

  /*!
   * \brief Sum the edge fluxes for each cell to populate the residual vector, only used on coarse grids.
   * \param[in] geometry - Geometrical definition of the problem.
   */
  void SumEdgeFluxes(CGeometry* geometry);

  /*!
   * \brief Compute a suitable under-relaxation parameter to limit the change in the solution variables over
   * a nonlinear iteration for stability. Default value 1.0 set in ctor of CScalarVariable.
   * \param[in] config - Definition of the particular problem.
   */
  virtual void ComputeUnderRelaxationFactor(const CConfig* config) {}

 public:
  /*!
   * \brief Constructor of the class.
   */
  CScalarSolver(bool conservative);

  /*!
   * \brief Destructor of the class.
   */
  ~CScalarSolver() override;

  /*!
   * \brief Constructor of the class.
   * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] config - Definition of the particular problem.
   */
  CScalarSolver(CGeometry* geometry, CConfig* config, bool conservative);

  /*!
   * \brief Compute the spatial integration using a upwind scheme.
   * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] solver_container - Container vector with all the solutions.
   * \param[in] numerics_container - Description of the numerical method.
   * \param[in] config - Definition of the particular problem.
   * \param[in] iMesh - Index of the mesh in multigrid computations.
   */
  void Upwind_Residual(CGeometry* geometry, CSolver** solver_container, CNumerics** numerics_container, CConfig* config,
                       unsigned short iMesh) override;

  /*!
   * \brief Impose the Symmetry Plane boundary condition.
   * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] solver_container - Container vector with all the solutions.
   * \param[in] conv_numerics - Description of the numerical method.
   * \param[in] visc_numerics - Description of the numerical method.
   * \param[in] config - Definition of the particular problem.
   * \param[in] val_marker - Surface marker where the boundary condition is applied.
   */
  inline void BC_Sym_Plane(CGeometry* geometry, CSolver** solver_container, CNumerics* conv_numerics,
                           CNumerics* visc_numerics, CConfig* config, unsigned short val_marker) override{
      /*--- Convective and viscous fluxes across symmetry plane are equal to zero. ---*/
  };

  /*!
   * \brief Impose via the residual the Euler wall boundary condition.
   * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] solver_container - Container vector with all the solutions.
   * \param[in] conv_numerics - Description of the numerical method.
   * \param[in] visc_numerics - Description of the numerical method.
   * \param[in] config - Definition of the particular problem.
   * \param[in] val_marker - Surface marker where the boundary condition is applied.
   */
  inline void BC_Euler_Wall(CGeometry* geometry, CSolver** solver_container, CNumerics* conv_numerics,
                            CNumerics* visc_numerics, CConfig* config, unsigned short val_marker) override{
      /*--- Convective fluxes across euler wall are equal to zero. ---*/
  };

  /*!
   * \brief Impose a periodic boundary condition by summing contributions from the complete control volume.
   * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] solver_container - Container vector with all the solutions.
   * \param[in] numerics - Description of the numerical method.
   * \param[in] config - Definition of the particular problem.
   */
  void BC_Periodic(CGeometry* geometry, CSolver** solver_container, CNumerics* numerics, CConfig* config) final;

  /*!
   * \brief Set the solution using the Freestream values.
   * \param[in] config - Definition of the particular problem.
   */
  inline void SetFreeStream_Solution(const CConfig* config) final {
    SU2_OMP_FOR_STAT(omp_chunk_size)
    for (unsigned long iPoint = 0; iPoint < nPoint; iPoint++) {
      nodes->SetSolution(iPoint, Solution_Inf);
    }
    END_SU2_OMP_FOR
  }

  /*!
   * \brief Prepare an implicit iteration.
   * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] solver_container - Container vector with all the solutions.
   * \param[in] config - Definition of the particular problem.
   */
  void PrepareImplicitIteration(CGeometry* geometry, CSolver** solver_container, CConfig* config) final;

  /*!
   * \brief Complete an implicit iteration.
   * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] solver_container - Container vector with all the solutions.
   * \param[in] config - Definition of the particular problem.
   */
  void CompleteImplicitIteration(CGeometry* geometry, CSolver** solver_container, CConfig* config) final;

  /*!
   * \brief Update the solution using an implicit solver.
   * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] solver_container - Container vector with all the solutions.
   * \param[in] config - Definition of the particular problem.
   */
  void ImplicitEuler_Iteration(CGeometry* geometry, CSolver** solver_container, CConfig* config) override;

  /*!
   * \brief Set the total residual adding the term that comes from the Dual Time-Stepping Strategy.
   * \param[in] geometry - Geometric definition of the problem.
   * \param[in] solver_container - Container vector with all the solutions.
   * \param[in] config - Definition of the particular problem.
   * \param[in] iRKStep - Current step of the Runge-Kutta iteration.
   * \param[in] iMesh - Index of the mesh in multigrid computations.
   * \param[in] RunTime_EqSystem - System of equations which is going to be solved.
   */
  void SetResidual_DualTime(CGeometry* geometry, CSolver** solver_container, CConfig* config, unsigned short iRKStep,
                            unsigned short iMesh, unsigned short RunTime_EqSystem) final;

  /*!
   * \brief Load a solution from a restart file.
   * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] solver - Container vector with all of the solvers.
   * \param[in] config - Definition of the particular problem.
   * \param[in] val_iter - Current external iteration number.
   * \param[in] val_update_geo - Flag for updating coords and grid velocity.
   */
  void LoadRestart(CGeometry** geometry, CSolver*** solver, CConfig* config, int val_iter, bool val_update_geo) final;

  /*!
   * \brief Scalar solvers support OpenMP+MPI.
   */
  inline bool GetHasHybridParallel() const override { return true; }
};