#pragma once
#include "../../includes.hpp"

class CTriggerBot
{
public:
	bool IsLineGoesThroughSmoke(Vector vStartPos, Vector vEndPos);
	void TriggerBot(CUserCmd* pCmd);
private:
	int trigger_delay{ 0 };
	bool enabled;
	bool is_delayed = false;
	bool is_delayed_trigger = false;
	float shot_delay_time = 0;
	float shot_delay_time_trigger = 0;
	bool shot_delay = false;
	bool shot_delay_trigger = false;
	int key = 0;


	bool is_knife(void* weapon) {
		if (!weapon)
			return false;
		int id = g_ctx.globals.weapon->m_iItemDefinitionIndex();
		static const std::vector<int> v = { WEAPON_KNIFE, WEAPON_KNIFEGG, WEAPON_KNIFE_BUTTERFLY, WEAPON_KNIFE_FALCHION, WEAPON_KNIFE_FLIP, WEAPON_KNIFE_GUT, 520, WEAPON_KNIFE_KARAMBIT, WEAPON_KNIFE_M9_BAYONET, WEAPON_KNIFE_PUSH, 522, WEAPON_KNIFE_T, WEAPON_KNIFE_TACTICAL, 519, 523, WEAPON_KNIFE_BAYONET };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}

	bool is_grenade(void* weapon) {
		if (!weapon)
			return false;

		int id = g_ctx.globals.weapon->m_iItemDefinitionIndex();
		static const std::vector<int> v = { WEAPON_HEGRENADE, WEAPON_INCGRENADE, WEAPON_SMOKEGRENADE, WEAPON_MOLOTOV , WEAPON_DECOY };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}

	bool is_bomb(void* weapon) {
		if (!weapon)
			return false;
		int id = g_ctx.globals.weapon->m_iItemDefinitionIndex();
		static const std::vector<int> v = { WEAPON_C4 };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}

};
extern CTriggerBot TriggerBot;

