######################################################################################
# CMake directives
######################################################################################

#Require 2.6 or higher. 
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

#See if we've got our cmake modules checked out. If not, advise to do so.
IF(EXISTS "${CMAKE_SOURCE_DIR}/../cmake_modules")
  SET(NP_CMAKE_DIR "${CMAKE_SOURCE_DIR}/../cmake_modules")
  MESSAGE(STATUS "Using outside cmake_modules directory ${NP_CMAKE_DIR}")
ELSEIF(EXISTS "${CMAKE_SOURCE_DIR}/cmake_modules")
  SET(NP_CMAKE_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
  MESSAGE(STATUS "Using cmake_modules git submodule ${NP_CMAKE_DIR}")
ELSE(EXISTS "${CMAKE_SOURCE_DIR}/cmake_modules")
  MESSAGE(FATAL_ERROR 
	"Cannot find cmake_modules directory for np labs project functions.\n"
	"Make sure you've either put the cmake_modules directory in the same root as your repository directory, or that you've used the cmake_modules submodule (git submodule update --init).\n"
	"NP Labs cmake_module repository is available at git://github.com/qdot/cmake_modules.git"
	)
ENDIF(EXISTS "${CMAKE_SOURCE_DIR}/../cmake_modules")

LIST(APPEND CMAKE_MODULE_PATH ${NP_CMAKE_DIR})

INCLUDE( ${NP_CMAKE_DIR}/NPCMakeLib.cmake )

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
