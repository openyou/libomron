######################################################################################
# Add subdirectories
######################################################################################

# Add our repository cmake_modules directory
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

######################################################################################
# Project Setup
######################################################################################

# ------------------------------------------------------------------------------
# INITIALIZE_BUILD is a beast of a macro that defines a lot of
# defaults about what we expect to be in projects. Instead of
# documenting all of the functionality here, I've added comment blocks
# through the functions since it does so much.
# ------------------------------------------------------------------------------

MACRO(INITIALIZE_BUILD)

  MACRO_ENSURE_OUT_OF_SOURCE_BUILD()

  # Set up default install directories if they aren't set for us
  IF(UNIX)
    IF(NOT CMAKE_INSTALL_PREFIX)
      SET(CMAKE_INSTALL_PREFIX /usr/local)
    ENDIF()
  ENDIF()

  IF(CMAKE_INSTALL_PREFIX)
    MESSAGE(STATUS "Installation Prefix: ${CMAKE_INSTALL_PREFIX}")
  ENDIF()

  # Set up defaults to look like "usr" format. We want all of our
  # projects in this layout.
  IF(NOT INCLUDE_INSTALL_DIR)
    SET(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)
  ENDIF()
  IF(NOT LIBRARY_INSTALL_DIR)
    SET(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)
  ENDIF()
  IF(NOT RUNTIME_INSTALL_DIR)
    SET(RUNTIME_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
  ENDIF()
  IF(NOT SYMBOL_INSTALL_DIR)
    SET(SYMBOL_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/debug)
  ENDIF()
  IF(NOT DOC_INSTALL_DIR)
    SET(DOC_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/doc)
  ENDIF()
  IF(NOT FRAMEWORK_INSTALL_DIR)
    SET(FRAMEWORK_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/frameworks)
  ENDIF()
  IF(NOT CMAKE_MODULES_INSTALL_DIR)
    SET(CMAKE_MODULES_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/cmake_modules)
  ENDIF()

  SET(CMAKE_SKIP_BUILD_RPATH TRUE)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  SET(CMAKE_INSTALL_RPATH "${LIBRARY_INSTALL_DIR}")
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  # We always want to output our binaries and libraries to the same place, set that here
  SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
  SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
  SET(DOC_OUTPUT_PATH ${CMAKE_BINARY_DIR}/doc)

  #Always assume we want to build threadsafe mingw binaries
  IF(MINGW)
    LIST(APPEND BUILDSYS_GLOBAL_DEFINES -mthreads)
    SET(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -mthreads")
  ENDIF()

  #defines we always need on gcc compilers
  IF(CMAKE_COMPILER_IS_GNUCXX)
    LIST(APPEND BUILDSYS_GLOBAL_DEFINES
      -DREENTRANT
      -D_REENTRANT
      -D_THREAD_SAFE
      -D_FILE_OFFSET_BITS=64
      -D_LARGEFILE_SOURCE
      )
  ENDIF()

  IF(NOT MINGW)
    LIST(APPEND BUILDSYS_GLOBAL_DEFINES -D__STDC_LIMIT_MACROS)
  ENDIF()

  FOREACH(DEFINE ${BUILDSYS_GLOBAL_DEFINES})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEFINE}")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DEFINE}")
  ENDFOREACH(DEFINE ${BUILDSYS_GLOBAL_DEFINES})	

  #taken from OpenSceneGraph CMake.
  #Handy visual studio functions
  #Assuming /MP to always be on though
  IF(MSVC)

    # Fun with MSVC2010 linking 
    # 
    # As of VS2010, the "setting PREFIX to ../" hack no longer works
    # to avoid VS's injection of build types into the library output
    # path. Therefore, we have to set everything ourselves here.  I
    # pulled this block from the OutDir test in the cmake source code,
    # because it's not really documented otherwise.
    # 
    # Good times.

    if(CMAKE_CONFIGURATION_TYPES)
      foreach(config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${config}" CONFIG)
        list(APPEND configs "${CONFIG}")
      endforeach()
      set(CMAKE_BUILD_TYPE)
    elseif(NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE Debug)
    endif()

    # Now that we've gathered the configurations we're using, set them
    # all to the paths without the configuration type
    FOREACH(config ${configs})
      SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config} "${LIBRARY_OUTPUT_PATH}")
      SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config} "${LIBRARY_OUTPUT_PATH}")
      SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} "${EXECUTABLE_OUTPUT_PATH}")
    ENDFOREACH()

    # Check to see if we're using nmake. If so, set the NMAKE variable
    IF(CMAKE_MAKE_PROGRAM STREQUAL "nmake")
      SET(NMAKE 1)
    ENDIF()

    # Turn on PDB building
    OPTION(BUILDSYS_GLOBAL_INSTALL_PDB "When building DLLs or EXEs, always build and store a PDB file" OFF)

    # This option is to enable the /MP switch for Visual Studio 2005 and above compilers
    OPTION(WIN32_USE_MP "Set to ON to build with the /MP multiprocessor compile option (Visual Studio 2005 and above)." ON)
    IF(WIN32_USE_MP)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    ENDIF()

    # More MSVC specific compilation flags
    ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)

    #Assume we always want NOMINMAX defined, and lean and mean,
    #and no winsock1. Tends to throw redefinition warnings, but eh.
    ADD_DEFINITIONS(-DNOMINMAX -DWIN32_LEAN_AND_MEAN)
  ENDIF()
  
  IF(APPLE)
    SET(BUILDSYS_FRAMEWORKS)
  ENDIF()

