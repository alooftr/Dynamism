// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "player_esp.h"
#include "..\misc\misc.h"
#include "..\ragebot\aim.h"
#include "dormant_esp.h"
#include "../../Render.h"


ImVec2 ChangeSidePosText(CEsp::MoveStruct* Curt, ImVec2 Size, ImVec2 BoxPos, int boxSizeX, int boxSizeY)
{
	switch (Curt->Side)
	{
	case CEsp::LEFT_S: return ImVec2(BoxPos.x - Size.x - Curt->CorrectPadding.x, BoxPos.y + Curt->CorrectPadding.y);
	case CEsp::RIGHT_S:return ImVec2(BoxPos.x + boxSizeX + Curt->CorrectPadding.x, BoxPos.y + Curt->CorrectPadding.y);
	case CEsp::TOP_S:  return ImVec2(BoxPos.x + (boxSizeX / 2) - (Size.x / 2), BoxPos.y - Size.y - Curt->CorrectPadding.y);
	case CEsp::DOWN_S: return ImVec2(BoxPos.x + (boxSizeX / 2) - (Size.x / 2), BoxPos.y + boxSizeY + Curt->CorrectPadding.y);
	case CEsp::LEFT_TOP_S:  return ImVec2(BoxPos.x, BoxPos.y - Size.y - Curt->CorrectPadding.y);
	case CEsp::RIGHT_TOP_S: return ImVec2(BoxPos.x + boxSizeX - Size.x, BoxPos.y - Size.y - Curt->CorrectPadding.y);
	case CEsp::LEFT_DOWN_S: return ImVec2(BoxPos.x, BoxPos.y + boxSizeY + Curt->CorrectPadding.y);
	case CEsp::RIGHT_DOWN_S:return ImVec2(BoxPos.x + boxSizeX - Size.x, BoxPos.y + boxSizeY + Curt->CorrectPadding.y);
	case CEsp::CENTER_LEFT_S:    return ImVec2(BoxPos.x - Size.x - Curt->CorrectPadding.x, BoxPos.y + (boxSizeY / 2 + Curt->CorrectPadding.y));
	case CEsp::CENTER_RIGHT_S:   return ImVec2(BoxPos.x + boxSizeX + Curt->CorrectPadding.x, BoxPos.y + (boxSizeY / 2) + Curt->CorrectPadding.y);
	case CEsp::DOWN_LEFT_DOWN_S: return ImVec2(BoxPos.x - Size.x - Curt->CorrectPadding.x, BoxPos.y + boxSizeY - Size.y + Curt->CorrectPadding.y);
	case CEsp::DOWN_RIGHT_DOWN_S:return ImVec2(BoxPos.x + boxSizeX + Curt->CorrectPadding.x, BoxPos.y + boxSizeY - Size.y + Curt->CorrectPadding.y);
	default: return ImVec2(0, 0);
	}
};

class RadarPlayer_t
{
public:
	Vector pos; //0x0000
	Vector angle; //0x000C
	Vector spotted_map_angle_related; //0x0018
	DWORD tab_related; //0x0024
	char pad_0x0028[0xC]; //0x0028
	float spotted_time; //0x0034
	float spotted_fraction; //0x0038
	float time; //0x003C
	char pad_0x0040[0x4]; //0x0040
	__int32 player_index; //0x0044
	__int32 entity_index; //0x0048
	char pad_0x004C[0x4]; //0x004C
	__int32 health; //0x0050
	char name[32]; //0x785888
	char pad_0x0074[0x75]; //0x0074
	unsigned char spotted; //0x00E9
	char pad_0x00EA[0x8A]; //0x00EA
};

class CCSGO_HudRadar
{
public:
	char pad_0x0000[0x14C];
	RadarPlayer_t radar_info[65];
};

