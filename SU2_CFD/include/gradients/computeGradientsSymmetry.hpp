/*!
 * \file computeGradientsSymmetry.hpp
 * \brief Implements the symmetry boundary conditions for the gradient computations.
 * \author N. Beishuizen
 * \version 8.0.1 "Harrier"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2024, SU2 Contributors (cf. AUTHORS.md)
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

#include <vector>
#include <algorithm>

#include "../../../Common/include/parallelization/omp_structure.hpp"
#include "../../../Common/include/toolboxes/geometry_toolbox.hpp"


/*!
 * \brief Reflect a gradient using a tensor mapping. Used for symmetry reflection.
 * \ingroup FvmAlgos
 * \param[in] nDim - number of dimensions, 2 or 3.
 * \param[in] varBegin - start of the variables.
 * \param[in] varEnd - end of the variables.
 * \param[in] isFlowSolver - are we using the flow solver.
 * \param[in] TensorMAp - the tensor map to map to rotated base.
 * \param[out] Gradients_iPoint - the gradient for the point.
 */


template <typename Int, class Matrix>
inline void ReflectGradient(Int nDim, size_t& varBegin, size_t& varEnd, bool isFlowSolver, Matrix& TensorMap,
                            Matrix& Gradients_iPoint) {
  static constexpr size_t MAXNDIM = 3;

  su2activematrix Gradients_Velocity(nDim, nDim);
  su2activematrix Gradients_Velocity_Reflected(nDim, nDim);
  su2double gradPhi[MAXNDIM] = {0.0};
  su2double gradPhiReflected[MAXNDIM] = {0.0};

  if (isFlowSolver == true) {
    /*--- Get gradients of primitives of boundary cell ---*/
    for (auto iVar = 0u; iVar < nDim; iVar++) {
      for (auto iDim = 0u; iDim < nDim; iDim++) {
        // todo: 1 ->idx.velocity
        Gradients_Velocity[iVar][iDim] = Gradients_iPoint[1 + iVar][iDim];
        Gradients_Velocity_Reflected[iVar][iDim] = 0.0;
      }
    }

    /*--- Q' = L^T*Q*T ---*/
    for (auto iDim = 0u; iDim < nDim; iDim++) {
      for (auto jDim = 0u; jDim < nDim; jDim++) {
        for (auto kDim = 0u; kDim < nDim; kDim++) {
          for (auto mDim = 0u; mDim < nDim; mDim++) {
            Gradients_Velocity_Reflected[iDim][jDim] +=
                TensorMap[iDim][mDim] * TensorMap[jDim][kDim] * Gradients_Velocity[mDim][kDim];
          }
        }
      }
    }

    /*--- we have aligned such that U is the direction of the normal
     *    in 2D: dU/dy = dV/dx = 0
     *    in 3D: dU/dy = dV/dx = 0
     *           dU/dz = dW/dx = 0 ---*/
    for (auto iDim = 1u; iDim < nDim; iDim++) {
      Gradients_Velocity_Reflected[0][iDim] = 0.0;
      Gradients_Velocity_Reflected[iDim][0] = 0.0;
    }

    for (auto iDim = 0u; iDim < nDim; iDim++) {
      for (auto jDim = 0u; jDim < nDim; jDim++) {
        Gradients_Velocity[iDim][jDim] = 0.0;
      }
    }

    /*--- now transform back the corrected velocity gradients by taking the inverse again
     * T = (L^-1)*T' ---*/
    for (auto iDim = 0u; iDim < nDim; iDim++) {
      for (auto jDim = 0u; jDim < nDim; jDim++) {
        for (auto kDim = 0u; kDim < nDim; kDim++) {
          for (auto mDim = 0u; mDim < nDim; mDim++) {
            Gradients_Velocity[iDim][jDim] +=
                TensorMap[mDim][iDim] * TensorMap[kDim][jDim] * Gradients_Velocity_Reflected[mDim][kDim];
          }
        }
      }
    }

    for (auto iDim = 0u; iDim < nDim; iDim++) {
      for (auto jDim = 0u; jDim < nDim; jDim++) {
        // todo: 1->idx.velocity
        Gradients_iPoint[iDim + 1][jDim] = Gradients_Velocity[iDim][jDim];
      }
    }
  }

  /*--- Reflect the gradients for all scalars (we exclude velocity). --*/
  for (auto iVar = varBegin; iVar < varEnd; iVar++) {
    if ((isFlowSolver == false) || ((isFlowSolver == true) && (iVar == 0 || iVar > nDim))) {
      /*--- project to symmetry aligned base ---*/
      for (auto iDim = 0u; iDim < nDim; iDim++) {
        gradPhi[iDim] = Gradients_iPoint[iVar][iDim];
        gradPhiReflected[iDim] = 0.0;
      }

      for (auto jDim = 0u; jDim < nDim; jDim++) {
        for (auto iDim = 0u; iDim < nDim; iDim++) {
          /*--- map transpose T' * grad(phi) ---*/
          gradPhiReflected[jDim] += TensorMap[jDim][iDim] * Gradients_iPoint[iVar][iDim];
        }
      }

      for (auto iDim = 0u; iDim < nDim; iDim++) gradPhi[iDim] = 0.0;

      /*--- gradient in direction normal to symmetry is cancelled ---*/
      gradPhiReflected[0] = 0.0;

      /*--- Now transform back ---*/
      for (auto jDim = 0u; jDim < nDim; jDim++) {
        for (auto iDim = 0u; iDim < nDim; iDim++) {
          gradPhi[jDim] += TensorMap[iDim][jDim] * gradPhiReflected[iDim];
        }
      }

      for (auto iDim = 0u; iDim < nDim; iDim++) Gradients_iPoint[iVar][iDim] = gradPhi[iDim];
    }
  }
}