ENDMACRO(INITIALIZE_BUILD)

######################################################################################
# Parse Arguments Macro (for named argument building)
######################################################################################

#taken from http://www.cmake.org/Wiki/CMakeMacroParseArguments

MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})    
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})            
    SET(larg_names ${arg_names})    
    LIST(FIND larg_names "${arg}" is_arg_name)                   
    IF (is_arg_name GREATER -1)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name GREATER -1)
      SET(loption_names ${option_names})    
      LIST(FIND loption_names "${arg}" is_option)            
      IF (is_option GREATER -1)
		SET(${prefix}_${arg} TRUE)
      ELSE (is_option GREATER -1)
		LIST(APPEND current_arg_list ${arg})
      ENDIF (is_option GREATER -1)
    ENDIF (is_arg_name GREATER -1)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)

######################################################################################
# Compile flag array building macro
######################################################################################

#taken from http://www.cmake.org/pipermail/cmake/2006-February/008334.html

MACRO(SET_COMPILE_FLAGS TARGET)
  SET(FLAGS)
  FOREACH(flag ${ARGN})
    SET(FLAGS "${FLAGS} ${flag}")
  ENDFOREACH(flag)
  SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "${FLAGS}")
ENDMACRO(SET_COMPILE_FLAGS)

MACRO(SET_LINK_FLAGS TARGET)
  SET(FLAGS)
  FOREACH(flag ${ARGN})
    SET(FLAGS "${FLAGS} ${flag}")
  ENDFOREACH(flag)
  SET_TARGET_PROPERTIES(${TARGET} PROPERTIES LINK_FLAGS "${FLAGS}")
ENDMACRO(SET_LINK_FLAGS)

######################################################################################
# Generalized library building function for all C++ libraries
######################################################################################

# Function for building libraries
#
# All arguments are prefixed with BUILDSYS_LIB in the function
#
# Arguments
# NAME - name of the library
# SOURCES - list of sources to compile into library
# CXX_FLAGS - flags to pass to the compiler
# LINK_LIBS - libraries to link library to (dynamic libs only)
# LINK_FLAGS - list of flags to use in linking (dynamic libs only)
# DEPENDS - Targets that should be built before this target
# LIB_TYPES_OVERRIDE - override the global types as set by OPTION_BUILD_STATIC/SHARED
# SHOULD_INSTALL - should install commands be generated for this target?
# VERSION - version number for library naming (non-windows dynamic libs only)
# GROUP - Name of the compilation group this should be a part of (so you can build all of a group at once as a target)
# EXCLUDE_FROM_ALL - Don't add as part of all target
#
# When finished, multiple targets are created
# 
# - A target for building the library
# -- [NAME]_[BUILD_TYPE] - i.e. for a static libfoo, there'd be a foo_STATIC target
# - A target for setting dependencies on all versions of the library being built
# -- [NAME]_DEPEND - i.e. foo_DEPEND, which will clear once foo_STATIC/SHARED is built
#

