/*!
 * \file variable_direct_tne2.cpp
 * \brief Definition of the solution fields.
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

#include "../include/variable_structure.hpp"
#include <math.h>

CTNE2EulerVariable::CTNE2EulerVariable(void) : CVariable() {  

  /*--- Array initialization ---*/
	Primitive = NULL;
	Gradient_Primitive = NULL;
	Limiter_Primitive = NULL;

}

CTNE2EulerVariable::CTNE2EulerVariable(double val_pressure,
                                       double *val_massfrac,
                                       double *val_mach,
                                       double val_temperature,
                                       double val_temperature_ve,
                                       unsigned short val_ndim,
                                       unsigned short val_nvar,
                                       unsigned short val_nvarprim,
                                       unsigned short val_nvarprimgrad,
                                       CConfig *config) : CVariable(val_ndim,
                                                                    val_nvar,
                                                                    config   ) {
  
  unsigned short iEl, iMesh, iDim, iSpecies, iVar, nDim, nEl, nHeavy, nMGSmooth;
  unsigned short *nElStates;
  double *xi, *Ms, *thetav, **thetae, **g, *hf, *Tref;
  double rhoE, rhoEve, Ev, Ee, Ef, T, Tve, rho, rhoCvtr, rhos;
  double Ru, sqvel, num, denom, conc, soundspeed;
  
  /*--- Get Mutation++ mixture ---*/
  nSpecies     = config->GetnSpecies();
  nDim         = val_ndim;
  nPrimVar     = val_nvarprim;
  nPrimVarGrad = val_nvarprimgrad;
  nMGSmooth    = 0;
  
  /*--- Define structure of the primtive variable vector ---*/
  // Primitive: [rho1, ..., rhoNs, T, Tve, u, v, w, P, rho, h, a, rhoCvtr, rhoCvve]^T
  // GradPrim:  [rho1, ..., rhoNs, T, Tve, u, v, w, P]^T
  RHOS_INDEX    = 0;
  T_INDEX       = nSpecies;
  TVE_INDEX     = nSpecies+1;
  VEL_INDEX     = nSpecies+2;
  P_INDEX       = nSpecies+nDim+2;
  RHO_INDEX     = nSpecies+nDim+3;
  H_INDEX       = nSpecies+nDim+4;
  A_INDEX       = nSpecies+nDim+5;
  RHOCVTR_INDEX = nSpecies+nDim+6;
  RHOCVVE_INDEX = nSpecies+nDim+7;
  
  /*--- Array initialization ---*/
	Primitive = NULL;
	Gradient_Primitive = NULL;
	Limiter_Primitive = NULL;
  
  /*--- Allocate & initialize residual vectors ---*/
	Res_TruncError = new double [nVar];
	for (iVar = 0; iVar < nVar; iVar++) {
		Res_TruncError[iVar] = 0.0;
	}
  
	/*--- If using multigrid, allocate residual-smoothing vectors ---*/
	for (iMesh = 0; iMesh <= config->GetMGLevels(); iMesh++)
		nMGSmooth += config->GetMG_CorrecSmooth(iMesh);
	if (nMGSmooth > 0) {
		Residual_Sum = new double [nVar];
		Residual_Old = new double [nVar];
	}
  
  /*--- If using limiters, allocate the arrays ---*/
  if ((config->GetKind_ConvNumScheme_Flow() == SPACE_UPWIND) &&
			(config->GetKind_SlopeLimit_Flow() != NONE)) {
		Limiter      = new double [nVar];
		Solution_Max = new double [nVar];
		Solution_Min = new double [nVar];
		for (iVar = 0; iVar < nVar; iVar++) {
			Limiter[iVar]      = 0.0;
			Solution_Max[iVar] = 0.0;
			Solution_Min[iVar] = 0.0;
		}
	}
  
  /*--- Allocate & initialize primitive variable & gradient arrays ---*/
  Primitive = new double [nPrimVar];
  for (iVar = 0; iVar < nPrimVar; iVar++) Primitive[iVar] = 0.0;
  Gradient_Primitive = new double* [nPrimVarGrad];
  for (iVar = 0; iVar < nPrimVarGrad; iVar++) {
    Gradient_Primitive[iVar] = new double [nDim];
    for (iDim = 0; iDim < nDim; iDim++)
      Gradient_Primitive[iVar][iDim] = 0.0;
  }
  
  /*--- Allocate partial derivative vectors ---*/
  dPdrhos = new double [nSpecies];
  
  /*--- Determine the number of heavy species ---*/
  ionization = config->GetIonization();
  if (ionization) { nHeavy = nSpecies-1; nEl = 1; }
  else            { nHeavy = nSpecies;   nEl = 0; }

  /*--- Load variables from the config class --*/
  xi        = config->GetRotationModes();      // Rotational modes of energy storage
  Ms        = config->GetMolar_Mass();         // Species molar mass
  thetav    = config->GetCharVibTemp();        // Species characteristic vib. temperature [K]
  thetae    = config->GetCharElTemp();         // Characteristic electron temperature [K]
  g         = config->GetElDegeneracy();       // Degeneracy of electron states
  nElStates = config->GetnElStates();          // Number of electron states
  Tref      = config->GetRefTemperature();     // Thermodynamic reference temperature [K]
  hf        = config->GetEnthalpy_Formation(); // Formation enthalpy [J/kg]
  
  /*--- Rename & initialize for convenience ---*/
  Ru      = UNIVERSAL_GAS_CONSTANT;         // Universal gas constant [J/(kmol*K)]
  Tve     = val_temperature_ve;             // Vibrational temperature [K]
  T       = val_temperature;                // Translational-rotational temperature [K]
  sqvel   = 0.0;                            // Velocity^2 [m2/s2]
  rhoE    = 0.0;                            // Mixture total energy per mass [J/kg]
  rhoEve  = 0.0;                            // Mixture vib-el energy per mass [J/kg]
  denom   = 0.0;
  conc    = 0.0;
  rhoCvtr = 0.0;
  
  /*--- Calculate mixture density from supplied primitive quantities ---*/
  for (iSpecies = 0; iSpecies < nHeavy; iSpecies++)
    denom += val_massfrac[iSpecies] * (Ru/Ms[iSpecies]) * T;
  for (iSpecies = 0; iSpecies < nEl; iSpecies++)
    denom += val_massfrac[nSpecies-1] * (Ru/Ms[nSpecies-1]) * Tve;
  rho = val_pressure / denom;
  
  /*--- Calculate sound speed and extract velocities ---*/
  for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
    conc += val_massfrac[iSpecies]*rho/Ms[iSpecies];
    rhoCvtr += rho*val_massfrac[iSpecies] * (3.0/2.0 + xi[iSpecies]/2.0) * Ru/Ms[iSpecies];
  }
  soundspeed = sqrt((1.0 + Ru/rhoCvtr*conc) * val_pressure/rho);
  for (iDim = 0; iDim < nDim; iDim++)
    sqvel += val_mach[iDim]*soundspeed * val_mach[iDim]*soundspeed;
  
  /*--- Calculate energy (RRHO) from supplied primitive quanitites ---*/
  for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
    // Species density
    rhos = val_massfrac[iSpecies]*rho;
    
    // Species formation energy
    Ef = hf[iSpecies] - Ru/Ms[iSpecies]*Tref[iSpecies];
    
    // Species vibrational energy
    if (thetav[iSpecies] != 0.0)
      Ev = Ru/Ms[iSpecies] * thetav[iSpecies] / (exp(thetav[iSpecies]/Tve)-1.0);
    else
      Ev = 0.0;
    
    // Species electronic energy    
    num = 0.0;
    denom = g[iSpecies][0] * exp(thetae[iSpecies][0]/Tve);
    for (iEl = 1; iEl < nElStates[iSpecies]; iEl++) {
      num   += g[iSpecies][iEl] * thetae[iSpecies][iEl] * exp(-thetae[iSpecies][iEl]/Tve);
      denom += g[iSpecies][iEl] * exp(-thetae[iSpecies][iEl]/Tve);
    }
    Ee = Ru/Ms[iSpecies] * (num/denom);
    
    // Mixture total energy
    rhoE += rhos * ((3.0/2.0+xi[iSpecies]/2.0) * Ru/Ms[iSpecies] * (T-Tref[iSpecies])
                    + Ev + Ee + Ef + 0.5*sqvel);
    
    // Mixture vibrational-electronic energy
    rhoEve += rhos * (Ev + Ee);
  }
  for (iSpecies = 0; iSpecies < nEl; iSpecies++) {
    // Species formation energy
    Ef = hf[nSpecies-1] - Ru/Ms[nSpecies-1] * Tref[nSpecies-1];
    
    // Electron t-r mode contributes to mixture vib-el energy
    rhoEve += (3.0/2.0) * Ru/Ms[nSpecies-1] * (Tve - Tref[nSpecies-1]);
  }
  
  /*--- Initialize Solution & Solution_Old vectors ---*/
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    Solution[iSpecies]     = rho*val_massfrac[iSpecies];
    Solution_Old[iSpecies] = rho*val_massfrac[iSpecies];
  }
  for (iDim = 0; iDim < nDim; iDim++) {
    Solution[nSpecies+iDim]     = rho*val_mach[iDim]*soundspeed;
    Solution_Old[nSpecies+iDim] = rho*val_mach[iDim]*soundspeed;
  }
  Solution[nSpecies+nDim]       = rhoE;
  Solution_Old[nSpecies+nDim]   = rhoE;
  Solution[nSpecies+nDim+1]     = rhoEve;
  Solution_Old[nSpecies+nDim+1] = rhoEve;
  
  /*--- Assign primitive variables ---*/
  Primitive[T_INDEX]   = val_temperature;
  Primitive[TVE_INDEX] = val_temperature_ve;
  Primitive[P_INDEX]   = val_pressure;
}

