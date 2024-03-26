This is a simple delay line build as a plugin for VCVRack using the DaisySP library. It is a first step towards prototyping a Daisy seed based audio processing Eurorack module.

# Things I learned building this module

I learned much more about the general structure of the plugin code in `DelayProto.cpp`, including how to declare the four categories of elements that make up a plugin:

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

The `struct DelayProtoWidget : ModuleWidget` represents the user interface. Here we just add the elements we use...

```
addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 77.478)), module, DelayProto::AUDIO_INPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.713)), module, DelayProto::AUDIO_OUTPUT));
```

## Namespaces

We can reference our DaisySP code with or without a namespace.

### With a namespace

For example by including...
```
using namespace daisysp;
```

...before our module declaration ```struct DelayProto : Module```

### Without a namespace (chose this approach for now)

```
daisysp::DelayLine<float, MAX_DELAY> del;
```

### We don't need to wrap our processing code in a for loop representing the audio block size

Instead it's okay to process a single sample each time void process is called

### Use of static

We don't use it:

```
	// static DelayLine<float, MAX_DELAY> del;
	daisysp::DelayLine<float, MAX_DELAY> del;
```

### Deriving the sample rate

We don't do that yet, our delay is hard coded to 48000Hz:

#define MAX_DELAY static_cast<size_t>(48000 * 0.75f)