FUNCTION(BUILDSYS_BUILD_LIB)

  # Parse out the arguments
  PARSE_ARGUMENTS(BUILDSYS_LIB
    "NAME;SOURCES;CXX_FLAGS;LINK_LIBS;LINK_FLAGS;DEPENDS;LIB_TYPES_OVERRIDE;SHOULD_INSTALL;VERSION;GROUP;EXCLUDE_FROM_ALL;"
    ""
    ${ARGN}
    )

  # Set up the types of library we want to build (STATIC, DYNAMIC, both)
  IF(BUILDSYS_LIB_LIB_TYPES_OVERRIDE)
	SET(BUILDSYS_LIB_TYPES_LIST ${BUILDSYS_LIB_LIB_TYPES_OVERRIDE})
  ELSE()
	SET(BUILDSYS_LIB_TYPES_LIST ${BUILDSYS_LIB_TYPES})
  ENDIF()

  # Remove all dupes from the source list, otherwise CMake freaks out
  LIST(REMOVE_DUPLICATES BUILDSYS_LIB_SOURCES)

  # Build each library type
  FOREACH(LIB_TYPE ${BUILDSYS_LIB_TYPES_LIST})
    # Setup library name, targets, properties, etc...
    SET(CURRENT_LIB ${BUILDSYS_LIB_NAME}_${LIB_TYPE})

    # To make sure we name our target correctly, but still link against the correct type
    IF(LIB_TYPE STREQUAL "FRAMEWORK")
      SET(TARGET_LIB_TYPE "SHARED")
    ELSE()
      SET(TARGET_LIB_TYPE ${LIB_TYPE})
    ENDIF()

    IF(BUILDSYS_LIB_EXCLUDE_FROM_ALL)
      ADD_LIBRARY (${CURRENT_LIB} EXCLUDE_FROM_ALL ${TARGET_LIB_TYPE} ${BUILDSYS_LIB_SOURCES})
    ELSE()
      ADD_LIBRARY (${CURRENT_LIB} ${TARGET_LIB_TYPE} ${BUILDSYS_LIB_SOURCES})      
    ENDIF()

    # Add this library to the list of all libraries we're building
    LIST(APPEND LIB_DEPEND_LIST ${CURRENT_LIB})

    # This allows use to build static/shared libraries of the same name.
    # See http://www.itk.org/Wiki/CMake_FAQ#How_do_I_make_my_shared_and_static_libraries_have_the_same_root_name.2C_but_different_suffixes.3F
	IF(USE_STATIC_SUFFIX AND LIB_TYPE STREQUAL "STATIC")
      SET_TARGET_PROPERTIES (${CURRENT_LIB} PROPERTIES OUTPUT_NAME ${BUILDSYS_LIB_NAME}_s)
	ELSE()
      SET_TARGET_PROPERTIES (${CURRENT_LIB} PROPERTIES OUTPUT_NAME ${BUILDSYS_LIB_NAME})	  
	ENDIF()
    SET_TARGET_PROPERTIES (${CURRENT_LIB} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

    # Add version, if we're given one
    IF(BUILDSYS_LIB_VERSION)
      SET_TARGET_PROPERTIES (${CURRENT_LIB} PROPERTIES SOVERSION ${BUILDSYS_LIB_VERSION})
      SET_TARGET_PROPERTIES (${CURRENT_LIB} PROPERTIES VERSION ${BUILDSYS_LIB_VERSION})
    ENDIF()

    IF(LIB_TYPE STREQUAL "FRAMEWORK")
      SET_TARGET_PROPERTIES (${CURRENT_LIB} PROPERTIES FRAMEWORK 1)
      # As far as I can find, even in CMake 2.8.2, there's no way to
      # explictly copy header directories. This makes me sad.
      GET_TARGET_PROPERTY(OUT_LIB ${CURRENT_LIB} LOCATION)
      GET_FILENAME_COMPONENT(OUT_DIR ${OUT_LIB} PATH)
      MESSAGE(STATUS ${OUT_DIR})
      ADD_CUSTOM_TARGET(${CURRENT_LIB}_FRAMEWORK_HEADER_COPY
        COMMAND "${CMAKE_COMMAND}" "-E" "make_directory" "${OUT_DIR}/Headers"
        COMMAND "${CMAKE_COMMAND}" "-E" "copy_directory" "${CMAKE_SOURCE_DIR}/include" "${OUT_DIR}/Headers"
        COMMAND "${CMAKE_COMMAND}" "-E" "create_symlink" "Versions/Current/Headers" "${OUT_DIR}/../../Headers")
      ADD_DEPENDENCIES(${CURRENT_LIB} ${CURRENT_LIB}_FRAMEWORK_HEADER_COPY)
    ENDIF()

    # Libraries we should link again
    IF(BUILDSYS_LIB_LINK_LIBS)
	  TARGET_LINK_LIBRARIES(${CURRENT_LIB} ${BUILDSYS_LIB_LINK_LIBS})
    ENDIF()

    # Defines and compiler flags, if any
    IF(BUILDSYS_LIB_CXX_FLAGS)
      SET_COMPILE_FLAGS(${CURRENT_LIB} ${BUILDSYS_LIB_CXX_FLAGS})
    ENDIF()

    # Linker flags, if any
    IF(BUILDSYS_LIB_LINK_FLAGS)
      SET_LINK_FLAGS(${CURRENT_LIB} ${BUILDSYS_LIB_LINK_FLAGS})
    ENDIF()

    # Installation commands
    IF(BUILDSYS_LIB_SHOULD_INSTALL AND NOT BUILDSYS_LIB_EXCLUDE_FROM_ALL)
      INSTALL(TARGETS ${CURRENT_LIB} LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR} ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR} FRAMEWORK DESTINATION ${FRAMEWORK_INSTALL_DIR})
    ELSEIF(BUILDSYS_LIB_SHOULD_INSTALL AND BUILDSYS_LIB_EXCLUDE_FROM_ALL)
      # Only install the output file if it exists. This doesn't work for targets under exclude from all, but we may build them anyways
      MESSAGE(STATUS "NOTE: Target ${BUILDSYS_LIB_NAME} will only be installed after target is specifically built (not build using all target)")
      GET_TARGET_PROPERTY(LIB_OUTPUT_NAME ${CURRENT_LIB} LOCATION)
      INSTALL(FILES ${LIB_OUTPUT_NAME} LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR} ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR} OPTIONAL)
    ENDIF()

    # Rewrite of install_name_dir in apple binaries
    IF(APPLE)
      SET_TARGET_PROPERTIES(${CURRENT_LIB} PROPERTIES INSTALL_NAME_DIR ${LIBRARY_INSTALL_DIR})
    ENDIF()


    # If the library depends on anything, set up dependency
    IF(BUILDSYS_LIB_DEPENDS)
      ADD_DEPENDENCIES(${CURRENT_LIB} ${BUILDSYS_LIB_DEPENDS})
    ENDIF()

  ENDFOREACH()

  # Build the dependency name for ourselves and set up the target for it
  SET(DEPEND_NAME "${BUILDSYS_LIB_NAME}_DEPEND")
  ADD_CUSTOM_TARGET(${DEPEND_NAME} DEPENDS ${LIB_DEPEND_LIST})

  IF(BUILDSYS_LIB_GROUP)
    IF(NOT TARGET ${BUILDSYS_LIB_GROUP})
      MESSAGE(STATUS "Creating build group ${BUILDSYS_LIB_GROUP}")
      ADD_CUSTOM_TARGET(${BUILDSYS_LIB_GROUP} DEPENDS ${DEPEND_NAME})
    ELSE()
      ADD_DEPENDENCIES(${BUILDSYS_LIB_GROUP} ${DEPEND_NAME})
    ENDIF()
  ENDIF()

