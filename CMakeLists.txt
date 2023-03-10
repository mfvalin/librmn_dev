# The minimum version can not be 3.16 because of the LINK_LANGUAGE generator expression in Config.cmake.in
cmake_minimum_required(VERSION 3.20)

message(STATUS "(EC) Generating librmn Makefile")

# Append EC specific module path
list(APPEND CMAKE_MODULE_PATH $ENV{EC_CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_rpn/modules)

include(ec_init)           # Initialise compilers and ec specific functions
ec_git_version()           # Get version from git state
ec_parse_manifest()        # Parse MANIFEST file

project(${NAME} DESCRIPTION "${DESCRIPTION}" LANGUAGES C Fortran)
set(PROJECT_VERSION ${VERSION}${STATE})
set(rmn_VERSION ${PROJECT_VERSION} CACHE INTERNAL "rmn version" FORCE) # Needed for cascaded version identification
message(STATUS "(EC) ${PROJECT_NAME} version = ${PROJECT_VERSION}")
string(REGEX MATCH "([0-9]+)" SOVERSION ${PROJECT_VERSION})

ec_build_info()            # Generate build information

include(ec_compiler_presets)

# Need to add HAVE_RMN definition for auto-registering librmn within App
add_compile_definitions(HAVE_RMN=\"${PROJECT_VERSION}\")
add_subdirectory(App App)

set(WITH_OPENMP TRUE CACHE BOOL "Control whether to use OpenMP")
include(ec_openmp)          # Enable OpenMP (-DWITH_OPENMP=TRUE)
include(ec_doxygen)         # Doxygen target doc

# Fortran compiler options required to build this project
set(Intel_FFLAGS -convert big_endian -align array32byte -assume byterecl)
set(GNU_FFLAGS -fconvert=big-endian -fcray-pointer -frecord-marker=4 -fno-second-underscore)
set(Flang_FFLAGS -Mbyteswapio)
set(PGI_FFLAGS -byteswapio)

# This is required when using GNU compilers without OpenMP.
# It has no adverse effect with other compilers and/or when OpenMP is enabled.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ldl -pthread")
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -ldl -pthread")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -ldl -pthread")

find_package(MPI)
#set(CMAKE_Fortran_COMPILER ${MPI_Fortran_COMPILER})
#set(CMAKE_C_COMPILER ${MPI_C_COMPILER})

#set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_definitions(_${CMAKE_SYSTEM_NAME}_ _GNU_SOURCE)

option(EXTRA_DEBUG_OUTPUT "Enable extra debug output" OFF)
if(EXTRA_DEBUG_OUTPUT)
    add_compile_definitions(DEBUG)
endif()

# Figure out the correct arch_dependent include path
set(BUILD_ARCH "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Build architecture: ${BUILD_ARCH}")

# TODO Adapt Config.cmake.in to be ready for this
#set(CMAKE_DEBUG_POSTFIX "d")

include_directories(
   include
   src/PUBLIC_INCLUDES
   src/PUBLIC_INCLUDES/rmn
)

file(GLOB PROJECT_C_FILES
   src/*/*.c
)
file(GLOB PROJECT_F_FILES
   src/*/*.F90
   src/*/*.F
   src/*/*.f
   src/*/*.f90
)
file(GLOB PROJECT_H_FILES
   src/*/*.h
   src/*/*.inc
   src/*/*.hf
   src/*/*.cdk
   src/*/*.tpl*
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/rmn)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
set(MODULE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)

# We need to create a file here because gfortran won't process a file nammmed *.hf
file(GLOB WHITEBOARD_HEADER_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/src/whiteboard/*.tmpl90 ${CMAKE_CURRENT_SOURCE_DIR}/src/whiteboard/*.hf)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rmn/WhiteBoard.hf
    PRE_BUILD
    COMMAND sed -e "'s/ \"/ </g'" -e "'s/\"/>/g'" ${CMAKE_CURRENT_SOURCE_DIR}/src/whiteboard/WhiteBoard.hf > ${CMAKE_CURRENT_BINARY_DIR}/whiteboard_header.F90
    COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/WhiteBoard.hf
    COMMAND cp -av ${CMAKE_CURRENT_SOURCE_DIR}/src/whiteboard/WhiteBoard_constants.h ${CMAKE_CURRENT_BINARY_DIR}/include/rmn/WhiteBoard.hf
    COMMAND ${CMAKE_Fortran_COMPILER} -I${CMAKE_CURRENT_SOURCE_DIR}/include -I${CMAKE_CURRENT_SOURCE_DIR}/src/PUBLIC_INCLUDES -I${CMAKE_CURRENT_SOURCE_DIR}/src/whiteboard -P -E ${CMAKE_CURRENT_BINARY_DIR}/whiteboard_header.F90 >> ${CMAKE_CURRENT_BINARY_DIR}/include/rmn/WhiteBoard.hf
    COMMENT "Generating WhiteBoard.hf"
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/src/whiteboard/WhiteBoard.hf
    DEPENDS ${WHITEBOARD_HEADER_DEPS}

    USES_TERMINAL
)
# Add a custom target to regenerate WhiteBoard.hf
add_custom_target(WhiteBoardFortranHeader DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rmn/WhiteBoard.hf)

# Object collection for rmn[shared]
add_library(rmn-obj OBJECT ${PROJECT_C_FILES} ${PROJECT_F_FILES})
set_target_properties(rmn-obj PROPERTIES 
   Fortran_MODULE_DIRECTORY ${MODULE_DIR}
   POSITION_INDEPENDENT_CODE ON)

target_link_libraries(rmn-obj PUBLIC App)
add_dependencies(rmn-obj rmn_build_info)
target_include_directories(rmn-obj INTERFACE $<BUILD_INTERFACE:${MODULE_DIR}>)

# Build static library from object collection
add_library(rmn STATIC)
target_link_libraries(rmn PUBLIC rmn-obj)

# Build shared library from object collection
add_library(rmnshared SHARED)
target_link_libraries(rmnshared PUBLIC rmn-obj)

set_target_properties(rmn rmnshared PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/PUBLIC_INCLUDES/rmn.h
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME rmn
    SOVERSION ${SOVERSION})

set(TARGETS rmn rmnshared)

if(OpenMP_FOUND)
   set(TARGETS rmn rmnshared rmn-ompi rmnshared-ompi)

   # Object library for rmn[shared]-ompi
   add_library(rmn-obj-ompi OBJECT ${PROJECT_C_FILES} ${PROJECT_F_FILES})
   add_dependencies(rmn-obj-ompi rmn_build_info)
   target_link_libraries(rmn-obj-ompi PUBLIC App-ompi OpenMP::OpenMP_C OpenMP::OpenMP_Fortran MPI::MPI_C MPI::MPI_Fortran)
   target_compile_definitions(rmn-obj-ompi PUBLIC HAVE_MPI HAVE_OPENMP)
   set_target_properties(rmn-obj-ompi PROPERTIES
       POSITION_INDEPENDENT_CODE ON
       Fortran_MODULE_DIRECTORY ${MODULE_DIR}/ompi
   )
   target_include_directories(rmn-obj-ompi INTERFACE $<BUILD_INTERFACE:${MODULE_DIR}/ompi>)

   # Build static library from object collection
   add_library(rmn-ompi STATIC)
   target_link_libraries(rmn-ompi PUBLIC rmn-obj-ompi)

   # Build shared library from object collection
   add_library(rmnshared-ompi SHARED)
   target_link_libraries(rmnshared-ompi PUBLIC rmn-obj-ompi)

   set_target_properties(rmn-ompi rmnshared-ompi PROPERTIES
      VERSION ${PROJECT_VERSION}
      PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/PUBLIC_INCLUDES/rmn.h
      OUTPUT_NAME rmn-ompi
      SOVERSION ${SOVERSION})

   # Symbolic link for archive numbered version
   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
       COMMAND ln -sfv lib${PROJECT_NAME}-ompi.a lib${PROJECT_NAME}-ompi_${PROJECT_VERSION}.a)
   install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}-ompi_${PROJECT_VERSION}.a DESTINATION lib)
endif()

foreach(TARGET IN ITEMS ${TARGETS})
    target_compile_options(${TARGET}
        PUBLIC $<$<COMPILE_LANG_AND_ID:Fortran,GNU>:${GNU_FFLAGS}>
               $<$<COMPILE_LANG_AND_ID:Fortran,Intel>:${Intel_FFLAGS}>
               $<$<COMPILE_LANG_AND_ID:Fortran,Flang>:${Flang_FFLAGS}>
               $<$<COMPILE_LANG_AND_ID:Fortran,PGI>:${PGI_FFLAGS}>
               $<$<COMPILE_LANG_AND_ID:Fortran,NVHPC>:${PGI_FFLAGS}>)
    add_dependencies(${TARGET}
        ${PROJECT_NAME}_build_info
        WhiteBoardFortranHeader)

    # Control where the Fortran modules *.mod will end up
    set_target_properties(${TARGET} PROPERTIES Fortran_MODULE_DIRECTORY ${MODULE_DIR})
    target_include_directories(${TARGET} INTERFACE ${MODULE_DIR})
endforeach()

install(TARGETS ${TARGETS})

# Symbolic link for archive numbered version
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ln -sfv lib${PROJECT_NAME}.a lib${PROJECT_NAME}_${PROJECT_VERSION}.a)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}_${PROJECT_VERSION}.a DESTINATION lib)

# Fortran modules are installed in the include directory for now
install(FILES
    ${MODULE_DIR}/rmn_jar.mod
    ${MODULE_DIR}/rmn_gmm.mod
    DESTINATION include)

file(GLOB PROJECT_INCLUDE_FILES src/PUBLIC_INCLUDES/rmn/*)
# WhiteBoard.hf is generated by make and doesn't exist when cmake is executed
# It must therefore be added explicitly
list(APPEND PROJECT_INCLUDE_FILES ${CMAKE_CURRENT_BINARY_DIR}/include/rmn/WhiteBoard.hf)
install(FILES ${PROJECT_INCLUDE_FILES} DESTINATION include/rmn)
add_custom_target(
    cprotos
    ${CMAKE_CURRENT_SOURCE_DIR}/extractProtos.bash ${CMAKE_CURRENT_SOURCE_DIR}/src ${PROJECT_C_FILES}
    #WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Extract function prototypes from C source code"
)

# Generate the config file for the project to be usable via cmake's find_package command
set(INCLUDE_INSTALL_DIR include)
set(LIB_INSTALL_DIR lib)
set(CONFIG_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}")

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}"
    PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${CONFIG_INSTALL_DIR}"
)

# Packaging
ec_package_name()                 # Define package prefix  
ec_build_config()                 # Create build configuration script
ec_prepare_ssm()                  # Prepare ssm packaging files

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_VENDOR "ECCC")
set(CPACK_PACKAGE_CONTACT "${MAINTAINER}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/package")
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CPACK_PACKAGE_FILE_NAME "${PACKAGE_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${NAME}_${PROJECT_VERSION}")
include(CPack)

if (EC_INIT_DONE LESS 2)
   include(CTest)
   add_subdirectory(tests)
endif()