void playeresp::paint_traverse()
{
	static auto alpha = 1.0f;
	c_dormant_esp::get().start();

	if (config_system.g_cfg.player.arrows && g_ctx.local()->is_alive())
	{
		static auto switch_alpha = false;

		if (alpha <= 0.0f || alpha >= 1.0f)
			switch_alpha = !switch_alpha;

		alpha += switch_alpha ? 2.0f * m_globals()->m_frametime : -2.0f * m_globals()->m_frametime;
		alpha = math::clamp(alpha, 0.0f, 1.0f);
	}

	static auto FindHudElement = (DWORD(__thiscall*)(void*, const char*))util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	static auto hud_ptr = *(DWORD**)(util::FindSignature(crypt_str("client.dll"), crypt_str("81 25 ? ? ? ? ? ? ? ? 8B 01")) + 0x2);

	auto radar_base = FindHudElement(hud_ptr, "CCSGO_HudRadar");
	auto hud_radar = (CCSGO_HudRadar*)(radar_base - 0x14);

	for (auto i = 1; i < m_globals()->m_maxclients; i++) //-V807
	{
		auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

		if (!e->valid(false, false))
			continue;

		type = ENEMY;

		if (e == g_ctx.local())
			type = LOCAL;
		else if (e->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			type = TEAM;

		if (type == LOCAL && !m_input()->m_fCameraInThirdPerson)
			continue;



		auto valid_dormant = false;
		auto backup_flags = e->m_fFlags();
		auto backup_origin = e->GetAbsOrigin();

		if (e->IsDormant())
			valid_dormant = c_dormant_esp::get().adjust_sound(e);
		else
		{
			health[i] = e->m_iHealth();
			c_dormant_esp::get().m_cSoundPlayers[i].reset(true, e->GetAbsOrigin(), e->m_fFlags());
		}

		if (radar_base && hud_radar && e->IsDormant() && e->m_iTeamNum() != g_ctx.local()->m_iTeamNum() && e->m_bSpotted())
			health[i] = hud_radar->radar_info[i].health;

		if (!health[i])
		{
			if (e->IsDormant())
			{
				e->m_fFlags() = backup_flags;
				e->set_abs_origin(backup_origin);
			}

			continue;
		}

		auto fast = 2.5f * m_globals()->m_frametime; //-V807
		auto slow = 0.25f * m_globals()->m_frametime;

		if (e->IsDormant())
		{
			auto origin = e->GetAbsOrigin();

			if (origin.IsZero())
				esp_alpha_fade[i] = 0.0f;
			else if (!valid_dormant && esp_alpha_fade[i] > 0.0f)
				esp_alpha_fade[i] -= slow;
			else if (valid_dormant && esp_alpha_fade[i] < 1.0f)
				esp_alpha_fade[i] += fast;
		}
		else if (esp_alpha_fade[i] < 1.0f)
			esp_alpha_fade[i] += fast;

		esp_alpha_fade[i] = math::clamp(esp_alpha_fade[i], 0.0f, 1.0f);

		if (config_system.g_cfg.player.type[type].skeleton && !e->IsDormant())
		{
			auto color = config_system.g_cfg.player.type[type].skeleton_color;
			color.SetAlpha(min(255.0f * esp_alpha_fade[i], color.a()));


			draw_skeleton(e, color, e->m_CachedBoneData().Base());
		}

		Box box;

		if (util::get_bbox(e, box, true))
		{
			if (e->GetRenderOrigin() == Vector(0, 0, 0))
				continue;

			draw_box(e, box);
			draw_name(e, box);



			draw_health(e, box);
			draw_ammobar(e, box);
			draw_weapon(e, box);
			if (!e->IsDormant())
				draw_flags(e, box);
		}

		if (type == ENEMY || type == TEAM)
		{
			draw_lines(e);

			if (type == ENEMY)
			{
				if (config_system.g_cfg.player.arrows && g_ctx.local()->is_alive())
					misc::get().PovArrows(e);
			}
		}

		if (e->IsDormant())
		{
			e->m_fFlags() = backup_flags;
			e->set_abs_origin(backup_origin);
		}
	}
}

void playeresp::draw_skeleton(player_t* e, Color color, matrix3x4_t matrix[MAXSTUDIOBONES])
{
	auto model = e->GetModel();

	if (!model)
		return;

	auto studio_model = m_modelinfo()->GetStudioModel(model);

	if (!studio_model)
		return;

	auto get_bone_position = [&](int bone) -> Vector
	{
		return Vector(matrix[bone][0][3], matrix[bone][1][3], matrix[bone][2][3]);
	};

	auto upper_direction = get_bone_position(7) - get_bone_position(6);
	auto breast_bone = get_bone_position(6) + upper_direction * 0.5f;

	for (auto i = 0; i < studio_model->numbones; i++)
	{
		auto bone = studio_model->pBone(i);

		if (!bone)
			continue;

		if (bone->parent == -1)
			continue;

		if (!(bone->flags & BONE_USED_BY_HITBOX))
			continue;

		auto child = get_bone_position(i);
		auto parent = get_bone_position(bone->parent);

		auto delta_child = child - breast_bone;
		auto delta_parent = parent - breast_bone;

		if (delta_parent.Length() < 9.0f && delta_child.Length() < 9.0f)
			parent = breast_bone;

		if (i == 5)
			child = breast_bone;

		if (fabs(delta_child.z) < 5.0f && delta_parent.Length() < 5.0f && delta_child.Length() < 5.0f || i == 6)
			continue;

		auto schild = ZERO;
		auto sparent = ZERO;

		if (math::WorldToScreen(child, schild) && math::WorldToScreen(parent, sparent))
			g_Render->DrawLine(schild.x, schild.y, sparent.x, sparent.y, color);
	}
}

void DrawLine(float x1, float y1, float x2, float y2, Color color, float size = 1.f) {


	if (size == 1.f)
	{

		g_Render->DrawLine(x1, y1, x2, y2, Color(color.r(), color.g(), color.b(), color.a()));
	}
	else
	{
		g_Render->FilledRect(x1 - (size / 2.f), y1 - (size / 2.f), x2 + (size / 2.f), y2 + (size / 2.f), Color(color.r(), color.g(), color.b(), color.a()));
	}

}

void Corners(float x1, float y1, float x2, float y2, Color clr, float edge_size, float size) {
	if (fabs(x1 - x2) < (edge_size * 2)) {
		//x2 = x1 + fabs(x1 - x2);
		edge_size = fabs(x1 - x2) / 4.f;
	}

	DrawLine(x1, y1, x1, y1 + edge_size + (0.5f * edge_size), clr, size);
	DrawLine(x2, y1, x2, y1 + edge_size + (0.5f * edge_size), clr, size);
	DrawLine(x1, y2, x1, y2 - edge_size - (0.5f * edge_size), clr, size);
	DrawLine(x2, y2, x2, y2 - edge_size - (0.5f * edge_size), clr, size);
	DrawLine(x1, y1, x1 + edge_size, y1, clr, size);
	DrawLine(x2, y1, x2 - edge_size, y1, clr, size);
	DrawLine(x1, y2, x1 + edge_size, y2, clr, size);
	DrawLine(x2, y2, x2 - edge_size, y2, clr, size);
}

void playeresp::draw_box(player_t* m_entity, const Box& box)
{
	if (!config_system.g_cfg.player.type[type].box)
		return;

	auto alpha = 255.0f * esp_alpha_fade[m_entity->EntIndex()];
	auto outline_alpha = (int)(alpha * 0.6f);

	Color outline_color
	{
		0,
		0,
		0,
		outline_alpha
	};

	auto color = m_entity->IsDormant() ? Color(200, 200, 200, 130) : config_system.g_cfg.player.type[type].box_color;
	color.SetAlpha(min(alpha, color.a()));
	float edge_size = 25.f;
	switch (config_system.g_cfg.player.type[type].box_type)
	{
	case 0:
		g_Render->Rect(box.x, box.y, box.w, box.h, color);
		g_Render->Rect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(0, 0, 0, 80));
		g_Render->Rect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(0, 0, 0, 80));
		break;
	case 1:


		Corners(box.x, box.y, box.j, box.t, color, edge_size, 1);
		Corners(box.x + 1, box.y + 1, box.j - 1, box.t - 1, Color(0, 0, 0, 80), edge_size, 1);
		Corners(box.x - 1, box.y - 1, box.j + 1, box.t + 1, Color(0, 0, 0, 80), edge_size, 1);
		break;
	case 2:
		g_Render->Rect(box.x, box.y, box.w, box.h, color, config_system.g_cfg.player.type[type].rouding);
		g_Render->Rect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(0, 0, 0, 80), config_system.g_cfg.player.type[type].rouding);
		g_Render->Rect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(0, 0, 0, 80), config_system.g_cfg.player.type[type].rouding);
		break;
	}
	if (config_system.g_cfg.player.type[type].filledbox && !m_entity->IsDormant())
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		auto color = config_system.g_cfg.player.type[type].filledboxcolor;
		if (config_system.g_cfg.player.type[type].box_type < 2)
		{
			window->DrawList->AddRectFilledMultiColor(ImVec2(box.x, box.y), ImVec2(box.j, box.t),
				Color(color.r(), color.g(), color.b(), 0).u32(), Color(color.r(), color.g(), color.b(), 0).u32(), color.u32(), color.u32());
		}
		if (config_system.g_cfg.player.type[type].box_type == 2)
		{
			window->DrawList->AddRectFilled(ImVec2(box.x, box.y), ImVec2(box.j, box.t), color.u32(), config_system.g_cfg.player.type[type].rouding);
		}
	}

}

