######################################################################################
# CMake directives
######################################################################################

#Require 2.6 or higher. 
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

#See if we've got our cmake modules checked out. If not, advise to do so.

# Does it exist next to the repo? This is usually where it is for development.
IF(EXISTS "${CMAKE_SOURCE_DIR}/../compily_buildd/cmake")
  SET(BUILDSYS_CMAKE_DIR "${CMAKE_SOURCE_DIR}/../compily_buildd/cmake")
  MESSAGE(STATUS "Using outside compily_buildd directory ${BUILDSYS_CMAKE_DIR}")
# Does it exist as a submodule or local directory?
ELSEIF(EXISTS "${CMAKE_SOURCE_DIR}/compily_buildd/cmake")
  SET(BUILDSYS_CMAKE_DIR "${CMAKE_SOURCE_DIR}/compily_buildd/cmake")
  MESSAGE(STATUS "Using compily_buildd git submodule ${BUILDSYS_CMAKE_DIR}")
# Ah well.
ELSE()
  MESSAGE(FATAL_ERROR 
    "Cannot find compily_buildd directory for np labs project compilation functions.\n"
    "Make sure you've either put the compily_buildd directory in the same root as your repository directory, or that you've used the compily_buildd submodule (git submodule update --init).\n"
    "NP Labs build repository is available at git://github.com/qdot/compily_buildd.git"
    )
ENDIF(EXISTS "${CMAKE_SOURCE_DIR}/../compily_buildd/cmake")

LIST(APPEND CMAKE_MODULE_PATH ${BUILDSYS_CMAKE_DIR})

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

# Project initialization
INITIALIZE_BUILD()

#Common Options

OPTION_LIBRARY_BUILD_STATIC(ON)
OPTION_LIBRARY_BUILD_SHARED(ON)
OPTION_LIBRARY_BUILD_FRAMEWORK(OFF)
OPTION_BUILD_RPATH(ON)

######################################################################################
# Project specific globals
######################################################################################

#library definitions

SET(LIBOMRON_MAJOR_VERSION 1)
SET(LIBOMRON_MINOR_VERSION 0)
SET(LIBOMRON_BUILD_VERSION 0)

SET(LIBOMRON_VERSION ${LIBOMRON_MAJOR_VERSION}.${LIBOMRON_MINOR_VERSION}.${LIBOMRON_BUILD_VERSION})

CREATE_LIBRARY_LINK_NAME("omron")

# Just set our include and linking directories globally
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)

#library name definitions
SET(LIBOMRON_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include")
SET(LIBOMRON_REQUIRED_LIBS)

IF(WIN32)
  SET(WDK_PATH CACHE PATH "Path to WDK Installation")
  INCLUDE_DIRECTORIES(${WDK_PATH}/inc/api ${WDK_PATH}/inc/crt ${WDK_PATH}/inc)
  LINK_DIRECTORIES(${WDK_PATH}/lib/wxp/i386)
  LIST(APPEND LIBOMRON_REQUIRED_LIBS hid setupapi)
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/win)
ELSEIF(UNIX)
  FIND_PACKAGE(libusb-1.0 REQUIRED)
  IF(LIBUSB_1_FOUND)
    INCLUDE_DIRECTORIES(${LIBUSB_1_INCLUDE_DIRS})
    LIST(APPEND LIBOMRON_REQUIRED_LIBS ${LIBUSB_1_LIBRARIES})
  ENDIF(LIBUSB_1_FOUND)
ENDIF(WIN32)

ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(swig)
# ADD_SUBDIRECTORY(python)