ENDFUNCTION()

######################################################################################
# Generalized executable building function
######################################################################################

# Function for building executables
#
# All arguments are prefixed with BUILDSYS_EXE in the function
#
# Arguments
# NAME - name of the executable
# SOURCES - list of sources to compile into executable
# CXX_FLAGS - flags to pass to the compiler
# LINK_LIBS - libraries to link executable to
# LINK_FLAGS - list of flags to use in linking
# DEPENDS - Targets that should be built before this target
# SHOULD_INSTALL - should install commands be generated for this target?
# GROUP - Name of the compilation group this should be a part of (so you can build all of a group at once as a target)
# EXCLUDE_FROM_ALL - Don't add as part of all target
# INSTALL_PDB - On windows, if this is true, always create and install a PDB. Will also always happen with BUILDSYS_GLOBAL_INSTALL_PDB is on.
#
# When finished, one target is created, which is the NAME argument
# 

FUNCTION(BUILDSYS_BUILD_EXE)
  PARSE_ARGUMENTS(BUILDSYS_EXE
    "NAME;SOURCES;CXX_FLAGS;LINK_LIBS;LINK_FLAGS;DEPENDS;SHOULD_INSTALL;GROUP;EXCLUDE_FROM_ALL;INSTALL_PDB;"
    ""
    ${ARGN}
    )

  # Remove all dupes from the source list, otherwise CMake freaks out
  LIST(REMOVE_DUPLICATES BUILDSYS_EXE_SOURCES)

  # Create the target
  IF(BUILDSYS_EXE_EXCLUDE_FROM_ALL)
    ADD_EXECUTABLE(${BUILDSYS_EXE_NAME} EXCLUDE_FROM_ALL ${BUILDSYS_EXE_SOURCES})
  ELSE()
    ADD_EXECUTABLE(${BUILDSYS_EXE_NAME} ${BUILDSYS_EXE_SOURCES})
  ENDIF()
  SET_TARGET_PROPERTIES (${BUILDSYS_EXE_NAME} PROPERTIES OUTPUT_NAME ${BUILDSYS_EXE_NAME})

  # Defines and compiler flags, if any
  IF(BUILDSYS_EXE_CXX_FLAGS)
    SET_COMPILE_FLAGS(${BUILDSYS_EXE_NAME} ${BUILDSYS_EXE_CXX_FLAGS})
  ENDIF()

  # Set up rpaths to look in a few different places for libraries
  # - . (cwd)
  # - @loader_path/. (NOTE: @loader_path with no following path seems to fail)
  # - All of the library paths we linked again

  IF(NOT BUILDSYS_EXE_LINK_FLAGS)
    SET(BUILDSYS_EXE_LINK_FLAGS)
  ENDIF()

  IF(APPLE)   
    # The three normal paths
    # Right next to us, in the path of the requesting binary, and @loader_path/../Frameworks (the bundle packing Frameworks version)
    LIST(APPEND BUILDSYS_EXE_LINK_FLAGS "-Wl,-rpath,@loader_path/." "-Wl,-rpath,@loader_path/../Frameworks" "-Wl,-rpath,.")
    IF(BUILDSYS_DEP_PATHS)
      FOREACH(PATH ${BUILDSYS_DEP_PATHS})
        LIST(APPEND BUILDSYS_EXE_LINK_FLAGS "-Wl,-rpath,${PATH}/lib")
      ENDFOREACH()
    ENDIF()
  ENDIF()

  # If we're using Visual Studio, see whether or not we should
  # generate and install PDB files, even if we're in release
  IF(MSVC)
    IF(BUILDSYS_GLOBAL_INSTALL_PDB OR BUILDSYS_EXE_INSTALL_PDB)
      LIST(APPEND BUILDSYS_EXE_LINK_FLAGS "/DEBUG")
      GET_TARGET_PROPERTY(EXE_OUTPUT_NAME ${BUILDSYS_EXE_NAME} LOCATION)
      # Strip the .exe off the end and replace with .pdb
      STRING(REGEX REPLACE ".exe$" ".pdb" PDB_OUTPUT_NAME ${EXE_OUTPUT_NAME})
      STRING(REGEX REPLACE "\\$\\(OutDir\\)" "" PDB_OUTPUT_NAME ${PDB_OUTPUT_NAME})
      INSTALL(FILES ${PDB_OUTPUT_NAME} DESTINATION ${SYMBOL_INSTALL_DIR} OPTIONAL)
    ENDIF()
  ENDIF()
  
  # Linker flags, if any
  IF(BUILDSYS_EXE_LINK_FLAGS)
    SET_LINK_FLAGS(${BUILDSYS_EXE_NAME} ${BUILDSYS_EXE_LINK_FLAGS})
  ENDIF()
  
  # Libraries to link to 
  IF(BUILDSYS_EXE_LINK_LIBS)
    TARGET_LINK_LIBRARIES(${BUILDSYS_EXE_NAME} ${BUILDSYS_EXE_LINK_LIBS})
  ENDIF()

  # Install commands
  IF(BUILDSYS_EXE_SHOULD_INSTALL AND NOT BUILDSYS_EXE_EXCLUDE_FROM_ALL)
    INSTALL(TARGETS ${BUILDSYS_EXE_NAME} RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR})
  ELSEIF(BUILDSYS_EXE_SHOULD_INSTALL AND BUILDSYS_EXE_EXCLUDE_FROM_ALL)
    # Only install the output file if it exists. This doesn't work for targets under exclude from all, but we may build them anyways
    MESSAGE(STATUS "NOTE: Target ${BUILDSYS_EXE_NAME} will only be installed after target is specifically built (not build using all target)")
    GET_TARGET_PROPERTY(EXE_OUTPUT_NAME ${BUILDSYS_EXE_NAME} LOCATION)
    INSTALL(FILES ${EXE_OUTPUT_NAME} RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR} OPTIONAL)
  ENDIF()


  # If the executable depends on anything, set up dependency
  IF(BUILDSYS_EXE_DEPENDS)
    ADD_DEPENDENCIES(${BUILDSYS_EXE_NAME} ${BUILDSYS_EXE_DEPENDS})
  ENDIF()

  IF(BUILDSYS_EXE_GROUP)
    IF(NOT TARGET ${BUILDSYS_EXE_GROUP})
      MESSAGE(STATUS "Creating build group ${BUILDSYS_EXE_GROUP}")
      ADD_CUSTOM_TARGET(${BUILDSYS_EXE_GROUP} DEPENDS ${BUILDSYS_EXE_NAME})
    ELSE()
      ADD_DEPENDENCIES(${BUILDSYS_EXE_GROUP} ${BUILDSYS_EXE_NAME})
    ENDIF()
  ENDIF()

