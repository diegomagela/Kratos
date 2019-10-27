//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ \.
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Pooyan Dadvand
//

#if !defined(KRATOS_VARIABLES_H_INCLUDED )
#define  KRATOS_VARIABLES_H_INCLUDED

// System includes
#include <string>
#include <iostream>

// External includes

// Project includes
#include "includes/define.h"
#include "containers/variable.h"
#include "containers/variable_component.h"
#include "containers/vector_component_adaptor.h"
#include "includes/kratos_components.h"
#include "includes/ublas_interface.h"
#include "containers/array_1d.h"
#include "containers/global_pointers_vector.h"
#include "containers/periodic_variables_container.h"
#include "utilities/table_stream_utility.h"
#include "utilities/adjoint_extensions.h"

#undef  KRATOS_EXPORT_MACRO
#define KRATOS_EXPORT_MACRO KRATOS_API

namespace Kratos
{
    //Define Variables by type:

    //bools
    KRATOS_DEFINE_VARIABLE(bool, IS_RESTARTED )
    KRATOS_DEFINE_VARIABLE(bool, COMPUTE_DYNAMIC_TANGENT )
    KRATOS_DEFINE_VARIABLE(bool, COMPUTE_LUMPED_MASS_MATRIX )

    //ints
    KRATOS_DEFINE_VARIABLE( int, DOMAIN_SIZE )
    KRATOS_DEFINE_VARIABLE( int, SPACE_DIMENSION )

    //for General kratos application:
    KRATOS_DEFINE_VARIABLE( int, LOAD_RESTART )
    KRATOS_DEFINE_VARIABLE( int, TIME_STEPS )
    KRATOS_DEFINE_VARIABLE( int, RIGID_BODY_ID )

    //for Structural application
    KRATOS_DEFINE_VARIABLE( int, FIRST_TIME_STEP )
    KRATOS_DEFINE_VARIABLE( int, QUASI_STATIC_ANALYSIS )


    KRATOS_DEFINE_VARIABLE( int, NL_ITERATION_NUMBER )
    KRATOS_DEFINE_VARIABLE( int, PERIODIC_PAIR_INDEX )
    KRATOS_DEFINE_VARIABLE( int, STATIONARY )
    KRATOS_DEFINE_VARIABLE( int, ACTIVATION_LEVEL )

    KRATOS_DEFINE_VARIABLE( int, PARTITION_MASK )

    //for PFEM fluids application:
    KRATOS_DEFINE_VARIABLE( int, SCALE )

    //for Level Set application:
    KRATOS_DEFINE_VARIABLE( int, REFINEMENT_LEVEL )

    KRATOS_DEFINE_VARIABLE( int, STEP )
    KRATOS_DEFINE_VARIABLE( int, PRINTED_STEP )
    KRATOS_DEFINE_VARIABLE( int, PRINTED_RESTART_STEP )


    //doubles

    //for General kratos application:
    KRATOS_DEFINE_VARIABLE( double, TIME )
    KRATOS_DEFINE_VARIABLE( double, START_TIME )
    KRATOS_DEFINE_VARIABLE( double, END_TIME )
    KRATOS_DEFINE_VARIABLE( double, DELTA_TIME )
    KRATOS_DEFINE_VARIABLE( double, PREVIOUS_DELTA_TIME )
    KRATOS_DEFINE_VARIABLE( double, INTERVAL_END_TIME )

    KRATOS_DEFINE_VARIABLE( double, RESIDUAL_NORM )
    KRATOS_DEFINE_VARIABLE( double, CONVERGENCE_RATIO )

    KRATOS_DEFINE_VARIABLE( double, TEMPERATURE )
    KRATOS_DEFINE_VARIABLE( double, PRESSURE )

    KRATOS_DEFINE_VARIABLE( double, NEGATIVE_FACE_PRESSURE )
    KRATOS_DEFINE_VARIABLE( double, POSITIVE_FACE_PRESSURE )
    KRATOS_DEFINE_VARIABLE( double, FACE_HEAT_FLUX )

