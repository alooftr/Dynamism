#include "movements.h"
#include "..\misc\prediction_system.h"
#include "../../Render.h"
#include "../menu.h"

#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)
#define M_RADPI 57.295779513082f
void movements::BunnyHop(CUserCmd* cmd)
{
	//if (g_ctx.local()->get_move_type() == MOVETYPE_LADDER) //-V807
	//	return;

	static bool bLastJumped = false;
	static bool bShouldFake = false;

	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		cmd->m_buttons |= IN_JUMP;
	}
	else if (cmd->m_buttons & IN_JUMP)
	{
		if (g_ctx.local()->m_fFlags() & FL_ONGROUND)
		{
			bShouldFake = bLastJumped = true;
		}
		else
		{
			cmd->m_buttons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bShouldFake = bLastJumped = false;
	}
}

void movements::AirStrafe(CUserCmd* m_pcmd, Vector& orig)
{
	if (g_ctx.local()->get_move_type() == MOVETYPE_LADDER) //-V807
		return;

	if (g_ctx.local()->m_fFlags() & FL_ONGROUND || engineprediction::get().backup_data.flags & FL_ONGROUND)
		return;

	static auto cl_sidespeed = m_cvar()->FindVar(crypt_str("cl_sidespeed"));
	auto side_speed = cl_sidespeed->GetFloat();

	if (config_system.g_cfg.misc.airstrafe == 1)
	{
		Vector engine_angles;
		m_engine()->GetViewAngles(engine_angles);

		auto velocity = g_ctx.local()->m_vecVelocity();

		m_pcmd->m_forwardmove = min(5850.0f / velocity.Length2D(), side_speed);
		m_pcmd->m_sidemove = m_pcmd->m_command_number % 2 ? side_speed : -side_speed;

		auto yaw_velocity = math::calculate_angle(Vector(0.0f, 0.0f, 0.0f), velocity).y;
		auto ideal_rotation = math::clamp(RAD2DEG(atan2(15.0f, velocity.Length2D())), 0.0f, 45.0f);

		auto yaw_rotation = fabs(yaw_velocity - engine_angles.y) + (m_pcmd->m_command_number % 2 ? ideal_rotation : -ideal_rotation);
		auto ideal_yaw_rotation = yaw_rotation < 5.0f ? yaw_velocity : engine_angles.y;

		util::RotateMovement(m_pcmd, ideal_yaw_rotation);
	}
	else if (config_system.g_cfg.misc.airstrafe == 2)
	{
		bool back = m_pcmd->m_buttons & IN_BACK;
		bool forward = m_pcmd->m_buttons & IN_FORWARD;
		bool right = m_pcmd->m_buttons & IN_MOVELEFT;
		bool left = m_pcmd->m_buttons & IN_MOVERIGHT;

		static auto old_yaw = 0.0f;

		auto get_velocity_degree = [](float velocity)
		{
			auto tmp = RAD2DEG(atan(30.0f / velocity));

			if (CheckIfNonValidNumber(tmp) || tmp > 90.0f)
				return 90.0f;

			else if (tmp < 0.0f)
				return 0.0f;
			else
				return tmp;
		};

		if (g_ctx.local()->get_move_type() != MOVETYPE_WALK)
			return;

		auto velocity = g_ctx.local()->m_vecVelocity();
		velocity.z = 0.0f;

		auto forwardmove = m_pcmd->m_forwardmove;
		auto sidemove = m_pcmd->m_sidemove;

		if (velocity.Length2D() < 5.0f && !forwardmove && !sidemove)
			return;

		static auto flip = false;
		flip = !flip;

		auto turn_direction_modifier = flip ? 1.0f : -1.0f;
		auto viewangles = m_pcmd->m_viewangles;

		if (forwardmove || sidemove)
		{
			m_pcmd->m_forwardmove = 0.0f;
			m_pcmd->m_sidemove = 0.0f;

			auto turn_angle = atan2(-sidemove, forwardmove);
			viewangles.y += turn_angle * M_RADPI;
		}
		else if (forwardmove) //-V550
			m_pcmd->m_forwardmove = 0.0f;

		auto strafe_angle = RAD2DEG(atan(15.0f / velocity.Length2D()));

		if (strafe_angle > 90.0f)
			strafe_angle = 90.0f;
		else if (strafe_angle < 0.0f)
			strafe_angle = 0.0f;

		auto temp = Vector(0.0f, viewangles.y - old_yaw, 0.0f);
		temp.y = math::normalize_yaw(temp.y);

		auto yaw_delta = temp.y;
		old_yaw = viewangles.y;

		auto abs_yaw_delta = fabs(yaw_delta);

		if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.0f)
		{
			Vector velocity_angles;
			math::vector_angles(velocity, velocity_angles);

			temp = Vector(0.0f, viewangles.y - velocity_angles.y, 0.0f);
			temp.y = math::normalize_yaw(temp.y);

			auto velocityangle_yawdelta = temp.y;
			auto velocity_degree = get_velocity_degree(velocity.Length2D());

			if (velocityangle_yawdelta <= velocity_degree || velocity.Length2D() <= 15.0f)
			{
				if (-velocity_degree <= velocityangle_yawdelta || velocity.Length2D() <= 15.0f)
				{
					viewangles.y += strafe_angle * turn_direction_modifier;
					m_pcmd->m_sidemove = side_speed * turn_direction_modifier;
				}
				else
				{
					viewangles.y = velocity_angles.y - velocity_degree;
					m_pcmd->m_sidemove = side_speed;
				}
			}
			else
			{
				viewangles.y = velocity_angles.y + velocity_degree;
				m_pcmd->m_sidemove = -side_speed;
			}
		}
		else if (yaw_delta > 0.0f)
			m_pcmd->m_sidemove = -side_speed;
		else if (yaw_delta < 0.0f)
			m_pcmd->m_sidemove = side_speed;

		auto move = Vector(m_pcmd->m_forwardmove, m_pcmd->m_sidemove, 0.0f);
		auto speed = move.Length();

		Vector angles_move;
		math::vector_angles(move, angles_move);

		auto normalized_x = fmod(m_pcmd->m_viewangles.x + 180.0f, 360.0f) - 180.0f;
		auto normalized_y = fmod(m_pcmd->m_viewangles.y + 180.0f, 360.0f) - 180.0f;

		auto yaw = DEG2RAD(normalized_y - viewangles.y + angles_move.y);

		if (normalized_x >= 90.0f || normalized_x <= -90.0f || m_pcmd->m_viewangles.x >= 90.0f && m_pcmd->m_viewangles.x <= 200.0f || m_pcmd->m_viewangles.x <= -90.0f && m_pcmd->m_viewangles.x <= 200.0f) //-V648
			m_pcmd->m_forwardmove = -cos(yaw) * speed;
		else
			m_pcmd->m_forwardmove = cos(yaw) * speed;

		m_pcmd->m_sidemove = sin(yaw) * speed;
	}

}

