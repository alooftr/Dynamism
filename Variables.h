#pragma once
#include "sdk\interfaces\IInputSystem.hpp"

#include "nSkinz\SkinChanger.h"
#include "nSkinz\item_definitions.hpp"
#include <limits>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <vector>
#include <filesystem>
#define MAX_WEAPONS 530 // Used in parser/weapon config/etc
#include "Kit_parser.h"

struct Pos
{
	Vector			viewAngles[200];
	float			forwardmove[200];
	float			sidemove[200];
	float			upmove[200];
	int				buttons[200];
	std::string		name[200];
	std::string		map[200];
};



struct MovementRecorder
{
	int movement_int = 1;


	struct Position
	{
		Vector			position[200];
	};

	std::vector<Pos> CmdFinal;
	std::vector<Position> CmdFinal2;

	Position tempCmd2;
	Pos tempCmd;
	Vector startVec[200];


};

inline MovementRecorder GetMrecorder;


struct sticker_setting
{

	void update()
	{
		const std::vector <SkinChanger::PaintKit>* kit_names;
		kit = (game_data::sticker_kits)[kit_vector_index].id;
	}
	IDirect3DTexture9* CircleButton1 = nullptr;

	int kit = 0;
	int kit_vector_index = 0;
	float wear = 0.0f;
	float scale = 1.f;
	float rotation = 0.f;
};

enum
{
	FLAGS_MONEY,
	FLAGS_ARMOR,
	FLAGS_KIT,
	FLAGS_SCOPED,
	FLAGS_C4
};

enum
{
	BUY_Vest,
	BUY_Helmet,
	BUY_Molotov,
	BUY_Smoke,
	BUY_He,
	BUY_Taser,
	BUY_Defuser
};

enum
{
	WEAPON_ICON,
	WEAPON_TEXT,
	WEAPON_BOX,
	WEAPON_AMMO
};

enum
{
	GRENADE_BOX,
	GRENADE_ICON,
	GRENADE_TEXT,
	GRENADE_TRAIL
};

enum
{
	PLAYER_CHAMS_VISIBLE,
	PLAYER_CHAMS_INVISIBLE
};

enum
{
	ENEMY,
	TEAM,
	LOCAL
};

enum TESTE
{
	REMOVALS_SCOPE,
	REMOVALS_ZOOM,
	REMOVALS_SMOKE,
	REMOVALS_FLASH,
	REMOVALS_RECOIL,
	REMOVALS_LANDING_BOB,
	REMOVALS_POSTPROCESSING,
	REMOVALS_FOGS
};

enum
{
	INDICATOR_FAKE,
	INDICATOR_DESYNC_SIDE,
	INDICATOR_CHOKE,
	INDICATOR_DAMAGE,
	INDICATOR_SAFE_POINTS,
	INDICATOR_BODY_AIM,
	INDICATOR_DT,
	INDICATOR_HS
};

enum
{
	BAIM_AIR,
	BAIM_HIGH_VELOCITY,
	BAIM_LETHAL,
	BAIM_DOUBLE_TAP,
	BAIM_UNRESOLVED,
	BAIM_PREFER
};

enum
{
	AUTOSTOP_BETWEEN_SHOTS,
	AUTOSTOP_LETHAL,
	AUTOSTOP_VISIBLE,
	AUTOSTOP_CENTER,
	AUTOSTOP_FORCE_ACCURACY,
	AUTOSTOP_PREDICTIVE
};

enum
{
	EVENTLOG_HIT,
	EVENTLOG_ITEM_PURCHASES,
	EVENTLOG_BOMB
};

enum
{
	EVENTLOG_OUTPUT_CONSOLE,
	EVENTLOG_OUTPUT_CHAT
};

enum
{
	FAKELAG_SLOW_WALK,
	FAKELAG_MOVE,
	FAKELAG_AIR,
	FAKELAG_PEEK
};

enum
{
	ANTIAIM_STAND,
	ANTIAIM_SLOW_WALK,
	ANTIAIM_MOVE,
	ANTIAIM_AIR,
	ANTIAIM_LEGIT
};

struct mirage
{
	std::string info;
	Vector pos;
	Vector ang;
	float ring_height = 0.f;
};