    KRATOS_DEFINE_VARIABLE( double, DENSITY )
    KRATOS_DEFINE_VARIABLE( double, YOUNG_MODULUS )
    KRATOS_DEFINE_VARIABLE( double, POISSON_RATIO )
    KRATOS_DEFINE_VARIABLE( double, SHEAR_MODULUS_GAMMA12)
    KRATOS_DEFINE_VARIABLE( double, SHEAR_MODULUS_GAMMA12_2)
    KRATOS_DEFINE_VARIABLE( double, SHEAR_MODULUS_GAMMA12_3)
    KRATOS_DEFINE_VARIABLE( double, SHEAR_MODULUS_GAMMA12_4)

    KRATOS_DEFINE_VARIABLE( double, THICKNESS )
    KRATOS_DEFINE_VARIABLE( double, EQUIVALENT_YOUNG_MODULUS )

    KRATOS_DEFINE_VARIABLE( double, NODAL_H )

    KRATOS_DEFINE_VARIABLE( double, KINETIC_ENERGY )
    KRATOS_DEFINE_VARIABLE( double, INTERNAL_ENERGY )
    KRATOS_DEFINE_VARIABLE( double, STRAIN_ENERGY )
    KRATOS_DEFINE_VARIABLE( double, EXTERNAL_ENERGY )
    KRATOS_DEFINE_VARIABLE( double, TOTAL_ENERGY )

    KRATOS_DEFINE_VARIABLE( double, VOLUMETRIC_STRAIN )

    KRATOS_DEFINE_VARIABLE( double, THERMAL_EXPANSION_COEFFICIENT )
    KRATOS_DEFINE_VARIABLE( double, STABILIZATION_FACTOR )
    KRATOS_DEFINE_VARIABLE( double, DETERMINANT_F )

    KRATOS_DEFINE_VARIABLE( double, GRADIENT_PENALTY_COEFFICIENT )

    //sheme info :: pass to elements
    KRATOS_DEFINE_VARIABLE( double, NEWMARK_BETA )
    KRATOS_DEFINE_VARIABLE( double, NEWMARK_GAMMA )
    KRATOS_DEFINE_VARIABLE( double, BOSSAK_ALPHA )
    KRATOS_DEFINE_VARIABLE( double, EQUILIBRIUM_POINT )

    // Lagrange multiplier for general purpose
    KRATOS_DEFINE_VARIABLE( double,            SCALAR_LAGRANGE_MULTIPLIER )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( VECTOR_LAGRANGE_MULTIPLIER )

    //for Structural application
    KRATOS_DEFINE_VARIABLE( double, FRICTION_COEFFICIENT )
    KRATOS_DEFINE_VARIABLE( double, LAMBDA )
    KRATOS_DEFINE_VARIABLE( double, MIU )
    KRATOS_DEFINE_VARIABLE( double, SCALE_FACTOR )
    KRATOS_DEFINE_VARIABLE( double, NORMAL_CONTACT_STRESS )
    KRATOS_DEFINE_VARIABLE( double, TANGENTIAL_CONTACT_STRESS )

    KRATOS_DEFINE_VARIABLE( double, PARTITION_INDEX )
    KRATOS_DEFINE_VARIABLE( double, TEMPERATURE_OLD_IT )
    KRATOS_DEFINE_VARIABLE( double, VISCOSITY )
    KRATOS_DEFINE_VARIABLE( double, ERROR_RATIO )
    KRATOS_DEFINE_VARIABLE( double, ENERGY_NORM_OVERALL )
    KRATOS_DEFINE_VARIABLE( double, ERROR_OVERALL )
    KRATOS_DEFINE_VARIABLE( double, RHS_WATER )
    KRATOS_DEFINE_VARIABLE( double, RHS_AIR )
    KRATOS_DEFINE_VARIABLE( double, WEIGHT_FATHER_NODES )
    KRATOS_DEFINE_VARIABLE( double, INITIAL_PENALTY )
    KRATOS_DEFINE_VARIABLE( double, DP_EPSILON )
    KRATOS_DEFINE_VARIABLE( double, DP_ALPHA1 )
    KRATOS_DEFINE_VARIABLE( double, DP_K )
    KRATOS_DEFINE_VARIABLE( double, INTERNAL_FRICTION_ANGLE )
    KRATOS_DEFINE_VARIABLE( double, K0 )
    KRATOS_DEFINE_VARIABLE( double, NODAL_VOLUME )

