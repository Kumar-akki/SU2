/*!
 * \file CFEAVariable.hpp
 * \brief Class for defining the variables of the FEM structural problem.
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

#pragma once

#include "CVariable.hpp"

/*!
 * \class CFEAVariable
 * \brief Class for defining the variables of the FEM structural problem.
 * \ingroup Structural Finite Element Analysis Variables
 * \author F. Palacios, R. Sanchez.
 * \version 6.2.0 "Falcon"
 */
class CFEAVariable : public CVariable {
protected:

  Mat_t Stress;  /*!< \brief Stress tensor. */

  Mat_t Residual_Ext_Body;  /*!< \brief Term of the residual due to body forces */

  Vec_t VonMises_Stress;    /*!< \brief Von Mises stress. */

  Mat_t Solution_Vel;         /*!< \brief Velocity of the nodes. */
  Mat_t Solution_Vel_time_n;  /*!< \brief Velocity of the nodes at time n. */

  Mat_t Solution_Accel;         /*!< \brief Acceleration of the nodes. */
  Mat_t Solution_Accel_time_n;  /*!< \brief Acceleration of the nodes at time n. */

  Mat_t Solution_Pred;       /*!< \brief Predictor of the solution for FSI purposes */
  Mat_t Solution_Pred_Old;   /*!< \brief Predictor of the solution at time n for FSI purposes */

  Mat_t Reference_Geometry;  /*!< \brief Reference solution for optimization problems */

  Mat_t Prestretch;          /*!< \brief Prestretch geometry */

  Mat_t Solution_BGS_k;      /*!< \brief Old solution container for BGS iterations ---*/

  /*!
   * \brief Constructor of the class.
   * \note This class is not supposed to be instantiated, it is only a building block for CFEABoundVariable
   * \param[in] val_fea - Values of the fea solution (initialization value).
   * \param[in] npoint - Number of points/nodes/vertices in the domain.
   * \param[in] ndim - Number of dimensions of the problem.
   * \param[in] nvar - Number of variables of the problem.
   * \param[in] config - Definition of the particular problem.
   */
  CFEAVariable(const su2double *val_fea, Idx_t npoint, Idx_t ndim, Idx_t nvar, CConfig *config);

public:
  /*!
   * \brief Destructor of the class.
   */
  virtual ~CFEAVariable() = default;

  /*!
   * \brief Get the value of the stress.
   * \return Value of the stress.
   */
  inline su2double *GetStress_FEM(Idx_t iPoint) final { return Stress[iPoint]; }

  /*!
   * \brief Set the value of the stress at the node
   * \param[in] iVar - index of the stress term
   * \param[in] val_stress - value of the stress
   */
  inline void SetStress_FEM(Idx_t iPoint, Idx_t iVar, su2double val_stress) final { Stress(iPoint,iVar) = val_stress; }

  /*!
   * \brief Add a certain value to the value of the stress at the node
   * \param[in] iVar - index of the stress term
   * \param[in] val_stress - value of the stress
   */
  inline void AddStress_FEM(Idx_t iPoint, Idx_t iVar, su2double val_stress) final { Stress(iPoint,iVar) += val_stress; }

