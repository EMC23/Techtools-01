//----------------------------------------------------------------------------
//	EMC23 Tech tools Plugin for VCV Rack - 16 Step Glitch Sequencer Rompler
//  Copyright (C) 2022  EMC23
//----------------------------------------------------------------------------
#include "plugin.hpp"
#include "../inc/Utility.hpp"
#include "../inc/GateProcessor.hpp"
#include "../inc/SequencerExpanderMessage.hpp"
#include <iostream>
#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;
#include "../components/CountModulaComponents.hpp"
#include "../components/CountModulaPushButtons.hpp"
#include "../components/StdComponentPositions.hpp"
using namespace std;
#include <osdialog.h>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include <vector>
#include "cmath"
#include <dirent.h>
#include <algorithm> //----added by Joakim Lindbom
// #include "PLAY.hpp"
#define STRUCT_NAME Glompler
#define WIDGET_NAME GlomplerWidget
#define MODULE_NAME "Glompler"
#define PANEL_FILE "Glompler.svg"
#define MODEL_NAME	modelGlompler
#define TRIGSEQ_NUM_ROWS	4
#define TRIGSEQ_NUM_STEPS	16
// set the module name for the theme selection functions
#define THEME_MODULE_NAME Glompler
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

struct Glompler : Module {

	enum ParamIds {
		ENUMS(STEP_PARAMS, 16 * 4),
		ENUMS(LENGTH_PARAMS, 4),
		ENUMS(CV_PARAMS, 4),
		ENUMS(MUTE_PARAMS, 4 * 2),
   	    ENUMS(PREV_PARAM2, 8),
		ENUMS(NEXT_PARAM2, 8),
		ENUMS(LSPEED_PARAM2, 8),
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(RUN_INPUTS, 4),
		ENUMS(CLOCK_INPUTS, 4),
		ENUMS(RESET_INPUTS, 4),
		ENUMS(CV_INPUTS, 4),
	    ENUMS(TRIG_INPUT2, 8),
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(TRIG_OUTPUTS, 4 * 2),
		ENUMS(OUT_OUTPUT2, 8),
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(STEP_LIGHTS, 16 * 4),
		ENUMS(TRIG_LIGHTS, 4 * 2),
		ENUMS(LENGTH_LIGHTS,  16 * 4),
		ENUMS(MUTE_PARAM_LIGHTS, 4 * 2),
		NUM_LIGHTS
	};

	unsigned int channels;
	unsigned int sampleRate;
    drwav_uint64 totalSampleCount2[8]={};
    vector<vector<float>> playBuffer2[8]={};

    bool loading2[8] = {false,false,false,false,false,false,false,false};

	bool run2[8] = {false,false,false,false,false,false,false,false};

	std::string lastPath2[8] = {
		asset::plugin(pluginInstance,"res/Sounds/01.wav"),
		asset::plugin(pluginInstance,"res/Sounds/02.wav"),
        asset::plugin(pluginInstance,"res/Sounds/03.wav"),
		asset::plugin(pluginInstance,"res/Sounds/04.wav"),
		asset::plugin(pluginInstance,"res/Sounds/05.wav"),
		asset::plugin(pluginInstance,"res/Sounds/06.wav"),
        asset::plugin(pluginInstance,"res/Sounds/07.wav"),
        asset::plugin(pluginInstance,"res/Sounds/08.wav")
		};


 	float samplePos2[8] = {0,0,0,0,0,0,0,0};

	std::string fileDesc2[8] = {};

    bool fileLoaded2[8] = {true,true,true,true,true,true,true,true};

	bool reload2[8] = {false,false,false,false,false,false,false,false};

 	int sampnumber2[8] = {0,0,0,0,0,0,0,0};

	vector <std::string> fichier2[8]={};
	dsp::SchmittTrigger loadsampleTrigger2[8]={};
	dsp::SchmittTrigger playTrigger2[8]={};
	dsp::SchmittTrigger nextTrigger2[8]={};
	dsp::SchmittTrigger prevTrigger2[8]={};