void playeresp::draw_health(player_t* m_entity, const Box& box)
{
	static CEsp::MoveStruct* MV_Hp = &GP_Esp->MVItemList[CEsp::MI_HEALTH_BAR];

	if (!config_system.g_cfg.player.type[type].health)
		return;

	constexpr float SPEED_FREQ = 255 / 1.0f;
	int hp = m_entity->m_iHealth();

	if (hp > 100)
		hp = 100;


	int red = 255 - (hp * 2.55);
	int green = hp * 2.55;

	auto alpha = (int)(255.0f * esp_alpha_fade[m_entity->EntIndex()]);

	auto hp_color = m_entity->IsDormant() ? Color(200, 200, 200, 130) : Color(red, green, 0);
	hp_color.SetAlpha(min(alpha, hp_color.a()));

	static float prev_player_hp[65];

	if (prev_player_hp[m_entity->EntIndex()] > hp)
		prev_player_hp[m_entity->EntIndex()] -= SPEED_FREQ * m_globals()->m_frametime;
	else
		prev_player_hp[m_entity->EntIndex()] = hp;

	int hp_percent = box.h - (int)((box.h * prev_player_hp[m_entity->EntIndex()]) / 100);
	int hp_percentw = box.w - (int)((box.w * prev_player_hp[m_entity->EntIndex()]) / 100);

	int healthpos_X = 0;
	int healthpos_Y = 0;
	switch (MV_Hp->Side)
	{
	case CEsp::Sides::LEFT_S:
		healthpos_X = box.x - (box.w / MV_Hp->CorrectOfst.x) - 2 - MV_Hp->CorrectPadding.x;
		healthpos_Y = box.y - (box.h / MV_Hp->CorrectOfst.y);
		break;
	case CEsp::Sides::RIGHT_S:
		healthpos_X = box.x - (box.w / MV_Hp->CorrectOfst.x) + MV_Hp->CorrectPadding.x;
		healthpos_Y = box.y - (box.h / MV_Hp->CorrectOfst.y);
		break;
	case CEsp::Sides::TOP_S:
		healthpos_X = box.x - (box.w / MV_Hp->CorrectOfst.x);
		healthpos_Y = box.y - (box.h / MV_Hp->CorrectOfst.y) - 2 - MV_Hp->CorrectPadding.y;
		break;
	case CEsp::Sides::DOWN_S:
		healthpos_X = box.x - (box.w / MV_Hp->CorrectOfst.x);
		healthpos_Y = box.y - (box.h / MV_Hp->CorrectOfst.y) - 2 - MV_Hp->CorrectPadding.y;
		break;
	}

	bool vertical = (MV_Hp->Side == CEsp::Sides::LEFT_S || MV_Hp->Side == CEsp::Sides::RIGHT_S);
	if (vertical) {
		g_Render->FilledRect(healthpos_X - 1, healthpos_Y - 1, 4, box.h, Color(0, 0, 0, 100));
		g_Render->FilledRect(healthpos_X, healthpos_Y + hp_percent, 2, box.h - hp_percent, config_system.g_cfg.player.type[type].custom_health_color ? config_system.g_cfg.player.type[type].health_color : hp_color);
	}
	else
	{
		g_Render->FilledRect(healthpos_X - 1, healthpos_Y - 1, box.w, 4, Color(0, 0, 0, 100));
		g_Render->FilledRect(healthpos_X + hp_percentw, healthpos_Y, box.w - hp_percentw, 2, config_system.g_cfg.player.type[type].custom_health_color ? config_system.g_cfg.player.type[type].health_color : hp_color);
	}
}