ENDFUNCTION(BUILDSYS_BUILD_EXE)

######################################################################################
# Make sure we aren't trying to do an in-source build
######################################################################################

#taken from http://www.mail-archive.com/cmake@cmake.org/msg14236.html

MACRO(MACRO_ENSURE_OUT_OF_SOURCE_BUILD)
  STRING(COMPARE EQUAL "${${PROJECT_NAME}_SOURCE_DIR}" "${${PROJECT_NAME}_BINARY_DIR}" insource)
  GET_FILENAME_COMPONENT(PARENTDIR ${${PROJECT_NAME}_SOURCE_DIR} PATH)
  STRING(COMPARE EQUAL "${${PROJECT_NAME}_SOURCE_DIR}" "${PARENTDIR}" insourcesubdir)
  IF(insource OR insourcesubdir)
    MESSAGE(FATAL_ERROR 
      "${PROJECT_NAME} requires an out of source build (make a build dir and call cmake from that.)\n"
      "A script (Makefile or python) should've been included in your build to generate this, check your project root directory.\n"
      "If you get this error from a sub-directory, make sure there is not a CMakeCache.txt in your project root directory."
      )
  ENDIF()
ENDMACRO()

######################################################################################
# Create a library name that fits our platform
######################################################################################

MACRO(CREATE_LIBRARY_LINK_NAME LIBNAME)
  if(BUILD_STATIC AND NOT BUILD_SHARED)
    IF(NOT MSVC)
      SET(LIB_STATIC_PRE "lib")
      SET(LIB_STATIC_EXT ".a")
    ELSE(NOT MSVC)
      SET(LIB_STATIC_PRE "")
      SET(LIB_STATIC_EXT ".lib")
    ENDIF(NOT MSVC)
    SET(LIB_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/)
  ELSE(BUILD_STATIC AND NOT BUILD_SHARED)
    SET(LIB_STATIC_PRE)
    SET(LIB_STATIC_EXT)
    SET(LIB_OUTPUT_PATH)
  ENDIF(BUILD_STATIC AND NOT BUILD_SHARED)
  SET(lib${LIBNAME}_LIBRARY ${LIB_OUTPUT_PATH}${LIB_STATIC_PRE}${LIBNAME}${LIB_STATIC_EXT})  
