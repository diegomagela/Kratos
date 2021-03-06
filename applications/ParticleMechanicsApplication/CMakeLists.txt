set(CMAKE_INCLUDE_CURRENT_DIR ON)

message("**** configuring KratosParticleMechanicsApplication ****")

################### PYBIND11
include(pybind11Tools)

include_directories( ${CMAKE_SOURCE_DIR}/kratos )

# Define application core sources (pure C++ parts)
file(GLOB_RECURSE KRATOS_PARTICLE_MECHANICS_APPLICATION_CORE
    ${CMAKE_CURRENT_SOURCE_DIR}/particle_mechanics_application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/particle_mechanics_application_variables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_conditions/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_constitutive/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_elements/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_modelers/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_processes/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_strategies/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_utilities/*.cpp
)

# If there is mpi support, use..
if((${USE_MPI} MATCHES ON) )
  file( GLOB KRATOS_PARTICLE_MECHANICS_APPLICATION_CORE ${KRATOS_PARTICLE_MECHANICS_APPLICATION_CORE}
  ${CMAKE_CURRENT_SOURCE_DIR}/custom_utilities/mpi/*.cpp
  )
endif((${USE_MPI} MATCHES ON) )

# Define python interface sources
file(GLOB_RECURSE KRATOS_PARTICLE_MECHANICS_APPLICATION_PYTHON_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/custom_python/*.cpp)

# Generate variables with the testing sources
if(${KRATOS_BUILD_TESTING} MATCHES ON)
  file(GLOB_RECURSE  KRATOS_PARTICLE_MECHANICS_APPLICATION_TESTING_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp_tests/*.cpp)
endif(${KRATOS_BUILD_TESTING} MATCHES ON)

add_library(KratosParticleMechanicsCore SHARED ${KRATOS_PARTICLE_MECHANICS_APPLICATION_CORE} ${KRATOS_PARTICLE_MECHANICS_APPLICATION_TESTING_SOURCES})
target_link_libraries(KratosParticleMechanicsCore PUBLIC KratosCore)
set_target_properties(KratosParticleMechanicsCore PROPERTIES COMPILE_DEFINITIONS "PARTICLE_MECHANICS_APPLICATION=EXPORT,API")

# Define Kratos library which defines the basic python interface
pybind11_add_module(KratosParticleMechanicsApplication MODULE THIN_LTO ${KRATOS_PARTICLE_MECHANICS_APPLICATION_PYTHON_INTERFACE})
target_link_libraries(KratosParticleMechanicsApplication PRIVATE KratosParticleMechanicsCore)
set_target_properties(KratosParticleMechanicsApplication PROPERTIES PREFIX "")

# changing the .dll suffix to .pyd
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set_target_properties(KratosParticleMechanicsApplication PROPERTIES SUFFIX .pyd)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# changing the .dylib suffix to .so (OS X)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(KratosParticleMechanicsApplication PROPERTIES SUFFIX .so)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Cotire
if(USE_COTIRE MATCHES ON)
    cotire(KratosParticleMechanicsCore)
    cotire(KratosParticleMechanicsApplication)
endif(USE_COTIRE MATCHES ON)

# Add to the KratosMultiphisics Python module
kratos_python_install(${INSTALL_PYTHON_USING_LINKS} ${CMAKE_CURRENT_SOURCE_DIR}/ParticleMechanicsApplication.py KratosMultiphysics/ParticleMechanicsApplication/__init__.py )

# Install python files
get_filename_component (CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
kratos_python_install_directory(${INSTALL_PYTHON_USING_LINKS} ${CMAKE_CURRENT_SOURCE_DIR}/python_scripts KratosMultiphysics/${CURRENT_DIR_NAME} )

# Kratos Testing. Install everything except sources to ensure that reference and configuration files are copied.
if(${INSTALL_TESTING_FILES} MATCHES ON )
    get_filename_component (CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests DESTINATION applications/${CURRENT_DIR_NAME}
        PATTERN "*.git" EXCLUDE
        PATTERN "*.c" EXCLUDE
        PATTERN "*.h" EXCLUDE
        PATTERN "*.cpp" EXCLUDE
        PATTERN "*.hpp" EXCLUDE
  )
endif(${INSTALL_TESTING_FILES} MATCHES ON)

# Install targets
install(TARGETS KratosParticleMechanicsCore DESTINATION libs )
install(TARGETS KratosParticleMechanicsApplication DESTINATION libs )

