// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "GrenadePrediction.h"
#include "../../Render.h"

void CGrenadePrediction::predict(CUserCmd* user_cmd) noexcept {
	if (!m_engine()->IsConnected() && !m_engine()->IsInGame())
		return;

	//	readability
	constexpr float restitution = 0.45f;
	constexpr float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	constexpr float velocity = 403.0f * 0.9f;

	float step, gravity, new_velocity, unk01;
	int index{}, grenade_act{ 1 };
	Vector  pos, thrown_direction, start, eye_origin;
	Vector  angles, thrown;

	//	first time setup
	static auto sv_gravity = m_cvar()->FindVar("sv_gravity");

	//	calculate step and actual gravity value
	gravity = sv_gravity->GetFloat() / 8.0f;
	step = m_globals()->m_intervalpertick;

	//	get local view and eye origin
	eye_origin = g_ctx.local()->get_shoot_position();
	angles = user_cmd->m_viewangles;

	//	copy current angles and normalise pitch
	thrown = angles;

	if (thrown.x < 0) {
		thrown.x = -10 + thrown.x * ((90 - 10) / 90.0f);
	}
	else {
		thrown.x = -10 + thrown.x * ((90 + 10) / 90.0f);
	}

	//	find out how we're throwing the grenade
	auto primary_attack = user_cmd->m_buttons & IN_ATTACK;
	auto secondary_attack = user_cmd->m_buttons & IN_ATTACK2;

	if (primary_attack && secondary_attack) {
		grenade_act = ACT_LOB;
	}
	else if (secondary_attack) {
		grenade_act = ACT_DROP;
	}

	//	apply 'magic' and modulate by velocity
	unk01 = power[grenade_act];

	unk01 = unk01 * 0.7f;
	unk01 = unk01 + 0.3f;

	new_velocity = velocity * unk01;

	//	here's where the fun begins
	math::angle_vectors(thrown, thrown_direction);

	start = eye_origin + thrown_direction * 16.0f;
	thrown_direction = (thrown_direction * new_velocity) + g_ctx.local()->m_vecVelocity();

	//	let's go ahead and predict
	for (auto time = 0.0f; index < 500; time += step) {

		pos = start + thrown_direction * step;
		trace_t trace;
		Ray_t ray;
		CTraceFilterSkipEntity filter(g_ctx.local());

		ray.Init(start, pos);
		m_trace()->TraceRay(ray, MASK_SOLID, &filter, &trace);

		//	modify path if we have hit something
		if (trace.fraction != 1.0f) {
			thrown_direction = trace.plane.normal * -2.0f * thrown_direction.Dot(trace.plane.normal) + thrown_direction;

			thrown_direction *= restitution;

			pos = start + thrown_direction * trace.fraction * step;

			time += (step * (1.0f - trace.fraction));
		}

		//	check for detonation
		auto detonate = detonated(g_ctx.globals.weapon, time, trace);

		//	emplace nade point
		_points.at(index++) = c_nadepoint(start, pos, trace.fraction != 1.0f, true, trace.plane.normal, detonate, trace.plane.normal.z > 0.7f ? true : false);

		start = pos;
		//	apply gravity modifier
		thrown_direction.z -= gravity * trace.fraction * step;

		if (detonate) {
			break;
		}
	}

	//	invalidate all empty points and finish prediction
	for (auto n = index; n < 500; ++n) {
		_points.at(n).m_valid = false;
	}

	_predicted = true;
}

int sizeGrenade;

bool CGrenadePrediction::detonated(weapon_t* weapon, float time, trace_t& trace)noexcept {

	if (!weapon) {
		return true;
	}

	//	get weapon item index
	const auto index = weapon->m_iItemDefinitionIndex();

	switch (index) {
		//	flash and HE grenades only live up to 2.5s after thrown
	case WEAPON_HEGRENADE:
		sizeGrenade = 80;
	case WEAPON_FLASHBANG:
		sizeGrenade = 30;
		if (time > 2.5f) {
			return true;
		}
		break;

		//	fire grenades detonate on ground hit, or 3.5s after thrown
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
		if (trace.fraction != 1.0f && trace.plane.normal.z > 0.7f || time > 3.5f) {
			return true;
		}
		sizeGrenade = 150;
		break;

		//	decoy and smoke grenades were buggy in prediction, so i used this ghetto work-around
	case WEAPON_DECOY:
		sizeGrenade = 10;
	case WEAPON_SMOKEGRENADE:
		sizeGrenade = 150;
		if (time > 5.0f) {
			return true;
		}
		break;
	}

	return false;
}

void CGrenadePrediction::trace(CUserCmd* user_cmd) noexcept {
	if (!m_engine()->IsConnected() && !m_engine()->IsInGame())
		return;

	if (!config_system.g_cfg.esp.grenade_prediction) {
		return;
	}

	if (!(user_cmd->m_buttons & IN_ATTACK) && !(user_cmd->m_buttons & IN_ATTACK2)) {
		_predicted = false;
		return;
	}

	const static std::vector< int > nades{
		WEAPON_FLASHBANG,
		WEAPON_SMOKEGRENADE,
		WEAPON_HEGRENADE,
		WEAPON_MOLOTOV,
		WEAPON_DECOY,
		WEAPON_INCGRENADE
	};

	//	grab local weapon

	if (!g_ctx.globals.weapon)
		return;

	if (std::find(nades.begin(), nades.end(), g_ctx.globals.weapon->m_iItemDefinitionIndex()) != nades.end()) {
		return predict(user_cmd);
	}

	_predicted = false;
}


