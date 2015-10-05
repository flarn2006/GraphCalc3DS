#include <3ds.h>
#include <sf2d.h>
#include <vector>
#include <cmath>
#include "ViewWindow.h"
#include "RpnInstruction.h"
#include "TableLayout.h"
#include "ControlGrid.h"
#include "Button.h"

std::vector<RpnInstruction> equation;
float exprX;

void drawGraph(const ViewWindow &view, u32 color)
{
	Point<int> lastPoint;
	
	for (int x=0; x<400; x+=2) {
		Point<int> pt;
		exprX = Interpolate((float)x, 0.0f, 399.0f, view.xmin, view.xmax);
		float y; ExecuteRpn(equation, y);
		pt = view.GetScreenCoords(exprX, y);
		
		if (x > 0) {
			sf2d_draw_line(lastPoint.x, lastPoint.y, pt.x, pt.y, color);
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
	
	ControlGrid<4, 4> cgrid(80, 60);
	for (int r=0; r<4; r++) {
		for (int c=0; c<4; c++) {
			cgrid.cells[r][c].content = new Button();
		}
	}
	
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0xE0, 0xE0, 0xE0, 0xFF));
	
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
	
	sf2d_fini();
	
	return 0;
}