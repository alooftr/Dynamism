#include "Render.h"
#define PI			3.14159265358979323846

#include "cheats/visuals/player_esp.h"
#include "cheats/visuals/GrenadePrediction.h"
#include "cheats/visuals/other_esp.h"
#include "cheats/visuals/world_esp.h"
#include "cheats/visuals/GHelper.h"
#include "cheats/misc/logs.h"
#include "cheats/movement/movements.h"
#include "cheats/movement/movementrecorder.h"
#include "cheats/legitbot/legitbot.h"
#include "cheats/ragebot/aim.h"
void ImGuiRendering::BeginScene()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
	ImGui::Begin("##Backbuffer", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);

	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);


	ImGuiWindow* window = ImGui::GetCurrentWindow();


	static int w, h;
	m_engine()->GetScreenSize(w, h);
	if (config_system.g_cfg.menu.watermark)
		window->DrawList->AddImage(hooks::WaterMarker, ImVec2(w + 80, h - 890), ImVec2(w - 340, h - 1190));



	static auto alpha = 0;
	auto speed = 800.0f * m_globals()->m_frametime;

	if (m_engine()->IsInGame() && m_engine()->IsConnected())
	{
		auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

		static float anim1 = 20;

		if (!g_ctx.local()->m_bIsScoped())
			anim1 = 20;

		if (config_system.g_cfg.esp.REMOVALS_SCOPE && g_ctx.globals.scoped && weapon->is_sniper())
		{
			static int w, h;
			m_engine()->GetScreenSize(w, h);
			g_Render->FilledRect(w / 2 - 2, h / 2, 2, 2, Color(200, 50, 80));

			anim1 += m_globals()->m_frametime * 500;

			if (anim1 >= 130)
				anim1 = 130;

			window->DrawList->AddRectFilledMultiColor(ImVec2(w / 2 + 20, h / 2), ImVec2(w / 2 + anim1, h / 2 + 2), ImColor(255, 255, 255, 170), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 170));
			window->DrawList->AddRectFilledMultiColor(ImVec2(w / 2 - 20, h / 2), ImVec2(w / 2 - anim1, h / 2 + 2), ImColor(255, 255, 255, 170), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 170));
			window->DrawList->AddRectFilledMultiColor(ImVec2(w / 2, h / 2 - 20), ImVec2(w / 2 - 2, h / 2 - anim1), ImColor(255, 255, 255, 170), ImColor(255, 255, 255, 170), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 0));
			window->DrawList->AddRectFilledMultiColor(ImVec2(w / 2, h / 2 + 20), ImVec2(w / 2 - 2, h / 2 + anim1), ImColor(255, 255, 255, 170), ImColor(255, 255, 255, 170), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 0));

		}
		if (config_system.g_cfg.misc.trail_types > 1)
			g_Movement.MovementTrails();
		CGrenadePrediction::get().draw();
		otheresp::get().zeuseknife();
		otheresp::get().automatic_peek_indicator();
		g_Movement.Draw();
		legit_bot::get().draw_fov();
		g_MovementRecorder.MovementD();
		worldesp::get().grenade_tracer();
		worldesp::get().paint_traverse();
		playeresp::get().paint_traverse();
		visuals::DrawRing3D();
		visuals::RenderCircle();
		visuals::RenderInfo();
		otheresp::get().hitmarker_paint();

		if (config_system.g_cfg.misc.events_to_log)
			eventlogs::get().paint_traverse();
	}


	if (hooks::menu_open)
	{
		if (alpha + speed <= 120)
			alpha += speed;

		window->DrawList->AddRectFilled(ImVec2(0, 0), ImVec2(w, h), Color(0, 0, 0, alpha).u32(), 0.f);

	}
	else
	{
		if (alpha - speed >= 0)
			alpha -= speed;

		window->DrawList->AddRectFilled(ImVec2(0, 0), ImVec2(w, h), Color(0, 0, 0, alpha).u32(), 0.f);
	}
}

void ImGuiRendering::EndScene()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DrawList->PushClipRectFullScreen();

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

void ImGuiRendering::arc(float x, float y, float radius, float min_angle, float max_angle, Color col, float thickness) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DrawList->PathArcTo(ImVec2(x, y), radius, DEG2RAD(min_angle), DEG2RAD(max_angle), 32);
	window->DrawList->PathStroke(col.u32(), false, thickness);
}

