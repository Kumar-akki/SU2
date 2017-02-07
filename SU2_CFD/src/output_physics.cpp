/*!
 * \file output_physics.cpp
 * \brief Main subroutines to compute physical output quantities such as CL, CD, entropy generation, mass flow, ecc... .
 * \author S. Vitale
 * \version 5.0.0 "Raven"
 *
 * SU2 Lead Developers: Dr. Francisco Palacios (Francisco.D.Palacios@boeing.com).
 *                      Dr. Thomas D. Economon (economon@stanford.edu).
 *
 * SU2 Developers: Prof. Juan J. Alonso's group at Stanford University.
 *                 Prof. Piero Colonna's group at Delft University of Technology.
 *                 Prof. Nicolas R. Gauger's group at Kaiserslautern University of Technology.
 *                 Prof. Alberto Guardone's group at Polytechnic University of Milan.
 *                 Prof. Rafael Palacios' group at Imperial College London.
 *                 Prof. Edwin van der Weide's group at the University of Twente.
 *                 Prof. Vincent Terrapon's group at the University of Liege.
 *
 * Copyright (C) 2012-2017 SU2, the open-source CFD code.
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

#include "../include/output_structure.hpp"


void COutput::ComputeTurboPerformance(CSolver *solver_container, CGeometry *geometry, CConfig *config) {

	CFluidModel *FluidModel;
	unsigned short nDim = geometry->GetnDim();
  unsigned short iMarkerTP, iSpan, iDim, iStage, iBlade;
	unsigned short nMarkerTP = config->GetnMarker_Turbomachinery();
	FluidModel = solver_container->GetFluidModel();
  su2double area, absVel2, soundSpeed, mach, tangVel, tangVel2, *relVel, relVel2;
  su2double relPressureIn, relPressureOut, enthalpyOutIs, relVelOutIs2;
  relVel = new su2double[nDim];

  unsigned short nBladesRow, nStages;

  nBladesRow = config->GetnMarker_Turbomachinery();
  nStages    = SU2_TYPE::Int(nBladesRow/2);


  /*--- Compute BC imposed value for convergence monitoring ---*/
  for(iMarkerTP = 0; iMarkerTP < nMarkerTP; iMarkerTP++ ){
  	for(iSpan = 0; iSpan < config->GetnSpan_iZones(iMarkerTP) + 1; iSpan++){
  		FluidModel->SetTDState_PT(config->GetTotalPressureIn_BC(), config->GetTotalTemperatureIn_BC());
  		TotalEnthalpyIn_BC[iMarkerTP][iSpan] = FluidModel->GetStaticEnergy()+ FluidModel->GetPressure()/FluidModel->GetDensity();
  		EntropyIn_BC[iMarkerTP][iSpan] = FluidModel->GetEntropy();
  	}
  }

	/*--- Compute performance for each blade ---*/
	for(iMarkerTP = 0; iMarkerTP < nMarkerTP; iMarkerTP++ ){
		for(iSpan = 0; iSpan < config->GetnSpan_iZones(iMarkerTP) + 1; iSpan++){


			/*--- INFLOW ---*/
			/*--- Retrieve Inflow primitive quantities ---*/
			DensityIn[iMarkerTP][iSpan]          = solver_container->GetDensityIn(iMarkerTP, iSpan);
			PressureIn[iMarkerTP][iSpan]         = solver_container->GetPressureIn(iMarkerTP, iSpan);

			absVel2 = 0.0;

			for (iDim = 0; iDim < nDim; iDim++){
        TurboVelocityIn[iMarkerTP][iSpan][iDim]    = solver_container->GetTurboVelocityIn(iMarkerTP, iSpan)[iDim];
        absVel2   += TurboVelocityIn[iMarkerTP][iSpan][iDim]*TurboVelocityIn[iMarkerTP][iSpan][iDim];
      }
			TurboVelocityIn[iMarkerTP][iSpan][nDim] = sqrt(absVel2);

			TRadius[iMarkerTP][iSpan]            = geometry->GetTurboRadiusIn(iMarkerTP, iSpan);
      area																 = geometry->GetSpanAreaIn(iMarkerTP, iSpan);

			/*--- Compute static Inflow quantities ---*/
			FluidModel->SetTDState_Prho(PressureIn[iMarkerTP][iSpan], DensityIn[iMarkerTP][iSpan]);
			EntropyIn[iMarkerTP][iSpan]					 = FluidModel->GetEntropy();
			MassFlowIn[iMarkerTP][iSpan]         = config->GetnBlades(iMarkerTP)*DensityIn[iMarkerTP][iSpan]*TurboVelocityIn[iMarkerTP][iSpan][0]*area;
			AbsFlowAngleIn[iMarkerTP][iSpan]     = atan(TurboVelocityIn[iMarkerTP][iSpan][1]/TurboVelocityIn[iMarkerTP][iSpan][0]);
			EnthalpyIn[iMarkerTP][iSpan]         = FluidModel->GetStaticEnergy() + PressureIn[iMarkerTP][iSpan]/DensityIn[iMarkerTP][iSpan];
			soundSpeed                           = FluidModel->GetSoundSpeed();


			/*--- Compute Total Inflow quantities ---*/
			TotalEnthalpyIn[iMarkerTP][iSpan]    = EnthalpyIn[iMarkerTP][iSpan] + 0.5*absVel2;
			FluidModel->SetTDState_hs(TotalEnthalpyIn[iMarkerTP][iSpan], EntropyIn[iMarkerTP][iSpan]);
			TotalPressureIn[iMarkerTP][iSpan]    = FluidModel->GetPressure();
			TotalTemperatureIn[iMarkerTP][iSpan] = FluidModel->GetTemperature();

			/*--- Retrieve Inflow relative quantities ---*/
			tangVel = geometry->GetTangGridVelIn(iMarkerTP, iSpan);
			tangVel2 = tangVel*tangVel;

			for (iDim = 0; iDim < nDim; iDim++){
				relVel[iDim] = TurboVelocityIn[iMarkerTP][iSpan][iDim];
			}
			relVel[1] -= tangVel;

			relVel2 = 0.0;
			for (iDim = 0; iDim < nDim; iDim++){
				relVel2 += relVel[iDim]*relVel[iDim];
			}

			/*--- Compute Total relative Inflow quantities ---*/
			RothalpyIn[iMarkerTP][iSpan]  = EnthalpyIn[iMarkerTP][iSpan] + 0.5*relVel2 - 0.5*tangVel2;
			FluidModel->SetTDState_hs(RothalpyIn[iMarkerTP][iSpan], EntropyIn[iMarkerTP][iSpan]);
			relPressureIn   = FluidModel->GetPressure();

			/*--- Compute kinematic relative Inflow quantities ---*/
			FlowAngleIn[iMarkerTP][iSpan]   = atan(relVel[1]/relVel[0]);
			mach          = 0.0;
			for (iDim = 0; iDim < nDim; iDim++){
				MachIn[iMarkerTP][iSpan][iDim]       = relVel[iDim]/soundSpeed;
				mach 																 = MachIn[iMarkerTP][iSpan][iDim]*MachIn[iMarkerTP][iSpan][iDim];
			}
			MachIn[iMarkerTP][iSpan][nDim]            = sqrt(mach);



			/*--- OUTFLOW ---*/
			/*--- Retrieve Outflow primitive quantities ---*/
			DensityOut[iMarkerTP][iSpan]         = solver_container->GetDensityOut(iMarkerTP, iSpan);
			PressureOut[iMarkerTP][iSpan]         = solver_container->GetPressureOut(iMarkerTP, iSpan);
			absVel2 = 0.0;

			for (iDim = 0; iDim < nDim; iDim++){
			  TurboVelocityOut[iMarkerTP][iSpan][iDim]    = solver_container->GetTurboVelocityOut(iMarkerTP, iSpan)[iDim];
			  absVel2   += TurboVelocityOut[iMarkerTP][iSpan][iDim]*TurboVelocityOut[iMarkerTP][iSpan][iDim];
      }
			TurboVelocityOut[iMarkerTP][iSpan][nDim] = sqrt(absVel2);


			for (iDim = 0; iDim < 3; iDim++){
      }
//			TRadius[iMarkerTP][iSpan]            = geometry->GetTurboRadiusIn(iMarkerTP, iSpan);
			area																 = geometry->GetSpanAreaOut(iMarkerTP, iSpan);


			/*--- Compute all the Outflow quantities ---*/
			FluidModel->SetTDState_Prho(PressureOut[iMarkerTP][iSpan], DensityOut[iMarkerTP][iSpan]);
			EntropyOut[iMarkerTP][iSpan]				 = FluidModel->GetEntropy();
			MassFlowOut[iMarkerTP][iSpan]         = config->GetnBlades(iMarkerTP)*DensityOut[iMarkerTP][iSpan]*TurboVelocityOut[iMarkerTP][iSpan][0]*area;
			AbsFlowAngleOut[iMarkerTP][iSpan]     = atan(TurboVelocityOut[iMarkerTP][iSpan][1]/TurboVelocityOut[iMarkerTP][iSpan][0]);
			EnthalpyOut[iMarkerTP][iSpan]         = FluidModel->GetStaticEnergy() + PressureOut[iMarkerTP][iSpan]/DensityOut[iMarkerTP][iSpan];
			soundSpeed                           = FluidModel->GetSoundSpeed();

			/*--- Compute Total Outflow quantities ---*/
			TotalEnthalpyOut[iMarkerTP][iSpan]    = EnthalpyOut[iMarkerTP][iSpan] + 0.5*absVel2;
			FluidModel->SetTDState_hs(TotalEnthalpyOut[iMarkerTP][iSpan], EntropyOut[iMarkerTP][iSpan]);
			TotalPressureOut[iMarkerTP][iSpan]    = FluidModel->GetPressure();
			TotalTemperatureOut[iMarkerTP][iSpan] = FluidModel->GetTemperature();

			/*--- Retrieve relative Outflow  quantities ---*/
			tangVel = geometry->GetTangGridVelOut(iMarkerTP, iSpan);
			tangVel2 = tangVel*tangVel;

			for (iDim = 0; iDim < nDim; iDim++){
				relVel[iDim] = TurboVelocityOut[iMarkerTP][iSpan][iDim];
			}
			relVel[1] -= tangVel;

			relVel2 = 0.0;
			for (iDim = 0; iDim < nDim; iDim++){
				relVel2 += relVel[iDim]*relVel[iDim];
			}

			/*--- Compute Total relative Outflow quantities ---*/
			RothalpyOut[iMarkerTP][iSpan] = EnthalpyOut[iMarkerTP][iSpan] + 0.5*relVel2 - 0.5*tangVel2;
			FluidModel->SetTDState_hs(RothalpyOut[iMarkerTP][iSpan], EntropyOut[iMarkerTP][iSpan]);
			relPressureOut    = FluidModel->GetPressure();

			/*--- Compute isentropic Outflow quantities ---*/
			FluidModel->SetTDState_Ps(PressureOut[iMarkerTP][iSpan], EntropyIn[iMarkerTP][iSpan]);
			enthalpyOutIs = FluidModel->GetStaticEnergy() + PressureOut[iMarkerTP][iSpan]/FluidModel->GetDensity();
      relVelOutIs2  = 2*(RothalpyOut[iMarkerTP][iSpan] - enthalpyOutIs) + tangVel2;


			/*--- Compute kinematic relative Outflow quantities ---*/
			FlowAngleOut[iMarkerTP][iSpan]   = atan(relVel[1]/relVel[0]);
			mach          = 0.0;
			for (iDim = 0; iDim < nDim; iDim++){
				MachOut[iMarkerTP][iSpan][iDim]       = relVel[iDim]/soundSpeed;
				mach 																 = MachOut[iMarkerTP][iSpan][iDim]*MachOut[iMarkerTP][iSpan][iDim];
			}
			MachOut[iMarkerTP][iSpan][nDim]            = sqrt(mach);



			/*--- TURBO-PERFORMANCE---*/
			EntropyGen[iMarkerTP][iSpan]				 = (EntropyOut[iMarkerTP][iSpan] - EntropyIn[iMarkerTP][iSpan])/abs(EntropyIn_BC[iMarkerTP][iSpan] + 1);
			EulerianWork[iMarkerTP][iSpan]       = TotalEnthalpyIn[iMarkerTP][iSpan] - TotalEnthalpyOut[iMarkerTP][iSpan];
			TotalPressureLoss[iMarkerTP][iSpan]  = (relPressureIn - relPressureOut)/(relPressureIn - PressureOut[iMarkerTP][iSpan]);
			KineticEnergyLoss[iMarkerTP][iSpan]  = 2*(EnthalpyOut[iMarkerTP][iSpan] - enthalpyOutIs)/relVelOutIs2;

		}
	}

	if(nBladesRow > 1){
		/*--- Compute performance for each stage ---*/

		EulerianWork[nBladesRow + nStages][nSpanWiseSections]           = 0.0;
		/*---Comnpute performance for each stage---*/
		for(iStage = 0; iStage < nStages; iStage++ ){
			FluidModel->SetTDState_Ps(PressureOut[iStage*2 +1][config->GetnSpan_iZones(iStage*2 +1)], EntropyIn[iStage*2][config->GetnSpan_iZones(iStage*2)]);
			EnthalpyOutIs[nBladesRow + iStage][nSpanWiseSections]         = FluidModel->GetStaticEnergy() + PressureOut[iStage*2 +1][config->GetnSpan_iZones(iStage*2 +1)]/FluidModel->GetDensity();
			FluidModel->SetTDState_Prho(PressureOut[iStage*2 +1][config->GetnSpan_iZones(iStage*2 +1)], DensityOut[iStage*2 +1][config->GetnSpan_iZones(iStage*2 +1)]);
			absVel2 = 0.0;
			for (iDim = 0; iDim<nDim; iDim++)
				absVel2 += MachOut[iStage*2 +1][config->GetnSpan_iZones(iStage*2 +1)][iDim]*MachOut[iStage*2 +1][config->GetnSpan_iZones(iStage*2 +1)][iDim];
			absVel2 *= FluidModel->GetSoundSpeed2();
			TotalEnthalpyOutIs[nBladesRow + iStage][nSpanWiseSections]    = EnthalpyOutIs[nBladesRow + iStage][nSpanWiseSections] + 0.5*absVel2;

			TotalTotalEfficiency[nBladesRow + iStage][nSpanWiseSections]  = (TotalEnthalpyIn[iStage*2][config->GetnSpan_iZones(iStage*2)] - TotalEnthalpyOut[iStage*2 + 1][config->GetnSpan_iZones(iStage*2 +1)]);
			TotalTotalEfficiency[nBladesRow + iStage][nSpanWiseSections]  /= (TotalEnthalpyIn[iStage*2][config->GetnSpan_iZones(iStage*2)] - TotalEnthalpyOutIs[nBladesRow + iStage][nSpanWiseSections]);
			TotalStaticEfficiency[nBladesRow + iStage][nSpanWiseSections] = (TotalEnthalpyIn[iStage*2][config->GetnSpan_iZones(iStage*2)] - TotalEnthalpyOut[iStage*2 + 1][config->GetnSpan_iZones(iStage*2+1)]);
			TotalStaticEfficiency[nBladesRow + iStage][nSpanWiseSections] /= (TotalEnthalpyIn[iStage*2][config->GetnSpan_iZones(iStage*2)] - EnthalpyOutIs[nBladesRow + iStage][nSpanWiseSections]);
			PressureRatio[nBladesRow + iStage][nSpanWiseSections]         = (PressureRatio[iStage*2][config->GetnSpan_iZones(iStage*2)]*PressureOut[iStage*2][config->GetnSpan_iZones(iStage*2)]/PressureOut[iStage*2 + 1][config->GetnSpan_iZones(iStage*2+1)]);
			MassFlowIn[nBladesRow + iStage][nSpanWiseSections]            = MassFlowIn[iStage*2][config->GetnSpan_iZones(iStage*2)];
			MassFlowOut[nBladesRow + iStage][nSpanWiseSections]           = MassFlowOut[iStage*2 + 1][config->GetnSpan_iZones(iStage*2+1)];
			EntropyGen[nBladesRow + iStage][nSpanWiseSections]            = EntropyGen[iStage*2 + 1][config->GetnSpan_iZones(iStage*2 +1)] + EntropyGen[iStage*2][config->GetnSpan_iZones(iStage*2)];

		}

		/*---Compute turbo performance for full machine---*/
		FluidModel->SetTDState_Ps(PressureOut[nBladesRow-1][config->GetnSpan_iZones(nBladesRow-1)], EntropyIn[0][config->GetnSpan_iZones(0)]);
		EnthalpyOutIs[nBladesRow + nStages][nSpanWiseSections]          = FluidModel->GetStaticEnergy() + PressureOut[nBladesRow-1][config->GetnSpan_iZones(nBladesRow-1)]/FluidModel->GetDensity();
		FluidModel->SetTDState_Prho(PressureOut[nBladesRow-1][config->GetnSpan_iZones(nBladesRow-1)], DensityOut[nBladesRow-1][config->GetnSpan_iZones(nBladesRow-1)]);
		absVel2 = 0.0;
		for (iDim = 0; iDim<nDim;iDim++) absVel2 += MachOut[nBladesRow-1][config->GetnSpan_iZones(nBladesRow-1)][iDim]*MachOut[nBladesRow-1][config->GetnSpan_iZones(nBladesRow-1)][iDim];
		absVel2 *= FluidModel->GetSoundSpeed2();
		TotalEnthalpyOutIs[nBladesRow + nStages][nSpanWiseSections]     = EnthalpyOutIs[nBladesRow + nStages][nSpanWiseSections] + 0.5*absVel2;

		TotalTotalEfficiency[nBladesRow + nStages][nSpanWiseSections]   = (TotalEnthalpyIn[0][config->GetnSpan_iZones(0)] - TotalEnthalpyOut[nBladesRow-1][config->GetnSpan_iZones(nBladesRow-1)]);
		TotalTotalEfficiency[nBladesRow + nStages][nSpanWiseSections]	 /= (TotalEnthalpyIn[0][config->GetnSpan_iZones(0)] - TotalEnthalpyOutIs[nBladesRow + nStages][nSpanWiseSections]);
		TotalStaticEfficiency[nBladesRow +nStages][nSpanWiseSections]   = (TotalEnthalpyIn[0][config->GetnSpan_iZones(0)] - TotalEnthalpyOut[nBladesRow-1][config->GetnSpan_iZones(nBladesRow-1)]);
		TotalStaticEfficiency[nBladesRow +nStages][nSpanWiseSections]  /= (TotalEnthalpyIn[0][config->GetnSpan_iZones(0)] - EnthalpyOutIs[nBladesRow + nStages][nSpanWiseSections]);
		PressureRatio[nBladesRow + nStages][nSpanWiseSections]          = PressureRatio[0][config->GetnSpan_iZones(0)]*PressureOut[0][config->GetnSpan_iZones(0)]/PressureOut[nBladesRow-1][config->GetnSpan_iZones(nBladesRow-1)];
		MassFlowIn[nBladesRow + nStages][nSpanWiseSections]             = MassFlowIn[0][config->GetnSpan_iZones(0)];
		MassFlowOut[nBladesRow + nStages][nSpanWiseSections]            = MassFlowOut[nBladesRow-1][config->GetnSpan_iZones(nBladesRow-1)];

		EntropyGen[nBladesRow + nStages][nSpanWiseSections]     = 0.0;
		for(iBlade = 0; iBlade < nBladesRow; iBlade++ ){
			EntropyGen[nBladesRow + nStages][nSpanWiseSections]  += EntropyGen[iBlade][config->GetnSpan_iZones(iBlade)];
		}
	}

	delete [] relVel;
}


su2double COutput::GetEntropyGen(unsigned short iMarkerTP, unsigned short iSpan){return EntropyGen[iMarkerTP][iSpan];}




