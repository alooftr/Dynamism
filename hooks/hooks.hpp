#pragma once

#include "..\ImGui\imgui.h"
#include "..\ImGui\imgui_internal.h"
#include "..\ImGui\imgui_impl_dx9.h"
#include "..\ImGui\imgui_impl_win32.h"

#include "..\includes.hpp"
#include "..\sdk\interfaces\IBaseClientDll.hpp"
#include "..\sdk\interfaces\IClientMode.hpp"
#include "..\sdk\misc\CUserCmd.hpp"
#include "..\sdk\interfaces\IInputSystem.hpp"
#include "..\sdk\interfaces\IMDLCache.hpp"
#include "..\sdk\interfaces\IGameEventManager.hpp"
#include "..\utils\util.hpp"

#include "vfunc_hook.hpp"
#include <utils\detourhooks.h>

class C_HookedEvents : public IGameEventListener2
{
public:
	void FireGameEvent(IGameEvent * event);
	void RegisterSelf();
	void RemoveSelf();
	int GetEventDebugID(void);
};

namespace INIT
{
	extern HMODULE Dll;
	extern HWND Window;
	extern WNDPROC OldWindow;
}

class c_baseplayeranimationstate;

namespace hooks
{
	extern bool menu_open;
	extern bool input_shouldListen;
	extern int rage_weapon;
	extern int legit_weapon;

	extern ButtonCode_t* input_receivedKeyval;

	inline CDetourHook directx_hook;
	inline CDetourHook directx_hook2;
	inline CDetourHook directx_hook3;

	inline CDetourHook client_hook;
	inline CDetourHook client_hook2;
	inline CDetourHook clientstate_hook;
	inline CDetourHook clientstate_hook2;
	inline CDetourHook engine_hook;
	inline CDetourHook engine_hook2;
	inline CDetourHook engine_hook3;

	inline CDetourHook clientmode_hook;
	inline CDetourHook clientmode_hook2;
	inline CDetourHook clientmode_hook3;
	inline CDetourHook clientmode_hook4;

	inline CDetourHook inputinternal_hook;
	inline CDetourHook inputinternal_hook2;
	inline CDetourHook renderview_hook;
	extern vmthook* modelcache_hook;
	inline CDetourHook panel_hook;
	inline CDetourHook materialsys_hook;
	inline CDetourHook materialsys_hook2;
	inline CDetourHook modelrender_hook;
	inline CDetourHook surface_hook;
	inline CDetourHook bspquery_hook;
	inline CDetourHook prediction_hook;
	inline CDetourHook trace_hook;
	inline CDetourHook trace_hook2;
	inline CDetourHook filesystem_hook;

	extern C_HookedEvents hooked_events;

	using GetForeignFallbackFontNameFn = const char*(__thiscall*)(void*);
	using SetupBonesFn = bool(__thiscall*)(void*, matrix3x4_t*, int, int, float);
	using DoExtraBonesProcessingFn = void(__thiscall*)(player_t*, CStudioHdr*, Vector*, Quaternion*, const matrix3x4_t&, uint8_t*, void*);
	using StandardBlendingRulesFn = void(__thiscall*)(player_t*, CStudioHdr*, Vector*, Quaternion*, float, int);
	using UpdateClientSideAnimationFn = void(__fastcall*)(player_t*);
	using PhysicsSimulateFn = void(__fastcall*)(player_t*);
	using ModifyEyePositionFn = void(__thiscall*)(c_baseplayeranimationstate*, Vector&);
	using CalcViewmodelBobFn = void(__thiscall*)(player_t*, Vector&);
	using ProcessInterpolatedListFn = int(*)(void);

	extern DWORD original_getforeignfallbackfontname;
	extern DWORD original_setupbones;
	extern DWORD original_doextrabonesprocessing;
	extern DWORD original_standardblendingrules;
	extern DWORD original_updateclientsideanimation;
	extern DWORD original_physicssimulate;
	extern DWORD original_modifyeyeposition;
	extern DWORD original_calcviewmodelbob;
	extern DWORD original_processinterpolatedlist;