/*! \brief Construct a 2D or 3D base given a normal vector.
           Constructs 1 (2D) or 2 (3D) additional vectors orthogonal to the normal to form a base. */
template <class Matrix, class Scalar, typename Int>
inline void BaseFromNormal(Int nDim, const Scalar* UnitNormal, Matrix& TensorMap) {
  /*--- Preprocessing: Compute unit tangential, the direction is arbitrary as long as
        t*n=0 && |t|_2 = 1 ---*/
  Scalar Tangential[3] = {0.0};
  Scalar Orthogonal[3] = {0.0};
  switch (nDim) {
    case 2: {
      Tangential[0] = -UnitNormal[1];
      Tangential[1] = UnitNormal[0];
      for (auto iDim = 0u; iDim < nDim; iDim++) {
        TensorMap[0][iDim] = UnitNormal[iDim];
        TensorMap[1][iDim] = Tangential[iDim];
      }
      break;
    }
    case 3: {
      /*--- n = ai + bj + ck, if |b| > |c| ---*/
      if (abs(UnitNormal[1]) > abs(UnitNormal[2])) {
        /*--- t = bi + (c-a)j - bk  ---*/
        Tangential[0] = UnitNormal[1];
        Tangential[1] = UnitNormal[2] - UnitNormal[0];
        Tangential[2] = -UnitNormal[1];
      } else {
        /*--- t = ci - cj + (b-a)k  ---*/
        Tangential[0] = UnitNormal[2];
        Tangential[1] = -UnitNormal[2];
        Tangential[2] = UnitNormal[1] - UnitNormal[0];
      }
      /*--- Make it a unit vector. ---*/
      Scalar TangentialNorm = GeometryToolbox::Norm(3, Tangential);
      Tangential[0] = Tangential[0] / TangentialNorm;
      Tangential[1] = Tangential[1] / TangentialNorm;
      Tangential[2] = Tangential[2] / TangentialNorm;

      /*--- Compute 3rd direction of the base using cross product ---*/
      GeometryToolbox::CrossProduct(UnitNormal, Tangential, Orthogonal);

      // now we construct the tensor mapping T, note that its inverse is the transpose of T
      for (auto iDim = 0u; iDim < nDim; iDim++) {
        TensorMap[0][iDim] = UnitNormal[iDim];
        TensorMap[1][iDim] = Tangential[iDim];
        TensorMap[2][iDim] = Orthogonal[iDim];
      }
      break;
    }
  }  // switch
}
