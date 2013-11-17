/*!
 * \file variable_structure.inl
 * \brief In-Line subroutines of the <i>variable_structure.hpp</i> file.
 * \author Aerospace Design Laboratory (Stanford University) <http://su2.stanford.edu>.
 * \version 2.0.9
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

inline void CVariable::SetVelSolutionOldDVector(void) { }

inline void CVariable::SetVelSolutionDVector(void) { }

inline void CVariable::SetStress(unsigned short iVar, unsigned short jVar, double val_stress) { }
  
inline double **CVariable::GetStress(void) { return 0; }
  
inline void CVariable::SetVonMises_Stress(double val_stress) { }
  
inline double CVariable::GetVonMises_Stress(void) { return 0; }

inline void CVariable::SetFlow_Pressure(double val_pressure) { }

inline double CVariable::GetFlow_Pressure(void) { return 0; }

inline void CVariable::SetPressureInc(double val_pressure) { }

inline double CVariable::GetBetaInc2(void) { return 0; }

inline double CVariable::GetDiffLevelSet(void) { return 0; }

inline double CVariable::GetDensityInc(void) { return 0; }

inline double CVariable::GetLevelSet(void) { return 0; }

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

inline double* CVariable::GetDiffusionCoeff(void) { return NULL; }

inline double CVariable::GetThermalConductivity(void) { return 0; }

inline double CVariable::GetThermalConductivity(unsigned short iSpecies) { return 0; }

inline double CVariable::GetThermalConductivity_ve(void) { return 0; }

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

inline bool CVariable::SetPrimVar_FreeSurface(CConfig *config) { return true; }

inline bool CVariable::SetPrimVar_Incompressible(double Density_Inf, double Viscosity_Inf, double turb_ke, CConfig *config) { return true; }

inline bool CVariable::SetPrimVar_FreeSurface(double turb_ke, CConfig *config) { return true; }

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

inline double *CVariable::GetdTdrhos() { return NULL; }

inline double *CVariable::GetdTvedrhos() { return NULL; }

inline void CVariable::SetDensity() { }

inline void CVariable::SetDeltaPressure(double *val_velocity, double Gamma) { }

inline bool CVariable::SetSoundSpeed(CConfig *config) { return false; }

inline bool CVariable::SetSoundSpeed() { return false; }

inline bool CVariable::SetSoundSpeed(double Gamma) { return false; }

inline bool CVariable::SetTemperature(double Gas_Constant) { return false; }

inline bool CVariable::SetTemperature_ve(double val_Tve) {return false; }

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

inline double CEulerVariable::GetLevelSet(void) { return Solution[nDim+1]; }

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

inline void CTurbSAVariable::SetTimeSpectral_Source(unsigned short val_var, double val_source) { TS_Source[val_var] = val_source; }

inline double CTurbSAVariable::GetTimeSpectral_Source(unsigned short val_var) { return TS_Source[val_var]; }

inline void CVariable::SetEddyViscSens(double *val_EddyViscSens, unsigned short numTotalVar) { }

inline double *CVariable::GetEddyViscSens(void) { return NULL; }
