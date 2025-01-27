//----------------------------------------------------------------------------
//	EMC23 Tech tools Plugin for VCV Rack -
//  Copyright (C) 2021  EMC23
//----------------------------------------------------------------------------
#include "plugin.hpp"

struct Sinth : Module {

 float phase = 0.f;
	float blinkPhase = 0.f;

	enum ParamIds {
		PITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SINE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

    Sinth() {
            config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs &args) override;
};

void Sinth::process(const ProcessArgs &args) {
   	// Compute the frequency from the pitch parameter and input
		float pitch = params[PITCH_PARAM].getValue();
		pitch += inputs[PITCH_INPUT].getVoltage();
		pitch = clamp(pitch, -4.f, 4.f);
		// The default pitch is C4 = 261.6256f
		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

		// Accumulate the phase
		phase += freq * args.sampleTime;
		if (phase >= 0.5f)
			phase -= 1.f;

		// Compute the sine output
		//float sine = std::sin(2.f * M_PI * phase) +std::sin(2.f * M_PI * phase * 2.0f);
	//	float sine = Chicken_process2(processor, phase);
		// Audio signals are typically +/-5V
		// https://vcvrack.com/manual/VoltageStandards.html
		outputs[SINE_OUTPUT].setVoltage(5.f * 1.0);

		// Blink light at 1Hz
		blinkPhase += args.sampleTime;
		if (blinkPhase >= 1.f)
			blinkPhase -= 1.f;
		lights[BLINK_LIGHT].setBrightness(blinkPhase < 0.5f ? 1.f : 0.f);
	}


struct SinthWidget : ModuleWidget {
    SinthWidget(Sinth* module) {
        setModule(module);

        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Sinth.svg")));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.24, 46.063)), module, Sinth::PITCH_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.24, 77.478)), module, Sinth::PITCH_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.24, 108.713)), module, Sinth::SINE_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.24, 25.81)), module, Sinth::BLINK_LIGHT));

    }
};

Model * modelSinth = createModel<Sinth,SinthWidget>("Sinth");