void playeresp::draw_ammobar(player_t* m_entity, const Box& box)
{
	static CEsp::MoveStruct* MV_Armor = &GP_Esp->MVItemList[CEsp::MI_ARMOR_BAR];

	if (!m_entity->is_alive())
		return;

	if (!config_system.g_cfg.player.type[type].ammo)
		return;

	auto weapon = m_entity->m_hActiveWeapon().Get();

	if (weapon->is_non_aim())
		return;

	auto ammo = weapon->m_iClip1();

	auto alpha = (int)(255.0f * esp_alpha_fade[m_entity->EntIndex()]);

	auto color = m_entity->IsDormant() ? Color(200, 200, 200, 130) : config_system.g_cfg.player.type[type].ammobar_color;

	color.SetAlpha(min(alpha, color.a()));

	Box n_box =
	{
		box.x,
		box.y + box.h + 3,
		box.w + 2,
		2
	};

	auto weapon_info = m_weaponsystem()->GetWeaponData(m_entity->m_hActiveWeapon()->m_iItemDefinitionIndex());

	if (!weapon_info)
		return;

	auto bar_width = ammo * box.w / weapon_info->iMaxClip1;
	auto reloading = false;

	auto animlayer = m_entity->get_animlayers()[1];
	int health_tik = ammo * box.h / weapon_info->iMaxClip1;

	if (animlayer.m_nSequence)
	{
		auto activity = m_entity->sequence_activity(animlayer.m_nSequence);

		reloading = activity == ACT_CSGO_RELOAD && animlayer.m_flWeight;

		if (reloading && animlayer.m_flCycle < 1.0f)
		{
			bar_width = animlayer.m_flCycle * box.w;
			health_tik = animlayer.m_flCycle * box.h;
		}
	}

	int healthpos_X = 0;
	int healthpos_Y = 0;
	switch (MV_Armor->Side)
	{
	case CEsp::Sides::LEFT_S:
		healthpos_X = box.x - (box.w / MV_Armor->CorrectOfst.x) - 2 - MV_Armor->CorrectPadding.x;
		healthpos_Y = box.y - (box.h / MV_Armor->CorrectOfst.y);
		break;
	case CEsp::Sides::RIGHT_S:
		healthpos_X = box.x - (box.w / MV_Armor->CorrectOfst.x) + MV_Armor->CorrectPadding.x;
		healthpos_Y = box.y - (box.h / MV_Armor->CorrectOfst.y);
		break;
	case CEsp::Sides::TOP_S:
		healthpos_X = box.x - (box.w / MV_Armor->CorrectOfst.x);
		healthpos_Y = box.y - (box.h / MV_Armor->CorrectOfst.y) - 2 - MV_Armor->CorrectPadding.y;
		break;
	case CEsp::Sides::DOWN_S:
		healthpos_X = box.x - (box.w / MV_Armor->CorrectOfst.x);
		healthpos_Y = box.y - (box.h / MV_Armor->CorrectOfst.y) - 2 - MV_Armor->CorrectPadding.y;
		break;
	}



	bool vertical = (MV_Armor->Side == CEsp::Sides::LEFT_S || MV_Armor->Side == CEsp::Sides::RIGHT_S);
	if (vertical) {
		g_Render->FilledRect(healthpos_X - 1, healthpos_Y - 1, 4, box.h, Color(0, 0, 0, 100));
		g_Render->FilledRect(healthpos_X, healthpos_Y, 2, health_tik, color);
	}
	else {
		g_Render->FilledRect(healthpos_X - 1, healthpos_Y - 1, n_box.w, 4, Color(0, 0, 0, 100));
		g_Render->FilledRect(healthpos_X, healthpos_Y, bar_width, 2, color);
	}
}

