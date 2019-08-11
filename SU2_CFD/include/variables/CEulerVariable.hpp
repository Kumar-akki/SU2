/*!
 * \file CEulerVariable.hpp
 * \brief Class for defining the variables of the compressible Euler solver.
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
 * \class CEulerVariable
 * \brief Class for defining the variables of the compressible Euler solver.
 * \ingroup Euler_Equations
 * \author F. Palacios, T. Economon
 */
class CEulerVariable : public CVariable {
protected:
  Vec_t Velocity2;     /*!< \brief Square of the velocity vector. */
  Mat_t HB_Source;     /*!< \brief harmonic balance source term. */
  Vec_t Precond_Beta;  /*!< \brief Low Mach number preconditioner value, Beta. */
  Mat_t WindGust;      /*! < \brief Wind gust value */
  Mat_t WindGustDer;   /*! < \brief Wind gust derivatives value */

  /*--- Primitive variable definition ---*/
  Mat_t Primitive;  /*!< \brief Primitive variables (T, vx, vy, vz, P, rho, h, c) in compressible flows. */
  VectorOfMatrix Gradient_Primitive;  /*!< \brief Gradient of the primitive variables (T, vx, vy, vz, P, rho). */
  Mat_t Limiter_Primitive;    /*!< \brief Limiter of the primitive variables (T, vx, vy, vz, P, rho). */

  /*--- Secondary variable definition ---*/
  Mat_t Secondary;              /*!< \brief Primitive variables (T, vx, vy, vz, P, rho, h, c) in compressible flows. */
//  VectorOfMatrix Gradient_Secondary;  /*!< \brief Gradient of the primitive variables (T, vx, vy, vz, P, rho). */
//  Mat_t Limiter_Secondary;      /*!< \brief Limiter of the primitive variables (T, vx, vy, vz, P, rho). */

  Mat_t Solution_New;     /*!< \brief New solution container for Classical RK4. */
  Mat_t Solution_BGS_k;   /*!< \brief Old solution container for BGS iterations. */

public:
  /*!
   * \brief Constructor of the class.
   * \param[in] density - Value of the flow density (initialization value).
   * \param[in] velocity - Value of the flow velocity (initialization value).
   * \param[in] energy - Value of the flow energy (initialization value).
   * \param[in] npoint - Number of points/nodes/vertices in the domain.
   * \param[in] ndim - Number of dimensions of the problem.
   * \param[in] nvar - Number of variables of the problem.
   * \param[in] config - Definition of the particular problem.
   */
  CEulerVariable(su2double density, const su2double *velocity, su2double energy,
                 Idx_t npoint, Idx_t ndim, Idx_t nvar, CConfig *config);

  /*!
   * \brief Destructor of the class.
   */
  virtual ~CEulerVariable() = default;

  /*!
   * \brief Get the new solution of the problem (Classical RK4).
   * \param[in] iVar - Index of the variable.
   * \return Pointer to the old solution vector.
   */
  inline su2double GetSolution_New(Idx_t iPoint, Idx_t iVar) const final { return Solution_New(iPoint,iVar); }

  /*!
   * \brief Set the new solution container for Classical RK4.
   */
  void SetSolution_New() final;

  /*!
   * \brief Add a value to the new solution container for Classical RK4.
   * \param[in] iVar - Number of the variable.
   * \param[in] val_solution - Value that we want to add to the solution.
   */
  inline void AddSolution_New(Idx_t iPoint, Idx_t iVar, su2double val_solution) final {
    Solution_New(iPoint,iVar) += val_solution;
  }

  /*!
   * \brief Set to zero the gradient of the primitive variables.
   */
  void SetGradient_PrimitiveZero() final;

  /*!
   * \brief Add <i>value</i> to the gradient of the primitive variables.
   * \param[in] iVar - Index of the variable.
   * \param[in] iDim - Index of the dimension.
   * \param[in] value - Value to add to the gradient of the primitive variables.
   */
  inline void AddGradient_Primitive(Idx_t iPoint, Idx_t iVar, Idx_t iDim, su2double value) final {
    Gradient_Primitive(iPoint,iVar,iDim) += value;
  }

