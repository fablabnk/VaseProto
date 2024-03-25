#include "plugin.hpp"
#include "daisysp.h"

#define MAX_DELAY static_cast<size_t>(48000 * 0.75f)

using namespace daisysp;

struct VaseProto : Module {
	enum ParamIds {
		PITCH_PARAM,
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
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	VaseProto() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        // args.sampleRate;
	}

	void process(const ProcessArgs& args) override {
		float feedback, del_out, sig_out;
        del_out = del.Read();
        sig_out  = del_out + inputs[AUDIO_INPUT].getVoltage();
        feedback = (del_out * 0.75f) + inputs[AUDIO_INPUT].getVoltage();
        del.Write(feedback);
		outputs[AUDIO_OUTPUT].setVoltage(sig_out);
	}

    // for(size_t i = 0; i < size; i += 2)
    // {
    // }
	static DelayLine<float, MAX_DELAY> del;
};


struct VaseProtoWidget : ModuleWidget {
	VaseProtoWidget(VaseProto* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VaseProto.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 46.063)), module, VaseProto::PITCH_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 77.478)), module, VaseProto::AUDIO_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.713)), module, VaseProto::AUDIO_OUTPUT));

		// addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 25.81)), module, VaseProto::BLINK_LIGHT));
	}
};


Model* modelVaseProto = createModel<VaseProto, VaseProtoWidget>("VaseProto");