CTNE2EulerVariable::CTNE2EulerVariable(double *val_solution, unsigned short val_ndim,
                                       unsigned short val_nvar, unsigned short val_nvarprim,
                                       unsigned short val_nvarprimgrad, CConfig *config) : CVariable(val_ndim, val_nvar, config) {
	unsigned short iVar, iDim, iMesh, nMGSmooth = 0;
  
  nSpecies     = config->GetnSpecies();
  nDim         = val_ndim;
  nPrimVar     = val_nvarprim;
  nPrimVarGrad = val_nvarprimgrad;
  
  /*--- Define structure of the primtive variable vector ---*/
  // Primitive: [rho1, ..., rhoNs, T, Tve, u, v, w, P, rho, h, a, rhoCvtr, rhoCvve]^T
  // GradPrim:  [rho1, ..., rhoNs, T, Tve, u, v, w, P]^T
  RHOS_INDEX    = 0;
  T_INDEX       = nSpecies;
  TVE_INDEX     = nSpecies+1;
  VEL_INDEX     = nSpecies+2;
  P_INDEX       = nSpecies+nDim+2;
  RHO_INDEX     = nSpecies+nDim+3;
  H_INDEX       = nSpecies+nDim+4;
  A_INDEX       = nSpecies+nDim+5;
  RHOCVTR_INDEX = nSpecies+nDim+6;
  RHOCVVE_INDEX = nSpecies+nDim+7;
  
  /*--- Array initialization ---*/
	Primitive = NULL;
	Gradient_Primitive = NULL;
  Limiter_Primitive = NULL;
  
  /*--- Allocate & initialize residual vectors ---*/
	Res_TruncError = new double [nVar];
	for (iVar = 0; iVar < nVar; iVar++) {
		Res_TruncError[iVar] = 0.0;
	}
  
	/*--- If using multigrid, allocate residual-smoothing vectors ---*/
	for (iMesh = 0; iMesh <= config->GetMGLevels(); iMesh++)
		nMGSmooth += config->GetMG_CorrecSmooth(iMesh);
	if (nMGSmooth > 0) {
		Residual_Sum = new double [nVar];
		Residual_Old = new double [nVar];
	}
  
  /*--- If using limiters, allocate the arrays ---*/
  if ((config->GetKind_ConvNumScheme_Flow() == SPACE_UPWIND) &&
			(config->GetKind_SlopeLimit_Flow() != NONE)) {
		Limiter      = new double [nVar];
		Solution_Max = new double [nVar];
		Solution_Min = new double [nVar];
		for (iVar = 0; iVar < nVar; iVar++) {
			Limiter[iVar]      = 0.0;
			Solution_Max[iVar] = 0.0;
			Solution_Min[iVar] = 0.0;
		}
	}
  
  /*--- Allocate & initialize primitive variable & gradient arrays ---*/
  Primitive = new double [nPrimVar];
  for (iVar = 0; iVar < nPrimVar; iVar++) Primitive[iVar] = 0.0;
  Gradient_Primitive = new double* [nPrimVarGrad];
  for (iVar = 0; iVar < nPrimVarGrad; iVar++) {
    Gradient_Primitive[iVar] = new double [nDim];
    for (iDim = 0; iDim < nDim; iDim++)
      Gradient_Primitive[iVar][iDim] = 0.0;
  }
  
  /*--- Allocate partial derivative vectors ---*/
  dPdrhos = new double [nSpecies];
  
	/*--- Initialize Solution & Solution_Old vectors ---*/
	for (iVar = 0; iVar < nVar; iVar++) {
		Solution[iVar]     = val_solution[iVar];
		Solution_Old[iVar] = val_solution[iVar];
	}
  
  /*--- Initialize Tve to the free stream for Newton-Raphson method ---*/
  Primitive[TVE_INDEX] = config->GetTemperature_FreeStream();
  Primitive[T_INDEX]   = config->GetTemperature_FreeStream();
  Primitive[P_INDEX]   = config->GetPressure_FreeStream();
}

