#include <3ds.h>
#include <sf2d.h>
#include <vector>
#include <cmath>
#include <sstream>
#include "ViewWindow.h"
#include "BmpFont.h"
#include "RpnInstruction.h"
#include "TableLayout.h"
#include "ControlGrid.h"
#include "Button.h"
#include "TextDisplay.h"
#include "NumpadController.h"
#include "Slider.h"

constexpr int plotCount = 4;

std::vector<RpnInstruction> equations[plotCount];
double exprX;
BmpFont mainFont, btnFont, smallFont;
TextDisplay *equDisp;
Control *btnBackspace;
NumpadController numpad;
std::vector<ControlGridBase*> controlGrids;
int cgridIndex = 0;
int plotIndex = 0;
int keys = 0, down = 0;
bool altMode = false;
ViewWindow view(-5.0, 5.0, -3.0, 3.0);
Slider *sliders[4];
double plotCache[400*plotCount];
bool plotNeedsUpdate[plotCount];

const int plotColors[] = { RGBA8(0x00, 0x00, 0xC0, 0xFF), RGBA8(0x00, 0x80, 0x00, 0xFF), RGBA8(0xD5, 0x00, 0xDD, 0xFF), RGBA8(0xD2, 0x94, 0x00, 0xFF) };

void UpdateEquationDisplay();
void UpdateEquation(int equNum);
void SetUpMainControlGrid(ControlGrid<5, 7> &cgrid);
void SetUpVarsControlGrid(ControlGrid<5, 7> &cgrid);
SwkbdCallbackResult ValidateSwkbdExpr(void *user, const char **ppMessage, const char *text, size_t textlen);

void drawAxes(const ViewWindow &view, u32 color, double originX = 0.0, double originY = 0.0, bool hideHorizontal = false)
{
	Point<int> center = view.GetScreenCoords(originX, originY);
	if (0 <= center.x && center.x < 400) {
		sf2d_draw_rectangle(center.x, 0, 1, 240, color);
	}
	if (!hideHorizontal && 0 <= center.y && center.y < 240) {
		sf2d_draw_rectangle(0, center.y, 399, 1, color);
	}
}

void drawGraph(int plotNum, ViewWindow &view, u32 color, bool showErrors = true)
{
	Point<int> lastPoint;
	bool ignoreLastPoint = true;
	bool showRPNHint = false;
	const std::vector<RpnInstruction> &equation = equations[plotNum];
	
	// If the plot was offset since the last time it was drawn, offset the cache accordingly.
	int offset = view.GetOffset();
	double *cache = plotCache + 400*plotNum;
	if (!plotNeedsUpdate[plotNum]) {
		if (-400 < offset && offset < 0) {
			// offset is negative here, so '+offset' is really subtraction, and '-offset' is addition;
			for (int i=0; i<400+offset; ++i) {
				cache[i] = cache[i-offset];
			}
		} else if (0 < offset && offset < 400) {
			for (int i=offset; i<400; ++i) {
				cache[i] = cache[i-offset];
			}
		}
	}

	for (int x=0; x<400; x++) {
		Point<int> pt;
		exprX = Interpolate((double)x, 0.0, 399.0, view.xmin, view.xmax);
		double y;
		RpnInstruction::Status status;

		if (plotNeedsUpdate[plotNum] || (offset < 0 && x >= 400+offset) || (offset > 0 && x < offset)) {
			status = ExecuteRpn(equation, y);
			cache[x] = y;
		} else {
			y = cache[x];
			status = std::isfinite(y) ? RpnInstruction::S_OK : RpnInstruction::S_UNDEFINED;
		}

		pt = view.GetScreenCoords(exprX, y);
		
		if (status == RpnInstruction::S_OK && !ignoreLastPoint) {
			sf2d_draw_line(lastPoint.x, lastPoint.y, pt.x, pt.y, 2.0, color);
		} else if (status == RpnInstruction::S_OVERFLOW) {
			if (showErrors) {
                mainFont.drawStr("Error: Stack overflow", 4, 4, color);
				showRPNHint = true;
			}
			break;
		} else if (status == RpnInstruction::S_UNDERFLOW) {
			if (showErrors) {
                mainFont.drawStr("Error: Stack underflow", 4, 4, color);
				showRPNHint = true;
			}
			break;
		} else {
			ignoreLastPoint = (status == RpnInstruction::S_UNDEFINED);
		}

		lastPoint = pt;
	}
	
	view.ResetOffset();
	plotNeedsUpdate[plotNum] = false;

	if (showRPNHint) {
		const char *text;
		if (altMode)
			text = "(Press Select, A if you don't know RPN.)";
		else
			text = "(Press A if you don't know RPN.)";
		smallFont.drawStr(text, 4, 23, color);
	}
}

