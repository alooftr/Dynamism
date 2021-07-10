// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "slowwalk.h"

void slowwalk::create_move(CUserCmd* m_pcmd)
{
	if (config_system.g_cfg.misc.slowwalk)
	{

		auto weapon_info = m_weaponsystem()->GetWeaponData(g_ctx.globals.weapon->m_iItemDefinitionIndex());

		if (!weapon_info)
			return;

		float speed = 0.0034f * config_system.g_cfg.misc.slowwalk_speed;
		if (weapon_info) {
			float max_speed = g_ctx.globals.weapon->m_zoomLevel() == 0 ? weapon_info->flMaxPlayerSpeed : weapon_info->flMaxPlayerSpeedAlt;
			float ratio = max_speed / 250.0f;
			speed *= ratio;
		}



		m_pcmd->m_forwardmove *= speed;
		m_pcmd->m_sidemove *= speed;
	}
}