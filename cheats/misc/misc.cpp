// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "misc.h"
#include "fakelag.h"
#include "..\ragebot\aim.h"
#include "..\visuals\world_esp.h"
#include "prediction_system.h"
#include "logs.h"


void misc::AutoCrouch(CUserCmd* cmd)
{
	if (fakelag::get().condition)
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (m_gamerules()->m_bIsValveDS())
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (!key_binds::get().get_key_bind_state(20))
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (m_gamerules()->m_bIsValveDS())
		return;

	cmd->m_buttons |= IN_BULLRUSH;

	if (!g_ctx.globals.fakeducking && m_clientstate()->iChokedCommands != 7)
		return;

	if (m_clientstate()->iChokedCommands >= 7)
		cmd->m_buttons |= IN_DUCK;
	else
		cmd->m_buttons &= ~IN_DUCK;

	g_ctx.globals.fakeducking = true;
}

void misc::SlideWalk(CUserCmd* cmd)
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;

	if (g_ctx.local()->get_move_type() == MOVETYPE_LADDER)
		return;

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
		return;

	if (antiaim::get().condition(cmd, true) && config_system.g_cfg.misc.slidewalk)
	{
		if (cmd->m_forwardmove > 0.0f)
		{
			cmd->m_buttons |= IN_BACK;
			cmd->m_buttons &= ~IN_FORWARD;
		}
		else if (cmd->m_forwardmove < 0.0f)
		{
			cmd->m_buttons |= IN_FORWARD;
			cmd->m_buttons &= ~IN_BACK;
		}

		if (cmd->m_sidemove > 0.0f)
		{
			cmd->m_buttons |= IN_MOVELEFT;
			cmd->m_buttons &= ~IN_MOVERIGHT;
		}
		else if (cmd->m_sidemove < 0.0f)
		{
			cmd->m_buttons |= IN_MOVERIGHT;
			cmd->m_buttons &= ~IN_MOVELEFT;
		}
	}
	else
	{
		auto buttons = cmd->m_buttons & ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD);

		if (config_system.g_cfg.misc.slidewalk)
		{
			if (cmd->m_forwardmove <= 0.0f)
				buttons |= IN_BACK;
			else
				buttons |= IN_FORWARD;

			if (cmd->m_sidemove > 0.0f)
				goto LABEL_15;
			else if (cmd->m_sidemove >= 0.0f)
				goto LABEL_18;

			goto LABEL_17;
		}
		else
			goto LABEL_18;

		if (cmd->m_forwardmove <= 0.0f) //-V779
			buttons |= IN_FORWARD;
		else
			buttons |= IN_BACK;

		if (cmd->m_sidemove > 0.0f)
		{
		LABEL_17:
			buttons |= IN_MOVELEFT;
			goto LABEL_18;
		}

		if (cmd->m_sidemove < 0.0f)
			LABEL_15:

		buttons |= IN_MOVERIGHT;

	LABEL_18:
		cmd->m_buttons = buttons;
	}
}