struct inferno
{
	std::string info;
	Vector pos;
	Vector ang;
	float ring_height = 0.f;
};

struct overpass
{
	std::string info;
	Vector pos;
	Vector ang;
	float ring_height = 0.f;
};

struct nuke
{
	std::string info;
	Vector pos;
	Vector ang;
	float ring_height = 0.f;
};

struct cbble
{
	std::string info;
	Vector pos;
	Vector ang;
	float ring_height = 0.f;
};

struct cache
{
	std::string info;
	Vector pos;
	Vector ang;
	float ring_height = 0.f;
};

struct dust2
{
	std::string info;
	Vector pos;
	Vector ang;
	float ring_height = 0.f;
};

struct train
{
	std::string info;
	Vector pos;
	Vector ang;
	float ring_height = 0.f;
};




struct item_setting
{
	void update()
	{
		itemId = game_data::weapon_names[itemIdIndex].definition_index;
		quality = game_data::quality_names[entity_quality_vector_index].index;

		const std::vector <SkinChanger::PaintKit>* kit_names;
		const game_data::weapon_name* defindex_names;

		if (itemId == 100)
		{
			kit_names = &SkinChanger::gloveKits;
			defindex_names = game_data::glove_names;
		}
		else
		{
			kit_names = &SkinChanger::skinKits;
			defindex_names = game_data::knife_names;
		}

		paintKit = (*kit_names)[paint_kit_vector_index].id;
		definition_override_index = definition_override_vector_index;
		skin_name = (*kit_names)[paint_kit_vector_index].skin_name;


		for (auto& sticker : stickers)
			sticker.update();
	}
	int itemIdIndex = 0;
	int itemId = 1;
	int entity_quality_vector_index = 0;
	int quality = 0;
	int paint_kit_vector_index = 0;
	int paintKit = 0;
	int definition_override_vector_index = 0;
	int definition_override_index = 0;
	int seed = 0;
	int stat_trak = 0;
	float wear = 0.0f;
	char custom_name[24] = "\0";
	std::string skin_name;
	std::array<sticker_setting, 5> stickers;
};

item_setting* get_by_definition_index(const int definition_index);


struct Player_list_data
{
	int i = -1;
	std::string name;

	Player_list_data()
	{
		i = -1;
		name.clear();
	}

	Player_list_data(int i, std::string name) //-V818
	{
		this->i = i;
		this->name = name; //-V820
	}
};


extern std::unordered_map <std::string, float[4]> colors;

class c_config
{
public:
	void run(const char*) noexcept;
	void refresh() noexcept;
	void load(size_t) noexcept;
	void save(size_t) const noexcept;
	void add(const char*) noexcept;
	void remove(size_t) noexcept;
	void rename(size_t, const char*) noexcept;
	void reset() noexcept;


	constexpr auto& get_configs() noexcept {
		return configs;
	}

	struct
	{
		struct
		{
			bool enabled;
			bool friendly_fire;
			bool autopistol;
			bool draw_fov;
			bool autoscope;
			bool unscope;
			bool sniper_in_zoom_only;

			bool do_if_local_flashed;
			bool do_if_local_in_air;
			bool do_if_enemy_in_smoke;

		
			int autofire_delay;
			key_bind autofire_key;
			key_bind key;

			struct
			{
				int priority;
				//int hitbox = 0 ;
				bool auto_stop;
				bool silent;
				int fov_type;
				float fov;

				int smooth_type;
				float smooth;

				float silent_fov;

				int rcs_type;
				float rcs;
				float custom_rcs_smooth;
				float custom_rcs_fov;

				int awall_dmg;

				int target_switch_delay;
				int autofire_hitchance;
			} weapon[8];
		} legitbot;

		struct {
			bool trigger_enable{ false };
			bool trigger_hitbox_head{ false };
			bool trigger_hitbox_body{ false };
			bool trigger_hitbox_arms{ false };
			bool trigger_hitbox_legs{ false };
			int trigger_delay{ 0 };
			key_bind trigger_key;
			float trigger_hitchance{ 0.f };
			bool triggercheck{ false };
			bool triggercheck2{ false };
			bool triggercheck3{ false };
			bool trigger_recoil{ false };
		}triggerbot;

