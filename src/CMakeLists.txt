######################################################################################
# Build function for main library
######################################################################################

SET(LIBRARY_SRCS 
  omron.c
  )

IF(WIN32)
  LIST(APPEND LIBRARY_SRCS omron_win32.c)
ELSEIF(UNIX)
  LIST(APPEND LIBRARY_SRCS omron_libusb.c)
# ELSEIF(UNIX)
#   LIST(APPEND LIBRARY_SRCS omron_usbfs.c)
ENDIF(WIN32)

SET(LIBS)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

BUILDSYS_BUILD_LIB(
  NAME omron
  SOURCES "${LIBRARY_SRCS}" 
  CXX_FLAGS FALSE 
  LINK_LIBS "${LIBS}"
  LINK_FLAGS FALSE 
  DEPENDS FALSE
  SHOULD_INSTALL TRUE
  VERSION ${LIBOMRON_VERSION}
  )
