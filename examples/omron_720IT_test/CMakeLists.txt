######################################################################################
# Build function for omron_790IT_test
######################################################################################

SET(SRCS omron_720IT_test.c)

SET(LIBS ${LIBOMRON_LIBRARY})

IF(WIN32)
  LIST(APPEND LIBS hid setupapi)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${LIBOMRON_INCLUDE_DIRS})
LINK_DIRECTORIES(../../lib)

BUILDSYS_BUILD_EXE(
  NAME omron_720IT_test
  SOURCES "${SRCS}" 
  CXX_FLAGS FALSE
  LINK_LIBS "${LIBS}"
  LINK_FLAGS FALSE 
  DEPENDS ${LIBOMRON_C}_DEPEND
  SHOULD_INSTALL TRUE
  )
