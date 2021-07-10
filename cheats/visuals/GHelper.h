#pragma once
#include <string>
#include "../../includes.hpp"

#define M_PI 3.14159265358979323846
#define PI_F	((float)(M_PI)) 

class GrenadeData
{
public:
	std::string info;
	Vector pos = Vector(0, 0, 0);
	Vector ang = Vector(0, 0, 0);
	float ring_height = 0.f;
};

namespace visuals
{
	void RenderInfo();
	void RenderCircle();
	void DrawRing3D();
}