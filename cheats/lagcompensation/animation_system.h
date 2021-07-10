#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

enum resolver_history
{
	HISTORY_UNKNOWN = -1,
	HISTORY_ORIGINAL,
	HISTORY_ZERO,
	HISTORY_DEFAULT,
	HISTORY_LOW
};

struct player_settings
{
	__int64 id;
	resolver_history res_type;
	bool faking;
	int neg;
	int pos;

	player_settings(__int64 id, resolver_history res_type, bool faking, int left, int right) noexcept : id(id), res_type(res_type), faking(faking), neg(neg), pos(pos)
	{

	}
};

enum
{
	MAIN,
	NONE,
	FIRST,
	SECOND
};

enum resolver_type
{
	ORIGINAL,
	BRUTEFORCE,
	LBY,
	LAYERS,
	TRACE,
	DIRECTIONAL,
	NON_RESOLVED
};

enum resolver_side
{
	SIDE,
	RESOLVER_ORIGINAL,
	RESOLVER_ZERO,
	RESOLVER_FIRST,
	RESOLVER_SECOND,
	RESOLVER_LOW_FIRST,
	RESOLVER_LOW_SECOND
};

struct matrixes
{
	matrix3x4_t main[MAXSTUDIOBONES];
	matrix3x4_t zero[MAXSTUDIOBONES];
	matrix3x4_t first[MAXSTUDIOBONES];
	matrix3x4_t second[MAXSTUDIOBONES];
};

class adjust_data;

class resolver
{
	player_t* player = nullptr;
	adjust_data* player_record = nullptr;

	bool side = false;
	bool fake = false;
	bool was_first_bruteforce = false;
	bool was_second_bruteforce = false;

	bool was_first_low_bruteforce = false;
	bool was_second_low_bruteforce = false;

	resolver* record;
	int* Side{};
	int FreestandSide[64];
	int DetectSide[64];
	float lock_side = 0.0f;
	float resolving_way = 0.0f;
	bool ApproachAngle = false;
	float original_pitch = 0.0f;
public:
	void initialize(player_t* e, adjust_data* record, const float& pitch);
	void reset();
	void ResolveAngles();
	float resolve_pitch();
	float goal_feet_yaw = 0.0f;
	bool is_slow_walk(player_t* entity);
	float original_goal_feet_yaw = 0.0f;
	/*static*/ int freestand_side[64];
	float resolver_goal_feet_yaw[3];
	void anti_freestanding(player_t* player);
	void SideDetectiongg(player_t* player);
	void nn(player_t* player);
	struct resolver_info {
		float resolver_data[64];
		float choked_time;
		float fake_goal_feet_yaw;

		bool use_freestand_angle[65];
		float freestand_angle[65];
		float last_freestanding_angle[65];
		bool safepoint[64];
	} info;
	//	resolver_mode side;
	resolver_type type;

	AnimationLayer resolver_layers[3][13];
	player_t* e;
	AnimationLayer previous_layers[13];
	float gfy_default = 0.0f;

	AnimationLayer resolver_anim_layer[4][13];
	AnimationLayer anim_layers[13];
	void StoreAntifreestand();
	resolver_side last_side = RESOLVER_ORIGINAL;
};

class adjust_data
{
public:
	player_t* player;
	int i;

	AnimationLayer layers[13];
	matrixes matrixes_data;

	resolver_type type;
	resolver_side side;

	bool invalid;
	bool immune;
	bool dormant;
	bool bot;
	bool shot;

	int flags;
	int bone_count;

	float simulation_time;
	float duck_amount;
	float lby;

	Vector angles;
	Vector abs_angles;
	Vector velocity;
	Vector origin;
	Vector mins;
	Vector maxs;

	adjust_data()
	{
		reset();
	}

	void reset()
	{
		player = nullptr;
		i = -1;

		type = ORIGINAL;
		side = RESOLVER_ORIGINAL;

		invalid = false;
		immune = false;
		dormant = false;
		bot = false;
		shot = false;

		flags = 0;
		bone_count = 0;

		simulation_time = 0.0f;
		duck_amount = 0.0f;
		lby = 0.0f;

		angles.Zero();
		abs_angles.Zero();
		velocity.Zero();
		origin.Zero();
		mins.Zero();
		maxs.Zero();
	}

	adjust_data(player_t* e, bool store = true)
	{
		type = ORIGINAL;
		side = RESOLVER_ORIGINAL;

		invalid = false;
		store_data(e, store);
	}

