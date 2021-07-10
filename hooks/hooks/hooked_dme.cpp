// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\misc\fakelag.h"
#include "..\..\cheats\lagcompensation\local_animations.h"
#include "..\..\cheats\visuals\player_esp.h"

using DrawModelExecute_t = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);

void DrawChams(Color color, const ModelRenderInfo_t& info, int material_i, float alpha_modifier, bool xqz)
{
	static IMaterial* materials[] =
	{
		m_materialsystem()->CreateMaterial("normal", KeyValues::fromString("VertexLitGeneric", nullptr)), // "Texture\0Flat\0Metallicconfig_system.g_cfg.player.transparency_in_scope_amount"
		m_materialsystem()->CreateMaterial("flat", KeyValues::fromString("UnlitGeneric", nullptr)),
		m_materialsystem()->CreateMaterial("reflective", KeyValues::fromString("VertexLitGeneric", "$basetexture vgui/white_additive $ignorez 0 $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halflambert 1 $znearer 0 $flat 1")),
		m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/trophy_majors/crystal_clear"), nullptr),
		m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/cologne_prediction/cologne_prediction_glass"), nullptr),
		m_materialsystem()->FindMaterial(crypt_str("dev/glow_armsrace.vmt"), nullptr),
		m_materialsystem()->CreateMaterial(crypt_str("glowref"), KeyValues::fromString("VertexLitGeneric", "$additive 1 $envmap env_cubemap $envmaptint [1 1 1] $envmapfresnel 1 $envmapfresnelminmaxexp [0 1 2] $alpha 1")),

	
	};

	auto model_entity = static_cast<player_t*>(m_entitylist()->GetClientEntity(info.entity_index));
	auto material = materials[material_i];
	auto alpha = (float)color.a() / 255.0f;

	float material_color[3] =
	{
		color[0] / 255.0f,
		color[1] / 255.0f,
		color[2] / 255.0f
	};

	m_renderview()->SetBlend(alpha * alpha_modifier);
	util::color_modulate(material_color, material);

	material->IncrementReferenceCount();
	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, xqz);

	m_modelrender()->ForcedMaterialOverride(material);
	//original_fn( m_modelrender( ), ctx, state, info, bone_to_world );

}


#include "../../cheats/ragebot/aim.h"

