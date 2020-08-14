#!/usr/bin/env python
# This Python file uses the following encoding: utf-8

## \file ReducedSQP.py
#  \brief Python script for performing the reducedSQP optimization w.
#  \author T. Dick

import sys
import numpy as np
from scipy import optimize
from .. import eval as su2eval
from .project import Project
from .reducedSQP_handmade import SQPconstrained
from .reducedSQP_handmade import SQPequalconstrained

global glob_project

def reduced_sqp(x0, func, f_eqcons, f_ieqcons, fprime, fprime_eqcons, fprime_ieqcons, fdotdot, project, iter, acc, xb ):
    "This is the implementation of the reduced SQP optimizer for smoothed derivatives"

    SQP_MODE = project.config['SQP_MODE']

    # use the SciPy optimizer
    if SQP_MODE == 'SCIPY':

        # preprocessing before the optimization run
        opt = {'maxiter': iter, 'verbose': 3}

        # pack the constaints in form for trust-constr
        equal = optimize.NonlinearConstraint(feq, 0.0, 0.0)
        inequal = optimize.NonlinearConstraint(fieq, 0.0, np.inf)

        # some global function voodoo
        global glob_project
        glob_project = project

        # call the optimizer from Scipy minimize
        outputs = optimize.minimize( fun          = f                 ,
                                     x0           = x0                ,
                                     method       = 'trust-constr'    ,
                                     jac          = df            ,
                                     hess         = ddf           ,
                                     constraints  = [equal, inequal]  ,
                                     bounds       = xb                ,
                                     tol          = acc               ,
                                     options      = opt               )

    # use the self implemented SQP optimizer with a cvxopt core
    elif SQP_MODE == 'SQP_CVXOPT':

        sys.stdout.write('Using implemented SQP version. \n')
        outputs = SQPconstrained(x0, func, f_eqcons, f_ieqcons,
                                 fprime, fprime_eqcons, fprime_ieqcons, fdotdot,
                                 project, iter, acc, None)

    # use the self implemented SQP optimizer with a LES core
    elif SQP_MODE == 'SQP_LES':

        sys.stdout.write('Using implemented SQP version. \n')
        outputs = SQPequalconstrained(x0, func, f_eqcons,
                                      fprime, fprime_eqcons, fdotdot,
                                      project, iter, acc, None)

    # call to gradient descend under constraint.
    elif SQP_MODE =='SIMPLIFIED':

        sys.stdout.write('Using simplified SQP iterations \n')
        outputs = SQPequalconstrained(x0, func, f_eqcons,
                                      fprime, fprime_eqcons, unit_hessian,
                                      project, iter, acc, None)

    else:
        sys.stdout.write('Please choose a valid SQP_MODE! \n')

    # return the results
    return outputs

#
# end of reduced_sqp
#


# we need a unit matrix to have a dummy for optimization tests.
def unit_hessian(x, project):
    return np.identity(np.size(x))


# this is a stupid idea
def f(x):
    global glob_project
    obj_list = glob_project.obj_f(x)
    obj = 0
    for this_obj in obj_list:
        obj = obj+this_obj

    return obj


def df(x):
    global glob_project
    dobj_list = glob_project.obj_df(x)
    dobj=[0.0]*len(dobj_list[0])

    for this_dobj in dobj_list:
        idv=0
        for this_dv_dobj in this_dobj:
            dobj[idv] = dobj[idv]+this_dv_dobj;
            idv+=1
    dobj = np.array( dobj )

    return dobj


def ddf(x):
    global glob_project
    dobj_list = glob_project.obj_ddf(x)
    dobj=[0.0]*len(dobj_list[0])

    for this_dobj in dobj_list:
        idv=0
        for this_dv_dobj in this_dobj:
            dobj[idv] = dobj[idv]+this_dv_dobj;
            idv+=1
    dobj = np.array( dobj )

    return dobj


def feq(x):
    global glob_project
    cons = glob_project.con_ceq(x)

    if cons: cons = np.array(cons)
    else:    cons = np.zeros([0])

    return cons


def fieq(x):
    global glob_project
    cons = glob_project.con_cieq(x)

    if cons:
        cons = np.array(cons)
    else:
        cons = np.zeros([0])

    return -cons


"""
left over code snippets

"""
