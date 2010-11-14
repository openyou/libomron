######################################################################################
# Build function for omron_790IT_test
######################################################################################

SET(LIBOMRON_EXAMPLE_LIBS ${libomron_LIBRARY} ${LIBOMRON_REQUIRED_LIBS} -lm)

SET(EXAMPLES omron_720IT_test omron_790IT_test)

FOREACH(EX ${EXAMPLES})
  SET(SRCS ${EX}/${EX}.c)
  
  BUILDSYS_BUILD_EXE(
    NAME ${EX}
    SOURCES "${SRCS}" 
    CXX_FLAGS FALSE
    LINK_LIBS "${LIBOMRON_EXAMPLE_LIBS}"
    LINK_FLAGS FALSE 
    DEPENDS ${omron_DEPEND}
    SHOULD_INSTALL TRUE
    )
ENDFOREACH()