void CGrenadePrediction::draw() noexcept
{
	if (!m_engine()->IsConnected() && !m_engine()->IsInGame())
		return;

	if (!g_ctx.local()->is_alive())
		return;

	if (!config_system.g_cfg.esp.grenade_prediction)
		return;
	static bool teste = false;

	if (!teste)
	{
		afterPredi.push_back({ Vector(0,0,0), Vector(0,0,0), false, false, false, false });
		teste = true;
	}

	Vector  start, end;
	if (_predicted) {
		for (auto& p : _points) {
			if (!p.m_valid) {
				break;
			}
			afterPredi.push_back({ p.m_start, p.m_end, p.m_valid, p.m_ground,  p.m_plane, p.m_detonate });

			if (math::WorldToScreen(p.m_start, start) && math::WorldToScreen(p.m_end, end)) {

				g_Render->DrawLine(start.x, start.y, end.x, end.y, config_system.g_cfg.esp.grenade_prediction_tracer_color);

				if (!p.m_detonate && p.m_plane && !p.m_ground)
					g_Render->DrawGradientCircle(start.x, start.y - 2, 10.f, 0, 1, 50, D3DCOLOR_RGBA(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 150), D3DCOLOR_RGBA(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 0));
				if (!p.m_detonate && p.m_ground)
					g_Render->DrawRing3D(p.m_start.x, p.m_start.y - 2, p.m_start.z, 10.f, 20.f, Color(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 0), Color(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 150), 1.f);

				if (p.m_detonate && p.m_ground)
					g_Render->DrawRing3D(p.m_end.x, p.m_end.y, p.m_end.z, 10, 30, Color(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 0), Color(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 150), 1.f);

				if (p.m_detonate && p.m_plane && !p.m_ground)
					g_Render->DrawGradientCircle(start.x, start.y - 2, 10.f, 0, 1, 50, D3DCOLOR_RGBA(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 150), D3DCOLOR_RGBA(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 0));

			}
		}
	}
	//if (!_predicted)
	//{
	//	for (int i = 1; i <= m_entitylist()->GetHighestEntityIndex(); i++)  //-V807
	//	{
	//		auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

	//		if (!e)
	//			continue;

	//		if (e->is_player())
	//			continue;

	//		if (e->IsDormant())
	//			continue;

	//		auto client_class = e->GetClientClass();

	//		if (!client_class)
	//			continue;

	//		if (client_class->m_ClassID == CBaseCSGrenadeProjectile || client_class->m_ClassID == CDecoyProjectile || client_class->m_ClassID == CMolotovProjectile || client_class->m_ClassID == CSmokeGrenadeProjectile)
	//		{
	//			if (client_class->m_ClassID == CDecoyProjectile && e->m_fFlags() & FL_ONGROUND)
	//				continue;

	//			if (client_class->m_ClassID == CSmokeGrenadeProjectile && e->m_fFlags() & FL_ONGROUND)
	//				continue;

	//			Vector start2d, end2d;
	//			for (int p = 0; p < _points.size(); p++) {

	//				if (math::WorldToScreen(_points[p].m_start, start2d) && math::WorldToScreen(_points[p].m_end, end2d)) {

	//					g_Render->DrawLine(start2d.x, start2d.y, end2d.x, end2d.y, config_system.g_cfg.esp.grenade_prediction_tracer_color);

	//					if (!_points[p].m_detonate && _points[p].m_plane && !_points[p].m_ground)
	//						g_Render->DrawGradientCircle(start2d.x, start2d.y - 2, 10.f, 0, 1, 50, D3DCOLOR_RGBA(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 150), D3DCOLOR_RGBA(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 0));

	//					if (!_points[p].m_detonate && _points[p].m_ground)
	//						g_Render->DrawRing3D(_points[p].m_start.x, _points[p].m_start.y - 2, _points[p].m_start.z, 10.f, 20.f, Color(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 0), Color(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 150), 1.f);

	//					if (_points[p].m_detonate && _points[p].m_ground)
	//						g_Render->DrawRing3D(_points[p].m_end.x, _points[p].m_end.y, _points[p].m_end.z, 10, 30, Color(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 0), Color(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 150), 1.f);

	//					if (_points[p].m_detonate && _points[p].m_plane && !_points[p].m_ground)
	//						g_Render->DrawGradientCircle(start2d.x, start2d.y - 2, 10.f, 0, 1, 50, D3DCOLOR_RGBA(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 150), D3DCOLOR_RGBA(config_system.g_cfg.esp.grenade_prediction_color.r(), config_system.g_cfg.esp.grenade_prediction_color.g(), config_system.g_cfg.esp.grenade_prediction_color.b(), 0));

	//				}
	//				if (e->Explode() > 1)
	//					delete &_points;
	//			}
	//		}
	//	}
	//}
}