void ImGuiRendering::DrawRing3D(int16_t x, int16_t y, int16_t z, int16_t radius, uint16_t points, Color color1, Color color2, float thickness)
{
	Vector pos = Vector(x, y, z);

	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step)
	{
		Vector start(radius * cosf(a) + pos.x, radius * sinf(a) + pos.y, pos.z);
		Vector end(radius * cosf(a + step) + pos.x, radius * sinf(a + step) + pos.y, pos.z);

		Vector start2d;
		Vector end2d;

		Vector Position;


		if (math::WorldToScreen(start, start2d) &&
			math::WorldToScreen(end, end2d) && math::WorldToScreen(pos, Position))
		{

			DrawLine(start2d[0], start2d[1], end2d[0], end2d[1], color1);
			TriangleFilled(Position.x, Position.y, end2d.x, end2d.y, start2d.x, start2d.y, color2);
		}
	}
}

void ImGuiRendering::DrawCircle(float x, float y, float r, float s, Color color)
{
	float Step = PI * 2.0 / s;
	for (float a = 0; a < (PI * 2.0); a += Step)
	{
		float x1 = r * cos(a) + x;
		float y1 = r * sin(a) + y;
		float x2 = r * cos(a + Step) + x;
		float y2 = r * sin(a + Step) + y;

		DrawLine(x1, y1, x2, y2, color);
	}
}

struct CUSTOMVERTEX {
	FLOAT x, y, z;
	FLOAT rhw;
	DWORD color;
};

void ImGuiRendering::DrawGradientCircle(float x, float y, float rad, float rotate, int type, int resolution, DWORD color, DWORD color2)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2;

	std::vector<CUSTOMVERTEX> circle(resolution + 2);

	float angle = rotate * D3DX_PI / 180, pi = D3DX_PI;

	if (type == 1)
		pi = D3DX_PI; // Full circle
	if (type == 2)
		pi = D3DX_PI / 2; // 1/2 circle
	if (type == 3)
		pi = D3DX_PI / 4; // 1/4 circle

	pi = D3DX_PI / type; // 1/4 circle

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0;
	circle[0].rhw = 1;
	circle[0].color = color2;

	for (int i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - rad * cos(pi * ((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - rad * sin(pi * ((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0;
		circle[i].rhw = 1;
		circle[i].color = color;
	}

	// Rotate matrix
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		circle[i].x = x + cos(angle) * (circle[i].x - x) - sin(angle) * (circle[i].y - y);
		circle[i].y = y + sin(angle) * (circle[i].x - x) + cos(angle) * (circle[i].y - y);
	}

	hooks::device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, NULL);

	VOID* pVertices;
	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX));
	g_pVB2->Unlock();

	hooks::device->SetTexture(0, NULL);
	hooks::device->SetPixelShader(NULL);
	hooks::device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	hooks::device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	hooks::device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	hooks::device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX));
	hooks::device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	hooks::device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
	if (g_pVB2 != NULL)
		g_pVB2->Release();
}


void ImGuiRendering::DrawString(float x, float y, Color color, int flags, ImFont* font, const char* message, ...)
{
	char output[1024] = {};
	va_list args;
	va_start(args, message);
	vsprintf_s(output, message, args);
	va_end(args);

	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->PushTextureID(font->ContainerAtlas->TexID);
	ImGui::PushFont(font);
	auto coord = ImVec2(x, y);
	auto size = ImGui::CalcTextSize(output);
	auto coord_out = ImVec2{ coord.x + 1.f, coord.y + 1.f };
	Color outline_clr = Color(0, 0, 0, color.a());

	int width = 0, height = 0;

	if (!(flags & render2::centered_x))
		size.x = 0;
	if (!(flags & render2::centered_y))
		size.y = 0;

	ImVec2 pos = ImVec2(coord.x - (size.x * .5), coord.y - (size.y * .5));

	if (flags & render2::outline)
	{
		pos.y++;
		window->DrawList->AddText(pos, outline_clr.u32(), output);
		pos.x++;
		window->DrawList->AddText(pos, outline_clr.u32(), output);
		pos.y--;
		window->DrawList->AddText(pos, outline_clr.u32(), output);
		pos.x--;
		window->DrawList->AddText(pos, outline_clr.u32(), output);
	}

	window->DrawList->AddText(pos, color.u32(), output);
	ImGui::PopFont();
}