void moveCursor(double &cursorX, double &cursorY, double dx, double dy)
{
	cursorX += dx;
	if (cursorX < 0.0)
		cursorX = 0.0;
	else if (cursorX > 399.0)
		cursorX = 399.0;
	
	cursorY += dy;
	if (cursorY < 0.0)
		cursorY = 0.0;
	else if (cursorY > 239.0)
		cursorY = 239.0;
}

void addInstruction(const RpnInstruction &inst)
{
	if (numpad.EntryInProgress()) {
		numpad.Reset();
	}
	equations[plotIndex].push_back(inst);
	UpdateEquation(plotIndex);
}

int main(int argc, char *argv[])
{
	double cursorX = 200.0, cursorY = 120.0;
	double traceUnit = 0;
	bool traceUndefined = false;

	for (int i=0; i<plotCount; ++i) {
		double *cache = plotCache + 400*i;
		for (int j=0; j<400; ++j) {
			cache[j] = 0.0;
		}
		plotNeedsUpdate[i] = true;
	}
	
	equations[0].push_back(RpnInstruction(&exprX, "x"));
	equations[0].push_back(RpnInstruction(std::sin, "sin"));
	
	ControlGrid<5, 7> cgridMain(45, 48);
	cgridMain.SetDrawOffset(2, 0);
	SetUpMainControlGrid(cgridMain);
	controlGrids.push_back(&cgridMain);
	
	ControlGrid<5, 7> cgridVars(45, 48);
	cgridVars.SetDrawOffset(2, 0);
	SetUpVarsControlGrid(cgridVars);
	controlGrids.push_back(&cgridVars);
	
	irrstInit();

	sf2d_init();
	sf2d_set_clear_color(RGBA8(0xE0, 0xE0, 0xE0, 0xFF));
	
	romfsInit();
	mainFont.load("romfs:/mainfont.bff");
    btnFont.load("romfs:/buttons.bff");
	smallFont.load("romfs:/small.bff");
	
	while (aptMainLoop()) {
		hidScanInput();
		keys = hidKeysHeld();
		down = hidKeysDown();
		
		touchPosition touch;
		if (keys & KEY_TOUCH) {
			hidTouchRead(&touch);
		}
		
		if (keys & KEY_START) {
			break;
		}

		if (keys & KEY_A) {
			SwkbdState swkbd;
			swkbdInit(&swkbd, SWKBD_TYPE_QWERTY, 2, 512);
			swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_FILTER_CALLBACK, 0);
			swkbdSetFilterCallback(&swkbd, ValidateSwkbdExpr, nullptr);

			if (altMode) {
				swkbdSetHintText(&swkbd, "Enter a function of 'x'\n(ALT active; will append to RPN.)");
			} else {
				swkbdSetHintText(&swkbd, "Enter a function of 'x'\nexample: sin(x)+cos(2*x)");
			}

			if (!equations[plotIndex].empty()) {
				const RpnInstruction &inst = equations[plotIndex].back();
				if (!altMode && inst.GetOpcode() == RpnInstruction::OP_EXPR) {
					swkbdSetInitialText(&swkbd, inst.GetName().c_str());
				}
			}

			char buf[512];
			if (swkbdInputText(&swkbd, buf, 512) == SWKBD_BUTTON_RIGHT) {
				if (!altMode) {
					equations[plotIndex].clear();
					equations[plotIndex].emplace_back(&exprX, "y =");
				} else {
					altMode = false;
				}
				equations[plotIndex].emplace_back(buf);
				UpdateEquation(plotIndex);
			}
		}
		
		if ((keys & KEY_L) && !(keys & KEY_TOUCH)) {
			view.ZoomOut(1.02);
		}
		
		if ((keys & KEY_R) && !(keys & KEY_TOUCH)) {
			view.ZoomIn(1.02);
		}
		
		if ((down & KEY_SELECT) && !(keys & KEY_TOUCH)) {
			altMode = !altMode;
		}
		
		if (down & (KEY_DUP | KEY_DDOWN)) {
			if (keys & KEY_X) {
				if (down & KEY_DUP) moveCursor(cursorX, cursorY, 0.0, -1.0);
				if (down & KEY_DDOWN) moveCursor(cursorX, cursorY, 0.0, 1.0);
			} else {
				numpad.Reset();
				
				if (down & KEY_DUP) --plotIndex;
				if (down & KEY_DDOWN) ++plotIndex;
				
				if (plotIndex < 0)
					plotIndex = plotCount - 1;
				else if (plotIndex >= plotCount)
					plotIndex = 0;
				
				UpdateEquationDisplay();
			}
		}
		
		if (down & (KEY_DLEFT | KEY_DRIGHT)) {
			if (keys & (KEY_X | KEY_Y)) {
				if (down & KEY_DLEFT) moveCursor(cursorX, cursorY, -1.0, 0.0);
				if (down & KEY_DRIGHT) moveCursor(cursorX, cursorY, 1.0, 0.0);
			} else if (!(keys & KEY_TOUCH)) {
				if (down & KEY_DLEFT) --cgridIndex;
				if (down & KEY_DRIGHT) ++cgridIndex;
			
				int count = controlGrids.size();
				if (cgridIndex < 0)
					cgridIndex = count - 1;
				else if (cgridIndex >= count) {
					cgridIndex = 0;
				}
			}
		}
		
		circlePosition circle;
		hidCircleRead(&circle);
		
		if (circle.dx * circle.dx + circle.dy * circle.dy > 20*20) {
			if (keys & (KEY_X | KEY_Y)) {
				moveCursor(cursorX, cursorY, 0.05 * circle.dx, -0.05 * circle.dy);
			} else {
				double rangeX = view.xmax - view.xmin;
				double rangeY = view.ymax - view.ymin;
				view.Pan(0.0002 * rangeX * circle.dx, 0.0002 * rangeY * circle.dy);
			}
		}

		circlePosition cstick;
		static bool ignoreCstickIfZero = true;
		irrstCstickRead(&cstick);
		bool cstickZero = (cstick.dx == 0 && cstick.dy == 0);
		if (!cstickZero || !ignoreCstickIfZero) {
			sliders[0]->value = Interpolate(cstick.dx, -146, 146, sliders[0]->GetMinimum(), sliders[0]->GetMaximum());
			sliders[1]->value = Interpolate(cstick.dy, -146, 146, sliders[1]->GetMinimum(), sliders[1]->GetMaximum());
			ignoreCstickIfZero = cstickZero;
		}
		
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0xFF, 0xFF, 0xFF, 0xFF));
		drawAxes(view, RGBA8(0x80, 0xFF, 0xFF, 0xFF));
		
		for (int i=0; i<plotCount; i++) {
			drawGraph(i, view, plotColors[i], i == plotIndex);
		}
		
		if (keys & (KEY_X | KEY_Y)) {
			Point<double> cursor = view.GetGraphCoords(cursorX, cursorY);
			if (keys & KEY_Y) {
				if (keys & KEY_B) {
					traceUnit = std::pow(10.0, std::ceil(std::log10((view.xmax - view.xmin) / 400)));
					cursor.x = std::round(cursor.x / traceUnit) * traceUnit;
				}
				exprX = cursor.x;
				RpnInstruction::Status status = ExecuteRpn(equations[plotIndex], cursor.y);
				traceUndefined = (status != RpnInstruction::S_OK);
			} else {
				traceUndefined = false;
			}
			u32 color = (keys & KEY_Y) ? RGBA8(0xFF, 0x00, 0x00, 0xFF) : RGBA8(0x00, 0xC0, 0x00, 0xFF);
			drawAxes(view, color, cursor.x, cursor.y, traceUndefined);
            mainFont.drawStr(ssprintf("X = %.5f", cursor.x), 2, 0, color);
			if (!traceUndefined)
                mainFont.drawStr(ssprintf("Y = %.5f", cursor.y), 2, 22, color);
		}
        if (altMode) btnFont.align(ALIGN_LEFT).drawStr("ALT", 2, 225, RGBA8(0x48, 0x67, 0x4E, 0xFF));
		sf2d_end_frame();
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		controlGrids[cgridIndex]->ScreenTouchStatus(keys & KEY_TOUCH, touch.px, touch.py);
		controlGrids[cgridIndex]->Draw();
		sf2d_end_frame();
		
		sf2d_swapbuffers();
	}
	
	romfsExit();
	sf2d_fini();
	irrstExit();
	
	return 0;
}

