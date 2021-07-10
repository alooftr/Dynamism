#pragma once
#include "includes.hpp"

namespace render2 {
	enum e_textflags {
		none = 0,
		outline = 1 << 0,
		centered_x = 1 << 1,
		centered_y = 1 << 2,
	};
}

class ImGuiRendering {
public:

	void BeginScene();
	void EndScene();

	void arc(float x, float y, float radius, float min_angle, float max_angle, Color col, float thickness);
	void DrawEspBox(Vector leftUpCorn, Vector rightDownCorn, Color clr, float width);
	void DrawLine(float x1, float y1, float x2, float y2, Color clr, float thickness = 1.f);
	void DrawLineGradient(float x1, float y1, float x2, float y2, Color clr1, Color cl2, float thickness = 1.f);
	void Rect(float x, float y, float w, float h, Color clr, float rounding = 0.f);
	void FilledRect(float x, float y, float w, float h, Color clr, float rounding = 0.f);
	void FilledRect2(float x, float y, float w, float h, Color clr, float rounding = 0.f);
	void Triangle(float x1, float y1, float x2, float y2, float x3, float y3, Color clr, float thickness = 1.f);
	void TriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3, Color clr);
	void QuadFilled(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, Color clr);

	void CircleFilled(float x1, float y1, float radius, Color col, int segments);
	void DrawWave(Vector loc, float radius, Color clr, float thickness = 1.f);
	void DrawString(float x, float y, Color color, int flags, ImFont* font, const char* message, ...);
	void DrawCircle(float x, float y, float r, float s, Color color);
	void DrawGradientCircle(float x, float y, float rad, float rotate, int type, int resolution, DWORD color, DWORD color2);
	void DrawString2(float x, float y, Color color, int flags, ImFont* font, const std::string message, ...);
	void DrawRing3D(int16_t x, int16_t y, int16_t z, int16_t radius, uint16_t points, Color color1, Color color2, float thickness);


};

inline ImGuiRendering* g_Render;