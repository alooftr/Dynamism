// By R0B2RT0

#include "local_animations.h"

void update_layers(player_t* e, AnimationLayer* layers) {
	auto last_movetype = e->get_move_type();
	auto movetype = e->get_move_type();
}

bool fresh_tick()
{
	static int old_tick_count;

	if (old_tick_count != m_globals()->m_tickcount)
	{
		old_tick_count = m_globals()->m_tickcount;
		return true;
	}

	return false;
}

void local_animations::run(ClientFrameStage_t stage)
{

	static bool bInitialize = false;
	static int Old_Flags, Old_MoveType;
	static AnimationLayer Last_AnimationLayers[13];
	static AnimationLayer WeaponSwitchLayer;

	c_baseplayeranimationstate* AnimationState = g_ctx.local()->get_animation_state();
	WeaponSwitchLayer = g_ctx.local()->get_animlayers()[1];

	const int iPreviousFramecount = m_globals()->m_framecount;
	const float flPreviousRealtime = m_globals()->m_realtime;
	const float flPreviousCurtime = m_globals()->m_curtime;
	const float flPreviousFrametime = m_globals()->m_frametime;
	const float flPreviousAbsoluteFrametime = m_globals()->m_absoluteframetime;
	const float flPreviousInterpolationAmount = m_globals()->m_interpolation_amount;
	const int iPreviousTickCount = m_globals()->m_tickcount;

	const float flTime = TICKS_TO_TIME(g_ctx.local()->m_nTickBase());
	const float flThiccTime = (flTime / m_globals()->m_intervalpertick) + 1.0f;

	m_globals()->m_realtime = flTime;
	m_globals()->m_curtime = flTime;
	m_globals()->m_frametime = m_globals()->m_intervalpertick;
	m_globals()->m_absoluteframetime = m_globals()->m_intervalpertick;
	m_globals()->m_framecount = flThiccTime;
	m_globals()->m_tickcount = flThiccTime;
	m_globals()->m_interpolation_amount = 0.f;

	auto BackupEFlags = g_ctx.local()->m_iEFlags();
	auto BackupAbsoluteVelocity = g_ctx.local()->m_vecAbsVelocity();
	g_ctx.local()->m_vecAbsVelocity() = g_ctx.local()->m_vecVelocity();
	g_ctx.local()->m_iEFlags() &= ~0x1000u;
	g_ctx.local()->get_animlayers()[1] = WeaponSwitchLayer;

	if (g_ctx.local()->m_vecVelocity().Length() <= 1.0f) {
		if (AnimationState->m_bOnGround) {
			g_ctx.local()->get_animlayers()[3].m_flCycle = 0.0;
			g_ctx.local()->get_animlayers()[3].m_flWeight = 0.0;
		}
	}

	g_ctx.local()->m_vecAbsVelocity() = BackupAbsoluteVelocity;
	g_ctx.local()->m_iEFlags() = BackupEFlags;

	m_globals()->m_realtime = flPreviousRealtime;
	m_globals()->m_curtime = flPreviousCurtime;
	m_globals()->m_frametime = flPreviousFrametime;
	m_globals()->m_absoluteframetime = flPreviousAbsoluteFrametime;
	m_globals()->m_interpolation_amount = flPreviousInterpolationAmount;
	m_globals()->m_framecount = iPreviousFramecount;
	m_globals()->m_tickcount = iPreviousTickCount;

	Old_Flags = g_ctx.local()->m_fFlags();
	Old_MoveType = g_ctx.local()->get_move_type();

	if (!fakelag::get().condition && key_binds::get().get_key_bind_state(20))
	{
		if (stage == FRAME_NET_UPDATE_END)
		{
			fake_server_update = false;

			if (g_ctx.local()->m_flSimulationTime() != fake_simulation_time)
			{
				fake_server_update = true;
				fake_simulation_time = g_ctx.local()->m_flSimulationTime();
			}

			update_fake_animations();
		}
		else if (stage == FRAME_RENDER_START)
		{
			auto animstate = g_ctx.local()->get_animation_state();

			if (!animstate)
				return;

			real_server_update = false;

			if (g_ctx.local()->m_flSimulationTime() != real_simulation_time)
			{
				real_server_update = true;
				real_simulation_time = g_ctx.local()->m_flSimulationTime();
			}

			update_local_animations(animstate);
		}
	}
	else if (stage == FRAME_RENDER_START)
	{
		auto animstate = g_ctx.local()->get_animation_state();

		if (!animstate)
			return;

		real_server_update = false;
		fake_server_update = false;

		if (g_ctx.local()->m_flSimulationTime() != real_simulation_time || g_ctx.local()->m_flSimulationTime() != fake_simulation_time)
		{
			real_server_update = true;
			fake_server_update = true;

			real_simulation_time = g_ctx.local()->m_flSimulationTime();
			fake_simulation_time = g_ctx.local()->m_flSimulationTime();
		}

		update_fake_animations();
		update_local_animations(animstate);
	}
}

