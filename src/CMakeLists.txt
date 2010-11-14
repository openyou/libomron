######################################################################################
# Build function for main library
######################################################################################

SET(LIBRARY_SRCS 
  omron.c
  )

IF(WIN32)
  LIST(APPEND LIBRARY_SRCS omron_win32.c ${LIBOMRON_INCLUDE_FILES})
ELSEIF(UNIX)
  LIST(APPEND LIBRARY_SRCS omron_libusb.c ${LIBOMRON_INCLUDE_FILES})
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

BUILDSYS_BUILD_LIB(
  NAME omron
  SOURCES "${LIBRARY_SRCS}" 
  CXX_FLAGS FALSE 
  LINK_LIBS "${LIBOMRON_REQUIRED_LIBS}"
  LINK_FLAGS FALSE 
  DEPENDS FALSE
  SHOULD_INSTALL TRUE
  VERSION ${LIBOMRON_VERSION}
  )
