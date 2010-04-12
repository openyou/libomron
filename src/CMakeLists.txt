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

IF(WIN32)
  LIST(APPEND ${LIBS} hid setupapi)
ELSEIF(UNIX)
  LIST(APPEND ${LIBS} nputil_libusb1)
# ELSEIF(UNIX)
#   LIST(APPEND ${LIBS} m)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/../libnputil/include/ ${CMAKE_SOURCE_DIR}/include)
LIST(APPEND LIBRARY_SRCS ${CMAKE_SOURCE_DIR}/../libnputil/src/nputil_libusb1.c)

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
