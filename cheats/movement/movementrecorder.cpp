#include "movementrecorder.h"

MovementRe g_MovementRecorder;

void MovementRe::MovementR(CUserCmd* cmd)
{

	if (key_binds::get().get_key_bind_state(26))
	{
			if (f2)
			{
				GetMrecorder.CmdFinal = {};
				GetMrecorder.CmdFinal2 = {};
				GetMrecorder.startVec[GetMrecorder.movement_int] = g_ctx.local()->GetAbsOrigin();
			}

			f2 = false;
			GetMrecorder.tempCmd2.position[GetMrecorder.movement_int] = g_ctx.local()->GetAbsOrigin();
			GetMrecorder.tempCmd.buttons[GetMrecorder.movement_int] = cmd->m_buttons;
			GetMrecorder.tempCmd.forwardmove[GetMrecorder.movement_int] = cmd->m_forwardmove;
			GetMrecorder.tempCmd.sidemove[GetMrecorder.movement_int] = cmd->m_sidemove;
			GetMrecorder.tempCmd.upmove[GetMrecorder.movement_int] =  cmd->m_upmove;
			GetMrecorder.tempCmd.viewAngles[GetMrecorder.movement_int] = cmd->m_viewangles;

			GetMrecorder.CmdFinal.push_back(GetMrecorder.tempCmd);
			GetMrecorder.CmdFinal2.push_back(GetMrecorder.tempCmd2);
	}
	else
	{
		f2 = true;
	}
}

void VectorSubtract(const Vector& a, Vector& b, Vector& c)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}