CTNE2EulerVariable::~CTNE2EulerVariable(void) {
	unsigned short iVar;
  
  if (Primitive         != NULL) delete [] Primitive;
  if (Limiter_Primitive != NULL) delete [] Limiter_Primitive;
  
  if (Res_TruncError != NULL) delete [] Res_TruncError;
  if (Residual_Old   != NULL) delete [] Residual_Old;
  if (Residual_Sum   != NULL) delete [] Residual_Sum;
  if (Limiter != NULL) delete [] Limiter;
  if (Solution_Max != NULL) delete [] Solution_Max;
  if (Solution_Min != NULL) delete [] Solution_Min;
  
  if (Primitive != NULL) delete [] Primitive;
  if (Gradient_Primitive != NULL) {
    for (iVar = 0; iVar < nPrimVarGrad; iVar++)
      delete Gradient_Primitive[iVar];
    delete [] Gradient_Primitive;
  }
  if (dPdrhos != NULL) delete [] dPdrhos;
  
}

void CTNE2EulerVariable::SetGradient_PrimitiveZero(unsigned short val_primvar) {
	unsigned short iVar, iDim;
  
	for (iVar = 0; iVar < val_primvar; iVar++)
		for (iDim = 0; iDim < nDim; iDim++)
			Gradient_Primitive[iVar][iDim] = 0.0;
}

void CTNE2EulerVariable::SetDensity(void) {
  unsigned short iSpecies;
  double Density;
  
  Density = 0.0;
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    Primitive[RHOS_INDEX+iSpecies] = Solution[iSpecies];
    Density += Solution[iSpecies];
  }
  Primitive[RHO_INDEX] = Density;
}

double CTNE2EulerVariable::GetProjVel(double *val_vector) {
	double ProjVel, density;
	unsigned short iDim, iSpecies;
  
	ProjVel = 0.0;
  density = 0.0;
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
    density += Solution[iSpecies];
	for (iDim = 0; iDim < nDim; iDim++)
		ProjVel += Solution[nSpecies+iDim]*val_vector[iDim]/density;
  
	return ProjVel;
}