  /*!
   * \brief Add body forces to the residual term.
   */
  inline void Add_BodyForces_Res(Idx_t iPoint, const su2double *val_bodyForce) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++)
      Residual_Ext_Body(iPoint,iVar) += val_bodyForce[iVar];
  }

  /*!
   * \brief Clear the surface load residual
   */
  inline void Clear_BodyForces_Res(Idx_t iPoint) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Residual_Ext_Body(iPoint,iVar) = 0.0;
  }

  /*!
   * \brief Get the body forces.
   */
  inline su2double Get_BodyForces_Res(Idx_t iPoint, Idx_t iVar) const final { return Residual_Ext_Body(iPoint,iVar); }

  /*!
   * \brief Set the value of the old solution.
   * \param[in] val_solution_old - Pointer to the residual vector.
   */
  inline void SetSolution_time_n(Idx_t iPoint) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_time_n(iPoint,iVar) = Solution(iPoint,iVar);
  }

  /*!
   * \brief Set the value of the old solution.
   * \param[in] val_solution_time_n - Pointer to the residual vector.
   */
  inline void SetSolution_time_n(Idx_t iPoint, const su2double *val_solution_time_n) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_time_n(iPoint,iVar) = val_solution_time_n[iVar];
  }

  /*!
   * \brief Set the value of the old solution.
   * \param[in] val_solution - Pointer to the residual vector.
   */
  inline void SetSolution_time_n(Idx_t iPoint, Idx_t iVar, su2double val_solution) final {
    Solution_time_n(iPoint,iVar) = val_solution;
  }

  /*!
   * \brief Set the value of the velocity (Structural Analysis).
   * \param[in] val_solution_vel - Solution of the problem (velocity).
   */
  inline void SetSolution_Vel(Idx_t iPoint, const su2double *val_solution_vel) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_Vel(iPoint,iVar) = val_solution_vel[iVar];
  }

  /*!
   * \overload
   * \param[in] iVar - Index of the variable.
   * \param[in] val_solution_vel - Value of the solution for the index <i>iVar</i>.
   */
  inline void SetSolution_Vel(Idx_t iPoint, Idx_t iVar, su2double val_solution_vel) final {
    Solution_Vel(iPoint,iVar) = val_solution_vel;
  }

  /*!
   * \brief Set the value of the velocity (Structural Analysis) at time n.
   * \param[in] val_solution - Solution of the problem (acceleration).
   */
  inline void SetSolution_Vel_time_n(Idx_t iPoint) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_Vel_time_n(iPoint,iVar) = Solution_Vel(iPoint,iVar);
  }

  /*!
   * \brief Set the value of the velocity (Structural Analysis) at time n.
   * \param[in] val_solution_vel_time_n - Pointer to the residual vector.
   */
  inline void SetSolution_Vel_time_n(Idx_t iPoint, const su2double *val_solution_vel_time_n) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_Vel_time_n(iPoint,iVar) = val_solution_vel_time_n[iVar];
  }

  /*!
   * \overload
   * \param[in] iVar - Index of the variable.
   * \param[in] val_solution_vel_time_n - Value of the old solution for the index <i>iVar</i>.
   */
  inline void SetSolution_Vel_time_n(Idx_t iPoint, Idx_t iVar, su2double val_solution_vel_time_n) final {
    Solution_Vel_time_n(iPoint,iVar) = val_solution_vel_time_n;
  }

  /*!
   * \brief Get the velocity (Structural Analysis).
   * \param[in] iVar - Index of the variable.
   * \return Value of the solution for the index <i>iVar</i>.
   */
  inline su2double GetSolution_Vel(Idx_t iPoint, Idx_t iVar) const final { return Solution_Vel(iPoint,iVar); }

  /*!
   * \brief Get the solution of the problem.
   * \return Pointer to the solution vector.
   */
  inline su2double *GetSolution_Vel(Idx_t iPoint) final { return Solution_Vel[iPoint]; }

  /*!
   * \brief Get the velocity of the nodes (Structural Analysis) at time n.
   * \param[in] iVar - Index of the variable.
   * \return Pointer to the old solution vector.
   */
  inline su2double GetSolution_Vel_time_n(Idx_t iPoint, Idx_t iVar) const final {
    return Solution_Vel_time_n(iPoint,iVar);
  }

  /*!
   * \brief Get the solution at time n.
   * \return Pointer to the solution (at time n) vector.
   */
  inline su2double *GetSolution_Vel_time_n(Idx_t iPoint) final { return Solution_Vel_time_n[iPoint]; }

  /*!
   * \brief Set the value of the acceleration (Structural Analysis).
   * \param[in] val_solution_accel - Solution of the problem (acceleration).
   */
  inline void SetSolution_Accel(Idx_t iPoint, const su2double *val_solution_accel) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_Accel(iPoint,iVar) = val_solution_accel[iVar];
  }

  /*!
   * \overload
   * \param[in] iVar - Index of the variable.
   * \param[in] val_solution_accel - Value of the solution for the index <i>iVar</i>.
   */
  inline void SetSolution_Accel(Idx_t iPoint, Idx_t iVar, su2double val_solution_accel) final {
    Solution_Accel(iPoint,iVar) = val_solution_accel;
  }

  /*!
   * \brief Set the value of the acceleration (Structural Analysis) at time n.
   * \param[in] val_solution_old - Pointer to the residual vector.
   */
  inline void SetSolution_Accel_time_n(Idx_t iPoint, const su2double *val_solution_accel_time_n) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++)
      Solution_Accel_time_n(iPoint,iVar) = val_solution_accel_time_n[iVar];
  }

  /*!
   * \brief Set the value of the acceleration (Structural Analysis) at time n.
   * \param[in] val_solution - Solution of the problem (acceleration).
   */
  inline void SetSolution_Accel_time_n(Idx_t iPoint) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_Accel_time_n(iPoint,iVar) = Solution_Accel(iPoint,iVar);
  }

  /*!
   * \overload
   * \param[in] iVar - Index of the variable.
   * \param[in] val_solution_accel_time_n - Value of the old solution for the index <i>iVar</i>.
   */
  inline void SetSolution_Accel_time_n(Idx_t iPoint, Idx_t iVar, su2double val_solution_accel_time_n) final {
    Solution_Accel_time_n(iPoint,iVar) = val_solution_accel_time_n;
  }

  /*!
   * \brief Get the acceleration (Structural Analysis).
   * \param[in] iVar - Index of the variable.
   * \return Value of the solution for the index <i>iVar</i>.
   */
  inline su2double GetSolution_Accel(Idx_t iPoint, Idx_t iVar) const final { return Solution_Accel(iPoint,iVar); }

  /*!
   * \brief Get the solution of the problem.
   * \return Pointer to the solution vector.
   */
  inline su2double *GetSolution_Accel(Idx_t iPoint) final { return Solution_Accel[iPoint]; }

  /*!
   * \brief Get the acceleration of the nodes (Structural Analysis) at time n.
   * \param[in] iVar - Index of the variable.
   * \return Pointer to the old solution vector.
   */
  inline su2double GetSolution_Accel_time_n(Idx_t iPoint, Idx_t iVar) const final {
    return Solution_Accel_time_n(iPoint,iVar);
  }

  /*!
   * \brief Get the solution at time n.
   * \return Pointer to the solution (at time n) vector.
   */
  inline su2double *GetSolution_Accel_time_n(Idx_t iPoint) final { return Solution_Accel_time_n[iPoint]; }

  /*!
   * \brief Set the value of the solution predictor.
   */
  inline void SetSolution_Pred(Idx_t iPoint) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_Pred(iPoint,iVar) = Solution(iPoint,iVar);
  }

  /*!
   * \brief Set the value of the old solution.
   * \param[in] val_solution_pred - Pointer to the residual vector.
   */
  inline void SetSolution_Pred(Idx_t iPoint, const su2double *val_solution_pred) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_Pred(iPoint,iVar) = val_solution_pred[iVar];
  }

  /*!
   * \brief  Set the value of the predicted solution.
   * \param[in] iVar - Index of the variable
   * \param[in] val_solution_pred - Value of the predicted solution.
   */
  inline void SetSolution_Pred(Idx_t iPoint, Idx_t iVar, su2double val_solution_pred) final {
    Solution_Pred(iPoint,iVar) = val_solution_pred;
  }

  /*!
   * \brief Get the value of the solution predictor.
   * \param[in] iVar - Index of the variable.
   * \return Pointer to the old solution vector.
   */
  inline su2double GetSolution_Pred(Idx_t iPoint, Idx_t iVar) const final { return Solution_Pred(iPoint,iVar); }

  /*!
   * \brief Get the solution at time n.
   * \return Pointer to the solution (at time n) vector.
   */
  inline su2double *GetSolution_Pred(Idx_t iPoint) final { return Solution_Pred[iPoint]; }

  /*!
   * \brief Set the value of the solution predictor.
   */
  inline void SetSolution_Pred_Old(Idx_t iPoint) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_Pred_Old(iPoint,iVar) = Solution_Pred(iPoint,iVar);
  }

  /*!
   * \brief Set the value of the old solution.
   * \param[in] val_solution_pred_old - Pointer to the residual vector.
   */
  inline void SetSolution_Pred_Old(Idx_t iPoint, const su2double *val_solution_pred_old) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_Pred_Old(iPoint,iVar) = val_solution_pred_old[iVar];
  }

  /*!
   * \brief  A virtual member. Set the value of the old solution predicted.
   * \param[in] iVar - Index of the variable
   * \param[in] val_solution_pred_old - Value of the old predicted solution.
   */
  inline void SetSolution_Pred_Old(Idx_t iPoint, Idx_t iVar, su2double val_solution_pred_old) final {
    Solution_Pred_Old(iPoint,iVar) = val_solution_pred_old;
  }

  /*!
   * \brief Get the value of the solution predictor.
   * \param[in] iVar - Index of the variable.
   * \return Pointer to the old solution vector.
   */
  inline su2double GetSolution_Pred_Old(Idx_t iPoint, Idx_t iVar) const final {
    return Solution_Pred_Old(iPoint,iVar);
  }

  /*!
   * \brief Get the solution at time n.
   * \return Pointer to the solution (at time n) vector.
   */
  inline su2double *GetSolution_Pred_Old(Idx_t iPoint) final { return Solution_Pred_Old[iPoint]; }

  /*!
   * \brief A virtual member.
   */
  inline void SetPrestretch(Idx_t iPoint, Idx_t iVar, su2double val_prestretch) final {
    Prestretch(iPoint,iVar) = val_prestretch;
  }

  /*!
   * \brief A virtual member.
   */
  inline su2double *GetPrestretch(Idx_t iPoint) final { return Prestretch[iPoint]; }

  /*!
   * \brief A virtual member.
   */
  inline su2double GetPrestretch(Idx_t iPoint, Idx_t iVar) const final { return Prestretch(iPoint,iVar); }

  /*!
   * \brief Set the value of the Von Mises stress.
   * \param[in] val_stress - Value of the Von Mises stress.
   */
  inline void SetVonMises_Stress(Idx_t iPoint, su2double val_stress) final { VonMises_Stress(iPoint) = val_stress; }

  /*!
   * \brief Get the value of the Von Mises stress.
   * \return Value of the Von Mises stress.
   */
  inline su2double GetVonMises_Stress(Idx_t iPoint) const final { return VonMises_Stress(iPoint); }

  /*!
   * \brief Set the reference geometry.
   * \return Pointer to the solution (at time n) vector.
   */
  inline void SetReference_Geometry(Idx_t iPoint, Idx_t iVar, su2double ref_geometry) final {
    Reference_Geometry(iPoint,iVar) = ref_geometry;
  }

  /*!
   * \brief Get the pointer to the reference geometry
   */
  inline su2double *GetReference_Geometry(Idx_t iPoint) final { return Reference_Geometry[iPoint]; }

  /*!
   * \brief Get the value of the reference geometry for the coordinate iVar
   */
  inline su2double GetReference_Geometry(Idx_t iPoint, Idx_t iVar) const final {
    return Reference_Geometry(iPoint,iVar);
  }

  /*!
   * \brief Register the variables in the solution time_n array as input/output variable.
   * \param[in] input - input or output variables.
   */
  inline void Register_femSolution_time_n(Idx_t iPoint) final {
	  for (Idx_t iVar = 0; iVar < nVar; iVar++)
	    AD::RegisterInput(Solution_time_n(iPoint,iVar));
  }

  /*!
   * \brief Register the variables in the velocity array as input/output variable.
   * \param[in] input - input or output variables.
   */
  inline void RegisterSolution_Vel(Idx_t iPoint, bool input) final {
	  if (input)
	    for (Idx_t iVar = 0; iVar < nVar; iVar++) AD::RegisterInput(Solution_Vel(iPoint,iVar));
	  else
      for (Idx_t iVar = 0; iVar < nVar; iVar++) AD::RegisterOutput(Solution_Vel(iPoint,iVar));
  }

  /*!
   * \brief Register the variables in the velocity time_n array as input/output variable.
   */
  inline void RegisterSolution_Vel_time_n(Idx_t iPoint) final {
	  for (Idx_t iVar = 0; iVar < nVar; iVar++) AD::RegisterInput(Solution_Vel_time_n(iPoint,iVar));
  }

  /*!
   * \brief Register the variables in the acceleration array as input/output variable.
   * \param[in] input - input or output variables.
   */
  inline void RegisterSolution_Accel(Idx_t iPoint, bool input) final {
	  if (input)
	    for (Idx_t iVar = 0; iVar < nVar; iVar++) AD::RegisterInput(Solution_Accel(iPoint,iVar));
	  else
      for (Idx_t iVar = 0; iVar < nVar; iVar++) AD::RegisterOutput(Solution_Accel(iPoint,iVar));
  }

  /*!
   * \brief Register the variables in the acceleration time_n array as input/output variable.
   */
  inline void RegisterSolution_Accel_time_n(Idx_t iPoint) final {
	  for (Idx_t iVar = 0; iVar < nVar; iVar++) AD::RegisterInput(Solution_Accel_time_n(iPoint,iVar));
  }

  /*!
   * \brief Set the velocity adjoint values of the solution.
   * \param[in] adj_sol - The adjoint values of the solution.
   */
  inline void SetAdjointSolution_Vel(Idx_t iPoint, const su2double *adj_sol) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++)
      SU2_TYPE::SetDerivative(Solution_Vel(iPoint,iVar), SU2_TYPE::GetValue(adj_sol[iVar]));
  }

  /*!
   * \brief Get the velocity adjoint values of the solution.
   * \param[in] adj_sol - The adjoint values of the solution.
   */
  inline void GetAdjointSolution_Vel(Idx_t iPoint, su2double *adj_sol) const final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++)
      adj_sol[iVar] = SU2_TYPE::GetDerivative(Solution_Vel(iPoint,iVar));
  }

  /*!
   * \brief Set the velocity adjoint values of the solution at time n.
   * \param[in] adj_sol - The adjoint values of the solution.
   */
  void SetAdjointSolution_Vel_time_n(Idx_t iPoint, const su2double *adj_sol) final {
	  for (Idx_t iVar = 0; iVar < nVar; iVar++)
      SU2_TYPE::SetDerivative(Solution_Vel_time_n(iPoint,iVar), SU2_TYPE::GetValue(adj_sol[iVar]));
  }

  /*!
   * \brief Get the velocity adjoint values of the solution at time n.
   * \param[in] adj_sol - The adjoint values of the solution.
   */
  inline void GetAdjointSolution_Vel_time_n(Idx_t iPoint, su2double *adj_sol) const final {
	  for (Idx_t iVar = 0; iVar < nVar; iVar++)
      adj_sol[iVar] = SU2_TYPE::GetDerivative(Solution_Vel_time_n(iPoint,iVar));
  }

  /*!
   * \brief Set the acceleration adjoint values of the solution.
   * \param[in] adj_sol - The adjoint values of the solution.
   */
  inline void SetAdjointSolution_Accel(Idx_t iPoint, const su2double *adj_sol) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++)
      SU2_TYPE::SetDerivative(Solution_Accel(iPoint,iVar), SU2_TYPE::GetValue(adj_sol[iVar]));
  }

  /*!
   * \brief Get the acceleration adjoint values of the solution.
   * \param[in] adj_sol - The adjoint values of the solution.
   */
  inline void GetAdjointSolution_Accel(Idx_t iPoint, su2double *adj_sol) const final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++)
      adj_sol[iVar] = SU2_TYPE::GetDerivative(Solution_Accel(iPoint,iVar));
  }

  /*!
   * \brief Set the acceleration adjoint values of the solution at time n.
   * \param[in] adj_sol - The adjoint values of the solution.
   */
  void SetAdjointSolution_Accel_time_n(Idx_t iPoint, const su2double *adj_sol) final {
	  for (Idx_t iVar = 0; iVar < nVar; iVar++)
      SU2_TYPE::SetDerivative(Solution_Accel_time_n(iPoint,iVar), SU2_TYPE::GetValue(adj_sol[iVar]));
  }

  /*!
   * \brief Get the acceleration adjoint values of the solution at time n.
   * \param[in] adj_sol - The adjoint values of the solution.
   */
  inline void GetAdjointSolution_Accel_time_n(Idx_t iPoint, su2double *adj_sol) const final {
	  for (Idx_t iVar = 0; iVar < nVar; iVar++)
      adj_sol[iVar] = SU2_TYPE::GetDerivative(Solution_Accel_time_n(iPoint,iVar));
  }

  /*!
   * \brief Set the value of the solution in the previous BGS subiteration.
   */
  inline void Set_BGSSolution_k(Idx_t iPoint) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++) Solution_BGS_k(iPoint,iVar) = Solution(iPoint,iVar);
  }

  /*!
   * \brief Get the value of the solution in the previous BGS subiteration.
   * \param[out] val_solution - solution in the previous BGS subiteration.
   */
  inline su2double Get_BGSSolution_k(Idx_t iPoint, Idx_t iVar) const final { return Solution_BGS_k(iPoint,iVar); }

};
