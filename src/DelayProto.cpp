// TODO: derive the sample rate from VCV Rack rather than hard coding it (to 48000)
// TODO: figure out why auto panel generation didn't work
// TODO: interpolation between delay time positions doesn't really work (zipper noise remains)

// #include <iostream> // for std::cout
#include "plugin.hpp"
#include "daisysp.h"

#define SAMPLE_RATE 48000
#define MIN_SHORT_DELAY_TIME 0.1f
#define MAX_SHORT_DELAY_TIME 1.0f
#define MIN_LONG_DELAY_TIME 0.5f
#define MAX_LONG_DELAY_TIME 5.0f
#define MAX_DELAY static_cast<size_t>(SAMPLE_RATE * MAX_LONG_DELAY_TIME)
#define MAX_FEEDBACK 0.75f

// clamp function
template<typename T>
const T& clamp(const T& value, const T& low, const T& high) {
    return (value < low) ? low : (value > high) ? high : value;
}

struct DelayProto : Module {
	enum ParamId {
		DELAY_TIME_PARAM,
		FEEDBACK_PARAM,
		SWITCH_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		DELAY_TIME_CV_INPUT,
		FEEDBACK_CV_INPUT,
		AUDIO_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIO_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LED,
		LIGHTS_LEN
	};

	DelayProto() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		float displayBase = 0.0f;
		float displayMultiplier = MAX_SHORT_DELAY_TIME - MIN_SHORT_DELAY_TIME;
		float displayOffset = MIN_SHORT_DELAY_TIME;
		configParam(DELAY_TIME_PARAM, 0.f, 1.f, 0.f, "Delay Time", " secs", displayBase, displayMultiplier, displayOffset);
		configParam(FEEDBACK_PARAM, 0.f, 1.f, 1.f, "Feedback");
		configSwitch(SWITCH_PARAM, 0.f, 1.f, 0.f, "Delay Length", {"Short", "Long"});
		configLight(LED, "");
		configInput(DELAY_TIME_CV_INPUT, "Delay Time CV");
		configInput(FEEDBACK_CV_INPUT, "Feedback CV");
		configInput(AUDIO_INPUT, "Audio");
		configOutput(AUDIO_OUTPUT, "Audio");
	}

	void process(const ProcessArgs& args) override {
		float feedback, del_out, sig_out;
		float minDelayTimeSecs, maxDelayTimeSecs;

		// Parameter 1: Delay Time (secs)
		float delayTimeParam = params[DELAY_TIME_PARAM].getValue() + (inputs[DELAY_TIME_CV_INPUT].getVoltage() / 5.0f);
		float clampedDelayTimeParam = clamp(delayTimeParam, 0.f, 1.f);

		// Choose min and max delay times based on position of switch
		if (params[SWITCH_PARAM].getValue() == 0)
		{
			lights[LED].setBrightness(0);
			paramQuantities[DELAY_TIME_PARAM]->displayMultiplier = MAX_SHORT_DELAY_TIME - MIN_SHORT_DELAY_TIME;
			paramQuantities[DELAY_TIME_PARAM]->displayOffset = MIN_SHORT_DELAY_TIME;
			minDelayTimeSecs = MIN_SHORT_DELAY_TIME;
			maxDelayTimeSecs = MAX_SHORT_DELAY_TIME;
		}
		else
		{
			lights[LED].setBrightness(1);
			paramQuantities[DELAY_TIME_PARAM]->displayMultiplier = MAX_LONG_DELAY_TIME - MIN_LONG_DELAY_TIME;
			paramQuantities[DELAY_TIME_PARAM]->displayOffset = MIN_LONG_DELAY_TIME;
			minDelayTimeSecs = MIN_LONG_DELAY_TIME;
			maxDelayTimeSecs = MAX_LONG_DELAY_TIME;
		}
		
		float rangeDelayTimeSecs = maxDelayTimeSecs - minDelayTimeSecs;
		float delayTime = (clampedDelayTimeParam * rangeDelayTimeSecs) + minDelayTimeSecs;

		// smoothing quick changes between delay times
		float smoothedDelayTime;
		float coeff = 1.0f / (2.5f * args.sampleRate);
		daisysp::fonepole(smoothedDelayTime, delayTime, coeff); 
		
		del.SetDelay(args.sampleRate * delayTime);

        del_out = del.Read();
        sig_out  = del_out + inputs[AUDIO_INPUT].getVoltage();

		// Parameter 2: Feedback
		float feedbackParam = params[FEEDBACK_PARAM].getValue() + (inputs[FEEDBACK_CV_INPUT].getVoltage() / 5.0f);
		float clampedFeedbackParam = clamp(feedbackParam, 0.f, 1.f);
        feedback = (del_out * clampedFeedbackParam * MAX_FEEDBACK) + inputs[AUDIO_INPUT].getVoltage();
        del.Write(feedback);

		outputs[AUDIO_OUTPUT].setVoltage(sig_out);
	}
	// APP->engine->getSampleRate() // I can get the sampleRate like this but still can't use it to instantiate the delay line below
	daisysp::DelayLine<float, MAX_DELAY> del;
};

struct DelayProtoWidget : ModuleWidget {
	DelayProtoWidget(DelayProto* module) {
		setModule(module);
		//setPanel(createPanel(asset::plugin(pluginInstance, "res/DelayProto.svg")));
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DelayProto.svg")));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(12.646, 26.755)), module, DelayProto::DELAY_TIME_PARAM));
		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(12.646, 56.388)), module, DelayProto::FEEDBACK_PARAM));

		addParam(createParamCentered<BefacoSwitch>(mm2px(Vec(7.297, 80.603)), module, DelayProto::SWITCH_PARAM));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(18.134, 80.603)), module, DelayProto::LED));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.297, 96.859)), module, DelayProto::DELAY_TIME_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.134, 96.859)), module, DelayProto::FEEDBACK_CV_INPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.297, 113.115)), module, DelayProto::AUDIO_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 113.115)), module, DelayProto::AUDIO_OUTPUT));
	}
};

Model* modelDelayProto = createModel<DelayProto, DelayProtoWidget>("DelayProto");
