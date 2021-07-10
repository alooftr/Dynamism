
// By R0B2RT0

#include "animation_system.h"
#include "..\misc\misc.h"
#include "..\misc\logs.h"

std::deque <adjust_data> player_records[65];

void lagcompensation::fsn(ClientFrameStage_t stage)
{
	if (stage != FRAME_NET_UPDATE_END)
		return;

	for (auto i = 1; i < m_globals()->m_maxclients; i++)
	{
		auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

		if (e == g_ctx.local())
			continue;

		if (!valid(i, e))
			continue;

		auto time_delta = abs(TIME_TO_TICKS(e->m_flSimulationTime()) - m_globals()->m_tickcount);

		if (time_delta > 1.0f / m_globals()->m_intervalpertick)
			continue;

		if (player_records[i].empty() || e->m_flSimulationTime() != e->m_flOldSimulationTime())
		{

			if (!player_records[i].empty() && (e->m_vecOrigin() - player_records[i].front().origin).LengthSqr() > 4096.0f)
				for (auto& record : player_records[i])
					record.invalid = true;

			apply_interpolation_flags(e, false);
			e->set_abs_origin(e->m_vecOrigin());
			player_records[i].emplace_front(adjust_data());
			update_player_animations(e);
		}
	}
}

void lagcompensation::apply_interpolation_flags(player_t* e, bool flag)
{
	auto map = e->var_mapping();

	if (!map)
		return;

	for (auto i = 0; i < map->m_nInterpolatedEntries; i++)
		map->m_Entries[i].m_bNeedsToInterpolate = flag;
}

bool lagcompensation::valid(int i, player_t* e)
{
	if (!e->valid(false))
	{
		if (!e->is_alive())
		{
			is_dormant[i] = false;
			player_resolver[i].reset();

			g_ctx.globals.fired_shots[i] = 0;
			g_ctx.globals.missed_shots[i] = 0;
		}
		else if (e->IsDormant())
			is_dormant[i] = true;

		player_records[i].clear();
		return false;
	}

	return true;
}

void lagcompensation::extrapolation(player_t* player, Vector& origin, Vector& velocity, int& flags, bool on_ground)
{
	static const auto sv_gravity = m_cvar()->FindVar(crypt_str("sv_gravity"));
	static const auto sv_jump_impulse = m_cvar()->FindVar(crypt_str("sv_jump_impulse"));

	if (!(flags & FL_ONGROUND))
		velocity.z -= TICKS_TO_TIME(sv_gravity->GetFloat());

	else if (player->m_fFlags() & FL_ONGROUND && !on_ground)
		velocity.z = sv_jump_impulse->GetFloat();

	const auto src = origin;
	auto end = src + velocity * m_globals()->m_intervalpertick;

	Ray_t r;
	r.Init(src, end, player->GetCollideable()->OBBMins(), player->GetCollideable()->OBBMaxs());

	CGameTrace t;
	CTraceFilter filter;
	filter.pSkip = player;

	m_trace()->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

	if (t.fraction != 1.f)
	{
		for (auto i = 0; i < 2; i++)
		{
			velocity -= t.plane.normal * velocity.Dot(t.plane.normal);

			const auto dot = velocity.Dot(t.plane.normal);
			if (dot < 0.f)
				velocity -= Vector(dot * t.plane.normal.x,
					dot * t.plane.normal.y, dot * t.plane.normal.z);

			end = t.endpos + velocity * TICKS_TO_TIME(1.f - t.fraction);

			r.Init(t.endpos, end, player->GetCollideable()->OBBMins(), player->GetCollideable()->OBBMaxs());
			m_trace()->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

			if (t.fraction == 1.f)
				break;
		}
	}

	origin = end = t.endpos;
	end.z -= 2.f;

	r.Init(origin, end, player->GetCollideable()->OBBMins(), player->GetCollideable()->OBBMaxs());
	m_trace()->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

	flags &= ~FL_ONGROUND;

	if (t.DidHit() && t.plane.normal.z > .7f)
		flags |= FL_ONGROUND;
}

