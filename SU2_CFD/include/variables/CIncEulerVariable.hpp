/*!
 * \file CIncEulerVariable.hpp
 * \brief Class for defining the variables of the incompressible Euler solver.
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
 * \class CIncEulerVariable
 * \brief Class for defining the variables of the incompressible Euler solver.
 * \ingroup Euler_Equations
 * \author F. Palacios, T. Economon, T. Albring
 */
class CIncEulerVariable : public CVariable {
protected:
  Vec_t Velocity2;              /*!< \brief Square of the velocity vector. */
  Mat_t Primitive;              /*!< \brief Primitive variables (T, vx, vy, vz, P, rho, h, c) in compressible flows. */
  VectorOfMatrix Gradient_Primitive;  /*!< \brief Gradient of the primitive variables (T, vx, vy, vz, P, rho). */
  Mat_t Limiter_Primitive;      /*!< \brief Limiter of the primitive variables (T, vx, vy, vz, P, rho). */
  Mat_t Solution_BGS_k;         /*!< \brief Old solution container for BGS iterations. */
  Vec_t Density_Old;            /*!< \brief Old density for variable density turbulent flows (SST). */

public:
  /*!
   * \brief Constructor of the class.
   * \param[in] val_pressure - value of the pressure.
   * \param[in] velocity - Value of the flow velocity (initialization value).
   * \param[in] temperature - Value of the temperature (initialization value).
   * \param[in] npoint - Number of points/nodes/vertices in the domain.
   * \param[in] ndim - Number of dimensions of the problem.
   * \param[in] nvar - Number of variables of the problem.
   * \param[in] config - Definition of the particular problem.
   */
  CIncEulerVariable(su2double pressure, const su2double *velocity, su2double temperature,
                    Idx_t npoint, Idx_t ndim, Idx_t nvar, CConfig *config);

  /*!
   * \brief Destructor of the class.
   */
  virtual ~CIncEulerVariable() = default;

  /*!
   * \brief Set to zero the gradient of the primitive variables.
   */
  void SetGradient_PrimitiveZero(Idx_t iPoint, Idx_t val_primvar) final;

  /*!
   * \brief Add <i>value</i> to the gradient of the primitive variables.
   * \param[in] iPoint - Point index.
   * \param[in] iVar - Index of the variable.
   * \param[in] iDim - Index of the dimension.
   * \param[in] value - Value to add to the gradient of the primitive variables.
   */
  inline void AddGradient_Primitive(Idx_t iPoint, Idx_t iVar, Idx_t iDim, su2double value) final {
    Gradient_Primitive(iPoint,iVar,iDim) += value;
  }

  /*!
   * \brief Subtract <i>value</i> to the gradient of the primitive variables.
   * \param[in] iPoint - Point index.
   * \param[in] iVar - Index of the variable.
   * \param[in] iDim - Index of the dimension.
   * \param[in] value - Value to subtract to the gradient of the primitive variables.
   */
  inline void SubtractGradient_Primitive(Idx_t iPoint, Idx_t iVar, Idx_t iDim, su2double value) final {
    Gradient_Primitive(iPoint,iVar,iDim) -= value;
  }

  /*!
   * \brief Get the value of the primitive variables gradient.
   * \param[in] iPoint - Point index.
   * \param[in] iVar - Index of the variable.
   * \param[in] iDim - Index of the dimension.
   * \return Value of the primitive variables gradient.
   */
  inline su2double GetGradient_Primitive(Idx_t iPoint, Idx_t iVar, Idx_t iDim) const final {
    return Gradient_Primitive(iPoint,iVar,iDim);
  }

  /*!
   * \brief Get the value of the primitive variables gradient.
   * \param[in] iPoint - Point index.
   * \param[in] iVar - Index of the variable.
   * \return Value of the primitive variables gradient.
   */
  inline su2double GetLimiter_Primitive(Idx_t iPoint, Idx_t iVar) const final {
    return Limiter_Primitive(iPoint,iVar);
  }

  /*!
   * \brief Set the gradient of the primitive variables.
   * \param[in] iPoint - Point index.
   * \param[in] iVar - Index of the variable.
   * \param[in] iDim - Index of the dimension.
   * \param[in] value - Value of the gradient.
   */
  inline void SetGradient_Primitive(Idx_t iPoint, Idx_t iVar, Idx_t iDim, su2double value) final {
    Gradient_Primitive(iPoint,iVar,iDim) = value;
  }

