#!/usr/bin/env python

## \file geometry.py
#  \brief python package for running geometry analyses
#  \author T. Lukaczyk, F. Palacios
#  \version 3.2.9 "eagle"
#
# SU2 Lead Developers: Dr. Francisco Palacios (francisco.palacios@boeing.com).
#                      Dr. Thomas D. Economon (economon@stanford.edu).
#
# SU2 Developers: Prof. Juan J. Alonso's group at Stanford University.
#                 Prof. Piero Colonna's group at Delft University of Technology.
#                 Prof. Nicolas R. Gauger's group at Kaiserslautern University of Technology.
#                 Prof. Alberto Guardone's group at Polytechnic University of Milan.
#                 Prof. Rafael Palacios' group at Imperial College London.
#
# Copyright (C) 2012-2015 SU2, the open-source CFD code.
#
# SU2 is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# SU2 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with SU2. If not, see <http://www.gnu.org/licenses/>.

# ----------------------------------------------------------------------
#  Imports
# ----------------------------------------------------------------------

import os, sys, shutil, copy

from .. import io  as su2io
from interface import GEO       as SU2_GEO
from ..util import ordered_bunch

# ----------------------------------------------------------------------
#  Direct Simulation
# ----------------------------------------------------------------------

def geometry ( config , step = 1e-3 ): 
    """ info = SU2.run.geometry(config)
        
        Runs an geometry analysis with:
            SU2.run.decomp()
            SU2.run.GEO()
            
        Assumptions:
            Performs both function and gradient analysis
                        
        Inputs:
            config - an SU2 configuration
            step   - gradient finite difference step if config.GEO_MODE=GRADIENT
        
        Outputs:
            info - SU2 State with keys:
                FUNCTIONS
                GRADIENTS
                
        Updates:
        
        Executes in:
            ./
    """
    
    # local copy
    konfig = copy.deepcopy(config)
    
    # unpack
    function_name = konfig['GEO_PARAM']
    func_filename = 'of_func.dat'
    grad_filename = 'of_grad.dat'
    
    # choose dv values 
    Definition_DV = konfig['DEFINITION_DV']
    n_DV          = len(Definition_DV['KIND'])
    if isinstance(step,list):
        assert len(step) == n_DV , 'unexpected step vector length'
    else:
        step = [step]*n_DV
    dv_old = [0.0]*n_DV # SU2_DOT input requirement, assumes linear superposition of design variables
    dv_new = step
    konfig.unpack_dvs(dv_new,dv_old)    
    
    # Run Solution
    SU2_GEO(konfig)
    
    # info out
    info = su2io.State()    
    
    # get function values
    if konfig.GEO_MODE == 'FUNCTION':
        functions = su2io.tools.read_plot(func_filename)
        for key,value in functions.items():
            functions[key] = value[0]
        info.FUNCTIONS.update( functions )
    
    # get gradient_values
    if konfig.GEO_MODE == 'GRADIENT':
        gradients = su2io.tools.read_plot(grad_filename)
        info.GRADIENTS.update( gradients )

    return info