void lagcompensation::update_player_animations(player_t* e)
{
	auto animstate = e->get_animation_state();

	if (!animstate)
		return;

	player_info_t player_info;

	auto records = &player_records[e->EntIndex()];

	if (records->empty())
		return;

	adjust_data* previous_record = nullptr;
	auto record = &records->front();
	AnimationLayer animlayers[13];

	const auto simtime_delta = e->m_flSimulationTime() - e->m_flOldSimulationTime();
	const auto choked_ticks = ((simtime_delta / m_globals()->m_intervalpertick) + 0.5);
	const auto simulation_tick_delta = choked_ticks - 2;
	const auto delta_ticks = (std::clamp(TIME_TO_TICKS(m_engine()->GetNetChannelInfo()->GetLatency(1) + m_engine()->GetNetChannelInfo()->GetLatency(0)) + m_globals()->m_tickcount - TIME_TO_TICKS(e->m_flSimulationTime() + get_interpolation()), 0, 100)) - simulation_tick_delta;

	if (delta_ticks > 0 && records->size() >= 2)
	{
		auto ticks_left = static_cast<int>(simulation_tick_delta);
		ticks_left = std::clamp(ticks_left, 1, 10);
		do
		{
			auto data_origin = record->origin;
			auto data_velocity = record->velocity;
			auto data_flags = record->flags;

			extrapolation(e, data_origin, data_velocity, data_flags, !(e->m_fFlags() & FL_ONGROUND));

			record->simulation_time += m_globals()->m_intervalpertick;
			record->origin = data_origin;
			record->velocity = data_velocity;
			--ticks_left;
		} while (ticks_left > 0);
	}
	else if (records->size() >= 2)
		previous_record = &records->at(1);

	const float frametime = m_globals()->m_frametime;
	const float curtime = m_globals()->m_curtime;

	if (animstate->m_iLastClientSideAnimationUpdateFramecount == m_globals()->m_framecount)
		--animstate->m_iLastClientSideAnimationUpdateFramecount;

	if (animstate->m_flLastClientSideAnimationUpdateTime == m_globals()->m_curtime)
		animstate->m_flLastClientSideAnimationUpdateTime = m_globals()->m_curtime - m_globals()->m_intervalpertick;

	g_ctx.globals.updating_animation = true;
	e->update_clientside_animation();
	g_ctx.globals.updating_animation = false;

	m_globals()->m_frametime = frametime;
	m_globals()->m_curtime = curtime;

	auto backup_lower_body_yaw_target = e->m_flLowerBodyYawTarget();
	auto backup_duck_amount = e->m_flDuckAmount();
	auto backup_flags = e->m_fFlags();
	auto backup_eflags = e->m_iEFlags();
	auto backup_curtime = m_globals()->m_curtime;
	auto backup_frametime = m_globals()->m_frametime;
	auto backup_realtime = m_globals()->m_realtime;
	auto backup_framecount = m_globals()->m_framecount;
	auto backup_tickcount = m_globals()->m_tickcount;
	auto backup_interpolation_amount = m_globals()->m_interpolation_amount;

	m_globals()->m_curtime = e->m_flSimulationTime();
	m_globals()->m_frametime = m_globals()->m_intervalpertick;

	if (previous_record)
	{
		Vector vecPreviousOrigin = previous_record->origin;
		int fPreviousFlags = previous_record->flags;

		auto velocity = e->m_vecVelocity();
		auto was_in_air = e->m_fFlags() & FL_ONGROUND && previous_record->flags & FL_ONGROUND;

		auto time_difference = max(m_globals()->m_intervalpertick, e->m_flSimulationTime() - previous_record->simulation_time);
		auto origin_delta = e->m_vecOrigin() - previous_record->origin;

		auto animation_speed = 0.0f;

		if (!origin_delta.IsZero() && TIME_TO_TICKS(time_difference) > 0)
		{
			e->m_vecVelocity() = origin_delta * (1.0f / time_difference);

			/*	if (e->m_vecVelocity().Length2D() >= 100.f)
				{
					player_extrapolation(e, vecPreviousOrigin, e->m_vecVelocity(), e->m_fFlags(), fPreviousFlags & FL_ONGROUND, 8);
					g_ctx.globals.extrapolate_state = 2;
				}
				if (e->m_vecVelocity().z > 0)
				{
					player_extrapolation(e, vecPreviousOrigin, e->m_vecVelocity(), e->m_fFlags(), !(fPreviousFlags & FL_ONGROUND), 3);
					g_ctx.globals.extrapolate_state = 1;
				}*/

			if (e->m_fFlags() & FL_ONGROUND && animlayers[11].m_flWeight > 0.0f && animlayers[11].m_flWeight < 1.0f && animlayers[11].m_flCycle > previous_record->layers[11].m_flCycle)
			{
				auto weapon = e->m_hActiveWeapon().Get();

				if (weapon)
				{
					auto max_speed = 260.0f;
					auto weapon_info = e->m_hActiveWeapon().Get()->get_csweapon_info();

					if (weapon_info)
						max_speed = e->m_bIsScoped() ? weapon_info->flMaxPlayerSpeedAlt : weapon_info->flMaxPlayerSpeed;

					auto modifier = 0.35f * (1.0f - animlayers[11].m_flWeight);

					if (modifier > 0.0f && modifier < 1.0f)
						animation_speed = max_speed * (modifier + 0.55f);
				}
			}

			if (animation_speed > 0.0f)
			{
				animation_speed /= e->m_vecVelocity().Length2D();

				e->m_vecVelocity().x *= animation_speed;
				e->m_vecVelocity().y *= animation_speed;
			}

			if (records->size() >= 3 && time_difference > m_globals()->m_intervalpertick)
			{
				auto previous_velocity = (previous_record->origin - records->at(2).origin) * (1.0f / time_difference);

				if (!previous_velocity.IsZero() && !was_in_air)
				{
					auto current_direction = math::normalize_yaw(RAD2DEG(atan2(e->m_vecVelocity().y, e->m_vecVelocity().x)));
					auto previous_direction = math::normalize_yaw(RAD2DEG(atan2(previous_velocity.y, previous_velocity.x)));

					auto average_direction = current_direction - previous_direction;
					average_direction = DEG2RAD(math::normalize_yaw(current_direction + average_direction * 0.5f));

					auto direction_cos = cos(average_direction);
					auto dirrection_sin = sin(average_direction);

					auto velocity_speed = e->m_vecVelocity().Length2D();

					e->m_vecVelocity().x = direction_cos * velocity_speed;
					e->m_vecVelocity().y = dirrection_sin * velocity_speed;
				}
			}

			if (!(e->m_fFlags() & FL_ONGROUND))
			{
				static auto sv_gravity = m_cvar()->FindVar(crypt_str("sv_gravity"));

				auto fixed_timing = math::clamp(time_difference, m_globals()->m_intervalpertick - get_interpolation(), 1.0f);
				e->m_vecVelocity().z -= sv_gravity->GetFloat() * fixed_timing * 0.5f;
			}
			else
				e->m_vecVelocity().z = 0.0f;
		}
	}

	if (!m_engine()->GetPlayerInfo(e->EntIndex(), &player_info))
		return;

	memcpy(animlayers, e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));
	memcpy(record->layers, animlayers, e->animlayer_count() * sizeof(AnimationLayer));

	m_globals()->m_curtime = e->m_flSimulationTime();
	m_globals()->m_frametime = m_globals()->m_intervalpertick;

	auto velocity = e->m_vecVelocity();
	auto current_weapon = e->m_hActiveWeapon().Get();

	if (e->m_fFlags() & FL_ONGROUND) {
		if (animlayers[6].m_flPlaybackRate == 0.f) {
			velocity.Zero();
		}
		else
		{
			const auto avg_speed = velocity.Length2D();
			if (avg_speed != 0.f) {
				const auto weight = animlayers[11].m_flWeight;

				const auto speed_as_portion = 0.58f - (weight - 1.f) * 0.38f;

				const auto avg_speed_modifier = speed_as_portion * (current_weapon ? max(current_weapon->get_csweapon_info()->flMaxPlayerSpeed, 0.001f) : 260.f);

				if (weight >= 1.f && avg_speed > avg_speed_modifier
					|| weight < 1.f && (avg_speed_modifier > avg_speed || weight > 0.f)) {
					velocity.x /= avg_speed;
					velocity.y /= avg_speed;

					velocity.x *= avg_speed_modifier;
					velocity.y *= avg_speed_modifier;
				}
			}
		}
	}

	e->m_iEFlags() &= ~0x1800;

	if (animlayers[6].m_flWeight == 0.0f || animlayers[6].m_flPlaybackRate == 0.0f)
		e->m_vecVelocity().Zero();

	e->m_iEFlags() &= ~0x1000;

	memcpy(animlayers, e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));

	e->m_bClientSideAnimation() = true;

	if (e->get_animlayers()[5].m_flWeight)
		e->m_fFlags() |= FL_ONGROUND;

	e->m_iEFlags() &= ~0x1000;
	e->m_vecAbsVelocity() = e->m_vecVelocity();
	c_baseplayeranimationstate state;

	bool old_onground = animstate->m_bOnGround;
	bool new_onladder = !animstate->m_bOnGround && e->get_move_type() == MOVETYPE_LADDER;
	bool new_onground = e->m_fFlags() & FL_ONGROUND;

	bool just_landed;
	if (old_onground != new_onground && new_onground)
		just_landed = true;
	else
		just_landed = false;

	if (animstate->m_bOnGround)
	{
		if (!animstate->m_bInHitGroundAnimation)
		{
			if (just_landed || new_onladder)
			{
				int landing_activity = animstate->time_since_in_air() > 1.0f ? ACT_CSGO_LAND_HEAVY : ACT_CSGO_LAND_LIGHT;
				record->layers[5].m_nSequence = landing_activity;
				record->layers[5].m_flCycle = 0.0f;
				animstate->m_bInHitGroundAnimation = true;
			}
		}
	}

	if (!e->get_move_type() == MOVETYPE_LADDER)
	{
		if (record->layers[5].m_flWeight > 0.0f)
		{
			float v175 = (animstate->time_since_in_air() - 0.2f) * -5.0f;
			v175 = std::clamp(v175, 0.0f, 1.0f);
			float newlayer5_weight = ((3.0f - (v175 + v175)) * (v175 * v175)) * record->layers[5].m_flWeight;
			record->layers[5].m_flWeight = newlayer5_weight;
		}
	}

	e->m_iEFlags() &= ~0x1800;

	if (e->m_fFlags() & FL_ONGROUND && e->m_vecVelocity().Length() > 0.0f && animlayers[6].m_flWeight <= 0.0f)
		e->m_vecVelocity().Zero();

	e->m_vecAbsVelocity() = e->m_vecVelocity();
	e->m_bClientSideAnimation() = true;

	if (is_dormant[e->EntIndex()])
	{
		is_dormant[e->EntIndex()] = false;

		if (e->m_fFlags() & FL_ONGROUND)
		{
			animstate->m_bOnGround = true;
			animstate->m_bInHitGroundAnimation = false;
		}

		animstate->time_since_in_air() = 0.0f;
		// animstate->m_flGoalFeetYaw = math::normalize_yaw(animstate->m_flEyeYaw);
	}

	memcpy(&state, animstate, sizeof(c_baseplayeranimationstate));

	m_globals()->m_curtime = e->m_flSimulationTime();
	m_globals()->m_frametime = m_globals()->m_intervalpertick;

	auto realtime_backup = m_globals()->m_realtime;
	auto absoluteframetime = m_globals()->m_absoluteframetime;
	auto framecount = m_globals()->m_framecount;
	auto tickcount = m_globals()->m_tickcount;
	auto interpolation_amount = m_globals()->m_interpolation_amount;
	int ticks = TIME_TO_TICKS(record->simulation_time);

	m_globals()->m_realtime = realtime_backup;
	m_globals()->m_curtime = e->m_flSimulationTime();
	m_globals()->m_frametime = m_globals()->m_intervalpertick;
	m_globals()->m_absoluteframetime = m_globals()->m_intervalpertick;
	m_globals()->m_framecount = ticks;
	m_globals()->m_tickcount = ticks;
	m_globals()->m_interpolation_amount = ticks;

	g_ctx.globals.updating_animation = true;
	e->update_clientside_animation();
	g_ctx.globals.updating_animation = false;

	m_globals()->m_realtime = realtime_backup;
	m_globals()->m_curtime = curtime;
	m_globals()->m_frametime = frametime;
	m_globals()->m_absoluteframetime = absoluteframetime;
	m_globals()->m_framecount = framecount;
	m_globals()->m_tickcount = tickcount;
	m_globals()->m_interpolation_amount = interpolation_amount;

	memcpy(animstate, &state, sizeof(c_baseplayeranimationstate));

	auto setup_matrix = [&](player_t* e, AnimationLayer* layers, const int& matrix) -> void
	{
		e->invalidate_physics_recursive(8);
		AnimationLayer backup_layers[13];
		memcpy(backup_layers, e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));
		memcpy(e->get_animlayers(), layers, e->animlayer_count() * sizeof(AnimationLayer));

		const auto backup_occlusion_flags = e->m_iOcclusionFlags();
		const auto backup_occlusion_framecount = e->m_iOcclusionFramecount();
		auto m_flSimulationTime = e->m_flSimulationTime();
		e->m_iOcclusionFlags() = 0;
		e->m_iOcclusionFramecount() = 0;
		e->m_BoneAccessor().m_ReadableBones = e->m_BoneAccessor().m_WritableBones = 0;
		e->m_iMostRecentModelBoneCounter() = 0;
		e->m_flLastBoneSetupTime() = -FLT_MAX;
		bool resolve = false;
		e->m_fEffects() |= 8;

		if (!resolve) {
			e->SetupBones(m_Matrix, 128, BONE_USED_BY_ANYTHING, m_flSimulationTime);
			resolve = true;
		}
		else {
			const auto backup_bone = e->m_BoneAccessor().GetBoneArrayForWrite();
			e->m_BoneAccessor().SetBoneArrayForWrite(m_res);
			e->SetupBones(nullptr, -1, BONE_USED_BY_ANYTHING, m_flSimulationTime);
			e->m_BoneAccessor().SetBoneArrayForWrite(backup_bone);
			resolve = false;
		}

		e->m_iOcclusionFlags() = backup_occlusion_flags;
		e->m_iOcclusionFramecount() = backup_occlusion_framecount;
		e->m_fEffects() &= ~8;

		switch (matrix)
		{
		case MAIN:
			e->setup_bones_fixed(record->matrixes_data.main, BONE_USED_BY_ANYTHING);
			break;
		case NONE:
			e->setup_bones_fixed(record->matrixes_data.zero, BONE_USED_BY_HITBOX);
			break;
		case FIRST:
			e->setup_bones_fixed(record->matrixes_data.first, BONE_USED_BY_HITBOX);
			break;
		case SECOND:
			e->setup_bones_fixed(record->matrixes_data.second, BONE_USED_BY_HITBOX);
			break;
		}

		memcpy(e->get_animlayers(), backup_layers, e->animlayer_count() * sizeof(AnimationLayer));
	};

	if (!player_info.fakeplayer && g_ctx.local()->is_alive() && e->m_iTeamNum() != g_ctx.local()->m_iTeamNum())
	{
		animstate->m_flGoalFeetYaw = previous_goal_feet_yaw[e->EntIndex()];

		g_ctx.globals.updating_animation = true;
		e->update_clientside_animation();
		g_ctx.globals.updating_animation = false;

		previous_goal_feet_yaw[e->EntIndex()] = animstate->m_flGoalFeetYaw;

		animstate->m_flGoalFeetYaw = math::normalize_yaw(e->m_angEyeAngles().y);

		g_ctx.globals.updating_animation = true;
		e->update_clientside_animation();
		g_ctx.globals.updating_animation = false;

		setup_matrix(e, animlayers, NONE);
		memcpy(player_resolver[e->EntIndex()].resolver_anim_layer[0], e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));
		player_resolver[e->EntIndex()].resolver_goal_feet_yaw[0] = e->m_angEyeAngles().y;

		animstate->m_flGoalFeetYaw = math::normalize_yaw(e->m_angEyeAngles().y - e->get_max_desync_delta());

		g_ctx.globals.updating_animation = true;
		e->update_clientside_animation();
		g_ctx.globals.updating_animation = false;

		setup_matrix(e, animlayers, FIRST);
		memcpy(player_resolver[e->EntIndex()].resolver_anim_layer[1], e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));
		player_resolver[e->EntIndex()].resolver_goal_feet_yaw[1] = e->m_angEyeAngles().y;

		animstate->m_flGoalFeetYaw = math::normalize_yaw(e->m_angEyeAngles().y + e->get_max_desync_delta());

		g_ctx.globals.updating_animation = true;
		e->update_clientside_animation();
		g_ctx.globals.updating_animation = false;

		setup_matrix(e, animlayers, SECOND);
		memcpy(player_resolver[e->EntIndex()].resolver_anim_layer[2], e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));
		player_resolver[e->EntIndex()].resolver_goal_feet_yaw[2] = e->m_angEyeAngles().y;

		player_resolver[e->EntIndex()].initialize(e, record, e->m_angEyeAngles().x);
		player_resolver[e->EntIndex()].ResolveAngles();

		switch (record->side)
		{
		case RESOLVER_ORIGINAL:
			animstate->m_flGoalFeetYaw = previous_goal_feet_yaw[e->EntIndex()];
			break;
		case RESOLVER_ZERO:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(e->m_angEyeAngles().y);
			break;
		case RESOLVER_FIRST:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(e->m_angEyeAngles().y + e->get_max_desync_delta());
			break;
		case RESOLVER_SECOND:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(e->m_angEyeAngles().y - e->get_max_desync_delta());
			break;
		case RESOLVER_LOW_FIRST:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(e->m_angEyeAngles().y + min(45.0f, e->get_max_desync_delta()));
			break;
		case RESOLVER_LOW_SECOND:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(e->m_angEyeAngles().y - min(45.0f, e->get_max_desync_delta()));
			break;
		}

		e->m_angEyeAngles().x = player_resolver[e->EntIndex()].resolve_pitch();
	}

	g_ctx.globals.updating_animation = true;
	e->update_clientside_animation();
	g_ctx.globals.updating_animation = false;

	setup_matrix(e, animlayers, MAIN);
	memcpy(e->m_CachedBoneData().Base(), record->matrixes_data.main, e->m_CachedBoneData().Count() * sizeof(matrix3x4_t));

	// Restore
	m_globals()->m_curtime = backup_curtime;
	m_globals()->m_frametime = backup_frametime;
	e->m_flLowerBodyYawTarget() = backup_lower_body_yaw_target;
	e->m_flDuckAmount() = backup_duck_amount;
	e->m_fFlags() = backup_flags;
	e->m_iEFlags() = backup_eflags;
	m_globals()->m_interpolation_amount = backup_curtime;

	memcpy(e->get_animlayers(), animlayers, e->animlayer_count() * sizeof(AnimationLayer));
	memcpy(player_resolver[e->EntIndex()].anim_layers, animlayers, e->animlayer_count() * sizeof(AnimationLayer));

	player_resolver[e->EntIndex()].resolver_goal_feet_yaw[0] = player_resolver[e->EntIndex()].resolver_anim_layer[0]->m_flPlaybackRate;
	player_resolver[e->EntIndex()].resolver_goal_feet_yaw[1] = player_resolver[e->EntIndex()].resolver_anim_layer[1]->m_flPlaybackRate;
	player_resolver[e->EntIndex()].resolver_goal_feet_yaw[2] = player_resolver[e->EntIndex()].resolver_anim_layer[2]->m_flPlaybackRate;

	record->store_data(e, false);

	if (e->m_flSimulationTime() < e->m_flOldSimulationTime())
		record->invalid = true;
}

