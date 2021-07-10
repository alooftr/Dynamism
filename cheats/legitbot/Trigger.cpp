#include "Trigger.h"

#define M_PI 3.14159265358979323846
#define PI_F	((float)(M_PI)) 

#define _USE_MATH_DEFINES
#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10


CTriggerBot TriggerBot;

bool CTriggerBot::IsLineGoesThroughSmoke(Vector vStartPos, Vector vEndPos)
{
	static auto LineGoesThroughSmokeFn = (bool(*)(Vector vStartPos, Vector vEndPos))util::FindSignature("client.dll", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");
	return LineGoesThroughSmokeFn(vStartPos, vEndPos);
}


bool HitChance(Vector angles, player_t* ent, float chance)
{
	auto RandomFloat = [](float a, float b) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	};
	auto weapon = g_ctx.globals.weapon;

	if (!weapon)
		return false;

	Vector forward, right, up;
	Vector src = g_ctx.local()->get_shoot_position();
	math::angle_vectors(angles, &forward, &right, &up);

	int cHits = 0;
	int cNeededHits = static_cast<int> (150.f * (chance / 100.f));

	weapon->update_accuracy_penality();
	float weap_spread = weapon->get_spread();
	float weap_inaccuracy = weapon->get_inaccuracy();

	for (int i = 0; i < 150; i++)
	{
		float a = RandomFloat(0.f, 1.f);
		float b = RandomFloat(0.f, 2.f * PI_F);
		float c = RandomFloat(0.f, 1.f);
		float d = RandomFloat(0.f, 2.f * PI_F);

		float inaccuracy = a * weap_inaccuracy;
		float spread = c * weap_spread;

		if (weapon->m_iItemDefinitionIndex() == 64)
		{
			a = 1.f - a * a;
			a = 1.f - c * c;
		}

		Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

		direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
		direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
		direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
		direction.Normalized();

		Vector viewAnglesSpread;
		math::vector_angles(direction, viewAnglesSpread);
		viewAnglesSpread.Normalize();

		Vector viewForward;
		math::angle_vectors(viewAnglesSpread, viewForward);
		viewForward.NormalizeInPlace();

		viewForward = src + (viewForward * m_weaponsystem()->GetWeaponData(weapon->m_iItemDefinitionIndex())->flRange);

		trace_t tr;
		Ray_t ray;

		ray.Init(src, viewForward);
		m_trace()->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, ent, &tr);

		if (tr.hit_entity == ent)
			++cHits;

		if (static_cast<int> ((static_cast<float> (cHits) / 150.f) * 100.f) >= chance)
			return true;

		if ((150 - i + cHits) < cNeededHits)
			return false;
	}

	return false;
}


void CTriggerBot::TriggerBot(CUserCmd* pCmd)
{
		if (!m_engine()->IsConnected() && !m_engine()->IsInGame())
			return;
	
		if (!config_system.g_cfg.triggerbot.trigger_enable)
			return;
	
		if (key_binds::get().get_key_bind_state(29))
		{
	
			if (!g_ctx.local() && !g_ctx.local()->is_alive())
				return;
	
			bool OnGround = (g_ctx.local()->m_fFlags() & FL_ONGROUND);
			if (config_system.g_cfg.triggerbot.triggercheck3 && !OnGround)
				return;
	
			if (config_system.g_cfg.triggerbot.triggercheck2 && g_ctx.local()->m_flFlashDuration() >= 0)
				return;
	
			if (!g_ctx.globals.weapon || is_bomb(g_ctx.globals.weapon) || is_knife(g_ctx.globals.weapon) || is_grenade(g_ctx.globals.weapon))
				return;
	
			auto weapon_data = m_weaponsystem()->GetWeaponData(g_ctx.globals.weapon->m_iItemDefinitionIndex());
			if (!weapon_data)
				return;
	
			Vector src, dst, forward, crosshair_forward;
			trace_t tr;
			Ray_t ray;
			CTraceFilter filter;
	
			math::angle_vectors(pCmd->m_viewangles, forward);
	
			forward *= weapon_data->flRange;
			filter.pSkip = g_ctx.local();
			src = g_ctx.local()->get_shoot_position();
			dst = src + forward;
			ray.Init(src, dst);
	
			m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);
			player_t* entity = (player_t*)tr.hit_entity;
			if (IsLineGoesThroughSmoke(g_ctx.local()->get_shoot_position(), entity->GetRenderOrigin()) && config_system.g_cfg.triggerbot.triggercheck)
				return;
	
			if (config_system.g_cfg.triggerbot.trigger_recoil)
				pCmd->m_viewangles += g_ctx.local()->m_aimPunchAngle() * 2.5f;
	
			math::angle_vectors(pCmd->m_viewangles, crosshair_forward);
			crosshair_forward *= 8000.f;
		
			if (tr.DidHit())
				return;
	
			if (g_ctx.local()->m_iTeamNum() == entity->m_iTeamNum())
				return;
	
			int hitgroup = tr.hitgroup;
			bool didhit = false;
	
			if (config_system.g_cfg.triggerbot.trigger_hitbox_head)
			{
				if (hitgroup == HITGROUP_HEAD) didhit = true;
			}
			if (config_system.g_cfg.triggerbot.trigger_hitbox_body)
			{
				if (hitgroup == HITGROUP_CHEST || hitgroup == HITGROUP_STOMACH) didhit = true;
			}
			if (config_system.g_cfg.triggerbot.trigger_hitbox_arms)
			{
				if (hitgroup == HITGROUP_LEFTARM || hitgroup == HITGROUP_RIGHTARM) didhit = true;
			}
			if (config_system.g_cfg.triggerbot.trigger_hitbox_legs)
			{
				if (hitgroup == HITGROUP_LEFTLEG || hitgroup == HITGROUP_RIGHTLEG) didhit = true;
			}
	
			if (trigger_delay >= 2 + config_system.g_cfg.triggerbot.trigger_delay && didhit && HitChance(pCmd->m_viewangles, (player_t*)tr.hit_entity, config_system.g_cfg.triggerbot.trigger_hitchance))
			{
				trigger_delay = 0;
				pCmd->m_buttons |= IN_ATTACK;
			}
			trigger_delay++;
		}
}