  /*!
   * \brief Subtract <i>value</i> to the gradient of the primitive variables.
   * \param[in] iVar - Index of the variable.
   * \param[in] iDim - Index of the dimension.
   * \param[in] value - Value to subtract to the gradient of the primitive variables.
   */
  inline void SubtractGradient_Primitive(Idx_t iPoint, Idx_t iVar, Idx_t iDim, su2double value) final {
    Gradient_Primitive(iPoint,iVar,iDim) -= value;
  }

  /*!
   * \brief Get the value of the primitive variables gradient.
   * \param[in] iVar - Index of the variable.
   * \param[in] iDim - Index of the dimension.
   * \return Value of the primitive variables gradient.
   */
  inline su2double GetGradient_Primitive(Idx_t iPoint, Idx_t iVar, Idx_t iDim) const final {
    return Gradient_Primitive(iPoint,iVar,iDim);
  }

  /*!
   * \brief Get the value of the primitive variables gradient.
   * \param[in] iVar - Index of the variable.
   * \return Value of the primitive variables gradient.
   */
  inline su2double GetLimiter_Primitive(Idx_t iPoint, Idx_t iVar) const final {return Limiter_Primitive(iPoint,iVar); }

  /*!
   * \brief Set the gradient of the primitive variables.
   * \param[in] iVar - Index of the variable.
   * \param[in] iDim - Index of the dimension.
   * \param[in] value - Value of the gradient.
   */
  inline void SetGradient_Primitive(Idx_t iPoint, Idx_t iVar, Idx_t iDim, su2double value) final {
    Gradient_Primitive(iPoint,iVar,iDim) = value;
  }

  /*!
   * \brief Set the gradient of the primitive variables.
   * \param[in] iVar - Index of the variable.
   * \param[in] value - Value of the gradient.
   */
  inline void SetLimiter_Primitive(Idx_t iPoint, Idx_t iVar, su2double value) final {
    Limiter_Primitive(iPoint,iVar) = value;
  }

  /*!
   * \brief Get the value of the primitive variables gradient.
   * \return Value of the primitive variables gradient.
   */
  inline su2double **GetGradient_Primitive(Idx_t iPoint) final { return Gradient_Primitive[iPoint]; }

