//----------------------------------------------------------------------------
//	EMC23 Tech tools Plugin for VCV Rack -
//  Copyright (C) 2021  EMC23
//----------------------------------------------------------------------------
#include "plugin.hpp"

struct AComparator : Module {

    enum ParamIds {
    NUM_PARAMS,
    };

    enum InputIds {
    INPUTA1,
    INPUTB1,
    INPUTA2,
    INPUTB2,
    NUM_INPUTS,
    };

    enum OutputIds {
    OUTPUT1,
    OUTPUT2,
    NUM_OUTPUTS,
    };

    enum LightsIds {
    LIGHT_1,
    LIGHT_2,
    NUM_LIGHTS,
    };

    AComparator() {
            config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs &args) override;
};

void AComparator::process(const ProcessArgs &args) {
    if (inputs[INPUTA1].isConnected() && inputs[INPUTB1].isConnected())
    {
        float out = inputs[INPUTA1].getVoltage() >= inputs[INPUTB1].getVoltage();
        outputs[OUTPUT1].setVoltage(10.f * out);
        lights[OUTPUT1].setBrightness(out);
    }
      if (inputs[INPUTA2].isConnected() && inputs[INPUTB2].isConnected())
    {
        float out = inputs[INPUTA2].getVoltage() >= inputs[INPUTB2].getVoltage();
        outputs[OUTPUT2].setVoltage(10.f * out);
        lights[OUTPUT2].setBrightness(out);
    }
}

struct AComparatorWidget : ModuleWidget {
    AComparatorWidget(AComparator* module) {
        setModule(module);

        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/AComparator.svg")));

        addInput(createInput<PJ301MPort>(Vec(50, 78), module, AComparator::INPUTA1));

        addInput(createInput<PJ301MPort>(Vec(50, 108), module, AComparator::INPUTB1));

        addOutput(createOutput<PJ3410Port>(Vec(46, 138), module, AComparator::OUTPUT1));

        addChild(createLight<TinyLight<GreenLight>>(Vec(80, 150), module, AComparator::LIGHT_1));

        addInput(createInput<PJ301MPort>(Vec(50, 248), module, AComparator::INPUTA2));

        addInput(createInput<PJ301MPort>(Vec(50, 278), module, AComparator::INPUTB2));

        addOutput(createOutput<PJ3410Port>(Vec(46, 308), module, AComparator::OUTPUT2));

        addChild(createLight<TinyLight<GreenLight>>(Vec(80, 320), module, AComparator::LIGHT_2));
    }
};

Model *modelAComparator = createModel<AComparator,AComparatorWidget>("AComparator");
