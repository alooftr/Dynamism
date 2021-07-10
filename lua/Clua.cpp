// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Clua.h"	
#include <ShlObj_core.h>
#include <Windows.h>
#include <any>
#include "..\..\cheats\visuals\other_esp.h"
#include "..\utils\csgo.hpp"
#include "..\cheats\misc\logs.h"
#include "..\includes.hpp"
#include "../../cheats/menu.h"
#include "../../Render.h"


void lua_panic(sol::optional <std::string> message)
{
	if (!message)
		return;

	auto log = crypt_str("Script error ") + message.value_or("unknown");
	eventlogs::get().add(log, false);
}

std::string get_current_script(sol::this_state s)
{
	sol::state_view lua_state(s);
	sol::table rs = lua_state["debug"]["getinfo"](2, ("S"));
	std::string source = rs["source"];
	std::string filename = std::filesystem::path(source.substr(1)).filename().string();

	return filename;
}

int get_current_script_id(sol::this_state s)
{
	return c_lua::get().get_script_id(get_current_script(s));
}

namespace ns_client
{
	void add_callback(sol::this_state s, std::string eventname, sol::protected_function func)
	{		
		if (eventname != crypt_str("on_paint") && eventname != crypt_str("on_createmove") && eventname != "on_render")
		{
			return;
		}

		if (c_lua::get().loaded.at(get_current_script_id(s)))//new
		c_lua::get().hooks.registerHook(eventname, get_current_script_id(s), func);
	}

	void load_script(std::string name)
	{
		c_lua::get().refresh_scripts();
		c_lua::get().load_script(c_lua::get().get_script_id(name));
	}

	void unload_script(std::string name)
	{
		c_lua::get().refresh_scripts();
		c_lua::get().unload_script(c_lua::get().get_script_id(name));
	}

	void log(std::string text)
	{
		eventlogs::get().add(text, false);
	}
}

std::vector <std::pair <std::string, menu_item>>::iterator find_item(std::vector <std::pair <std::string, menu_item>>& items, const std::string& name)
{
	for (auto it = items.begin(); it != items.end(); ++it)
		if (it->first == name)
			return it;

	return items.end();
}

menu_item find_item(std::vector <std::vector <std::pair <std::string, menu_item>>>& scripts, const std::string& name)
{
	for (auto& script : scripts)
	{
		for (auto& item : script)
		{
			std::string item_name;

			auto first_point = false;
			auto second_point = false;

			for (auto& c : item.first)
			{
				if (c == '.')
				{
					if (first_point)
					{
						second_point = true;
						continue;
					}
					else
					{
						first_point = true;
						continue;
					}
				}

				if (!second_point)
					continue;

				item_name.push_back(c);
			}

			if (item_name == name)
				return item.second;
		}
	}

	return menu_item();
}

namespace ns_menu
{
	bool get_visible()
	{
		return hooks::menu_open;
	}

	void set_visible(bool visible)
	{
		hooks::menu_open = visible;
	}

	auto next_line_counter = 0;

	void next_line(sol::this_state s)
	{
		c_lua::get().items.at(get_current_script_id(s)).emplace_back(std::make_pair(crypt_str("next_line_") + std::to_string(next_line_counter), menu_item()));
		++next_line_counter;
	}

	void add_check_box(sol::this_state s, const std::string& name)
	{
		auto script = get_current_script(s);
		auto script_id = c_lua::get().get_script_id(script);

		auto& items = c_lua::get().items.at(script_id);
		auto full_name = script + '.' + name;

		if (find_item(items, full_name) != items.end())
			return;

		items.emplace_back(std::make_pair(full_name, menu_item(false)));
	}

	void add_combo_box(sol::this_state s, std::string name, std::vector <std::string> labels) //-V813
	{
		if (labels.empty())
			return;

		auto script = get_current_script(s);
		auto script_id = c_lua::get().get_script_id(script);

		auto& items = c_lua::get().items.at(script_id);
		auto full_name = script + '.' + name;

		if (find_item(items, full_name) != items.end())
			return;

		items.emplace_back(std::make_pair(full_name, menu_item(labels, 0)));
	}

	void add_slider_int(sol::this_state s, const std::string& name, int min, int max)
	{
		auto script = get_current_script(s);
		auto script_id = c_lua::get().get_script_id(script);

		auto& items = c_lua::get().items.at(script_id);
		auto full_name = script + '.' + name;

		if (find_item(items, full_name) != items.end())
			return;

		items.emplace_back(std::make_pair(full_name, menu_item(min, max, min)));
	}

	void add_slider_float(sol::this_state s, const std::string& name, float min, float max)
	{
		auto script = get_current_script(s);
		auto script_id = c_lua::get().get_script_id(script);

		auto& items = c_lua::get().items.at(script_id);
		auto full_name = script + '.' + name;

		if (find_item(items, full_name) != items.end())
			return;

		items.emplace_back(std::make_pair(full_name, menu_item(min, max, min)));
	}

