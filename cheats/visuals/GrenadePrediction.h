#include "..\..\includes.hpp"

class Vector;
class QAngle;
class CViewSetup;


class c_nadepoint {
public:
	c_nadepoint() {
		m_valid = false;
	}

	c_nadepoint(Vector  start, Vector  end, bool plane, bool valid, Vector  normal, bool detonate, bool ground) {
		m_start = start;
		m_end = end;
		m_plane = plane;
		m_valid = valid;
		m_normal = normal;
		m_detonate = detonate;
		m_ground = ground;
	}

	Vector  m_start, m_end, m_normal;
	bool m_valid, m_plane, m_detonate, m_ground;
};

struct afterPredi {
	Vector start;
	Vector end;
	bool isValid;
	bool isGround;
	bool isWall;
	bool isDetonate;
};

enum nade_throw_act {
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP
};

class CGrenadePrediction : public singleton<CGrenadePrediction>
{
	std::vector<afterPredi> afterPredi;
	std::array< c_nadepoint, 500 >	_points{ };
	bool		 _predicted = false;

	void predict(CUserCmd* user_cmd) noexcept;
	bool detonated(weapon_t* weapon, float time, trace_t& trace) noexcept;
public:
	void trace(CUserCmd* user_cmd) noexcept;
	void draw() noexcept;
};