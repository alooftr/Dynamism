// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.hpp"

#include <tchar.h>
#include <iostream>
#include <d3d9.h>
#include <dinput.h>

#include "../Render.h"
#include "..\cheats\misc\logs.h"
#include "..\cheats\misc\misc.h"
#include "..\cheats\visuals\other_esp.h"
#include "..\cheats\visuals\radar.h"
#include "../ImGui/imgui_freetype.h"

#include "../DynamismIcons.hpp"
#include "../cheats/lagcompensation/animation_system.h"
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment(lib, "freetype.lib")
#include "../ImGui/implot.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <thread>
#include "..\cheats\menu.h"
#include "../Bytesa.h"

auto _visible = true;
static auto d3d_init = false;

namespace INIT
{
	HMODULE Dll;
	HWND Window;
	WNDPROC OldWindow;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace hooks
{

	LPDIRECT3DTEXTURE9 Head = nullptr;
	LPDIRECT3DTEXTURE9 WaterMarker = nullptr;
	LPDIRECT3DTEXTURE9 EnemyTabs = nullptr;
	LPDIRECT3DTEXTURE9 SelfTabs = nullptr;
	LPDIRECT3DTEXTURE9 EyeTabs = nullptr;
	LPDIRECT3DTEXTURE9 TeamTabs = nullptr;
	LPDIRECT3DTEXTURE9 LayerTabs = nullptr;
	LPDIRECT3DTEXTURE9 HomeTabs = nullptr;
	LPDIRECT3DTEXTURE9 RageTabs = nullptr;
	LPDIRECT3DTEXTURE9 AntiAimTabs = nullptr;
	LPDIRECT3DTEXTURE9 LegitTabs = nullptr;
	LPDIRECT3DTEXTURE9 TriggerTabs = nullptr;
	LPDIRECT3DTEXTURE9 LogoImg = nullptr;
	LPDIRECT3DTEXTURE9 SkinTabs = nullptr;
	LPDIRECT3DTEXTURE9 LuaTabs = nullptr;
	LPDIRECT3DTEXTURE9 MiscTabs = nullptr;
	LPDIRECT3DTEXTURE9 MovementTabs = nullptr;
	LPDIRECT3DTEXTURE9 SkinChangerTabs = nullptr;
	LPDIRECT3DTEXTURE9 ChatBoxTabs = nullptr;
	LPDIRECT3DTEXTURE9 Profile = nullptr;
	LPDIRECT3DTEXTURE9 Recorted = nullptr;
	LPDIRECT3DTEXTURE9 LogoTwo = nullptr;
	int rage_weapon = 0;
	int legit_weapon = 0;
	bool menu_open = false;
	bool input_shouldListen = false;

	ButtonCode_t* input_receivedKeyval;

	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static auto is_down = true;
		static auto is_clicked = false;

		if (GetAsyncKeyState(VK_INSERT))
		{
			is_clicked = false;
			is_down = true;
		}
		else if (!GetAsyncKeyState(VK_INSERT) && is_down)
		{
			is_clicked = true;
			is_down = false;
		}
		else
		{
			is_clicked = false;
			is_down = false;
		}

		if (is_clicked)
			menu_open = !menu_open;

		auto pressed_buttons = false;
		auto pressed_menu_key = uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_MOUSEWHEEL;

		if (g_ctx.local()->is_alive() && !pressed_menu_key && !g_ctx.globals.focused_on_input)
			pressed_buttons = true;

		if (!pressed_buttons && d3d_init && menu_open && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam) && !input_shouldListen)
			return true;

		if (menu_open && (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE) && !input_shouldListen)
			return false;

