Note: This branch is an intro to creating a simple plugin using DaisySP code. Check the other branches for more advanced examples (delay lines etc)

This is a fork of [VaseProto](https://github.com/stephenhensley/VaseProto), which has been updated to work with the latest version of VCV Rack.

- Its purpose is to build a simple sine wave oscillator plugin for VCV Rack using DaisySP code
- This template can then be used as a development environment to prototype more complex modules without the Daisy hardware
- We are using this approach for our SynthLab sessions in the [42 Berlin FabLab](https://workish.berlin/fablab) to prototype our first hardware Eurorack module

# Prerequisites

## 1. Install VCV Rack and the Rack SDK

For our plugin to compile, we need VCV Rack, the Rack SDK and Daisy SP to be installed in a project folder. In my case this is `~/Documents/SynthLab` and it looks as follows:

```
.
├── DaisySP : digital signal processing code for the Daisy platform
├── Rack2Free : VCV Rack itself
└── Rack-SDK : the Rack Software Development Kit
```

For the second two items, we can follow the VCV Rack [plugin development tutorial](https://vcvrack.com/manual/PluginDevelopmentTutorial), which is recommended to get a feeling for how plugins work and are built.

## 2. Download and Build DaisySP

To install DaisySP we can simply clone [the repo](https://github.com/electro-smith/DaisySP) into our project folder and build it. When using the Daisy hardware directly we would install the `arm-none-eabi` compiler and compile for the arm architecture by simply typing `make`. However when making a plugin we want to build for our local architecture. For this we first use cmake, as follows:

```
cmake .
make
```

If this completes, you should find the `libDaisySP.a` static library in the root DaisySP folder, which we will link to when compiling our VCV Rack plugin.

# Building the Plugin

Now we are ready to ahead and build the plugin. Back up in your top level folder, clone this repo as follows:

`git clone https://github.com/fablabnk/VaseProtoPlugin.git`

Your folder structure should now look like this:

```
.
├── Rack2Free
├── Rack-SDK
├── DaisySP
└── VaseProtoPlugin
```

Go into the VaseProtoPlugin folder, edit the Makefile and provoide the correct paths to our previously downloaded tools, which should be:

```
RACK_DIR ?= ../Rack-SDK
DAISYSP_DIR = ../DaisySP
```

Also note that the LDFLAGS line is linking to our previously created `libDaisySP.a` file, where `-L` points it at the right folder, whilst `-l` tells us the name of the library to look for (`DaisySP` will repsolve to `libDaisySP.a`).

```
LDFLAGS += -L$(DAISYSP_DIR)/ -lDaisySP
```

Now you can go ahead and build like so:

```
make
make dist
make install
```

Here, `make` builds the plugin, `make dist` makes a distributable version and `make install` places it in your VCV Rack plugins folder in ~/.Rack2. When running `make`, some warnings are normal.

Once this is done you should be able to open VCVRack, right click and see and use your plugin. If your plugin does not appear, check `~/.Rack2/logs.txt` for errors.

# Things to note

- When you build your plugin, an extra DaisySP folder will be created with object and dependency files in it - this is normal. The plugin Makefile has the target `clean_extra` to clean it up whilst still calling the original `clean` target
- If you have scratchy audio in VCV Rack using Ubuntu, use PulseAudio instead of ALSA

# Technical details: What I updated to make it work

- updated plugin.json to "version": "2.4.1" (probably not necessary but nice)

in Makefile:
- Point correctly to source code in DaisySP folder (one folder up):
```
DAISYSP_DIR = ../DaisySP
```
- point the compiler to the header files `daisysp.h` and `dsp.h`:
```
CXXFLAGS += -I$(DAISYSP_DIR)/Source -I$(DAISYSP_DIR)/Source/Utility
```
- commented out the -ldaisysp flag here (I could never find what it's trying to link to here)
```
LDFLAGS += -L$(DAISYSP_DIR)/build # -ldaisysp
```
- added this line to include all the DaisySP source files (probably a bit overkill)
```
SOURCES += ${wildcard $(DAISYSP_DIR)/Source/**/*.cpp}
```

# Useful links

This link helped me to realise that I don't have to build DaisySP for arm:
https://forum.electro-smith.com/t/daisysp-away-from-the-hardware-aka-plugins-with-juce/1106