ENDMACRO(CREATE_LIBRARY_LINK_NAME)

######################################################################################
# Library Build Type Options
######################################################################################

MACRO(OPTION_LIBRARY_BUILD_STATIC DEFAULT)
  OPTION(BUILD_STATIC "Build static libraries" ${DEFAULT})

  IF(BUILD_STATIC)
	LIST(APPEND BUILDSYS_LIB_TYPES STATIC)
	MESSAGE(STATUS "Building Static Libraries for ${CMAKE_PROJECT_NAME}")
  ELSE()
  	MESSAGE(STATUS "NOT Building Static Libraries for ${CMAKE_PROJECT_NAME}")
  ENDIF()
ENDMACRO()

MACRO(OPTION_USE_STATIC_SUFFIX DEFAULT)
  OPTION(USE_STATIC_SUFFIX "If building static libraries, suffix their name with _s. Handy on windows when building both." ${DEFAULT})

  IF(USE_STATIC_SUFFIX)
	MESSAGE(STATUS "Building Static Libraries with suffix '_s'")
  ELSE()
  	MESSAGE(STATUS "Building Static Libraries with same name as shared (may cause issues on windows)")
  ENDIF()
ENDMACRO()

MACRO(OPTION_LIBRARY_BUILD_SHARED DEFAULT)
  OPTION(BUILD_SHARED "Build shared libraries" ${DEFAULT})

  IF(BUILD_SHARED)
	LIST(APPEND BUILDSYS_LIB_TYPES SHARED)
	MESSAGE(STATUS "Building Shared Libraries for ${CMAKE_PROJECT_NAME}")
  ELSE()
  	MESSAGE(STATUS "NOT Building Shared Libraries for ${CMAKE_PROJECT_NAME}")
  ENDIF()
