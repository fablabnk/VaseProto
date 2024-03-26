RACK_DIR ?= ../Rack-SDK
DAISYSP_DIR = ../DaisySP

# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS +=
CXXFLAGS += -I$(DAISYSP_DIR)/Source

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS += -L$(DAISYSP_DIR)/ -lDaisySP

# Add .cpp files to the build
SOURCES += ./src/plugin.cpp
SOURCES += ./src/DelayProto.cpp

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

plugin:
	make
	make dist
	make install

clean_extra:
	rm -rf DaisySP

clean: clean_extra

.PHONY: clean