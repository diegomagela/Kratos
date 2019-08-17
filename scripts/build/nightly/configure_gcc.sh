BOOST_DIR="${HOME}/CompiledLibs/boost_1_59_0"
PYTHON_VERSION_S=3
PYTHON_VERSION_M=5

cmake .. \
-DCMAKE_C_COMPILER=gcc                                                                          \
-DCMAKE_INSTALL_RPATH="${HOME}/Kratos/libs"                                                     \
-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE                                                        \
-DCMAKE_CXX_COMPILER=g++                                                                        \
-DCMAKE_BUILD_TYPE=Custom                                                                       \
-DCMAKE_C_FLAGS="${CMAKE_C_FLAGS} -msse3 -fopenmp"                                              \
-DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -msse3 -std=c++11 -fopenmp"                               \
-DBOOST_ROOT="${BOOST_DIR}"                                                                     \
-DPYTHON_EXECUTABLE="/usr/bin/python${PYTHON_VERSION_S}.${PYTHON_VERSION_M}"                    \
-DMESH_MOVING_APPLICATION=ON                                                                    \
-DCONVECTION_DIFFUSION_APPLICATION=ON                                                           \
-DHDF5_APPLICATION=ON                                                                           \
-DDAM_APPLICATION=ON                                                                            \
-DDEM_APPLICATION=ON                                                                            \
-DCO_SIMULATION_APPLICATION=ON                                                                  \
-DEXTERNAL_SOLVERS_APPLICATION=ON                                                               \
-DFSI_APPLICATION=ON                                                                            \
-DFLUID_DYNAMICS_APPLICATION=ON                                                                 \
-DCSHARP_WRAPPER_APPLICATION=ON                                                                 \
-DMESHING_APPLICATION=ON                                                                        \
-DINCLUDE_MMG=ON                                                                                \
-DMMG_INCLUDE_DIR="${HOME}/MMGPrecompiled-master/include/"                                      \
-DMMG2D_INCLUDE_DIR="${HOME}/MMGPrecompiled-master/include/mmg/mmg2d/"                          \
-DMMG3D_INCLUDE_DIR="${HOME}/MMGPrecompiled-master/include/mmg/mmg3d/"                          \
-DMMGS_INCLUDE_DIR="${HOME}/MMGPrecompiled-master/include/mmg/mmgs/"                            \
-DMMG_LIBRARY="${HOME}/MMGPrecompiled-master/lib/libmmg.a"                                      \
-DMMG2D_LIBRARY="${HOME}/MMGPrecompiled-master/lib/libmmg2d.a"                                  \
-DMMG3D_LIBRARY="${HOME}/MMGPrecompiled-master/lib/libmmg3d.a"                                  \
-DMMGS_LIBRARY="${HOME}/MMGPrecompiled-master/lib/libmmgs.a"                                    \
-DEIGEN_SOLVERS_APPLICATION=ON                                                                  \
-DUSE_EIGEN_MKL=OFF                                                                             \
-DEIGEN_ROOT="${HOME}/eigen/"                                                                   \
-DMULTISCALE_APPLICATION=OFF                                                                    \
-DPARTICLE_MECHANICS_APPLICATION=ON                                                             \
-DPFEM_APPLICATION=OFF                                                                          \
-DCONTACT_MECHANICS_APPLICATION=OFF                                                             \
-DPFEM_FLUID_DYNAMICS_APPLICATION=OFF                                                           \
-DPFEM_SOLID_MECHANICS_APPLICATION=OFF                                                          \
-DPOROMECHANICS_APPLICATION=ON                                                                  \
-DSHAPE_OPTIMIZATION_APPLICATION=ON                                                             \
-DSOLID_MECHANICS_APPLICATION=ON                                                                \
-DCONSTITUTIVE_MODELS_APPLICATION=ON                                                            \
-DSTRUCTURAL_MECHANICS_APPLICATION=ON                                                           \
-DIGA_APPLICATION=ON                                                                            \
-DANURBS_ROOT="${HOME}/ANurbs"                                                                  \
-DSWIMMING_DEM_APPLICATION=OFF                                                                  \
-DTHERMO_MECHANICAL_APPLICATION=ON                                                              \
-DCONTACT_STRUCTURAL_MECHANICS_APPLICATION=ON                                                   \
-DMAPPING_APPLICATION=ON                                                                        \
-DMETIS_APPLICATION=OFF                                                                         \
-DCOMPRESSIBLE_POTENTIAL_FLOW_APPLICATION=ON                                                    \
-DPARMETIS_ROOT_DIR=\"UNSET\"                                                                   \
-DTRILINOS_APPLICATION=OFF                                                                      \
-DTRILINOS_ROOT=\"UNSET\"                                                                       \
-DINSTALL_EMBEDDED_PYTHON=ON                                                                    \
-DINCLUDE_FEAST=ON                                                                              \