ENDMACRO()

MACRO(OPTION_LIBRARY_BUILD_FRAMEWORK DEFAULT)
  IF(APPLE)
    OPTION(BUILD_FRAMEWORK "Build OS X Frameworks" ${DEFAULT})

    IF(BUILD_FRAMEWORK)
	  LIST(APPEND BUILDSYS_LIB_TYPES FRAMEWORK)
	  MESSAGE(STATUS "Building Shared Libraries for ${CMAKE_PROJECT_NAME}")
    ELSE()
  	  MESSAGE(STATUS "NOT Building Shared Libraries for ${CMAKE_PROJECT_NAME}")
    ENDIF()
  ENDIF()
ENDMACRO()

######################################################################################
# RPATH Relink Options
######################################################################################

MACRO(OPTION_BUILD_RPATH DEFAULT)
  OPTION(SET_BUILD_RPATH "Set the build RPATH to local directories, relink to install directories at install time" ${DEFAULT})

  IF(SET_BUILD_RPATH)
  	MESSAGE(STATUS "Setting build RPATH for ${CMAKE_PROJECT_NAME}")
	# use, i.e. don't skip the full RPATH for the build tree
	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    
	# when building, don't use the install RPATH already
	# (but later on when installing)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
    
	# the RPATH to be used when installing
	SET(CMAKE_INSTALL_RPATH "${LIBRARY_INSTALL_DIR}")
    
	# add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  ELSE()
    MESSAGE(STATUS "NOT Setting build RPATH for ${CMAKE_PROJECT_NAME}")
  ENDIF()
ENDMACRO()

######################################################################################
# Create software version code file
######################################################################################

MACRO(OPTION_CREATE_VERSION_FILE DEFAULT OUTPUT_FILES)
  OPTION(CREATE_VERSION_FILE "Creates a version.cc file using the setlocalversion script" ${DEFAULT})
  IF(CREATE_VERSION_FILE)
	MESSAGE(STATUS "Generating git information for ${CMAKE_PROJECT_NAME}")	
	FOREACH(VERSION_FILE ${OUTPUT_FILES})
	  MESSAGE(STATUS "- Generating to ${VERSION_FILE}")	
      SET(COMMAND_LIST "python" "${BUILDSYS_CMAKE_DIR}/../python/get_version.py" "-f" "${VERSION_FILE}" "-d" "${CMAKE_SOURCE_DIR}")
 	  EXECUTE_PROCESS(COMMAND ${COMMAND_LIST})
	ENDFOREACH(VERSION_FILE ${OUTPUT_FILES})
  ELSE()
	MESSAGE(STATUS "NOT generating git information for ${CMAKE_PROJECT_NAME}")	
  ENDIF()
ENDMACRO()

######################################################################################
# Turn on GProf based profiling 
######################################################################################

MACRO(OPTION_GPROF DEFAULT)
  IF(CMAKE_COMPILER_IS_GNUCXX)
	OPTION(ENABLE_GPROF "Compile using -g -pg for gprof output" ${DEFAULT})
	IF(ENABLE_GPROF)
	  MESSAGE(STATUS "Using gprof output for ${CMAKE_PROJECT_NAME}")
	  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -pg")
	  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg")
	  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -pg")
	  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -pg")
	ELSE()
	  MESSAGE(STATUS "NOT using gprof output for ${CMAKE_PROJECT_NAME}")
	ENDIF()
  ELSE()
	MESSAGE(STATUS "gprof generation NOT AVAILABLE - Not a GNU compiler")
  ENDIF()
ENDMACRO()

######################################################################################
# Turn on "extra" compiler warnings (SPAMMY WITH BOOST)
######################################################################################

MACRO(OPTION_EXTRA_COMPILER_WARNINGS DEFAULT)
  IF(CMAKE_COMPILER_IS_GNUCXX)
	OPTION(EXTRA_COMPILER_WARNINGS "Turn on -Wextra for gcc" ${DEFAULT})
	IF(EXTRA_COMPILER_WARNINGS)
	  MESSAGE(STATUS "Turning on extra c/c++ warnings for ${CMAKE_PROJECT_NAME}")
	  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
	  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
	ELSE()
	  MESSAGE(STATUS "NOT turning on extra c/c++ warnings for ${CMAKE_PROJECT_NAME}")
	ENDIF()
  ELSE()
	MESSAGE(STATUS "Extra compiler warnings NOT AVAILABLE - Not a GNU compiler")
  ENDIF()