void CTNE2EulerVariable::SetVelocity(double *val_velocity, bool val_incomp) {
	if (val_incomp) {
		for (unsigned short iDim = 0; iDim < nDim; iDim++)
			Solution[nSpecies+iDim] = val_velocity[iDim]*Primitive[RHO_INDEX];
	}
	else {
		for (unsigned short iDim = 0; iDim < nDim; iDim++)
			Solution[nSpecies+iDim] = val_velocity[iDim]*Primitive[RHO_INDEX];
	}
}

void CTNE2EulerVariable::SetVelocity2(void) {
  unsigned short iDim;
  
  Velocity2 = 0.0;
  for (iDim = 0; iDim < nDim; iDim++) {
    Primitive[VEL_INDEX+iDim] = Solution[nSpecies+iDim] / Primitive[RHO_INDEX];
    Velocity2 +=  Solution[nSpecies+iDim]*Solution[nSpecies+iDim]
    / (Primitive[RHO_INDEX]*Primitive[RHO_INDEX]);
  }
}

bool CTNE2EulerVariable::SetTemperature(CConfig *config) {
  
  // Note: Requires previous call to SetDensity()
  // Note: Missing contribution from electronic energy
  bool ionization;
  unsigned short iEl, iSpecies, iDim, nHeavy, nEl, iIter, maxIter, *nElStates;
  double *xi, *Ms, *thetav, **thetae, **g, *hf, *Tref;
  double rho, rhoE, rhoEve, rhoEve_t, rhoE_ref, rhoE_f;
  double evs, eels;
  double Ru, sqvel, rhoCvtr, rhoCvve;
  double Cvvs, Cves, Tve, Tve2;
  double f, df, tol;
  double exptv, thsqr, thoTve;
  double num, denom, num2, num3;
  
  /*--- Set tolerance for Newton-Raphson method ---*/
  tol     = 1.0E-4;
  maxIter = 50;
  
  /*--- Determine the number of heavy species ---*/
  ionization = config->GetIonization();
  if (ionization) { nHeavy = nSpecies-1; nEl = 1; }
  else            { nHeavy = nSpecies;   nEl = 0; }
  
  /*--- Load variables from the config class --*/
  xi        = config->GetRotationModes();      // Rotational modes of energy storage
  Ms        = config->GetMolar_Mass();         // Species molar mass
  thetav    = config->GetCharVibTemp();        // Species characteristic vib. temperature [K]
  Tref      = config->GetRefTemperature();     // Thermodynamic reference temperature [K]
  hf        = config->GetEnthalpy_Formation(); // Formation enthalpy [J/kg]
  thetae    = config->GetCharElTemp();
  g         = config->GetElDegeneracy();
  nElStates = config->GetnElStates();
  
  /*--- Rename & initialize for convenience ---*/
  Ru       = UNIVERSAL_GAS_CONSTANT;           // Universal gas constant [J/(kmol*K)]
  rho      = Primitive[RHO_INDEX];             // Mixture density [kg/m3]
  rhoE     = Solution[nSpecies+nDim];          // Density * energy [J/m3]
  rhoEve   = Solution[nSpecies+nDim+1];        // Density * energy_ve [J/m3]
  rhoE_f   = 0.0;                              // Density * formation energy [J/m3]
  rhoE_ref = 0.0;                              // Density * reference energy [J/m3]
  rhoCvtr  = 0.0;                              // Mix spec. heat @ const. volume [J/(kg*K)]
  sqvel    = 0.0;                              // Velocity^2 [m2/s2]
  
  /*--- Calculate mixture properties (heavy particles only) ---*/
  for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
    rhoCvtr  += Solution[iSpecies] * (3.0/2.0 + xi[iSpecies]/2.0) * Ru/Ms[iSpecies];
    rhoE_ref += Solution[iSpecies] * (3.0/2.0 + xi[iSpecies]/2.0) * Ru/Ms[iSpecies] * Tref[iSpecies];
    rhoE_f   += Solution[iSpecies] * (hf[iSpecies] - Ru/Ms[iSpecies]*Tref[iSpecies]);
  }
  for (iDim = 0; iDim < nDim; iDim++)
    sqvel    += (Solution[nSpecies+iDim]/rho) * (Solution[nSpecies+iDim]/rho);
  
  /*--- Calculate translational-rotational temperature ---*/
  Primitive[T_INDEX] = (rhoE - rhoEve - rhoE_f + rhoE_ref - 0.5*rho*sqvel) / rhoCvtr;
  
  /*--- Calculate vibrational-electronic temperature ---*/
  // NOTE: Cannot write an expression explicitly in terms of Tve
  // NOTE: We use Newton-Raphson to iteratively find the value of Tve
  // NOTE: Use T as an initial guess
  Tve  = Primitive[TVE_INDEX];
  Tve2 = Primitive[TVE_INDEX];
  
  for (iIter = 0; iIter < maxIter; iIter++) {
    rhoEve_t = 0.0;
    rhoCvve  = 0.0;
    for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
      
      /*--- Vibrational energy ---*/
      if (thetav[iSpecies] != 0.0) {
        
        /*--- Rename for convenience ---*/
        thoTve = thetav[iSpecies]/Tve;
        exptv = exp(thetav[iSpecies]/Tve);
        thsqr = thetav[iSpecies]*thetav[iSpecies];
        
        /*--- Calculate vibrational energy ---*/
        evs  = Ru/Ms[iSpecies] * thetav[iSpecies] / (exptv - 1.0);
        
        /*--- Calculate species vibrational specific heats ---*/
        Cvvs  = Ru/Ms[iSpecies] * thoTve*thoTve * exptv / ((exptv-1.0)*(exptv-1.0));
        
        /*--- Add contribution ---*/
        rhoEve_t += Solution[iSpecies] * evs;
        rhoCvve  += Solution[iSpecies] * Cvvs;        
      }
      /*--- Electronic energy ---*/
      if (nElStates[iSpecies] != 0) {
        num = 0.0; num2 = 0.0;
        denom = g[iSpecies][0] * exp(thetae[iSpecies][0]/Tve);
        num3  = g[iSpecies][0] * (thetae[iSpecies][0]/(Tve*Tve))*exp(-thetae[iSpecies][0]/Tve);
        for (iEl = 1; iEl < nElStates[iSpecies]; iEl++) {
          thoTve = thetae[iSpecies][iEl]/Tve;
          exptv = exp(-thetae[iSpecies][iEl]/Tve);
          
          num   += g[iSpecies][iEl] * thetae[iSpecies][iEl] * exptv;
          denom += g[iSpecies][iEl] * exptv;
          num2  += g[iSpecies][iEl] * (thoTve*thoTve) * exptv;
          num3  += g[iSpecies][iEl] * thoTve/Tve * exptv;
        }
        eels = Ru/Ms[iSpecies] * (num/denom);
        Cves = Ru/Ms[iSpecies] * (num2/denom - num*num3/(denom*denom));
        
        rhoEve_t += Solution[iSpecies] * eels;
        rhoCvve  += Solution[iSpecies] * Cves;
      }
    }
    for (iSpecies = 0; iSpecies < nEl; iSpecies++) {
      Cves = 3.0/2.0 * Ru/Ms[nSpecies-1];
      rhoEve_t += Solution[nSpecies-1] * Cves * Tve;
      rhoCvve += Solution[nSpecies-1] * Cves;
    }
    
    /*--- Determine function f(Tve) and df/dTve ---*/
    f  = rhoEve - rhoEve_t;
    df = -rhoCvve;
    Tve2 = Tve - (f/df)*0.5;
    
    /*--- Check for convergence ---*/
    if (fabs(Tve2-Tve) < tol) break;
    if (iIter == maxIter-1) {
      cout << "WARNING!!! Tve convergence not reached!" << endl;
      Tve2 = Primitive[TVE_INDEX];
    }
    Tve = Tve2;
    
  }
  
  if (Tve2 <= 0)
    Tve2 = 1E-8;
  Primitive[TVE_INDEX] = Tve2;
  
  /*--- Assign Gas Properties ---*/
  Primitive[RHOCVTR_INDEX] = rhoCvtr;
  Primitive[RHOCVVE_INDEX] = rhoCvve;
  
  /*--- Check that the solution is physical ---*/
  if ((Primitive[T_INDEX] > 0.0) && (Primitive[TVE_INDEX])) return false;
  else return true;
}