#include "../menu.h"

void playeresp::draw_name(player_t* m_entity, const Box& box)
{

	static CEsp::MoveStruct* MV_Name = &GP_Esp->MVItemList[CEsp::MI_NAME_TEXT];

	if (!config_system.g_cfg.player.type[type].name)
		return;

	static auto sanitize = [](char* name) -> std::string
	{
		name[127] = '\0';

		std::string tmp(name);

		if (tmp.length() > 20)
		{
			tmp.erase(20, tmp.length() - 20);
			tmp.append("...");
		}

		return tmp;
	};

	player_info_t player_info;

	if (m_engine()->GetPlayerInfo(m_entity->EntIndex(), &player_info))
	{
		auto name = sanitize(player_info.szName);

		auto color = m_entity->IsDormant() ? Color(200, 200, 200, 130) : config_system.g_cfg.player.type[type].name_color;
		color.SetAlpha(min(255.0f * esp_alpha_fade[m_entity->EntIndex()], color.a()));

		ImVec2 TextPos = ChangeSidePosText(MV_Name,ImGui::CalcTextSize(name.c_str()), ImVec2(box.x, box.y), box.w, box.h);

		TextPos.x -= (box.w / MV_Name->CorrectOfst.x);
		TextPos.y -= (box.h / MV_Name->CorrectOfst.y);

		TextPos.x = int(TextPos.x);
		TextPos.y = int(TextPos.y);

		g_Render->DrawString(TextPos.x, TextPos.y, color, render2::none, c_menu::get().futura_small, name.c_str());
	}
}

