MAKEFILE_LOCATION=../../np_build_sys/util/MakefileDriver.mk

all:
ifneq "$(wildcard $(MAKEFILE_LOCATION))" ""
include $(MAKEFILE_LOCATION)
else
	@echo "Can't find Makefile Driver! Please make sure you have the np_build_sys repository checked out in the directory below this"
	@echo "np_build_sys can be checked out at git://github.com/qdot/np_build_sys"
endif

