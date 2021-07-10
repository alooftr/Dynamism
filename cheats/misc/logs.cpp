// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "logs.h"
#include "../../Render.h"
#include "../menu.h"

void eventlogs::paint_traverse()
{
	if (logs.empty())
		return;

	while (logs.size() > 10)
		logs.pop_back();

	auto last_y = 146;

	for (size_t i = 0; i < logs.size(); i++)
	{
		auto& log = logs.at(i);
		log.textsize = ImGui::CalcTextSize(log.message.c_str()).x;
		if (util::epoch_time() - log.log_time > 5000)
		{
			auto factor = log.log_time + 5000.0f - (float)util::epoch_time();
			factor *= 0.001f;

			log.maxsize -= m_globals()->m_frametime * 2000;
			log.maxsize3 -= m_globals()->m_frametime * 2000;
			log.maxsize2 -= m_globals()->m_frametime * 2000;
			if (log.maxsize2 < log.textsize / 2)
			{
				log.maxsize2 = -10;
				log.yes = true;

				if (log.maxsize3 < -log.textsize)
					log.maxsize3 = -log.textsize;

				if (log.maxsize < log.textsize)
				{
					log.maxsize = -10;
					logs.erase(logs.begin() + i);
					log.y -= factor * 1.25f;
					continue;
				}

			}

		}

		last_y -= 22;

		auto logs_size_inverted = 6 - logs.size();

		if (!log.yes)
		log.maxsize += log.maxsize > log.textsize - 40 ? m_globals()->m_frametime * 200 : m_globals()->m_frametime * 1300;
		if (log.maxsize > log.textsize - 40)
		{
			log.maxsize = log.textsize;

			log.maxsize2 += log.maxsize2 > log.textsize - 40 ? m_globals()->m_frametime * 200 : m_globals()->m_frametime * 1100;
			if (log.maxsize2 > log.textsize)
			{
				log.maxsize2 = log.textsize;
				log.maxsize3 = 0;
			}
		}

		g_Render->FilledRect(log.x, last_y + log.y - logs_size_inverted * 20, log.x + 8 + log.maxsize, 20, config_system.g_cfg.misc.log_color);
		g_Render->FilledRect(log.x, last_y + log.y - logs_size_inverted * 20, log.x + 5 + log.maxsize2, 20, Color(2, 3, 6, 255));
		g_Render->DrawString(log.x + log.maxsize3, last_y + log.y - logs_size_inverted * 20, log.color, render2::none, c_menu::get().futura_small, log.message.c_str());
	}
}