  /*!
   * \brief Set the gradient of the primitive variables.
   * \param[in] iPoint - Point index.
   * \param[in] iVar - Index of the variable.
   * \param[in] value - Value of the gradient.
   */
  inline void SetLimiter_Primitive(Idx_t iPoint, Idx_t iVar, su2double value) final {
    Limiter_Primitive(iPoint,iVar) = value;
  }

  /*!
   * \brief Get the value of the primitive variables gradient.
   * \param[in] iPoint - Point index.
   * \return Value of the primitive variables gradient.
   */
  inline su2double **GetGradient_Primitive(Idx_t iPoint) final { return Gradient_Primitive[iPoint]; }

  /*!
   * \brief Get the value of the primitive variables gradient.
   * \param[in] iPoint - Point index.
   * \return Value of the primitive variables gradient.
   */
  inline su2double *GetLimiter_Primitive(Idx_t iPoint) final { return Limiter_Primitive[iPoint]; }

  /*!
   * \brief Set the value of the pressure.
   * \param[in] iPoint - Point index.
   */
  inline void SetPressure(Idx_t iPoint) final { Primitive(iPoint,0) = Solution(iPoint,0); }

  /*!
   * \brief Get the primitive variables.
   * \param[in] iPoint - Point index.
   * \param[in] iVar - Index of the variable.
   * \return Value of the primitive variable for the index <i>iVar</i>.
   */
  inline su2double GetPrimitive(Idx_t iPoint, Idx_t iVar) const final { return Primitive(iPoint,iVar); }

  /*!
   * \brief Set the value of the primitive variables.
   * \param[in] iPoint - Point index.
   * \param[in] iVar - Index of the variable.
   * \param[in] iVar - Index of the variable.
   * \return Set the value of the primitive variable for the index <i>iVar</i>.
   */
  inline void SetPrimitive(Idx_t iPoint, Idx_t iVar, su2double val_prim) final { Primitive(iPoint,iVar) = val_prim; }

  /*!
   * \brief Set the value of the primitive variables.
   * \param[in] iPoint - Point index.
   * \param[in] val_prim - Primitive variables.
   * \return Set the value of the primitive variable for the index <i>iVar</i>.
   */
  inline void SetPrimitive(Idx_t iPoint, const su2double *val_prim) final {
    for (Idx_t iVar = 0; iVar < nPrimVar; iVar++) Primitive(iPoint,iVar) = val_prim[iVar];
  }

  /*!
   * \brief Get the primitive variables of the problem.
   * \param[in] iPoint - Point index.
   * \return Pointer to the primitive variable vector.
   */
  inline su2double *GetPrimitive(Idx_t iPoint) final { return Primitive[iPoint]; }

  /*!
   * \brief Set the value of the density for the incompressible flows.
   * \param[in] iPoint - Point index.
   */
  inline bool SetDensity(Idx_t iPoint, su2double val_density) final {
    Primitive(iPoint,nDim+2) = val_density;
    if (Primitive(iPoint,nDim+2) > 0.0) return false;
    else return true;
  }

  /*!
   * \brief Set the value of the density for the incompressible flows.
   * \param[in] iPoint - Point index.
   */
  inline void SetVelocity(Idx_t iPoint) final {
    Velocity2(iPoint) = 0.0;
    for (Idx_t iDim = 0; iDim < nDim; iDim++) {
      Primitive(iPoint,iDim+1) = Solution(iPoint,iDim+1);
      Velocity2(iPoint) += pow(Primitive(iPoint,iDim+1),2);
    }
  }

  /*!
   * \brief Set the value of the temperature for incompressible flows with energy equation.
   * \param[in] iPoint - Point index.
   */
  inline bool SetTemperature(Idx_t iPoint, su2double val_temperature) final {
    Primitive(iPoint,nDim+1) = val_temperature;
    if (Primitive(iPoint,nDim+1) > 0.0) return false;
    else return true;
  }

