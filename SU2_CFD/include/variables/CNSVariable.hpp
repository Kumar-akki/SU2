/*!
 * \file CNSVariable.hpp
 * \brief Class for defining the variables of the compressible Navier-Stokes solver.
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

#include "CEulerVariable.hpp"

/*!
 * \class CNSVariable
 * \brief Class for defining the variables of the compressible Navier-Stokes solver.
 * \ingroup Navier_Stokes_Equations
 * \author F. Palacios, T. Economon
 */
class CNSVariable final : public CEulerVariable {
private:
  su2double inv_TimeScale;   /*!< \brief Inverse of the reference time scale. */

  Mat_t Vorticity;       /*!< \brief Vorticity of the fluid. */
  Vec_t StrainMag;       /*!< \brief Magnitude of rate of strain tensor. */
  Vec_t Tau_Wall;        /*!< \brief Magnitude of the wall shear stress from a wall function. */
  Vec_t DES_LengthScale; /*!< \brief DES Length Scale. */
  Vec_t Roe_Dissipation; /*!< \brief Roe low dissipation coefficient. */
  Vec_t Vortex_Tilting;  /*!< \brief Value of the vortex tilting variable for DES length scale computation. */

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
  CNSVariable(su2double density, const su2double *velocity, su2double energy,
              Idx_t npoint, Idx_t ndim, Idx_t nvar, CConfig *config);

  /*!
   * \brief Destructor of the class.
   */
  ~CNSVariable() = default;

  /*!
   * \brief Set the laminar viscosity.
   */
  inline void SetLaminarViscosity(Idx_t iPoint, su2double laminarViscosity) override {
    Primitive(iPoint,nDim+5) = laminarViscosity;
  }

  /*!
   * \brief Set the laminar viscosity.
   */
  inline void SetThermalConductivity(Idx_t iPoint, su2double thermalConductivity) override {
    Primitive(iPoint,nDim+7) = thermalConductivity;
  }

  /*!
   * \brief Set the specific heat Cp.
   */
  inline void SetSpecificHeatCp(Idx_t iPoint, su2double val_Cp) override { Primitive(iPoint,nDim+8) = val_Cp; }

  /*!
   * \brief Set the vorticity value.
   */
  bool SetVorticity(Idx_t iPoint) override;

  /*!
   * \brief Set the rate of strain magnitude.
   */
  bool SetStrainMag(Idx_t iPoint) override;

  /*!
   * \overload
   * \param[in] eddy_visc - Value of the eddy viscosity.
   */
  inline void SetEddyViscosity(Idx_t iPoint, su2double eddy_visc) override { Primitive(iPoint,nDim+6) = eddy_visc; }

  /*!
   * \brief Get the laminar viscosity of the flow.
   * \return Value of the laminar viscosity of the flow.
   */
  inline su2double GetLaminarViscosity(Idx_t iPoint) const override { return Primitive(iPoint,nDim+5); }

  /*!
   * \brief Get the thermal conductivity of the flow.
   * \return Value of the laminar viscosity of the flow.
   */
  inline su2double GetThermalConductivity(Idx_t iPoint) const override { return Primitive(iPoint,nDim+7); }

  /*!
   * \brief Get the eddy viscosity of the flow.
   * \return The eddy viscosity of the flow.
   */
  inline su2double GetEddyViscosity(Idx_t iPoint) const override { return Primitive(iPoint,nDim+6); }

  /*!
   * \brief Get the specific heat at constant P of the flow.
   * \return Value of the specific heat at constant P  of the flow.
   */
  inline su2double GetSpecificHeatCp(Idx_t iPoint) const override { return Primitive(iPoint,nDim+8); }

  /*!
   * \brief Set the temperature at the wall
   */
  inline void SetWallTemperature(Idx_t iPoint, su2double temperature_wall) override {
    Primitive(iPoint,0) = temperature_wall;
  }

  /*!
   * \brief Get the value of the vorticity.
   * \param[in] val_dim - Index of the dimension.
   * \return Value of the vorticity.
   */
  inline su2double *GetVorticity(Idx_t iPoint) override { return Vorticity[iPoint]; }