	void add_color_picker(sol::this_state s, const std::string& name)
	{
		auto script = get_current_script(s);
		auto script_id = c_lua::get().get_script_id(script);

		auto& items = c_lua::get().items.at(script_id);
		auto full_name = script + '.' + name;

		if (find_item(items, full_name) != items.end())
			return;

		items.emplace_back(std::make_pair(full_name, menu_item(Color::White)));
	}

	std::unordered_map <std::string, bool> first_update;
	std::unordered_map <std::string, menu_item> stored_values;
	std::unordered_map <std::string, void*> config_items;

	bool find_config_item(std::string name, std::string type)
	{
		if (config_items.find(name) == config_items.end())
		{
			auto found = false;

			for (auto item : cfg_manager->items)
			{
				if (item->name == name)
				{
					if (item->type != type)
					{
						return false;
					}

					found = true;
					config_items[name] = item->pointer;
					break;
				}
			}

			if (!found)
			{
				return false;
			}
		}

		return true;
	}

	bool get_bool(std::string name)
	{
		if (first_update.find(name) == first_update.end())
			first_update[name] = false;

		if (!hooks::menu_open && first_update[name])
		{
			if (stored_values.find(name) != stored_values.end())
				return stored_values[name].check_box_value;
			else if (config_items.find(name) != config_items.end())
				return *(bool*)config_items[name];
			else
				return false;
		}

		auto& it = find_item(c_lua::get().items, name);

		if (it.type == NEXT_LINE)
		{
			if (find_config_item(name, crypt_str("bool")))
				return *(bool*)config_items[name];

			return false;
		}

		first_update[name] = true;
		stored_values[name] = it;

		return it.check_box_value;
	}

	int get_int(std::string name)
	{
		if (first_update.find(name) == first_update.end())
			first_update[name] = false;

		if (!hooks::menu_open && first_update[name])
		{
			if (stored_values.find(name) != stored_values.end())
				return stored_values[name].type == COMBO_BOX ? stored_values[name].combo_box_value : stored_values[name].slider_int_value;
			else if (config_items.find(name) != config_items.end())
				return *(int*)config_items[name]; //-V206
			else
				return 0;
		}

		auto& it = find_item(c_lua::get().items, name);

		if (it.type == NEXT_LINE)
		{
			if (find_config_item(name, crypt_str("int")))
				return *(int*)config_items[name]; //-V206

			return 0;
		}

		first_update[name] = true;
		stored_values[name] = it;

		return it.type == COMBO_BOX ? it.combo_box_value : it.slider_int_value;
	}

	float get_float(std::string name)
	{
		if (first_update.find(name) == first_update.end())
			first_update[name] = false;

		if (!hooks::menu_open && first_update[name])
		{
			if (stored_values.find(name) != stored_values.end())
				return stored_values[name].slider_float_value;
			else if (config_items.find(name) != config_items.end())
				return *(float*)config_items[name];
			else
				return 0.0f;
		}

		auto& it = find_item(c_lua::get().items, name);

		if (it.type == NEXT_LINE)
		{
			if (find_config_item(name, crypt_str("float")))
				return *(float*)config_items[name];

			return 0.0f;
		}

		first_update[name] = true;
		stored_values[name] = it;

		return it.slider_float_value;
	}

	Color get_color(std::string name)
	{
		if (first_update.find(name) == first_update.end())
			first_update[name] = false;

		if (!hooks::menu_open && first_update[name])
		{
			if (stored_values.find(name) != stored_values.end())
				return stored_values[name].color_picker_value;
			else if (config_items.find(name) != config_items.end())
				return *(Color*)config_items[name];
			else
				return Color::White;
		}

		auto& it = find_item(c_lua::get().items, name);

		if (it.type == NEXT_LINE)
		{
			if (find_config_item(name, crypt_str("Color")))
				return *(Color*)config_items[name];

			return Color::White;
		}

		first_update[name] = true;
		stored_values[name] = it;

		return it.color_picker_value;
	}

	bool get_key_bind_state(int key_bind)
	{
		return key_binds::get().get_key_bind_state_lua(key_bind);
	}

	int get_key_bind_mode(int key_bind)
	{
		return key_binds::get().get_key_bind_mode(key_bind);
	}
	
	void set_bool(std::string name, bool value)
	{
		if (!find_config_item(name, crypt_str("bool")))
			return;

		*(bool*)config_items[name] = value;
	}

	void set_int(std::string name, int value)
	{
		if (!find_config_item(name, crypt_str("int")))
			return;

		*(int*)config_items[name] = value; //-V206
	}

	void set_float(std::string name, float value)
	{
		if (!find_config_item(name, crypt_str("float")))
			return;

		*(float*)config_items[name] = value;
	}

	void set_color(std::string name, Color value)
	{
		if (!find_config_item(name, crypt_str("Color")))
			return;

		*(Color*)config_items[name] = value;
	}
}

