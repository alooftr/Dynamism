// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "other_esp.h"
#include "..\autowall\autowall.h"
#include "..\ragebot\antiaim.h"
#include "..\misc\logs.h"
#include "..\misc\misc.h"
#include "..\lagcompensation\local_animations.h"
#include "../../Render.h"


void otheresp::hitmarker_paint()
{
	if (!config_system.g_cfg.esp.hit)
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (!g_ctx.local()->is_alive())
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (hitmarker.hurt_time + 0.7f > m_globals()->m_curtime)
	{
		if (config_system.g_cfg.esp.hit)
		{
			Vector world;

			if (math::WorldToScreen(hitmarker.point, world))
			{
				auto alpha = (int)((hitmarker.hurt_time + 0.7f - m_globals()->m_curtime) * 255.0f);
				hitmarker.hurt_color.SetAlpha(alpha);

				auto offset = 7.0f - (float)alpha / 255.0f * 7.0f;

				g_Render->DrawLine(world.x + 2 + offset, world.y - 2 - offset, world.x + 6 + offset, world.y - 6 - offset, Color(255, 255, 255));
				g_Render->DrawLine(world.x + 2 + offset, world.y + 2 + offset, world.x + 6 + offset, world.y + 6 + offset, Color(255, 255, 255));
				g_Render->DrawLine(world.x - 2 - offset, world.y + 2 + offset, world.x - 6 - offset, world.y + 6 + offset, Color(255, 255, 255));
				g_Render->DrawLine(world.x - 2 - offset, world.y - 2 - offset, world.x - 6 - offset, world.y - 6 - offset, Color(255, 255, 255));
			}
		}
	}
}

void otheresp::damage_marker_paint()
{
	for (auto i = 1; i < m_globals()->m_maxclients; i++) //-V807
	{
		if (damage_marker[i].hurt_time + 2.0f > m_globals()->m_curtime)
		{
			Vector screen;

			if (!math::world_to_screen(damage_marker[i].position, screen))
				continue;

			auto alpha = (int)((damage_marker[i].hurt_time + 2.0f - m_globals()->m_curtime) * 127.5f);
			damage_marker[i].hurt_color.SetAlpha(alpha);

			render::get().text(fonts[DAMAGE_MARKER], screen.x, screen.y, damage_marker[i].hurt_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "%i", damage_marker[i].damage);
		}
	}
}

void otheresp::automatic_peek_indicator()
{
	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	static auto position = ZERO;

	if (!g_ctx.globals.start_position.IsZero())
		position = g_ctx.globals.start_position;

	if (position.IsZero())
		return;

	static auto alpha = 0.0f;

	if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18) || alpha)
	{
		if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18))
			alpha += 3.0f * m_globals()->m_frametime; //-V807
		else
			alpha -= 3.0f * m_globals()->m_frametime;

		alpha = math::clamp(alpha, 0.0f, 1.0f);



		static float maxsize{ 0.f };
		if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18)) {
			maxsize += m_globals()->m_frametime * 50;
			if (maxsize > 30.f) {
				maxsize = 30.f;

			}

			g_Render->DrawRing3D(position.x, position.y, position.z, maxsize, 20, Color(40, 40, 200, 255), Color(40, 40, 200, 100), 3.f);

			Vector screen;
			Vector quic;

			Vector local_origin = g_ctx.local()->GetAbsOrigin();
			Vector localorign;
			if (math::WorldToScreen(local_origin, localorign) && math::WorldToScreen(position, quic))
				g_Render->DrawLine(localorign.x, localorign.y, quic.x + 2, quic.y + 1, Color(40, 40, 200, 255));
			quic = local_origin;
		}
		else {
			if (maxsize > 0) maxsize -= m_globals()->m_frametime * 50;
		}
	}
}

void otheresp::zeuseknife()
{
	if (!config_system.g_cfg.misc.zeus)
		return;

	/*if (g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_TASER)
	{

		float step = M_PI * 2.0 / 2047;
		float rad = g_ctx.globals.weapon->is_knife() ? 64.f : g_ctx.globals.weapon->get_csweapon_info()->flRange;
		Vector origin = g_ctx.local()->get_shoot_position();
		static float hue_offset = 0;
		Vector prev_scr_pos, scr_pos;

		for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
		{
			Vector pos(rad * cos(rotation) + origin.x, rad * sin(rotation) + origin.y, origin.z);

			Ray_t ray;
			trace_t trace;
			CTraceFilter filter;

			filter.pSkip = g_ctx.local();
			ray.Init(origin, pos);

			m_trace()->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace);

			if (math::WorldToScreen(trace.endpos, scr_pos))
			{
				if (prev_scr_pos.IsValid())
				{
					int hue = RAD2DEG(rotation) + hue_offset;
					ImVec4 temp = ImColor::HSV(hue / 360.f, 1, 1);
					Color color = Color(temp.x, temp.y, temp.z);

					g_Render->DrawLine(prev_scr_pos.x, prev_scr_pos.y, scr_pos.x, scr_pos.y, color);
				}
				prev_scr_pos = scr_pos;
			}
		}
		hue_offset += 0.4f;
	}*/
}