void local_animations::update_prediction_animations()
{
	auto alloc = !local_data.prediction_animstate;
	auto change = !alloc && handle != &g_ctx.local()->GetRefEHandle();
	auto reset = !alloc && !change && g_ctx.local()->m_flSpawnTime() != spawntime;

	if (change)
		m_memalloc()->Free(local_data.prediction_animstate);

	if (reset)
	{
		util::reset_state(local_data.prediction_animstate);
		spawntime = g_ctx.local()->m_flSpawnTime();
	}

	if (alloc || change)
	{
		local_data.prediction_animstate = (c_baseplayeranimationstate*)m_memalloc()->Alloc(sizeof(c_baseplayeranimationstate));

		if (local_data.prediction_animstate)
			util::create_state(local_data.prediction_animstate, g_ctx.local());

		handle = (CBaseHandle*)&g_ctx.local()->GetRefEHandle();
		spawntime = g_ctx.local()->m_flSpawnTime();
	}

	if (!alloc && !change && !reset)
	{
		float pose_parameter[24];
		memcpy(pose_parameter, &g_ctx.local()->m_flPoseParameter(), 24 * sizeof(float));

		AnimationLayer layers[13];
		memcpy(layers, g_ctx.local()->get_animlayers(), g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));

		local_data.prediction_animstate->m_pBaseEntity = g_ctx.local();
		util::update_state(local_data.prediction_animstate, ZERO);

		g_ctx.local()->setup_bones_fixed(g_ctx.globals.prediction_matrix, BONE_USED_BY_HITBOX);

		memcpy(&g_ctx.local()->m_flPoseParameter(), pose_parameter, 24 * sizeof(float));
		memcpy(g_ctx.local()->get_animlayers(), layers, g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));
	}
}