namespace ns_globals
{
	int get_framerate()
	{
		return g_ctx.globals.framerate;
	}

	int get_ping()
	{
		return g_ctx.globals.ping;
	}

	std::string get_server_address()
	{
		if (!m_engine()->IsInGame())
			return "Unknown";

		auto nci = m_engine()->GetNetChannelInfo();

		if (!nci)
			return "Unknown";

		auto server = nci->GetAddress();

		if (!strcmp(server, "loopback"))
			server = "Local server";
		else if (m_gamerules()->m_bIsValveDS())
			server = "Valve server";

		return server;
	}

	std::string get_time()
	{
		return g_ctx.globals.time;
	}

	std::string get_username()
	{
		return g_ctx.username;
	}

	float get_realtime()
	{
		return m_globals()->m_realtime;
	}

	float get_curtime()
	{
		return m_globals()->m_curtime;
	}

	float get_frametime()
	{
		return m_globals()->m_frametime;
	}

	int get_tickcount()
	{
		return m_globals()->m_tickcount;
	}

	int get_framecount()
	{
		return m_globals()->m_framecount;
	}

	float get_intervalpertick()
	{
		return m_globals()->m_intervalpertick;
	}

	int get_maxclients()
	{
		return m_globals()->m_maxclients;
	}
}

namespace ns_engine
{
	static int width, height;

	Vector get_screen_size()
	{
		m_engine()->GetScreenSize(width, height);
		return Vector(width, height, 0);
	}

	player_info_t get_player_info(int i)
	{
		player_info_t player_info;
		m_engine()->GetPlayerInfo(i, &player_info);

		return player_info;
	}

	int get_player_for_user_id(int i) 
	{
		return m_engine()->GetPlayerForUserID(i);
	}

	int get_local_player_index() 
	{
		return m_engine()->GetLocalPlayer();
	}

	Vector get_view_angles() 
	{
		Vector view_angles;
		m_engine()->GetViewAngles(view_angles);

		return view_angles;
	}

	void set_view_angles(Vector view_angles)
	{
		math::normalize_angles(view_angles);
		m_engine()->SetViewAngles(view_angles);
	}

	bool is_in_game() 
	{
		return m_engine()->IsInGame();
	}

	bool is_connected() 
	{
		return m_engine()->IsConnected();
	}

	std::string get_level_name() 
	{
		return m_engine()->GetLevelName();
	}

	std::string get_level_name_short() 
	{
		return m_engine()->GetLevelNameShort();
	}

	std::string get_map_group_name() 
	{
		return m_engine()->GetMapGroupName();
	}

	bool is_playing_demo()
	{
		return m_engine()->IsPlayingDemo();
	}

	bool is_recording_demo()
	{
		return m_engine()->IsRecordingDemo();
	}

	bool is_paused() 
	{
		return m_engine()->IsPaused();
	}

	bool is_taking_screenshot() 
	{
		return m_engine()->IsTakingScreenshot();
	}

	bool is_hltv() 
	{
		return m_engine()->IsHLTV();
	}
}

namespace ns_render
{
	Vector world_to_screen(const Vector& world)
	{
		Vector screen;

		if (!math::WorldToScreen(world, screen))
			return ZERO;

		return screen;
	}

	int get_text_width(vgui::HFont font, const std::string& text)
	{
		return render::get().text_width(font, text.c_str());
	}

	void draw_text(ImFont* font, float x, float y, Color color, const std::string& text)
	{
		g_Render->DrawString((int)x, (int)y, color, render2::none, font, text.c_str());
	}

	void draw_text_centered(ImFont* font, float x, float y, Color color, bool centered_x, bool centered_y, const std::string& text)
	{
		DWORD centered_flags = HFONT_CENTERED_NONE;

		if (centered_x)
		{
			centered_flags &= ~HFONT_CENTERED_NONE; //-V753
			centered_flags |= HFONT_CENTERED_X;
		}

		if (centered_y)
		{
			centered_flags &= ~HFONT_CENTERED_NONE;
			centered_flags |= HFONT_CENTERED_Y;
		}

		g_Render->DrawString((int)x, (int)y, color, render2::centered_x | render2::centered_y, font, text.c_str());
	}

	void draw_line(float x, float y, float x2, float y2, Color color)
	{
		g_Render->DrawLine((int)x, (int)y, (int)x2, (int)y2, color);
	}

	void draw_rect(float x, float y, float w, float h, Color color)
	{
		g_Render->Rect((int)x, (int)y, (int)w, (int)h, color);
	}

	void draw_rect_filled(float x, float y, float w, float h, Color color)
	{
		g_Render->FilledRect((int)x, (int)y, (int)w, (int)h, color);
	}

	void draw_circle(float x, float y, float points, float radius, Color color)
	{
		g_Render->DrawCircle((int)x, (int)y, (int)radius, (int)points, color);
	}