void CTNE2EulerVariable::SetGasProperties(CConfig *config) {

  // NOTE: Requires computation of vib-el temperature.
  // NOTE: For now, neglecting contribution from electronic excitation
  
  unsigned short iSpecies, nHeavy, nEl;
  double rhoCvtr, rhoCvve, Ru, Tve;
  double *xi, *Ms, *thetav;
  
  /*--- Determine the number of heavy species ---*/
  if (ionization) { nHeavy = nSpecies-1; nEl = 1; }
  else            { nHeavy = nSpecies;   nEl = 0; }
  
  /*--- Load variables from the config class --*/
  xi     = config->GetRotationModes(); // Rotational modes of energy storage
  Ms     = config->GetMolar_Mass();    // Species molar mass
  thetav = config->GetCharVibTemp();   // Species characteristic vib. temperature [K]
  Ru     = UNIVERSAL_GAS_CONSTANT;     // Universal gas constant [J/(kmol*K)]
  Tve    = Primitive[nSpecies+1];      // Vibrational-electronic temperature [K]
  
  rhoCvtr = 0.0;
  rhoCvve = 0.0;
  
  /*--- Heavy particle contribution ---*/
  for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
    rhoCvtr += Solution[iSpecies] * (3.0/2.0 + xi[iSpecies]/2.0) * Ru/Ms[iSpecies];
    
    if (thetav[iSpecies] != 0.0) {
      rhoCvve += Ru/Ms[iSpecies] * (thetav[iSpecies]/Tve)*(thetav[iSpecies]/Tve) * exp(thetav[iSpecies]/Tve)
                 / ((exp(thetav[iSpecies]/Tve)-1.0)*(exp(thetav[iSpecies]/Tve)-1.0));
    }
    
    //--- [ Electronic energy goes here ] ---//
    
  }
  
  /*--- Free-electron contribution ---*/
  for (iSpecies = 0; iSpecies < nEl; iSpecies++) {
    rhoCvve += Solution[nSpecies-1] * 3.0/2.0 * Ru/Ms[nSpecies-1];
  }
  
  /*--- Store computed values ---*/
  Primitive[RHOCVTR_INDEX] = rhoCvtr;
  Primitive[RHOCVVE_INDEX] = rhoCvve;
}


