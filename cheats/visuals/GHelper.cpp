#include "GHelper.h"
#include "../../Render.h"
#include "../menu.h"

//-------------------------------------------------------------------------------------------------------------------------------

namespace visuals
{
	Vector CalcHelpPos(Vector target)
	{
		
		if (!g_ctx.local())
			return Vector(0, 0, 0);

		Vector vAngle = Vector(0, 0, 0);

		m_engine()->GetViewAngles(vAngle);

		float range = 5.f;

		float r_1, r_2;
		float x_1, y_1;

		Vector LocalRendOrig = g_ctx.local()->GetRenderOrigin();
		Vector LocalViewOfst = g_ctx.local()->m_vecViewOffset();

		Vector vEyeOrigin = LocalRendOrig + LocalViewOfst;

		r_1 = -(target.y - vEyeOrigin.y);
		r_2 = target.x - vEyeOrigin.x;
		float Yaw = vAngle.y - 90.0f;

		float yawToRadian = Yaw * (float)(M_PI / 180.0F);
		x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20.f;
		y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20.f;

		x_1 *= range;
		y_1 *= range;

		return Vector(x_1, y_1, 0);
	}


	static Vector CalcDir(const Vector& vAngles)
	{
		Vector vForward;
		float	sp, sy, cp, cy;

		sy = sin(DEG2RAD(vAngles[1]));
		cy = cos(DEG2RAD(vAngles[1]));

		sp = sin(DEG2RAD(vAngles[0]));
		cp = cos(DEG2RAD(vAngles[0]));

		vForward.x = cp * cy;
		vForward.y = cp * sy;
		vForward.z = -sp;

		return vForward;
	}