void eventlogs::events(IGameEvent* event)
{
	static auto get_hitgroup_name = [](int hitgroup) -> std::string
	{
		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			return crypt_str("head");
		case HITGROUP_CHEST:
			return crypt_str("chest");
		case HITGROUP_STOMACH:
			return crypt_str("stomach");
		case HITGROUP_LEFTARM:
			return crypt_str("left arm");
		case HITGROUP_RIGHTARM:
			return crypt_str("right arm");
		case HITGROUP_LEFTLEG:
			return crypt_str("left leg");
		case HITGROUP_RIGHTLEG:
			return crypt_str("right leg");
		default:
			return crypt_str("generic");
		}
	};

	if (config_system.g_cfg.misc.events_to_log && !strcmp(event->GetName(), crypt_str("player_hurt")))
	{
		auto userid = event->GetInt(crypt_str("userid")), attacker = event->GetInt(crypt_str("attacker"));

		if (!userid || !attacker)
			return;

		auto userid_id = m_engine()->GetPlayerForUserID(userid), attacker_id = m_engine()->GetPlayerForUserID(attacker); //-V807

		player_info_t userid_info, attacker_info;

		if (!m_engine()->GetPlayerInfo(userid_id, &userid_info))
			return;

		if (!m_engine()->GetPlayerInfo(attacker_id, &attacker_info))
			return;

		auto m_victim = static_cast<player_t*>(m_entitylist()->GetClientEntity(userid_id));

		std::stringstream ss;

		if (attacker_id == m_engine()->GetLocalPlayer() && userid_id != m_engine()->GetLocalPlayer())
		{
			ss << crypt_str("Hurt player | P: ") << userid_info.szName << crypt_str(" | H: ") << get_hitgroup_name(event->GetInt(crypt_str("hitgroup"))) << crypt_str(" | DMG:  ") << event->GetInt(crypt_str("dmg_health"));
			ss << crypt_str(" | LEFT: ") << event->GetInt(crypt_str("health")) << crypt_str(" |");

			add(ss.str());
		}
		else if (userid_id == m_engine()->GetLocalPlayer() && attacker_id != m_engine()->GetLocalPlayer())
		{
			ss << crypt_str("Harmed by player | A: ") << attacker_info.szName << crypt_str(" | DMG: ") << event->GetInt(crypt_str("dmg_health")) << crypt_str(" | H:  ") << get_hitgroup_name(event->GetInt(crypt_str("hitgroup")));
			ss << crypt_str(" | LEFT: ") << event->GetInt(crypt_str("health")) << crypt_str(" |");

			add(ss.str());
		}
	}

	if (config_system.g_cfg.misc.events_to_log && !strcmp(event->GetName(), crypt_str("item_purchase")))
	{
		auto userid = event->GetInt(crypt_str("userid"));

		if (!userid)
			return;

		auto userid_id = m_engine()->GetPlayerForUserID(userid);

		player_info_t userid_info;

		if (!m_engine()->GetPlayerInfo(userid_id, &userid_info))
			return;

		auto m_player = static_cast<player_t*>(m_entitylist()->GetClientEntity(userid_id));

		if (!g_ctx.local() || !m_player)
			return;

		if (g_ctx.local() == m_player)
			g_ctx.globals.should_buy = 0;

		if (m_player->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			return;

		std::string weapon = event->GetString(crypt_str("weapon"));

		std::stringstream ss;
		ss << userid_info.szName << crypt_str(" bought ") << weapon;

		add(ss.str());
	}

	if (config_system.g_cfg.misc.events_to_log && !strcmp(event->GetName(), crypt_str("bomb_beginplant")))
	{
		auto userid = event->GetInt(crypt_str("userid"));

		if (!userid)
			return;

		auto userid_id = m_engine()->GetPlayerForUserID(userid);

		player_info_t userid_info;

		if (!m_engine()->GetPlayerInfo(userid_id, &userid_info))
			return;

		auto m_player = static_cast<player_t*>(m_entitylist()->GetClientEntity(userid_id));

		if (!m_player)
			return;

		std::stringstream ss;
		ss << userid_info.szName << crypt_str("'s planting the bomb");

		add(ss.str());
	}

	if (config_system.g_cfg.misc.events_to_log && !strcmp(event->GetName(), crypt_str("bomb_begindefuse")))
	{
		auto userid = event->GetInt(crypt_str("userid"));

		if (!userid)
			return;

		auto userid_id = m_engine()->GetPlayerForUserID(userid);

		player_info_t userid_info;

		if (!m_engine()->GetPlayerInfo(userid_id, &userid_info))
			return;

		auto m_player = static_cast<player_t*>(m_entitylist()->GetClientEntity(userid_id));

		if (!m_player)
			return;

		std::stringstream ss;
		ss << userid_info.szName << crypt_str("'s defusing the bomb");

		add(ss.str());
	}
}

void eventlogs::add(std::string text, bool full_display)
{
	logs.emplace_front(loginfo_t(util::epoch_time(), text, Color(255, 255, 255)));

	if (!full_display)
		return;

	if (config_system.g_cfg.misc.events_to_log)
	{
		last_log = true;

		m_cvar()->ConsoleColorPrintf(Color::Blue, crypt_str("[ Nodahook ] ")); //-V807

		m_cvar()->ConsoleColorPrintf(Color::White, text.c_str());
		m_cvar()->ConsolePrintf(crypt_str("\n"));
	}

}