	void draw_circle_filled(float x, float y, float points, float radius, Color color)
	{
		g_Render->CircleFilled((int)x, (int)y, (int)radius, color, (int)points);
	}

	void draw_triangle(float x, float y, float x2, float y2, float x3, float y3, Color color)
	{
		g_Render->TriangleFilled(x, y, x2, y2, x3, y3, color);
	}

	void draw_3DCIRCLE(float x, float y, float z, float r, float p, Color color, Color color2)
	{
		g_Render->DrawRing3D(x, y, x, r, p, color, color2, 1.f);
	}
}

namespace ns_console
{
	ConVar* FindVar(const char* CVar)
	{
		return m_cvar()->FindVar(CVar);
	}

}

namespace ns_events
{
	void register_event(sol::this_state s, std::string event_name, sol::protected_function function)
	{
		if (std::find(g_ctx.globals.events.begin(), g_ctx.globals.events.end(), event_name) == g_ctx.globals.events.end())
		{
			m_eventmanager()->AddListener(&hooks::hooked_events, event_name.c_str(), false);
			g_ctx.globals.events.emplace_back(event_name);
		}

		c_lua::get().events[get_current_script_id(s)][event_name] = function;
	}
}

namespace ns_entitylist
{
	sol::optional <player_t*> get_local_player()
	{
		if (!m_engine()->IsInGame())
			return sol::optional <player_t*> (sol::nullopt);

		return (player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer());
	}

	sol::optional <player_t*> get_player_by_index(int i)
	{
		if (!m_engine()->IsInGame())
			return sol::optional <player_t*> (sol::nullopt);

		return (player_t*)m_entitylist()->GetClientEntity(i);
	}

	player_t* GetClientEntityFromHandle(unsigned int handle)
	{
		return (player_t*)m_entitylist()->GetClientEntityFromHandle(handle);
	}

	int GetHighestEntityIndex()
	{
		return m_entitylist()->GetHighestEntityIndex();
	}
}

namespace ns_cmd
{
	bool get_send_packet()
	{
		if (!g_ctx.get_command())
			return true;

		return g_ctx.send_packet;
	}

	void set_send_packet(bool send_packet)
	{
		if (!g_ctx.get_command())
			return;

		g_ctx.send_packet = send_packet;
	}

	int get_choke()
	{
		if (!g_ctx.get_command())
			return 0;

		return m_clientstate()->iChokedCommands;
	}

	bool get_button_state(int button)
	{
		if (!g_ctx.get_command())
			return false;

		return g_ctx.get_command()->m_buttons & button;
	}

	void set_button_state(int button, bool state)
	{
		if (!g_ctx.get_command())
			return;

		if (state)
			g_ctx.get_command()->m_buttons |= button;
		else
			g_ctx.get_command()->m_buttons &= ~button;
	}
}

namespace ns_utils 
{
	uint64_t find_signature(const std::string& szModule, const std::string& szSignature)
	{
		return util::FindSignature(szModule.c_str(), szSignature.c_str());
	}
}

namespace ns_file //new
{
	void append(sol::this_state s, std::string& path, std::string& data)
	{
		if (!config_system.g_cfg.scripts.allow_file)
		{
			c_lua::get().unload_script(get_current_script_id(s));
			return;
		}

		std::ofstream out(path, std::ios::app | std::ios::binary);

		if (out.is_open())
			out << data;
	
		out.close();
	}
	void write(sol::this_state s, std::string& path, std::string& data)
	{
		if (!config_system.g_cfg.scripts.allow_file)
		{
			c_lua::get().unload_script(get_current_script_id(s));
			return;
		}

		std::ofstream out(path, std::ios::binary);

		if (out.is_open())
			out << data;

		
		out.close();
	}
	std::string read(sol::this_state s, std::string& path)
	{
		if (!config_system.g_cfg.scripts.allow_file)
		{
			c_lua::get().unload_script(get_current_script_id(s));
			return "";
		}

		std::ifstream file(path, std::ios::binary);

		if (file.is_open())
		{
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();
			return content;
		}
		else
		{
			file.close();
			return "";
		}
	}
}