void DrawBeamPaw(Vector src, Vector end, Color color)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = 0;
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;
	beamInfo.m_pszModelName = "sprites/glow01.vmt";
	beamInfo.m_pszHaloName = "sprites/glow01.vmt";
	beamInfo.m_flHaloScale = 3.0;
	beamInfo.m_flWidth = 4.5f;
	beamInfo.m_flEndWidth = 4.5f;
	beamInfo.m_flFadeLength = 0.5f;
	beamInfo.m_flAmplitude = 0;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.0f;
	beamInfo.m_nStartFrame = 0.0;
	beamInfo.m_flFrameRate = 0.0;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = 1;
	beamInfo.m_bRenderable = true;
	beamInfo.m_flLife = 3;
	beamInfo.m_nFlags = 256 | 512 | 32768;
	Beam_t* myBeam = m_viewrenderbeams()->CreateBeamPoints(beamInfo);
	if (myBeam)
		m_viewrenderbeams()->DrawBeam(myBeam);
}

void movements::MovementTrails()
{

	if (!config_system.g_cfg.misc.movement_trails)
		return;

	if (g_ctx.local()->is_alive() && m_engine()->IsInGame() && m_engine()->IsConnected())
	{
		auto local_pos = g_ctx.local()->GetAbsOrigin();

		static float rainbow;
		rainbow += 0.001f;
		if (rainbow > 1.f)
			rainbow = 0.f;

		auto rainbow_col = Color::FromHSB(rainbow, 1, 1);

		switch (config_system.g_cfg.misc.trail_types)
		{
		case 0:
			DrawBeamPaw(local_pos, Vector(local_pos.x, local_pos.y + 10, local_pos.z), config_system.g_cfg.misc.rainbow_trails ? rainbow_col : config_system.g_cfg.misc.trail_color);
			break;
		case 1:
			iEffects()->EnergySplash(local_pos, Vector(0, 0, 0), true);
			break;
		case 2:
			float current_time = g_ctx.local()->m_nTickBase() * m_globals()->m_intervalpertick;
			trail_pos.push_back({ g_ctx.local()->GetAbsOrigin(), current_time + 1.f });
			Vector world_to_screen;
			Vector last_w2s;
			Vector last_pos = trail_pos[0].position;

			for (int c = 0; c < trail_pos.size(); c++) {
			
				if (trail_pos[c].time < current_time) {
					trail_pos.erase(trail_pos.begin() + c);
					continue;
				}


				auto tracer = trail_pos.at(c);

				if (trail_pos.at(c).position.IsZero())
					continue;

				if (!trail_pos.at(c).position.IsValid())
					continue;

				if (math::WorldToScreen(tracer.position, world_to_screen) && math::WorldToScreen(last_pos, last_w2s))
					g_Render->DrawLine(world_to_screen.x, world_to_screen.y, last_w2s.x, last_w2s.y, config_system.g_cfg.misc.rainbow_trails ? rainbow_col : config_system.g_cfg.misc.trail_color);

				last_pos = tracer.position;
			}
			break;
		}
	}
}

