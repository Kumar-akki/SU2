/*!
 * \file variable_structure.inl
 * \brief In-Line subroutines of the <i>variable_structure.hpp</i> file.
 * \author Aerospace Design Laboratory (Stanford University) <http://su2.stanford.edu>.
 * \version 2.0.8
 *
 * Stanford University Unstructured (SU2).
 * Copyright (C) 2012-2013 Aerospace Design Laboratory (ADL).
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

inline void CVariable::SetVelocityInc2(void) { }

inline void CVariable::SetPressureValue(double val_pressure) { }

inline void CVariable::SetStress(unsigned short iVar, unsigned short jVar, double val_stress) { }
  
inline double **CVariable::GetStress(void) { return 0; }
  
inline void CVariable::SetVonMises_Stress(double val_stress) { }
  
inline double CVariable::GetVonMises_Stress(void) { return 0; }

inline void CVariable::SetPressureInc(double val_pressure) { }

inline double CVariable::GetBetaInc2(void) { return 0; }

inline double CVariable::GetDiffLevelSet(void) { return 0; }

inline double CVariable::GetDensityInc(void) { return 0; }

inline double CVariable::GetMassFraction(unsigned short val_Species) { return 0; }

inline double CVariable::GetProjVelInc(double *val_vector) { return 0; }

inline void CVariable::SetSolution(unsigned short val_var, double val_solution) { Solution[val_var] = val_solution; }

inline void CVariable::SetUndivided_Laplacian(unsigned short val_var, double val_undivided_laplacian) { Undivided_Laplacian[val_var] = val_undivided_laplacian; }

inline void CVariable::SetAuxVar(double val_auxvar) { AuxVar = val_auxvar; }

inline void CVariable::SetSolution_Old(unsigned short val_var, double val_solution_old) { Solution_Old[val_var] = val_solution_old; }

inline void CVariable::SetLimiter(unsigned short val_var, double val_limiter) { Limiter[val_var] = val_limiter; }

inline void CVariable::SetLimiterPrimitive(unsigned short val_species, unsigned short val_var, double val_limiter) { }

inline double CVariable::GetLimiterPrimitive(unsigned short val_species, unsigned short val_var) { return 0.0; }

inline void CVariable::SetSolution_Max(unsigned short val_var, double val_solution) { Solution_Max[val_var] = val_solution; }

inline void CVariable::SetSolution_Min(unsigned short val_var, double val_solution) { Solution_Min[val_var] = val_solution; }

inline void CVariable::SetAuxVarGradient(unsigned short iDim, double val_gradient) { Grad_AuxVar[iDim] = val_gradient; }

inline double *CVariable::GetSolution(void) { return Solution; }

inline double *CVariable::GetSolution_Old(void) { return Solution_Old; }

inline double *CVariable::GetSolution_time_n(void) { return Solution_time_n; }

inline double *CVariable::GetSolution_time_n1(void) { return Solution_time_n1; }

inline double CVariable::GetAuxVar(void) { return AuxVar; }

inline double *CVariable::GetUndivided_Laplacian(void) { return Undivided_Laplacian; }

inline double CVariable::GetUndivided_Laplacian(unsigned short val_var) { return Undivided_Laplacian[val_var]; }

inline void CVariable::AddSolution(unsigned short val_var, double val_solution) {Solution[val_var] = Solution_Old[val_var] + val_solution; }

inline double CVariable::GetSolution(unsigned short val_var) { return Solution[val_var]; }

inline double CVariable::GetSolution_Old(unsigned short val_var) { return Solution_Old[val_var]; }

inline double *CVariable::GetResidual_Sum(void) { return Residual_Sum; }

inline double *CVariable::GetResidual_Old(void) { return Residual_Old; }

inline void CVariable::SetGradient(unsigned short val_var, unsigned short val_dim, double val_value) { Gradient[val_var][val_dim] = val_value; }

inline void CVariable::AddGradient(unsigned short val_var, unsigned short val_dim, double val_value) { Gradient[val_var][val_dim] += val_value; }

inline void CVariable::SubtractGradient(unsigned short val_var, unsigned short val_dim, double val_value) { Gradient[val_var][val_dim] -= val_value; }

inline void CVariable::AddAuxVarGradient(unsigned short val_dim, double val_value) { Grad_AuxVar[val_dim] += val_value; }

inline void CVariable::SubtractAuxVarGradient(unsigned short val_dim, double val_value) { Grad_AuxVar[val_dim] -= val_value; }

inline double CVariable::GetGradient(unsigned short val_var, unsigned short val_dim) { return Gradient[val_var][val_dim]; }

inline double CVariable::GetLimiter(unsigned short val_var) { return Limiter[val_var]; }

inline double CVariable::GetSolution_Max(unsigned short val_var) { return Solution_Max[val_var]; }

inline double CVariable::GetSolution_Min(unsigned short val_var) { return Solution_Min[val_var]; }

inline double CVariable::GetPreconditioner_Beta() { return 0; }

inline void CVariable::SetPreconditioner_Beta( double val_Beta) { }

inline double* CVariable::GetMagneticField() { return 0; }

inline void CVariable::SetMagneticField( double* val_B) {}

inline double* CVariable::GetWindGust() { return 0; }

inline void CVariable::SetWindGust( double* val_WindGust) {}

inline double* CVariable::GetWindGustDer() { return 0; }

inline void CVariable::SetWindGustDer( double* val_WindGustDer) {}

inline double **CVariable::GetGradient(void) { return Gradient; }

inline double *CVariable::GetLimiter(void) { return Limiter; }

inline double *CVariable::GetAuxVarGradient(void) { return Grad_AuxVar; }

inline double CVariable::GetAuxVarGradient(unsigned short val_dim) { return Grad_AuxVar[val_dim]; }

inline double *CVariable::GetResTruncError(void) { return Res_TruncError; }

inline void CVariable::SetDelta_Time(double val_delta_time) { Delta_Time = val_delta_time; }

inline void CVariable::SetDelta_Time(double val_delta_time, unsigned short iSpecies) {  }

inline double CVariable::GetDelta_Time(void) { return Delta_Time; }

inline double CVariable::GetDelta_Time(unsigned short iSpecies) { return 0;}

inline void CVariable::SetMax_Lambda(double val_max_lambda) { Max_Lambda = val_max_lambda; }

inline void CVariable::SetMax_Lambda_Inv(double val_max_lambda) { Max_Lambda_Inv = val_max_lambda; }

inline void CVariable::SetMax_Lambda_Inv(double val_max_lambda, unsigned short val_species) { }

inline void CVariable::SetMax_Lambda_Visc(double val_max_lambda) { Max_Lambda_Visc = val_max_lambda; }

inline void CVariable::SetMax_Lambda_Visc(double val_max_lambda, unsigned short val_species) { }

inline void CVariable::SetLambda(double val_lambda) { Lambda = val_lambda; }

inline void CVariable::SetLambda(double val_lambda, unsigned short iSpecies) {}

inline void CVariable::AddMax_Lambda(double val_max_lambda) { Max_Lambda += val_max_lambda; }

inline void CVariable::AddMax_Lambda_Inv(double val_max_lambda) { Max_Lambda_Inv += val_max_lambda; }

inline void CVariable::AddMax_Lambda_Visc(double val_max_lambda) { Max_Lambda_Visc += val_max_lambda; }

inline void CVariable::AddLambda(double val_lambda) { Lambda += val_lambda; }

inline void CVariable::AddLambda(double val_lambda, unsigned short iSpecies) {}

inline double CVariable::GetMax_Lambda(void) { return Max_Lambda; }

inline double CVariable::GetMax_Lambda_Inv(void) { return Max_Lambda_Inv; }

inline double CVariable::GetMax_Lambda_Visc(void) { return Max_Lambda_Visc; }

inline double CVariable::GetLambda(void) { return Lambda; }

inline double CVariable::GetLambda(unsigned short iSpecies) { return 0; }

inline double CVariable::GetSensor(void) { return Sensor; }

inline double CVariable::GetSensor(unsigned short iSpecies) { return 0;}

inline double CVariable::GetMax_Lambda_Inv(unsigned short iFluids) { return 0; }

inline double CVariable::GetMax_Lambda_Visc(unsigned short iFluids) { return 0; }

inline void CVariable::AddMax_Lambda_Inv(double val_max_lambda, unsigned short iSpecies) { }

inline void CVariable::AddMax_Lambda_Visc(double val_max_lambda, unsigned short iSpecies) { }

inline void CVariable::SetSensor(double val_sensor) { Sensor = val_sensor; }

inline void CVariable::SetSensor(double val_sensor, unsigned short val_iSpecies) {}

inline double CVariable::GetDensity(void) {	return 0; }

inline double CVariable::GetDensity(unsigned short val_iSpecies) {	return 0; }

inline double CVariable::GetEnergy(void) { return 0; }

inline double *CVariable::GetForceProj_Vector(void) { return NULL; }

inline double *CVariable::GetObjFuncSource(void) { return NULL; }

inline double *CVariable::GetIntBoundary_Jump(void) { return NULL; }

inline double CVariable::GetEddyViscosity(void) { return 0; }

inline void CVariable::SetGammaEff(void) { }

inline void CVariable::SetGammaSep(double gamma_sep) { }

inline double CVariable::GetIntermittency(void) { return 0; }

inline double CVariable::GetEnthalpy(void) { return 0; }

inline double CVariable::GetEnthalpy(unsigned short iSpecies) { return 0; }

inline double CVariable::GetPressure(unsigned short val_incomp) { return 0; }

inline double CVariable::GetPressure(void) { return 0; }

inline double CVariable::GetDeltaPressure(void) { return 0; }

inline double CVariable::GetProjVel(double *val_vector) { return 0; }

inline double CVariable::GetProjVel(double *val_vector, unsigned short val_species) { return 0; }

inline double CVariable::GetSoundSpeed(void) { return 0; }

inline double CVariable::GetSoundSpeed(unsigned short val_var) { return 0; }

inline double CVariable::GetTemperature(void) { return 0; }

inline double CVariable::GetTemperature_ve(void) { return 0; }

inline double CVariable::GetRhoCv_tr(void) { return 0; }

inline double CVariable::GetRhoCv_ve(void) { return 0; }

inline double CVariable::GetTemperature_tr(unsigned short val_iSpecies) { return 0; }

inline double CVariable::GetTemperature_vib(unsigned short val_iSpecies) { return 0; }

inline double CVariable::GetVelocity(unsigned short val_dim, unsigned short val_incomp) { return 0; }

inline double CVariable::GetVelocity(unsigned short val_dim) { return 0; }

inline double CVariable::GetVelocity2(void) { return 0; }

inline double CVariable::GetVelocity2(unsigned short val_species) { return 0;}

inline double CVariable::GetLaminarViscosity(void) { return 0; }

inline double CVariable::GetLaminarViscosityInc(void) { return 0; }

inline double CVariable::GetLaminarViscosity(unsigned short iSpecies) { return 0; }

inline double CVariable::GetThermalConductivity(unsigned short iSpecies) { return 0; }

inline double CVariable::GetThermalConductivity_vib(unsigned short iSpecies) { return 0; }

inline double CVariable::GetEddyViscosity(unsigned short iSpecies) { return 0; }

inline double CVariable::GetVorticity(unsigned short val_dim) { return 0; }

inline double CVariable::GetStrainMag(void) { return 0; }

inline void CVariable::SetForceProj_Vector(double *val_ForceProj_Vector) { }

inline void CVariable::SetObjFuncSource(double *val_ObjFuncSource) { }

inline void CVariable::SetIntBoundary_Jump(double *val_IntBoundary_Jump) { }

inline void CVariable::SetEddyViscosity(unsigned short val_Kind_Turb_Model, CVariable *TurbVariable) { }

inline void CVariable::SetEnthalpy(void) { }

inline bool CVariable::SetPrimVar_Compressible(double SharpEdge_Distance, bool check, CConfig *config) { return true; }

inline bool CVariable::SetPrimVar_Incompressible(double SharpEdge_Distance, bool check, CConfig *config) { return true; }

inline bool CVariable::SetPrimVar_FreeSurface(double SharpEdge_Distance, bool check, CConfig *config) { return true; }

inline bool CVariable::SetPrimVar_Compressible(CConfig *config) { return true; }

inline bool CVariable::SetPrimVar_Compressible(double turb_ke, CConfig *config) { return true; }

inline bool CVariable::SetPrimVar_Incompressible(double Density_Inf, CConfig *config) { return true; }

inline bool CVariable::SetPrimVar_FreeSurface(double Density_Inf, CConfig *config) { return true; }

inline bool CVariable::SetPrimVar_Incompressible(double Density_Inf, double Viscosity_Inf, double turb_ke, CConfig *config) { return true; }

inline bool CVariable::SetPrimVar_FreeSurface(double Density_Inf, double Viscosity_Inf, double turb_ke, CConfig *config) { return true; }

inline double CVariable::GetPrimVar(unsigned short val_var) { return 0; }

inline void CVariable::SetPrimVar(unsigned short val_var, double val_prim) { }

inline void CVariable::SetPrimVar(double *val_prim) { }

inline double *CVariable::GetPrimVar(void) { return NULL; }

inline double **CVariable::GetPrimVar_Plasma(void) { return NULL; }

inline double CVariable::GetPrimVar(unsigned short iSpecies, unsigned short iVar) { return 0; }

inline void CVariable::SetBetaInc2(double val_betainc2) { }

inline void CVariable::SetDensityInc(double val_densityinc) { }

inline void CVariable::SetPhi_Old(double *val_phi) { }

inline void CVariable::SetDiffLevelSet(double val_difflevelset) { }

inline bool CVariable::SetPressure(double Gamma) { return false; }

inline bool CVariable::SetPressure(CConfig *config) { return false; }

inline bool CVariable::SetPressure(double Gamma, double turb_ke) { return false; }

inline void CVariable::SetPressure() { }

inline void CVariable::SetdPdrhos(CConfig *config) { }

inline double *CVariable::GetdPdrhos() { return NULL; }

inline void CVariable::SetDensity() { }

inline void CVariable::SetDeltaPressure(double *val_velocity, double Gamma) { }

inline bool CVariable::SetSoundSpeed(CConfig *config) { return false; }

inline bool CVariable::SetSoundSpeed() { return false; }

inline bool CVariable::SetSoundSpeed(double Gamma) { return false; }

inline bool CVariable::SetTemperature(double Gas_Constant) { return false; }

inline bool CVariable::SetTemperature(CConfig *config) { return false; }

inline void CVariable::SetTemperature_tr(CConfig *config) { }

inline void CVariable::SetTemperature_vib(CConfig *config) { }

inline void CVariable::SetPrimVar(CConfig *config) { }

inline void CVariable::SetPrimVar(CConfig *config, double *Coord) { }

inline void CVariable::SetWallTemperature(double Temperature_Wall) { }

inline void CVariable::SetWallTemperature(double* Temperature_Wall) { }

inline void CVariable::SetThermalCoeff(CConfig *config) { }

inline void CVariable::SetVelocity(double *val_velocity, unsigned short val_incomp) { }

inline void CVariable::SetVelocity2(void) { }

inline void CVariable::SetVelocity_Old(double *val_velocity, unsigned short val_incomp) { }

inline void CVariable::SetVel_ResTruncError_Zero(unsigned short iSpecies) { }

inline void CVariable::SetLaminarViscosity() { }

inline void CVariable::SetLaminarViscosity(CConfig *config) { }

inline void CVariable::SetLaminarViscosity(double val_laminar_viscosity) { }

inline void CVariable::SetLaminarViscosity(double val_laminar_viscosity, unsigned short iSpecies) { }

inline void CVariable::SetLaminarViscosityInc(double val_laminar_viscosity_inc) { }

inline void CVariable::SetEddyViscosity(double val_eddy_viscosity) { }

inline void CVariable::SetVorticity(void) { }

inline void CVariable::SetStrainMag(void) { }

inline void CVariable::SetGradient_PrimitiveZero(unsigned short val_primvar) { }

inline void CVariable::AddGradient_Primitive(unsigned short val_var, unsigned short val_dim, double val_value) { }

inline void CVariable::AddGradient_Primitive(unsigned short val_species, unsigned short val_var, unsigned short val_dim, double val_value) { }

inline void CVariable::SubtractGradient_Primitive(unsigned short val_var, unsigned short val_dim, double val_value) { }

inline void CVariable::SubtractGradient_Primitive(unsigned short val_species, unsigned short val_var, unsigned short val_dim, double val_value) { }

inline double CVariable::GetGradient_Primitive(unsigned short val_var, unsigned short val_dim) { return 0; }

inline double CVariable::GetGradient_Primitive(unsigned short val_species, unsigned short val_var, unsigned short val_dim) { return 0; }

inline void CVariable::SetGradient_Primitive(unsigned short val_var, unsigned short val_dim, double val_value) { }

inline void CVariable::SetGradient_Primitive(unsigned short val_species, unsigned short val_var, unsigned short val_dim, double val_value) { }

inline double **CVariable::GetGradient_Primitive(void) { return NULL; }

inline double **CVariable::GetGradient_Primitive(unsigned short val_species) { return NULL; }

inline double ***CVariable::GetGradient_Primitive_Plasma(void) { return NULL; }

inline void CVariable::SetBlendingFunc(double val_viscosity, double val_dist, double val_density) { }

inline double CVariable::GetF1blending(void) { return 0; }

inline double CVariable::GetF2blending(void) { return 0; }

inline double CVariable::GetmuT() { return 0;}

inline void CVariable::SetmuT(double val_muT) { }

inline void CVariable::SetThickness_Noise(double val_thickness_noise) { }

inline void CVariable::SetLoading_Noise(double val_loading_noise) { }

inline void CVariable::SetQuadrupole_Noise(double val_quadrupole_noise) { }

inline double CVariable::GetThickness_Noise() { return 0;}

inline double CVariable::GetLoading_Noise() { return 0;}

inline double CVariable::GetQuadrupole_Noise() { return 0;}

inline double* CVariable::GetSolution_Direct() { return NULL; }

inline void CVariable::SetSolution_Direct(double *val_solution_direct) { }

inline void CVariable::SetChargeDensity(double positive_charge, double negative_charge) { }

inline void CVariable::SetPlasmaRhoUGradient(unsigned short iSpecies, double val_gradient, unsigned short iDim) { }

inline void CVariable::SetPlasmaTimeStep(double dt) {}

inline double* CVariable::GetChargeDensity() { return 0;}

inline double** CVariable::GetPlasmaRhoUGradient() { return 0;}

inline double CVariable::GetPlasmaTimeStep() { return 0;}

inline void CVariable::SetpoissonField(double* val_poissonField) { }

inline double* CVariable::GetpoissonField() { return 0;}

inline void CVariable::SetTimeSpectral_Source(unsigned short val_var, double val_source) { }

inline unsigned short CVariable::GetRhosIndex(void) { return 0; }

inline unsigned short CVariable::GetRhoIndex(void) { return 0; }

inline unsigned short CVariable::GetPIndex(void) { return 0; }

inline unsigned short CVariable::GetTIndex(void) { return 0; }

inline unsigned short CVariable::GetTveIndex(void) { return 0; }

inline unsigned short CVariable::GetVelIndex(void) { return 0; }

inline unsigned short CVariable::GetHIndex(void) { return 0; }

inline unsigned short CVariable::GetAIndex(void) { return 0; }

inline unsigned short CVariable::GetRhoCvtrIndex(void) { return 0; }

inline unsigned short CVariable::GetRhoCvveIndex(void) { return 0; }

inline double CVariable::GetTimeSpectral_Source(unsigned short val_var) { return 0; }

inline double CEulerVariable::GetDensity(void) { return Solution[0]; }

inline double CEulerVariable::GetDensityInc(void) { return Primitive[0]; }

inline double CEulerVariable::GetBetaInc2(void) { return Primitive[nDim+1]; }

inline double CEulerVariable::GetEnergy(void) { return Solution[nVar-1]/Solution[0]; };

inline double CEulerVariable::GetEnthalpy(void) { return Primitive[nDim+3]; }

inline double CEulerVariable::GetPressure(unsigned short val_incomp) {
double pressure;
   if (val_incomp == COMPRESSIBLE) pressure = Primitive[nDim+1]; 
   if ((val_incomp == INCOMPRESSIBLE) || (val_incomp == FREESURFACE)) pressure = Solution[0];
return pressure;
}

inline double CEulerVariable::GetSoundSpeed(void) { return Primitive[nDim+4]; }

inline double CEulerVariable::GetTemperature(void) { return Primitive[0]; }

inline double CEulerVariable::GetVelocity(unsigned short val_dim, unsigned short val_incomp) {
double velocity;
   if (val_incomp == COMPRESSIBLE) velocity = Solution[val_dim+1]/Solution[0]; 
   if ((val_incomp == INCOMPRESSIBLE) || (val_incomp == FREESURFACE)) velocity = Solution[val_dim+1]/Primitive[0];
return velocity;
}

inline double CEulerVariable::GetVelocity2(void) { return Velocity2; }

inline void CEulerVariable::SetEnthalpy(void) { Primitive[nDim+3] = (Solution[nVar-1] + Primitive[nDim+1]) / Solution[0]; }

inline void CEulerVariable::SetDensityInc(double val_density) { Primitive[0] = val_density; }

inline void CEulerVariable::SetBetaInc2(double val_betainc2) { Primitive[nDim+1] = val_betainc2; }

inline bool CEulerVariable::SetSoundSpeed(double Gamma) {
   double radical = Gamma*Primitive[nDim+1]/Solution[0];
   if (radical < 0.0) return true;
   else {
      Primitive[nDim+4] = sqrt(radical);
      return false;
   }
}

inline bool CEulerVariable::SetTemperature(double Gas_Constant) {
   Primitive[0] = Primitive[nDim+1] / ( Gas_Constant * Solution[0]);
   if (Primitive[0] > 0.0) return false;
   else return true;
}

inline double CEulerVariable::GetPrimVar(unsigned short val_var) { return Primitive[val_var]; }

inline void CEulerVariable::SetPrimVar(unsigned short val_var, double val_prim) { Primitive[val_var] = val_prim; }

inline void CEulerVariable::SetPrimVar(double *val_prim) {
   for (unsigned short iVar = 0; iVar < nPrimVar; iVar++) 
      Primitive[iVar] = val_prim[iVar]; 
}

inline double *CEulerVariable::GetPrimVar(void) { return Primitive; }

inline void CEulerVariable::SetVelocity(double *val_velocity, unsigned short val_incomp) {
	if (val_incomp == COMPRESSIBLE) {
		for (unsigned short iDim = 0; iDim < nDim; iDim++) 
			Solution[iDim+1] = val_velocity[iDim]*Solution[0]; 
	}
	if ((val_incomp == INCOMPRESSIBLE) || (val_incomp == FREESURFACE)) {
		for (unsigned short iDim = 0; iDim < nDim; iDim++) 
			Solution[iDim+1] = val_velocity[iDim]*Primitive[0]; 
	}
}

inline void CEulerVariable::SetVelocity2(void) { Velocity2 = 0.0; for (unsigned short iDim = 0; iDim < nDim; iDim++) Velocity2 += Solution[iDim+1]*Solution[iDim+1]/(Solution[0]*Solution[0]); }

inline void CEulerVariable::SetVelocityInc2(void) { Velocity2 = 0.0; for (unsigned short iDim = 0; iDim < nDim; iDim++) Velocity2 += (Solution[iDim+1]/Primitive[0])*(Solution[iDim+1]/Primitive[0]); }

inline void CEulerVariable::SetPressureInc(double val_pressure) { Solution[0] = val_pressure; }

inline void CEulerVariable::SetVelocity_Old(double *val_velocity, unsigned short val_incomp) { 
	if (val_incomp == COMPRESSIBLE) {
		for (unsigned short iDim = 0; iDim < nDim; iDim++)	
			Solution_Old[iDim+1] = val_velocity[iDim]*Solution[0]; 
	}
	if ((val_incomp == INCOMPRESSIBLE) || (val_incomp == FREESURFACE)) {
		for (unsigned short iDim = 0; iDim < nDim; iDim++)	
			Solution_Old[iDim+1] = val_velocity[iDim]*Primitive[0];
	}
}

inline void CEulerVariable::AddGradient_Primitive(unsigned short val_var, unsigned short val_dim, double val_value) { Gradient_Primitive[val_var][val_dim] += val_value; }

inline void CEulerVariable::SubtractGradient_Primitive(unsigned short val_var, unsigned short val_dim, double val_value) { Gradient_Primitive[val_var][val_dim] -= val_value; }

inline double CEulerVariable::GetGradient_Primitive(unsigned short val_var, unsigned short val_dim) { return Gradient_Primitive[val_var][val_dim]; }

inline void CEulerVariable::SetGradient_Primitive(unsigned short val_var, unsigned short val_dim, double val_value) { Gradient_Primitive[val_var][val_dim] = val_value; }

inline double **CEulerVariable::GetGradient_Primitive(void) { return Gradient_Primitive; }

inline void CEulerVariable::SetTimeSpectral_Source(unsigned short val_var, double val_source) { TS_Source[val_var] = val_source; }

inline double CEulerVariable::GetTimeSpectral_Source(unsigned short val_var) { return TS_Source[val_var]; }

inline double CEulerVariable::GetPreconditioner_Beta() { return Precond_Beta; }

inline void CEulerVariable::SetPreconditioner_Beta(double val_Beta) { Precond_Beta = val_Beta; }

inline bool CEulerVariable::SetPressure(double Gamma) {
   Primitive[nDim+1] = (Gamma-1.0)*Solution[0]*(Solution[nVar-1]/Solution[0]-0.5*Velocity2);
   if (Primitive[nDim+1] > 0.0) return false;
   else return true;
}

inline void CEulerVariable::SetMagneticField( double* val_B) { B_Field[0] = val_B[0]; B_Field[1] = val_B[1];B_Field[2] = val_B[2];}

inline double* CEulerVariable::GetMagneticField() { return B_Field;}

inline void CEulerVariable::SetWindGust( double* val_WindGust) { 
        for (unsigned short iDim = 0; iDim < nDim; iDim++)	
            WindGust[iDim] = val_WindGust[iDim];}

inline double* CEulerVariable::GetWindGust() { return WindGust;}

inline void CEulerVariable::SetWindGustDer( double* val_WindGustDer) {
        for (unsigned short iDim = 0; iDim < nDim+1; iDim++)
            WindGustDer[iDim] = val_WindGustDer[iDim];}

inline double* CEulerVariable::GetWindGustDer() { return WindGustDer;}

inline double CNSVariable::GetEddyViscosity(void) { return EddyViscosity; }

inline double CNSVariable::GetLaminarViscosity(void) { return LaminarViscosity; }

inline double CNSVariable::GetLaminarViscosityInc(void) { return LaminarViscosityInc; }

inline double CNSVariable::GetVorticity(unsigned short val_dim) { return Vorticity[val_dim]; }

inline double CNSVariable::GetStrainMag(void) { return StrainMag; }

inline void CNSVariable::SetLaminarViscosity(double val_laminar_viscosity) { LaminarViscosity = val_laminar_viscosity; }

inline void CNSVariable::SetLaminarViscosityInc(double val_laminar_viscosity_inc) { LaminarViscosityInc = val_laminar_viscosity_inc; }

inline void CNSVariable::SetEddyViscosity(double val_eddy_viscosity) { EddyViscosity = val_eddy_viscosity; }

inline void CNSVariable::SetWallTemperature(double Temperature_Wall ) { Primitive[0] = Temperature_Wall; }

inline bool CNSVariable::SetPressure(double Gamma, double turb_ke) {
   Primitive[nDim+1] = (Gamma-1.0)*Solution[0]*(Solution[nVar-1]/Solution[0]-0.5*Velocity2 - turb_ke);
   if (Primitive[nDim+1] > 0.0) return false;
   else return true;
}

inline double CTransLMVariable::GetIntermittency() { return Solution[0]; }

inline void CTransLMVariable::SetGammaSep(double gamma_sep_in) {gamma_sep = gamma_sep_in;}

inline double *CAdjEulerVariable::GetForceProj_Vector(void) { return ForceProj_Vector; }

inline double *CAdjEulerVariable::GetObjFuncSource(void) { return ObjFuncSource; }

inline double *CAdjEulerVariable::GetIntBoundary_Jump(void) { return IntBoundary_Jump; }

inline void CAdjEulerVariable::SetForceProj_Vector(double *val_ForceProj_Vector) { for (unsigned short iDim = 0; iDim < nDim; iDim++) ForceProj_Vector[iDim] = val_ForceProj_Vector[iDim]; }

inline void CAdjEulerVariable::SetObjFuncSource(double *val_ObjFuncSource) { for (unsigned short iVar = 0; iVar < nVar; iVar++) ObjFuncSource[iVar] = val_ObjFuncSource[iVar]; }

inline void CAdjEulerVariable::SetIntBoundary_Jump(double *val_IntBoundary_Jump) { for (unsigned short iVar = 0; iVar < nVar; iVar++) IntBoundary_Jump[iVar] = val_IntBoundary_Jump[iVar]; }

inline void CAdjEulerVariable::SetPhi_Old(double *val_phi) { for (unsigned short iDim = 0; iDim < nDim; iDim++) Solution_Old[iDim+1]=val_phi[iDim]; };

inline void CAdjEulerVariable::SetTimeSpectral_Source(unsigned short val_var, double val_source) { TS_Source[val_var] = val_source; }

inline double CAdjEulerVariable::GetTimeSpectral_Source(unsigned short val_var) { return TS_Source[val_var]; }

inline double *CAdjNSVariable::GetForceProj_Vector(void) { return ForceProj_Vector; }

inline void CAdjNSVariable::SetForceProj_Vector(double *val_ForceProj_Vector) {	for (unsigned short iDim = 0; iDim < nDim; iDim++) ForceProj_Vector[iDim] = val_ForceProj_Vector[iDim]; }

inline void CAdjNSVariable::SetPhi_Old(double *val_phi) { for (unsigned short iDim = 0; iDim < nDim; iDim++) Solution_Old[iDim+1] = val_phi[iDim]; };

inline void CAdjNSVariable::SetVelSolutionOldDVector(void) { for (unsigned short iDim = 0; iDim < nDim; iDim++) Solution_Old[iDim+1] = ForceProj_Vector[iDim]; };

inline void CAdjNSVariable::SetVelSolutionDVector(void) { for (unsigned short iDim = 0; iDim < nDim; iDim++) Solution[iDim+1] = ForceProj_Vector[iDim]; };

inline double *CLinEulerVariable::GetForceProj_Vector(void) { return ForceProj_Vector; }

inline void CLinEulerVariable::SetForceProj_Vector(double *val_ForceProj_Vector) { for (unsigned short iDim = 0; iDim < nDim; iDim++) ForceProj_Vector[iDim] = val_ForceProj_Vector[iDim]; }

inline void CLinEulerVariable::SetDeltaVel_Old(double *val_deltavel) { for (unsigned short iDim = 0; iDim < nDim; iDim++) Solution_Old[iDim+1]=val_deltavel[iDim]; };

inline double CLinEulerVariable::GetDeltaPressure(void) { return DeltaPressure; }

inline double CPlasmaVariable::GetPressure(unsigned short val_iSpecies) { return Primitive[val_iSpecies][nDim+2]; }

inline double CPlasmaVariable::GetVelocity2(unsigned short val_iSpecies) { return Velocity2[val_iSpecies]; }

inline void CPlasmaVariable::AddGradient_Primitive(unsigned short val_species, unsigned short val_var, unsigned short val_dim, double val_value) { Gradient_Primitive[val_species][val_var][val_dim] += val_value; }

inline void CPlasmaVariable::SubtractGradient_Primitive(unsigned short val_species, unsigned short val_var, unsigned short val_dim, double val_value) { Gradient_Primitive[val_species][val_var][val_dim] -= val_value; }

inline double CPlasmaVariable::GetPrimVar(unsigned short iSpecies, unsigned short iVar) { return Primitive[iSpecies][iVar]; }

inline double **CPlasmaVariable::GetPrimVar_Plasma(void) { return Primitive; }

inline double **CPlasmaVariable::GetGradient_Primitive(unsigned short val_species) { return Gradient_Primitive[val_species]; }

inline double ***CPlasmaVariable::GetGradient_Primitive_Plasma(void) { return Gradient_Primitive; }

inline double CPlasmaVariable::GetGradient_Primitive(unsigned short val_species, unsigned short val_var, unsigned short val_dim) { return Gradient_Primitive[val_species][val_var][val_dim]; }

inline void CPlasmaVariable::SetGradient_Primitive(unsigned short val_species, unsigned short val_var, unsigned short val_dim, double val_value) { Gradient_Primitive[val_species][val_var][val_dim] = val_value; }

inline void CPlasmaVariable::SetLimiterPrimitive(unsigned short val_species, unsigned short val_var, double val_limiter) { LimiterPrimitive[val_species][val_var] = val_limiter; }

inline double CPlasmaVariable::GetLimiterPrimitive(unsigned short val_species, unsigned short val_var) { return LimiterPrimitive[val_species][val_var]; }

inline double CPlasmaVariable::GetSoundSpeed(unsigned short val_species) { return Primitive[val_species][nDim+5]; }

inline double CPlasmaVariable::GetEnthalpy(unsigned short val_species) { return Primitive[val_species][nDim+4]; }

inline double CPlasmaVariable::GetMax_Lambda_Inv(unsigned short iSpecies) { return Max_Lambda_Inv_MultiSpecies[iSpecies]; }

inline double CPlasmaVariable::GetMax_Lambda_Visc(unsigned short iSpecies) { return Max_Lambda_Visc_MultiSpecies[iSpecies]; }

inline void CPlasmaVariable::SetMax_Lambda_Inv(double val_max_lambda, unsigned short val_species) { Max_Lambda_Inv_MultiSpecies[val_species] = val_max_lambda; }

inline void CPlasmaVariable::SetMax_Lambda_Visc(double val_max_lambda, unsigned short val_species) { Max_Lambda_Visc_MultiSpecies[val_species] = val_max_lambda;}

inline void CPlasmaVariable::AddMax_Lambda_Inv(double val_max_lambda, unsigned short iSpecies) { Max_Lambda_Inv_MultiSpecies[iSpecies] += val_max_lambda; }

inline void CPlasmaVariable::AddMax_Lambda_Visc(double val_max_lambda, unsigned short iSpecies) { Max_Lambda_Visc_MultiSpecies[iSpecies] += val_max_lambda; }

inline void CPlasmaVariable::SetLambda(double val_lambda, unsigned short iSpecies) { Lambda[iSpecies] = val_lambda; }

inline void CPlasmaVariable::AddLambda(double val_lambda, unsigned short iSpecies) { Lambda[iSpecies] += val_lambda; }

inline double CPlasmaVariable::GetLambda(unsigned short iSpecies) { return Lambda[iSpecies]; }

inline double CPlasmaVariable::GetLaminarViscosity(unsigned short iSpecies) { return LaminarViscosity_MultiSpecies[iSpecies]; }

inline double CPlasmaVariable::GetThermalConductivity(unsigned short iSpecies) {return ThermalCoeff[iSpecies]; }

inline double CPlasmaVariable::GetThermalConductivity_vib(unsigned short iSpecies) {return ThermalCoeff_vib[iSpecies]; }

inline double CPlasmaVariable::GetEddyViscosity(unsigned short iSpecies) { return EddyViscosity_MultiSpecies[iSpecies]; }

inline void CPlasmaVariable::SetLaminarViscosity(double val_laminar_viscosity, unsigned short iSpecies ) { LaminarViscosity_MultiSpecies[iSpecies] = val_laminar_viscosity; }

inline double CPlasmaVariable::GetTemperature_tr(unsigned short iSpecies) { return Primitive[iSpecies][0]; }

inline double CPlasmaVariable::GetTemperature_vib(unsigned short iSpecies) { return Primitive[iSpecies][nDim+1]; }

inline double CPlasmaVariable::GetDensity(unsigned short iSpecies) { return Primitive[iSpecies][nDim+3]; }

inline void CPlasmaVariable::SetWallTemperature(double* Temperature_Wall ) { for (unsigned short iSpecies = 0; iSpecies < nSpecies; iSpecies++) Primitive[iSpecies][0] = Temperature_Wall[iSpecies]; }

inline void CPlasmaVariable::SetDelta_Time(double val_delta_time, unsigned short iSpecies) { Species_Delta_Time[iSpecies] = val_delta_time;}

inline double CPlasmaVariable::GetDelta_Time(unsigned short iSpecies) { return Species_Delta_Time[iSpecies];}

inline void CPlasmaVariable::SetSensor(double val_sensor, unsigned short iSpecies) {Sensor_MultiSpecies[iSpecies] = val_sensor;}

inline double CPlasmaVariable::GetSensor(unsigned short iSpecies) {return Sensor_MultiSpecies[iSpecies]; }

inline void CPlasmaVariable::SetpoissonField(double* val_poissonField) {Elec_Field = val_poissonField; }

inline double* CPlasmaVariable::GetpoissonField() { return Elec_Field;}

inline double* CPlasmaVariable::GetMagneticField() { return B_Field; }

inline void CPlasmaVariable::SetMagneticField( double* val_B) { B_Field[0] = val_B[0]; B_Field[1] = val_B[1];B_Field[2] = val_B[2];}

inline double *CAdjPlasmaVariable::GetForceProj_Vector(void) { return ForceProj_Vector; }

inline double *CAdjPlasmaVariable::GetIntBoundary_Jump(void) { return IntBoundary_Jump; }

inline void CAdjPlasmaVariable::SetForceProj_Vector(double *val_ForceProj_Vector) { for (unsigned short iDim = 0; iDim < nDim; iDim++) ForceProj_Vector[iDim] = val_ForceProj_Vector[iDim]; }

inline void CAdjPlasmaVariable::SetIntBoundary_Jump(double *val_IntBoundary_Jump) { for (unsigned short iVar = 0; iVar < nVar; iVar++) IntBoundary_Jump[iVar] = val_IntBoundary_Jump[iVar]; }

inline void CAdjPlasmaVariable::SetPhi_Old(double *val_phi) { for (unsigned short iDim = 0; iDim < nDim; iDim++) Solution_Old[iDim+1]=val_phi[iDim]; };

inline void CLevelSetVariable::SetDiffLevelSet(double val_difflevelset) { DiffLevelSet = val_difflevelset; }

inline double CLevelSetVariable::GetDiffLevelSet(void) { return DiffLevelSet; }

inline void CLevelSetVariable::SetPrimVar(unsigned short val_var, double val_prim) { Primitive[val_var] = val_prim; }

inline double CLevelSetVariable::GetPrimVar(unsigned short val_var) { return Primitive[val_var]; }

inline void CFEAVariable::SetPressureValue(double val_pressure) { Pressure = val_pressure; }

inline void CFEAVariable::SetStress(unsigned short iVar, unsigned short jVar, double val_stress) { Stress[iVar][jVar] = val_stress; }
  
inline double **CFEAVariable::GetStress(void) { return Stress; }
  
inline void CFEAVariable::SetVonMises_Stress(double val_stress) { VonMises_Stress = val_stress; }
  
inline double CFEAVariable::GetVonMises_Stress(void) { return VonMises_Stress; }

inline void CWaveVariable::SetThickness_Noise(double val_thickness_noise) { Thickness_Noise = val_thickness_noise; }

inline void CWaveVariable::SetLoading_Noise(double val_loading_noise) { Loading_Noise = val_loading_noise; }

inline void CWaveVariable::SetQuadrupole_Noise(double val_quadrupole_noise) { Quadrupole_Noise = val_quadrupole_noise; }

inline double CWaveVariable::GetThickness_Noise() { return Thickness_Noise;}

inline double CWaveVariable::GetLoading_Noise() { return Loading_Noise;}

inline double CWaveVariable::GetQuadrupole_Noise() { return Quadrupole_Noise;}

inline double* CWaveVariable::GetSolution_Direct() { return Solution_Direct;}

inline void CWaveVariable::SetSolution_Direct(double *val_solution_direct) { for (unsigned short iVar = 0; iVar < nVar; iVar++) Solution_Direct[iVar] += val_solution_direct[iVar];}

inline double* CPotentialVariable::GetChargeDensity() { return Charge_Density;}

inline void CPotentialVariable::SetChargeDensity(double positive_charge, double negative_charge) {Charge_Density[0] = positive_charge; Charge_Density[1] = negative_charge;}

inline void CPotentialVariable::SetPlasmaRhoUGradient(unsigned short iSpecies, double val_gradient, unsigned short iDim) { PlasmaRhoUGradient[iSpecies][iDim] = val_gradient;}

inline double** CPotentialVariable::GetPlasmaRhoUGradient() { return PlasmaRhoUGradient;}

inline void CPotentialVariable::SetPlasmaTimeStep(double dt) { PlasmaTimeStep = dt;}

inline double CPotentialVariable::GetPlasmaTimeStep() { return PlasmaTimeStep;}

inline double* CHeatVariable::GetSolution_Direct() { return Solution_Direct;}

inline void CHeatVariable::SetSolution_Direct(double *val_solution_direct) { for (unsigned short iVar = 0; iVar < nVar; iVar++) Solution_Direct[iVar] += val_solution_direct[iVar];}

inline void CTurbSAVariable::SetTimeSpectral_Source(unsigned short val_var, double val_source) { TS_Source[val_var] = val_source; }

inline double CTurbSAVariable::GetTimeSpectral_Source(unsigned short val_var) { return TS_Source[val_var]; }

inline void CVariable::SetEddyViscSens(double *val_EddyViscSens, unsigned short numTotalVar) { }

inline void CAdjTurbVariable::SetEddyViscSens(double *val_EddyViscSens, unsigned short numTotalVar) { 
    for (unsigned short iVar = 0; iVar < numTotalVar; iVar++) {
        EddyViscSens[iVar] = val_EddyViscSens[iVar];}
         }

inline double *CVariable::GetEddyViscSens(void) { return NULL; }

inline double *CAdjTurbVariable::GetEddyViscSens(void) { return EddyViscSens; }

inline double CTNE2EulerVariable::GetDensity(void) { return Primitive[RHO_INDEX]; }

inline double CTNE2EulerVariable::GetMassFraction(unsigned short val_Species) {
    return Primitive[RHOS_INDEX+val_Species] / Primitive[RHO_INDEX]; 
}

inline double CTNE2EulerVariable::GetEnergy(void) { return Solution[nSpecies+nDim]/Primitive[RHO_INDEX]; };

inline double CTNE2EulerVariable::GetEnthalpy(void) { return Primitive[H_INDEX]; }

inline double CTNE2EulerVariable::GetPressure(void) { return Primitive[P_INDEX]; }

inline double CTNE2EulerVariable::GetSoundSpeed(void) { return Primitive[A_INDEX]; }

inline double CTNE2EulerVariable::GetTemperature(void) { return Primitive[T_INDEX]; }

inline double CTNE2EulerVariable::GetTemperature_ve(void) { return Primitive[TVE_INDEX]; }

inline double CTNE2EulerVariable::GetRhoCv_tr(void) { return Primitive[RHOCVTR_INDEX]; }

inline double CTNE2EulerVariable::GetRhoCv_ve(void) { return Primitive[RHOCVVE_INDEX]; }

inline double* CTNE2EulerVariable::GetdPdrhos(void) { return dPdrhos; }

inline double CTNE2EulerVariable::GetVelocity(unsigned short val_dim) {
double velocity;
   //velocity = Solution[nSpecies+val_dim]/Primitive[RHO_INDEX]; 
   velocity = Primitive[VEL_INDEX+val_dim];
return velocity;
}

inline double CTNE2EulerVariable::GetVelocity2(void) { return Velocity2; }

inline void CTNE2EulerVariable::SetEnthalpy(void) { Primitive[H_INDEX] = (Solution[nSpecies+nDim] + Primitive[P_INDEX]) / Primitive[RHO_INDEX]; }

inline double CTNE2EulerVariable::GetPrimVar(unsigned short val_var) { return Primitive[val_var]; }

inline void CTNE2EulerVariable::SetPrimVar(unsigned short val_var, double val_prim) { Primitive[val_var] = val_prim; }

inline void CTNE2EulerVariable::SetPrimVar(double *val_prim) {
   for (unsigned short iVar = 0; iVar < nPrimVar; iVar++) 
      Primitive[iVar] = val_prim[iVar]; 
}

inline double *CTNE2EulerVariable::GetPrimVar(void) { return Primitive; }

inline void CTNE2EulerVariable::SetVelocity_Old(double *val_velocity, bool val_incomp) {
  for (unsigned short iDim = 0; iDim < nDim; iDim++)
    Solution_Old[nSpecies+iDim] = val_velocity[iDim]*Primitive[RHO_INDEX];

}

inline void CTNE2EulerVariable::AddGradient_Primitive(unsigned short val_var, unsigned short val_dim, double val_value) { Gradient_Primitive[val_var][val_dim] += val_value; }

inline void CTNE2EulerVariable::SubtractGradient_Primitive(unsigned short val_var, unsigned short val_dim, double val_value) { Gradient_Primitive[val_var][val_dim] -= val_value; }

inline double CTNE2EulerVariable::GetGradient_Primitive(unsigned short val_var, unsigned short val_dim) { return Gradient_Primitive[val_var][val_dim]; }

inline void CTNE2EulerVariable::SetGradient_Primitive(unsigned short val_var, unsigned short val_dim, double val_value) { Gradient_Primitive[val_var][val_dim] = val_value; }

inline double **CTNE2EulerVariable::GetGradient_Primitive(void) { return Gradient_Primitive; }

inline double CTNE2EulerVariable::GetPreconditioner_Beta() { return Precond_Beta; }

inline void CTNE2EulerVariable::SetPreconditioner_Beta(double val_Beta) { Precond_Beta = val_Beta; }

inline unsigned short CTNE2EulerVariable::GetRhosIndex(void) { return RHOS_INDEX; }

inline unsigned short CTNE2EulerVariable::GetRhoIndex(void) { return RHO_INDEX; }

inline unsigned short CTNE2EulerVariable::GetPIndex(void) { return P_INDEX; }

inline unsigned short CTNE2EulerVariable::GetTIndex(void) { return T_INDEX; }

inline unsigned short CTNE2EulerVariable::GetTveIndex(void) { return TVE_INDEX; }

inline unsigned short CTNE2EulerVariable::GetVelIndex(void) { return VEL_INDEX; }

inline unsigned short CTNE2EulerVariable::GetHIndex(void) { return H_INDEX; }

inline unsigned short CTNE2EulerVariable::GetAIndex(void) { return A_INDEX; }

inline unsigned short CTNE2EulerVariable::GetRhoCvtrIndex(void) { return RHOCVTR_INDEX; }

inline unsigned short CTNE2EulerVariable::GetRhoCvveIndex(void) { return RHOCVVE_INDEX; }

inline double  CTNE2NSVariable::GetLaminarViscosity(void) { return LaminarViscosity; }

inline double  CTNE2NSVariable::GetVorticity(unsigned short val_dim) { return Vorticity[val_dim]; }

inline void    CTNE2NSVariable::SetLaminarViscosity(double val_laminar_viscosity) { LaminarViscosity = val_laminar_viscosity; }

inline void    CTNE2NSVariable::SetWallTemperature(double Temperature_Wall ) { Primitive[T_INDEX] = Temperature_Wall; }

inline void    CAdjTNE2EulerVariable::SetPhi_Old(double *val_phi) { for (unsigned short iDim = 0; iDim < nDim; iDim++) Solution_Old[iDim+1]=val_phi[iDim]; };

inline double *CAdjTNE2EulerVariable::GetObjFuncSource(void) { return ObjFuncSource; }

inline void    CAdjTNE2EulerVariable::SetObjFuncSource(double *val_ObjFuncSource) { for (unsigned short iVar = 0; iVar < nVar; iVar++) ObjFuncSource[iVar] = val_ObjFuncSource[iVar]; }

inline double *CAdjTNE2EulerVariable::GetForceProj_Vector(void) { return ForceProj_Vector; }

inline void    CAdjTNE2EulerVariable::SetForceProj_Vector(double *val_ForceProj_Vector) { for (unsigned short iDim = 0; iDim < nDim; iDim++) ForceProj_Vector[iDim] = val_ForceProj_Vector[iDim]; }

inline double  CAdjTNE2EulerVariable::GetTheta(void) { return Theta; }

inline void    CAdjTNE2NSVariable::SetPhi_Old(double *val_phi) { for (unsigned short iDim = 0; iDim < nDim; iDim++) Solution_Old[iDim+1] = val_phi[iDim]; };

inline double* CAdjTNE2NSVariable::GetForceProj_Vector(void) { return ForceProj_Vector; }

inline void    CAdjTNE2NSVariable::SetForceProj_Vector(double *val_ForceProj_Vector) {	for (unsigned short iDim = 0; iDim < nDim; iDim++) ForceProj_Vector[iDim] = val_ForceProj_Vector[iDim]; }

inline void    CAdjTNE2NSVariable::SetVelSolutionDVector(void) { for (unsigned short iDim = 0; iDim < nDim; iDim++) Solution[iDim+1] = ForceProj_Vector[iDim]; };

inline void    CAdjTNE2NSVariable::SetVelSolutionOldDVector(void) { for (unsigned short iDim = 0; iDim < nDim; iDim++) Solution_Old[iDim+1] = ForceProj_Vector[iDim]; };

inline double  CAdjTNE2NSVariable::GetTheta(void) { return Theta; }