	inline uint8_t* present_addr;
	using present_fn = long(__stdcall*)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);

	void __stdcall hooked_fsn(ClientFrameStage_t);
	bool __stdcall hooked_createmove(float, CUserCmd*);
	bool __fastcall hooked_drawfog(void* ecx, void* edx);
	void __stdcall hooked_overrideview(CViewSetup * setup);
	bool __fastcall hooked_isconnected(void* ecx, void* edx);
	float __fastcall hooked_getscreenaspectratio(void* ecx, void* edx, int width, int height);
	bool __fastcall hooked_ishltv(void* ecx, void* edx);
	void __stdcall hooked_dme(IMatRenderContext * ctx, const DrawModelState_t & state, const ModelRenderInfo_t & info, matrix3x4_t * bone_to_world);
	void  __fastcall hooked_postscreeneffects(void * thisptr, void * edx, CViewSetup * setup);
	void __fastcall hooked_setkeycodestate(void* thisptr, void* edx, ButtonCode_t code, bool bDown);
	void __fastcall hooked_setmousecodestate(void* thisptr, void* edx, ButtonCode_t code, MouseCodeState_t state);
	void __fastcall hooked_sceneend(void* ecx, void* edx);
	void __fastcall hooked_painttraverse(void* ecx, void* edx, vgui::VPANEL panel, bool force_repaint, bool allow_force);
	void __fastcall hooked_beginframe(void* ecx, void* edx, float ft);
	const char* __fastcall hooked_getforeignfallbackfontname(void* ecx, uint32_t i);
	_declspec(noinline)const char* getforeignfallbackfontname_detour(void* ecx, uint32_t i);
	bool __fastcall hooked_setupbones(void* ecx, void* edx, matrix3x4_t* bone_world_out, int max_bones, int bone_mask, float current_time);
	_declspec(noinline)bool setupbones_detour(void* ecx, matrix3x4_t* bone_world_out, int max_bones, int bone_mask, float current_time);
	void __fastcall hooked_doextrabonesprocessing(player_t* player, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_list, void* context);
	_declspec(noinline)void doextrabonesprocessing_detour(player_t* player, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_list, void* context);
	void __fastcall hooked_standardblendingrules(player_t* player, int i, CStudioHdr* hdr, Vector* pos, Quaternion* q, float curtime, int boneMask);
	_declspec(noinline)void standardblendingrules_detour(player_t* player, int i, CStudioHdr* hdr, Vector* pos, Quaternion* q, float curtime, int boneMask);
	void __fastcall hooked_updateclientsideanimation(player_t* player, uint32_t i);
	_declspec(noinline)void updateclientsideanimation_detour(player_t* player);
	void __fastcall hooked_physicssimulate(player_t* player);
	_declspec(noinline)void physicssimulate_detour(player_t* player);
	void __fastcall hooked_modifyeyeposition(c_baseplayeranimationstate* state, void* edx, Vector& position);
	_declspec(noinline)void modifyeyeposition_detour(c_baseplayeranimationstate* state, Vector& position);
	void __fastcall hooked_calcviewmodelbob(player_t* player, void* edx, Vector& position);
	_declspec(noinline)void calcviewmodelbob_detour(player_t* player, Vector& position);
	bool __fastcall hooked_shouldskipanimframe();
	int processinterpolatedlist();
	IMaterial* __fastcall hooked_getmaterial(void* ecx, void* edx, const char* material_name, const char* texture_group_name, bool complain, const char* complain_prefix);
	void __fastcall hooked_packetstart(void* ecx, void* edx, int incoming, int outgoing);
	void __fastcall hooked_packetend(void* ecx, void* edx);
	void __stdcall hooked_lockcursor();
	int __fastcall hooked_listleavesinbox(void* ecx, void* edx, Vector& mins, Vector& maxs, unsigned short* list, int list_max);
	void __fastcall hooked_runcommand(void* ecx, void* edx, player_t* player, CUserCmd* m_pcmd, IMoveHelper* move_helper);
	bool __stdcall hooked_inprediction();
	bool __fastcall hooked_writeusercmddeltatobuffer(void* ecx, void* edx, int slot, bf_write* buf, int from, int to, bool is_new_command);
	void __fastcall hooked_clip_ray_collideable(void* ecx, void* edx, const Ray_t& ray, uint32_t fMask, ICollideable* pCollide, CGameTrace* pTrace);
	void __fastcall hooked_trace_ray(void* ecx, void* edx, const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace);
	bool __fastcall hooked_loosefileallowed(void* ecx, void* edx);
	void __fastcall hooked_checkfilecrcswithserver(void* ecx, void* edx);
	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	long __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice);
	long __stdcall Hooked_EndScene_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	long __stdcall hooked_present(IDirect3DDevice9* device, RECT* src_rect, RECT* dest_rect, HWND dest_wnd_override, RGNDATA* dirty_region);
	void GUI_Init(IDirect3DDevice9* pDevice);

	typedef long(__stdcall *EndSceneFn)(IDirect3DDevice9* device);
	typedef long(__stdcall *EndSceneResetFn)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	typedef long(__stdcall* PresentFn)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);

	typedef void(__thiscall* SetKeyCodeState_t) (void*, ButtonCode_t, bool);
	extern SetKeyCodeState_t o_SetKeyCodeState;

	typedef void(__thiscall* SetMouseCodeState_t) (void*, ButtonCode_t, MouseCodeState_t);
	extern SetMouseCodeState_t o_SetMouseCodeState;


	extern LPDIRECT3DTEXTURE9 Head;
	extern LPDIRECT3DTEXTURE9 WaterMarker;
	extern LPDIRECT3DTEXTURE9 EnemyTabs;
	extern LPDIRECT3DTEXTURE9 SelfTabs;
	extern LPDIRECT3DTEXTURE9 EyeTabs;
	extern LPDIRECT3DTEXTURE9 TeamTabs;
	extern LPDIRECT3DTEXTURE9 LayerTabs;
	extern LPDIRECT3DTEXTURE9 HomeTabs;
	extern LPDIRECT3DTEXTURE9 RageTabs;
	extern LPDIRECT3DTEXTURE9 AntiAimTabs;
	extern LPDIRECT3DTEXTURE9 LegitTabs;
	extern LPDIRECT3DTEXTURE9 TriggerTabs;
	extern LPDIRECT3DTEXTURE9 LogoImg;
	extern LPDIRECT3DTEXTURE9 LuaTabs;
	extern LPDIRECT3DTEXTURE9 MiscTabs;
	extern LPDIRECT3DTEXTURE9 MovementTabs;
	extern LPDIRECT3DTEXTURE9 SkinChangerTabs;
	extern LPDIRECT3DTEXTURE9 ChatBoxTabs;
	extern LPDIRECT3DTEXTURE9 Profile;
	extern LPDIRECT3DTEXTURE9 Recorted;
	extern LPDIRECT3DTEXTURE9 SkinTabs;
	inline IDirect3DDevice9* device;
	extern LPDIRECT3DTEXTURE9 LogoTwo;
	inline int currentweapon = -1;

	inline uint8_t* present_address;
	inline present_fn original_present;
	
}

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);