		struct
		{
			bool enable;

			bool silent_aim;
			int field_of_view;
			bool autowall;
			bool zeus_bot;
			bool knife_bot;
			bool autoshoot;
			bool double_tap;
			bool slow_teleport;
			key_bind double_tap_key;

			key_bind safe_point_key;
			key_bind body_aim_key;
			bool pitch_antiaim_correction;
			bool resolver;
			key_bind resolver_override;

			struct weapon
			{
				bool autowall;
				bool noscope_ht;
				int noscope_hitchance;
				bool double_tap_hitchance;
				int double_tap_hitchance_amount;
				bool awall_minimum_damage;
				int awall_minimum_damage_type;
				bool auto_override;
				bool awall_auto_override;
				bool hitchance;
				int hitchance_amount;
				int minimum_visible_damage;
				int minimum_damage;
				int minimum_damage_type;
				int hp_to_override;
				int awall_hp_to_override;
				int auto_override_damage;
				int awall_auto_override_damage;
				int awall_override_to;
				int override_to;
				int doubletap_mode;
				key_bind damage_override_key;
				int minimum_override_damage;
				bool UPERR_CHEST;
				bool CHEST;
				bool HEAD;
				bool NECK;
				bool STOMACH;
				bool PELVIS;
				bool LOWER_CHEST;
				bool RIGHT_UPPER_ARM;
				bool RIGHT_THIGH;
				bool FOOT;
				bool static_point_scale;
				float head_scale;
				float body_scale;
				bool max_misses;
				int max_misses_amount;
				bool prefer_safe_points;
				bool prefer_body_aim;
				bool autostop;
				bool AUTOSTOP_BETWEEN_SHOTS;
				bool AUTOSTOP_LETHAL;
				bool AUTOSTOP_VISIBLE;
				int selection_type;
				bool autoscope;

				struct {
					bool enable;
					bool if_under;
					int under_hp;
					bool air;
					bool lethal;
				}body_if;
				struct {
					bool enable;
					bool running;
					bool air;
				}head_if;

			} weapon[8];
		} ragebot;

		struct
		{
			bool enable;
			int antiaim_type;
			bool hide_shots;
			key_bind hide_shots_key;
			int desync;
			int legit_lby_type;
			int lby_type;
			bool manual;
			key_bind manual_back;
			key_bind manual_left;
			key_bind manual_right;
			key_bind flip_desync;
			bool flip_indicator;
			Color flip_indicator_color;
			bool fakelag;

			int fakelag_type;
			int fakelag_amount;
			int triggers_fakelag_amount;
			float min_amount;
			float max_amount;

			bool auto_inverter;
			int inverter_type;
			bool walk;
			bool stand;
			bool air;


		//	struct type
		//	{
				int pitch;
				int base_angle;
				bool at_target;
				int yaw;
				int range;
				int speed;
				int desynch;
				int desync_range;
				int inverted_desync_range;
				int body_lean;
				int inverted_body_lean;
			//} type[4];
		} antiaim;

		struct
		{
			bool enable;
			bool arrows;
			Color arrows_color;
			int distance;
			int size;
			bool show_multi_points;
			Color show_multi_points_color;
			bool lag_hitbox;
			Color lag_hitbox_color;
			int player_model_t;
			int player_model_ct;
			int local_chams_type;
			bool fake_chams_enable;
			bool visualize_lag;
			bool layered;
			Color fake_chams_color;
			int fake_chams_type;
			bool fake_double_material;
			int fake_double_material_m;
			Color fake_double_material_color;
			bool fake_animated_material;
			Color fake_animated_material_color;
			bool backtrack_chams;
			int backtrack_chams_material;
			Color backtrack_chams_color;
			bool transparency_in_scope;
			float transparency_in_scope_amount;
			int lagcomp_chams_material;
			Color lagcomp_chams_color;
			struct
			{
				bool FLAGS_MONEY;
				bool FLAGS_ARMOR;
				bool FLAGS_KIT;
				bool FLAGS_SCOPED;
				bool FLAGS_C4;
				Color Scoped;
				Color BombCarrie;
				Color Armor;
				Color Defuse;
				bool box;
				int box_type;
				float rouding;
				bool filledbox;
				Color box_color;
				bool name;
				Color name_color;
				bool health;
				bool custom_health_color;
				Color health_color;
				bool weapon_icon;
				bool weapon_text;
				Color weapon_color;
				bool skeleton;
				Color skeleton_color;
				bool ammo;
				Color ammobar_color;
				bool snap_lines;
				Color snap_lines_color;
				bool footsteps;
				Color footsteps_color;
				int thickness = 6;
				int radius = 350;
				Color filledboxcolor;
				bool dLight;
				float dLightR;
				float dLightDecay;
				int dLightStyle;
				int dLightExpoent;
				Color dLight_color;

