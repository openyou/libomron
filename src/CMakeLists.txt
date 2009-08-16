######################################################################################
# Build function for main library
######################################################################################

SET(LIBRARY_SRCS 
  omron.c
  )

IF(WIN32)
  LIST(APPEND LIBRARY_SRCS omron_win32.c)
ELSEIF(LINUX)
  LIST(APPEND LIBRARY_SRCS omron_usbfs.c)
ENDIF(WIN32)

SET(LIBS)

IF(WIN32)
  LIST(APPEND ${LIBS} hid setupapi)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${LIBOMRON_INCLUDE_DIRS})

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
