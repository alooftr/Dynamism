// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"

using GetMaterial_t = IMaterial*(__thiscall*)(void*, const char*, const char*, bool, const char*);

IMaterial* __fastcall hooks::hooked_getmaterial(void* ecx, void* edx, const char* material_name, const char* texture_group_name, bool complain, const char* complain_prefix)
{
	static auto original_fn = materialsys_hook2.GetOriginal <GetMaterial_t> ();

	if (!material_name)
		return original_fn(ecx, material_name, texture_group_name, complain, complain_prefix);

	if (config_system.g_cfg.player.enable && config_system.g_cfg.esp.REMOVALS_SCOPE && !strcmp(material_name, "dev/scope_bluroverlay"))
		return original_fn(ecx, "dev/clearalpha", nullptr, complain, complain_prefix);

	return original_fn(ecx, material_name, texture_group_name, complain, complain_prefix);
}