void ImGuiRendering::DrawString2(float x, float y, Color color, int flags, ImFont* font, const std::string message, ...)
{
	char output[1024] = {};
	va_list args;
	va_start(args, message);
	vsprintf_s(output, message.c_str(), args);
	va_end(args);

	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->PushTextureID(font->ContainerAtlas->TexID);
	ImGui::PushFont(font);
	auto coord = ImVec2(x, y);
	auto size = ImGui::CalcTextSize(output);
	auto coord_out = ImVec2{ coord.x + 1.f, coord.y + 1.f };
	Color outline_clr = Color(0, 0, 0, color.a());

	int width = 0, height = 0;

	if (!(flags & render2::centered_x))
		size.x = 0;
	if (!(flags & render2::centered_y))
		size.y = 0;

	ImVec2 pos = ImVec2(coord.x - (size.x * .5), coord.y - (size.y * .5));

	if (flags & render2::outline)
	{
		pos.y++;
		window->DrawList->AddText(pos, outline_clr.u32(), output);
		pos.x++;
		window->DrawList->AddText(pos, outline_clr.u32(), output);
		pos.y--;
		window->DrawList->AddText(pos, outline_clr.u32(), output);
		pos.x--;
		window->DrawList->AddText(pos, outline_clr.u32(), output);
	}

	window->DrawList->AddText(pos, color.u32(), output);
	ImGui::PopFont();
}


void ImGuiRendering::DrawEspBox(Vector leftUpCorn, Vector rightDownCorn, Color clr, float width)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	ImVec2 min = ImVec2(leftUpCorn.x, leftUpCorn.y);
	ImVec2 max = ImVec2(rightDownCorn.x, rightDownCorn.y);
	window->DrawList->AddRect(min, max, clr.u32(), 0.0F, -1, width);
}

void ImGuiRendering::DrawLine(float x1, float y1, float x2, float y2, Color clr, float thickness)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DrawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), clr.u32(), thickness);
}

void ImGuiRendering::DrawLineGradient(float x1, float y1, float x2, float y2, Color clr1, Color cl2, float thickness)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->AddRectFilledMultiColor(ImVec2(x1, y1), ImVec2(x2, y2),
		clr1.u32(), cl2.u32(), cl2.u32(), clr1.u32());
}

void ImGuiRendering::Rect(float x, float y, float w, float h, Color clr, float rounding)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), clr.u32(), rounding);
}

void ImGuiRendering::FilledRect(float x, float y, float w, float h, Color clr, float rounding)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), clr.u32(), rounding);
}

void ImGuiRendering::FilledRect2(float x, float y, float w, float h, Color clr, float rounding)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->AddRectFilled(ImVec2(x, y), ImVec2(w, h), clr.u32(), rounding);
}

void ImGuiRendering::Triangle(float x1, float y1, float x2, float y2, float x3, float y3, Color clr, float thickness)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), clr.u32(), thickness);
}

void ImGuiRendering::TriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3, Color clr)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), clr.u32());
}

void ImGuiRendering::QuadFilled(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, Color clr)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->AddQuadFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImVec2(x4, y4), clr.u32());
}

void ImGuiRendering::CircleFilled(float x1, float y1, float radius, Color col, int segments)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->AddCircleFilled(ImVec2(x1, y1), radius, col.u32(), segments);
}

void ImGuiRendering::DrawWave(Vector loc, float radius, Color color, float thickness)
{
	static float Step = PI * 3.0f / 40;
	Vector prev;
	for (float lat = 0; lat <= PI * 3.0f; lat += Step)
	{
		float sin1 = sin(lat);
		float cos1 = cos(lat);
		float sin3 = sin(0.0f);
		float cos3 = cos(0.0f);

		Vector point1;
		point1 = Vector(sin1 * cos3, cos1, sin1 * sin3) * radius;
		Vector point3 = loc;
		Vector Out;
		point3 += point1;

		if (math::world_to_screen(point3, Out))
		{
			if (lat > 0.000f)
				DrawLine(prev.x, prev.y, Out.x, Out.y, color, thickness);
		}
		prev = Out;
	}
}