void local_animations::update_fake_animations()
{
	auto alloc = !local_data.animstate;
	auto change = !alloc && handle != &g_ctx.local()->GetRefEHandle();
	auto reset = !alloc && !change && g_ctx.local()->m_flSpawnTime() != spawntime;

	if (change)
		m_memalloc()->Free(local_data.animstate);

	if (reset)
	{
		util::reset_state(local_data.animstate);
		spawntime = g_ctx.local()->m_flSpawnTime();
	}

	if (alloc || change)
	{
		local_data.animstate = (c_baseplayeranimationstate*)m_memalloc()->Alloc(sizeof(c_baseplayeranimationstate));

		if (local_data.animstate)
			util::create_state(local_data.animstate, g_ctx.local());

		handle = (CBaseHandle*)&g_ctx.local()->GetRefEHandle();
		spawntime = g_ctx.local()->m_flSpawnTime();
	}

	g_ctx.local()->m_fEffects() |= 0x8;
	g_ctx.local()->invalidate_bone_cache();

	if (!alloc && !change && !reset && fake_server_update)
	{
		float pose_parameter[24];
		memcpy(pose_parameter, &g_ctx.local()->m_flPoseParameter(), 24 * sizeof(float));

		AnimationLayer layers[13];
		memcpy(layers, g_ctx.local()->get_animlayers(), g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));

		auto backup_frametime = m_globals()->m_frametime;
		auto backup_curtime = m_globals()->m_curtime;

		m_globals()->m_frametime = m_globals()->m_intervalpertick;
		m_globals()->m_curtime = g_ctx.local()->m_flSimulationTime();

		local_data.animstate->m_pBaseEntity = g_ctx.local();
		util::update_state(local_data.animstate, local_animations::get().local_data.fake_angles);

		local_data.animstate->m_bInHitGroundAnimation = false;
		local_data.animstate->m_fLandingDuckAdditiveSomething = 0.0f;
		local_data.animstate->m_flHeadHeightOrOffsetFromHittingGroundAnimation = 1.0f;

		g_ctx.local()->invalidate_bone_cache();
		g_ctx.local()->setup_bones_fixed(g_ctx.globals.fake_matrix, BONE_USED_BY_ANYTHING);
		local_data.visualize_lag = config_system.g_cfg.player.visualize_lag;

		if (!local_data.visualize_lag)
		{
			for (auto& i : g_ctx.globals.fake_matrix)
			{
				i[0][3] -= g_ctx.local()->GetRenderOrigin().x;
				i[1][3] -= g_ctx.local()->GetRenderOrigin().y;
				i[2][3] -= g_ctx.local()->GetRenderOrigin().z;
			}
		}

		m_globals()->m_frametime = backup_frametime;
		m_globals()->m_curtime = backup_curtime;

		memcpy(&g_ctx.local()->m_flPoseParameter(), pose_parameter, 24 * sizeof(float));
		memcpy(g_ctx.local()->get_animlayers(), layers, g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));
	}

	g_ctx.local()->m_fEffects() &= ~0x8;
}

bool CreateMatrix(player_t* player, matrix3x4_t* matrix, float curtime) { // nemesis

	float BackupCurtime = m_globals()->m_curtime;
	float BackupFrameTime = m_globals()->m_frametime;
	float BackupFrameCount = m_globals()->m_framecount;
	m_globals()->m_frametime = m_globals()->m_intervalpertick;
	m_globals()->m_curtime = curtime;
	auto BackupEffects = player->m_fEffects();
	auto BackupOcclusionCheckFlags = g_ctx.local()->m_iOcclusionFlags();// m_iLastOcclusionCheckFlags 
	auto BackupLastOcclusionCheckFrameCount = g_ctx.local()->m_iOcclusionFramecount();// m_iLastOcclusionCheckFrameCount 
	auto v22 = *(uintptr_t*)(player + 0x68);
	auto BackupInverseKinematics = *(uintptr_t*)(0x2670 + player);

	static std::uintptr_t uOffset = netvars::get().get_offset(crypt_str("CBaseAnimating"), crypt_str("m_nForceBone"));

	static auto offs_bone_mask = uOffset + 0x20;
	*reinterpret_cast<int*> (reinterpret_cast<uintptr_t> (player) + offs_bone_mask) = 0;

	player->m_fEffects() |= 8u;
	*(uintptr_t*)(player + 0xA28) &= ~0xAu;
	*(uintptr_t*)(player + 0xA30) = 0;

	*(uintptr_t*)(0x2670 + player) = 0;
	*(uintptr_t*)(player + 0x68) |= 2u;
	*(uintptr_t*)(player + 0xA68) = 0;

	*(uintptr_t*)(player + 0x2924) = -8388609;
	*(uintptr_t*)(player + 0x2690) = 0;

	bool Matrix = player->SetupBones(matrix, 256, 0x0007FF00, curtime); // 256 bro ez

	player->m_fEffects() = BackupEffects;
	*(uintptr_t*)(player + 0xA28) = BackupOcclusionCheckFlags;
	*(uintptr_t*)(player + 0xA30) = BackupLastOcclusionCheckFrameCount;

	*(uintptr_t*)(0x2670 + player) = BackupInverseKinematics;
	*(uintptr_t*)(player + 0x68) = v22;

	m_globals()->m_curtime = BackupCurtime;
	m_globals()->m_frametime = BackupFrameTime;
	m_globals()->m_framecount = BackupFrameCount;

	return Matrix;
}

