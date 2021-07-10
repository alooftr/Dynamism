#include "..\..\includes.hpp"

class misc : public singleton <misc> 
{
public:
	void AutoCrouch(CUserCmd* cmd);
	void SlideWalk(CUserCmd* cmd);
	void automatic_peek(CUserCmd* cmd, float wish_yaw);
	void ViewModel();
	void PovArrows(player_t* e);
	void NightmodeFix();

	void desync_arrows();
	void aimbot_hitboxes();

	void rank_reveal();
	void KillEffect(IGameEvent* pEvent);
	void spectators_list();	
	bool double_tap(CUserCmd* m_pcmd);
	void hide_shots(CUserCmd* m_pcmd, bool should_work);

	bool recharging_double_tap = false;

	bool double_tap_enabled = false;
	bool double_tap_key = false;

	bool hide_shots_enabled = false;
	bool hide_shots_key = false;
};

class CTeslaInfo
{
public:
	Vector m_vPos;
	Vector m_vAngles;
	int m_nEntIndex;
	const char* m_pszSpriteName;
	float m_flBeamWidth;
	int m_nBeams;
	Vector m_vColor;
	float m_flTimeVisible;
	float m_flRadius;
	float m_flRed;
	float m_flGreen;
	float m_flBlue;
	float m_flBrightness;
};