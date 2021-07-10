// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "spammers.h"

void spammers::clan_tag()
{
	auto apply = [](const char* tag) -> void
	{
		using Fn = int(__fastcall*)(const char*, const char*);
		static auto fn = reinterpret_cast<Fn>(util::FindSignature(crypt_str("engine.dll"), crypt_str("53 56 57 8B DA 8B F9 FF 15")));

		fn(tag, tag);
	};

	static auto removed = false;

	if (!config_system.g_cfg.misc.clantag_spammer && !removed)
	{
		removed = true;
		apply(crypt_str(""));
		return;
	}

	if (config_system.g_cfg.misc.clantag_spammer)
	{
		auto nci = m_engine()->GetNetChannelInfo();

		if (!nci)
			return;

		static bool Reset = false;


			Reset = false;
			static float oldTime;
			float flCurTime = g_ctx.local()->m_nTickBase() * m_globals()->m_intervalpertick;
			static float flNextTimeUpdate = 0;
			static int iLastTime;

			float latency = m_engine()->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + m_engine()->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
			if (int(m_globals()->m_curtime * 2 + latency) != iLastTime)
			{
				if (flNextTimeUpdate <= flCurTime || flNextTimeUpdate - flCurTime > 1.f)
				{
					switch (int(m_globals()->m_curtime * 2.4) % 1) {
					case 0: apply("Dynamism.space"); break;
					}
				}
				iLastTime = int(m_globals()->m_curtime * 2 + latency);
			}

		removed = false;
	}
}