void misc::automatic_peek(CUserCmd* cmd, float wish_yaw)
{
	if (!g_ctx.globals.weapon->is_non_aim() && key_binds::get().get_key_bind_state(18))
	{
		if (g_ctx.globals.start_position.IsZero())
		{
			g_ctx.globals.start_position = g_ctx.local()->GetAbsOrigin();

			if (!(engineprediction::get().backup_data.flags & FL_ONGROUND))
			{
				Ray_t ray;
				CTraceFilterWorldAndPropsOnly filter;
				CGameTrace trace;

				ray.Init(g_ctx.globals.start_position, g_ctx.globals.start_position - Vector(0.0f, 0.0f, 1000.0f));
				m_trace()->TraceRay(ray, MASK_SOLID, &filter, &trace);

				if (trace.fraction < 1.0f)
					g_ctx.globals.start_position = trace.endpos + Vector(0.0f, 0.0f, 2.0f);
			}
		}
		else
		{
			auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (cmd->m_buttons & IN_ATTACK || cmd->m_buttons & IN_ATTACK2);

			if (cmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
				g_ctx.globals.fired_shot = true;

			if (g_ctx.globals.fired_shot)
			{
				auto current_position = g_ctx.local()->GetAbsOrigin();
				auto difference = current_position - g_ctx.globals.start_position;

				if (difference.Length2D() > 5.0f)
				{
					auto velocity = Vector(difference.x * cos(wish_yaw / 180.0f * M_PI) + difference.y * sin(wish_yaw / 180.0f * M_PI), difference.y * cos(wish_yaw / 180.0f * M_PI) - difference.x * sin(wish_yaw / 180.0f * M_PI), difference.z);

					cmd->m_forwardmove = -velocity.x * 20.0f;
					cmd->m_sidemove = velocity.y * 20.0f;
				}
				else
				{
					g_ctx.globals.fired_shot = false;
					g_ctx.globals.start_position.Zero();
				}
			}
		}
	}
	else
	{
		g_ctx.globals.fired_shot = false;
		g_ctx.globals.start_position.Zero();
	}
}

void misc::ViewModel()
{
	if (!config_system.g_cfg.esp.viewmodel)
		return;
	if (config_system.g_cfg.esp.viewmodel_fov)
	{
		auto viewFOV = (float)config_system.g_cfg.esp.viewmodel_fov + 68.0f;
		static auto viewFOVcvar = m_cvar()->FindVar(crypt_str("viewmodel_fov"));

		if (viewFOVcvar->GetFloat() != viewFOV) //-V550
		{
			*(float*)((DWORD)&viewFOVcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewFOVcvar->SetValue(viewFOV);
		}
	}

	if (config_system.g_cfg.esp.viewmodel_x)
	{
		auto viewX = (float)config_system.g_cfg.esp.viewmodel_x / 2.0f;
		static auto viewXcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_x")); //-V807

		if (viewXcvar->GetFloat() != viewX) //-V550
		{
			*(float*)((DWORD)&viewXcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewXcvar->SetValue(viewX);
		}
	}

	if (config_system.g_cfg.esp.viewmodel_y)
	{
		auto viewY = (float)config_system.g_cfg.esp.viewmodel_y / 2.0f;
		static auto viewYcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_y"));

		if (viewYcvar->GetFloat() != viewY) //-V550
		{
			*(float*)((DWORD)&viewYcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewYcvar->SetValue(viewY);
		}
	}

	if (config_system.g_cfg.esp.viewmodel_z)
	{
		auto viewZ = (float)config_system.g_cfg.esp.viewmodel_z / 2.0f;
		static auto viewZcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_z"));

		if (viewZcvar->GetFloat() != viewZ) //-V550
		{
			*(float*)((DWORD)&viewZcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewZcvar->SetValue(viewZ);
		}
	}
}

#include "../../Render.h"

void misc::PovArrows(player_t* e)
{
	float width = 10.f;
	Vector viewangles;
	m_engine()->GetViewAngles(viewangles);

	auto angle = viewangles.y - math::calculate_angle(g_ctx.globals.eye_pos, e->GetAbsOrigin()).y - 90;

	int w, h;
	m_engine()->GetScreenSize(w, h);

	auto clr = config_system.g_cfg.player.arrows_color;
	clr.SetAlpha(config_system.g_cfg.player.arrows_color.a() / 2.f);


	g_Render->arc(w / 2, h / 2, 106, angle - width, angle + width, config_system.g_cfg.player.arrows_color, 4.f);
	g_Render->arc(w / 2, h / 2, 100, angle - width, angle + width, clr, 1.5f);

}

void misc::NightmodeFix()
{
	static auto in_game = false;

	if (m_engine()->IsInGame() && !in_game)
	{
		in_game = true;

		g_ctx.globals.change_materials = true;
		worldesp::get().changed = true;

		static auto skybox = m_cvar()->FindVar(crypt_str("sv_skyname"));
		worldesp::get().backup_skybox = skybox->GetString();
		return;
	}
	else if (!m_engine()->IsInGame() && in_game)
		in_game = false;

	static auto player_enable = config_system.g_cfg.player.enable;

	if (player_enable != config_system.g_cfg.player.enable)
	{
		player_enable = config_system.g_cfg.player.enable;
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting = config_system.g_cfg.esp.nightmode;

	if (setting != config_system.g_cfg.esp.nightmode)
	{
		setting = config_system.g_cfg.esp.nightmode;
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting_world = config_system.g_cfg.esp.worlds_color;

	if (setting_world != config_system.g_cfg.esp.worlds_color)
	{
		setting_world = config_system.g_cfg.esp.worlds_color;
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting_props = config_system.g_cfg.esp.pops_color;

	if (setting_props != config_system.g_cfg.esp.pops_color)
	{
		setting_props = config_system.g_cfg.esp.pops_color;
		g_ctx.globals.change_materials = true;
	}
}

void misc::desync_arrows()
{
	if (!g_ctx.local()->is_alive())
		return;

	if (!config_system.g_cfg.ragebot.enable)
		return;

	if (!config_system.g_cfg.antiaim.enable)
		return;

	if ((config_system.g_cfg.antiaim.manual_back.key <= KEY_NONE || config_system.g_cfg.antiaim.manual_back.key >= KEY_MAX) && (config_system.g_cfg.antiaim.manual_left.key <= KEY_NONE || config_system.g_cfg.antiaim.manual_left.key >= KEY_MAX) && (config_system.g_cfg.antiaim.manual_right.key <= KEY_NONE || config_system.g_cfg.antiaim.manual_right.key >= KEY_MAX))
		antiaim::get().manual_side = SIDE_NONE;

	if (!config_system.g_cfg.antiaim.flip_indicator)
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	static auto alpha = 1.0f;
	static auto switch_alpha = false;

	if (alpha <= 0.0f || alpha >= 1.0f)
		switch_alpha = !switch_alpha;

	alpha += switch_alpha ? 2.0f * m_globals()->m_frametime : -2.0f * m_globals()->m_frametime;
	alpha = math::clamp(alpha, 0.0f, 1.0f);

	auto color = config_system.g_cfg.antiaim.flip_indicator_color;
	color.SetAlpha((int)(min(255.0f * alpha, color.a())));

	if (antiaim::get().manual_side == SIDE_BACK)
		render::get().triangle(Vector2D(width / 2, height / 2 + 80), Vector2D(width / 2 - 10, height / 2 + 60), Vector2D(width / 2 + 10, height / 2 + 60), color);
	else if (antiaim::get().manual_side == SIDE_LEFT)
		render::get().triangle(Vector2D(width / 2 - 55, height / 2 + 10), Vector2D(width / 2 - 75, height / 2), Vector2D(width / 2 - 55, height / 2 - 10), color);
	else if (antiaim::get().manual_side == SIDE_RIGHT)
		render::get().triangle(Vector2D(width / 2 + 55, height / 2 - 10), Vector2D(width / 2 + 75, height / 2), Vector2D(width / 2 + 55, height / 2 + 10), color);
}

void misc::aimbot_hitboxes()
{
	if (!config_system.g_cfg.player.enable)
		return;


}

void misc::rank_reveal()
{
	if (!config_system.g_cfg.misc.rank_reveal)
		return;

	using RankReveal_t = bool(__cdecl*)(int*);
	static auto Fn = (RankReveal_t)(util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 51 A1 ? ? ? ? 85 C0 75 37")));

	int array[3] =
	{
		0,
		0,
		0
	};

	Fn(array);
}

void FX_Tesla(CTeslaInfo& pInfo)
{
	using FX_TeslaFn = void(__thiscall*)(CTeslaInfo&);
	static FX_TeslaFn pEffects = (FX_TeslaFn)util::FindSignature("client.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 8B 47 18");
	if (!pEffects)
		return;

	pEffects(pInfo);
}

void misc::KillEffect(IGameEvent* pEvent)
{
	player_t* pEntity = (player_t*)m_entitylist()->GetClientEntity(m_engine()->GetPlayerForUserID(pEvent->GetInt(("userid"))));
	player_t* pAttacker = (player_t*)m_entitylist()->GetClientEntity(m_engine()->GetPlayerForUserID(pEvent->GetInt(("attacker"))));

	if (!config_system.g_cfg.misc.lightingonshot)
		return;

	if (pEntity == g_ctx.local())
		return;

	if (pEntity->EntIndex() <= 0 || pEntity->EntIndex() > 64)
		return;

	if (!pAttacker || !pEntity)
		return;

	if (pAttacker != g_ctx.local())
		return;

	CTeslaInfo teslaInfo;
	teslaInfo.m_flBeamWidth = 10.f;
	teslaInfo.m_flRadius = 500.f;
	teslaInfo.m_nEntIndex = pEntity->EntIndex();
	teslaInfo.m_vColor.Init(1.f, 1.f, 1.f);
	teslaInfo.m_vPos = pEntity->hitbox_position(8);
	teslaInfo.m_flTimeVisible = 0.75f;
	teslaInfo.m_nBeams = 12;
	teslaInfo.m_pszSpriteName = "sprites/physbeam.vmt";

	FX_Tesla(teslaInfo);
}

//void draw_filled_rect(float x, float y , float z, float w, float h) {
//	const Vertex_t vertices[] = {
//		 Vertex_t(x, y, Color()),
//		 Vertex_t(x + w - 1, y,          rect.color),
//		 Vertex_t(x,         y + h - 1, rect.color),
//		 Vertex_t(x + w - 1, y + h - 1, rect.color)
//	};
//
//	hooks::device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
//	hooks::device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(Vertex_t));
//}

#include "../../ImGuiConnect.h"
#include "../menu.h"
void misc::spectators_list()
{
	if (config_system.g_cfg.misc.spectatos)
	{

		int specs = 0;
		std::string spect = "";

		for (int i = 1; i < m_globals()->m_maxclients; i++)
		{
			auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

			if (!e)
				continue;

			if (e->is_alive())
				continue;

			if (e->IsDormant())
				continue;

			if (e->m_hObserverTarget().Get() != g_ctx.local())
				continue;

			player_info_t player_info;
			m_engine()->GetPlayerInfo(i, &player_info);

			spect += player_info.szName;
			spect += "\n";
			specs++;
		}


		auto size = ImGui::CalcTextSize(spect.c_str()); // 16 на размер шрифта меняете и Menu::Get().globalFont на ваш шрифт

		ImGui::SetNextWindowSize(ImVec2(200, size.y + 60.f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.f);

		ImVec2 MenuP;
		ImGui::SetNextWindowSize(ImVec2(200, 15));
		ImGui::Begin("Specs", &config_system.g_cfg.misc.spectators_list, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);
		MenuP = ImGui::GetCurrentWindow()->Pos;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetWindowWidth() / 2) - 95);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
		ImGui::SameLine(80);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
		ImGui::Text("Spectators");
		auto pos = ImGui::GetWindowPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGui::End();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::SetNextWindowPos({ MenuP.x, MenuP.y + 35 });
		if (ImGui::Begin("Spectator List", &config_system.g_cfg.misc.spectators_list, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
			ImGui::Text(spect.c_str());
			
			ImGui::End();
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}

bool misc::double_tap(CUserCmd* m_pcmd)
{
	double_tap_enabled = true;

	static auto recharge_double_tap = false;
	static auto last_double_tap = 0;

	static weapon_t* last_weapon;

	if (recharge_double_tap)
	{
		recharge_double_tap = false;
		recharging_double_tap = true;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;

		return false;
	}

	if (recharging_double_tap)
	{
		auto recharge_time = g_ctx.globals.weapon->can_double_tap() ? TIME_TO_TICKS(0.75f) : TIME_TO_TICKS(1.5f);

		if (!aim::get().should_stop && fabs(g_ctx.globals.fixed_tickbase - last_double_tap) > recharge_time)
		{
			last_double_tap = 0;

			recharging_double_tap = false;
			double_tap_key = true;
		}
		else if (m_pcmd->m_buttons & IN_ATTACK)
			last_double_tap = g_ctx.globals.fixed_tickbase;

	}

	if (!config_system.g_cfg.ragebot.enable || !config_system.g_cfg.ragebot.double_tap)
	{
		double_tap_enabled = false;
		double_tap_key = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}


	if (config_system.g_cfg.ragebot.double_tap_key.key <= KEY_NONE || config_system.g_cfg.ragebot.double_tap_key.key >= KEY_MAX)
	{
		double_tap_enabled = false;
		double_tap_key = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (double_tap_key && config_system.g_cfg.ragebot.double_tap_key.key != config_system.g_cfg.antiaim.hide_shots_key.key)
		hide_shots_key = false;

	if (!double_tap_key)
	{
		double_tap_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (g_ctx.local()->m_bGunGameImmunity() || g_ctx.local()->m_fFlags() & FL_FROZEN) //-V807
	{
		double_tap_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (m_gamerules()->m_bIsValveDS())
	{
		double_tap_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (g_ctx.globals.fakeducking || key_binds::get().get_key_bind_state(20))
	{
		double_tap_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (antiaim::get().freeze_check)
		return true;

	//if (old_fd != key_binds::get().get_key_bind_state(20))
	//{
	//	if (g_ctx.globals.weapon->can_double_tap()) {
	//		old_fd = key_binds::get().get_key_bind_state(20);
	//		recharge_double_tap = true;
	//	}
	//}

	auto max_tickbase_shift = g_ctx.local()->m_hActiveWeapon()->get_max_tickbase_shift();

	if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_SSG08 && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER && g_ctx.send_packet && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2 && g_ctx.globals.weapon->is_knife())) //-V648
	{
		auto next_command_number = m_pcmd->m_command_number + 1;
		auto user_cmd = m_input()->GetUserCmd(next_command_number);

		memcpy(user_cmd, m_pcmd, sizeof(CUserCmd)); //-V598
		user_cmd->m_command_number = next_command_number;

		util::copy_command(user_cmd, max_tickbase_shift);

		if (g_ctx.globals.aimbot_working)
		{
			g_ctx.globals.double_tap_aim = true;
			g_ctx.globals.double_tap_aim_check = true;
		}

		recharge_double_tap = true;
		double_tap_enabled = false;
		double_tap_key = false;

		last_double_tap = g_ctx.globals.fixed_tickbase;
	}
	else if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
		g_ctx.globals.tickbase_shift = max_tickbase_shift;

	return true;
}

void misc::hide_shots(CUserCmd* m_pcmd, bool should_work)
{
	hide_shots_enabled = true;

	if (!config_system.g_cfg.ragebot.enable)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;

		if (should_work)
		{
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.next_tickbase_shift = 0;
		}

		return;
	}

	if (!config_system.g_cfg.antiaim.hide_shots)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;

		if (should_work)
		{
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.next_tickbase_shift = 0;
		}

		return;
	}

	if (config_system.g_cfg.antiaim.hide_shots_key.key <= KEY_NONE || config_system.g_cfg.antiaim.hide_shots_key.key >= KEY_MAX)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;

		if (should_work)
		{
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.next_tickbase_shift = 0;
		}

		return;
	}

	if (!should_work && double_tap_key)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;
		return;
	}

	if (!hide_shots_key)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	double_tap_key = false;

	if (g_ctx.local()->m_bGunGameImmunity() || g_ctx.local()->m_fFlags() & FL_FROZEN)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	if (g_ctx.globals.fakeducking)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	if (antiaim::get().freeze_check)
		return;

	g_ctx.globals.next_tickbase_shift = m_gamerules()->m_bIsValveDS() ? 6 : 9;

	auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);
	auto weapon_shoot = m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || m_pcmd->m_buttons & IN_ATTACK2 && g_ctx.globals.weapon->is_knife() || revolver_shoot;

	if (g_ctx.send_packet && !g_ctx.globals.weapon->is_grenade() && weapon_shoot)
		g_ctx.globals.tickbase_shift = g_ctx.globals.next_tickbase_shift;
}