float last_vel = 0.f;
int tick_prev = 0;
void movements::Draw()
{
	if (config_system.g_cfg.misc.speedindicatior) {
		int screenWidth, screenHeight;
		m_engine()->GetScreenSize(screenWidth, screenHeight);
		if (!g_ctx.local()->is_alive())
			return;

		if (!g_ctx.local())
		{
			last_vel = 0;
			return;
		}
		int vel = g_ctx.local()->m_vecVelocity().Length2D() + 0.5;
		int stamina = g_ctx.local()->stamina();

		int logVel = 0;

		Color c = vel == last_vel ? Color(255, 199, 89) : vel < last_vel ? Color(255, 119, 119) : Color(30, 255, 109);
		Color c2 = vel == last_vel ? Color(70, 70, 120) : vel < last_vel ? Color(70, 70, 180) : Color(70, 70, 230);

		if (g_ctx.local()->m_fFlags() & FL_ONGROUND) {
			logVel = g_ctx.local()->m_vecVelocity().Length2D() + 0.5;
		}

		std::string finalVelo = std::to_string(vel);
		std::string finalStamina = std::to_string(stamina);
		std::string tick = std::to_string(m_globals()->m_tickcount);

		std::stringstream ss;
		ss << "(" << finalStamina.c_str() << ")";

		g_Render->DrawString(screenWidth / 2, screenHeight - 100, c, render2::centered_x, c_menu::get().speed_font, finalVelo.c_str());
		g_Render->DrawString2(screenWidth / 2, screenHeight - 115, c2, render2::centered_x, c_menu::get().stamina, ss.str());

		if (tick_prev + 5 < m_globals()->m_tickcount) {
			last_vel = vel;
			tick_prev = m_globals()->m_tickcount;
		}
		if (m_globals()->m_tickcount <= 2000)
		{
			vel = 0;
			logVel = 0;
			finalVelo = "";
			tick_prev = 0;

		}
	}

	if (config_system.g_cfg.misc.velocitygraph)
	{
		if (!g_ctx.local()->is_alive())
			return;
		static std::vector<float> velData(44, 0);

		Vector vecVelocity = g_ctx.local()->m_vecVelocity();
		float currentVelocity = sqrt((vecVelocity.x * vecVelocity.x) + (vecVelocity.y * vecVelocity.y) + (vecVelocity.z * vecVelocity.z));

		velData.erase(velData.begin());
		velData.push_back(currentVelocity);


		int width, height;

		m_engine()->GetScreenSize(width, height);

		g_Render->DrawLine(width / 2 - 100, (height + 550) / 2 + 25, width / 2 - 100, (height + 550) / 2 + 145, Color(100, 100, 100, 175));
		g_Render->DrawLine(width / 2 - 115, (height + 550) / 2 + 130, width / 2 + 115, (height + 550) / 2 + 130, Color(100, 100, 100, 175));

		for (auto i = 0; i < velData.size() - 1; i++)
		{
			int cur = velData.at(i);
			int next = velData.at(i + 1);
			bool landed = velData.at(i) && !velData.at(i + 1);

			g_Render->DrawLine(width / 2 + (velData.size() * 5 / 2) - (i - 1) * 5.f, height / 2 - (std::clamp(cur, 0, 450) * .2f) + 400, width / 2 + (velData.size() * 5 / 2) - i * 5.f, height / 2 - (std::clamp(next, 0, 450) * .2f) + 400, Color(255, 255, 255, 255));
		}
	}

	if (config_system.g_cfg.misc.wasd)
	{
		g_Render->FilledRect(10, 300 + 300, 30, 35, Color(53, 78, 141, 85));
		g_Render->FilledRect(10 + 32, 300 + 300, 30, 35, Color(53, 78, 141, 85));
		g_Render->FilledRect(10 + 32 + 32, 300 + 300, 30, 35, Color(53, 78, 141, 85));
		g_Render->FilledRect(10 + 32 + 32, 300 - 37 + 300, 30, 35, Color(53, 78, 141, 85));
		g_Render->FilledRect(10 + 32, 300 - 37 + 300, 30, 35, Color(53, 78, 141, 85));
		g_Render->FilledRect(10, 200 + 160 + 277, 95, 35, Color(53, 78, 141, 85));
		g_Render->DrawString(18, 305 + 300, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "A");
		g_Render->DrawString(17 + 35, 305 + 300, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "S");
		g_Render->DrawString(17 + 35 + 32, 305 + 300, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "D");
		g_Render->DrawString(17 + 35 + 32, 305 - 35 + 300, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "E");
		g_Render->DrawString(18 + 31, 305 - 35 + 300, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "W");
		g_Render->DrawString(30, 585 - 220 + 277, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "SPACE");

		for (int i = 0; i < 255; i++)
			if (GetAsyncKeyState(i))
				switch (i)
				{
				case 'A':

					g_Render->FilledRect(10, 300 + 300, 30, 35, Color(53, 78, 141, 145));
					g_Render->DrawString(18, 305 + 300, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "A");

					break;

				case 'S':

					g_Render->FilledRect(10 + 32, 300 + 300, 30, 35, Color(53, 78, 141, 145));
					g_Render->DrawString(17 + 35, 305 + 300, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "S");

					break;

				case 'D':

					g_Render->FilledRect(10 + 32 + 32, 300 + 300, 30, 35, Color(53, 78, 141, 145));
					g_Render->DrawString(17 + 35 + 32, 305 + 300, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "D");

					break;

				case 'E':

					g_Render->FilledRect(10 + 32 + 32, 300 - 37 + 300, 30, 35, Color(53, 78, 141, 85));
					g_Render->DrawString(17 + 35 + 32, 305 - 35 + 300, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "E");

					break;

				case 'W':

					g_Render->FilledRect(10 + 32, 300 - 37 + 300, 30, 35, Color(53, 78, 141, 145));
					g_Render->DrawString(18 + 31, 305 - 35 + 300, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "W");

					break;

				case VK_SPACE:

					g_Render->FilledRect(10, 200 + 160 + 277, 95, 35, Color(53, 78, 141, 145));
					g_Render->DrawString(30, 585 - 220 + 277, Color(255, 255, 255, 255), render2::none, c_menu::get().futura_small, "SPACE");

					break;
				}

	}
}

