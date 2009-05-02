######################################################################################
# Build function for omron_790IT_test
######################################################################################

SET(SRCS omron_790IT_test.c)

SET(LIBS omron hid setupapi)

INCLUDE_DIRECTORIES(${LIBOMRON_INCLUDE_DIRS})
LINK_DIRECTORIES(../../lib)

NP_BUILD_EXE(
  NAME omron_790IT_test
  SOURCES "${SRCS}" 
  CXX_FLAGS FALSE
  LINK_LIBS "${LIBS}"
  LINK_FLAGS FALSE 
  DEPENDS ${LIBOMRON_C}_DEPEND
  SHOULD_INSTALL TRUE
  )
