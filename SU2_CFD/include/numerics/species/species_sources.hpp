﻿/*!
 * \file species_sources.hpp
 * \brief Declarations of numerics classes for integration of source
 *        terms in species problems.
 * \author T. Kattmann
 * \version 7.5.1 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2023, SU2 Contributors (cf. AUTHORS.md)
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

#include "../scalar/scalar_sources.hpp"

/*!
 * \class CSourceBase_Species
 * \brief Intermediate source term class to allocate the internally
 *        stored residual and Jacobian. Not for stand alone use,
 *        just a helper to build more complicated classes.
 * \ingroup SourceDiscr
 * \author T. Kattmann
 */
class CSourceBase_Species : public CNumerics {
 protected:
  su2double* residual = nullptr;
  su2double** jacobian = nullptr;

 public:
  /*!
   * \brief Constructor of the class.
   * \param[in] val_nDim - Number of dimensions of the problem.
   * \param[in] val_nVar - Number of variables of the problem.
   * \param[in] config - Definition of the particular problem.
   */
  CSourceBase_Species(unsigned short val_nDim, unsigned short val_nVar, const CConfig* config);

  /*!
   * \brief Destructor of the class.
   */
  ~CSourceBase_Species();
};

/*!
 * \class CSourceAxisymmetric_Species
 * \brief Class for source term for solving axisymmetric problems.
 * \ingroup SourceDiscr
 * \author T. Kattmann
 */
template <class FlowIndices>
class CSourceAxisymmetric_Species : public CSourceBase_Species {
 protected:
  const FlowIndices idx;  /*!< \brief Object to manage the access to the flow primitives. */
  const bool implicit;
  const bool viscous;
  const bool turbulence;
  const bool incompressible;
  const su2double Sc_t;

 public:
  /*!
   * \brief Constructor of the class.
   * \param[in] val_nDim - Number of dimensions of the problem.
   * \param[in] val_nVar - Number of variables of the problem.
   * \param[in] config - Definition of the particular problem.
   */
  CSourceAxisymmetric_Species(unsigned short val_nDim, unsigned short val_nVar, const CConfig* config);

  /*!
   * \brief Residual of the axisymmetric source term.
   * \param[in] config - Definition of the particular problem.
   * \return Lightweight const-view of residual and Jacobian.
   */
  ResidualType<> ComputeResidual(const CConfig* config) override;

};

/*!
 * \class CSourcePieceWise_transportedScalar_general
 * \brief Class for integrating the source terms of the transported scalar turbulence model equations.
 * \ingroup SourceDiscr
 * \author N. Beishuizen
 */
class CSourcePieceWise_transportedScalar_general final : public CNumerics {

private:
  su2double *Residual = nullptr;
  su2double **Jacobian_i = nullptr;
  //bool flamelet;
  su2double *scalar_sources = nullptr;

  //su2double Sc_t;

  //su2double source_pv;

  bool incompressible;
  //bool viscous;
  //bool axisymmetric;
  //bool implicit;
  //bool inc_rans;

public:
  /*!
   * \brief Constructor of the class.
   * \param[in] val_nDim - Number of dimensions of the problem.
   * \param[in] val_nVar - Number of variables of the problem.
   * \param[in] config - Definition of the particular problem.
   */
  CSourcePieceWise_transportedScalar_general(unsigned short val_nDim, unsigned short val_nVar, const CConfig* config);

  /*!
   * \brief Residual for source term integration.
   * \param[in] config - Definition of the particular problem.
   * \return A lightweight const-view (read-only) of the residual/flux and Jacobians.
   */
  ResidualType<> ComputeResidual(const CConfig* config) override;

  /*!
   * \brief Set the value of the progress variable source term for the flamelet model.
   * \param[in] val_sourcepv_i - Value of the source term at point i.
   * \param[in] val_sourcepv_j - Value of the source term at point j.
   */
  inline void SetScalarSources(su2double *val_scalar_sources) override {
    for (auto i_var=0u; i_var < nVar; i_var++)
      scalar_sources[i_var] = val_scalar_sources[i_var];
  }

  /*!
   * \brief Destructor of the class.
   */
  ~CSourcePieceWise_transportedScalar_general(void) override;

};