	void store_data(player_t* e, bool store = true)
	{
		if (!e->is_alive())
			return;

		player = e;
		i = player->EntIndex();

		if (store)
		{
			memcpy(layers, e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));
			memcpy(matrixes_data.main, player->m_CachedBoneData().Base(), player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));
		}

		immune = player->m_bGunGameImmunity() || player->m_fFlags() & FL_FROZEN;
		dormant = player->IsDormant();

		player_info_t player_info;
		m_engine()->GetPlayerInfo(i, &player_info);

		bot = player_info.fakeplayer;
		shot = player->m_hActiveWeapon() && (player->m_hActiveWeapon()->m_fLastShotTime() == player->m_flSimulationTime());

		flags = player->m_fFlags();
		bone_count = player->m_CachedBoneData().Count();

		simulation_time = player->m_flSimulationTime();
		duck_amount = player->m_flDuckAmount();
		lby = player->m_flLowerBodyYawTarget();

		angles = player->m_angEyeAngles();
		abs_angles = player->GetAbsAngles();
		velocity = player->m_vecVelocity();
		origin = player->m_vecOrigin();
		mins = player->GetCollideable()->OBBMins();
		maxs = player->GetCollideable()->OBBMaxs();
	}

	void adjust_player()
	{
		if (!valid(false))
			return;

		memcpy(player->get_animlayers(), layers, player->animlayer_count() * sizeof(AnimationLayer));
		memcpy(player->m_CachedBoneData().Base(), matrixes_data.main, player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));

		player->m_fFlags() = flags;
		player->m_CachedBoneData().m_Size = bone_count;

		player->m_flSimulationTime() = simulation_time;
		player->m_flDuckAmount() = duck_amount;
		player->m_flLowerBodyYawTarget() = lby;

		player->m_angEyeAngles() = angles;
		player->set_abs_angles(abs_angles);
		player->m_vecVelocity() = velocity;
		player->m_vecOrigin() = origin;
		player->set_abs_origin(origin);
		player->GetCollideable()->OBBMins() = mins;
		player->GetCollideable()->OBBMaxs() = maxs;
	}

	bool valid(bool extra_checks = true)
	{
		if (!this)
			return false;

		if (i > 0)
			player = (player_t*)m_entitylist()->GetClientEntity(i);

		if (!player)
			return false;

		if (player->m_lifeState() != LIFE_ALIVE)
			return false;

		if (immune)
			return false;

		if (dormant)
			return false;

		if (!extra_checks)
			return true;

		if (invalid)
			return false;

		auto net_channel_info = m_engine()->GetNetChannelInfo();

		if (!net_channel_info)
			return false;

		static auto sv_maxunlag = m_cvar()->FindVar(crypt_str("sv_maxunlag"));

		auto outgoing = net_channel_info->GetLatency(FLOW_OUTGOING);
		auto incoming = net_channel_info->GetLatency(FLOW_INCOMING);

		auto correct = math::clamp(outgoing + incoming + util::get_interpolation(), 0.0f, sv_maxunlag->GetFloat());

		auto curtime = g_ctx.local()->is_alive() ? TICKS_TO_TIME(g_ctx.globals.fixed_tickbase) : m_globals()->m_curtime;
		auto delta_time = correct - (curtime - simulation_time);

		if (fabs(delta_time) > 0.2f)
			return false;

		auto extra_choke = 0;

		if (g_ctx.globals.fakeducking)
			extra_choke = 14 - m_clientstate()->iChokedCommands;

		auto server_tickcount = extra_choke + m_globals()->m_tickcount + TIME_TO_TICKS(outgoing + incoming);
		auto dead_time = (int)(TICKS_TO_TIME(server_tickcount) - sv_maxunlag->GetFloat());

		if (simulation_time < (float)dead_time)
			return false;

		return true;
	}
};

class optimized_adjust_data
{
public:
	int i;

	player_t* player;

	float simulation_time;
	float speed;
	float duck_amount;
	bool shot;

	Vector angles;
	Vector origin;

	optimized_adjust_data()
	{
		reset();
	}

	void reset()
	{
		i = 0;

		player = nullptr;

		simulation_time = 0.0f;
		speed = 0.0f;
		duck_amount = 0.0f;

		shot = false;
		angles.Zero();
		origin.Zero();
	}
};

extern std::deque <adjust_data> player_records[65];

class lagcompensation : public singleton <lagcompensation>
{
public:
	void fsn(ClientFrameStage_t stage);
	bool valid(int i, player_t* e);
	void update_player_animations(player_t* e);
	void extrapolate(player_t* player, Vector& origin, Vector& velocity, int& flags, bool wasonground);
	void BuildBones(player_t* player, bool resolve);
	//	void update_animation_system(player_t* player, adjust_data* record, C_Tickrecord * previous, int resolver_side);
	void extrapolation(player_t* player, Vector& origin, Vector& velocity, int& flags, bool on_ground);
	void player_extrapolation(player_t* e, Vector& vecorigin, Vector& vecvelocity, int& fFlags, bool bOnGround, int ticks);
	void apply_interpolation_flags(player_t* e, bool flag);

	BoneArray* m_Matrix;
	BoneArray* m_res;
	resolver player_resolver[65];
	std::vector<player_settings> player_sets;
	player_info_t player_info;

	float get_interpolation();
	bool is_dormant[65];
	float previous_goal_feet_yaw[65];
};