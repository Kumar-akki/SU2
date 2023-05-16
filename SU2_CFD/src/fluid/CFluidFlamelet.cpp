
/*!
 * \file CfluidFlamelet.cpp
 * \brief Main subroutines of CFluidFlamelet class
 * \author D. Mayer, T. Economon, N. Beishuizen
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

#include "../include/fluid/CFluidFlamelet.hpp"
#include "../../../Common/include/containers/CLookUpTable.hpp"

CFluidFlamelet::CFluidFlamelet(CConfig* config, su2double value_pressure_operating) : CFluidModel() {
#ifdef HAVE_MPI
  SU2_MPI::Comm_rank(SU2_MPI::GetComm(),&rank);
#endif

  /* -- number of auxiliary species transport equations, e.g. 1=CO, 2=NOx  --- */
  n_user_scalars = config->GetNUserScalars();
  n_control_vars = config->GetNControlVars();
  n_scalars = config->GetNScalars();

  if (rank == MASTER_NODE) {
    cout << "Number of scalars:           " << n_scalars << endl;
    cout << "Number of user scalars:      " << n_user_scalars << endl;
    cout << "Number of control variables: " << n_control_vars << endl;
  }

  if (rank == MASTER_NODE) {
    cout << "*****************************************" << endl;
    cout << "***   initializing the lookup table   ***" << endl;
    cout << "*****************************************" << endl;
  }

  table_scalar_names.resize(n_scalars);
  table_scalar_names[I_ENTH] = "EnthalpyTot";
  table_scalar_names[I_PROGVAR] = "ProgressVariable";
  /*--- auxiliary species transport equations---*/
  for (size_t i_aux = 0; i_aux < n_user_scalars; i_aux++) {
    table_scalar_names[n_control_vars + i_aux] = config->GetUserScalarName(i_aux);
  }

  config->SetLUTScalarNames(table_scalar_names);

  /*--- we currently only need 1 source term from the LUT for the progress variable
        and each auxiliary equations needs 2 source terms ---*/
  n_table_sources = 1 + 2 * n_user_scalars;

  table_source_names.resize(n_table_sources);
  table_sources.resize(n_table_sources);
  table_source_names[I_SRC_TOT_PROGVAR] = "ProdRateTot_PV";
  /*--- No source term for enthalpy ---*/

  /*--- For the auxiliary equations, we use a positive (production) and a negative (consumption) term:
        S_tot = S_PROD + S_CONS * Y ---*/

  for (size_t i_aux = 0; i_aux < n_user_scalars; i_aux++) {
    /*--- Order of the source terms: S_prod_1, S_cons_1, S_prod_2, S_cons_2, ...---*/
    table_source_names[1 + 2 * i_aux] = config->GetUserSourceName(2 * i_aux);
    table_source_names[1 + 2 * i_aux + 1] = config->GetUserSourceName(2 * i_aux + 1);
  }

  config->SetLUTSourceNames(table_source_names);

  look_up_table = new CLookUpTable(config->GetFileNameLUT(), table_scalar_names[I_PROGVAR], table_scalar_names[I_ENTH]);

  n_lookups = config->GetNLookups();
  table_lookup_names.resize(n_lookups);
  for (int i_lookup = 0; i_lookup < n_lookups; ++i_lookup) {
    table_lookup_names[i_lookup] = config->GetLUTLookupName(i_lookup);
  }

  source_scalar.resize(n_scalars);

  Pressure = value_pressure_operating;

  PreprocessLookUp();
}

CFluidFlamelet::~CFluidFlamelet() {
  delete look_up_table;
}

