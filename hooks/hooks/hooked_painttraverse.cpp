#include "..\hooks.hpp"
#include "..\..\cheats\menu.h"
#include "..\..\cheats\lagcompensation\animation_system.h"
#include "..\..\cheats\visuals\player_esp.h"
#include "..\..\cheats\visuals\other_esp.h"
#include "..\..\cheats\misc\logs.h"
#include "..\..\cheats\visuals\world_esp.h"
#include "..\..\cheats\misc\misc.h"
#include "..\..\cheats\visuals\GrenadePrediction.h"
#include "..\..\cheats\visuals\bullet_tracers.h"
#include "..\..\cheats\visuals\dormant_esp.h"
#include "../../cheats/movement/movements.h"
#include "..\..\cheats\lagcompensation\local_animations.h"

using PaintTraverse_t = void(__thiscall*)(void*, vgui::VPANEL, bool, bool);


void __fastcall hooks::hooked_painttraverse(void* ecx, void* edx, vgui::VPANEL panel, bool force_repaint, bool allow_force)
{
	static auto original_fn = panel_hook.GetOriginal <PaintTraverse_t>();
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true); //-V807

	static vgui::VPANEL panel_id = 0;
	static uint32_t HudZoomPanel = 0;

	if (!HudZoomPanel)
		if (!strcmp(crypt_str("HudZoom"), m_panel()->GetName(panel)))
			HudZoomPanel = panel;

	if (HudZoomPanel == panel && config_system.g_cfg.esp.REMOVALS_SCOPE)
		return;

	original_fn(ecx, panel, force_repaint, allow_force);

	if (!panel_id)
	{
		auto panelName = m_panel()->GetName(panel);

		if (!strcmp(panelName, crypt_str("MatSystemTopPanel")))
			panel_id = panel;
	}


	if (panel_id == panel)
	{
		if (g_ctx.available())
		{
			playeresp::get().dlight();
			worldesp::get().trasparency();
			worldesp::get().skybox_changer();
			//worldesp::get().grenade_tracer();
			if (config_system.g_cfg.misc.trail_types <= 1)
				g_Movement.MovementTrails();
			misc::get().NightmodeFix();
			bullettracers::get().draw_beams();
		}

	}
}