void playeresp::draw_weapon(player_t* m_entity, const Box& box)
{
	if (!config_system.g_cfg.player.type[type].weapon_text && !config_system.g_cfg.player.type[type].weapon_icon)
		return;

	auto weapon = m_entity->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto color = m_entity->IsDormant() ? Color(200, 200, 200, 130) : config_system.g_cfg.player.type[type].weapon_color;
	color.SetAlpha(min(255.0f * esp_alpha_fade[m_entity->EntIndex()], color.a()));

	if (config_system.g_cfg.player.type[type].weapon_icon)
	{
		static CEsp::MoveStruct* MV_Weapon = &GP_Esp->MVItemList[CEsp::MI_WEAPON_TEXT];

		ImVec2 TextPos = ChangeSidePosText(MV_Weapon, ImGui::CalcTextSize(weapon->get_icon()), ImVec2(box.x, box.y), box.w, box.h);

		g_Render->DrawString((int)TextPos.x - (box.w / MV_Weapon->CorrectOfst.x),
			(int)TextPos.y - (box.h / MV_Weapon->CorrectOfst.y), color, render2::centered_x | render2::outline, c_menu::get().ico_menu, weapon->get_icon());
	}
}

void playeresp::draw_flags(player_t* e, const Box& box)
{
	auto weapon = e->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto _x = box.x + box.w + 3, _y = box.y - 3;

	if (config_system.g_cfg.player.type[type].FLAGS_MONEY)
	{
		auto color = Color(170, 250, 80);

		static CEsp::MoveStruct* MV_Money = &GP_Esp->MVItemList[CEsp::MI_MONEY_TEXT];

		ImVec2 TextPos = ChangeSidePosText(MV_Money, ImGui::CalcTextSize("%i$", std::to_string(e->m_iAccount()).c_str()), ImVec2(box.x, box.y), box.w, box.h);

		int size = config_system.g_cfg.player.type[type].health ? 45 : 40;
		g_Render->DrawString((int)TextPos.x - (box.w / MV_Money->CorrectOfst.x),
			(int)TextPos.y - (box.h / MV_Money->CorrectOfst.y), color, render2::none | render2::outline, c_menu::get().futura_small, "%i$", e->m_iAccount());
	}

	if (config_system.g_cfg.player.type[type].FLAGS_ARMOR)
	{
		static CEsp::MoveStruct* MV_Armor = &GP_Esp->MVItemList[CEsp::MI_HEALTH_TEXT];

		auto color = config_system.g_cfg.player.type[type].Armor;

		auto kevlar = e->m_ArmorValue() > 0;
		auto helmet = e->m_bHasHelmet();

		std::string text;

		if (helmet && kevlar)
			text = "HK";
		else if (kevlar)
			text = "K";

		ImVec2 TextPos = ChangeSidePosText(MV_Armor, ImGui::CalcTextSize(helmet && kevlar ? "HK" : "K"), ImVec2(box.x, box.y), box.w, box.h);

		if (kevlar)
		{
			g_Render->DrawString((int)TextPos.x - (box.w / MV_Armor->CorrectOfst.x),
				(int)TextPos.y - (box.h / MV_Armor->CorrectOfst.y), color, render2::none | render2::outline, c_menu::get().futura_small, text.c_str());
		}
	}

	if (config_system.g_cfg.player.type[type].FLAGS_KIT && e->m_bHasDefuser())
	{
		static CEsp::MoveStruct* MV_Defused = &GP_Esp->MVItemList[CEsp::MI_DEFUSING_TEXT];

		auto color = config_system.g_cfg.player.type[type].Defuse;
		ImVec2 TextPos = ChangeSidePosText(MV_Defused, ImGui::CalcTextSize("DEFUSE"), ImVec2(box.x, box.y), box.w, box.h);

		g_Render->DrawString((int)TextPos.x - (box.w / MV_Defused->CorrectOfst.x),
			(int)TextPos.y - (box.h / MV_Defused->CorrectOfst.y), color, render2::none | render2::outline, c_menu::get().futura_small, "DEFUSE");
	}

	if (config_system.g_cfg.player.type[type].FLAGS_SCOPED)
	{
		static CEsp::MoveStruct* MV_Scoped = &GP_Esp->MVItemList[CEsp::MI_SCOPE_TEXT];

		auto scoped = e->m_bIsScoped();

		if (e == g_ctx.local())
			scoped = g_ctx.globals.scoped;

		if (scoped)
		{
			ImVec2 TextPos = ChangeSidePosText(MV_Scoped, ImGui::CalcTextSize("SCOPED"), ImVec2(box.x, box.y), box.w, box.h);

			auto color = config_system.g_cfg.player.type[type].Scoped;

			g_Render->DrawString((int)TextPos.x - (box.w / MV_Scoped->CorrectOfst.x),
				(int)TextPos.y - (box.h / MV_Scoped->CorrectOfst.y), color, render2::none | render2::outline, c_menu::get().futura_small, "SCOPED");
		}
	}

	if (config_system.g_cfg.player.type[type].FLAGS_C4 && e->EntIndex() == g_ctx.globals.bomb_carrier)
	{
		auto color = config_system.g_cfg.player.type[type].BombCarrie;
		static CEsp::MoveStruct* MV_C4 = &GP_Esp->MVItemList[CEsp::MI_FLASHED_TEXT];

		ImVec2 TextPos = ChangeSidePosText(MV_C4, ImGui::CalcTextSize("BOMB"), ImVec2(box.x, box.y), box.w, box.h);


		g_Render->DrawString((int)TextPos.x - (box.w / MV_C4->CorrectOfst.x),
			(int)TextPos.y - (box.h / MV_C4->CorrectOfst.y), color, render2::none | render2::outline, c_menu::get().futura_small, "BOMB");
	}
}