	json_t *dataToJson() override {

		json_t *rootJ = json_object();// lastPath
		//json_t *root = json_object();

		json_object_set_new(rootJ, "moduleVersion", json_real(moduleVersion));

		json_t *currentStep = json_array();
		json_t *clk = json_array();
		json_t *run = json_array();

		for (int i = 0; i < 4; i++) {
			json_array_insert_new(currentStep, i, json_integer(count[i]));
			json_array_insert_new(clk, i, json_boolean(gateClock[i].high()));
			json_array_insert_new(run, i, json_boolean(gateRun[i].high()));
		}

		json_object_set_new(rootJ, "currentStep", currentStep);
		json_object_set_new(rootJ, "clockState", clk);
		json_object_set_new(rootJ, "runState", run);
    	json_object_set_new(rootJ, "lastPath9", json_string(lastPath2[7].c_str()));
		json_object_set_new(rootJ, "lastPath8", json_string(lastPath2[6].c_str()));
		json_object_set_new(rootJ, "lastPath7", json_string(lastPath2[5].c_str()));
		json_object_set_new(rootJ, "lastPath6", json_string(lastPath2[4].c_str()));
		json_object_set_new(rootJ, "lastPath5", json_string(lastPath2[3].c_str()));
		json_object_set_new(rootJ, "lastPath4", json_string(lastPath2[2].c_str()));
		json_object_set_new(rootJ, "lastPath3", json_string(lastPath2[1].c_str()));
		json_object_set_new(rootJ, "lastPath2", json_string(lastPath2[0].c_str()));

				// add the theme details
		#include "../themes/dataToJson.hpp"
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{	 // lastPath

    	json_t *version = json_object_get(rootJ, "moduleVersion");
		json_t *currentStep = json_object_get(rootJ, "currentStep");
		json_t *clk = json_object_get(rootJ, "clockState");
		json_t *run = json_object_get(rootJ, "runState");

		if (version)
			moduleVersion = json_number_value(version);

		for (int i = 0; i < 4; i++) {
			if (currentStep) {
				json_t *v = json_array_get(currentStep, i);
				if (v)
					count[i] = json_integer_value(v);
			}

			if (clk) {
				json_t *v = json_array_get(clk, i);
				if (v)
					gateClock[i].preset(json_boolean_value(v));
			}

			if (run) {
				json_t *v = json_array_get(run, i);
				if (v)
					gateRun[i].preset(json_boolean_value(v));

				running[i] = gateRun[i].high();
			}
		}

		// older module version, use the old length CV scale
		if (moduleVersion < 1.1f) {
			lengthCVScale = (float)(16 - 1);
		}
		// grab the theme details
		#include "../themes/dataFromJson.hpp"
		startUpCounter = 20;
		for (int r = 0; r < 8; r++) {
				reload2[r] = true ;
				loadSample2(lastPath2[r],r);
		}
	}
/////////////////////////////////////////////////////////////////////////////////////////
		void loadSample2(std::string path2,int r)
		{
		 		loading2[r] = true;
		 		unsigned int c;
		 		unsigned int sr;
				drwav_uint64 sc;
		 		float *pSampleData2;
				pSampleData2 = drwav_open_and_read_file_f32(path2.c_str(), &c, &sr, &sc);




			if (pSampleData2 != NULL) {
		 		channels = c;
		 		sampleRate = sr;
				playBuffer2[r][0].clear();
				for (unsigned int i=0; i < sc; i = i + c) {
					playBuffer2[r][0].push_back(pSampleData2[i]);
				}
				totalSampleCount2[r] = playBuffer2[r][0].size();
				drwav_free(pSampleData2);
				loading2[r] = false;
				fileLoaded2[r] = true;
				fileDesc2[r] = system::getFilename(path2);
				if (reload2[r]) {
					DIR* rep2 = NULL;
					struct dirent* dirp = NULL;
					std::string dir2 = path2.empty() ? NULL :system::getDirectory(path2); /////////////////////////////////////////
					rep2 = opendir(dir2.c_str());
					int i = 0;
					fichier2[r].clear();
					while ((dirp = readdir(rep2)) != NULL) {
						std::string name = dirp->d_name;
						std::size_t found = name.find(".wav",name.length()-5);
						if (found==std::string::npos) found = name.find(".WAV",name.length()-5);
						if (found!=std::string::npos) {
							fichier2[r].push_back(name);
							if ((dir2 + "/" + name)==path2) {sampnumber2[r] = i;}
							i=i+1;
							}
						}
		//----added by Joakim Lindbom
				sort(fichier2[r].begin(), fichier2[r].end());  // Linux needs this to get files in right order
					for (int o=0;o<int(fichier2[r].size()-1); o++) {
						if ((dir2 + "/" + fichier2[r][o])==path2) {
							sampnumber2[r] = o;
						}
					}
		//---------------
					closedir(rep2);
					reload2[r] = false;
				}
					lastPath2[r] = path2;
			}
			else {
				fileLoaded2[r] = false;
			}
		};

	float moduleVersion = 1.1f;

	GateProcessor gateClock[4];
	GateProcessor gateReset[4];
	GateProcessor gateRun[4];
	dsp::PulseGenerator pgClock[4 * 2];

	int count[4] = {};
	int length[4] = {};
	bool running[4] = {};

	float lengthCVScale = (float)(16);

	int startUpCounter = 0;

////#ifdef SEQUENCER_EXP_MAX_CHANNELS
//	SequencerExpanderMessage rightMessages[2][1]; // messages to right module (expander)
//#endif

	// add the variables we'll use when managing themes
	#include "../themes/variables.hpp"

	Glompler() {


		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		char rowText[20];

		 for (int r = 0; r < 8; r++) {
		 	 configParam(PREV_PARAM2 + r, 0.f, 1.f, 0.f);
		 	 configParam(NEXT_PARAM2 + r, 0.f, 1.f, 0.f);
		 	 configParam(LSPEED_PARAM2 + r, -5.0f, 5.0f, 0.0f, "Speed");
			 playBuffer2[r].resize(1);
			 playBuffer2[r][0].resize(0);
		 	}

		for (int r = 0; r < 4; r++) {

			// length & CV parms
			sprintf(rowText, "Channel %d length", r + 1);
			configParam(LENGTH_PARAMS + r, 1.0f, (float)(16), (float)(16), rowText);

			// row lights and switches
			int i = 0;
			char stepText[20];
			for (int s = 0; s < 16; s++) {
				sprintf(stepText, "Step %d select", s + 1);
				configParam(STEP_PARAMS + (r * 16) + i++, 0.0f, 2.0f, 1.0f, stepText);
			}

			// output lights, mute buttons and jacks
			for (int i = 0; i < 2; i++) {
				configParam(MUTE_PARAMS + + (r * 2) + i, 0.0f, 1.0f, 0.0f, "Mute this output");
			}
		}

		// set the theme from the current default value
		#include "../themes/setDefaultTheme.hpp"
	}

	void onReset() override {

		for (int i = 0; i < 4; i++) {
			gateClock[i].reset();
			gateReset[i].reset();
			gateRun[i].reset();
			pgClock[i].reset();
			count[i] = 0;
			length[i] = 16;
		}
	}

	void process(const ProcessArgs &args) override {
		// wait a number of cycles before we use the clock and run inputs to allow them propagate correctly after startup
		if (startUpCounter > 0)
			startUpCounter--;

		// grab all the input values up front
		float reset = 0.0f;
		float run = 10.0f;
		float clock = 0.0f;
		float f;

		// bool gateOutputs[SEQUENCER_EXP_NUM_TRIGGER_OUTS] = {};

		for (int r = 0; r < 4; r++) {
			// reset input
			f = inputs[RESET_INPUTS + r].getNormalVoltage(reset);
			gateReset[r].set(f);
			reset = f;

			if (startUpCounter == 0) {
				// run input
				f = inputs[RUN_INPUTS + r].getNormalVoltage(run);
				gateRun[r].set(f);
				run = f;

				// clock
				f = inputs[CLOCK_INPUTS + r].getNormalVoltage(clock);
				gateClock[r].set(f);
				clock = f;
			}

			// sequence length - jack overrides knob
			if (inputs[CV_INPUTS + r].isConnected()) {
				// scale the input such that 10V = step 16, 0V = step 1
				length[r] = (int)(clamp(lengthCVScale/10.0f * inputs[CV_INPUTS + r].getVoltage(), 0.0f, lengthCVScale)) + 1;
			}
			else {
				length[r] = (int)(params[LENGTH_PARAMS + r].getValue());
			}

			// set the length lights
			for(int i = 0; i < 16; i++) {
				lights[LENGTH_LIGHTS + (r * 16) + i].setBrightness(boolToLight(i < length[r]));
			}
		}

		// now process the steps for each row as required
		for (int r = 0; r < 4; r++) {
			if (gateReset[r].leadingEdge()) {
				// reset the count at zero
				count[r] = 0;
			}

			// process the clock trigger - we'll use this to allow the run input edge to act like the clock if it arrives shortly after the clock edge
			bool clockEdge = gateClock[r].leadingEdge();
			if (clockEdge)
				pgClock[r].trigger(1e-4f);
			else if (pgClock[r].process(args.sampleTime)) {
				// if within cooey of the clock edge, run or reset is treated as a clock edge.
				clockEdge = (gateRun[r].leadingEdge() || gateReset[r].leadingEdge());
			}

			if (gateRun[r].low())
				running[r] = false;

			// advance count on positive clock edge or the run edge if it is close to the clock edge
			if (clockEdge && gateRun[r].high()) {

				// flag that we are now actually running
				running[r] = true;

				count[r]++;

				if (count[r] > length[r])
					count[r] = 1;
			}

			// now process the lights and outputs
			bool outA = false, outB = false;
			for (int c = 0; c < 16; c++) {
				// set step lights here
				bool stepActive = (c + 1 == count[r]);
				lights[STEP_LIGHTS + (r * 16) + c].setBrightness(boolToLight(stepActive));

				// now determine the output values
				if (stepActive) {
					switch ((int)(params[STEP_PARAMS + (r * 16) + c].getValue())) {
						case 0: // lower output
							outA = false;
							outB = true;
							break;
						case 2: // upper output
							outA = true;
							outB = false;
							break;
						default: // off
							outA = false;
							outB = false;
							break;
					}
				}
			}

			// save the gates for passing across the gate expander later

	//		gateOutputs[r * 2] = outA && (params[MUTE_PARAMS + (r * 2)].getValue() < 0.5f);
	//		gateOutputs[(r * 2) + 1] = outB && (params[MUTE_PARAMS + (r * 2) + 1].getValue() < 0.5f);

			// outputs follow clock width
			outA &= (running[r] && gateClock[r].high() && (params[MUTE_PARAMS + (r * 2)].getValue() < 0.5f));
			outB &= (running[r] && gateClock[r].high() && (params[MUTE_PARAMS + (r * 2) + 1].getValue() < 0.5f));

			// set the outputs accordingly
			outputs[TRIG_OUTPUTS + (r * 2)].setVoltage(boolToGate(outA));
			outputs[TRIG_OUTPUTS + (r * 2) + 1].setVoltage(boolToGate(outB));
			lights[TRIG_LIGHTS + (r * 2)].setBrightness(boolToLight(outA));
			lights[TRIG_LIGHTS + (r * 2) + 1].setBrightness(boolToLight(outB));
		}
		for (int r = 0; r < 8; r++)
		{
				if (fileLoaded2[r]) {
					if (nextTrigger2[r].process(params[NEXT_PARAM2 + r].getValue()))
						{
						std::string dir2 = lastPath2[r].empty() ? NULL : system::getDirectory(lastPath2[r]); ///////////////////////assetLocal()
						if (sampnumber2[r] < int(fichier2[r].size()-1)) sampnumber2[r]=sampnumber2[r]+1; else sampnumber2[r] =0;
						loadSample2(dir2 + "/" + fichier2[r][sampnumber2[r]],r);
						}
					if (prevTrigger2[r].process(params[PREV_PARAM2 + r].getValue()))
						{
						std::string dir2 = lastPath2[r].empty() ? NULL : system::getDirectory(lastPath2[r]); /////////////////////////////////////////
						if (sampnumber2[r] > 0) sampnumber2[r]=sampnumber2[r]-1; else sampnumber2[r] =int(fichier2[r].size()-1);
						loadSample2(dir2 + "/" + fichier2[r][sampnumber2[r]],r);
						}
				} else{

                for (int r = 0; r < 8; r++) {
				reload2[r] = true ;
				loadSample2(lastPath2[r],r);
		}

				}


	//	if (inputs[TRIG_INPUT2 + r ].isConnected()) {
		//		channels = 1;

				if (playTrigger2[r].process(outputs[TRIG_OUTPUTS + r].getVoltage()))
					{
					run2[r] = true;
					samplePos2[r] = 0;
				}
		//		}

			if ((!loading2[r]) && (run2[r]) && ((abs(floor(samplePos2[r])) < totalSampleCount2[r]))) {
				if (samplePos2[r]>=0)
				outputs[OUT_OUTPUT2 + r].setVoltage(5 * playBuffer2[r][0][floor(samplePos2[r])]);
			else outputs[OUT_OUTPUT2 + r].setVoltage(5 * playBuffer2[r][0][floor(totalSampleCount2[r]-1+samplePos2[r])]);
			samplePos2[r] = samplePos2[r]+1+(params[LSPEED_PARAM2 + r].getValue()) /3;
			}
			else
			{
			run2[r] = false;
		outputs[OUT_OUTPUT2 + r].setVoltage(0);
			}
		}
        /////////////////////////////////End Process 1
	}
};
struct upButton : app::SvgSwitch {
	upButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/upButton.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/upButtonDown.svg")));
	}
};
struct downButton : app::SvgSwitch {
	downButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/downButton.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/downButtonDown.svg")));
	};
};
struct PLAYDisplay : TransparentWidget {
	Glompler *module;
	int frame = 0;
	shared_ptr<Font> font;

