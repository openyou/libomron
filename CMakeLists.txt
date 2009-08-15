######################################################################################
# CMake directives
######################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

SET(BUILDSYS_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../build_sys/cmake/")
LIST(APPEND CMAKE_MODULE_PATH "${BUILDSYS_CMAKE_DIR}")
INCLUDE( ${BUILDSYS_CMAKE_DIR}/BuildSysCMakeLib.cmake )

#setting link directory policy

IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

######################################################################################
# Project declaration and options
######################################################################################

#Project declaration

PROJECT(LIBOMRON)
INITIALIZE_BUILD()

#Common Options

OPTION_LIBRARY_BUILD_STATIC(ON)

######################################################################################
# Project specific globals
######################################################################################

#library definitions

SET(LIBOMRON_MAJOR_VERSION 0)
SET(LIBOMRON_MINOR_VERSION 1)
SET(LIBOMRON_BUILD_VERSION 0)

SET(LIBOMRON_VERSION ${LIBOMRON_MAJOR_VERSION}.${LIBOMRON_MINOR_VERSION}.${LIBOMRON_BUILD_VERSION})

#library name definitions

SET(LIBOMRON_C "omron")
SET(LIBOMRON_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples/omron_790IT_test)
ADD_SUBDIRECTORY(examples/omron_720IT_test)
