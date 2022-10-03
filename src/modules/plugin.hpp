/*--------------------------- ABC ---------------------------------*
 *
 * Author: Leonardo Gabrielli <l.gabrielli@univpm.it>
 * License: GPLv3
 *
 * For a detailed guide of the code and functions see the book:
 * "Developing Virtual Synthesizers with VCV Rack" by L.Gabrielli
 *
 * Copyright 2020, Leonardo Gabrielli
 *
 *-----------------------------------------------------------------*/
//----------------------------------------------------------------------------
//	EMC23 Tech tools Plugin for VCV Rack - 16 Step Glitch Sequencer Rompler
//  Copyright (C) 2021  EMC23
//----------------------------------------------------------------------------

#include <cmath>

#include <rack.hpp>

using namespace rack;

// Forward-declare the Plugin
extern Plugin *pluginInstance;

 extern Model *modelSinth;
 extern Model *modelGlompler;
 extern Model *modelScanner;
// extern Model *modelSeq08;
// extern Model *modelAComparator;
// extern Model *modelAMuxDemux;


// settings file
json_t * readSettings();
void saveSettings(json_t *rootJ);

// settings functions
int readDefaultIntegerValue(std::string);
void saveDefaultIntegerValue(std::string setting, int value);

struct ATextLabel : TransparentWidget {
	std::shared_ptr<Font> font;
	NVGcolor txtCol;
	char text[128];
	const int fh = 14;

	ATextLabel(Vec pos) {
		box.pos = pos;
		box.size.y = fh;
		setColor(0x00, 0x00, 0x00, 0xFF);
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/DejaVuSansMono.ttf"));
		setText(" ");
	}

	ATextLabel(Vec pos, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		box.pos = pos;
		box.size.y = fh;
		setColor(r, g, b, a);
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/DejaVuSansMono.ttf"));
		setText(" ");
	}

	void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		txtCol.r = r;
		txtCol.g = g;
		txtCol.b = b;
		txtCol.a = a;
	}

	void setText(const char * txt) {
		strncpy(text, txt, sizeof(text));
		box.size.x = strlen(text) * 8;
	}

	void drawBG(const DrawArgs &args) {
		Vec c = Vec(box.size.x/2, box.size.y);
		const int whalf = box.size.x/2;

		// Draw rectangle
		nvgFillColor(args.vg, nvgRGBA(0xF0, 0xF0, 0xF0, 0xFF));
		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, c.x -whalf, c.y +2);
			nvgLineTo(args.vg, c.x +whalf, c.y +2);
			nvgLineTo(args.vg, c.x +whalf, c.y+fh+2);
			nvgLineTo(args.vg, c.x -whalf, c.y+fh+2);
			nvgLineTo(args.vg, c.x -whalf, c.y +2);
			nvgClosePath(args.vg);
		}
		nvgFill(args.vg);
	}

	void drawTxt(const DrawArgs &args, const char * txt) {

		Vec c = Vec(box.size.x/2, box.size.y);

		nvgFontSize(args.vg, fh);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, -2);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
		nvgFillColor(args.vg, nvgRGBA(txtCol.r, txtCol.g, txtCol.b, txtCol.a));

		nvgText(args.vg, c.x, c.y+fh, txt, NULL);
	}

	void draw(const DrawArgs &args) override {
		TransparentWidget::draw(args);
		drawBG(args);
		drawTxt(args, text);
	}



};

struct ATextHeading : TransparentWidget {
	std::shared_ptr<Font> font;
	NVGcolor txtCol;
	char text[128];
	const int fh = 14;

	ATextHeading(Vec pos) {
		box.pos = pos;
		box.size.y = fh;
		setColor(0xFF, 0xFF, 0xFF, 0xFF);
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/DejaVuSansMono.ttf"));
		setText(" ");
	}

	ATextHeading(Vec pos, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		box.pos = pos;
		box.size.y = fh;
		setColor(r, g, b, a);
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/DejaVuSans.ttf"));
		setText(" ");
	}

	void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		txtCol.r = r;
		txtCol.g = g;
		txtCol.b = b;
		txtCol.a = a;
	}

	void setText(const char * txt) {
		strncpy(text, txt, sizeof(text));
		box.size.x = strlen(text) * 8;
	}

	void draw(const DrawArgs &args) override {
		TransparentWidget::draw(args);
		drawBG(args);
		drawTxt(args, text);
	}

	void drawBG(const DrawArgs &args) {
		Vec c = Vec(box.size.x/2, box.size.y);
		const int whalf = box.size.x/2;

		// Draw rectangle
		nvgFillColor(args.vg, nvgRGBA(0xAA, 0xCC, 0xFF, 0xFF));
		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, c.x -whalf, c.y +2);
			nvgLineTo(args.vg, c.x +whalf, c.y +2);
			nvgLineTo(args.vg, c.x +whalf, c.y+fh+2);
			nvgLineTo(args.vg, c.x -whalf, c.y+fh+2);
			nvgLineTo(args.vg, c.x -whalf, c.y +2);
			nvgClosePath(args.vg);
		}
		nvgFill(args.vg);
	}

	void drawTxt(const DrawArgs &args, const char * txt) {

		Vec c = Vec(box.size.x/2, box.size.y);

		nvgFontSize(args.vg, fh);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, -2);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
		nvgFillColor(args.vg, nvgRGBA(txtCol.r, txtCol.g, txtCol.b, txtCol.a));

		nvgText(args.vg, c.x, c.y+fh, txt, NULL);
	}

};