bool CTNE2EulerVariable::SetPressure(CConfig *config) {
  
  // NOTE: Requires computation of trans-rot & vib-el temperatures.
  
  unsigned short iSpecies, nHeavy, nEl;
  double *Ms;
  double P, Ru;
  
  /*--- Determine the number of heavy species ---*/
  if (ionization) { nHeavy = nSpecies-1; nEl = 1; }
  else            { nHeavy = nSpecies;   nEl = 0; }
  
  /*--- Read gas mixture properties from config ---*/
  Ms = config->GetMolar_Mass();
  
  /*--- Rename for convenience ---*/
  Ru = UNIVERSAL_GAS_CONSTANT;
  
  /*--- Solve for mixture pressure using ideal gas law & Dalton's law ---*/
  // Note: If free electrons are present, use Tve for their partial pressure
  P = 0.0;
  for(iSpecies = 0; iSpecies < nHeavy; iSpecies++)
    P += Solution[iSpecies] * Ru/Ms[iSpecies] * Primitive[T_INDEX];
  
  for (iSpecies = 0; iSpecies < nEl; iSpecies++)
    P += Solution[nSpecies-1] * Ru/Ms[nSpecies-1] * Primitive[TVE_INDEX];
  
  /*--- Store computed values and check for a physical solution ---*/
  Primitive[P_INDEX] = P;
  if (Primitive[P_INDEX] > 0.0) return false;
  else return true;
}

bool CTNE2EulerVariable::SetSoundSpeed(CConfig *config) {
  
  // NOTE: Requires SetDensity(), SetTemperature(), SetPressure(), & SetGasProperties().
  
  unsigned short iSpecies, nHeavy, nEl;
  double dPdrhoE, factor, Ru, radical;
  double *Ms, *xi;
  
  /*--- Determine the number of heavy species ---*/
  if (ionization) { nHeavy = nSpecies-1; nEl = 1; }
  else            { nHeavy = nSpecies;   nEl = 0; }
  
  /*--- Read gas mixture properties from config ---*/
  Ms = config->GetMolar_Mass();
  xi = config->GetRotationModes();
  
  /*--- Rename for convenience ---*/
  Ru = UNIVERSAL_GAS_CONSTANT;
  
  /*--- Calculate partial derivative of pressure w.r.t. rhoE ---*/
  factor = 0.0;
  for (iSpecies = 0; iSpecies < nHeavy; iSpecies++)
    factor += Solution[iSpecies] / Ms[iSpecies];
  dPdrhoE = Ru/Primitive[RHOCVTR_INDEX] * factor;
  
  /*--- Calculate a^2 using Gnoffo definition (NASA TP 2867) ---*/
  radical = (1.0+dPdrhoE) * Primitive[P_INDEX]/Primitive[RHO_INDEX];
  
  if (radical < 0.0) return true;
  else { Primitive[A_INDEX] = sqrt(radical); return false; }
}

void CTNE2EulerVariable::SetdPdrhos(CConfig *config) {

  // Note: Requires SetDensity(), SetTemperature(), SetPressure(), & SetGasProperties()
  // Note: Electron energy not included properly.
  
  unsigned short iDim, iSpecies, iEl, nHeavy, nEl, *nElStates;
  double *Ms, *Tref, *hf, *xi, *thetav, **thetae, **g;
  double Ru, RuBAR, CvtrBAR, rhoCvtr, rhoCvve, Cvtrs, rho_el, sqvel, conc;
  double rho, rhos, rhoEve, T, Tve, evibs, eels, ef;
  double num, denom;
  
  /*--- Determine the number of heavy species ---*/
  if (ionization) {
    nHeavy = nSpecies-1;
    nEl    = 1;
    rho_el = Primitive[RHOS_INDEX+nSpecies-1];
  } else {
    nHeavy = nSpecies;
    nEl    = 0;
    rho_el = 0.0;
  }
  
  /*--- Read gas mixture properties from config ---*/
  Ms        = config->GetMolar_Mass();
  Tref      = config->GetRefTemperature();
  hf        = config->GetEnthalpy_Formation();
  xi        = config->GetRotationModes();
  thetav    = config->GetCharVibTemp();
  thetae    = config->GetCharElTemp();
  g         = config->GetElDegeneracy();
  nElStates = config->GetnElStates();
  
  /*--- Rename for convenience ---*/
  Ru      = UNIVERSAL_GAS_CONSTANT;
  T       = Primitive[T_INDEX];
  Tve     = Primitive[TVE_INDEX];
  rho     = Primitive[RHO_INDEX];
  rhoEve  = Solution[nSpecies+nDim+1];
  rhoCvtr = Primitive[RHOCVTR_INDEX];
  rhoCvve = Primitive[RHOCVVE_INDEX];
  
  /*--- Pre-compute useful quantities ---*/
  RuBAR   = 0.0;
  CvtrBAR = 0.0;
  sqvel   = 0.0;
  for (iDim = 0; iDim < nDim; iDim++)
    sqvel += Primitive[VEL_INDEX+iDim] * Primitive[VEL_INDEX+iDim];
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    CvtrBAR += Primitive[RHOS_INDEX+iSpecies]*(3.0/2.0 + xi[iSpecies]/2.0)*Ru/Ms[iSpecies];
  }

  
  
  conc  = 0.0;
  sqvel = 0.0;
  for (iDim = 0; iDim < nDim; iDim++)
    sqvel += Primitive[VEL_INDEX+iDim] * Primitive[VEL_INDEX+iDim];
  for (iSpecies = 0; iSpecies < nHeavy; iSpecies++)
    conc += Primitive[RHOS_INDEX+iSpecies]/Ms[iSpecies];
  
  for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
    rhos  = Primitive[RHOS_INDEX+iSpecies];
    ef    = hf[iSpecies] - Ru/Ms[iSpecies]*Tref[iSpecies];
    Cvtrs = (3.0/2.0 + xi[iSpecies]/2.0)*Ru/Ms[iSpecies];
    
    dPdrhos[iSpecies] =  T*Ru/Ms[iSpecies]
                       + Ru*conc/rhoCvtr * (-Cvtrs*(T-Tref[iSpecies]) - ef + 0.5*sqvel);
  }
  if (ionization) {
    for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
      evibs = Ru/Ms[iSpecies] * thetav[iSpecies] / (exp(thetav[iSpecies]/Tve) - 1.0);
      num = 0.0;
      denom = g[iSpecies][0] * exp(thetae[iSpecies][0]/Tve);
      for (iEl = 1; iEl < nElStates[iSpecies]; iEl++) {
        num   += g[iSpecies][iEl] * thetae[iSpecies][iEl] * exp(-thetae[iSpecies][iEl]/Tve);
        denom += g[iSpecies][iEl] * exp(-thetae[iSpecies][iEl]/Tve);
      }
      eels = Ru/Ms[iSpecies] * (num/denom);
      
      dPdrhos[iSpecies] -= rho_el * Ru/Ms[nSpecies-1] * (evibs + eels)/rhoCvve;
    }
    ef = hf[nSpecies-1] - Ru/Ms[nSpecies-1]*Tref[nSpecies-1];
    dPdrhos[nSpecies-1] =  Ru*conc/rhoCvtr * (-ef + 0.5*sqvel)
                         + Ru/Ms[nSpecies-1]*Tve
                         - rho_el*Ru/Ms[nSpecies-1] * (-3.0/2.0 * Ru/Ms[nSpecies-1] * Tve)/rhoCvve;
  }
}