void playeresp::draw_lines(player_t* e)
{
	if (!config_system.g_cfg.player.type[type].snap_lines)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	Vector angle;

	if (!math::WorldToScreen(e->GetAbsOrigin(), angle))
		return;

	auto color = config_system.g_cfg.player.type[type].snap_lines_color;

	g_Render->DrawLine(width / 2, height, angle.x, angle.y, color);
}

void playeresp::draw_multi_points(player_t* e)
{
	if (!config_system.g_cfg.ragebot.enable)
		return;

	if (!config_system.g_cfg.player.show_multi_points)
		return;

	if (!g_ctx.local()->is_alive()) //-V807
		return;

	if (g_ctx.local()->get_move_type() == MOVETYPE_NOCLIP)
		return;

	if (g_ctx.globals.current_weapon == -1)
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (weapon->is_non_aim())
		return;

	auto records = &player_records[e->EntIndex()]; //-V826

	if (records->empty())
		return;

	auto record = &records->front();

	if (!record->valid(false))
		return;

	std::vector <scan_point> points; //-V826
	auto hitboxes = aim::get().get_hitboxes(record);

	for (auto& hitbox : hitboxes)
	{
		auto current_points = aim::get().get_points(record, hitbox, false);

		for (auto& point : current_points)
			points.emplace_back(point);
	}

	for (auto& point : points)
	{
		if (points.empty())
			break;

		if (point.hitbox == HITBOX_HEAD)
			continue;

		for (auto it = points.begin(); it != points.end(); ++it)
		{
			if (point.point == it->point)
				continue;

			auto first_angle = math::calculate_angle(g_ctx.globals.eye_pos, point.point);
			auto second_angle = math::calculate_angle(g_ctx.globals.eye_pos, it->point);

			auto distance = g_ctx.globals.eye_pos.DistTo(point.point);
			auto fov = math::fast_sin(DEG2RAD(math::get_fov(first_angle, second_angle))) * distance;

			if (fov < 5.0f)
			{
				points.erase(it);
				break;
			}
		}
	}

	if (points.empty())
		return;


}