void __stdcall hooks::hooked_dme(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone_to_world)
{
	static auto original_fn = modelrender_hook.GetOriginal <DrawModelExecute_t>();
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

	if (m_engine()->IsTakingScreenshot() && config_system.g_cfg.misc.anti_screenshot)
		return original_fn(m_modelrender(), ctx, state, info, bone_to_world);

	if (!info.pModel)
		return original_fn(m_modelrender(), ctx, state, info, bone_to_world);

	auto model_entity = static_cast<player_t*>(m_entitylist()->GetClientEntity(info.entity_index));
	auto name = m_modelinfo()->GetModelName(info.pModel);

	auto is_player = strstr(name, "models/player") && model_entity->is_alive() && (config_system.g_cfg.player.type[LOCAL].enable_chams ||
		config_system.g_cfg.player.type[ENEMY].enable_chams || config_system.g_cfg.player.type[ENEMY].xqz_enable ||
		config_system.g_cfg.player.type[TEAM].enable_chams || config_system.g_cfg.player.type[TEAM].xqz_enable ||
		config_system.g_cfg.player.fake_chams_enable ||
		config_system.g_cfg.player.backtrack_chams);
	auto is_weapon = strstr(name, "weapons/v_") && !strstr(name, "arms") && config_system.g_cfg.esp.weapon_chams;
	auto is_arms = strstr(name, "arms") && config_system.g_cfg.esp.arms_chams;
	auto is_sleeve = strstr(name, "sleeve") && config_system.g_cfg.esp.arms_chams;

	if (m_modelrender()->IsForcedMaterialOverride() && !is_weapon && !is_arms && !is_sleeve)
		return original_fn(m_modelrender(), ctx, state, info, bone_to_world);

	m_renderview()->SetColorModulation(1.0f, 1.0f, 1.0f); //-V807

	if (!is_player && !is_weapon && !is_arms && !is_sleeve)
		return original_fn(m_modelrender(), ctx, state, info, bone_to_world);

	static IMaterial* materials[] =
	{
		m_materialsystem()->CreateMaterial("normal", KeyValues::fromString("VertexLitGeneric", nullptr)), // "Texture\0Flat\0Metallicconfig_system.g_cfg.player.transparency_in_scope_amount"
		m_materialsystem()->CreateMaterial("flat", KeyValues::fromString("UnlitGeneric", nullptr)),
		m_materialsystem()->CreateMaterial("metallic", KeyValues::fromString("VertexLitGeneric", "$basetexture white $ignorez 0 $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halfambert 1 $znearer 0 $flat 1")),
		m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/trophy_majors/crystal_clear"), nullptr),
		m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/cologne_prediction/cologne_prediction_glass"), nullptr),
		m_materialsystem()->FindMaterial(crypt_str("dev/glow_armsrace.vmt"), nullptr),
		m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/wildfire_gold/wildfire_gold_detail"), nullptr),

	};

	auto called_original = false;


	if (is_player)
	{
		bool not_seeing = !(g_ctx.local()->CanSeePlayer(model_entity, model_entity->hitbox_position(HITBOX_HEAD))
			&& g_ctx.local()->CanSeePlayer(model_entity, model_entity->hitbox_position(HITBOX_NECK)) &&
			g_ctx.local()->CanSeePlayer(model_entity, model_entity->hitbox_position(HITBOX_UPPER_CHEST)) &&
			g_ctx.local()->CanSeePlayer(model_entity, model_entity->hitbox_position(HITBOX_CHEST)) &&
			g_ctx.local()->CanSeePlayer(model_entity, model_entity->hitbox_position(HITBOX_PELVIS)) &&
			g_ctx.local()->CanSeePlayer(model_entity, model_entity->hitbox_position(HITBOX_STOMACH)) &&
			g_ctx.local()->CanSeePlayer(model_entity, model_entity->hitbox_position(HITBOX_RIGHT_FOOT)) &&
			g_ctx.local()->CanSeePlayer(model_entity, model_entity->hitbox_position(HITBOX_LEFT_FOOT)));

		auto type = ENEMY;

		if (model_entity == g_ctx.local())
			type = LOCAL;
		else if (model_entity->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			type = TEAM;

		if (type == ENEMY)
		{
			auto alpha_modifier = playeresp::get().esp_alpha_fade[model_entity->EntIndex()];

			auto material = materials[config_system.g_cfg.player.type[ENEMY].chams_type];
			auto double_material = materials[6];


			if (config_system.g_cfg.player.type[ENEMY].enable_chams || config_system.g_cfg.player.type[ENEMY].xqz_enable)
			{
				auto alpha = (float)config_system.g_cfg.player.backtrack_chams_color.a() / 255.0f;;

				if (config_system.g_cfg.player.backtrack_chams)
				{
					auto backtrack_material = materials[config_system.g_cfg.player.backtrack_chams_material];

					if (backtrack_material && !backtrack_material->IsErrorMaterial())
					{
						matrix3x4_t matrix[MAXSTUDIOBONES];

						if (util::get_backtrack_matrix(model_entity, matrix))
						{
							DrawChams(config_system.g_cfg.player.backtrack_chams_color, info, config_system.g_cfg.player.backtrack_chams_material, alpha_modifier, false);
							original_fn(m_modelrender(), ctx, state, info, matrix);
							m_modelrender()->ForcedMaterialOverride(nullptr);
						}
					}
				}

				if (config_system.g_cfg.player.type[ENEMY].xqz_enable)
				{
					if (not_seeing) {

						DrawChams(config_system.g_cfg.player.type[ENEMY].xqz_color, info, config_system.g_cfg.player.type[ENEMY].chams_xqz, alpha_modifier, true);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);
						if (config_system.g_cfg.player.type[ENEMY].double_material_xqz)
						{
							DrawChams(config_system.g_cfg.player.type[ENEMY].double_material_color_xqz, info, config_system.g_cfg.player.type[ENEMY].double_material_m_xqz, alpha_modifier, true);
							original_fn(m_modelrender(), ctx, state, info, bone_to_world);
							m_modelrender()->ForcedMaterialOverride(nullptr);
						}
					}
				}
				if (config_system.g_cfg.player.type[ENEMY].enable_chams)
				{
					DrawChams(config_system.g_cfg.player.type[ENEMY].chams_color, info, config_system.g_cfg.player.type[ENEMY].chams_type, alpha_modifier, false);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);
					if (config_system.g_cfg.player.type[ENEMY].double_material)
					{
						DrawChams(config_system.g_cfg.player.type[ENEMY].double_material_color, info, config_system.g_cfg.player.type[ENEMY].double_material_m, alpha_modifier, false);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);
					}
				}



				called_original = true;
			}

			/*if (config_system.g_cfg.player.lag_hitbox)
			{		
				if (aim::get().last_target[aim::get().last_target_index].record.invalid || !g_ctx.local()->is_alive())
					return;

					aim::get().last_target[aim::get().last_target_index].alpha -= m_globals()->m_frametime * 25;
					DrawChams(Color(config_system.g_cfg.player.lagcomp_chams_color.r(), config_system.g_cfg.player.lagcomp_chams_color.g(), config_system.g_cfg.player.lagcomp_chams_color.b(), aim::get().last_target[aim::get().last_target_index].alpha), info, config_system.g_cfg.player.lagcomp_chams_material, alpha_modifier, false);
					original_fn(m_modelrender(), ctx, state, info, &aim::get().last_target[aim::get().last_target_index].record.matrixes_data.main[128]);
					m_modelrender()->ForcedMaterialOverride(nullptr);		
			}*/


			if (!called_original)
				return original_fn(m_modelrender(), ctx, state, info, bone_to_world);
		}
		else if (type == TEAM)
		{
			auto alpha_modifier = playeresp::get().esp_alpha_fade[model_entity->EntIndex()];
			auto material = materials[config_system.g_cfg.player.type[TEAM].chams_type];
			auto double_material = materials[6];

			if (!material->IsErrorMaterial() && !double_material->IsErrorMaterial())
			{
				if (config_system.g_cfg.player.type[TEAM].enable_chams && config_system.g_cfg.player.type[TEAM].xqz_enable)
				{

					if (config_system.g_cfg.player.type[TEAM].xqz_enable)
					{
						if (not_seeing) {
							DrawChams(config_system.g_cfg.player.type[TEAM].xqz_color, info, config_system.g_cfg.player.type[TEAM].chams_xqz, alpha_modifier, true);
							original_fn(m_modelrender(), ctx, state, info, bone_to_world);
							m_modelrender()->ForcedMaterialOverride(nullptr);

							if (config_system.g_cfg.player.type[TEAM].double_material_xqz)
							{
								DrawChams(config_system.g_cfg.player.type[TEAM].double_material_color_xqz, info, config_system.g_cfg.player.type[TEAM].double_material_m_xqz, alpha_modifier, true);
								original_fn(m_modelrender(), ctx, state, info, bone_to_world);
								m_modelrender()->ForcedMaterialOverride(nullptr);
							}

						}
					}
					if (config_system.g_cfg.player.type[TEAM].enable_chams)
					{
						DrawChams(config_system.g_cfg.player.type[TEAM].chams_color, info, config_system.g_cfg.player.type[TEAM].chams_type, alpha_modifier, false);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);

						if (config_system.g_cfg.player.type[TEAM].double_material)
						{
							DrawChams(config_system.g_cfg.player.type[TEAM].double_material_color, info, config_system.g_cfg.player.type[TEAM].double_material_m, alpha_modifier, false);
							original_fn(m_modelrender(), ctx, state, info, bone_to_world);
							m_modelrender()->ForcedMaterialOverride(nullptr);
						}

					}



					called_original = true;
				}
			}

			if (!called_original)
				return original_fn(m_modelrender(), ctx, state, info, bone_to_world);
		}
		else if (m_input()->m_fCameraInThirdPerson)
		{
			auto alpha_modifier = 1.0f;

			if (config_system.g_cfg.player.transparency_in_scope && g_ctx.globals.scoped)
				alpha_modifier = config_system.g_cfg.player.transparency_in_scope_amount / 100.f;

			auto material = materials[config_system.g_cfg.player.type[LOCAL].chams_type];
			auto double_material = materials[6];

			if (!material->IsErrorMaterial() && !double_material->IsErrorMaterial())
			{
				if (config_system.g_cfg.player.type[LOCAL].enable_chams)
				{
					if (config_system.g_cfg.player.type[LOCAL].enable_chams)
					{
						DrawChams(config_system.g_cfg.player.type[LOCAL].chams_color, info, config_system.g_cfg.player.type[LOCAL].chams_type, alpha_modifier, false);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);
					}

					if (config_system.g_cfg.player.type[LOCAL].double_material)
					{
						DrawChams(config_system.g_cfg.player.type[LOCAL].double_material_color, info, config_system.g_cfg.player.type[LOCAL].double_material_m, alpha_modifier, false);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);
					}


					called_original = true;
				}
			}

			if (!called_original && config_system.g_cfg.player.layered)
			{
				m_renderview()->SetBlend(alpha_modifier);
				m_renderview()->SetColorModulation(1.0f, 1.0f, 1.0f);

				original_fn(m_modelrender(), ctx, state, info, bone_to_world);
			}

			if (config_system.g_cfg.player.fake_chams_enable)
			{

				for (auto& i : g_ctx.globals.fake_matrix)
				{
					i[0][3] += info.origin.x;
					i[1][3] += info.origin.y;
					i[2][3] += info.origin.z;
				}


				DrawChams(config_system.g_cfg.player.fake_chams_color, info, config_system.g_cfg.player.fake_chams_type, alpha_modifier, false);
				original_fn(m_modelrender(), ctx, state, info, g_ctx.globals.fake_matrix);
				m_modelrender()->ForcedMaterialOverride(nullptr);

				if (config_system.g_cfg.player.fake_double_material)
				{
					DrawChams(config_system.g_cfg.player.fake_double_material_color, info, config_system.g_cfg.player.fake_double_material_m, alpha_modifier, false);
					original_fn(m_modelrender(), ctx, state, info, g_ctx.globals.fake_matrix);
					m_modelrender()->ForcedMaterialOverride(nullptr);
				}


				for (auto& i : g_ctx.globals.fake_matrix)
				{
					i[0][3] -= info.origin.x;
					i[1][3] -= info.origin.y;
					i[2][3] -= info.origin.z;
				}

			}

			
			if (!called_original && !config_system.g_cfg.player.layered)
			{
				m_renderview()->SetBlend(alpha_modifier);
				m_renderview()->SetColorModulation(1.0f, 1.0f, 1.0f);

				original_fn(m_modelrender(), ctx, state, info, bone_to_world);
			}
		}
	}
	else if (is_weapon)
	{
		auto alpha = (float)config_system.g_cfg.esp.weapon_chams_color.a() / 255.0f;

		auto material = materials[config_system.g_cfg.esp.weapon_chams_type];
		auto double_material = materials[6];

		if (!material->IsErrorMaterial() && !double_material->IsErrorMaterial())
		{
			if (config_system.g_cfg.esp.weapon_chams) {

				DrawChams(config_system.g_cfg.esp.weapon_chams_color, info, config_system.g_cfg.esp.weapon_chams_type, 100, false);
				original_fn(m_modelrender(), ctx, state, info, bone_to_world);
				m_modelrender()->ForcedMaterialOverride(nullptr);
				if (config_system.g_cfg.esp.weapon_double_material)
				{
					DrawChams(config_system.g_cfg.esp.weapon_double_material_color, info, config_system.g_cfg.esp.weapon_double_material_m, 100, false);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);
				}

			}

			called_original = true;
		}

		if (!called_original)
			return original_fn(m_modelrender(), ctx, state, info, bone_to_world);
	}
	else if (is_arms || is_sleeve)
	{
		auto alpha = (float)config_system.g_cfg.esp.arms_chams_color.a() / 255.0f;

		auto material = materials[config_system.g_cfg.esp.arms_chams_type];
		auto double_material = materials[6];

		if (!material->IsErrorMaterial() && !double_material->IsErrorMaterial())
		{
			DrawChams(config_system.g_cfg.esp.arms_chams_color, info, config_system.g_cfg.esp.arms_chams_type, 1, false);
			original_fn(m_modelrender(), ctx, state, info, bone_to_world);
			m_modelrender()->ForcedMaterialOverride(nullptr);


			if (config_system.g_cfg.esp.arms_double_material && config_system.g_cfg.esp.arms_chams_type != 6)
			{
				DrawChams(config_system.g_cfg.esp.arms_double_material_color, info, config_system.g_cfg.esp.arms_double_material_m, 1, false);
				original_fn(m_modelrender(), ctx, state, info, bone_to_world);
				m_modelrender()->ForcedMaterialOverride(nullptr);
			}

			called_original = true;
		}

		if (!called_original)
			return original_fn(m_modelrender(), ctx, state, info, bone_to_world);
	}
}