sol::state lua;
void c_lua::initialize()
{
	lua = sol::state(sol::c_call<decltype(&lua_panic), &lua_panic>);
	lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::debug, sol::lib::package);

	lua.new_enum(crypt_str("key_binds"),
		crypt_str("legit_enable"), 1,
		crypt_str("double_tap"), 2,
		crypt_str("safe_points"), 3,
		crypt_str("damage_override"), 4,
		crypt_str("hide_shots"), 12,
		crypt_str("manual_back"), 13,
		crypt_str("manual_left"), 14,
		crypt_str("manual_right"), 15,
		crypt_str("flip_desync"), 16,
		crypt_str("thirdperson"), 17,
		crypt_str("automatic_peek"), 18,
		crypt_str("edge_jump"), 19,
		crypt_str("fakeduck"), 20,
		crypt_str("slowwalk"), 21,
		crypt_str("body_aim"), 22
	);

	lua.new_enum(crypt_str("key_bind_mode"),
		crypt_str("hold"), 0,
		crypt_str("toggle"), 1
	);

	lua.new_usertype<player_t>(crypt_str("Offset"), // new
		(std::string)crypt_str("OffsetInt"), &player_t::GetPropInt,
		(std::string)crypt_str("OffsetFloat"), &player_t::GetPropFloat,
		(std::string)crypt_str("OffsetBool"), &player_t::GetPropBool,
		(std::string)crypt_str("OffsetString"), &player_t::GetPropString,
		(std::string)crypt_str("setOffsetInt"), &player_t::SetPropInt,
		(std::string)crypt_str("setOffsetFloat"), &player_t::SetPropFloat,
		(std::string)crypt_str("setOffsetBool"), &player_t::SetPropBool
	);

	lua.new_usertype <Color> (crypt_str("Color"), sol::constructors <Color(), Color(int, int, int), Color(int, int, int, int)> (), 
		(std::string)crypt_str("r"), &Color::r, 
		(std::string)crypt_str("g"), &Color::g,
		(std::string)crypt_str("b"), &Color::b, 
		(std::string)crypt_str("a"), &Color::a
	);

	lua.new_usertype <Vector> (crypt_str("Vector"), sol::constructors <Vector(), Vector(float, float, float)> (),
		(std::string)crypt_str("x"), &Vector::x,
		(std::string)crypt_str("y"), &Vector::y,
		(std::string)crypt_str("z"), &Vector::z,
		(std::string)crypt_str("Length"), &Vector::Length,
		(std::string)crypt_str("Length2D"), &Vector::Length2D,
		(std::string)crypt_str("IsZero"), &Vector::IsZero,
		(std::string)crypt_str("DistTo"), &Vector::DistTo,
		(std::string)crypt_str("Normalize"), &Vector::Normalize
	);

	lua.new_usertype <player_info_t> (crypt_str("player_info"),
		crypt_str("FakePlayer"), &player_info_t::fakeplayer,
		crypt_str("szName"), &player_info_t::szName,
		crypt_str("isHLTV"), &player_info_t::ishltv,
		crypt_str("szSteamID"), &player_info_t::szSteamID

	);

	lua.new_usertype <shot_info>(crypt_str("shot_info"), sol::constructors <>(),
		(std::string)crypt_str("targetName"), &shot_info::target_name,
		(std::string)crypt_str("hitbox"), &shot_info::server_hitbox,
		(std::string)crypt_str("damage"), &shot_info::server_damage,
		(std::string)crypt_str("hitchance"), &shot_info::hitchance,
		(std::string)crypt_str("backtrack"), &shot_info::backtrack_ticks,
		(std::string)crypt_str("angle"), &shot_info::aim_point
		);

	lua.new_usertype <IGameEvent> (crypt_str("GameEvent"),
		(std::string)crypt_str("GetBool"), &IGameEvent::GetBool,
		(std::string)crypt_str("GetInt"), &IGameEvent::GetInt,
		(std::string)crypt_str("GetFloat"), &IGameEvent::GetFloat,
		(std::string)crypt_str("GetString"), &IGameEvent::GetString,
		(std::string)crypt_str("SetBool"), &IGameEvent::SetBool,
		(std::string)crypt_str("SetInt"), &IGameEvent::SetInt,
		(std::string)crypt_str("SetFloat"), &IGameEvent::SetFloat,
		(std::string)crypt_str("SetString"), &IGameEvent::SetString
	);

	lua.new_enum(crypt_str("hitboxes"),
		crypt_str("HITBOX_HEAD"), HITBOX_HEAD,
		crypt_str("HITBOX_NECK"), HITBOX_NECK,
		crypt_str("HITBOX_PELVIS"), HITBOX_PELVIS,
		crypt_str("HITBOX_STOMACH"), HITBOX_STOMACH,
		crypt_str("HITBOX_LOWER_CHEST"), HITBOX_LOWER_CHEST,
		crypt_str("HITBOX_CHEST"), HITBOX_CHEST,
		crypt_str("HITBOX_UPPER_CHEST"), HITBOX_UPPER_CHEST,
		crypt_str("HITBOX_RIGHT_THIGH"), HITBOX_RIGHT_THIGH,
		crypt_str("HITBOX_LEFT_THIGH"), HITBOX_LEFT_THIGH,
		crypt_str("HITBOX_RIGHT_CALF"), HITBOX_RIGHT_CALF,
		crypt_str("HITBOX_LEFT_CALF"), HITBOX_LEFT_CALF,
		crypt_str("HITBOX_RIGHT_FOOT"), HITBOX_RIGHT_FOOT,
		crypt_str("HITBOX_LEFT_FOOT"), HITBOX_LEFT_FOOT,
		crypt_str("HITBOX_RIGHT_HAND"), HITBOX_RIGHT_HAND,
		crypt_str("HITBOX_LEFT_HAND"), HITBOX_LEFT_HAND,
		crypt_str("HITBOX_RIGHT_UPPER_ARM"), HITBOX_RIGHT_UPPER_ARM,
		crypt_str("HITBOX_RIGHT_FOREARM"), HITBOX_RIGHT_FOREARM,
		crypt_str("HITBOX_LEFT_UPPER_ARM"), HITBOX_LEFT_UPPER_ARM,
		crypt_str("HITBOX_LEFT_FOREARM"), HITBOX_LEFT_FOREARM
	);

	lua.new_usertype <player_t> (crypt_str("Entity"), sol::base_classes, sol::bases<entity_t>(), //new
		(std::string)crypt_str("GetIndex"), &player_t::EntIndex,
		(std::string)crypt_str("GetDormant"), &player_t::IsDormant,
		(std::string)crypt_str("GetTeam"), &player_t::m_iTeamNum,
		(std::string)crypt_str("GetAlive"), &player_t::is_alive,
		(std::string)crypt_str("GetVelocity"), &player_t::m_vecVelocity,
		(std::string)crypt_str("GetOrigin"), &player_t::GetAbsOrigin,
		(std::string)crypt_str("GetAngles"), &player_t::m_angEyeAngles,
		(std::string)crypt_str("GetHitbox"), &player_t::hitbox_position,
		(std::string)crypt_str("HasHelmet"), &player_t::m_bHasHelmet,
		(std::string)crypt_str("HasArmor"), &player_t::m_bHasHeavyArmor,
		(std::string)crypt_str("GetScoped"), &player_t::m_bIsScoped,
		(std::string)crypt_str("GetHealth"), &player_t::m_iHealth
	);

	lua.new_usertype <weapon_t> (crypt_str("Weapon"), sol::base_classes, sol::bases<entity_t>(),
		(std::string)crypt_str("IsEmpty"), &weapon_t::is_empty,
		(std::string)crypt_str("CanFire"), &weapon_t::can_fire,
		(std::string)crypt_str("CanDT"), &weapon_t::can_double_tap,
		(std::string)crypt_str("GetName"), &weapon_t::get_name,
		(std::string)crypt_str("GetInaccuracy"), &weapon_t::get_inaccuracy,
		(std::string)crypt_str("GetSpread"), &weapon_t::get_spread
	);

	lua.new_enum(crypt_str("buttons"),
		crypt_str("IN_ATTACK"), IN_ATTACK,
		crypt_str("IN_JUMP"), IN_JUMP,
		crypt_str("IN_DUCK"), IN_DUCK,
		crypt_str("IN_FORWARD"), IN_FORWARD,
		crypt_str("IN_BACK"), IN_BACK,
		crypt_str("IN_USE"), IN_USE,
		crypt_str("IN_CANCEL"), IN_CANCEL,
		crypt_str("IN_LEFT"), IN_LEFT,
		crypt_str("IN_RIGHT"), IN_RIGHT,
		crypt_str("IN_MOVELEFT"), IN_MOVELEFT,
		crypt_str("IN_MOVERIGHT"), IN_MOVERIGHT,
		crypt_str("IN_ATTACK2"), IN_ATTACK2,
		crypt_str("IN_RUN"), IN_RUN,
		crypt_str("IN_RELOAD"), IN_RELOAD,
		crypt_str("in_alt1"), IN_ALT1,
		crypt_str("IN_ALT2"), IN_ALT2,
		crypt_str("IN_SCORE"), IN_SCORE,
		crypt_str("IN_SPEED"), IN_SPEED,
		crypt_str("IN_WALK"), IN_WALK,
		crypt_str("IN_ZOOM"), IN_ZOOM,
		crypt_str("IN_WEAPON1"), IN_WEAPON1,
		crypt_str("IN_WEAPON2"), IN_WEAPON2,
		crypt_str("IN_BULLRUSH"), IN_BULLRUSH,
		crypt_str("IN_GRENADE1"), IN_GRENADE1,
		crypt_str("IN_GRENADE2"), IN_GRENADE2,
		crypt_str("IN_LOOKSPIN"), IN_LOOKSPIN
	);

	auto client = lua.create_table();
	client[crypt_str("AddCallBack")] = ns_client::add_callback;

	auto menu = lua.create_table();
	menu[crypt_str("new_line")] = ns_menu::next_line;
	menu[crypt_str("Checkbox")] = ns_menu::add_check_box;
	menu[crypt_str("Combo")] = ns_menu::add_combo_box;
	menu[crypt_str("SliderInt")] = ns_menu::add_slider_int;
	menu[crypt_str("SliderFloat")] = ns_menu::add_slider_float;
	menu[crypt_str("ColorPicker")] = ns_menu::add_color_picker;
	menu[crypt_str("getBool")] = ns_menu::get_bool;
	menu[crypt_str("getInt")] = ns_menu::get_int;
	menu[crypt_str("getFloat")] = ns_menu::get_float;
	menu[crypt_str("getColor")] = ns_menu::get_color;
	menu[crypt_str("getKey")] = ns_menu::get_key_bind_state;
	menu[crypt_str("addBool")] = ns_menu::set_bool;
	menu[crypt_str("addInt")] = ns_menu::set_int;
	menu[crypt_str("addFloat")] = ns_menu::set_float;
	menu[crypt_str("addColor")] = ns_menu::set_color;

	auto globals = lua.create_table();
	globals[crypt_str("GetFramerate")] = ns_globals::get_framerate;
	globals[crypt_str("GetPing")] = ns_globals::get_ping;
	globals[crypt_str("GetRealtime")] = ns_globals::get_realtime;
	globals[crypt_str("GetCurtime")] = ns_globals::get_curtime;
	globals[crypt_str("GetFrametime")] = ns_globals::get_frametime;
	globals[crypt_str("GetTickcount")] = ns_globals::get_tickcount;
	globals[crypt_str("GetFramecount")] = ns_globals::get_framecount;
	globals[crypt_str("GetIntervalpertick")] = ns_globals::get_intervalpertick;
	globals[crypt_str("GetMaxclients")] = ns_globals::get_maxclients;

	auto engine = lua.create_table();
	engine[crypt_str("GetScreenSize")] = ns_engine::get_screen_size;
	engine[crypt_str("GetLevelName")] = ns_engine::get_level_name;
	engine[crypt_str("GetLevelNameShort")] = ns_engine::get_level_name_short;
	engine[crypt_str("GetLocalPlayer")] = ns_engine::get_local_player_index;
	engine[crypt_str("GetMapGroupName")] = ns_engine::get_map_group_name;
	engine[crypt_str("GetPlayerForUserId")] = ns_engine::get_player_for_user_id;
	engine[crypt_str("GetPlayerInfo")] = ns_engine::get_player_info;
	engine[crypt_str("GetViewAngles")] = ns_engine::get_view_angles;
	engine[crypt_str("GetConnected")] = ns_engine::is_connected;
	engine[crypt_str("GetHltv")] = ns_engine::is_hltv;
	engine[crypt_str("GetInGame")] = ns_engine::is_in_game;
	engine[crypt_str("GetPaused")] = ns_engine::is_paused;
	engine[crypt_str("GetPlayingDemo")] = ns_engine::is_playing_demo;
	engine[crypt_str("IsRecordingDemo")] = ns_engine::is_recording_demo;
	engine[crypt_str("IsTakingScreenshot")] = ns_engine::is_taking_screenshot;
	engine[crypt_str("SetViewAngles")] = ns_engine::set_view_angles;

	auto render = lua.create_table();
	render[crypt_str("WorldToSreen")] = ns_render::world_to_screen;
	render[crypt_str("drawString")] = ns_render::draw_text;
	render[crypt_str("drawLine")] = ns_render::draw_line;
	render[crypt_str("drawRect")] = ns_render::draw_rect;
	render[crypt_str("drawRectFilled")] = ns_render::draw_rect_filled;
	render[crypt_str("drawRing3D")] = ns_render::draw_3DCIRCLE;
	render[crypt_str("drawCircle")] = ns_render::draw_circle;
	render[crypt_str("drawCircleFilled")] = ns_render::draw_circle_filled;
	render[crypt_str("drawTriangle")] = ns_render::draw_triangle;

	auto console = lua.create_table();
	console[crypt_str("FindVar")] = ns_console::FindVar;

	lua.new_usertype<ConVar>("ConVar",
		"GetInt", &ConVar::GetInt,
		"GetFloat", &ConVar::GetFloat,
		"GetBool", &ConVar::GetBool,
		"GetString", &ConVar::GetString,
		"SetString", &ConVar::sSetValue,
		"SetBool", &ConVar::bSetValue,
		"SetInt", &ConVar::fSetValue,
		"SetFloat", &ConVar::iSetValue);

	lua.new_usertype<CUserCmd>("cmd",
		"viewangles", &CUserCmd::m_viewangles,
		"mousedx", &CUserCmd::m_mousedx,
		"mousedy", &CUserCmd::m_mousedy,
		"forwardmove", &CUserCmd::m_forwardmove,
		"sidemove", &CUserCmd::m_sidemove,
		"upmove", &CUserCmd::m_upmove,
		"tickcount", &CUserCmd::m_tickcount,
		"command_number", &CUserCmd::m_command_number,
		"buttons", &CUserCmd::m_buttons);

	auto events = lua.create_table(); //-V688
	events[crypt_str("event")] = ns_events::register_event;

	auto entitylist = lua.create_table();
	entitylist[crypt_str("GetClientEntity")] = ns_entitylist::get_player_by_index;
	entitylist[crypt_str("GetClientEntityFromHandle")] = ns_entitylist::GetClientEntityFromHandle;
	entitylist["GetHighestEntityIndex"] = ns_entitylist::GetHighestEntityIndex;


	auto utils = lua.create_table();//new
	utils[crypt_str("FindPattern")] = ns_utils::find_signature;//new
	utils[crypt_str("GetSendPacket")] = ns_cmd::get_send_packet;
	utils[crypt_str("EnableSendPacket")] = ns_cmd::set_send_packet;
	utils[crypt_str("GetChoke")] = ns_cmd::get_choke;
	utils[crypt_str("GetButtonState")] = ns_cmd::get_button_state;
	utils[crypt_str("SetButtonState")] = ns_cmd::set_button_state;

	lua.new_enum("fonts",
		"Verdana", c_menu::get().menu_font,
		"Verdana30", c_menu::get().speed_font);


	lua[crypt_str("DynEvents")] = client; //Feito
	lua[crypt_str("gui")] = menu; //Feito
	lua[crypt_str("Globals")] = globals; //Feito
	lua[crypt_str("Engine")] = engine;
	lua[crypt_str("g_Render")] = render; //Feito
	lua[crypt_str("cvars")] = console; //Feito
	lua[crypt_str("Events")] = events;
	lua[crypt_str("EntList")] = entitylist;//Feito
	lua[crypt_str("Utils")] = utils; //Feito

	refresh_scripts();
}