  /*!
   * \brief Get the value of the primitive variables gradient.
   * \return Value of the primitive variables gradient.
   */
  inline su2double *GetLimiter_Primitive(Idx_t iPoint) final { return Limiter_Primitive[iPoint]; }

//  /*!
//   * \brief Set to zero the gradient of the primitive variables.
//   */
//  void SetGradient_SecondaryZero(Idx_t iPoint, Idx_t val_secondaryvar) final;
//
//  /*!
//   * \brief Add <i>value</i> to the gradient of the primitive variables.
//   * \param[in] iVar - Index of the variable.
//   * \param[in] iDim - Index of the dimension.
//   * \param[in] value - Value to add to the gradient of the primitive variables.
//   */
//  inline void AddGradient_Secondary(Idx_t iPoint, Idx_t iVar, Idx_t iDim, su2double value) final {
//    Gradient_Secondary(iPoint,iVar,iDim) += value;
//  }
//
//  /*!
//   * \brief Subtract <i>value</i> to the gradient of the primitive variables.
//   * \param[in] iVar - Index of the variable.
//   * \param[in] iDim - Index of the dimension.
//   * \param[in] value - Value to subtract to the gradient of the primitive variables.
//   */
//  inline void SubtractGradient_Secondary(Idx_t iPoint, Idx_t iVar, Idx_t iDim, su2double value) final {
//    Gradient_Secondary(iPoint,iVar,iDim) -= value;
//  }
//
//  /*!
//   * \brief Get the value of the primitive variables gradient.
//   * \param[in] iVar - Index of the variable.
//   * \param[in] iDim - Index of the dimension.
//   * \return Value of the primitive variables gradient.
//   */
//  inline su2double GetGradient_Secondary(Idx_t iPoint, Idx_t iVar, Idx_t iDim) const final {
//    return Gradient_Secondary(iPoint,iVar,iDim);
//  }
//
//  /*!
//   * \brief Get the value of the primitive variables gradient.
//   * \param[in] iVar - Index of the variable.
//   * \param[in] iDim - Index of the dimension.
//   * \return Value of the primitive variables gradient.
//   */
//  inline su2double GetLimiter_Secondary(Idx_t iPoint, Idx_t iVar) const final { return Limiter_Secondary(iPoint,iVar); }
//
//  /*!
//   * \brief Set the gradient of the primitive variables.
//   * \param[in] iVar - Index of the variable.
//   * \param[in] iDim - Index of the dimension.
//   * \param[in] value - Value of the gradient.
//   */
//  inline void SetGradient_Secondary(Idx_t iPoint, Idx_t iVar, Idx_t iDim, su2double value) final {
//    Gradient_Secondary(iPoint,iVar,iDim) = value;
//  }
//
//  /*!
//   * \brief Set the gradient of the primitive variables.
//   * \param[in] iVar - Index of the variable.
//   * \param[in] iDim - Index of the dimension.
//   * \param[in] value - Value of the gradient.
//   */
//  inline void SetLimiter_Secondary(Idx_t iPoint, Idx_t iVar, su2double value) final {
//    Limiter_Secondary(iPoint,iVar) = value;
//  }
//
//  /*!
//   * \brief Get the value of the primitive variables gradient.
//   * \return Value of the primitive variables gradient.
//   */
//  inline su2double **GetGradient_Secondary(Idx_t iPoint) final {return Gradient_Secondary[iPoint]; }
//
//  /*!
//   * \brief Get the value of the primitive variables gradient.
//   * \return Value of the primitive variables gradient.
//   */
//  inline su2double *GetLimiter_Secondary(Idx_t iPoint) final {return Limiter_Secondary[iPoint]; }

  /*!
   * \brief A virtual member.
   */
  inline void SetdPdrho_e(Idx_t iPoint, su2double dPdrho_e) final { Secondary(iPoint,0) = dPdrho_e;}

  /*!
   * \brief A virtual member.
   */
  inline void SetdPde_rho(Idx_t iPoint, su2double dPde_rho) final { Secondary(iPoint,1) = dPde_rho;}

  /*!
   * \brief Set the value of the pressure.
   */
  inline bool SetPressure(Idx_t iPoint, su2double pressure) final {
    Primitive(iPoint,nDim+1) = pressure;
    return pressure <= 0.0;
  }

  /*!
   * \brief Set the value of the speed of the sound.
   * \param[in] soundspeed2 - Value of soundspeed^2.
   */
  bool SetSoundSpeed(Idx_t iPoint, su2double soundspeed2) final {
    su2double radical = soundspeed2;
    if (radical < 0.0) return true;
    else {
      Primitive(iPoint,nDim+4) = sqrt(radical);
      return false;
    }
  }

  /*!
   * \brief Set the value of the enthalpy.
   */
  inline void SetEnthalpy(Idx_t iPoint) final {
    Primitive(iPoint,nDim+3) = (Solution(iPoint,nVar-1) + Primitive(iPoint,nDim+1)) / Solution(iPoint,0);
  }

  /*!
   * \brief Set all the primitive variables for compressible flows.
   */
  bool SetPrimVar(Idx_t iPoint, CFluidModel *FluidModel) final;

  /*!
   * \brief A virtual member.
   */
  void SetSecondaryVar(Idx_t iPoint, CFluidModel *FluidModel);

  /*!
   * \brief Get the primitive variables.
   * \param[in] iVar - Index of the variable.
   * \return Value of the primitive variable for the index <i>iVar</i>.
   */
  inline su2double GetPrimitive(Idx_t iPoint, Idx_t iVar) const final { return Primitive(iPoint,iVar); }

  /*!
   * \brief Set the value of the primitive variables.
   * \param[in] iVar - Index of the variable.
   * \param[in] iVar - Index of the variable.
   * \return Set the value of the primitive variable for the index <i>iVar</i>.
   */
  inline void SetPrimitive(Idx_t iPoint, Idx_t iVar, su2double val_prim) final { Primitive(iPoint,iVar) = val_prim; }

