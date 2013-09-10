/*!
 * \file math_ad.hpp
 * \brief Differentiated math routines
 * \author Aerospace Design Laboratory (Stanford University) <http://su2.stanford.edu>.
 * \version 2.0.7
 *
 * Stanford University Unstructured (SU2), 
 * copyright (C) 2012-2013 Aerospace Design Laboratory (ADL), is
 * distributed under the GNU Lesser General Public License (GNU LGPL).
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cmath>

//************************************************//
// Please do not delete //SU2_DIFF comment lines  //
//************************************************//

//SU2_DIFF START fabs_d

double fabs_d(double a, double ad, double *fabs) ;

//SU2_DIFF END fabs_d

//************************************************//
// Please do not delete //SU2_DIFF comment lines  //
//************************************************//

//SU2_DIFF START pow_d

double pow_d(double a, double ad, double b, double *c) ;

//SU2_DIFF END pow_d