void UpdateEquationDisplay()
{
	std::ostringstream ss;
	auto count = equations[plotIndex].size();
	if (numpad.EntryInProgress()) --count;
	
	for (decltype(count) i=0; i<count; i++) {
		ss << equations[plotIndex][i] << ' ';
	}
	
	if (numpad.EntryInProgress()) {
		std::string entry;
		numpad.GetEntryString(entry);
		ss << entry;
	}
	
	equDisp->SetText(ss.str());
	equDisp->SetTextColor(plotColors[plotIndex]);
}

void UpdateEquation(int equNum)
{
	plotNeedsUpdate[equNum] = true;
	if (equNum == plotIndex)
		UpdateEquationDisplay();
}

SwkbdCallbackResult ValidateSwkbdExpr(void *user, const char **ppMessage, const char *text, size_t textlen)
{
	if (TinyExpr::TestExpr(text) == 0) {
		return SWKBD_CALLBACK_OK;
	} else {
		*ppMessage = "Invalid expression entered.";
		return SWKBD_CALLBACK_CONTINUE;
	}
}

void SetUpMainControlGrid(ControlGrid<5, 7> &cgrid)
{
	equDisp = new TextDisplay();
	cgrid.cells[0][0].content = equDisp;
	UpdateEquationDisplay();
	
	const char *btnText[5][7] = {
		{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "\xAB"},
		{"7",     "8",     "9",     "\xF7",  "^",     "mod",   "abs"},
		{"4",     "5",     "6",     "*",  "sqrt",  "exp",   "ln"},
		{"1",     "2",     "3",     "-",     "sin",   "cos",   "tan"},
		{"0",     ".",     "+/-",   "+",     "x",     "clear", nullptr}
	};
	
	const char *btnTextAlt[5][7] = {
		{nullptr,     nullptr,     nullptr,     nullptr,     nullptr,     nullptr,     nullptr},
		{nullptr,     nullptr,     nullptr,     nullptr,     nullptr,     nullptr,     nullptr},
		{nullptr,     nullptr,     nullptr,     nullptr,     nullptr,     nullptr,     "log"},
		{nullptr,     nullptr,     nullptr,     nullptr,     "asin",      "acos",      "atan"},
		{nullptr,     nullptr,     nullptr,     nullptr,     "dup",       "def. view", nullptr}
	};
	
	constexpr Button::ColorPreset C_BLUE = Button::ColorPreset::C_BLUE;
	constexpr Button::ColorPreset C_GREEN = Button::ColorPreset::C_GREEN;
	constexpr Button::ColorPreset C_PINK = Button::ColorPreset::C_PINK;
	constexpr Button::ColorPreset C_ORANGE = Button::ColorPreset::C_ORANGE;
	
	const Button::ColorPreset btnColors[5][7] = {
		{C_BLUE,   C_BLUE,   C_BLUE,   C_BLUE,   C_BLUE,   C_BLUE,   C_ORANGE},
		{C_BLUE,   C_BLUE,   C_BLUE,   C_GREEN,  C_GREEN,  C_GREEN,  C_PINK},
		{C_BLUE,   C_BLUE,   C_BLUE,   C_GREEN,  C_PINK,   C_PINK,   C_PINK},
		{C_BLUE,   C_BLUE,   C_BLUE,   C_GREEN,  C_PINK,   C_PINK,   C_PINK},
		{C_BLUE,   C_BLUE,   C_BLUE,   C_GREEN,  C_BLUE,   C_ORANGE, C_ORANGE}
	};
	
	const RpnInstruction btnInstructions[5][7] = {
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL },
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_DIVIDE, RpnInstruction::OP_POWER, RpnInstruction::OP_MODULO, RpnInstruction(std::abs, "abs") },
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_MULTIPLY, RpnInstruction(std::sqrt, "sqrt", ~RpnInstruction::D_NEGATIVE), RpnInstruction(std::exp, "exp"), RpnInstruction(std::log, "ln", RpnInstruction::D_POSITIVE) },
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_SUBTRACT, RpnInstruction(std::sin, "sin"), RpnInstruction(std::cos, "cos"), RpnInstruction(std::tan, "tan") },
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_ADD, RpnInstruction(&exprX, "x"), RpnInstruction::OP_NULL, RpnInstruction::OP_NULL }
	};
	
	const RpnInstruction btnInstructionsAlt[5][7] = {
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL },
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL },
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction(std::log10, "log", RpnInstruction::D_POSITIVE) },
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction(std::asin, "asin"), RpnInstruction(std::acos, "acos"), RpnInstruction(std::atan, "atan") },
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_DUP, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL }
	};
	
	const char *numpadKeys[] = { "789", "456", "123", "0.-" };
	
	for (int r=0; r<5; r++) {
		for (int c=0; c<7; c++) {
			if (btnText[r][c] != nullptr) {
				Button *btn = new Button(btnText[r][c], btnColors[r][c]);
				if (btnTextAlt[r][c] != nullptr) btn->SetText(btnTextAlt[r][c], true);
				RpnInstruction::Opcode opcode = btnInstructions[r][c].GetOpcode();
				
				if (r == 4 && c == 5) {
					//clear button
					btn->SetAction([](Button&) {
						if (altMode) {
							view = ViewWindow(-5.0, 5.0, -3.0, 3.0);
						} else {
							equations[plotIndex].clear();
							numpad.Reset();
							UpdateEquation(plotIndex);
						}
					});
				} else if ((r > 0 && c < 3) || (r == 0 && c == 6)) {
					//one of the numpad keys, or backspace
					char key;
					if (r == 0) { //&& c == 6, always the case if r == 0
						key = '\b';
					} else {
						key = numpadKeys[r-1][c];
					}
					btn->SetAction([key](Button&) {
						if (key == '\b' && !numpad.EntryInProgress()) {
							if (equations[plotIndex].size() > 0) {
								equations[plotIndex].pop_back();
							}
						} else {
							const RpnInstruction *lastInst = nullptr;
							if (equations[plotIndex].size() > 0) {
								lastInst = &equations[plotIndex].back();
							}
							NumpadController::Reply reply = numpad.KeyPressed(key, lastInst);
							if (reply.replaceLast) {
								equations[plotIndex].back() = reply.inst;
							} else {
								equations[plotIndex].push_back(reply.inst);
							}
						}
						UpdateEquation(plotIndex);
					});
				} else if (opcode != RpnInstruction::OP_NULL) {
					//one of the buttons that adds an RPN instruction
					RpnInstruction inst = btnInstructions[r][c];
					RpnInstruction instAlt = btnInstructionsAlt[r][c];
					if (instAlt.GetOpcode() == RpnInstruction::OP_NULL) instAlt = inst;
					btn->SetAction([inst, instAlt](Button&) {
						addInstruction(altMode ? instAlt : inst);
					});
				}
				
				cgrid.cells[r][c].content = btn;
			}
		}
	}
	
	btnBackspace = cgrid.cells[0][6].content;
	
	cgrid.cells[0][0].colSpan = 6;
	cgrid.cells[4][5].colSpan = 2;
}