void playeresp::dlight() {

	for (auto i = 1; i < m_globals()->m_maxclients; i++) //-V807
	{
		auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

		if (!e->valid(false, false))
			continue;

		type = ENEMY;

		if (e == g_ctx.local())
			type = LOCAL;
		else if (e->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			type = TEAM;

		if (config_system.g_cfg.player.type[type].dLight)
		{

			Vector getorig = e->GetAbsOrigin();
			Vector getheadorig = g_ctx.globals.eye_pos;

			if (!e->IsDormant()) {

				dlight_t* elight = DLightT()->cl_alloc_elight(e->EntIndex());
				elight->color = config_system.g_cfg.player.type[type].dLight_color;
				elight->direction = getheadorig;
				elight->origin = getheadorig;
				elight->radius = 400.0f;		//200.0f
				elight->die_time = m_globals()->m_curtime + 0.1f;
				elight->decay = 50.0f;			//25.0f
				elight->key = e->EntIndex();

				dlight_t* dlight = DLightT()->cl_alloc_dlight(e->EntIndex());
				dlight->color = config_system.g_cfg.player.type[type].dLight_color;
				dlight->direction = getorig;
				dlight->origin = getorig;
				dlight->radius = config_system.g_cfg.player.type[type].dLightR;		//100.0f
				dlight->die_time = m_globals()->m_curtime + 0.1f;
				dlight->decay = config_system.g_cfg.player.type[type].dLightDecay;		//5.f
				dlight->key = e->EntIndex();
				dlight->style = config_system.g_cfg.player.type[type].dLightStyle;
				dlight->min_light = config_system.g_cfg.player.type[type].dLightExpoent;
			}
		}
	}
}