void movements::eBug()
{
}

void movements::jBug(CUserCmd* cmd)
{
	if (config_system.g_cfg.misc.jumpbug && key_binds::get().get_key_bind_state(25))
	{


		float max_radias = D3DX_PI * 2;
		float step = max_radias / 128;
		float xThick = 23;


		Vector pos = g_ctx.local()->GetAbsOrigin();
		for (float a = 0.f; a < max_radias; a += step)
		{
			Vector pt;
			pt.x = (xThick * cos(a)) + pos.x;
			pt.y = (xThick * sin(a)) + pos.y;
			pt.z = pos.z;

			Vector{ 0, 0, 4 };
			Vector pt2 = pt;
			pt2.z -= 6;

			trace_t fag;

			Ray_t ray;
			ray.Init(pt, pt2);

			CTraceFilter flt;
			flt.pSkip = g_ctx.local();
			m_trace()->TraceRay(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

			if (fag.fraction != 1.f && fag.fraction != 0.f && (g_ctx.local()->m_fFlags() & FL_ONGROUND))
			{
				cmd->m_buttons &= ~IN_DUCK; // duck
				cmd->m_buttons |= IN_JUMP;
			}
		}
		for (float a = 0.f; a < max_radias; a += step)
		{
			Vector pt;
			pt.x = ((xThick - 2.f) * cos(a)) + pos.x;
			pt.y = ((xThick - 2.f) * sin(a)) + pos.y;
			pt.z = pos.z;

			Vector{ 0, 0, 4 };
			Vector pt2 = pt;
			pt2.z -= 6;

			trace_t fag;

			Ray_t ray;
			ray.Init(pt, pt2);

			CTraceFilter flt;
			flt.pSkip = g_ctx.local();
			m_trace()->TraceRay(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

			if (fag.fraction != 1.f && fag.fraction != 0.f && g_ctx.local()->m_fFlags() & FL_ONGROUND)
			{
				cmd->m_buttons &= ~IN_DUCK; // duck
				cmd->m_buttons |= IN_JUMP;
			}
		}
		for (float a = 0.f; a < max_radias; a += step)
		{
			Vector pt;
			pt.x = ((xThick - 20.f) * cos(a)) + pos.x;
			pt.y = ((xThick - 20.f) * sin(a)) + pos.y;
			pt.z = pos.z;

			Vector{ 0, 0, 4 };
			Vector pt2 = pt;
			pt2.z -= 6;

			trace_t fag;

			Ray_t ray;
			ray.Init(pt, pt2);

			CTraceFilter flt;
			flt.pSkip = g_ctx.local();
			m_trace()->TraceRay(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);
			//
			if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND))
			{
				cmd->m_buttons |= IN_DUCK;
				if (fag.fraction != 1.f && fag.fraction != 0.f && g_ctx.local()->m_fFlags() & FL_ONGROUND)
				{
					cmd->m_buttons &= ~IN_DUCK; // duck
					cmd->m_buttons |= IN_JUMP;
				}
			}
		}
	}
}