				bool glow;
				Color glow_color;
				int glow_type;

				bool enable_chams;
				bool xqz_enable;
				Color chams_color;
				Color xqz_color;
				int chams_type;
				int chams_xqz;
				bool double_material;
				int double_material_m;
				Color double_material_color;

				bool double_material_xqz;
				int double_material_m_xqz;
				Color double_material_color_xqz;

				bool animated_material;
				Color animated_material_color;
				bool ragdoll_chams;
				int ragdoll_chams_material;
				Color ragdoll_chams_color;
			} type[3];
		} player;

		struct //-V730
		{
			bool refreshing = false;
			std::vector <Player_list_data> players;

			bool white_list[65];
			bool high_priority[65];
			bool force_safe_points[65];
			bool force_body_aim[65];
			bool low_delta[65];
		} player_list;

		struct
		{
			bool render_local;
			bool render_enemy;
			bool render_team;
			float size;
			bool render_health;

		} radar;

		struct
		{
			bool enable;
			std::vector <mirage> MirageGrenade;
			std::vector <inferno> InfernoGrenade;
			std::vector <train> trainGrenade;
			std::vector <overpass> overpassGrenade;
			std::vector <dust2> dust2Grenade;
			std::vector <cache> cacheGrenade;
			std::vector <cbble> cbbleGrenade;
			std::vector <nuke> nukeGrenade;
		} grenadehelper;

		struct
		{
			bool fakelag;
			bool fakelag_onpeek;
			int fakelag_type;
			int fakelag_amount;
			int switch_amount;
			float min_amount;
			float max_amount;
		}move_type[4];


		struct
		{
			bool REMOVALS_SCOPE;
			bool REMOVALS_ZOOM;
			bool REMOVALS_SMOKE;
			bool REMOVALS_FLASH;
			bool REMOVALS_RECOIL;
			bool REMOVALS_LANDING_BOB;
			bool REMOVALS_POSTPROCESSING;
			bool REMOVALS_FOGS;
			bool fix_zoom_sensivity;
			bool dynamic_scope_lines;
			bool grenade_prediction;
			bool on_click;
			Color grenade_prediction_color;
			Color grenade_prediction_tracer_color;
			bool projectiles;
			Color projectiles_color;
			bool molotov_timer;
			Color molotov_timer_color;
			bool smoke_timer;
			Color smoke_timer_color;
			bool bomb_timer;
			bool bright;
			bool nightmode;
			int skybox;
			Color skybox_color;
			Color worlds_color;
			Color pops_color;

			/*Color night_color;
			Color props_color;*/
			std::string custom_skybox;
			bool client_bullet_impacts;
			Color client_bullet_impacts_color;
			bool server_bullet_impacts;
			Color server_bullet_impacts_color;
			bool bullet_tracer;
			Color bullet_tracer_color;
			bool enemy_bullet_tracer;
			Color enemy_bullet_tracer_color;
			bool preserve_killfeed;
			bool hitmarker = false;
			bool hit;
			int hitsound;
			std::string hitfile;
			bool killsound;
			bool damage_marker;
			bool kill_effect;
			float kill_effect_duration;
			int fov;
			bool viewmodel;
			int viewmodel_fov;
			int viewmodel_x;
			int viewmodel_y;
			int viewmodel_z;
			int viewmodel_roll;
			bool arms_chams;
			int arms_chams_type;
			Color arms_chams_color;
			bool arms_double_material;
			int arms_double_material_m;
			Color arms_double_material_color;
			bool arms_animated_material;
			Color arms_animated_material_color;
			bool weapon_chams;
			int weapon_chams_type;
			Color weapon_chams_color;
			bool weapon_double_material;
			int weapon_double_material_m;
			Color weapon_double_material_color;
			bool weapon_animated_material;
			Color weapon_animated_material_color;
			bool taser_range;
			bool show_spread;
			Color show_spread_color;
			bool penetration_reticle;
			bool world_modulation;
			int world_transparency = 100;
			int props_transparency = 100;