  /*!
   * \brief Set the value of the beta coeffient for incompressible flows.
   * \param[in] iPoint - Point index.
   */
  inline void SetBetaInc2(Idx_t iPoint, su2double val_betainc2) final { Primitive(iPoint,nDim+3) = val_betainc2; }

  /*!
   * \brief Get the norm 2 of the velocity.
   * \return Norm 2 of the velocity vector.
   */
  inline su2double GetVelocity2(Idx_t iPoint) const final { return Velocity2(iPoint); }

  /*!
   * \brief Get the flow pressure.
   * \return Value of the flow pressure.
   */
  inline su2double GetPressure(Idx_t iPoint) const final { return Primitive(iPoint,0); }

  /*!
   * \brief Get the value of beta squared for the incompressible flow
   * \return Value of beta squared.
   */
  inline su2double GetBetaInc2(Idx_t iPoint) const final { return Primitive(iPoint,nDim+3); }

  /*!
   * \brief Get the density of the flow.
   * \return Value of the density of the flow.
   */
  inline su2double GetDensity(Idx_t iPoint) const final { return Primitive(iPoint,nDim+2); }

  /*!
   * \brief Get the density of the flow from the previous iteration.
   * \return Old value of the density of the flow.
   */
  inline su2double GetDensity_Old(Idx_t iPoint) const final { return Density_Old(iPoint); }

  /*!
   * \brief Get the temperature of the flow.
   * \return Value of the temperature of the flow.
   */
  inline su2double GetTemperature(Idx_t iPoint) const final { return Primitive(iPoint,nDim+1); }

  /*!
   * \brief Get the velocity of the flow.
   * \param[in] iDim - Index of the dimension.
   * \return Value of the velocity for the dimension <i>iDim</i>.
   */
  inline su2double GetVelocity(Idx_t iPoint, Idx_t iDim) const final { return Primitive(iPoint,iDim+1); }

  /*!
   * \brief Get the projected velocity in a unitary vector direction (compressible solver).
   * \param[in] val_vector - Direction of projection.
   * \return Value of the projected velocity.
   */
  su2double GetProjVel(Idx_t iPoint, const su2double *val_vector) const final;

  /*!
   * \brief Set the velocity vector from the old solution.
   * \param[in] val_velocity - Pointer to the velocity.
   */
  inline void SetVelocity_Old(Idx_t iPoint, const su2double *val_velocity) final {
    for (Idx_t iDim = 0; iDim < nDim; iDim++)
      Solution_Old(iPoint,iDim+1) = val_velocity[iDim];
  }

  /*!
   * \brief Set all the primitive variables for incompressible flows.
   */
  bool SetPrimVar(Idx_t iPoint, CFluidModel *FluidModel) final;

  /*!
   * \brief Set the specific heat Cp.
   */
  inline void SetSpecificHeatCp(Idx_t iPoint, su2double val_Cp) final { Primitive(iPoint, nDim+7) = val_Cp; }

  /*!
   * \brief Set the specific heat Cv.
   */
  inline void SetSpecificHeatCv(Idx_t iPoint, su2double val_Cv) final { Primitive(iPoint, nDim+8) = val_Cv; }

  /*!
   * \brief Get the specific heat at constant P of the flow.
   * \return Value of the specific heat at constant P of the flow.
   */
  inline su2double GetSpecificHeatCp(Idx_t iPoint) const final { return Primitive(iPoint, nDim+7); }

  /*!
   * \brief Get the specific heat at constant V of the flow.
   * \return Value of the specific heat at constant V of the flow.
   */
  inline su2double GetSpecificHeatCv(Idx_t iPoint) const final { return Primitive(iPoint, nDim+8); }

  /*!
   * \brief Set the value of the solution in the previous BGS subiteration.
   */
  inline void Set_BGSSolution_k(Idx_t iPoint) final {
    for (Idx_t iVar = 0; iVar < nVar; iVar++)
      Solution_BGS_k(iPoint,iVar) = Solution(iPoint,iVar);
  }

  /*!
   * \brief Get the value of the solution in the previous BGS subiteration.
   * \param[out] val_solution - solution in the previous BGS subiteration.
   */
  inline su2double Get_BGSSolution_k(Idx_t iPoint, Idx_t iDim) const final { return Solution_BGS_k(iPoint,iDim); }

};