void local_animations::update_local_animations(c_baseplayeranimationstate* animstate)
{
	//	if (fresh_tick())
	//	{
	auto alloc = !animstate;
	auto change = !alloc && handle != &g_ctx.local()->GetRefEHandle(); //-V807
	auto reset = !alloc && !change && g_ctx.local()->m_flSpawnTime() != spawntime; //-V550

	if (change)
		m_memalloc()->Free(animstate);

	if (reset)
	{
		util::reset_state(animstate);
		spawntime = g_ctx.local()->m_flSpawnTime();
	}

	if (alloc || change)
	{
		animstate = (c_baseplayeranimationstate*)m_memalloc()->Alloc(sizeof(c_baseplayeranimationstate));

		if (animstate)
			util::create_state(animstate, g_ctx.local());

		handle = (CBaseHandle*)&g_ctx.local()->GetRefEHandle();
		spawntime = g_ctx.local()->m_flSpawnTime();
	}

	if (local_data.animstate)
		animstate->m_fDuckAmount = local_data.animstate->m_fDuckAmount;

	if (animstate->m_iLastClientSideAnimationUpdateFramecount == m_globals()->m_framecount)
		animstate->m_iLastClientSideAnimationUpdateFramecount = m_globals()->m_framecount - 1;

	if (animstate->m_flLastClientSideAnimationUpdateTime == m_globals()->m_curtime)
		animstate->m_flLastClientSideAnimationUpdateTime = m_globals()->m_curtime - m_globals()->m_intervalpertick;

	//Store animoverlays
	memcpy(layers, g_ctx.local()->get_animlayers(), sizeof(AnimationLayer) * 13);

	animstate->m_flFeetYawRate = 0;

	g_ctx.local()->m_iEFlags() &= ~4096;
	g_ctx.local()->m_vecAbsVelocity() = g_ctx.local()->m_vecVelocity();
	animstate->m_flUpdateTimeDelta = max(0.f, m_globals()->m_curtime - animstate->m_flLastClientSideAnimationUpdateTime);

	//Update fake animations
	g_ctx.globals.updating_animation = true;
	util::update_state(animstate, local_animations::get().local_data.fake_angles);
	g_ctx.local()->update_clientside_animation();
	g_ctx.globals.updating_animation = false;

	animstate->m_vOrigin = g_ctx.local()->GetAbsOrigin();

	if (alloc || change || reset || real_server_update)
	{
		g_ctx.globals.updating_animation = true;
		abs_angles = animstate->m_flGoalFeetYaw;
		util::update_state(animstate, local_animations::get().local_data.fake_angles);
		g_ctx.local()->update_clientside_animation();
		memcpy(pose_parameter, &g_ctx.local()->m_flPoseParameter(), 24 * sizeof(float));
		g_ctx.globals.updating_animation = false;
	}

	//Fix fkin model sway
	g_ctx.local()->get_animlayers()[12].m_flWeight = 0.f;

	//Update real animations
	g_ctx.globals.updating_animation = true;
	util::update_state(animstate, local_animations::get().local_data.real_angles);
	g_ctx.local()->update_clientside_animation();
	g_ctx.globals.updating_animation = false;

	animstate->m_vLastOrigin = g_ctx.local()->m_vecOrigin();

	animstate->m_flLastClientSideAnimationUpdateTime = m_globals()->m_curtime;
	animstate->m_iLastClientSideAnimationUpdateFramecount = 0;

	c_baseplayeranimationstate backup_state;
	AnimationLayer backup_layers[13];
	auto backup_poses = g_ctx.local()->m_flPoseParameter();
	const auto oldBones = g_ctx.local()->m_BoneAccessor().m_pBones;
	std::memcpy(&backup_state, g_ctx.local()->get_animation_state(), sizeof(c_baseplayeranimationstate));
	memcpy(backup_layers, g_ctx.local()->get_animlayers(), sizeof(AnimationLayer) * g_ctx.local()->animlayer_count());
	g_ctx.local()->m_flPoseParameter() = local_animations::get().local_animation.poses;
	memcpy(g_ctx.local()->get_animlayers(), local_animations::get().local_animation.layers, sizeof(AnimationLayer) * g_ctx.local()->animlayer_count());
	memcpy(g_ctx.local()->get_animation_state(), &local_animations::get().local_animation.state, sizeof(c_baseplayeranimationstate));
	CreateMatrix(g_ctx.local(), local_animations::get().local_animation.bones, 0.0f);
	memcpy(g_ctx.local()->get_animation_state(), &backup_state, sizeof(c_baseplayeranimationstate));
	memcpy(g_ctx.local()->get_animlayers(), backup_layers, sizeof(AnimationLayer) * g_ctx.local()->animlayer_count());
	g_ctx.local()->m_flPoseParameter() = backup_poses;
	g_ctx.local()->m_BoneAccessor().m_pBones = local_animations::get().local_animation.bones;
	g_ctx.local()->AttachmentHelper();
	g_ctx.local()->m_BoneAccessor().m_pBones = oldBones;

	for (int BoneIndex = 0; BoneIndex < 256; BoneIndex++) {
		local_animations::get().local_animation.bone_origins[BoneIndex] = g_ctx.local()->GetAbsOrigin() - local_animations::get().local_animation.bones[BoneIndex].GetOrigin();
	}

	for (int i = 0; i < 256; i++)
		local_animations::get().local_animation.bones[i].SetOrigin(g_ctx.local()->GetAbsOrigin() - local_animations::get().local_animation.bone_origins[i]);

	bool old_onground = animstate->m_bOnGround;
	bool new_onladder = !animstate->m_bOnGround && g_ctx.local()->get_move_type() == MOVETYPE_LADDER;
	bool new_onground = g_ctx.local()->m_fFlags() & FL_ONGROUND;

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
				layers[5].m_nSequence = landing_activity;
				layers[5].m_flCycle = 0.0f;
				animstate->m_bInHitGroundAnimation = true;
			}
		}
	}

	if (!g_ctx.local()->get_move_type() == MOVETYPE_LADDER)
	{
		if (layers[5].m_flWeight > 0.0f)
		{
			float v175 = (animstate->time_since_in_air() - 0.2f) * -5.0f;
			v175 = std::clamp(v175, 0.0f, 1.0f);
			float newlayer5_weight = ((3.0f - (v175 + v175)) * (v175 * v175)) * layers[5].m_flWeight;
			layers[5].m_flWeight = newlayer5_weight;
		}
	}

	//g_ctx.local()->SetupBonesEx();

	if (animstate->m_pActiveWeapon != animstate->m_pLastActiveWeapon)
	{
		for (int i = 0; i < 13; i++)
		{
			AnimationLayer pLayer = g_ctx.local()->get_animlayers()[i];

			pLayer.m_pStudioHdr = NULL;
			pLayer.m_nDispatchSequence = -1;
			pLayer.m_nDispatchSequence_2 = -1;
		}
	}

	g_ctx.local()->m_nSequence() = 0;

	g_ctx.local()->get_animation_state()->m_flFeetCycle = layers[6].m_flCycle;
	g_ctx.local()->get_animation_state()->m_flFeetYawRate = layers[6].m_flWeight;

	animstate->m_flGoalFeetYaw = antiaim::get().condition(g_ctx.get_command()) ? abs_angles : local_animations::get().local_data.real_angles.y;
	g_ctx.local()->set_abs_angles(Vector(0.0f, abs_angles, 0.0f));

	memcpy(g_ctx.local()->get_animlayers(), layers, g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));
	memcpy(&g_ctx.local()->m_flPoseParameter(), pose_parameter, 24 * sizeof(float));
}