    KRATOS_DEFINE_VARIABLE( double, WATER_PRESSURE )
    KRATOS_DEFINE_VARIABLE( double, REACTION_WATER_PRESSURE )

    KRATOS_DEFINE_VARIABLE( double, WATER_PRESSURE_ACCELERATION )

    KRATOS_DEFINE_VARIABLE( double, AIR_PRESSURE )
    KRATOS_DEFINE_VARIABLE( double, REACTION_AIR_PRESSURE )
    KRATOS_DEFINE_VARIABLE( double, FLAG_VARIABLE )
    KRATOS_DEFINE_VARIABLE( double,  DISTANCE )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( DISTANCE_GRADIENT )

    KRATOS_DEFINE_VARIABLE( double, LAGRANGE_AIR_PRESSURE )
    KRATOS_DEFINE_VARIABLE( double, LAGRANGE_WATER_PRESSURE )
    KRATOS_DEFINE_VARIABLE( double, LAGRANGE_TEMPERATURE )

    //MeshMovingApplication
    KRATOS_DEFINE_VARIABLE( double, AUX_MESH_VAR )

    //for MultiScale application
    KRATOS_DEFINE_VARIABLE( Vector, INITIAL_STRAIN )
    KRATOS_DEFINE_VARIABLE( Vector, INITIAL_STRESS )
    KRATOS_DEFINE_VARIABLE( Matrix, INITIAL_DEFORMATION_GRADIENT )
    KRATOS_DEFINE_VARIABLE( double, COEFFICIENT_THERMAL_EXPANSION )
    KRATOS_DEFINE_VARIABLE( double, CHARACTERISTIC_LENGTH_MULTIPLIER )

    //Adjoint
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( ADJOINT_VECTOR_1 )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( ADJOINT_VECTOR_2 )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( ADJOINT_VECTOR_3 )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( AUX_ADJOINT_VECTOR_1 )
    KRATOS_DEFINE_VARIABLE(double, ADJOINT_SCALAR_1 )
    KRATOS_DEFINE_VARIABLE(double, SCALAR_SENSITIVITY )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( SHAPE_SENSITIVITY )
    KRATOS_DEFINE_VARIABLE(double, NORMAL_SENSITIVITY )
    KRATOS_DEFINE_VARIABLE(double, NUMBER_OF_NEIGHBOUR_ELEMENTS )
    KRATOS_DEFINE_VARIABLE(bool, UPDATE_SENSITIVITIES )
    KRATOS_DEFINE_VARIABLE(AdjointExtensions::Pointer, ADJOINT_EXTENSIONS )

    // For MeshingApplication
    KRATOS_DEFINE_VARIABLE( double, NODAL_ERROR )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( NODAL_ERROR_COMPONENTS )
    KRATOS_DEFINE_VARIABLE( double, ELEMENT_ERROR )
    KRATOS_DEFINE_VARIABLE( double, ELEMENT_H )
    KRATOS_DEFINE_VARIABLE( Vector, RECOVERED_STRESS )
    KRATOS_DEFINE_VARIABLE( double, ERROR_INTEGRATION_POINT )
    KRATOS_DEFINE_VARIABLE( double, CONTACT_PRESSURE )

    //for PFEM fluids application:
    KRATOS_DEFINE_VARIABLE( double, NODAL_AREA )

    KRATOS_DEFINE_VARIABLE( double, BULK_MODULUS )
    KRATOS_DEFINE_VARIABLE( double, SATURATION )
    KRATOS_DEFINE_VARIABLE( double, DENSITY_WATER )
    KRATOS_DEFINE_VARIABLE( double, VISCOSITY_WATER )
    KRATOS_DEFINE_VARIABLE( double, DENSITY_AIR )
    KRATOS_DEFINE_VARIABLE( double, VISCOSITY_AIR )
    KRATOS_DEFINE_VARIABLE( double, POROSITY )
    KRATOS_DEFINE_VARIABLE( double, DIAMETER )
    KRATOS_DEFINE_VARIABLE( double, LIN_DARCY_COEF )
    KRATOS_DEFINE_VARIABLE( double, NONLIN_DARCY_COEF )