	PLAYDisplay() {
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/LEDCalculator.ttf"));
	}
	void draw(const DrawArgs &args) override {
	   for (int r = 0; r < 8; r++) {
		   std::string fD2= module ? module->fileDesc2[r] : "load sample" ;
		   std::string to_display2 = "";
        for (int i=0; i<14; i++) to_display2 = to_display2 + fD2[i];
		nvgFontSize(args.vg, 24);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 0);
		nvgFillColor(args.vg, nvgRGBA(0x4c, 0xc7, 0xf3, 0xff));
		// nvgRotate(args.vg, -M_PI / 2.0f);
	   nvgTextBox(args.vg, 740 +  10, 5 + (r*42) ,350, to_display2.c_str(), NULL);
	   }
 	}
};
struct PLAYItem : MenuItem {
	Glompler *rm ;
	void onAction(const event::Action &e) override {

	    char *path2[8] = {
			osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL),
			osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL),
			osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL),
			osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL),
			osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL),
			osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL),
			osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL),
			osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL)
		};

 	for (int r = 0; r < 8; r++) {
		if (path2[r]) {
			rm->run2[r] = false;
			rm->reload2[r] = true;
			rm->loadSample2(path2[r],r);
			rm->samplePos2[r] = 0;
			rm->lastPath2[r] = std::string(path2[r]);
			free(path2[r]);
		}
 		}


	}
};
struct GlomplerWidget : ModuleWidget {

