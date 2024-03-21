# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..

DAISYSP_DIR = ../DaisySP

# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS +=
CXXFLAGS += -I$(DAISYSP_DIR)/Source -I$(DAISYSP_DIR)/Source/Utility

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS += -L$(DAISYSP_DIR)/build # -ldaisysp

# Add .cpp files to the build
SOURCES += $(wildcard src/*.cpp)
SOURCES += ${wildcard $(DAISYSP_DIR)/Source/**/*.cpp}

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk
