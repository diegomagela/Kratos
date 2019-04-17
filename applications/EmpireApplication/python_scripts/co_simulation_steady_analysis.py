from __future__ import print_function, absolute_import, division  # makes these scripts backward compatible with python 2.6 and 2.7

# Importing the base class
from co_simulation_analysis import CoSimulationAnalysis

import sys

class CoSimulationSteadyAnalysis(CoSimulationAnalysis):

    def RunSolutionLoop(self):
        print("")
        self.InitializeSolutionStep()
        self._GetSolver().Predict()
        self._GetSolver().SolveSolutionStep()
        self.FinalizeSolutionStep()
        self.OutputSolutionStep()

        if self.flush_stdout:
            sys.stdout.flush()
