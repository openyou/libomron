######################################################################################
# Build function for omron_790IT_test
######################################################################################

SET(LIBOMRON_EXAMPLE_LIBS ${libomron_LIBRARY} ${LIBOMRON_REQUIRED_LIBS})

#linux needs math lib
IF(UNIX AND NOT APPLE)
  LIST(APPEND LIBOMRON_EXAMPLE_LIBS m)
ENDIF()

SET(EXAMPLES omron_720IT_test omron_790IT_test)

FOREACH(EX ${EXAMPLES})
  SET(SRCS ${EX}/${EX}.c)
  
  BUILDSYS_BUILD_EXE(
    NAME ${EX}
    SOURCES "${SRCS}" 
    CXX_FLAGS FALSE
    LINK_LIBS "${LIBOMRON_EXAMPLE_LIBS}"
    LINK_FLAGS FALSE 
    DEPENDS omron_DEPEND
    SHOULD_INSTALL TRUE
    )
ENDFOREACH()

SET(SRCS omron_720IT_test/omron_720IT_csv_dump.c)
BUILDSYS_BUILD_EXE(
  NAME omron_720IT_csv_dump
  SOURCES "${SRCS}" 
  CXX_FLAGS FALSE
  LINK_LIBS "${LIBOMRON_EXAMPLE_LIBS}"
  LINK_FLAGS FALSE 
  DEPENDS omron_DEPEND
  SHOULD_INSTALL TRUE
  )