bool MovementRe::MovementP(CUserCmd* cmd)
{
	bool ret = false;
	if (!key_binds::get().get_key_bind_state(26))
	{

		for (int i = 0; i < 200; i++)
			if (g_ctx.local()->GetAbsOrigin().DistTo(GetMrecorder.startVec[i]) <= 150)
				switch (i)
				{
				case 1:
					GetMrecorder.movement_int = 1;
					break;
				case 2:
					GetMrecorder.movement_int = 2;
					break;
				case 3:
					GetMrecorder.movement_int = 3;
					break;
				case 4:
					GetMrecorder.movement_int = 4;
					break;
				case 5:
					GetMrecorder.movement_int = 5;
					break;
				case 6:
					GetMrecorder.movement_int = 6;
					break;
				case 7:
					GetMrecorder.movement_int = 7;
					break;
				case 8:
					GetMrecorder.movement_int = 8;
					break;
				case 9:
					GetMrecorder.movement_int = 9;
					break;
				case 10:
					GetMrecorder.movement_int = 10;
					break;
				case 11:
					GetMrecorder.movement_int = 11;
					break;
				case 12:
					GetMrecorder.movement_int = 12;
					break;
				case 13:
					GetMrecorder.movement_int = 13;
					break;
				case 14:
					GetMrecorder.movement_int = 14;
					break;
				case 15:
					GetMrecorder.movement_int = 15;
					break;
				case 16:
					GetMrecorder.movement_int = 16;
					break;
				case 17:
					GetMrecorder.movement_int = 17;
					break;
				case 18:
					GetMrecorder.movement_int = 18;
					break;
				case 19:
					GetMrecorder.movement_int = 19;
					break;
				case 20:
					GetMrecorder.movement_int = 20;
					break;
				case 21:
					GetMrecorder.movement_int = 21;
					break;
				case 22:
					GetMrecorder.movement_int = 22;
					break;
				case 23:
					GetMrecorder.movement_int = 23;
					break;
				case 24:
					GetMrecorder.movement_int = 24;
					break;
				case 25:
					GetMrecorder.movement_int = 25;
					break;
				case 26:
					GetMrecorder.movement_int = 26;
					break;
				case 27:
					GetMrecorder.movement_int = 27;
					break;
				case 28:
					GetMrecorder.movement_int = 28;
					break;
				case 29:
					GetMrecorder.movement_int = 29;
					break;
				case 30:
					GetMrecorder.movement_int = 30;
					break;
				}

	}
	if (key_binds::get().get_key_bind_state(27))
	{
		ret = true;
		if (!isPlayingback)
		{
			float dist = math::VectorDistances(g_ctx.local()->GetAbsOrigin(), GetMrecorder.startVec[GetMrecorder.movement_int]);
			if (dist < 3)
			{
				CrosshairFirst = true;
				isPlayingback = true;
			}
			else
			{
				Vector finalVec;
				VectorSubtract(GetMrecorder.startVec[GetMrecorder.movement_int], g_ctx.local()->GetAbsOrigin(), finalVec);
				math::vector_angles(finalVec, cmd->m_viewangles);
				cmd->m_forwardmove = dist;
			}
		}
		else
		{
			if (i >= GetMrecorder.CmdFinal.size())
			{
				i = 0;
				isPlayingback = false;
			}
			else
			{
				if (!isPlayingback)
					return false;


				if (CrosshairFirst)
				{
					Vector aimVec;
					math::angle_vectors(GetMrecorder.CmdFinal[0].viewAngles[GetMrecorder.movement_int], aimVec);
					Vector curVec;
					math::angle_vectors(cmd->m_viewangles, curVec);

					auto delta = aimVec - curVec;

					const auto smoothed = curVec + delta / config_system.g_cfg.misc.smooth;

					Vector aimAng;
					math::vector_angles(smoothed, aimAng);

					math::normalize_angles(aimAng);
					math::clamp_angles(aimAng);

					m_engine()->SetViewAngles(aimAng);

					cmd->m_viewangles = aimAng;

					auto deltadist = math::VectorLength(delta);


					if (deltadist <= 0.005f)
						CrosshairFirst = false;

				}
				else
				{
						cmd->m_buttons = GetMrecorder.CmdFinal[i].buttons[GetMrecorder.movement_int];
						cmd->m_forwardmove = GetMrecorder.CmdFinal[i].forwardmove[GetMrecorder.movement_int];
						cmd->m_sidemove = GetMrecorder.CmdFinal[i].sidemove[GetMrecorder.movement_int];
						cmd->m_upmove = GetMrecorder.CmdFinal[i].upmove[GetMrecorder.movement_int];
						cmd->m_viewangles = GetMrecorder.CmdFinal[i].viewAngles[GetMrecorder.movement_int];
						m_engine()->SetViewAngles(GetMrecorder.CmdFinal[i].viewAngles[GetMrecorder.movement_int]);
						i++;
				}
			}
		}
	}
	else
	{
		i = 0;
		//key_binds::get().get_key_bind_state(27) = false;
		isPlayingback = false;
	}

	return ret;
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

void MovementRe::MovementD()
{
		if (key_binds::get().get_key_bind_state(26) && config_system.g_cfg.misc.showfirstpath)
		{
			Vector last_w2s;
			Vector last_pos = GetMrecorder.CmdFinal2[0].position[GetMrecorder.movement_int];
			Vector to_world;

			for (int i = 0; i < GetMrecorder.CmdFinal2.size(); i++) {
				auto tracer = GetMrecorder.CmdFinal2.at(i);
				if (math::WorldToScreen(tracer.position[GetMrecorder.movement_int], to_world) && math::WorldToScreen(last_pos, last_w2s))
					g_Render->DrawLine(to_world.x, to_world.y, last_w2s.x, last_w2s.y, config_system.g_cfg.misc.showfirstpath_color);
				last_pos = tracer.position[GetMrecorder.movement_int];
			}
		}

		if (key_binds::get().get_key_bind_state(27))
		{
			Vector last_w2s;
			Vector last_pos = GetMrecorder.CmdFinal2[0].position[GetMrecorder.movement_int];
			Vector to_world;

			if (isPlayingback)
			{
				if (config_system.g_cfg.misc.showpath)
				{
					for (int i = 0; i < GetMrecorder.CmdFinal2.size(); i++) {
						auto tracer = GetMrecorder.CmdFinal2.at(i);
						if (g_ctx.local()->GetAbsOrigin().DistTo(tracer.position[GetMrecorder.movement_int]) <= 120)
							if (math::WorldToScreen(tracer.position[GetMrecorder.movement_int], to_world) && math::WorldToScreen(last_pos, last_w2s))
								g_Render->DrawLine(to_world.x, to_world.y, last_w2s.x, last_w2s.y, config_system.g_cfg.misc.pathcolor);
						last_pos = tracer.position[GetMrecorder.movement_int];
					}
				}
			}
		}


		if (config_system.g_cfg.misc.show3dcircle)
		{
			for (int i = 0; i < 200; i++)
				{
				if (GetMrecorder.startVec[i] == Vector(0, 0, 0))
					continue;

				if (g_ctx.local()->GetAbsOrigin().DistTo(GetMrecorder.startVec[i]) >= config_system.g_cfg.misc.showcircle)
					continue;

				g_Render->DrawRing3D(GetMrecorder.startVec[i].x, GetMrecorder.startVec[i].y, GetMrecorder.startVec[i].z, 25.f, 25.f, config_system.g_cfg.misc.circle3d, Color(config_system.g_cfg.misc.circle3d.r(), config_system.g_cfg.misc.circle3d.g(), config_system.g_cfg.misc.circle3d.b(), 100), 2.f);
				}
			
			if (g_ctx.local()->GetAbsOrigin().DistTo(GetMrecorder.startVec[GetMrecorder.movement_int]) <= 75)
			{
				int x, y;
				m_engine()->GetScreenSize(x, y);

				float cy = y / 2;
				float cx = x / 2;


				Vector OriginCrosshair = g_ctx.local()->get_shoot_position() + (CalcDir(GetMrecorder.CmdFinal[0].viewAngles[GetMrecorder.movement_int]) * 250.f);
				Vector cOrigin;

				if (math::WorldToScreen(OriginCrosshair, cOrigin))
				{
					int pixelsFromMiddleOfScreen = sqrt((cOrigin.x - x / 2) * (cOrigin.x - x / 2) + (cOrigin.y - y / 2) * (cOrigin.y - y / 2));

					if(pixelsFromMiddleOfScreen >= 31)
						g_Render->CircleFilled(cOrigin.x, cOrigin.y, 4.f, Color(250,0,0, 255), 90.f);
					else if (pixelsFromMiddleOfScreen <= 30)
						g_Render->CircleFilled(cOrigin.x, cOrigin.y, 4.f, Color(0, 250, 0, 255), 90.f);

					g_Render->DrawLine(cx, cy, cOrigin.x, cOrigin.y, Color(255, 255, 255, 255));
					g_Render->FilledRect2(cOrigin.x - 10.f, cOrigin.y - 10.f, cOrigin.x + 12.f, cOrigin.y + 12.f, Color(0, 0, 0, 180));
				}

			}
		}
}