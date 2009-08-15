######################################################################################
# Build function for omron_790IT_test
######################################################################################

SET(SRCS omron_720IT_test.c)

SET(LIBS /Users/qdot/git-projects/libomron/build_windows_mingw4_x86_release_static//lib/libomron.a hid setupapi)

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
