# Import Kratos
import KratosMultiphysics
from KratosMultiphysics.MultilevelMonteCarloApplication.adaptive_refinement_utilities import AdaptiveRefinement
from KratosMultiphysics.MultilevelMonteCarloApplication.tools import ParametersWrapper

# Import Kratos problem
from simulation_definition import SimulationScenario
import xmc.classDefs_solverWrapper.methodDefs_KratosSolverWrapper.solve as mds
import xmc.classDefs_solverWrapper.methodDefs_KratosSolverWrapper.utilities as mdu

# Import xmc classes
import xmc.solverWrapper as sw

# Math utilities
import math

# Import pickle to pickle the serializer
import pickle

from xmc.distributedEnvironmentFramework import *

class KratosSolverWrapper(sw.SolverWrapper):
    """
    Solver wrapper class managing Kratos Multiphysics (Kratos) solver.

    Attributes:
    - analysis: Kratos analysis stage. The analysis stage class default name is SimulationScenario. The default file name is simulation_scenario.
    - adaptive_refinement_jump_to_finest_level: boolean. Used in multilevel algorithms when "stochastic_adaptive_refinement" strategy is selected. If true, intermediate refinement indices are skipped. Set by adaptiveRefinementJumpToFinestLevel key.
    - asynchronous: boolean. If true, the asynchronous algorithm should be run. If false, the standard synchronous algorithms should be run. Set by asynchronous key.
    - different_tasks: boolean. Used in multilevel algorithms when "stochastic_adaptive_refinement" strategy is selected. If true, different indices are run all together in the same task. If false, each index is run in a different task. Set by not TaskAllAtOnce key.
    - fake_sample_to_serialize: list. A variable which is used just to serialize the Kratos Model and the Kratos Parameters. The list should be of the same type of the random variable generated by the generator. Set by fakeRandomVariable key.
    - mapping_output_quantities: boolean. If true, the analysis stage is prepared to map the variables of interest to a reference Kratos Model. By default, such Kratos Model is the coarsest index. Set by mappingOutputQuantities key.
    - number_contributions_per_instance: integer. Defines the number of realization per each solve call. Useful if one wants to exploit ensemble average, together with hierarchical Monte Carlo methods. Set by numberContributionsPerInstance key.
    - number_qoi: integer. Defines the number of Quantities of Interest the user wants to return. Set by numberQoI key.
    - number_combined_qoi: integer. Defines the number of combined Quantities of Interest the user wants to return. Set by numberCombinedQoi key.
    - outputBatchSize: integer. Defines the size of each sub-list of the Quantities of Interest list which is returned by the solve method. It is alternative to outputDimension, defined below. Set by OutputBatchSize.
    - outputDimension: integer or list of integers. If integer, equals to len(sample), where sample is the first output argument of self.solve(). If list of integers, then it means that samples are split in future lists, and outputDimension is [len(subSample) for subSample in sample]. Set by OutputDimension key.
    - print_to_file: boolean. If true, prepares the distributed environment programing model PyCOMPSs to write a file inside the solve task. Set by printToFile key.
    - project_parameters_path: string or list of strings. Defines the path to Kratos Project Parameters. Set by projectParametersPath key.
    - refinement_parameters_path: string. Define the path to the Kratos Adaptive Refinement Project Parameters. Set by refinementParametersPath key.
    - refinement_strategy: string. Options are: "reading_from_file", "deterministic_adaptive_refinement", "stochastic_adaptive_refinement". It defines the refinement strategy for multilevel algorithms. Set by refinementStrategy key.

    Methods:
    - serialize: method serializing Kratos Model and Kratos Parameters.
    - solve: method running the problem.
    Other methods are called from the two methods defined above.
    """

    # TODO: solverWrapperIndex will be removed from here and will have an indicator about the level we are at and not which algorithm we are using
    # TODO: are both outputBatchSize and outputBatchSize needed? Probably not.
    # TODO: integrate MultiXMomentEstimators with ensemble average.
    def __init__(self,**keywordArgs):
        super().__init__(**keywordArgs)
        self.analysis = SimulationScenario
        self.adaptive_refinement_jump_to_finest_level = keywordArgs.get("adaptiveRefinementJumpToFinestLevel",False)
        self.asynchronous = keywordArgs.get("asynchronous",False)
        self.different_tasks = not keywordArgs.get('taskAllAtOnce',False)
        self.fake_sample_to_serialize =  keywordArgs.get('fakeRandomVariable')
        self.mapping_output_quantities = keywordArgs.get("mappingOutputQuantities",False)
        self.number_contributions_per_instance = keywordArgs.get("numberContributionsPerInstance",1)
        self.number_qoi = keywordArgs.get("numberQoI",1)
        self.number_combined_qoi = keywordArgs.get("numberCombinedQoi",0)
        self.outputBatchSize = keywordArgs.get('outputBatchSize',1)
        self.print_to_file = keywordArgs.get("printToFile",False)
        self.project_parameters_path = keywordArgs.get('projectParametersPath')
        self.refinement_parameters_path = keywordArgs.get('refinementParametersPath')
        self.refinement_strategy = keywordArgs.get('refinementStrategy')

        # Set outputDimension
        self.outputDimension = keywordArgs.get('outputDimension',None)
        # If not given, compute from self.outputBatchSize for backward compatibility
        if self.outputDimension is None:
            outputNb = self._numberOfScalarOutputs()
            # Total number of output splits, including (possibly) a last one of smaller size
            batchNb = int(math.ceil(outputNb/self.outputBatchSize))
            # Assemble the list of sizes of each split
            # They are all equal to outputBatchSize, except perhaps the last one
            # E.g. outputBatchSize=2 and outputNb=5 gives [2,2,1]
            self.outputDimension = [min(self.outputBatchSize, outputNb-i*self.outputBatchSize)
                                    for i in range(batchNb)]

        # workaround for Monte Carlo
        if (self.solverWrapperIndex == []):
            self.solverWrapperIndex.append(0)

        if (self.solverWrapperIndex[0] >= 0): # for index < 0 not needed
            if (self.asynchronous is not True): # synchronous framework
                self.serialize()
            else: # asynchronous framework
                pass


    def serialize(self):
        """
        Method serializing Kratos Model and Kratos Parameters.

        Inputs:
        - self: an instance of the class.
        """

        if (self.refinement_strategy == "stochastic_adaptive_refinement"):
            # serialization
            self.is_project_parameters_pickled = False
            self.is_model_pickled = False
            self.is_custom_settings_metric_refinement_pickled = False
            self.is_custom_settings_remesh_refinement_pickled = False
            self.SetRefinementParameters()
            self.SerializeRefinementParameters()
            self.SerializeModelParameters()
            # estimate mesh size of current index
            self.ComputeMeshParameters()
        elif (self.refinement_strategy == "deterministic_adaptive_refinement"):
            # serialization
            self.is_project_parameters_pickled = False
            self.is_model_pickled = False
            self.is_custom_settings_metric_refinement_pickled = False
            self.is_custom_settings_remesh_refinement_pickled = False
            self.SetRefinementParameters()
            self.SerializeRefinementParameters()
            self.SerializeModelParameters()
            # estimate mesh size of current index
            self.ComputeMeshParameters()
        elif (self.refinement_strategy == "reading_from_file"):
            # serialization
            self.is_project_parameters_pickled = False
            self.is_model_pickled = False
            self.is_custom_settings_metric_refinement_pickled = False
            self.is_custom_settings_remesh_refinement_pickled = False
            self.SetRefinementParameters()
            self.SerializeRefinementParameters()
            self.SerializeModelParameters()
            # estimate mesh size of current index
            self.ComputeMeshParameters()
        else:
            raise Exception ("Select KratosMultiphysics refinement stategy.\nOptions:\
                \n   i) stochastic_adaptive_refinement\
                \n  ii) deterministic_adaptive_refinement\
                \n iii) reading_from_file")


    def solve(self,random_variable):
        """
        Method running the problem.

        Inputs:
        - self: an instance of the class
        - random_variable: random event in the form of list.

        Outputs:
        - qoi_list: list of structure respecting self.outputDimension. It contains the quantities of interest.
        - time_for_qoi: float. Measure of time to generate the sample.
        """

        if all([component>=0 for component in self.solverWrapperIndex]):
            aux_qoi_array = []
            for contribution_counter in range (0,self.number_contributions_per_instance):
                self.current_local_contribution = contribution_counter
                if (self.refinement_strategy == "stochastic_adaptive_refinement"):
                    qoi,time_for_qoi = self.executeInstanceStochasticAdaptiveRefinement(random_variable)
                elif (self.refinement_strategy == "deterministic_adaptive_refinement"):
                    qoi,time_for_qoi = self.executeInstanceDeterministicAdaptiveRefinement(random_variable)
                elif (self.refinement_strategy == "reading_from_file"):
                    qoi,time_for_qoi = self.executeInstanceReadingFromFile(random_variable)
                # append components to aux array
                aux_qoi_array.append(qoi)
            # delete COMPSs future objects no longer needed
            delete_object(random_variable)

            # postprocess components
            if self.number_contributions_per_instance > 1:
                unm = mdu.UnfolderManager(self._numberOfScalarOutputs(),self.outputBatchSize)
                if (self._numberOfScalarOutputs() == self.outputBatchSize): # no-split solver
                    qoi_list = [unm.PostprocessContributionsPerInstance(aux_qoi_array,self.number_qoi,self.number_combined_qoi)]
                elif (self._numberOfScalarOutputs() > self.outputBatchSize): # split solver
                    qoi_list = unm.PostprocessContributionsPerInstance(aux_qoi_array,self.number_qoi,self.number_combined_qoi)
                else:
                    raise Exception("_numberOfScalarOutputs() returns a value smaller than self.outputBatchSize. Set outputBatchSize smaller or equal to the number of scalar outputs.")
                delete_object(unm)
            else:
                # unfold qoi into its components of fixed size
                unm = mdu.UnfolderManager(self._numberOfScalarOutputs(), self.outputBatchSize)
                if (self._numberOfScalarOutputs() == self.outputBatchSize): # no-split solver
                    qoi_list = [unm.UnfoldNValues_Task(aux_qoi_array[0])]
                elif (self._numberOfScalarOutputs() > self.outputBatchSize): # split solver
                    qoi_list = unm.UnfoldNValues_Task(aux_qoi_array[0])
                else:
                    raise Exception("_numberOfScalarOutputs() returns a value smaller than self.outputBatchSize. Set outputBatchSize smaller or equal to the number of scalar outputs.")
                # delete COMPSs future objects no longer needed
                delete_object(unm)

            # delete COMPSs future objects no longer needed
            for contribution_counter in range (0,self.number_contributions_per_instance):
                delete_object(aux_qoi_array[contribution_counter])
            delete_object(qoi)
            del(aux_qoi_array)

        else:
            qoi,time_for_qoi = mds.returnZeroQoiAndTime_Task(self._numberOfScalarOutputs())
            # unfold qoi into its components of fixed size
            unm = mdu.UnfolderManager(self._numberOfScalarOutputs(), self.outputBatchSize)
            if (self._numberOfScalarOutputs() == self.outputBatchSize): # no-split solver
                qoi_list = [unm.UnfoldNValues_Task(qoi)]
            elif (self._numberOfScalarOutputs() > self.outputBatchSize): # split solver
                qoi_list = unm.UnfoldNValues_Task(qoi)
            else:
                raise Exception("_numberOfScalarOutputs() returns a value smaller than self.outputBatchSize. Set outputBatchSize smaller or equal to the number of scalar outputs.")
            # delete COMPSs future objects no longer needed
            delete_object(unm)

        return qoi_list,time_for_qoi


    ####################################################################################################
    ######################################### EXECUTION TOOLS ##########################################
    ####################################################################################################


    def executeInstanceStochasticAdaptiveRefinement(self,random_variable):
        """
        Method executing an instance of the UQ algorithm, i.e. a single MC realization and eventually the refinement (that occurs before the simulation run). To be called if the selected refinement strategy is stochastic_adaptive_refinement.

        Inputs:
        - self: an instance of the class.

        Outputs:
        - qoi: list. It contains the quantities of interest.
        - time_for_qoi: float. Measure of time to generate the sample.
        """

        current_index = self.solverWrapperIndex[0]
        # local variables
        pickled_coarse_model = self.pickled_model[0]
        pickled_reference_model_mapping = pickled_coarse_model
        pickled_coarse_project_parameters = self.pickled_project_parameters[0]
        pickled_custom_metric_refinement_parameters = self.pickled_custom_metric_refinement_parameters
        pickled_custom_remesh_refinement_parameters = self.pickled_custom_remesh_refinement_parameters
        current_analysis = self.analysis
        different_tasks = self.different_tasks
        mapping_flag = self.mapping_output_quantities
        adaptive_refinement_jump_to_finest_level = self.adaptive_refinement_jump_to_finest_level
        print_to_file = self.print_to_file
        current_local_contribution = self.current_local_contribution
        time_for_qoi = 0.0
        if (different_tasks is False): # tasks all at once
            qoi,time_for_qoi = \
                mds.executeInstanceStochasticAdaptiveRefinementAllAtOnce_Wrapper(current_index,pickled_coarse_model,pickled_coarse_project_parameters,pickled_custom_metric_refinement_parameters,pickled_custom_remesh_refinement_parameters,random_variable,current_analysis,time_for_qoi,mapping_flag,adaptive_refinement_jump_to_finest_level,print_to_file,current_local_contribution)
        elif (different_tasks is True): # multiple tasks
            if (current_index == 0): # index = 0
                current_local_index = 0
                qoi,pickled_current_model,time_for_qoi = \
                    mds.executeInstanceStochasticAdaptiveRefinementMultipleTasks_Wrapper(current_index,pickled_coarse_model,pickled_coarse_project_parameters,pickled_custom_metric_refinement_parameters,pickled_custom_remesh_refinement_parameters,random_variable,current_local_index,current_analysis,time_for_qoi,mapping_flag,print_to_file,current_local_contribution)
                delete_object(pickled_current_model)
            else: # index > 0
                for current_local_index in range(current_index+1):
                    if ((adaptive_refinement_jump_to_finest_level is False) or (adaptive_refinement_jump_to_finest_level is True and (current_local_index == 0 or current_local_index == current_index))):
                        if (mapping_flag is False):
                            qoi,pickled_current_model,time_for_qoi = \
                                mds.executeInstanceStochasticAdaptiveRefinementMultipleTasks_Wrapper(current_index,pickled_coarse_model,pickled_coarse_project_parameters,pickled_custom_metric_refinement_parameters,pickled_custom_remesh_refinement_parameters,random_variable,current_local_index,current_analysis,time_for_qoi,mapping_flag,print_to_file,current_local_contribution)
                        elif (mapping_flag is True):
                            qoi,pickled_current_model,time_for_qoi = \
                                mds.executeInstanceStochasticAdaptiveRefinementMultipleTasks_Wrapper(current_index,pickled_coarse_model,pickled_coarse_project_parameters,pickled_custom_metric_refinement_parameters,pickled_custom_remesh_refinement_parameters,random_variable,current_local_index,current_analysis,time_for_qoi,mapping_flag,print_to_file,current_local_contribution,pickled_mapping_reference_model=pickled_reference_model_mapping)
                            delete_object(pickled_coarse_model)
                            del(pickled_coarse_model)
                        pickled_coarse_model = pickled_current_model
                        del(pickled_current_model)
                    else: # not running since we jump from coarsest to finest level
                        pass
                delete_object(pickled_coarse_model)
        else:
            raise Exception ("Boolean variable different task is not a boolean, instead is equal to",different_tasks)
        return qoi,time_for_qoi


    def executeInstanceDeterministicAdaptiveRefinement(self,random_variable):
        """
        Method executing an instance of the UQ algorithm, i.e. a single MC realization and eventually the refinement (that occurs before the simulation run). To be called if the selected refinement strategy is deterministic_adaptive_refinement.

        Inputs:
        - self: an instance of the class.

        Outputs:
        - qoi: list. It contains the quantities of interest.
        - time_for_qoi: float. Measure of time to generate the sample.
        """

        # local variables
        current_index = self.solverWrapperIndex[0]
        pickled_model = self.pickled_model[current_index]
        pickled_project_parameters = self.pickled_project_parameters[0]
        current_analysis = self.analysis
        time_for_qoi = 0.0
        qoi,time_for_qoi = mds.executeInstanceDeterministicAdaptiveRefinement_Wrapper(current_index,pickled_model,pickled_project_parameters,current_analysis,random_variable,time_for_qoi)
        return qoi,time_for_qoi


    def executeInstanceReadingFromFile(self,random_variable):
        """
        Method executing an instance of the UQ algorithm, i.e. a single MC realization and eventually the refinement (that occurs before the simulation run). To be called if the selected refinement strategy is reading_from_file.

        Inputs:
        - self: an instance of the class.

        Outputs:
        - qoi: list. It contains the quantities of interest.
        - time_for_qoi: float. Measure of time to generate the sample.
        """

        # local variables
        current_index = self.solverWrapperIndex[0]
        pickled_model = self.pickled_model[current_index]
        pickled_mapping_reference_model = self.pickled_model[0]
        pickled_project_parameters = self.pickled_project_parameters[current_index]
        mapping_flag = self.mapping_output_quantities
        print_to_file = self.print_to_file
        current_local_contribution = self.current_local_contribution
        current_analysis = self.analysis
        time_for_qoi = 0.0
        qoi,time_for_qoi = mds.executeInstanceReadingFromFile_Wrapper(current_index,pickled_model,pickled_project_parameters,current_analysis,random_variable,time_for_qoi,mapping_flag,pickled_mapping_reference_model,print_to_file,current_local_contribution)
        return qoi,time_for_qoi


    ####################################################################################################
    ####################################### SERIALIZATION TOOLS ########################################
    ####################################################################################################


    def SerializeModelParameters(self):
        """
        Method managing the serialization and pickling of the Kratos Model and the Kratos Parameters of the problem. It builds self.pickled_model and self.pickled_project_parameters.
        The serialization-pickling process is the following:
        i)   from Model/Parameters Kratos object to MpiSerializer Kratos object,
        ii)  from MpiSerializer Kratos object to pickle string,
        iii) from pickle string to MpiSerializer Kratos object,
        iv)  from MpiSerializer Kratos object to Model/Parameters Kratos object.
        Depending on the refinement strategy, three different methods may be called and are defined next.

        Inputs:
        - self: an instance of the class.
        """

        self.serialized_model = []
        self.serialized_project_parameters = []
        self.pickled_model = []
        self.pickled_project_parameters = []

        if (self.refinement_strategy == "stochastic_adaptive_refinement"):
            self.SerializeModelParametersStochasticAdaptiveRefinement()
        elif (self.refinement_strategy == "deterministic_adaptive_refinement"):
            self.SerializeModelParametersDeterministicAdaptiveRefinement()
        elif (self.refinement_strategy == "reading_from_file"):
            self.SerializeModelParametersReadingFromFile()
        else:
            raise Exception ("Specify refinement_strategy: stochastic_adaptive_refinement or deterministic_adaptive_refinement or reading_from_file")
        self.is_project_parameters_pickled = True
        self.is_model_pickled = True
        print("\n","#"*50," SERIALIZATION MODEL AND PROJECT PARAMETERS COMPLETED ","#"*50,"\n")


    def SerializeModelParametersStochasticAdaptiveRefinement(self):
        """
        Method serializing and pickling the Kratos Model and the Kratos Parameters of the problem. It builds self.pickled_model and self.pickled_project_parameters. To be called if the selected refinement strategy is stochastic_adaptive_refinement.

        Inputs:
        - self: an instance of the class.
        """

        with open(self.project_parameters_path,'r') as parameter_file:
            parameters = KratosMultiphysics.Parameters(parameter_file.read())
        # create wrapper instance to modify current project parameters
        self.wrapper = ParametersWrapper(parameters)
        # serialize parmeters (to avoid adding new data dependent on the application)
        parameters = self.wrapper.SetModelImportSettingsInputType("use_input_model_part")
        serialized_project_parameters = KratosMultiphysics.MpiSerializer()
        serialized_project_parameters.Save("ParametersSerialization",parameters)
        self.serialized_project_parameters.append(serialized_project_parameters)
        # reset to read the model part
        parameters = self.wrapper.SetModelImportSettingsInputType("mdpa")
        # prepare the model to serialize
        model = KratosMultiphysics.Model()
        fake_sample = self.fake_sample_to_serialize
        simulation = self.analysis(model,parameters,fake_sample)
        simulation.Initialize()
        # reset general flags
        main_model_part_name = self.wrapper.GetModelPartName()
        simulation.model.GetModelPart(main_model_part_name).ProcessInfo.SetValue(KratosMultiphysics.IS_RESTARTED,True)
        # serialize model
        serialized_model = KratosMultiphysics.MpiSerializer()
        serialized_model.Save("ModelSerialization",simulation.model)
        self.serialized_model.append(serialized_model)
        # pickle dataserialized_data
        pickled_model = pickle.dumps(serialized_model, 2) # second argument is the protocol and is NECESSARY (according to pybind11 docs)
        pickled_project_parameters = pickle.dumps(serialized_project_parameters, 2)
        self.pickled_model.append(pickled_model)
        self.pickled_project_parameters.append(pickled_project_parameters)


    def SerializeModelParametersDeterministicAdaptiveRefinement(self):
        """
        Method serializing and pickling the Kratos Model and the Kratos Parameters of the problem. It builds self.pickled_model and self.pickled_project_parameters. To be called if the selected refinement strategy is deterministic_adaptive_refinement.

        Inputs:
        - self: an instance of the class.
        """

        self.SerializeModelParametersStochasticAdaptiveRefinement() # to prepare parameters and model part of coarsest level
        number_levels_to_serialize = self.solverWrapperIndex[0]
        # same routine of ExecuteInstanceConcurrentAdaptiveRefinemnt() to build models and parameters, but here we save models and parameters
        pickled_coarse_model = self.pickled_model[0]
        pickled_coarse_project_parameters = self.pickled_project_parameters[0]
        pickled_custom_metric_refinement_parameters = self.pickled_custom_metric_refinement_parameters
        pickled_custom_remesh_refinement_parameters = self.pickled_custom_remesh_refinement_parameters
        current_analysis = self.analysis
        # generate the sample and prepare auxiliary variables we need
        fake_sample = self.fake_sample_to_serialize
        fake_computational_time = 0.0
        if (number_levels_to_serialize > 0):
            for current_level in range(number_levels_to_serialize+1):
                fake_qoi,pickled_current_model,fake_computational_time = \
                    mds.executeInstanceStochasticAdaptiveRefinementMultipleTasks_Wrapper(number_levels_to_serialize,pickled_coarse_model,pickled_coarse_project_parameters,pickled_custom_metric_refinement_parameters,pickled_custom_remesh_refinement_parameters,fake_sample,current_level,current_analysis,fake_computational_time,mapping_flag=False,print_to_file=False,current_contribution=0)
                del(pickled_coarse_model)
                pickled_coarse_model = pickled_current_model
                # save if current level > 0 (level = 0 has already been saved)
                if (current_level>0):
                    # save pickled and serialized model and parameters
                    self.pickled_model.append(pickled_current_model)
                    self.serialized_model.append(pickle.loads(pickled_current_model))
                del(pickled_current_model)


    def SerializeModelParametersReadingFromFile(self):
        """
        Method serializing and pickling the Kratos Model and the Kratos Parameters of the problem. It builds self.pickled_model and self.pickled_project_parameters. To be called if the selected refinement strategy is reading_from_file.

        Inputs:
        - self: an instance of the class.
        """

        for parameters_path in self.project_parameters_path:
            with open(parameters_path,'r') as parameter_file:
                parameters = KratosMultiphysics.Parameters(parameter_file.read())
            # create wrapper instance to modify current project parameters
            self.wrapper = ParametersWrapper(parameters)
            # serialize parmeters (to avoid adding new data dependent on the application)
            parameters = self.wrapper.SetModelImportSettingsInputType("use_input_model_part")
            serialized_project_parameters = KratosMultiphysics.MpiSerializer()
            serialized_project_parameters.Save("ParametersSerialization",parameters)
            self.serialized_project_parameters.append(serialized_project_parameters)
            # reset to read the model part
            parameters = self.wrapper.SetModelImportSettingsInputType("mdpa")
            # prepare the model to serialize
            model = KratosMultiphysics.Model()
            fake_sample = self.fake_sample_to_serialize
            simulation = self.analysis(model,parameters,fake_sample)
            simulation.Initialize()
            # reset general flags
            main_model_part_name = self.wrapper.GetModelPartName()
            simulation.model.GetModelPart(main_model_part_name).ProcessInfo.SetValue(KratosMultiphysics.IS_RESTARTED,True)
            # serialize model
            serialized_model = KratosMultiphysics.MpiSerializer()
            serialized_model.Save("ModelSerialization",simulation.model)
            self.serialized_model.append(serialized_model)
            # pickle dataserialized_data
            pickled_model = pickle.dumps(serialized_model, 2) # second argument is the protocol and is NECESSARY (according to pybind11 docs)
            pickled_project_parameters = pickle.dumps(serialized_project_parameters, 2)
            self.pickled_model.append(pickled_model)
            self.pickled_project_parameters.append(pickled_project_parameters)


    def SerializeRefinementParameters(self):
        """
        Method serializing and pickling the custom setting metric and remeshing for the adaptive refinement. It requires self.custom_metric_refinement_parameters and self.custom_remesh_refinement_parameters. It builds self.pickled_custom_metric_refinement_parameters and self.pickled_custom_remesh_refinement_parameters.

        Inputs:
        - self: an instance of the class.
        """

        metric_refinement_parameters = self.custom_metric_refinement_parameters
        remeshing_refinement_parameters = self.custom_remesh_refinement_parameters
        # save parameters as MpiSerializer Kratos objects
        serialized_metric_refinement_parameters = KratosMultiphysics.MpiSerializer()
        serialized_metric_refinement_parameters.Save("MetricRefinementParametersSerialization",metric_refinement_parameters)
        serialized_remesh_refinement_parameters = KratosMultiphysics.MpiSerializer()
        serialized_remesh_refinement_parameters.Save("RemeshRefinementParametersSerialization",remeshing_refinement_parameters)
        # pickle parameters
        pickled_metric_refinement_parameters = pickle.dumps(serialized_metric_refinement_parameters, 2) # second argument is the protocol and is NECESSARY (according to pybind11 docs)
        pickled_remesh_refinement_parameters = pickle.dumps(serialized_remesh_refinement_parameters, 2) # second argument is the protocol and is NECESSARY (according to pybind11 docs)
        self.pickled_custom_metric_refinement_parameters = pickled_metric_refinement_parameters
        self.pickled_custom_remesh_refinement_parameters = pickled_remesh_refinement_parameters
        self.is_custom_settings_metric_refinement_pickled = True
        self.is_custom_settings_remesh_refinement_pickled = True
        print("\n","#"*50," SERIALIZATION REFINEMENT PARAMETERS COMPLETED ","#"*50,"\n")


    ####################################################################################################
    ######################################### AUXILIARY TOOLS ##########################################
    ####################################################################################################


    def SetRefinementParameters(self):
        """
        Method reading the refinement parameters passed from json file.

        Inputs:
        - self: an instance of the class.
        """

        with open(self.refinement_parameters_path,'r') as parameter_file:
            parameters = KratosMultiphysics.Parameters(parameter_file.read())
        self.custom_metric_refinement_parameters = parameters["hessian_metric"]
        self.custom_remesh_refinement_parameters = parameters["refinement_mmg"]

    def ComputeMeshParameters(self):
        """
        Method computing the mesh discretization parameter self.mesh_parameters and the mesh sizes self.mesh_sizes. The mesh parameter is the reciprocal of the minimum mesh size of the grid.

        Inputs:
        - self: an instance of the class.
        """

        # unpickle and unserialize model and build Kratos Model object
        serialized_model = pickle.loads(self.pickled_model[0])
        current_model = KratosMultiphysics.Model()
        serialized_model.Load("ModelSerialization",current_model)
        # unpickle and unserialize parameters and build Kratos Parameters object
        serialized_project_parameters = pickle.loads(self.pickled_project_parameters[0])
        current_project_parameters = KratosMultiphysics.Parameters()
        serialized_project_parameters.Load("ParametersSerialization",current_project_parameters)
        # unpickle and unserialize metric refinement parameters and build Kratos Parameters objects
        serialized_custom_metric_refinement_parameters = pickle.loads(self.pickled_custom_metric_refinement_parameters)
        current_custom_metric_refinement_parameters = KratosMultiphysics.Parameters()
        serialized_custom_metric_refinement_parameters.Load("MetricRefinementParametersSerialization",current_custom_metric_refinement_parameters)
        self.mesh_sizes = []
        self.mesh_parameters = []
        level = self.solverWrapperIndex[0]
        adaptive_refinement_manager = AdaptiveRefinement(level,current_model,current_project_parameters,current_custom_metric_refinement_parameters,None)
        adaptive_refinement_manager.EstimateMeshSizeCurrentLevel()
        h_current_level = adaptive_refinement_manager.mesh_size
        mesh_parameter_current_level = h_current_level**(-1)
        self.mesh_sizes.append(h_current_level)
        self.mesh_parameters.append(mesh_parameter_current_level)

    def _numberOfScalarOutputs(self):
        """
        Internal method returning the total number of scalar outputs, regardless of how
        they may be separated into vector quantities of interest.

        Inputs:
        - self: an instance of the class.
        """
        return self.number_qoi + self.number_combined_qoi