bool CTNE2EulerVariable::SetPrimVar_Compressible(CConfig *config) {
	unsigned short iVar, iSpecies;
  bool check_dens, check_press, check_sos, check_temp;
  
  /*--- Initialize booleans that check for physical solutions ---*/
  check_dens  = false;
  check_press = false;
  check_sos   = false;
  check_temp  = false;
  
  /*--- Calculate primitive variables ---*/
  // Solution:  [rho1, ..., rhoNs, rhou, rhov, rhow, rhoe, rhoeve]^T
  // Primitive: [rho1, ..., rhoNs, T, Tve, u, v, w, P, rho, h, a, rhoCvtr, rhoCvve]^T
  // GradPrim:  [rho1, ..., rhoNs, T, Tve, u, v, w, P]^T
  SetDensity();                             // Compute species & mixture density
	SetVelocity2();                           // Compute the square of the velocity (req. mixture density).
  
  /*--- Calculate velocities (req. density calculation) ---*/
//	for (iDim = 0; iDim < nDim; iDim++)
//		Primitive[nSpecies+iDim+2] = Solution[nSpecies+iDim] / Primitive[nSpecies+nDim+3];
  
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
    check_dens = ((Solution[iSpecies] < 0.0) || check_dens);  // Check the density
  check_temp  = SetTemperature(config);     // Compute temperatures (T & Tve) (req. mixture density).
 	check_press = SetPressure(config);        // Requires T & Tve computation.
	check_sos   = SetSoundSpeed(config);      // Requires density, pressure, rhoCvtr, & rhoCvve.
  SetdPdrhos(config);                       // Requires density, pressure, rhoCvtr, & rhoCvve.  
  
  /*--- Check that the solution has a physical meaning ---*/
  if (check_dens || check_press || check_sos || check_temp) {
    
    /*--- Copy the old solution ---*/
    for (iVar = 0; iVar < nVar; iVar++)
      Solution[iVar] = Solution_Old[iVar];
    
    /*--- Recompute the primitive variables ---*/
    SetDensity();                           // Compute mixture density
    SetVelocity2();                         // Compute square of the velocity (req. mixture density).
    check_temp  = SetTemperature(config);   // Compute temperatures (T & Tve)
    check_press = SetPressure(config);      // Requires T & Tve computation.
    check_sos   = SetSoundSpeed(config);    // Requires density & pressure computation.
  }
  SetEnthalpy();                            // Requires density & pressure computation.
  
  return true;
}

CTNE2NSVariable::CTNE2NSVariable(void) : CTNE2EulerVariable() { }

CTNE2NSVariable::CTNE2NSVariable(double val_density, double *val_massfrac, double *val_velocity,
                                 double val_temperature, double val_energy_ve,
                                 unsigned short val_ndim, unsigned short val_nvar,
                                 unsigned short val_nvarprim, unsigned short val_nvarprimgrad,
                                 CConfig *config) : CTNE2EulerVariable(val_density, val_massfrac, val_velocity,
                                                                       val_temperature, val_energy_ve, val_ndim,
                                                                       val_nvar, val_nvarprim, val_nvarprimgrad,
                                                                       config) {
  
	Temperature_Ref = config->GetTemperature_Ref();
	Viscosity_Ref   = config->GetViscosity_Ref();
	Viscosity_Inf   = config->GetViscosity_FreeStreamND();
	Prandtl_Lam     = config->GetPrandtl_Lam();

}

