#include <ShlObj.h>
#include <ShlObj_core.h>
#include "includes.hpp"
#include "utils\ctx.hpp"
#include "utils\recv.h"
#include "utils\imports.h"
#include "nSkinz\SkinChanger.h"
#include <Kit_parser.h>
#include "utils/anti_debug.h"
#include "cheats/lagcompensation/animation_system.h"
#include "cheats/visuals/player_esp.h"

PVOID base_address = nullptr;

__forceinline void setup_netvars();
__forceinline void setup_skins();
__forceinline void setup_hooks();

DWORD WINAPI main(PVOID base)
{

	std::filesystem::path path2;
	PWSTR pathToDocuments2;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments2))) {
		path2 = pathToDocuments2;
		path2 /= "Dynamism.space";
		CoTaskMemFree(pathToDocuments2);
	}
	std::filesystem::create_directory(path2);

	std::filesystem::path path;
	PWSTR pathToDocuments;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
		path = pathToDocuments;
		path /= "Dynamism.space/Configs";
		CoTaskMemFree(pathToDocuments);
	}
	std::filesystem::create_directory(path);

	std::filesystem::path path3;
	PWSTR pathToDocuments3;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments3))) {
		path3 = pathToDocuments3;
		path3 /= "Dynamism.space/MovementRecorder";
		CoTaskMemFree(pathToDocuments3);
	}
	std::filesystem::create_directory(path3);

	g_ctx.signatures =
	{
		crypt_str("A1 ? ? ? ? 50 8B 08 FF 51 0C"),
		crypt_str("B9 ?? ?? ?? ?? A1 ?? ?? ?? ?? FF 10 A1 ?? ?? ?? ?? B9"),
		crypt_str("0F 11 05 ?? ?? ?? ?? 83 C8 01"),
		crypt_str("8B 0D ?? ?? ?? ?? 8B 46 08 68"),
		crypt_str("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10"),
		crypt_str("8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7"),
		crypt_str("A1 ? ? ? ? 8B 0D ? ? ? ? 6A 00 68 ? ? ? ? C6"),
		crypt_str("80 3D ? ? ? ? ? 53 56 57 0F 85"),
		crypt_str("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C"),
		crypt_str("80 3D ? ? ? ? ? 74 06 B8"),
		crypt_str("55 8B EC 83 E4 F0 B8 D8"),
		crypt_str("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C"),
		crypt_str("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6"),
		crypt_str("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"),
		crypt_str("56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 23"),
		crypt_str("55 8B EC 83 E4 F8 83 EC 5C 53 8B D9 56 57 83"),
		crypt_str("55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9"),
		crypt_str("57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"),
		crypt_str("55 8B EC 81 EC ? ? ? ? 53 8B D9 89 5D F8 80"),
		crypt_str("53 0F B7 1D ? ? ? ? 56"),
		crypt_str("8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6")
	};

	g_ctx.indexes =
	{
		5,
		33,
		339,
		218,
		219,
		34,
		157,
		75,
		460,
		482,
		452,
		483,
		284,
		223,
		246,
		27,
		17,
		123
	};

	while (!IFH(GetModuleHandle)(crypt_str("serverbrowser.dll")))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	setup_netvars();
	game_data::initialize_kits();

	c_lua::get().initialize();
	config_system.run("");
	config_system.run2("");
	key_binds::get().initialize_key_binds();

	setup_hooks();
	Netvars::Netvars();
	setup_skins();
	GP_Esp = new CEsp();
	GP_Esp->InitVisuals();

	//CHAT::get().Main();


	return 1UL;

}



BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(nullptr, 0U, main, hModule, 0U, nullptr); //-V718 //-V513
		return TRUE;
	}

	return FALSE;
}
//
__forceinline void setup_netvars()
{
	netvars::get().tables.clear();
	auto client = m_client()->GetAllClasses();

	if (!client)
		return;

	while (client)
	{
		auto recvTable = client->m_pRecvTable;

		if (recvTable)
			netvars::get().tables.emplace(std::string(client->m_pNetworkName), recvTable);

		client = client->m_pNext;
	}
}
//
__forceinline void setup_skins()
{
	auto items = std::ifstream(crypt_str("csgo/scripts/items/items_game_cdn.txt"));
	auto gameItems = std::string(std::istreambuf_iterator <char> { items }, std::istreambuf_iterator <char> { });

	if (!items.is_open())
		return;

	items.close();
	memory.initialize();

	for (auto i = 0; i <= memory.itemSchema()->paintKits.lastElement; i++)
	{
		auto paintKit = memory.itemSchema()->paintKits.memory[i].value;

		if (paintKit->id == 9001)
			continue;

		auto itemName = m_localize()->FindSafe(paintKit->itemName.buffer + 1);
		auto itemNameLength = WideCharToMultiByte(CP_UTF8, 0, itemName, -1, nullptr, 0, nullptr, nullptr);

		if (std::string name(itemNameLength, 0); WideCharToMultiByte(CP_UTF8, 0, itemName, -1, &name[0], itemNameLength, nullptr, nullptr))
		{
			if (paintKit->id < 10000)
			{
				if (auto pos = gameItems.find('_' + std::string{ paintKit->name.buffer } + '='); pos != std::string::npos && gameItems.substr(pos + paintKit->name.length).find('_' + std::string{ paintKit->name.buffer } + '=') == std::string::npos)
				{
					if (auto weaponName = gameItems.rfind(crypt_str("weapon_"), pos); weaponName != std::string::npos)
					{
						name.back() = ' ';
						name += '(' + gameItems.substr(weaponName + 7, pos - weaponName - 7) + ')';
					}
				}
				SkinChanger::skinKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
			else
			{
				std::string_view gloveName{ paintKit->name.buffer };
				name.back() = ' ';
				name += '(' + std::string{ gloveName.substr(0, gloveName.find('_')) } + ')';
				SkinChanger::gloveKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
		}
	}

	std::sort(SkinChanger::skinKits.begin(), SkinChanger::skinKits.end());
	std::sort(SkinChanger::gloveKits.begin(), SkinChanger::gloveKits.end());
}

template <typename T = void*>
constexpr T GetVFunc(void* thisptr, std::size_t nIndex)
{
	return (*static_cast<T**>(thisptr))[nIndex];
}
//
//
__forceinline void setup_hooks()
{
	MH_Initialize();

	static auto getforeignfallbackfontname = (DWORD)(util::FindSignature(crypt_str("vguimatsurface.dll"), g_ctx.signatures.at(9).c_str()));
	hooks::original_getforeignfallbackfontname = (DWORD)DetourFunction((PBYTE)getforeignfallbackfontname, (PBYTE)hooks::hooked_getforeignfallbackfontname); //-V206

	static auto setupbones = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(10).c_str()));
	hooks::original_setupbones = (DWORD)DetourFunction((PBYTE)setupbones, (PBYTE)hooks::hooked_setupbones); //-V206

	static auto doextrabonesprocessing = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(11).c_str()));
	hooks::original_doextrabonesprocessing = (DWORD)DetourFunction((PBYTE)doextrabonesprocessing, (PBYTE)hooks::hooked_doextrabonesprocessing); //-V206

	static auto standardblendingrules = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(12).c_str()));
	hooks::original_standardblendingrules = (DWORD)DetourFunction((PBYTE)standardblendingrules, (PBYTE)hooks::hooked_standardblendingrules); //-V206
	static auto physicssimulate = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(14).c_str()));
	hooks::original_physicssimulate = (DWORD)DetourFunction((PBYTE)physicssimulate, (PBYTE)hooks::hooked_physicssimulate);

	static auto modifyeyeposition = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(15).c_str()));
	hooks::original_modifyeyeposition = (DWORD)DetourFunction((PBYTE)modifyeyeposition, (PBYTE)hooks::hooked_modifyeyeposition);

	static auto calcviewmodelbob = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(16).c_str()));
	hooks::original_calcviewmodelbob = (DWORD)DetourFunction((PBYTE)calcviewmodelbob, (PBYTE)hooks::hooked_calcviewmodelbob);

	static auto shouldskipanimframe = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(17).c_str()));
	DetourFunction((PBYTE)shouldskipanimframe, (PBYTE)hooks::hooked_shouldskipanimframe);

	static auto checkfilecrcswithserver = (DWORD)(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(18).c_str()));
	DetourFunction((PBYTE)checkfilecrcswithserver, (PBYTE)hooks::hooked_checkfilecrcswithserver);

	static auto processinterpolatedlist = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(19).c_str()));
	hooks::original_processinterpolatedlist = (DWORD)DetourFunction((BYTE*)processinterpolatedlist, (BYTE*)hooks::processinterpolatedlist); //-V206

	static auto updateclientsideanimation = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(13).c_str()));
	hooks::original_updateclientsideanimation = (DWORD)DetourFunction((PBYTE)updateclientsideanimation, (BYTE*)hooks::hooked_updateclientsideanimation); //-V206

	hooks::client_hook.Create(GetVFunc(m_client(), 37), &hooks::hooked_fsn);
	hooks::client_hook2.Create(GetVFunc(m_client(), 24), &hooks::hooked_writeusercmddeltatobuffer);
	hooks::clientstate_hook.Create(GetVFunc(reinterpret_cast<DWORD**>((CClientState*)(uint32_t(m_clientstate()) + 0x8)), 5), &hooks::hooked_packetstart);
	hooks::clientstate_hook2.Create(GetVFunc(reinterpret_cast<DWORD**>((CClientState*)(uint32_t(m_clientstate()) + 0x8)), 6), &hooks::hooked_packetend);
	hooks::panel_hook.Create(GetVFunc(m_panel(), 41), &hooks::hooked_painttraverse);
	hooks::clientmode_hook.Create(GetVFunc(m_clientmode(), 24), &hooks::hooked_createmove);
	hooks::clientmode_hook2.Create(GetVFunc(m_clientmode(), 44), &hooks::hooked_postscreeneffects);
	hooks::clientmode_hook3.Create(GetVFunc(m_clientmode(), 18), &hooks::hooked_overrideview);
	hooks::clientmode_hook4.Create(GetVFunc(m_clientmode(), 17), &hooks::hooked_drawfog);
	hooks::inputinternal_hook.Create(GetVFunc(m_inputinternal(), 91), &hooks::hooked_setkeycodestate);
	hooks::inputinternal_hook2.Create(GetVFunc(m_inputinternal(), 92), &hooks::hooked_setmousecodestate);
	hooks::engine_hook.Create(GetVFunc(m_engine(), 27), &hooks::hooked_isconnected);
	hooks::engine_hook2.Create(GetVFunc(m_engine(), 101), &hooks::hooked_getscreenaspectratio);
	hooks::engine_hook3.Create(GetVFunc(m_engine(), 93), &hooks::hooked_ishltv);
	hooks::renderview_hook.Create(GetVFunc(m_renderview(), 9), &hooks::hooked_sceneend);
	hooks::materialsys_hook.Create(GetVFunc(m_materialsystem(), 42), &hooks::hooked_beginframe);
	hooks::materialsys_hook2.Create(GetVFunc(m_materialsystem(), 84), &hooks::hooked_getmaterial);
	hooks::modelrender_hook.Create(GetVFunc(m_modelrender(), 21), &hooks::hooked_dme);
	hooks::surface_hook.Create(GetVFunc(m_surface(), 67), &hooks::hooked_lockcursor);
	hooks::bspquery_hook.Create(GetVFunc(m_engine()->GetBSPTreeQuery(), 6), &hooks::hooked_listleavesinbox);
	hooks::prediction_hook.Create(GetVFunc(m_prediction(), 19), &hooks::hooked_runcommand);
	hooks::trace_hook.Create(GetVFunc(m_trace(), 4), &hooks::hooked_clip_ray_collideable);
	//hooks::trace_hook2.Create(GetVFunc(m_trace(), 5), &hooks::hooked_trace_ray);
	hooks::filesystem_hook.Create(GetVFunc(reinterpret_cast<DWORD**>(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(20).c_str()) + 0x2), 128), &hooks::hooked_loosefileallowed);

	while (!(INIT::Window = IFH(FindWindow)(crypt_str("Valve001"), nullptr)))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	INIT::OldWindow = (WNDPROC)IFH(SetWindowLongPtr)(INIT::Window, GWL_WNDPROC, (LONG_PTR)hooks::Hooked_WndProc);

	// fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
	hooks::directx_hook.Create(GetVFunc(m_device(), 16), &hooks::Hooked_EndScene_Reset);
	hooks::directx_hook2.Create(GetVFunc(m_device(), 17), &hooks::hooked_present);
	hooks::directx_hook3.Create(GetVFunc(m_device(), 42), &hooks::Hooked_EndScene);

	hooks::hooked_events.RegisterSelf();
}