void SetUpVarsControlGrid(ControlGrid<5, 7> &cgrid)
{
	cgrid.cells[0][0].content = equDisp;
	cgrid.cells[0][0].colSpan = 6;
	cgrid.cells[0][6].content = btnBackspace;
	
	for (int i=0; i<4; i++) {
		Slider *slider = new Slider();
		sliders[i] = slider;
		slider->value = 0.5;
		cgrid.cells[i+1][1] = slider;
		cgrid.cells[i+1][1].colSpan = 5;
		
		char varName[2] = {(char)('a' + i), '\0'};
		Button *btn = new Button(varName, Button::C_BLUE);
		btn->SetText(">|", true);
		btn->SetAction([slider, varName](Button&) {
			if (altMode) {
				slider->SetMinimum(slider->value);
			} else {
				addInstruction(RpnInstruction(&slider->value, varName));
			}
		});
		cgrid.cells[i+1][0].content = btn;
		
		btn = new Button("0-1");
		btn->SetText("|<", true);
		btn->SetColors(Button::C_ORANGE, Button::C_BLUE);
		btn->SetAction([slider](Button&) {
			if (altMode) {
				slider->SetMaximum(slider->value);
			} else {
				slider->SetRange(0.0, 1.0);
				slider->value = 0.5;
			}
		});
		cgrid.cells[i+1][6].content = btn;
	}

	TinyExpr::SetABCDVars(&sliders[0]->value, &sliders[1]->value, &sliders[2]->value, &sliders[3]->value);
}
