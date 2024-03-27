#include "plugin.hpp"
#include "daisysp.h"

#define MAX_DELAY static_cast<size_t>(48000 * 0.75f)

struct DelayParamsProto : Module {
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

	DelayParamsProto() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs& args) override {
		float feedback, del_out, sig_out;
        del_out = del.Read();
        sig_out  = del_out + inputs[AUDIO_INPUT].getVoltage();
        feedback = (del_out * 0.75f) + inputs[AUDIO_INPUT].getVoltage();
        del.Write(feedback);
		outputs[AUDIO_OUTPUT].setVoltage(sig_out);
	}
	daisysp::DelayLine<float, MAX_DELAY> del;
};


struct DelayParamsProtoWidget : ModuleWidget {
	DelayParamsProtoWidget(DelayParamsProto* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DelayParamsProto.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 77.478)), module, DelayParamsProto::AUDIO_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.713)), module, DelayParamsProto::AUDIO_OUTPUT));
	}
};


Model* modelDelayParamsProto = createModel<DelayParamsProto, DelayParamsProtoWidget>("DelayParamsProto");