int c_lua::get_script_id(const std::string& name)
{
	for (auto i = 0; i < scripts.size(); i++)
		if (scripts.at(i) == name) //-V106
			return i;

	return -1;
}

int c_lua::get_script_id_by_path(const std::string& path)
{
	for (auto i = 0; i < pathes.size(); i++)
		if (pathes.at(i).string() == path) //-V106
			return i;

	return -1;
}

void c_lua::refresh_scripts()
{
	auto oldLoaded = loaded;
	auto oldScripts = scripts;

	loaded.clear();
	pathes.clear();
	scripts.clear();

	std::string folder;
	static TCHAR path[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, NULL, path)))
	{
		SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path);
		folder = std::string(path) + "\\Dynamism.space\\Luas\\";
		CreateDirectory(folder.c_str(), 0);

		auto i = 0;

		for (auto& entry : std::filesystem::directory_iterator(folder))
		{
			if (entry.path().extension() == crypt_str(".lua") || entry.path().extension() == crypt_str(".luac"))
			{
				auto path = entry.path();
				auto filename = path.filename().string();

				auto didPut = false;

				for (auto i = 0; i < oldScripts.size(); i++)
				{
					if (filename == oldScripts.at(i)) //-V106
					{
						loaded.emplace_back(oldLoaded.at(i)); //-V106
						didPut = true;
					}
				}

				if (!didPut)
					loaded.emplace_back(false);

				pathes.emplace_back(path);
				scripts.emplace_back(filename);

				items.emplace_back(std::vector <std::pair <std::string, menu_item>> ());
				++i;
			}
		}
	}
}