ENDMACRO()

######################################################################################
# Turn on effective C++ compiler warnings
######################################################################################

MACRO(OPTION_EFFCXX_COMPILER_WARNINGS DEFAULT)
  IF(CMAKE_COMPILER_IS_GNUCXX)
	OPTION(EFFCXX_COMPILER_WARNINGS "Turn on -Weffc++ (effective c++ warnings) for gcc" ${DEFAULT})
	IF(EFFCXX_COMPILER_WARNINGS)
	  MESSAGE(STATUS "Turning on Effective c++ warnings for ${CMAKE_PROJECT_NAME}")
	  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
	ELSE()
	  MESSAGE(STATUS "NOT turning on Effective c++ warnings for ${CMAKE_PROJECT_NAME}")
	ENDIF()
  ELSE()
	MESSAGE(STATUS "Effective C++ compiler warnings NOT AVAILABLE - Not a GNU compiler")
  ENDIF()
ENDMACRO()

######################################################################################
# Return type compiler warnings
######################################################################################

MACRO(OPTION_RETURN_TYPE_COMPILER_WARNINGS DEFAULT)
  IF(CMAKE_COMPILER_IS_GNUCXX)
	OPTION(RETURN_TYPE_COMPILER_WARNINGS "Turn on -Wreturn-type for gcc" ${DEFAULT})
	IF(RETURN_TYPE_COMPILER_WARNINGS)
	  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wreturn-type")
	  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreturn-type")
	  MESSAGE(STATUS "Turning on return type warnings for ${CMAKE_PROJECT_NAME}")
	ELSE()
	  MESSAGE(STATUS "NOT turning on return type warnings for ${CMAKE_PROJECT_NAME}")
	ENDIF()
  ELSE()
	MESSAGE(STATUS "Return type warnings NOT AVAILABLE - Not a GNU compiler")
  ENDIF()
ENDMACRO()

######################################################################################
# Force 32-bit, regardless of the platform we're on
######################################################################################

MACRO(OPTION_FORCE_32_BIT DEFAULT)
  IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	IF(CMAKE_COMPILER_IS_GNUCXX)
	  OPTION(FORCE_32_BIT "Force compiler to use -m32 when compiling" ${DEFAULT})
	  IF(FORCE_32_BIT)
		MESSAGE(STATUS "Forcing 32-bit on 64-bit platform (using -m32)")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
		SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -m32")
		SET(CMAKE_LINK_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	  ELSE()
		MESSAGE(STATUS "Not forcing 32-bit on 64-bit platform")
	  ENDIF()
	ELSE()
	  MESSAGE(STATUS "Force 32 bit NOT AVAILABLE - Not using gnu compiler")
	ENDIF()
  ELSE({CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	MESSAGE(STATUS "Force 32 bit NOT AVAILABLE - Already on a 32 bit platform")
  ENDIF()
ENDMACRO()

######################################################################################
# Uninstall function
# From http://www.cmake.org/Wiki/CMake_FAQ#Can_I_do_.22make_uninstall.22_with_CMake.3F
######################################################################################

# IF(NOT EXISTS "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt")
#   MESSAGE(FATAL_ERROR "Cannot find install manifest: \"@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt\"")
# ENDIF(NOT EXISTS "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt")

# FILE(READ "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt" files)
# STRING(REGEX REPLACE "\n" ";" files "${files}")
# FOREACH(file ${files})
#   MESSAGE(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
#   IF(EXISTS "$ENV{DESTDIR}${file}")
#     EXEC_PROGRAM(
# 	  "@CMAKE_COMMAND@" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
# 	  OUTPUT_VARIABLE rm_out
# 	  RETURN_VALUE rm_retval
# 	  )
#     IF(NOT "${rm_retval}" STREQUAL 0)
# 	  MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
#     ENDIF(NOT "${rm_retval}" STREQUAL 0)
#   ELSE(EXISTS "$ENV{DESTDIR}${file}")
#     MESSAGE(STATUS "File \"$ENV{DESTDIR}${file}\" does not exist.")
#   ENDIF(EXISTS "$ENV{DESTDIR}${file}")
# ENDFOREACH(file)