    KRATOS_DEFINE_VARIABLE( double, AIR_ENTRY_VALUE )
    KRATOS_DEFINE_VARIABLE( double, FIRST_SATURATION_PARAM )
    KRATOS_DEFINE_VARIABLE( double, SECOND_SATURATION_PARAM )
    KRATOS_DEFINE_VARIABLE( double, PERMEABILITY_WATER )
    KRATOS_DEFINE_VARIABLE( double, PERMEABILITY_AIR )
    KRATOS_DEFINE_VARIABLE( double, BULK_AIR )

    KRATOS_DEFINE_VARIABLE( double, TEMP_CONV_PROJ )
    KRATOS_DEFINE_VARIABLE( double, CONVECTION_COEFFICIENT )

    KRATOS_DEFINE_VARIABLE( double, SOUND_VELOCITY )
    KRATOS_DEFINE_VARIABLE( double, AIR_SOUND_VELOCITY )
    KRATOS_DEFINE_VARIABLE( double, WATER_SOUND_VELOCITY )
    KRATOS_DEFINE_VARIABLE( double, NODAL_MASS )
    KRATOS_DEFINE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS( NODAL_INERTIA_TENSOR )
    KRATOS_DEFINE_VARIABLE( double, AUX_INDEX )
    KRATOS_DEFINE_VARIABLE( double, VELOCITY_PERIOD )
    KRATOS_DEFINE_VARIABLE( double, ANGULAR_VELOCITY_PERIOD )
    KRATOS_DEFINE_VARIABLE( std::string, IDENTIFIER )

    //for Other applications:
    KRATOS_DEFINE_VARIABLE( double, ARRHENIUS )
    KRATOS_DEFINE_VARIABLE( double, ARRHENIUSAUX )
    KRATOS_DEFINE_VARIABLE( double, ARRHENIUSAUX_ )
    KRATOS_DEFINE_VARIABLE( double, PRESSUREAUX )
    KRATOS_DEFINE_VARIABLE( double, NODAL_MAUX )
    KRATOS_DEFINE_VARIABLE( double, NODAL_PAUX )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( NODAL_VAUX )
    KRATOS_DEFINE_VARIABLE( double, HEAT_FLUX )
    KRATOS_DEFINE_VARIABLE( double, REACTION_FLUX )
    KRATOS_DEFINE_VARIABLE( double, TC )
    KRATOS_DEFINE_VARIABLE( double, CONDUCTIVITY )
    KRATOS_DEFINE_VARIABLE( double, SPECIFIC_HEAT )
    KRATOS_DEFINE_VARIABLE( double, MATERIAL_VARIABLE )
    KRATOS_DEFINE_VARIABLE( double, FUEL )
    KRATOS_DEFINE_VARIABLE( double, YO )
    KRATOS_DEFINE_VARIABLE( double, YF )
    KRATOS_DEFINE_VARIABLE( double, YI )
    KRATOS_DEFINE_VARIABLE( double, Y1 )
    KRATOS_DEFINE_VARIABLE( double, Y2 )
    KRATOS_DEFINE_VARIABLE( double, YP )

    KRATOS_DEFINE_VARIABLE( double, ABSORPTION_COEFFICIENT )
    KRATOS_DEFINE_VARIABLE( double, STEFAN_BOLTZMANN_CONSTANT )

    KRATOS_DEFINE_VARIABLE( double, EXTERNAL_PRESSURE )

    KRATOS_DEFINE_VARIABLE( double, EMISSIVITY )
    KRATOS_DEFINE_VARIABLE( double, ENTHALPY )
    KRATOS_DEFINE_VARIABLE( double, MIXTURE_FRACTION )


    KRATOS_DEFINE_VARIABLE( double, YCH4 )
    KRATOS_DEFINE_VARIABLE( double, YO2 )
    KRATOS_DEFINE_VARIABLE( double, YCO2 )
    KRATOS_DEFINE_VARIABLE( double, YH2O )
    KRATOS_DEFINE_VARIABLE( double, YN2 )

    KRATOS_DEFINE_VARIABLE( double, WET_VOLUME )
    KRATOS_DEFINE_VARIABLE( double, CUTTED_AREA )
    KRATOS_DEFINE_VARIABLE( double, NET_INPUT_MATERIAL )

