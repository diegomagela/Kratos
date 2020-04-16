# -*- coding: utf-8 -*-
import itertools
def findInterfaceFileName(list_of_interface_file_paths,working_path,this_step_out):
    this_step_out +=1
    for file in list_of_interface_file_paths:
        if file.startswith('%s'%working_path + '/Outputs/airfoilSol.MEMBRANE_i=' + '%s'%this_step_out):
            print file
            return file

def findInterfaceFileNumberOfLines(fname):
    with open(fname,'r') as f:
        it = 0
        pattern = {'E+', 'E-'}
        for line in f:
            if 'E+' in line or 'E-' in line:
                it = it+1
    return it