float lagcompensation::get_interpolation()
{
	static auto cl_interp = m_cvar()->FindVar(crypt_str("cl_interp"));
	static auto cl_interp_ratio = m_cvar()->FindVar(crypt_str("cl_interp_ratio"));
	static auto sv_client_min_interp_ratio = m_cvar()->FindVar(crypt_str("sv_client_min_interp_ratio"));
	static auto sv_client_max_interp_ratio = m_cvar()->FindVar(crypt_str("sv_client_max_interp_ratio"));
	static auto cl_updaterate = m_cvar()->FindVar(crypt_str("cl_updaterate"));
	static auto sv_minupdaterate = m_cvar()->FindVar(crypt_str("sv_minupdaterate"));
	static auto sv_maxupdaterate = m_cvar()->FindVar(crypt_str("sv_maxupdaterate"));

	auto updaterate = math::clamp(cl_updaterate->GetFloat(), sv_minupdaterate->GetFloat(), sv_maxupdaterate->GetFloat());
	auto lerp_ratio = math::clamp(cl_interp_ratio->GetFloat(), sv_client_min_interp_ratio->GetFloat(), sv_client_max_interp_ratio->GetFloat());

	return math::clamp(lerp_ratio / updaterate, cl_interp->GetFloat(), 1.0f);
}