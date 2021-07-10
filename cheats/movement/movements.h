#pragma once
#include "../../includes.hpp" 

struct trail_pos_info
{
	Vector position;
	float time = -1;
};

class movements
{
public:
	void BunnyHop(CUserCmd* cmd);
	void AirStrafe(CUserCmd* m_pcmd, Vector& orig);
	void MovementTrails();
	void Draw();

	void eBug();
	void jBug(CUserCmd* cmd);

	void LJump(CUserCmd* UserCmd);
	std::vector<trail_pos_info> trail_pos;

private:
	int flags_backup = 0;

};

inline movements g_Movement;

