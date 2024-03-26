This is a simple delay line build as a plugin for VCVRack using the DaisySP library.

- It is a first step towards prototyping a Daisy seed-based audio processing Eurorack module.
- The code is based on the Daisy example [here](https://github.com/electro-smith/DaisyExamples/blob/master/seed/DSP/delayline/delayline.cpp), but with the oscillator and envelope stripped out to create a simple fixed-length delay.
- The next step will be to add modulatable parameters.

I added a convenience target to the Makefile `make plugin` which does `make`, `make dist` and `make install` together.

# Things I learned building this module

## Plugin Element Categories

I learned more about the general structure of the plugin code in `DelayProto.cpp`, including how to declare the four categories of elements that make up a plugin:

- parameters
- input
- output
- lights

1. Module declaration

In `struct DelayProto : Module`, all four element categories must be provided, but we can leave some of them blank, by just ending them with NUM_xxx e.g.

```
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		AUDIO_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
```

2. Constructor

In `DelayProto()`, we must always reference the four elements:
`config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);`

3. User Interface

The `struct DelayProtoWidget : ModuleWidget` represents the user interface. Here we just add the elements we actually use...

```
addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 77.478)), module, DelayProto::AUDIO_INPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.713)), module, DelayProto::AUDIO_OUTPUT));
```

## Namespaces

We can reference our DaisySP code with or without a namespace.

1. Without a namespace (I chose this approach for now)

```
daisysp::DelayLine<float, MAX_DELAY> del;
```

2. With a namespace. For example by including the following line before our module declaration ```struct DelayProto : Module```
```
using namespace daisysp;
```

## No 'size' loop needed

Unlike in line 29 of the the [Daisy example](https://github.com/electro-smith/DaisyExamples/blob/master/seed/DSP/delayline/delayline.cpp#L29), we don't need to wrap our processing code in a for loop representing the audio block size.

```
for(size_t i = 0; i < size; i += 2)
{
// your DSP code here
}
```

Instead we simply output a single sample each time `void process` is called.

## Use of static

In [line 22 of the Daisy example] we can see the the DelayLine is declared as static. This applies to all variable, to ensure they take up a fixed size in memory of our embedded device. But when building VCVRack plugins, we don't and shouldn't use it. The plugin will build  but will not appear/load in VCVRack. Instead we simply do it like this:

```daisysp::DelayLine<float, MAX_DELAY> del;```

## Deriving the sample rate

We don't do that yet, our delay is hard coded to 48000Hz e.g.

```
#define MAX_DELAY static_cast<size_t>(48000 * 0.75f)
```

According to ChatGPT we could derive the sample rate by using `rack::engineGetSampleRate();` and by including "Rack.hpp" as a header, but we cannot use rack::engineGetSampleRate() directly in the above context because it's a runtime function and cannot be evaluated at compile time. So for now we live with the hardcoded value...

Also ChatGPT is wrong

```
src/DelayProto.cpp:26:23: error: ‘engineGetSampleRate’ is not a member of ‘rack’
   26 |                 rack::engineGetSampleRate();
```

I tried `rack::engine::getSampleRate();` and `rack::engine::Engine::getSampleRate();` and with the latter I get the error: `error: cannot call member function ‘float rack::engine::Engine::getSampleRate()’ without object`

## Panel Design

All text must be coverted to paths i.e. in Inkscape: Select the text, from Path menu choose `Object to Path`