    KRATOS_DEFINE_VARIABLE( double, INCIDENT_RADIATION_FUNCTION )

    KRATOS_DEFINE_VARIABLE( double, SWITCH_TEMPERATURE )
    KRATOS_DEFINE_VARIABLE( double, NODAL_SWITCH )

    //for Xfem application:
    KRATOS_DEFINE_VARIABLE( double, CRACK_OPENING )
    KRATOS_DEFINE_VARIABLE( double, CRACK_TRANSLATION )

    //for Level Set application:
    KRATOS_DEFINE_VARIABLE( double, MIN_DT )
    KRATOS_DEFINE_VARIABLE( double, MAX_DT )
    KRATOS_DEFINE_VARIABLE( double, VEL_ART_VISC )
    KRATOS_DEFINE_VARIABLE( double, PR_ART_VISC )

    //for Vulcan application
    KRATOS_DEFINE_VARIABLE( double, LATENT_HEAT )
    KRATOS_DEFINE_VARIABLE( double, AMBIENT_TEMPERATURE )

    //vectors

    //for General kratos application:
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( ROTATION )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( DELTA_ROTATION )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( REACTION_MOMENT )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( ANGULAR_VELOCITY )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( ANGULAR_ACCELERATION )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( VELOCITY_LAPLACIAN )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( VELOCITY_LAPLACIAN_RATE )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( VELOCITY_COMPONENT_GRADIENT )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( VELOCITY_X_GRADIENT )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( VELOCITY_Y_GRADIENT )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( VELOCITY_Z_GRADIENT )

    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( DISPLACEMENT )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( REACTION )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( VELOCITY )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( ACCELERATION )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( STEP_ROTATION )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( STEP_DISPLACEMENT )

    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( VOLUME_ACCELERATION )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( FORCE )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( TORQUE )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( MOMENT )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( FACE_LOAD )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( NORMAL )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( TANGENT_XI )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( TANGENT_ETA )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( BODY_FORCE )

    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( FORCE_RESIDUAL )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( MOMENT_RESIDUAL )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( INTERNAL_FORCE )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( EXTERNAL_FORCE )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( CONTACT_FORCE )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( CONTACT_NORMAL )

    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( LINEAR_MOMENTUM )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( ANGULAR_MOMENTUM )

    KRATOS_DEFINE_VARIABLE( Vector, EXTERNAL_FORCES_VECTOR )
    KRATOS_DEFINE_VARIABLE( Vector, INTERNAL_FORCES_VECTOR )
    KRATOS_DEFINE_VARIABLE( Vector, CONTACT_FORCES_VECTOR )

    KRATOS_DEFINE_VARIABLE( Vector, CAUCHY_STRESS_VECTOR )
    KRATOS_DEFINE_VARIABLE( Vector, PK2_STRESS_VECTOR )

    KRATOS_DEFINE_VARIABLE( Vector, RESIDUAL_VECTOR )

    KRATOS_DEFINE_VARIABLE( Vector, MARKER_LABELS )
    KRATOS_DEFINE_VARIABLE( Vector, MARKER_MESHES )

    KRATOS_DEFINE_VARIABLE( Vector, CONSTRAINT_LABELS )
    KRATOS_DEFINE_VARIABLE( Vector, CONSTRAINT_MESHES )

    KRATOS_DEFINE_VARIABLE( Vector, LOAD_LABELS )
    KRATOS_DEFINE_VARIABLE( Vector, LOAD_MESHES )

    //for Structural application:
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( MOMENTUM )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( LAGRANGE_DISPLACEMENT )

    KRATOS_DEFINE_VARIABLE( Vector, ELEMENTAL_DISTANCES )
    KRATOS_DEFINE_VARIABLE( Vector, MATERIAL_PARAMETERS )
    KRATOS_DEFINE_VARIABLE( Vector, INTERNAL_VARIABLES )
    KRATOS_DEFINE_VARIABLE( Vector, INSITU_STRESS )

    KRATOS_DEFINE_VARIABLE( Vector, PENALTY )
    KRATOS_DEFINE_VARIABLE( Vector, NORMAL_STRESS )
    KRATOS_DEFINE_VARIABLE( Vector, TANGENTIAL_STRESS )
    KRATOS_DEFINE_VARIABLE( Vector, STRESSES )
    KRATOS_DEFINE_VARIABLE( Vector, STRAIN )

