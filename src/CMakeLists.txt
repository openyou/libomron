######################################################################################
# Build function for main library
######################################################################################

SET(LIBRARY_SRCS 
  ${LIBNIFALCON_INCLUDE_FILES}
  omron.c
  omron_win32.c
  )

INCLUDE_DIRECTORIES(${LIBOMRON_INCLUDE_DIRS})

NP_BUILD_LIB(
  NAME ${LIBOMRON_C} 
  SOURCES "${LIBRARY_SRCS}" 
  CXX_FLAGS FALSE 
  LINK_LIBS FALSE
  LINK_FLAGS FALSE 
  DEPENDS FALSE
  SHOULD_INSTALL TRUE
  VERSION ${LIBOMRON_VERSION}
  )
