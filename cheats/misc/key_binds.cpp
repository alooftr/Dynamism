// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "key_binds.h"
#include "..\..\includes.hpp"
#include "misc.h"

void key_binds::update_key_bind(key_bind* key_bind, int key_bind_id)
{
	auto is_button_down = util::is_button_down(key_bind->key);
	
	if (key_bind->type == 0)
	{
		if (is_button_down)
			key_bind->holdortoggle = true;
		else if (!is_button_down)
			key_bind->holdortoggle = false;
	}

	switch (key_bind->mode)
	{
	case HOLD:
		switch (key_bind_id)
		{
		case 2:
			if (misc::get().recharging_double_tap)
				break;

			misc::get().double_tap_key = is_button_down;

			if (misc::get().double_tap_key && config_system.g_cfg.ragebot.double_tap_key.key != config_system.g_cfg.antiaim.hide_shots_key.key)
				misc::get().hide_shots_key = false;

			break;
		case 12:
			misc::get().hide_shots_key = is_button_down;

			if (misc::get().hide_shots_key && config_system.g_cfg.antiaim.hide_shots_key.key != config_system.g_cfg.ragebot.double_tap_key.key)
				misc::get().double_tap_key = false;

			break;
		case 13:
			if (is_button_down)
				antiaim::get().manual_side = SIDE_BACK;
			else if (antiaim::get().manual_side == SIDE_BACK)
				antiaim::get().manual_side = SIDE_NONE;

			break;
		case 14:
			if (is_button_down)
				antiaim::get().manual_side = SIDE_LEFT;
			else if (antiaim::get().manual_side == SIDE_LEFT)
				antiaim::get().manual_side = SIDE_NONE;

			break;
		case 15:
			if (is_button_down)
				antiaim::get().manual_side = SIDE_RIGHT;
			else if (antiaim::get().manual_side == SIDE_RIGHT)
				antiaim::get().manual_side = SIDE_NONE;

			break;
		default:
			keys[key_bind_id] = is_button_down;
			break;
		}
		
		break;
	case TOGGLE:
		if (!key_bind->holding && is_button_down)
		{
			switch (key_bind_id)
			{
			case 2:
				if (misc::get().recharging_double_tap)
					break;

				misc::get().double_tap_key = !misc::get().double_tap_key;

				if (misc::get().double_tap_key && config_system.g_cfg.ragebot.double_tap_key.key != config_system.g_cfg.antiaim.hide_shots_key.key)
					misc::get().hide_shots_key = false;

				break;
			case 12:
				misc::get().hide_shots_key = !misc::get().hide_shots_key;

				if (misc::get().hide_shots_key && config_system.g_cfg.antiaim.hide_shots_key.key != config_system.g_cfg.ragebot.double_tap_key.key)
					misc::get().double_tap_key = false;

				break;
			case 13:
				if (antiaim::get().manual_side == SIDE_BACK)
					antiaim::get().manual_side = SIDE_NONE;
				else
					antiaim::get().manual_side = SIDE_BACK;

				break;
			case 14:
				if (antiaim::get().manual_side == SIDE_LEFT)
					antiaim::get().manual_side = SIDE_NONE;
				else
					antiaim::get().manual_side = SIDE_LEFT;

				break;
			case 15:
				if (antiaim::get().manual_side == SIDE_RIGHT)
					antiaim::get().manual_side = SIDE_NONE;
				else
					antiaim::get().manual_side = SIDE_RIGHT;

				break;
			default:
				keys[key_bind_id] = !keys[key_bind_id];
				key_bind->holdortoggle = !key_bind->holdortoggle;
				break;
			}

			key_bind->holding = true;
		}
		else if (key_bind->holding && !is_button_down)
			key_bind->holding = false;

		break;
	}

	mode[key_bind_id] = key_bind->mode;
}

void key_binds::initialize_key_binds()
{
	for (auto i = 0; i < 29; i++)
	{
		keys[i] = false;

		if (i == 2 || i >= 12 && i <= 17) //-V648
			mode[i] = TOGGLE;
		else
			mode[i] = HOLD;
	}
}

void key_binds::update_key_binds()
{
	update_key_bind(&config_system.g_cfg.legitbot.autofire_key, 0);
	update_key_bind(&config_system.g_cfg.legitbot.key, 1);
	update_key_bind(&config_system.g_cfg.ragebot.double_tap_key, 2);
	update_key_bind(&config_system.g_cfg.ragebot.safe_point_key, 3);

	for (auto i = 0; i < 8; i++)
		update_key_bind(&config_system.g_cfg.ragebot.weapon[i].damage_override_key, 4 + i);

	update_key_bind(&config_system.g_cfg.antiaim.hide_shots_key, 12);
	update_key_bind(&config_system.g_cfg.antiaim.manual_back, 13);
	update_key_bind(&config_system.g_cfg.antiaim.manual_left, 14);
	update_key_bind(&config_system.g_cfg.antiaim.manual_right, 15);
	update_key_bind(&config_system.g_cfg.antiaim.flip_desync, 16);
	update_key_bind(&config_system.g_cfg.misc.thirdperson_toggle, 17);
	update_key_bind(&config_system.g_cfg.misc.automatic_peek, 18);
	update_key_bind(&config_system.g_cfg.misc.edge_jump_key, 19);
	update_key_bind(&config_system.g_cfg.misc.fakeduck_key, 20);
	update_key_bind(&config_system.g_cfg.misc.slowwalk_key, 21);
	update_key_bind(&config_system.g_cfg.ragebot.body_aim_key, 22);
	update_key_bind(&config_system.g_cfg.misc.longjump_key, 23);
	update_key_bind(&config_system.g_cfg.misc.edgebug_key, 24);
	update_key_bind(&config_system.g_cfg.misc.jumpbug_key, 25);
	update_key_bind(&config_system.g_cfg.misc.recorder, 26);
	update_key_bind(&config_system.g_cfg.misc.playing, 27);
	update_key_bind(&config_system.g_cfg.ragebot.resolver_override, 28);
	update_key_bind(&config_system.g_cfg.triggerbot.trigger_key, 29);
}

bool key_binds::get_key_bind_state(int key_bind_id)
{
	return keys[key_bind_id];
}

bool key_binds::get_key_bind_state_lua(int key_bind_id)
{
	if (key_bind_id < 0 || key_bind_id > 29)
		return false;

	switch (key_bind_id)
	{
	case 2:
		return misc::get().double_tap_key;
	case 4:
		if (g_ctx.globals.current_weapon < 0)
			return false;

		return keys[4 + g_ctx.globals.current_weapon];
	case 12:
		return misc::get().hide_shots_key;
	case 13:
		return antiaim::get().manual_side == SIDE_BACK;
	case 14:
		return antiaim::get().manual_side == SIDE_LEFT;
	case 15:
		return antiaim::get().manual_side == SIDE_RIGHT;
	default:
		return keys[key_bind_id];
	}
}

bool key_binds::get_key_bind_mode(int key_bind_id)
{
	return mode[key_bind_id];
}