  /*!
   * \brief Set the value of the primitive variables.
   * \param[in] val_prim - Primitive variables.
   * \return Set the value of the primitive variable for the index <i>iVar</i>.
   */
  inline void SetPrimitive(Idx_t iPoint, const su2double *val_prim) final {
    for (Idx_t iVar = 0; iVar < nPrimVar; iVar++)
      Primitive(iPoint,iVar) = val_prim[iVar];
  }

  /*!
   * \brief Get the primitive variables of the problem.
   * \return Pointer to the primitive variable vector.
   */
  inline su2double *GetPrimitive(Idx_t iPoint) final {return Primitive[iPoint]; }

  /*!
   * \brief Get the primitive variables.
   * \param[in] iVar - Index of the variable.
   * \return Value of the primitive variable for the index <i>iVar</i>.
   */
  inline su2double GetSecondary(Idx_t iPoint, Idx_t iVar) const final {return Secondary(iPoint,iVar); }

  /*!
   * \brief Set the value of the primitive variables.
   * \param[in] iVar - Index of the variable.
   * \param[in] iVar - Index of the variable.
   * \return Set the value of the primitive variable for the index <i>iVar</i>.
   */
  inline void SetSecondary(Idx_t iPoint, Idx_t iVar, su2double val_secondary) final {Secondary(iPoint,iVar) = val_secondary; }

  /*!
   * \brief Set the value of the primitive variables.
   * \param[in] val_prim - Primitive variables.
   * \return Set the value of the primitive variable for the index <i>iVar</i>.
   */
  inline void SetSecondary(Idx_t iPoint, const su2double *val_secondary) final {
    for (Idx_t iVar = 0; iVar < nSecondaryVar; iVar++)
      Secondary(iPoint,iVar) = val_secondary[iVar];
  }

  /*!
   * \brief Get the primitive variables of the problem.
   * \return Pointer to the primitive variable vector.
   */
  inline su2double *GetSecondary(Idx_t iPoint) final { return Secondary[iPoint]; }

  /*!
   * \brief Set the value of the density for the incompressible flows.
   */
  inline bool SetDensity(Idx_t iPoint) final {
    Primitive(iPoint,nDim+2) = Solution(iPoint,0);
    return Primitive(iPoint,nDim+2) <= 0.0;
  }

  /*!
   * \brief Set the value of the temperature.
   * \param[in] temperature - how agitated the particles are :)
   */
  inline bool SetTemperature(Idx_t iPoint, su2double temperature) final {
    Primitive(iPoint,0) = temperature;
    return temperature <= 0.0;
  }

  /*!
   * \brief Get the norm 2 of the velocity.
   * \return Norm 2 of the velocity vector.
   */
  inline su2double GetVelocity2(Idx_t iPoint) const final { return Velocity2(iPoint); }

  /*!
   * \brief Get the flow pressure.
   * \return Value of the flow pressure.
   */
  inline su2double GetPressure(Idx_t iPoint) const final { return Primitive(iPoint,nDim+1); }

  /*!
   * \brief Get the speed of the sound.
   * \return Value of speed of the sound.
   */
  inline su2double GetSoundSpeed(Idx_t iPoint) const final { return Primitive(iPoint,nDim+4); }

  /*!
   * \brief Get the enthalpy of the flow.
   * \return Value of the enthalpy of the flow.
   */
  inline su2double GetEnthalpy(Idx_t iPoint) const final { return Primitive(iPoint,nDim+3); }

  /*!
   * \brief Get the density of the flow.
   * \return Value of the density of the flow.
   */
  inline su2double GetDensity(Idx_t iPoint) const final { return Solution(iPoint,0); }

  /*!
   * \brief Get the energy of the flow.
   * \return Value of the energy of the flow.
   */
  inline su2double GetEnergy(Idx_t iPoint) const final { return Solution(iPoint,nVar-1)/Solution(iPoint,0); }

