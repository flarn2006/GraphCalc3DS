#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <vector>
#include <cmath>
#include <sstream>
#include "ViewWindow.h"
#include "RpnInstruction.h"
#include "TableLayout.h"
#include "ControlGrid.h"
#include "Button.h"
#include "TextDisplay.h"
#include "NumpadController.h"

constexpr int plotCount = 4;

std::vector<RpnInstruction> equations[plotCount];
float exprX;
sftd_font *font;
TextDisplay *equDisp;
NumpadController numpad;
std::vector<ControlGridBase*> controlGrids;
int cgridIndex = 0;
int plotIndex = 0;

const int plotColors[] = { RGBA8(0x00, 0x00, 0xC0, 0xFF), RGBA8(0x00, 0x80, 0x00, 0xFF), RGBA8(0xD5, 0x00, 0xDD, 0xFF), RGBA8(0xD2, 0x94, 0x00, 0xFF) };

void UpdateEquationDisplay();
void SetUpMainControlGrid(ControlGrid<5, 7> &cgrid);

void drawAxes(const ViewWindow &view, u32 color, float originX = 0.0f, float originY = 0.0f, bool hideHorizontal = false)
{
	Point<int> center = view.GetScreenCoords(originX, originY);
	if (0 <= center.x && center.x < 400) {
		sf2d_draw_rectangle(center.x, 0, 1, 240, color);
	}
	if (!hideHorizontal && 0 <= center.y && center.y < 240) {
		sf2d_draw_rectangle(0, center.y, 399, 1, color);
	}
}

void drawGraph(const std::vector<RpnInstruction> &equation, const ViewWindow &view, u32 color, bool showErrors = true)
{
	Point<int> lastPoint;
	bool ignoreLastPoint = true;
	
	for (int x=0; x<400; x++) {
		Point<int> pt;
		exprX = Interpolate((float)x, 0.0f, 399.0f, view.xmin, view.xmax);
		float y;
		RpnInstruction::Status status = ExecuteRpn(equation, y);
		pt = view.GetScreenCoords(exprX, y);
		
		if (status == RpnInstruction::S_OK && !ignoreLastPoint) {
			sf2d_draw_line(lastPoint.x, lastPoint.y, pt.x, pt.y, color);
		} else if (status == RpnInstruction::S_OVERFLOW) {
			if (showErrors) {
				sftd_draw_text(font, 4, 4, color, 20, "Error: Stack overflow");
			}
			break;
		} else if (status == RpnInstruction::S_UNDERFLOW) {
			if (showErrors) {
				sftd_draw_text(font, 4, 4, color, 20, "Error: Stack underflow");
			}
			break;
		} else {
			ignoreLastPoint = (status == RpnInstruction::S_UNDEFINED);
		}
		
		lastPoint = pt;
	}
}

