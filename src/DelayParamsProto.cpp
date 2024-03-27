#include <iostream>
#include "plugin.hpp"
#include "daisysp.h"

#define SAMPLE_RATE 48000
#define MAX_DELAY static_cast<size_t>(SAMPLE_RATE * 1.0f)

// clamp function
template<typename T>
const T& clamp(const T& value, const T& low, const T& high) {
    return (value < low) ? low : (value > high) ? high : value;
}

struct DelayParamsProto : Module {
	enum ParamId {
		PARAM_ONE,
		PARAM_TWO,
		PARAMS_LEN
	};
	enum InputId {
		PARAM_ONE_CV_INPUT,
		PARAM_TWO_CV_INPUT,
		AUDIO_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIO_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	DelayParamsProto() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PARAM_ONE, 0.f, 1.f, 1.f, "Delay Time (secs)");
		configParam(PARAM_TWO, 0.f, 1.f, 1.f, "Feedback");
		configInput(PARAM_ONE_CV_INPUT, "Delay Time CV Input");
		configInput(PARAM_TWO_CV_INPUT, "Feedback CV Input");
		configInput(AUDIO_INPUT, "Audio In");
		configOutput(AUDIO_OUTPUT, "Audio Out");
	}

	void process(const ProcessArgs& args) override {
		float feedback, del_out, sig_out;

		// Parameter 1: Delay Time (secs)
		float paramOne = params[PARAM_ONE].getValue() + (inputs[PARAM_ONE_CV_INPUT].getVoltage() / 5.0f);
		float clampedParamOne = clamp(paramOne, 0.f, 1.f);
		float rangeDelayTimeSecs = maxDelayTimeSecs - minDelayTimeSecs;
		float delayTime = (clampedParamOne * rangeDelayTimeSecs) + minDelayTimeSecs;
		del.SetDelay(SAMPLE_RATE * delayTime);

        del_out = del.Read();
        sig_out  = del_out + inputs[AUDIO_INPUT].getVoltage();

		// Parameter 2: Feedback
		float paramTwo = params[PARAM_TWO].getValue() + (inputs[PARAM_TWO_CV_INPUT].getVoltage() / 5.0f);
		float clampedParamTwo = clamp(paramTwo, 0.f, 1.f);
        feedback = (del_out * clampedParamTwo) + inputs[AUDIO_INPUT].getVoltage();
        del.Write(feedback);

		outputs[AUDIO_OUTPUT].setVoltage(sig_out);
	}
	daisysp::DelayLine<float, MAX_DELAY> del;
	float minDelayTimeSecs = 0.1, maxDelayTimeSecs = 1.0;
};

struct DelayParamsProtoWidget : ModuleWidget {
	DelayParamsProtoWidget(DelayParamsProto* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/DelayParamsProto.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(9.23, 37.3)), module, DelayParamsProto::PARAM_ONE));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(9.102, 61.5)), module, DelayParamsProto::PARAM_TWO));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.635, 37.3)), module, DelayParamsProto::PARAM_ONE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.507, 61.5)), module, DelayParamsProto::PARAM_TWO_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 83)), module, DelayParamsProto::AUDIO_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.7)), module, DelayParamsProto::AUDIO_OUTPUT));
	}
};

Model* modelDelayParamsProto = createModel<DelayParamsProto, DelayParamsProtoWidget>("DelayParamsProto");
