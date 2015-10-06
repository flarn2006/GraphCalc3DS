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

std::vector<RpnInstruction> equation;
float exprX;
sftd_font *font;
TextDisplay *equDisp;

typedef ControlGrid<5, 7> cgrid_t;
void SetUpControlGrid(cgrid_t &cgrid);

void drawGraph(const ViewWindow &view, u32 color)
{
	Point<int> lastPoint;
	bool ignoreLastPoint = true;
	
	for (int x=0; x<400; x+=2) {
		Point<int> pt;
		exprX = Interpolate((float)x, 0.0f, 399.0f, view.xmin, view.xmax);
		float y;
		RpnInstruction::Status status = ExecuteRpn(equation, y);
		pt = view.GetScreenCoords(exprX, y);
		
		if (status == RpnInstruction::S_OK && !ignoreLastPoint) {
			sf2d_draw_line(lastPoint.x, lastPoint.y, pt.x, pt.y, color);
		} else if (status == RpnInstruction::S_OVERFLOW) {
			sftd_draw_text(font, 4, 4, color, 20, "Error: Stack overflow");
			break;
		} else if (status == RpnInstruction::S_UNDERFLOW) {
			sftd_draw_text(font, 4, 4, color, 20, "Error: Stack underflow");
			break;
		} else {
			ignoreLastPoint = (status == RpnInstruction::S_UNDEFINED);
		}
		
		lastPoint = pt;
	}
}

int main(int argc, char *argv[])
{
	ViewWindow view(-5.0f, 5.0f, -3.0f, 3.0f);
	
	equation.push_back(1.25f);
	equation.push_back(RpnInstruction(&exprX, "x"));
	equation.push_back(RpnInstruction::OP_MULTIPLY);
	equation.push_back(RpnInstruction(&exprX, "x"));
	equation.push_back(RpnInstruction(std::cos, "cos"));
	equation.push_back(RpnInstruction::OP_MULTIPLY);
	
	cgrid_t cgrid(45, 48);
	cgrid.SetDrawOffset(2, 0);
	SetUpControlGrid(cgrid);
	
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0xE0, 0xE0, 0xE0, 0xFF));
	
	romfsInit();
	sftd_init();
	font = sftd_load_font_file("font.ttf");
	
	while (aptMainLoop()) {
		hidScanInput();
		int keys = hidKeysHeld();
		
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
		
		circlePosition circle;
		hidCircleRead(&circle);
		
		if (circle.dx * circle.dx + circle.dy * circle.dy > 15*15) {
			view.Pan(0.002f * circle.dx, 0.002f * circle.dy);
		}
		
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawGraph(view, RGBA8(0x00, 0x00, 0xC0, 0xFF));
		sf2d_end_frame();
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		cgrid.ScreenTouchStatus(keys & KEY_TOUCH, touch.px, touch.py);
		cgrid.Draw();
		sf2d_end_frame();
		
		sf2d_swapbuffers();
	}
	
	for (int r=0; r<5; r++) {
		for (int c=0; c<7; c++) {
			Control *ctl = cgrid.cells[r][c].content;
			if (ctl != nullptr) {
				delete ctl;
			}
		}
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
	for (auto i = equation.begin(); i != equation.end(); ++i) {
		ss << *i << ' ';
	}
	equDisp->SetText(ss.str());
}

void SetUpControlGrid(cgrid_t &cgrid)
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
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_MULTIPLY, RpnInstruction(std::sqrt, "sqrt"), RpnInstruction(std::exp, "exp"), RpnInstruction(std::log, "ln") },
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_SUBTRACT, RpnInstruction(std::sin, "sin"), RpnInstruction(std::cos, "cos"), RpnInstruction(std::tan, "tan") },
		{RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_NULL, RpnInstruction::OP_ADD, RpnInstruction(&exprX, "x"), RpnInstruction::OP_NULL, RpnInstruction::OP_NULL }
	};
	
	for (int r=0; r<5; r++) {
		for (int c=0; c<7; c++) {
			if (btnText[r][c] != nullptr) {
				Button *btn = new Button(btnText[r][c], btnColors[r][c]);
				RpnInstruction::Opcode opcode = btnInstructions[r][c].GetOpcode();
				
				if (r == 0 && c == 6) {
					btn->SetAction([](Button&) {
						if (equation.size() > 0) {
							equation.pop_back();
							UpdateEquationDisplay();
						}
					});
				} else if (r == 4 && c == 5) {
					btn->SetAction([](Button&) {
						equation.clear();
						UpdateEquationDisplay();
					});
				} else if (opcode != RpnInstruction::OP_NULL) {
					btn->SetAction([opcode](Button&) {
						equation.push_back(opcode);
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