int main(int argc, char *argv[])
{
	float cursorX = 200.0f, cursorY = 120.0f;
	bool traceUndefined = false;
	
	ViewWindow view(-5.0f, 5.0f, -3.0f, 3.0f);
	
	equations[0].push_back(1.25f);
	equations[0].push_back(RpnInstruction(&exprX, "x"));
	equations[0].push_back(RpnInstruction::OP_MULTIPLY);
	equations[0].push_back(RpnInstruction(&exprX, "x"));
	equations[0].push_back(RpnInstruction(std::cos, "cos"));
	equations[0].push_back(RpnInstruction::OP_MULTIPLY);
	
	ControlGrid<5, 7> cgridMain(45, 48);
	cgridMain.SetDrawOffset(2, 0);
	SetUpMainControlGrid(cgridMain);
	controlGrids.push_back(&cgridMain);
	
	ControlGrid<2, 2> cgridTest(160, 120);
	cgridTest.cells[0][0].content = new Button("Test", Button::C_BLUE);
	cgridTest.cells[0][1].content = new Button("Screen", Button::C_GREEN);
	cgridTest.cells[1][0].content = new Button("Please", Button::C_PINK);
	cgridTest.cells[1][1].content = new Button("Ignore", Button::C_ORANGE);
	controlGrids.push_back(&cgridTest);
	
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0xE0, 0xE0, 0xE0, 0xFF));
	
	romfsInit();
	sftd_init();
	font = sftd_load_font_file("font.ttf");
	
	while (aptMainLoop()) {
		hidScanInput();
		int keys = hidKeysHeld();
		int down = hidKeysDown();
		
		touchPosition touch;
		if (keys & KEY_TOUCH) {
			hidTouchRead(&touch);
		}
		
		if (keys & KEY_START) {
			break;
		}
		
		if (keys & KEY_L) {
			view.ZoomOut(1.02f);
		}
		
		if (keys & KEY_R) {
			view.ZoomIn(1.02f);
		}
		
		if (keys & KEY_SELECT) {
			view = ViewWindow(-5.0f, 5.0f, -3.0f, 3.0f);
		}
		
		if (down & (KEY_DUP | KEY_DDOWN)) {
			if (down & KEY_DUP) --plotIndex;
			if (down & KEY_DDOWN) ++plotIndex;
			
			if (plotIndex < 0)
				plotIndex = plotCount - 1;
			else if (plotIndex >= plotCount)
				plotIndex = 0;
			
			UpdateEquationDisplay();
		}
		
		if (down & (KEY_DLEFT | KEY_DRIGHT) && !(keys & KEY_TOUCH)) {
			if (down & KEY_DLEFT) --cgridIndex;
			if (down & KEY_DRIGHT) ++cgridIndex;
			
			int count = controlGrids.size();
			if (cgridIndex < 0)
				cgridIndex = count - 1;
			else if (cgridIndex >= count) {
				cgridIndex = 0;
			}
		}
		
		circlePosition circle;
		hidCircleRead(&circle);
		
		if (circle.dx * circle.dx + circle.dy * circle.dy > 15*15) {
			if (keys & (KEY_X | KEY_Y)) {
				cursorX += 0.05f * circle.dx;
				cursorY -= 0.05f * circle.dy;
				if (cursorX < 0.0f)
					cursorX = 0.0f;
				else if (cursorX > 399.0f)
					cursorX = 399.0f;
				if (cursorY < 0.0f)
					cursorY = 0.0f;
				else if (cursorY > 239.0f) {
					cursorY = 239.0f;
				}
			} else {
				float rangeX = view.xmax - view.xmin;
				float rangeY = view.ymax - view.ymin;
				view.Pan(0.0002f * rangeX * circle.dx, 0.0002f * rangeY * circle.dy);
			}
		}
		
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0xFF, 0xFF, 0xFF, 0xFF));
		drawAxes(view, RGBA8(0x80, 0xFF, 0xFF, 0xFF));
		
		for (int i=0; i<plotCount; i++) {
			drawGraph(equations[i], view, plotColors[i], i == plotIndex);
		}
		
		if (keys & (KEY_X | KEY_Y)) {
			Point<float> cursor = view.GetGraphCoords(cursorX, cursorY);
			if (keys & KEY_Y) {
				exprX = cursor.x;
				RpnInstruction::Status status = ExecuteRpn(equations[plotIndex], cursor.y);
				traceUndefined = (status != RpnInstruction::S_OK);
			} else {
				traceUndefined = false;
			}
			u32 color = (keys & KEY_Y) ? RGBA8(0xFF, 0x00, 0x00, 0xFF) : RGBA8(0x00, 0xC0, 0x00, 0xFF);
			drawAxes(view, color, cursor.x, cursor.y, traceUndefined);
			sftd_draw_textf(font, 2, 0, color, 18, "X = %.5f", cursor.x);
			if (!traceUndefined)
				sftd_draw_textf(font, 2, 22, color, 18, "Y = %.5f", cursor.y);
		}
		
		sf2d_end_frame();
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		controlGrids[cgridIndex]->ScreenTouchStatus(keys & KEY_TOUCH, touch.px, touch.py);
		controlGrids[cgridIndex]->Draw();
		sf2d_end_frame();
		
		sf2d_swapbuffers();
	}
	
	romfsExit();
	sftd_free_font(font);
	sftd_fini();
	sf2d_fini();
	
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

void SetUpMainControlGrid(ControlGrid<5, 7> &cgrid)
{
	equDisp = new TextDisplay();
	cgrid.cells[0][0].content = equDisp;
	UpdateEquationDisplay();
	
	const char *btnText[5][7] = {
		{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "\xAB"},
		{"7",     "8",     "9",     "\xF7",  "^",     "mod",   "abs"},
		{"4",     "5",     "6",     "\xD7",  "sqrt",  "exp",   "ln"},
		{"1",     "2",     "3",     "-",     "sin",   "cos",   "tan"},
		{"0",     ".",     "+/-",   "+",     "x",     "clear", nullptr}
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
	
	const char *numpadKeys[] = { "789", "456", "123", "0.-" };
	
	for (int r=0; r<5; r++) {
		for (int c=0; c<7; c++) {
			if (btnText[r][c] != nullptr) {
				Button *btn = new Button(btnText[r][c], btnColors[r][c]);
				RpnInstruction::Opcode opcode = btnInstructions[r][c].GetOpcode();
				
				if (r == 4 && c == 5) {
					btn->SetAction([](Button&) {
						equations[plotIndex].clear();
						numpad.Reset();
						UpdateEquationDisplay();
					});
				} else if ((r > 0 && c < 3) || (r == 0 && c == 6)) {
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
						UpdateEquationDisplay();
					});
				} else if (opcode != RpnInstruction::OP_NULL) {
					RpnInstruction inst = btnInstructions[r][c];
					btn->SetAction([inst](Button&) {
						if (numpad.EntryInProgress()) {
							numpad.Reset();
						}
						equations[plotIndex].push_back(inst);
						UpdateEquationDisplay();
					});
				}
				
				cgrid.cells[r][c].content = btn;
			}
		}
	}
	
	cgrid.cells[0][0].colSpan = 6;
	cgrid.cells[4][5].colSpan = 2;
}