		return CallWindowProc(INIT::OldWindow, hWnd, uMsg, wParam, lParam);
	}

	long __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		static auto original_fn = directx_hook3.GetOriginal <EndSceneFn> ();
		return original_fn(pDevice);
	}

	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGui_ImplWin32_Init(INIT::Window);
		ImGui_ImplDX9_Init(pDevice);

		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();

		style.WindowMinSize = ImVec2(10, 10);

		ImFontConfig m_config;
		m_config.OversampleH = m_config.OversampleV = 3;
		m_config.PixelSnapH = false;

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF,
			0x0400, 0x044F,
			0
		};

		char windows_directory[MAX_PATH];
		GetWindowsDirectory(windows_directory, MAX_PATH);

		auto verdana_directory = (std::string)windows_directory + "\\Fonts\\Verdana.ttf";

		hooks::device = pDevice;

		c_menu::get().futura_small = io.Fonts->AddFontFromMemoryCompressedTTF(DynamismPrincipal_compressed_data, DynamismPrincipal_compressed_size, 13.f);
		c_menu::get().futura = io.Fonts->AddFontFromMemoryCompressedTTF(DynamismPrincipal2_compressed_data, DynamismPrincipal2_compressed_size, 30.f);
		c_menu::get().futura_large = io.Fonts->AddFontFromMemoryCompressedTTF(DynamismPrincipal_compressed_data, DynamismPrincipal_compressed_size, 25.f);
		c_menu::get().ico_menu = io.Fonts->AddFontFromMemoryCompressedTTF(iconsfonts_compressed_data, iconsfonts_compressed_size, 20.f);
		c_menu::get().antiaim_icons = io.Fonts->AddFontFromMemoryCompressedTTF(antiaimf_compressed_data, antiaimf_compressed_size, 25.f);
		c_menu::get().arrow_font = io.Fonts->AddFontFromMemoryCompressedTTF(MiscFont_compressed_data, MiscFont_compressed_size, 25.f);
		c_menu::get().arrow = io.Fonts->AddFontFromMemoryCompressedTTF(MiscFont_compressed_data, MiscFont_compressed_size, 14.f);
		c_menu::get().misc_font = io.Fonts->AddFontFromMemoryCompressedTTF(MiscFont_compressed_data, MiscFont_compressed_size, 14.f);
		c_menu::get().compart_font = io.Fonts->AddFontFromMemoryCompressedTTF(Font_compressed_data, Font_compressed_size, 12.f);
		c_menu::get().load_font = io.Fonts->AddFontFromMemoryCompressedTTF(load_compressed_data, load_compressed_size, 14.f);
		c_menu::get().super_ico = io.Fonts->AddFontFromMemoryTTF((void*)super_ico, sizeof(super_ico), 20.f, &m_config, ranges);

		ImFontConfig cfg;

		c_menu::get().menu_font = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdana.ttf", 12, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		c_menu::get().speed_font = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdanab.ttf", 30, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		c_menu::get().stamina = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdanab.ttf", 15, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		if (LogoImg == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &dynamism, sizeof(dynamism), 500, 550, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &LogoImg);

		if (Head == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &HeaderMenu, sizeof(HeaderMenu), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Head);

		if (Recorted == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &recorted, sizeof(recorted), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Recorted);

		if (EnemyTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &EnemyTab, sizeof(EnemyTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &EnemyTabs);

		if (SelfTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &SelfTab, sizeof(SelfTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &SelfTabs);

		if (EyeTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &EyeTab, sizeof(EyeTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &EyeTabs);

		if (TeamTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &TeamTab, sizeof(TeamTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &TeamTabs);

		if (LayerTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &LeayerTab, sizeof(LeayerTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &LayerTabs);

		if (HomeTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &HomeTab, sizeof(HomeTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &HomeTabs);

		if (RageTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &RageTab, sizeof(RageTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &RageTabs);

		if (AntiAimTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &AntiTab, sizeof(AntiTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &AntiAimTabs);

		if (LegitTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &LegitTab, sizeof(LegitTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &LegitTabs);

		if (TriggerTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &TriggerTab, sizeof(TriggerTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &TriggerTabs);

		if (MovementTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &MovementTab, sizeof(MovementTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &MovementTabs);

		if (MiscTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &MiscTab, sizeof(MiscTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &MiscTabs);

		if (LuaTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &LuaTab, sizeof(LuaTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &LuaTabs);

		if (SkinChangerTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &SkinChangerTab, sizeof(SkinChangerTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &SkinChangerTabs);

		if (ChatBoxTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &ChatBoxTab, sizeof(ChatBoxTab), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &ChatBoxTabs);

		if (WaterMarker == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &WaterMark, sizeof(WaterMark), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &WaterMarker);

		if (SkinTabs == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &gloveMenu, sizeof(gloveMenu), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &SkinTabs);
		
		if (LogoTwo == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &Logo2, sizeof(Logo2), 500, 550, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &LogoTwo);

		ImGui_ImplDX9_CreateDeviceObjects();
		d3d_init = true;
	}

	long __stdcall hooked_present(IDirect3DDevice9* device, RECT* src_rect, RECT* dest_rect, HWND dest_wnd_override, RGNDATA* dirty_region)
	{

		static auto ofunc = directx_hook2.GetOriginal<PresentFn>();

		g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

		if (!d3d_init)
			GUI_Init(device);

		IDirect3DVertexDeclaration9* vertex_dec;
		device->GetVertexDeclaration(&vertex_dec);

		IDirect3DVertexShader9* vertex_shader;
		device->GetVertexShader(&vertex_shader);

		c_menu::get().device = device;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		c_menu::get().draw(menu_open);
		c_menu::get().AboutDyn(menu_open);

		g_Render->BeginScene();
		g_Render->EndScene();

		c_menu::get().draw_keybinds();
		c_menu::get().draw_indicators();

		misc::get().spectators_list();
		c_menu::get().DrawRadar();		

		ImGui::EndFrame();
		ImGui::Render();

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		device->SetVertexShader(vertex_shader);
		device->SetVertexDeclaration(vertex_dec);

		return ofunc(device, src_rect, dest_rect, dest_wnd_override, dirty_region);
	}

	long __stdcall Hooked_EndScene_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto ofunc = directx_hook.GetOriginal<EndSceneResetFn>();

		if (!d3d_init)
			return ofunc(pDevice, pPresentationParameters);

		ImGui_ImplDX9_InvalidateDeviceObjects();

		auto hr = ofunc(pDevice, pPresentationParameters);

		if (SUCCEEDED(hr))
			ImGui_ImplDX9_CreateDeviceObjects();

		return hr;
	}

	DWORD original_getforeignfallbackfontname;
	DWORD original_setupbones;
	DWORD original_doextrabonesprocessing;
	DWORD original_standardblendingrules;
	DWORD original_updateclientsideanimation;
	DWORD original_physicssimulate;
	DWORD original_modifyeyeposition;
	DWORD original_calcviewmodelbob;
	DWORD original_processinterpolatedlist;


	C_HookedEvents hooked_events;
}

void __fastcall hooks::hooked_setkeycodestate(void* thisptr, void* edx, ButtonCode_t code, bool bDown)
{
	static auto original_fn = inputinternal_hook.GetOriginal <SetKeyCodeState_t> ();

	if (input_shouldListen && bDown)
	{
		input_shouldListen = false;

		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return original_fn(thisptr, code, bDown);
}

void __fastcall hooks::hooked_setmousecodestate(void* thisptr, void* edx, ButtonCode_t code, MouseCodeState_t state)
{
	static auto original_fn = inputinternal_hook2.GetOriginal <SetMouseCodeState_t> ();

	if (input_shouldListen && state == BUTTON_PRESSED)
	{
		input_shouldListen = false;

		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return original_fn(thisptr, code, state);
}	