  /*!
   * \brief Get the value of the magnitude of rate of strain.
   * \return Value of the rate of strain magnitude.
   */
  inline su2double GetStrainMag(Idx_t iPoint) const override { return StrainMag(iPoint); }

  /*!
   * \brief Set the derivative of temperature with respect to density (at constant internal energy).
   */
  inline void SetdTdrho_e(Idx_t iPoint, su2double dTdrho_e) override { Secondary(iPoint,2) = dTdrho_e;}

  /*!
   * \brief Set the derivative of temperature with respect to internal energy (at constant density).
   */
  inline void SetdTde_rho(Idx_t iPoint, su2double dTde_rho) override { Secondary(iPoint,3) = dTde_rho;}

  /*!
   * \brief Set the derivative of laminar viscosity with respect to density (at constant temperature).
   */
  inline void Setdmudrho_T(Idx_t iPoint, su2double dmudrho_T) override { Secondary(iPoint,4) = dmudrho_T;}

  /*!
   * \brief Set the derivative of laminar viscosity with respect to temperature (at constant density).
   */
  inline void SetdmudT_rho(Idx_t iPoint, su2double dmudT_rho) override { Secondary(iPoint,5) = dmudT_rho;}

  /*!
   * \brief Set the derivative of thermal conductivity with respect to density (at constant temperature).
   */
  inline void Setdktdrho_T(Idx_t iPoint, su2double dktdrho_T) override { Secondary(iPoint,6) = dktdrho_T;}

  /*!
   * \brief Set the derivative of thermal conductivity with respect to temperature (at constant density).
   */
  inline void SetdktdT_rho(Idx_t iPoint, su2double dktdT_rho) override { Secondary(iPoint,7) = dktdT_rho;}

  /*!
   * \brief Set all the primitive variables for compressible flows
   */
  bool SetPrimVar(Idx_t iPoint, su2double eddy_visc, su2double turb_ke, CFluidModel *FluidModel) override;
  using CVariable::SetPrimVar;

  /*!
   * \brief Set all the secondary variables (partial derivatives) for compressible flows
   */
  void SetSecondaryVar(Idx_t iPoint, CFluidModel *FluidModel) override;

  /*!
   * \brief Set the value of the wall shear stress computed by a wall function.
   */
  inline void SetTauWall(Idx_t iPoint, su2double val_tau_wall) override { Tau_Wall(iPoint) = val_tau_wall; }

  /*!
   * \brief Get the value of the wall shear stress computed by a wall function.
   * \return Value of the wall shear stress computed by a wall function.
   */
  inline su2double GetTauWall(Idx_t iPoint) const override { return Tau_Wall(iPoint); }

  /*!
   * \brief Get the DES length scale
   * \return Value of the DES length Scale.
   */
  inline su2double GetDES_LengthScale(Idx_t iPoint) const override { return DES_LengthScale(iPoint); }

  /*!
   * \brief Set the DES Length Scale.
   */
  inline void SetDES_LengthScale(Idx_t iPoint, su2double val_des_lengthscale) override {
    DES_LengthScale(iPoint) = val_des_lengthscale;
  }

  /*!
   * \brief Set the new solution for Roe Dissipation.
   * \param[in] val_delta - A scalar measure of the grid size
   * \param[in] val_const_DES - The DES constant (C_DES)
   */
  void SetRoe_Dissipation_NTS(Idx_t iPoint, su2double val_delta, su2double val_const_DES) override;

  /*!
   * \brief Set the new solution for Roe Dissipation.
   */
  void SetRoe_Dissipation_FD(Idx_t iPoint, su2double wall_distance) override;

  /*!
   * \brief Get the Roe Dissipation Coefficient.
   * \return Value of the Roe Dissipation.
   */
  inline su2double GetRoe_Dissipation(Idx_t iPoint) const override { return Roe_Dissipation(iPoint); }

  /*!
   * \brief Set the Roe Dissipation Coefficient.
   * \param[in] val_dissipation - Value of the Roe dissipation factor.
   */
  inline void SetRoe_Dissipation(Idx_t iPoint, su2double val_dissipation) override {
    Roe_Dissipation(iPoint) = val_dissipation;
  }

};