	std::string panelName;

	GlomplerWidget(Glompler *module) {
		setModule(module);
		panelName = "Glompler.svg";
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/" + panelName)));

	//	setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PLAY.svg")));
	{
		PLAYDisplay *gdisplay = new PLAYDisplay();
		gdisplay->box.pos = Vec(38, 45);
		gdisplay->box.size = Vec(830, 250);
		gdisplay->module = module;
		addChild(gdisplay);
	}

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 865)));

	for (int r = 0; r < 8; r++) {
				addParam(createParam<Trimpot>(Vec(750 + 275, 31 + (r * 42)), module, Glompler::LSPEED_PARAM2 + r));
				// addInput(createInput<PJ301MPort>(Vec(750 + 3, 31 + (r * 42)), module, Glompler::TRIG_INPUT2 + r ));
				addParam(createParam<upButton>(Vec(750 + 296, 21 + (r * 42)), module, Glompler::PREV_PARAM2 + r));
				addParam(createParam<downButton>(Vec(750 + 296, 41 + (r * 42)), module, Glompler::NEXT_PARAM2 + r));
				addOutput(createOutput<PJ301MPort>(Vec(750 + 324, 31 + (r * 42)), module, Glompler::OUT_OUTPUT2 + r));
		}

		// screws
		#include "../components/stdScrews.hpp"

		// add everything row by row
		int rowOffset = -10;
		for (int r = 0; r < 4; r++) {
			// run input
			addInput(createInputCentered<CountModulaJack>(Vec(STD_COLUMN_POSITIONS[STD_COL1], STD_ROWS8[STD_ROW1 + (r * 2)]), module, Glompler::RUN_INPUTS + r));

			// reset input
			addInput(createInputCentered<CountModulaJack>(Vec(STD_COLUMN_POSITIONS[STD_COL1], STD_ROWS8[STD_ROW2 + (r * 2)]), module, Glompler::RESET_INPUTS + r));

			// clock input
			addInput(createInputCentered<CountModulaJack>(Vec(STD_COLUMN_POSITIONS[STD_COL3]-15, STD_ROWS8[STD_ROW1 + (r * 2)]), module, Glompler::CLOCK_INPUTS + r));

			// CV input
			addInput(createInputCentered<CountModulaJack>(Vec(STD_COLUMN_POSITIONS[STD_COL3]-15, STD_ROWS8[STD_ROW2 + (r * 2)]), module, Glompler::CV_INPUTS + r));

			// length & CV params
			switch (r % 4) {
				case 0:
					addParam(createParamCentered<CountModulaRotarySwitchRed>(Vec(STD_COLUMN_POSITIONS[STD_COL4], STD_HALF_ROWS8(STD_ROW1 + (r * 2))), module, Glompler::LENGTH_PARAMS + r));
					break;
				case 1:
					addParam(createParamCentered<CountModulaRotarySwitchGreen>(Vec( STD_COLUMN_POSITIONS[STD_COL4], STD_HALF_ROWS8(STD_ROW1 + (r * 2))), module, Glompler::LENGTH_PARAMS + r));
					break;
				case 2:
					addParam(createParamCentered<CountModulaRotarySwitchYellow>(Vec(STD_COLUMN_POSITIONS[STD_COL4], STD_HALF_ROWS8(STD_ROW1 + (r * 2))), module, Glompler::LENGTH_PARAMS + r));
					break;
				case 3:
					addParam(createParamCentered<CountModulaRotarySwitchBlue>(Vec(STD_COLUMN_POSITIONS[STD_COL4], STD_HALF_ROWS8(STD_ROW1 + (r * 2))), module, Glompler::LENGTH_PARAMS + r));
					break;
			}

			// row lights and switches
			int i = 0;
			for (int s = 0; s < 16; s++) {
				addChild(createLightCentered<MediumLight<RedLight>>(Vec(STD_COLUMN_POSITIONS[STD_COL6 + s] - 15, STD_ROWS8[STD_ROW1 + (r * 2)] + rowOffset), module, Glompler::STEP_LIGHTS + (r * 16) + i));
				addChild(createLightCentered<SmallLight<GreenLight>>(Vec(STD_COLUMN_POSITIONS[STD_COL6 + s]- 5, STD_ROWS8[STD_ROW1 + (r * 2)] + 3), module, Glompler::LENGTH_LIGHTS + (r * 16) + i));
				addParam(createParamCentered<CountModulaToggle3P>(Vec(STD_COLUMN_POSITIONS[STD_COL6 + s] - 15, STD_ROWS8[STD_ROW2 + (r * 2)] + rowOffset), module, Glompler:: STEP_PARAMS + (r * 16) + i++));
			}

			// output lights, mute buttons and jacks
			for (int i = 0; i < 2; i++) {
			//	addParam(createParamCentered<CountModulaLEDPushButton<CountModulaPBLight<GreenLight>>>(Vec(STD_COLUMN_POSITIONS[STD_COL6 + 16], STD_ROWS8[STD_ROW1 + (r * 2) + i]), module, Glompler::MUTE_PARAMS + + (r * 2) + i, Glompler::MUTE_PARAM_LIGHTS + + (r * 2) + i));
				addChild(createLightCentered<MediumLight<RedLight>>(Vec(STD_COLUMN_POSITIONS[STD_COL6 + 16 + 1], STD_ROWS8[STD_ROW1 + (r * 2) + i]), module, Glompler::TRIG_LIGHTS + (r * 2) + i));
			//	addOutput(createOutputCentered<CountModulaJack>(Vec(STD_COLUMN_POSITIONS[STD_COL6 + 16 + 2], STD_ROWS8[STD_ROW1 + (r * 2) + i]), module, Glompler::TRIG_OUTPUTS + (r * 2) + i));
			}
		}
	}

	struct InitMenuItem : MenuItem {
		GlomplerWidget *widget;
		int channel = 0;
		bool allInit = true;

		void onAction(const event::Action &e) override {
			// // text for history menu item
			char buffer[100];
			if (!allInit)
				sprintf(buffer, "initialize channel %d triggers", channel + 1);
			else
				sprintf(buffer, "initialize channel %d", channel + 1);

			// history - current settings
			history::ModuleChange *h = new history::ModuleChange;
			h->name = buffer;
			h->moduleId = widget->module->id;
			h->oldModuleJ = widget->toJson();

			// we're doing the entire channel so do the common controls here
			if (allInit) {
				// length switch
		//		widget->getParam(Glompler::LENGTH_PARAMS + channel)->reset();
			}

			// triggers/gates
			for (int c = 0; c < 16; c++) {
		//		widget->getParam(Glompler::STEP_PARAMS + (channel * 16) + c)->reset();
			}

			// history - new settings
			h->newModuleJ = widget->toJson();
			APP->history->push(h);
		}
	};

	struct RandMenuItem : MenuItem {
		GlomplerWidget *widget;
		int channel = 0;
		bool allRand = true;

		void onAction(const event::Action &e) override {

			// text for history menu item
			char buffer[100];
			if (!allRand)
				sprintf(buffer, "randomize channel %d triggers", channel + 1);
			else
				sprintf(buffer, "randomize channel %d", channel + 1);

			// history - current settings
			history::ModuleChange *h = new history::ModuleChange;
			h->name = buffer;
			h->moduleId = widget->module->id;
			h->oldModuleJ = widget->toJson();

			// we're doing the entire channel so do the common controls here
			if (allRand) {
				// length switch
		//		widget->getParam(Glompler::LENGTH_PARAMS + channel)->randomize();
			}

			// triggers/gates
			for (int c = 0; c < 16; c++) {
		//		widget->getParam(Glompler::STEP_PARAMS + (channel * 16) + c)->randomize();
			}

			// history - new settings
			h->newModuleJ = widget->toJson();
			APP->history->push(h);
		}
	};

	struct ChannelInitMenuItem : MenuItem {
		GlomplerWidget *widget;
		int channel = 0;

		Menu *createChildMenu() override {
			Menu *menu = new Menu;

			// full channel init
			InitMenuItem *initMenuItem = createMenuItem<InitMenuItem>("Entire Channel");
			initMenuItem->channel = channel;
			initMenuItem->widget = widget;
			menu->addChild(initMenuItem);

			// trigger only init
			InitMenuItem *initTrigMenuItem = createMenuItem<InitMenuItem>("Gates/Triggers Only");
			initTrigMenuItem->channel = channel;
			initTrigMenuItem->widget = widget;
			initTrigMenuItem->allInit = false;
			menu->addChild(initTrigMenuItem);

			return menu;
		}

	};

	struct ChannelRandMenuItem : MenuItem {
		GlomplerWidget *widget;
		int channel = 0;

		Menu *createChildMenu() override {
			Menu *menu = new Menu;

			// full channel random
			RandMenuItem *randMenuItem = createMenuItem<RandMenuItem>("Entire Channel");
			randMenuItem->channel = channel;
			randMenuItem->widget = widget;
			menu->addChild(randMenuItem);

			// trigger only random
			RandMenuItem *randTrigMenuItem = createMenuItem<RandMenuItem>("Gates/Triggers Only");
			randTrigMenuItem->channel = channel;
			randTrigMenuItem->widget = widget;
			randTrigMenuItem->allRand = false;
			menu->addChild(randTrigMenuItem);

			return menu;
		}
	};

	struct ChannelMenuItem : MenuItem {
		GlomplerWidget *widget;
		int channel = 0;

		Menu *createChildMenu() override {
			Menu *menu = new Menu;

			// initialize
			ChannelInitMenuItem *initMenuItem = createMenuItem<ChannelInitMenuItem>("Initialize", RIGHT_ARROW);
			initMenuItem->channel = channel;
			initMenuItem->widget = widget;
			menu->addChild(initMenuItem);

			// randomize
			ChannelRandMenuItem *randMenuItem = createMenuItem<ChannelRandMenuItem>("Randomize", RIGHT_ARROW);
			randMenuItem->channel = channel;
			randMenuItem->widget = widget;
			menu->addChild(randMenuItem);

			return menu;
		}
	};

	// include the theme menu item struct we'll when we add the theme menu items
	#include "../themes/ThemeMenuItem.hpp"

	void appendContextMenu(Menu *menu) override {
		Glompler *module = dynamic_cast<Glompler*>(this->module);
		assert(module);

		// blank separator
		menu->addChild(new MenuSeparator());

		// add the theme menu items
		#include "../themes/themeMenus.hpp"

		char textBuffer[100];
		for (int r = 0; r < 4; r++) {

			sprintf(textBuffer, "Channel %d", r + 1);
			ChannelMenuItem *chMenuItem = createMenuItem<ChannelMenuItem>(textBuffer, RIGHT_ARROW);
			chMenuItem->channel = r;
			chMenuItem->widget = this;
			menu->addChild(chMenuItem);
		}

		//menu->addChild(new MenuEntry);
    	//PLAYItem *rootDirItem = new PLAYItem;
		//rootDirItem->text = "Load Sample";
		//rootDirItem->rm = module;
		//menu->addChild(rootDirItem);

		// PLAYItem *rootDirItem2 = new PLAYItem;
		// rootDirItem2->text = "Load Sample2";
		// rootDirItem2->rm = module;
		// menu->addChild(rootDirItem2);

	}

	void step() override {
		if (module) {
			// process any change of theme
			#include "../themes/step.hpp"
		}

		Widget::step();
	}
};
Model * modelGlompler = createModel<Glompler, GlomplerWidget>("Glompler");