    KRATOS_DEFINE_VARIABLE( DenseVector<int>, NEIGHBOURS_INDICES )

    //ALE Application
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( DETERMINANT )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( ELEMENTSHAPE )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( MESH_VELOCITY )

    //for PFEM fluids application:
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( GRAVITY )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( NORMAL_TO_WALL )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( STRUCTURE_VELOCITY )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( DRAG_FORCE )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( EMBEDDED_VELOCITY )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( FRACT_VEL )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( ROTATION_CENTER )

    //for Other applications:
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( SEEPAGE_DRAG )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( MASS )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( RHS )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( FORCE_CM )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( MOMENTUM_CM )

    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( DIRECTION )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( Y )

    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( LOCAL_AXIS_1 )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( LOCAL_AXIS_2 )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( LOCAL_AXIS_3 )


    KRATOS_DEFINE_VARIABLE( Vector, BDF_COEFFICIENTS )

    //for Vulcan application
    KRATOS_DEFINE_VARIABLE( Vector, ENRICHED_PRESSURES )

    //matrices

    //for General kratos application:
    KRATOS_DEFINE_VARIABLE( Matrix, GREEN_LAGRANGE_STRAIN_TENSOR )
    KRATOS_DEFINE_VARIABLE( Matrix, PK2_STRESS_TENSOR )
    KRATOS_DEFINE_VARIABLE( Matrix, CAUCHY_STRESS_TENSOR )
    KRATOS_DEFINE_VARIABLE( Matrix, LOCAL_INERTIA_TENSOR )
    KRATOS_DEFINE_VARIABLE( Matrix, LOCAL_AXES_MATRIX )
    KRATOS_DEFINE_VARIABLE( Matrix, LOCAL_CONSTITUTIVE_MATRIX )
    KRATOS_DEFINE_VARIABLE( Matrix, CONSTITUTIVE_MATRIX )
    KRATOS_DEFINE_VARIABLE( Matrix, DEFORMATION_GRADIENT )
    KRATOS_DEFINE_VARIABLE( Matrix, MATERIAL_STIFFNESS_MATRIX )
    KRATOS_DEFINE_VARIABLE( Matrix, GEOMETRIC_STIFFNESS_MATRIX )

    //for Structural application:
    KRATOS_DEFINE_VARIABLE( Matrix, INERTIA )

    //for General kratos application:

    //for Structural application:
    //KRATOS_DEFINE_VARIABLE( GlobalPointersVector< GeometricalObject >, NEIGHBOUR_EMBEDDED_FACES )
    //KRATOS_DEFINE_VARIABLE( ConvectionDiffusionSettings::Pointer,  CONVECTION_DIFFUSION_SETTINGS )
    //KRATOS_DEFINE_VARIABLE( RadiationSettings::Pointer,  RADIATION_SETTINGS )

    KRATOS_DEFINE_VARIABLE( PeriodicVariablesContainer, PERIODIC_VARIABLES )

    // Variables that should be moved to applications (but have too many dependencies)
    KRATOS_DEFINE_VARIABLE(int, FRACTIONAL_STEP)
    KRATOS_DEFINE_VARIABLE(double, EQ_STRAIN_RATE)
    KRATOS_DEFINE_VARIABLE(double, POWER_LAW_N)
    KRATOS_DEFINE_VARIABLE(double, POWER_LAW_K)
    KRATOS_DEFINE_VARIABLE(double, YIELD_STRESS)
    KRATOS_DEFINE_VARIABLE(double, MU)
    KRATOS_DEFINE_VARIABLE(double, TAU)

    KRATOS_DEFINE_VARIABLE(double, SEARCH_RADIUS )

    KRATOS_DEFINE_VARIABLE(double, INTEGRATION_WEIGHT )
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( INTEGRATION_COORDINATES )
    KRATOS_DEFINE_VARIABLE(TableStreamUtility::Pointer, TABLE_UTILITY )

}  // namespace Kratos.

#undef  KRATOS_EXPORT_MACRO
#define KRATOS_EXPORT_MACRO KRATOS_NO_EXPORT

#endif // KRATOS_VARIABLES_H_INCLUDED  defined