void c_lua::load_script(int id)
{
	if (id == -1)
		return;

	if (loaded.at(id)) //-V106
		return;

	auto path = get_script_path(id);

	if (path == crypt_str(""))
		return;

	auto error_load = false;
	loaded.at(id) = true;
	lua.script_file(path, 
		[&error_load](lua_State*, sol::protected_function_result result)
		{
			if (!result.valid())
			{
				sol::error error = result;
				auto log = crypt_str("Script error ") + (std::string)error.what();

				eventlogs::get().add(log, false);
				error_load = true;
				
			}

			return result;
		}
	);

	if (error_load | loaded.at(id) == false)
	{
		loaded.at(id) = false;
		return;
	}
		

	 //-V106
	g_ctx.globals.loaded_script = true;
}

void c_lua::unload_script(int id)
{
	if (id == -1)
		return;

	if (!loaded.at(id)) //-V106
		return;

	items.at(id).clear(); //-V106

	if (c_lua::get().events.find(id) != c_lua::get().events.end()) //-V807
		c_lua::get().events.at(id).clear();

	hooks.unregisterHooks(id);
	loaded.at(id) = false; //-V106
}

void c_lua::reload_all_scripts()
{
	for (auto current : scripts)
	{
		if (!loaded.at(get_script_id(current))) //-V106
			continue;

		unload_script(get_script_id(current));
		load_script(get_script_id(current));
	}
}

void c_lua::unload_all_scripts()
{
	for (auto s : scripts)
		unload_script(get_script_id(s));
}

std::string c_lua::get_script_path(const std::string& name)
{
	return get_script_path(get_script_id(name));
}

std::string c_lua::get_script_path(int id)
{
	if (id == -1)
		return crypt_str("");

	return pathes.at(id).string(); //-V106
}