			bool asus;
			bool skyboxch;

			float bloom;
			float exposure;
			float ambient;
			bool fog;
			int fog_distance;
			int fog_density;
			bool grenade_helper;
			Color fog_color;
			bool WEAPON_ICON;
			bool WEAPON_TEXT;
			bool WEAPON_BOX;
			bool WEAPON_AMMO;
			Color box_color;
			Color weapon_color;
			Color weapon_glow_color;
			Color weapon_ammo_color;
			bool GRENADE_BOX;
			bool GRENADE_ICON;
			bool GRENADE_TEXT;
			bool GRENADE_TRAIL;
			Color grenade_trail_color;
			Color grenade_glow_color;
			Color grenade_box_color;
		} esp;

		struct
		{
			key_bind thirdperson_toggle;
			bool thirdperson_when_spectating;
			int thirdperson_distance;
			bool spectators_list;
			bool spectatos;
			bool ingame_radar;
			bool ragdolls;
			bool bunnyhop;
			int airstrafe;
			bool crouch_in_air;
			key_bind automatic_peek;

			bool noduck;
			key_bind fakeduck_key;
			bool fast_stop;
			bool slidewalk;
			int slowwalk_speed;
			bool slowwalk;
			key_bind slowwalk_key;
			bool chat;

			bool events_to_log;
			bool show_default_log;
			Color log_color;
			bool inventory_access;
			bool rank_reveal;
			bool clantag_spammer;
			bool buybot_enable;
			int buybot1;
			int buybot2;
			bool vest;
			bool helm;
			bool he;
			bool inc;
			bool smoke;
			bool zeus;
			bool defuse;
			int aspect_ratio;

			bool anti_screenshot;
			bool anti_untrusted;
			bool keybinds;
			//------ Movement ------

			bool edge;
			key_bind edge_jump_key;
			bool duckinair;

			bool edgebug;
			key_bind edgebug_key;

			bool jumpbug;
			key_bind jumpbug_key;

			bool longjump;
			key_bind longjump_key;

			bool speedindicatior;
			bool velocitygraph;
			bool wasd;
			bool lightingonshot;
			int trail_types;
			bool movement_trails;
			bool rainbow_trails;
			Color trail_color;

			bool enable_movement;
			key_bind recorder;
			key_bind playing;
			float smooth = 10;
			int showcircle;
			bool showpath;
			bool show3dcircle;
			Color pathcolor;
			Color circle3d;
			bool showfirstpath;
			Color showfirstpath_color;
			bool indicators;
		} misc;

		struct
		{
			bool rare_animations;
			std::array <item_setting, 100> skinChanger;

			std::string custom_name_tag[100];
		} skins;

		struct
		{
			int menu_theme;
			bool watermark = true;
			bool transparence = false;
		} menu;

		struct
		{
			bool developer_mode;
			bool allow_http;
			bool allow_file;
			std::vector <std::string> scripts;
		} scripts;

		int selected_config;

		std::string new_config_name;
		std::string new_script_name;

	} g_cfg;

	std::vector<std::string> configs;
private:
	std::filesystem::path path;




public:
	void run2(const char*) noexcept;
	void load2(size_t) noexcept;
	void save2(size_t) const noexcept;
	void add2(const char*) noexcept;
	void remove2(size_t) noexcept;
	void rename2(size_t, const char*) noexcept;
	void reset2() noexcept;


	constexpr auto& get_configs2() noexcept {
		return configs2;
	}

private:
	std::filesystem::path path2;
	std::vector<std::string> configs2;
};

inline c_config config_system;