void movements::LJump(CUserCmd* UserCmd)
{
	if (config_system.g_cfg.misc.longjump)
	{
		bool is_lj = false;
		// If we're not pressing the longjump hotkey...
		if (!key_binds::get().get_key_bind_state(23))
		{
			// And we're in the middle of a longjump...
			if (is_lj)
			{
				// Reset the longjump status and disable features.
				is_lj = false;
				m_engine()->ExecuteClientCmd("-jump");
			}

			// Return since we're not using the feature.
			return;
		}

		// If we're on the ground...
		if (g_ctx.local()->m_fFlags() & FL_ONGROUND)
		{

			// And we were in the middle of a longjump...
			if (is_lj)
			{
				// Reset the longjump status and disable features.
				is_lj = false;
				m_engine()->ExecuteClientCmd("-jump");
				return;
			}

			// Otherwise, if we're on the air...
		}
		else {

			// And we're not on the longjump status...
			if (!is_lj)
			{
				// Enable the longjump status and the features.
				is_lj = true;

				static auto cl_sidespeed = m_cvar()->FindVar(crypt_str("cl_sidespeed"));
				auto side_speed = cl_sidespeed->GetFloat();
				Vector engine_angles;
				m_engine()->GetViewAngles(engine_angles);

				auto velocity = g_ctx.local()->m_vecVelocity();

				UserCmd->m_forwardmove = min(5850.0f / velocity.Length2D(), side_speed);
				UserCmd->m_sidemove = UserCmd->m_command_number % 2 ? side_speed : -side_speed;

				auto yaw_velocity = math::calculate_angle(Vector(0.0f, 0.0f, 0.0f), velocity).y;
				auto ideal_rotation = math::clamp(RAD2DEG(atan2(15.0f, velocity.Length2D())), 0.0f, 45.0f);

				auto yaw_rotation = fabs(yaw_velocity - engine_angles.y) + (UserCmd->m_command_number % 2 ? ideal_rotation : -ideal_rotation);
				auto ideal_yaw_rotation = yaw_rotation < 5.0f ? yaw_velocity : engine_angles.y;

				util::RotateMovement(UserCmd, ideal_yaw_rotation);
				return;
			}

			// Keep IN_JUMP to ensure that the auto-strafer will work.
			m_engine()->ExecuteClientCmd("+jump");
		}
	}
}