struct ATitle: TransparentWidget {
	std::shared_ptr<Font> font;
	NVGcolor txtCol;
	char text[128];
	int fh = 20;
	float parentW = 0;

	ATitle(float pW) {
		parentW = pW;
		box.pos = Vec(1 , 1);
		box.size.y = fh;
		setColor(0x55, 0x99, 0xFF, 0xFF);
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/DejaVuSans.ttf"));
		setText(" ");
	}

	void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		txtCol.r = r;
		txtCol.g = g;
		txtCol.b = b;
		txtCol.a = a;
	}

	void setText(const char * txt) {
		strncpy(text, txt, sizeof(text));
		box.size.x = strlen(text) * 10;
	}

	void draw(const DrawArgs &args) override {
		TransparentWidget::draw(args);
		drawTxt(args, text);
	}

	void drawTxt(const DrawArgs &args, const char * txt) {
		float bounds[4];
		Vec c = Vec(box.pos.x, box.pos.y);

		nvgFontSize(args.vg, fh);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, -2);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT);

		// CHECK WHETHER TEXT FITS IN THE MODULE
		nvgTextBounds(args.vg, c.x, c.y, txt, NULL, bounds);
		float xmax = bounds[2];
		if (xmax > parentW) {
			float ratio = parentW / xmax;
			fh = (int)floor(ratio * fh); // reduce fontsize to fit the parent width
		} else {
			c.x += (parentW - xmax)/2; // center text
		}

		nvgFillColor(args.vg, nvgRGBA(txtCol.r, txtCol.g, txtCol.b, txtCol.a));
		nvgText(args.vg, c.x, c.y+fh, txt, NULL);
	}

};

struct valueKnob : RoundBlackKnob {
	std::shared_ptr<Font> font;
	NVGcolor txtCol;

	valueKnob() {
		setColor(0x00, 0x00, 0x00, 0xFF);
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/DejaVuSansMono.ttf"));
	}

	void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		txtCol.r = r;
		txtCol.g = g;
		txtCol.b = b;
		txtCol.a = a;
	}

	valueKnob(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		setColor(r, g, b, a);
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/DejaVuSansMono.ttf"));
	}

	void draw(const DrawArgs &args) override {
		char tbuf[128];

		ParamWidget::draw(args);

		engine::ParamQuantity* pq ;

		if (pq) {
			float value = pq->getSmoothValue();
			snprintf(tbuf, sizeof(tbuf), "%.3G", value);
			drawValue(args, tbuf);
		}

	}

	void drawValue(const DrawArgs &args, const char * txt) {

		Vec c = Vec(box.size.x/2, box.size.y);
		const int fh = 14;
		const int whalf = 15;

		// Draw rounded rectangle
		nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xF0));
		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, c.x -whalf, c.y +2);
			nvgLineTo(args.vg, c.x +whalf, c.y +2);
			nvgQuadTo(args.vg, c.x +whalf +5, c.y +2+7, c.x +whalf, c.y+fh+2);
			nvgLineTo(args.vg, c.x -whalf, c.y+fh+2);
			nvgQuadTo(args.vg, c.x -whalf -5, c.y +2+7, c.x -whalf, c.y +2);
			nvgClosePath(args.vg);
		}
		nvgFill(args.vg);
		nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0x0F));
		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, c.x -whalf, c.y +2);
			nvgLineTo(args.vg, c.x +whalf, c.y +2);
			nvgQuadTo(args.vg, c.x +whalf +5, c.y +2+7, c.x +whalf, c.y+fh+2);
			nvgLineTo(args.vg, c.x -whalf, c.y+fh+2);
			nvgQuadTo(args.vg, c.x -whalf -5, c.y +2+7, c.x -whalf, c.y +2);
			nvgClosePath(args.vg);
		}
		nvgStrokeWidth(args.vg, 0.5);
		nvgStroke(args.vg);
		nvgFontSize(args.vg, fh);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, -2);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
		nvgFillColor(args.vg, nvgRGBA(txtCol.r, txtCol.g, txtCol.b, txtCol.a));
		nvgText(args.vg, c.x, c.y+fh, txt, NULL);
	}
};
