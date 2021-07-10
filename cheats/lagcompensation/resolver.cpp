
// By R0B2RT0

#include "animation_system.h"
#include "..\ragebot\aim.h"
#include "..\autowall\autowall.h"

void resolver::initialize(player_t* e, adjust_data* record, const float& pitch)
{
	player = e;
	player_record = record;

	original_pitch = math::normalize_pitch(pitch);
}

static auto get_rotated_pos = [](Vector start, float rotation, float distance)
{
	float rad = DEG2RAD(rotation);
	start.x += cos(rad) * distance;
	start.y += sin(rad) * distance;

	return start;
};

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	side = false;
	fake = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}

float AngleDiff(float destAngle, float srcAngle) {
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.0f);
	if (destAngle > srcAngle) {
		if (delta >= 180)
			delta -= 360;
	}
	else {
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}

float GetBackwardYaw(player_t* player) {
	return math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y - 180.f;
}

void resolver::StoreAntifreestand()
{
	if (!g_ctx.local()->is_alive())
		return;

	if (!g_ctx.globals.weapon)
		return;

	for (int i = 1; i < m_globals()->m_maxclients; ++i)
	{
		auto get_player = m_entitylist()->GetClientEntity(i);

		if (!player || !player->is_alive() || player->IsDormant() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			continue;

		bool Autowalled = false, HitSide1 = false, HitSide2 = false;

		auto idx = player->EntIndex();

		float angToLocal = math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y;

		Vector ViewPoint = g_ctx.local()->m_vecOrigin() + Vector(0, 0, 90);
		Vector2D Side1 = { (45 * sin(DEG2RAD(angToLocal))),(45 * cos(DEG2RAD(angToLocal))) };
		Vector2D Side2 = { (45 * sin(DEG2RAD(angToLocal + 180))) ,(45 * cos(DEG2RAD(angToLocal + 180))) };

		Vector2D Side3 = { (50 * sin(DEG2RAD(angToLocal))),(50 * cos(DEG2RAD(angToLocal))) };
		Vector2D Side4 = { (50 * sin(DEG2RAD(angToLocal + 180))) ,(50 * cos(DEG2RAD(angToLocal + 180))) };

		Vector Origin = player->m_vecOrigin();

		Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

		Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

		for (int side = 0; side < 2; side++)
		{
			Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
			Vector ViewPointAutowall = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

			if (autowall::get().can_hit(OriginAutowall, ViewPoint))
			{
				if (side == -1)
				{
					HitSide1 = true;
					FreestandSide[idx] = -1;
				}
				else if (side == 1)
				{
					HitSide2 = true;
					FreestandSide[idx] = 1;
				}

				Autowalled = true;
			}
			else
			{
				for (int sidealternative = 0; sidealternative < 2; sidealternative++)
				{
					Vector ViewPointAutowallalternative = { Origin.x + OriginLeftRight[sidealternative].x,  Origin.y - OriginLeftRight[sidealternative].y , Origin.z + 90 };

					if (autowall::get().can_hit(ViewPointAutowallalternative, ViewPointAutowall))
					{
						if (sidealternative == 0)
						{
							HitSide1 = true;
							FreestandSide[idx] = -1;
							side = -1;
						}
						else if (sidealternative == 1)
						{
							HitSide2 = true;
							FreestandSide[idx] = 1;
							side = 1;
						}

						Autowalled = true;
					}
				}
			}
		}
	}
}

void resolver::ResolveAngles()
{
	float no_side = FLT_MAX;
	float resolve_value = -75.f;
	float resolve_value1 = 75.f;
	float resolve_yaw = 65;
	auto animstate = player->get_animation_state();
	float EyeDelta = player->m_angEyeAngles().y - animstate->m_flGoalFeetYaw;
	bool LowDelta = EyeDelta <= 35.f;
	int Side = (EyeDelta > 0.f) ? -1 : 1;
	float desync_delta = LowDelta ? player->get_max_desync_delta() / 2 : player->get_max_desync_delta();

	AnimationLayer layers[13];
	AnimationLayer moveLayers[3][13];
	memcpy(moveLayers, player->get_animlayers(), sizeof(AnimationLayer) * 13);

	player_info_t player_info;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	if (player_info.fakeplayer || !g_ctx.local()->is_alive() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	if (!animstate)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	const auto& layer = player->get_animlayers()[12];
    if (layer.m_flWeight < 0.0003f)
    {
		if (layer.m_flWeight >= 0.00014f)
		{
			float torso_yaw = abs(animstate->m_flCurrentTorsoYaw);
			float lean = abs(animstate->m_flUnknownVelocityLean);

			static float old_torso = 0.f;
			static float old_lean = 0.f;

			if (old_torso > torso_yaw && old_lean > lean)
			    side = -1;
			else if (old_torso < torso_yaw && old_lean < lean)
				side = 1;
			else
			{
				if (side == 1)
					side = -1;
				else if (side == -1)
					side = 1;
			}

			old_torso = torso_yaw;
			old_lean = lean;
		}
	}

	auto valid_move = true;
	if (animstate->m_velocity > 0.1f)
		valid_move = animstate->m_flTimeSinceStartedMoving < 0.22f;

	auto idx = player->EntIndex();

	static float last_anim_update[64];
	float desync_amount = player->get_max_desync_delta() * 0.87f;
	auto full = player->get_animlayers()[3].m_flWeight == 0.0f && player->get_animlayers()[3].m_flCycle == 0.0f;

	bool is_player_zero = false;
	bool is_player_faking = false;
	int positives = 0;
	int negatives = 0;

	resolver_history res_history = HISTORY_UNKNOWN;

	for (auto it = lagcompensation::get().player_sets.begin(); it != lagcompensation::get().player_sets.end(); ++it)
		if (it->id == player_info.steamID64)
		{
			res_history = it->res_type;
			is_player_faking = it->faking;
			positives = it->pos;
			negatives = it->neg;

			break;
		}

	if (res_history == HISTORY_ZERO)
		is_player_zero = true;

	if (valid_move && player_record->layers[3].m_flWeight == 0.f && player_record->layers[3].m_flCycle == 0.f && player_record->layers[6].m_flWeight == 0.f)
	{
		auto m_delta = AngleDiff(player->m_angEyeAngles().y, gfy_default);
		side = 2 * (m_delta <= 0.0f) - 1;
		type = LBY;
	}
	else if (!valid_move && !(int(player_record->layers[12].m_flWeight * 1000.f)) && static_cast<int>(player_record->layers[6].m_flWeight * 1000.f) == static_cast<int>(previous_layers[6].m_flWeight * 1000.f))
	{
		float delta_first = abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[0][6].m_flPlaybackRate);
		float delta_second = abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[2][6].m_flPlaybackRate);
		float delta_third = abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[1][6].m_flPlaybackRate);

		if (delta_first < delta_second || delta_third <= delta_second || (delta_second * 1000.f))
		{
			if (delta_first >= delta_third && delta_second > delta_third && !(delta_third * 1000.f))
				side = 1;
		}
		else
			side = -1;

		type = LAYERS;
	}
	else
	{
		if (m_globals()->m_curtime - lock_side > 2.0f)
		{
			auto fire_data_first = autowall::get().wall_penetration(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player);
			auto fire_data_second = autowall::get().wall_penetration(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player);

			if (fire_data_first.visible != fire_data_second.visible)
			{
				side = fire_data_second.visible;
				lock_side = m_globals()->m_curtime;

				type = TRACE;
			}
			else
			{
				if (abs(positives - negatives) > 3)
					side = positives > negatives;
				else if (fire_data_first.damage != fire_data_second.damage)
					side = fire_data_first.damage < fire_data_second.damage;

				type = DIRECTIONAL;
			}
		}
		else
		{
			if (g_ctx.globals.missed_shots[player->EntIndex()] >= 2 || g_ctx.globals.missed_shots[player->EntIndex()] && aim::get().last_target[player->EntIndex()].record.type != LBY)
			{
				if (g_ctx.globals.missed_shots[idx] == 0 || FreestandSide[idx] == 0)
				{
					if (player->m_vecVelocity().Length2D() <= 0.1f && full && player->get_animlayers()[6].m_flWeight == 0.0f)
					{
						FreestandSide[idx] = 2 * (AngleDiff(player->m_angEyeAngles().y, animstate->m_flGoalFeetYaw) <= 0.0f) - 1;
						last_anim_update[idx] = m_globals()->m_realtime;
						desync_amount = 58.f;
					}
					else if (last_anim_update[idx] + 2.f < m_globals()->m_realtime || FreestandSide[idx] == 0) // ANTI-FREESTAND
						StoreAntifreestand();
				}
			}
			else
			{
				switch (g_ctx.globals.missed_shots[idx] % 6) {
				case 0:
					animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + desync_amount * FreestandSide[idx]);

					side = FreestandSide[idx] > 0;
					break;
				case 1:
					animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - desync_amount * FreestandSide[idx]);

					side = FreestandSide[idx] < 0;
					break;
				case 2:
					animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + (desync_amount * 0.16f) * FreestandSide[idx]);

					side = FreestandSide[idx] > 0;
					break;
				case 3:

					animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - (desync_amount * 0.16f) * FreestandSide[idx]);

					side = FreestandSide[idx] < 0;
					break;
				case 4:

					animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + (desync_amount * 0.7f) * FreestandSide[idx]);

					side = FreestandSide[idx] > 0;
					break;

				case 5:
					animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - (desync_amount * 0.7f) * FreestandSide[idx]);

					side = FreestandSide[idx] < 0;
					break;
				}
			}
		}
	}

	auto choked = abs(TIME_TO_TICKS(player->m_flSimulationTime() - player->m_flOldSimulationTime()) - 1);

	if (side)
	{
		player_record->type = type;
		player_record->side = RESOLVER_FIRST;
	}
	else
	{
		player_record->type = type;
		player_record->side = RESOLVER_SECOND;
	}
}

float GRD_TO_BOG(float GRD) {
	return (M_PI / 180) * GRD;
}

float angle_mod(float flAngle)
{
	return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65535));
}

float resolver::resolve_pitch()
{
	return original_pitch;
}