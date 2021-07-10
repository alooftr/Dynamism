#pragma once

#include "../../includes.hpp"
#include "../../Render.h"
#include <vector>

#define M_PI 3.14159265358979323846
#define PI_F	((float)(M_PI)) 


class MovementRe
{
public:
	void MovementR(CUserCmd* cmd);
	bool MovementP(CUserCmd* cmd);
	void MovementD();
	bool isPlayingback = false;
private:
	int i;
	bool f2 = true;
	bool f1 = true;

	bool CrosshairFirst;



};

extern MovementRe g_MovementRecorder;