  /*!
   * \brief Get the temperature of the flow.
   * \return Value of the temperature of the flow.
   */
  inline su2double GetTemperature(Idx_t iPoint) const final { return Primitive(iPoint,0); }

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
   * \brief Set the velocity vector from the solution.
   * \param[in] val_velocity - Pointer to the velocity.
   */
  inline void SetVelocity(Idx_t iPoint) final {
    Velocity2(iPoint) = 0.0;
    for (Idx_t iDim = 0; iDim < nDim; iDim++) {
      Primitive(iPoint,iDim+1) = Solution(iPoint,iDim+1) / Solution(iPoint,0);
      Velocity2(iPoint) += pow(Primitive(iPoint,iDim+1),2);
    }
  }

  /*!
   * \brief Set the velocity vector from the old solution.
   * \param[in] val_velocity - Pointer to the velocity.
   */
  inline void SetVelocity_Old(Idx_t iPoint, const su2double *val_velocity) final {
    for (Idx_t iDim = 0; iDim < nDim; iDim++)
      Solution_Old(iPoint,iDim+1) = val_velocity[iDim]*Solution(iPoint,0);
  }

  /*!
   * \brief Set the harmonic balance source term.
   * \param[in] iVar - Index of the variable.
   * \param[in] val_solution - Value of the harmonic balance source term. for the index <i>iVar</i>.
   */
  inline void SetHarmonicBalance_Source(Idx_t iPoint, Idx_t iVar, su2double val_source) final {
    HB_Source(iPoint,iVar) = val_source;
  }

  /*!
   * \brief Get the harmonic balance source term.
   * \param[in] iVar - Index of the variable.
   * \return Value of the harmonic balance source term for the index <i>iVar</i>.
   */
  inline su2double GetHarmonicBalance_Source(Idx_t iPoint, Idx_t iVar) const final { return HB_Source(iPoint,iVar); }

//  /*!
//   * \brief Get the value of the preconditioner Beta.
//   * \return Value of the low Mach preconditioner variable Beta
//   */
//  inline su2double GetPreconditioner_Beta(Idx_t iPoint) const final { return Precond_Beta(iPoint); }

  /*!
   * \brief Set the value of the preconditioner Beta.
   * \param[in] Value of the low Mach preconditioner variable Beta
   */
  inline void SetPreconditioner_Beta(Idx_t iPoint, su2double val_Beta) final { Precond_Beta(iPoint) = val_Beta; }

  /*!
   * \brief Get the value of the wind gust
   * \return Value of the wind gust
   */
  inline su2double* GetWindGust(Idx_t iPoint) final { return WindGust[iPoint]; }

  /*!
   * \brief Set the value of the wind gust
   * \param[in] Value of the wind gust
   */
  inline void SetWindGust(Idx_t iPoint, const su2double* val_WindGust) final {
    for (Idx_t iDim = 0; iDim < nDim; iDim++)
      WindGust(iPoint,iDim) = val_WindGust[iDim];
  }

  /*!
   * \brief Get the value of the derivatives of the wind gust
   * \return Value of the derivatives of the wind gust
   */
  inline su2double* GetWindGustDer(Idx_t iPoint) final { return WindGustDer[iPoint]; }

  /*!
   * \brief Set the value of the derivatives of the wind gust
   * \param[in] Value of the derivatives of the wind gust
   */
  inline void SetWindGustDer(Idx_t iPoint, const su2double* val_WindGustDer) final {
    for (Idx_t iDim = 0; iDim < nDim+1; iDim++)
      WindGustDer(iPoint,iDim) = val_WindGustDer[iDim];
  }

  /*!
   * \brief Set the value of the solution in the previous BGS subiteration.
   */
  void Set_BGSSolution_k() final;

  /*!
   * \brief Get the value of the solution in the previous BGS subiteration.
   * \param[out] val_solution - solution in the previous BGS subiteration.
   */
  inline su2double Get_BGSSolution_k(Idx_t iPoint, Idx_t iVar) const final { return Solution_BGS_k(iPoint,iVar); }
};
