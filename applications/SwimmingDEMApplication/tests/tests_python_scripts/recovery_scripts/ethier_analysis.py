import KratosMultiphysics as Kratos
from KratosMultiphysics import Parameters, Vector
import swimming_DEM_procedures as SDP
import math
import numpy as np
import os
import sys
import weakref
import KratosMultiphysics.SwimmingDEMApplication as SDEM
from swimming_DEM_analysis import SwimmingDEMAnalysis
from swimming_DEM_analysis import Say
import parameters_tools as PT

class EthierBenchmarkAnalysis(SwimmingDEMAnalysis):
    def __init__(self, model, varying_parameters=Parameters("{}")):
        super(EthierBenchmarkAnalysis, self).__init__(model, varying_parameters)
        self.vars_man.fluid_vars += [SDEM.VECTORIAL_ERROR, SDEM.VECTORIAL_ERROR_1]
        self._GetDEMAnalysis().mdpas_folder_path = os.path.join(self._GetDEMAnalysis().main_path, 'recovery_tests')
        self.project_parameters["custom_fluid"]["fluid_already_calculated"].SetBool(True)
        self.project_parameters.AddEmptyValue("load_derivatives").SetBool(False)

    def GetEmbeddedCounter(self):
        return SDP.Counter(is_dead=True)

    def GetDebugInfo(self):
        return SDP.Counter(self.project_parameters["debug_tool_cycle"].GetInt(), 1, is_dead = 1)

    def PerformZeroStepInitializations(self):
        self.mat_deriv_errors = []
        self.laplacian_errors = []
        self.pressure_gradient_errors = []
        self.current_mat_deriv_errors = np.zeros(2)
        self.current_laplacian_errors = np.zeros(2)
        self.current_pressure_gradient_errors = np.zeros(2)
        for node in self.fluid_model_part.Nodes:
            vel= Vector(3)
            coor = Vector([node.X, node.Y, node.Z])
            self.flow_field.Evaluate(0.0, coor, vel, 0)
            pressure = self.pressure_field.Evaluate(0.0, coor, 0)
            node.SetSolutionStepValue(Kratos.VELOCITY, vel)
            node.SetSolutionStepValue(Kratos.PRESSURE, pressure)

    def InitializeSolutionStep(self):
        super(EthierBenchmarkAnalysis, self).InitializeSolutionStep()
        for node in self.fluid_model_part.Nodes:
            vel= Vector(3)
            coor = Vector([node.X, node.Y, node.Z])
            self.flow_field.Evaluate(self.time, coor, vel, 0)
            pressure = self.pressure_field.Evaluate(self.time, coor, 0)
            node.SetSolutionStepValue(Kratos.VELOCITY, vel)
            node.SetSolutionStepValue(Kratos.PRESSURE, pressure)

    def FinalizeSolutionStep(self):
        super(EthierBenchmarkAnalysis, self).FinalizeSolutionStep()
        self.CalculateRecoveryErrors(self.time)

    def GetFieldUtility(self):
        a = math.pi / 4
        d = math.pi / 2
        b = 1.0
        bx, by, bz = 1.0, 2.0, 5.0
        b = SDEM.LinearFunction(15.5, b)
        a0 = SDEM.LinearFunction(0.0, bx)
        a1 = SDEM.LinearFunction(0.0, by)
        a2 = SDEM.LinearFunction(0.0, bz)
        self.pressure_field = SDEM.LinearRealField(a0, a1, a2, b)
        self.flow_field = SDEM.EthierVelocityField(a, d)
        space_time_set = SDEM.SpaceTimeSet()
        self.field_utility = SDEM.FluidFieldUtility(space_time_set,
                                                    self.pressure_field,
                                                    self.flow_field,
                                                    1000.0,
                                                    1e-6)
        return self.field_utility

    def GetRecoveryCounter(self):
        return SDP.Counter(1, 1, self.project_parameters["coupling"]["coupling_level_type"].GetInt() or self.project_parameters.print_PRESSURE_GRADIENT_option)

    def CalculateRecoveryErrors(self, time):
        L2_norm_mat_deriv = 0.
        L2_norm_mat_deriv_error = 0.
        L2_norm_laplacian = 0.
        L2_norm_laplacian_error = 0.
        L2_norm_pressure_gradient = 0.
        L2_norm_pressure_gradient_error = 0.
        max_mat_deriv_error = 0.
        max_laplacian_error = 0.
        max_pressure_gradient_error = 0.
        total_volume = 0.

        calc_mat_deriv = np.zeros(3)
        calc_laplacian = np.zeros(3)
        calc_pressure_gradient = np.zeros(3)

        mat_deriv = Vector(3)
        laplacian = Vector(3)
        pressure_gradient = Vector(3)

        for node in self.fluid_model_part.Nodes:
            nodal_volume = node.GetSolutionStepValue(Kratos.NODAL_AREA)
            total_volume += nodal_volume
            coor = Vector([node.X, node.Y, node.Z])

            self.flow_field.CalculateMaterialAcceleration(0., coor, mat_deriv, 0)
            self.flow_field.CalculateLaplacian(0., coor, laplacian, 0)
            self.pressure_field.CalculateGradient(self.time, coor, pressure_gradient, 0)
            #Say(pressure_gradient)
            calc_mat_deriv = node.GetSolutionStepValue(Kratos.MATERIAL_ACCELERATION)
            calc_laplacian = node.GetSolutionStepValue(Kratos.VELOCITY_LAPLACIAN)
            calc_pressure_gradient = node.GetSolutionStepValue(Kratos.PRESSURE_GRADIENT)

            module_mat_deriv_squared = sum(x**2 for x in mat_deriv)
            module_laplacian_squared = sum(x**2 for x in laplacian)
            module_pressure_gradient_squared = sum(x**2 for x in pressure_gradient)

            L2_norm_mat_deriv += module_mat_deriv_squared * nodal_volume
            L2_norm_laplacian += module_laplacian_squared * nodal_volume
            L2_norm_pressure_gradient += module_pressure_gradient_squared * nodal_volume

            diff_mat_deriv = calc_mat_deriv - mat_deriv
            diff_laplacian = calc_laplacian - laplacian
            diff_pressure_gradient = calc_pressure_gradient - pressure_gradient
            Say(calc_pressure_gradient[0], pressure_gradient[0])
            node.SetSolutionStepValue(SDEM.VECTORIAL_ERROR, Vector(list(diff_mat_deriv)))
            node.SetSolutionStepValue(SDEM.VECTORIAL_ERROR_1, Vector(list(diff_laplacian)))
            module_mat_deriv_error_squared = sum(x**2 for x in diff_mat_deriv)
            module_laplacian_error_squared = sum(x**2 for x in diff_laplacian)
            module_pressure_gradient_error_squared = sum(x**2 for x in diff_pressure_gradient)
            L2_norm_mat_deriv_error += module_mat_deriv_error_squared * nodal_volume
            L2_norm_laplacian_error += module_laplacian_error_squared * nodal_volume
            L2_norm_pressure_gradient_error += module_pressure_gradient_error_squared * nodal_volume
            max_mat_deriv_error = max(max_mat_deriv_error, module_mat_deriv_error_squared)
            max_laplacian_error = max(max_laplacian_error, module_laplacian_error_squared)
            max_pressure_gradient_error = max(max_pressure_gradient_error, module_pressure_gradient_error_squared)

        L2_norm_mat_deriv **= 0.5
        L2_norm_mat_deriv /= total_volume ** 0.5
        L2_norm_mat_deriv_error **= 0.5
        L2_norm_mat_deriv_error /= total_volume ** 0.5

        L2_norm_laplacian **= 0.5
        L2_norm_laplacian /= total_volume ** 0.5
        L2_norm_laplacian_error **= 0.5
        L2_norm_laplacian_error /= total_volume ** 0.5

        L2_norm_pressure_gradient **= 0.5
        L2_norm_pressure_gradient /= total_volume ** 0.5
        L2_norm_pressure_gradient_error **= 0.5
        L2_norm_pressure_gradient_error /= total_volume ** 0.5

        max_mat_deriv_error **= 0.5
        max_laplacian_error **= 0.5
        max_pressure_gradient_error **= 0.5

        if L2_norm_mat_deriv > 0 and L2_norm_laplacian > 0 and L2_norm_pressure_gradient > 0:
            SDP.MultiplyNodalVariableByFactor(self.fluid_model_part, SDEM.VECTORIAL_ERROR, 1.0 / L2_norm_mat_deriv)
            SDP.MultiplyNodalVariableByFactor(self.fluid_model_part, SDEM.VECTORIAL_ERROR_1, 1.0 / L2_norm_laplacian)
            self.current_mat_deriv_errors[0] = L2_norm_mat_deriv_error / L2_norm_mat_deriv
            self.current_mat_deriv_errors[1] = max_mat_deriv_error / L2_norm_mat_deriv
            self.current_laplacian_errors[0] = L2_norm_laplacian_error / L2_norm_laplacian
            self.current_laplacian_errors[1] = max_laplacian_error / L2_norm_laplacian
            self.current_pressure_gradient_errors[0] = L2_norm_pressure_gradient_error / L2_norm_pressure_gradient
            self.current_pressure_gradient_errors[1] = max_pressure_gradient_error / L2_norm_pressure_gradient
            self.mat_deriv_errors.append(self.current_mat_deriv_errors)
            self.laplacian_errors.append(self.current_laplacian_errors)
            self.pressure_gradient_errors.append(self.current_pressure_gradient_errors)

            text_width = 40
            Say('\n' + '-.' * text_width)
            Say('L2 error for the material derivative'.ljust(text_width), self.current_mat_deriv_errors[0])
            Say('max error for the material derivative'.ljust(text_width), self.current_mat_deriv_errors[1])
            Say('L2 error for the laplacian'.ljust(text_width), self.current_laplacian_errors[0])
            Say('max error for the laplacian'.ljust(text_width), self.current_laplacian_errors[1])
            Say('L2 error for the pressure_gradient'.ljust(text_width), self.current_pressure_gradient_errors[0])
            Say('max error for the pressure_gradient'.ljust(text_width), self.current_pressure_gradient_errors[1])
            Say('-.' * text_width + '\n')