/*--- set the source terms for the transport equations ---*/
unsigned long CFluidFlamelet::SetScalarSources(const su2double* val_scalars) {
  table_sources[0] = 0.0;

  /*--- value for the progress variable and enthalpy ---*/
  su2double enth = val_scalars[I_ENTH];
  su2double prog = val_scalars[I_PROGVAR];

  /*--- perform table lookup ---*/
  unsigned long exit_code = look_up_table->LookUp_XY(varnames_Sources, val_vars_Sources, prog, enth);

  /*--- The source term for progress variable is always positive, we clip from below to makes sure. --- */
  source_scalar[I_PROGVAR] = max(EPS, table_sources[I_SRC_TOT_PROGVAR]);
  source_scalar[I_ENTH] = 0.0;

  /*--- Source term for the auxiliary species transport equations ---*/
  for (size_t i_aux = 0; i_aux < n_user_scalars; i_aux++) {
    /*--- The source term for the auxiliary equations consists of a production term and a consumption term:
          S_TOT = S_PROD + S_CONS * Y ---*/
    su2double y_aux = val_scalars[n_control_vars + i_aux];
    su2double source_prod = table_sources[1 + 2 * i_aux];
    su2double source_cons = table_sources[1 + 2 * i_aux + 1];
    source_scalar[n_control_vars + i_aux] = source_prod + source_cons * y_aux;
  }

  return exit_code;
}

void CFluidFlamelet::SetTDState_T(su2double val_temperature, const su2double* val_scalars) {
  su2double val_enth = val_scalars[I_ENTH];
  su2double val_prog = val_scalars[I_PROGVAR];

  /*--- Add all quantities and their names to the look up vectors. ---*/
  look_up_table->LookUp_XY(varnames_TD, val_vars_TD, val_prog, val_enth);

  /*--- Compute Cv from Cp and molar weight of the mixture (ideal gas). ---*/
  Cv = Cp - UNIVERSAL_GAS_CONSTANT / molar_weight;
}

/* --- Total enthalpy is the transported variable, but we usually have temperature as a boundary condition,
       so we do a reverse lookup */
unsigned long CFluidFlamelet::GetEnthFromTemp(su2double& val_enth, const su2double val_prog, const su2double val_temp,
                                              const su2double initial_value) {

  /*--- convergence criterion for temperature in [K], high accuracy needed for restarts. ---*/
  su2double delta_temp_final = 0.001;
  su2double enth_iter = initial_value;
  su2double delta_enth;
  su2double delta_temp_iter = 1e10;
  unsigned long exit_code = 0;
  const int counter_limit = 1000;

  int counter = 0;
  while ((abs(delta_temp_iter) > delta_temp_final) && (counter++ < counter_limit)) {
    /*--- Add all quantities and their names to the look up vectors. ---*/
    look_up_table->LookUp_XY(varnames_TD, val_vars_TD, val_prog, enth_iter);

    delta_temp_iter = val_temp - Temperature;

    delta_enth = Cp * delta_temp_iter;

    enth_iter += delta_enth;
  }

  val_enth = enth_iter;

  if (counter >= counter_limit) {
    exit_code = 1;
  }

  return exit_code;
}

void CFluidFlamelet::PreprocessLookUp() {
  /*--- Set lookup names and variables for all relevant lookup processes in the fluid model. ---*/

  /*--- Thermodynamic state variables and names. ---*/
  varnames_TD.resize(7);
  val_vars_TD.resize(7);

  /*--- The string in varnames_TD as it appears in the LUT file. ---*/
  varnames_TD[0] = "Temperature";
  val_vars_TD[0] = &Temperature;
  varnames_TD[1] = "Density";
  val_vars_TD[1] = &Density;
  varnames_TD[2] = "Cp";
  val_vars_TD[2] = &Cp;
  varnames_TD[3] = "ViscosityDyn";
  val_vars_TD[3] = &Mu;
  varnames_TD[4] = "Conductivity";
  val_vars_TD[4] = &Kt;
  varnames_TD[5] = "DiffusionCoefficient";
  val_vars_TD[5] = &mass_diffusivity;
  varnames_TD[6] = "MolarWeightMix";
  val_vars_TD[6] = &molar_weight;

  /*--- Source term variables ---*/
  varnames_Sources.resize(n_table_sources);
  val_vars_Sources.resize(n_table_sources);

  for (size_t iSource = 0; iSource < n_table_sources; iSource++) {
    varnames_Sources[iSource] = table_source_names[iSource];
    val_vars_Sources[iSource] = &table_sources[iSource];
  }

}