	void RenderCircle()
	{
		if (!m_engine()->IsInGame() || !m_engine()->IsConnected())
			return;

		if (!config_system.g_cfg.grenadehelper.enable)
			return;

		Vector InfoPosScreen;
		Vector CrosshairScreen;

		int x, y;
		m_engine()->GetScreenSize(x, y);

		float cy = y / 2;
		float cx = x / 2;

		if (strstr(m_engine()->GetLevelName(), "mirage"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.MirageGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.MirageGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.MirageGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.MirageGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.MirageGrenade[i].ang;
				Vector TCircleOfst = CalcHelpPos(config_system.g_cfg.grenadehelper.MirageGrenade[i].pos);

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.MirageGrenade[i].pos);
				auto size = ImGui::CalcTextSize(config_system.g_cfg.grenadehelper.MirageGrenade[i].info.c_str());

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 70 && math::WorldToScreen(Crosshair, CrosshairScreen))
					{
						g_Render->FilledRect2(CrosshairScreen.x - 10.f, CrosshairScreen.y - 10.f, CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f, Color(0, 0, 0, 180));
						g_Render->CircleFilled(CrosshairScreen.x, CrosshairScreen.y, 4.f, Color(20, 205, 50, 255), 90.f);
						g_Render->DrawLine(cx, cy, CrosshairScreen.x, CrosshairScreen.y, Color(255, 255, 255, 255));
						g_Render->DrawString(CrosshairScreen.x + 17, CrosshairScreen.y - 7.f, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.MirageGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "inferno"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.InfernoGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.InfernoGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.InfernoGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.InfernoGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.InfernoGrenade[i].ang;
				Vector TCircleOfst = CalcHelpPos(config_system.g_cfg.grenadehelper.InfernoGrenade[i].pos);

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.InfernoGrenade[i].pos);
				auto size = ImGui::CalcTextSize(config_system.g_cfg.grenadehelper.InfernoGrenade[i].info.c_str());

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 70 && math::WorldToScreen(Crosshair, CrosshairScreen))
					{
						g_Render->FilledRect2(CrosshairScreen.x - 10.f, CrosshairScreen.y - 10.f, CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f, Color(0, 0, 0, 180));
						g_Render->CircleFilled(CrosshairScreen.x, CrosshairScreen.y, 4.f, Color(20, 205, 50, 255), 90.f);
						g_Render->DrawLine(cx, cy, CrosshairScreen.x, CrosshairScreen.y, Color(255, 255, 255, 255));
						g_Render->DrawString(CrosshairScreen.x + 17, CrosshairScreen.y - 7.f, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.InfernoGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "dust2"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.dust2Grenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.dust2Grenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.dust2Grenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.dust2Grenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.dust2Grenade[i].ang;
				Vector TCircleOfst = CalcHelpPos(config_system.g_cfg.grenadehelper.dust2Grenade[i].pos);

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.dust2Grenade[i].pos);
				auto size = ImGui::CalcTextSize(config_system.g_cfg.grenadehelper.dust2Grenade[i].info.c_str());

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 70 && math::WorldToScreen(Crosshair, CrosshairScreen))
					{
						g_Render->FilledRect2(CrosshairScreen.x - 10.f, CrosshairScreen.y - 10.f, CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f, Color(0, 0, 0, 180));
						g_Render->CircleFilled(CrosshairScreen.x, CrosshairScreen.y, 4.f, Color(20, 205, 50, 255), 90.f);
						g_Render->DrawLine(cx, cy, CrosshairScreen.x, CrosshairScreen.y, Color(255, 255, 255, 255));
						g_Render->DrawString(CrosshairScreen.x + 17, CrosshairScreen.y - 7.f, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.dust2Grenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "nuke"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.nukeGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.nukeGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.nukeGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.nukeGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.nukeGrenade[i].ang;
				Vector TCircleOfst = CalcHelpPos(config_system.g_cfg.grenadehelper.nukeGrenade[i].pos);

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.nukeGrenade[i].pos);
				auto size = ImGui::CalcTextSize(config_system.g_cfg.grenadehelper.nukeGrenade[i].info.c_str());

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 70 && math::WorldToScreen(Crosshair, CrosshairScreen))
					{
						g_Render->FilledRect2(CrosshairScreen.x - 10.f, CrosshairScreen.y - 10.f, CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f, Color(0, 0, 0, 180));
						g_Render->CircleFilled(CrosshairScreen.x, CrosshairScreen.y, 4.f, Color(20, 205, 50, 255), 90.f);
						g_Render->DrawLine(cx, cy, CrosshairScreen.x, CrosshairScreen.y, Color(255, 255, 255, 255));
						g_Render->DrawString(CrosshairScreen.x + 17, CrosshairScreen.y - 7.f, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.nukeGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "overpass"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.overpassGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.overpassGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.overpassGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.overpassGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.overpassGrenade[i].ang;
				Vector TCircleOfst = CalcHelpPos(config_system.g_cfg.grenadehelper.overpassGrenade[i].pos);

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.overpassGrenade[i].pos);
				auto size = ImGui::CalcTextSize(config_system.g_cfg.grenadehelper.overpassGrenade[i].info.c_str());

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 70 && math::WorldToScreen(Crosshair, CrosshairScreen))
					{
						g_Render->FilledRect2(CrosshairScreen.x - 10.f, CrosshairScreen.y - 10.f, CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f, Color(0, 0, 0, 180));
						g_Render->CircleFilled(CrosshairScreen.x, CrosshairScreen.y, 4.f, Color(20, 205, 50, 255), 90.f);
						g_Render->DrawLine(cx, cy, CrosshairScreen.x, CrosshairScreen.y, Color(255, 255, 255, 255));
						g_Render->DrawString(CrosshairScreen.x + 17, CrosshairScreen.y - 7.f, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.overpassGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "train"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.trainGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.trainGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.trainGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.trainGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.trainGrenade[i].ang;
				Vector TCircleOfst = CalcHelpPos(config_system.g_cfg.grenadehelper.trainGrenade[i].pos);

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.trainGrenade[i].pos);
				auto size = ImGui::CalcTextSize(config_system.g_cfg.grenadehelper.trainGrenade[i].info.c_str());

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 70 && math::WorldToScreen(Crosshair, CrosshairScreen))
					{
						g_Render->FilledRect2(CrosshairScreen.x - 10.f, CrosshairScreen.y - 10.f, CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f, Color(0, 0, 0, 180));
						g_Render->CircleFilled(CrosshairScreen.x, CrosshairScreen.y, 4.f, Color(20, 205, 50, 255), 90.f);
						g_Render->DrawLine(cx, cy, CrosshairScreen.x, CrosshairScreen.y, Color(255, 255, 255, 255));
						g_Render->DrawString(CrosshairScreen.x + 17, CrosshairScreen.y - 7.f, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.trainGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "cache"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.cacheGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.cacheGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.cacheGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.cacheGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.cacheGrenade[i].ang;
				Vector TCircleOfst = CalcHelpPos(config_system.g_cfg.grenadehelper.cacheGrenade[i].pos);

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.cacheGrenade[i].pos);
				auto size = ImGui::CalcTextSize(config_system.g_cfg.grenadehelper.cacheGrenade[i].info.c_str());

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 70 && math::WorldToScreen(Crosshair, CrosshairScreen))
					{
						g_Render->FilledRect2(CrosshairScreen.x - 10.f, CrosshairScreen.y - 10.f, CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f, Color(0, 0, 0, 180));
						g_Render->CircleFilled(CrosshairScreen.x, CrosshairScreen.y, 4.f, Color(20, 205, 50, 255), 90.f);
						g_Render->DrawLine(cx, cy, CrosshairScreen.x, CrosshairScreen.y, Color(255, 255, 255, 255));
						g_Render->DrawString(CrosshairScreen.x + 17, CrosshairScreen.y - 7.f, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.cacheGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "cbble"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.cbbleGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.cbbleGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.cbbleGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.cbbleGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.cbbleGrenade[i].ang;
				Vector TCircleOfst = CalcHelpPos(config_system.g_cfg.grenadehelper.cbbleGrenade[i].pos);

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.cbbleGrenade[i].pos);
				auto size = ImGui::CalcTextSize(config_system.g_cfg.grenadehelper.cbbleGrenade[i].info.c_str());

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 70 && math::WorldToScreen(Crosshair, CrosshairScreen))
					{
						g_Render->FilledRect2(CrosshairScreen.x - 10.f, CrosshairScreen.y - 10.f, CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f, Color(0, 0, 0, 180));
						g_Render->CircleFilled(CrosshairScreen.x, CrosshairScreen.y, 4.f, Color(20, 205, 50, 255), 90.f);
						g_Render->DrawLine(cx, cy, CrosshairScreen.x, CrosshairScreen.y, Color(255, 255, 255, 255));
						g_Render->DrawString(CrosshairScreen.x + 17, CrosshairScreen.y - 7.f, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.cbbleGrenade[i].info.c_str());

					}
				}
			}
		}
	}




	void DrawCircle3D(int16_t x, int16_t y, int16_t z, int16_t radius, uint16_t points, Color clr, float thickness)
	{
		Vector pos = Vector(x, y, z);

		float step = (float)M_PI * 2.0f / points;

		for (float a = 0; a < (M_PI * 2.0f); a += step)
		{
			Vector start(radius * cosf(a) + pos.x, radius * sinf(a) + pos.y, pos.z);
			Vector end(radius * cosf(a + step) + pos.x, radius * sinf(a + step) + pos.y, pos.z);

			Vector start2d;
			Vector end2d;

			Vector start22d(start2d);
			Vector end22d(end2d);
			if (math::WorldToScreen(start, start2d) &&
				math::WorldToScreen(end, end2d))
			{
				start22d.x = start2d.x;
				start22d.y = start2d.y;

				end22d.x = end2d.x;
				end22d.y = end2d.y;

				g_Render->DrawLine(start22d[0], start22d[1], end22d[0], end22d[1], clr);

			}
		}
	}

	void DrawRing3D()
	{
		if (!m_engine()->IsInGame() || !m_engine()->IsConnected())
			return;

		if (!config_system.g_cfg.grenadehelper.enable)
			return;

		if (strstr(m_engine()->GetLevelName(), "mirage"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.MirageGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.MirageGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.MirageGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.MirageGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.MirageGrenade[i].ang;

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.MirageGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade())
					{
						if (dist <= 500)
							DrawCircle3D(ppos.x, ppos.y, ppos.z, 24.f, 24.f, dist <= 75 ? Color(255, 0, 0) : Color(255, 255, 255), 1.0f);

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "inferno"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.InfernoGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.InfernoGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.InfernoGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.InfernoGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.InfernoGrenade[i].ang;

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.InfernoGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon()->is_grenade())
				{
					if (dist <= 500)
						DrawCircle3D(ppos.x, ppos.y, ppos.z, 24.f, 24.f, dist <= 75 ? Color(255, 0, 0) : Color(255, 255, 255), 1.0f);
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "dust2"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.dust2Grenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.dust2Grenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.dust2Grenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.dust2Grenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.dust2Grenade[i].ang;

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.dust2Grenade[i].pos);
				if (g_ctx.local()->m_hActiveWeapon()->is_grenade())
				{
					if (dist <= 500)
						DrawCircle3D(ppos.x, ppos.y, ppos.z, 24.f, 24.f, dist <= 75 ? Color(255, 0, 0) : Color(255, 255, 255), 1.0f);
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "nuke"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.nukeGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.nukeGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.nukeGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.nukeGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.nukeGrenade[i].ang;

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.nukeGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon()->is_grenade())
				{
					if (dist <= 500)
						DrawCircle3D(ppos.x, ppos.y, ppos.z, 24.f, 24.f, dist <= 75 ? Color(255, 0, 0) : Color(255, 255, 255), 1.0f);
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "overpass"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.overpassGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.overpassGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.overpassGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.overpassGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.overpassGrenade[i].ang;

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.overpassGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon()->is_grenade())
				{
					if (dist <= 500)
						DrawCircle3D(ppos.x, ppos.y, ppos.z, 24.f, 24.f, dist <= 75 ? Color(255, 0, 0) : Color(255, 255, 255), 1.0f);
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "train"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.trainGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.trainGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.trainGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.trainGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.trainGrenade[i].ang;

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.trainGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon()->is_grenade())
				{
					if (dist <= 500)
						DrawCircle3D(ppos.x, ppos.y, ppos.z, 24.f, 24.f, dist <= 75 ? Color(255, 0, 0) : Color(255, 255, 255), 1.0f);
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "cache"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.cacheGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.cacheGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.cacheGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.cacheGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.cacheGrenade[i].ang;

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.cacheGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon()->is_grenade())
				{
					if (dist <= 500)
						DrawCircle3D(ppos.x, ppos.y, ppos.z, 24.f, 24.f, dist <= 75 ? Color(255, 0, 0) : Color(255, 255, 255), 1.0f);
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "cbble"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.cbbleGrenade.size(); ++i)
			{
				Vector ppos = config_system.g_cfg.grenadehelper.cbbleGrenade[i].pos;
				Vector Crosshair = g_ctx.local()->get_shoot_position() + (CalcDir(config_system.g_cfg.grenadehelper.cbbleGrenade[i].ang) * 250.f);
				Vector InfoPos = config_system.g_cfg.grenadehelper.cbbleGrenade[i].pos;
				Vector InfoAng = config_system.g_cfg.grenadehelper.cbbleGrenade[i].ang;

				auto dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.cbbleGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon()->is_grenade())
				{
					if (dist <= 500)
						DrawCircle3D(ppos.x, ppos.y, ppos.z, 24.f, 24.f, dist <= 75 ? Color(255, 0, 0) : Color(255, 255, 255), 1.0f);
				}
			}
		}
	}


	void RenderInfo()
	{
		if (!m_engine()->IsInGame() || !m_engine()->IsConnected())
			return;

		if (!config_system.g_cfg.grenadehelper.enable)
			return;


		if (strstr(m_engine()->GetLevelName(), "mirage"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.MirageGrenade.size(); ++i)
			{
				Vector InfoPosScreen;
				Vector InfoPos = config_system.g_cfg.grenadehelper.MirageGrenade[i].pos;

				float dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.MirageGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 500.f && math::WorldToScreen(InfoPos, InfoPosScreen))
					{
						g_Render->DrawString(InfoPosScreen.x - config_system.g_cfg.grenadehelper.MirageGrenade[i].info.length() - 15.f, InfoPosScreen.y - 75.f, Color(255, 255, 255, 255), render2::outline, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.MirageGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "inferno"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.InfernoGrenade.size(); ++i)
			{
				Vector InfoPosScreen;
				Vector InfoPos = config_system.g_cfg.grenadehelper.InfernoGrenade[i].pos;

				float dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.InfernoGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 500.f && math::WorldToScreen(InfoPos, InfoPosScreen))
					{
						g_Render->DrawString(InfoPosScreen.x - config_system.g_cfg.grenadehelper.InfernoGrenade[i].info.length() - 15.f, InfoPosScreen.y - 75.f, Color(255, 255, 255, 255), render2::outline, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.InfernoGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "dust2"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.dust2Grenade.size(); ++i)
			{

				Vector InfoPosScreen;
				Vector InfoPos = config_system.g_cfg.grenadehelper.dust2Grenade[i].pos;

				float dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.dust2Grenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 500.f && math::WorldToScreen(InfoPos, InfoPosScreen))
					{
						g_Render->DrawString(InfoPosScreen.x - config_system.g_cfg.grenadehelper.dust2Grenade[i].info.length() - 15.f, InfoPosScreen.y - 75.f, Color(255, 255, 255, 255), render2::outline, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.dust2Grenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "nuke"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.nukeGrenade.size(); ++i)
			{

				Vector InfoPosScreen;
				Vector InfoPos = config_system.g_cfg.grenadehelper.nukeGrenade[i].pos;

				float dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.nukeGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 500.f && math::WorldToScreen(InfoPos, InfoPosScreen))
					{
						g_Render->DrawString(InfoPosScreen.x - config_system.g_cfg.grenadehelper.nukeGrenade[i].info.length() - 15.f, InfoPosScreen.y - 75.f, Color(255, 255, 255, 255), render2::outline, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.nukeGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "overpass"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.overpassGrenade.size(); ++i)
			{
				Vector InfoPosScreen;
				Vector InfoPos = config_system.g_cfg.grenadehelper.overpassGrenade[i].pos;

				float dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.overpassGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 500.f && math::WorldToScreen(InfoPos, InfoPosScreen))
					{
						g_Render->DrawString(InfoPosScreen.x - config_system.g_cfg.grenadehelper.overpassGrenade[i].info.length() - 15.f, InfoPosScreen.y - 75.f, Color(255, 255, 255, 255), render2::outline, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.overpassGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "train"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.trainGrenade.size(); ++i)
			{

				Vector InfoPosScreen;
				Vector InfoPos = config_system.g_cfg.grenadehelper.trainGrenade[i].pos;

				float dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.trainGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 500.f && math::WorldToScreen(InfoPos, InfoPosScreen))
					{
						g_Render->DrawString(InfoPosScreen.x - config_system.g_cfg.grenadehelper.trainGrenade[i].info.length() - 15.f, InfoPosScreen.y - 75.f, Color(255, 255, 255, 255), render2::outline, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.trainGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "cache"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.cacheGrenade.size(); ++i)
			{

				Vector InfoPosScreen;
				Vector InfoPos = config_system.g_cfg.grenadehelper.cacheGrenade[i].pos;

				float dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.cacheGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 500.f && math::WorldToScreen(InfoPos, InfoPosScreen))
					{
						g_Render->DrawString(InfoPosScreen.x - config_system.g_cfg.grenadehelper.cacheGrenade[i].info.length() - 15.f, InfoPosScreen.y - 75.f, Color(255, 255, 255, 255), render2::outline, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.cacheGrenade[i].info.c_str());

					}
				}
			}
		}

		if (strstr(m_engine()->GetLevelName(), "cbble"))
		{
			for (int i = 0; i < config_system.g_cfg.grenadehelper.cbbleGrenade.size(); ++i)
			{

				Vector InfoPosScreen;
				Vector InfoPos = config_system.g_cfg.grenadehelper.cbbleGrenade[i].pos;

				float dist = g_ctx.local()->GetAbsOrigin().DistTo(config_system.g_cfg.grenadehelper.cbbleGrenade[i].pos);

				if (g_ctx.local()->m_hActiveWeapon())
				{
					if (g_ctx.local()->m_hActiveWeapon()->is_grenade() && dist <= 500.f && math::WorldToScreen(InfoPos, InfoPosScreen))
					{
						g_Render->DrawString(InfoPosScreen.x - config_system.g_cfg.grenadehelper.cbbleGrenade[i].info.length() - 15.f, InfoPosScreen.y - 75.f, Color(255, 255, 255, 255), render2::outline, c_menu::get().futura_small, config_system.g_cfg.grenadehelper.cbbleGrenade[i].info.c_str());
					}
				}
			}
		}
	}
}