CTNE2NSVariable::CTNE2NSVariable(double *val_solution, unsigned short val_ndim,
                                 unsigned short val_nvar, unsigned short val_nvarprim,
                                 unsigned short val_nvarprimgrad, CConfig *config) : CTNE2EulerVariable(val_solution, val_ndim, val_nvar, val_nvarprim, val_nvarprimgrad, config) {
  
	Temperature_Ref = config->GetTemperature_Ref();
	Viscosity_Ref   = config->GetViscosity_Ref();
	Viscosity_Inf   = config->GetViscosity_FreeStreamND();
	Prandtl_Lam     = config->GetPrandtl_Lam();
  
}

CTNE2NSVariable::~CTNE2NSVariable(void) { }

void CTNE2NSVariable::SetLaminarViscosity() {
	double Temperature_Dim;
  
	/*--- Calculate viscosity from a non-dim. Sutherland's Law ---*/
	Temperature_Dim = Primitive[0]*Temperature_Ref;
	LaminarViscosity = 1.853E-5*(pow(Temperature_Dim/300.0,3.0/2.0) * (300.0+110.3)/(Temperature_Dim+110.3));
	LaminarViscosity = LaminarViscosity/Viscosity_Ref;
  
}

void CTNE2NSVariable::SetVorticity(void) {
	double u_y = Gradient_Primitive[1][1];
	double v_x = Gradient_Primitive[2][0];
	double u_z = 0.0;
	double v_z = 0.0;
	double w_x = 0.0;
	double w_y = 0.0;
  
	if (nDim == 3) {
		u_z = Gradient_Primitive[1][2];
		v_z = Gradient_Primitive[2][2];
		w_x = Gradient_Primitive[3][0];
		w_y = Gradient_Primitive[3][1];
	}
  
	Vorticity[0] = w_y-v_z;
	Vorticity[1] = -(w_x-u_z);
	Vorticity[2] = v_x-u_y;
  
}

bool CTNE2NSVariable::SetPressure(CConfig *config) {
  
  /////////////////////////////////////////////////////////
  // SUBTRACT TURBULENT KINETIC ENERGY FROM PRESSURE???? //
  /////////////////////////////////////////////////////////
  
  unsigned short iSpecies;
  double *molarmass;
  double P;
  
  molarmass = config->GetMolar_Mass();
  P = 0.0;
  for(iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    P += Solution[iSpecies]*UNIVERSAL_GAS_CONSTANT/molarmass[iSpecies] * Primitive[nSpecies];
  }
  if (ionization) {
    iSpecies = nSpecies - 1;
    P -= Solution[iSpecies]*UNIVERSAL_GAS_CONSTANT/molarmass[iSpecies] * Primitive[nSpecies];
    P += Solution[iSpecies]*UNIVERSAL_GAS_CONSTANT/molarmass[iSpecies] * Primitive[nSpecies+1];
  }
  
  Primitive[nSpecies+nDim+2] = P;
  
  if (Primitive[nSpecies+nDim+2] > 0.0) return false;
  else return true;
}

bool CTNE2NSVariable::SetPrimVar_Compressible(CConfig *config) {
	unsigned short iDim, iVar, iSpecies;
  bool check_dens = false, check_press = false, check_sos = false, check_temp = false;
  
  /*--- Primitive variables [rho1,...,rhoNs,T,Tve,u,v,w,P,rho,h,c] ---*/
  
  SetDensity();                             // Compute mixture density
	SetVelocity2();                           // Compute the modulus of the velocity (req. mixture density).
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
    check_dens = ((Solution[iSpecies] < 0.0) || check_dens);  // Check the density
  check_temp  = SetTemperature(config);     // Compute temperatures (T & Tve)
 	check_press = SetPressure(config);        // Requires T & Tve computation.
	check_sos   = SetSoundSpeed(config);      // Requires density & pressure computation.
  
  /*--- Check that the solution has a physical meaning ---*/
  if (check_dens || check_press || check_sos || check_temp) {
    
    /*--- Copy the old solution ---*/
    for (iVar = 0; iVar < nVar; iVar++)
      Solution[iVar] = Solution_Old[iVar];
    
    /*--- Recompute the primitive variables ---*/
    SetDensity();                           // Compute mixture density
    SetVelocity2();                         // Compute square of the velocity (req. mixture density).
    check_temp  = SetTemperature(config);   // Compute temperatures (T & Tve)
    check_press = SetPressure(config);      // Requires T & Tve computation.
    check_sos   = SetSoundSpeed(config);    // Requires density & pressure computation.
  }
  SetEnthalpy();                            // Requires density & pressure computation.
  SetLaminarViscosity();                    // Requires temperature computation.
  
  /*--- Calculate velocities (req. density calculation) ---*/
	for (iDim = 0; iDim < nDim; iDim++)
		Primitive[nSpecies+iDim+2] = Solution[nSpecies+iDim] / Primitive[nSpecies+nDim+3];
  
  return true;
}
