#pragma once

#include "../EffectorPP.h"
#include "../Effector.h"
#include "../CameraManager.h"

class CShootingHitEffectorPP : public CEffectorPP {
	typedef CEffectorPP inherited;	

	SPPInfo state;
	float	m_total;
	float	m_attack;
	float	m_release;

public:
					CShootingHitEffectorPP	(const SPPInfo &ppi, float life_time, float attack_time = 0.0f, float release_time = 0.0f);
	virtual	BOOL	Process					(SPPInfo& pp);
};

class CShootingHitEffector : public CEffectorCam {
	typedef CEffectorCam inherited;

	float total;
	float amp_x;
	float amp_y;
	float power;

public:
	CShootingHitEffector(float x, float y, float power);
	virtual	BOOL	Process					(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);
};

struct SShootingEffector {
	SPPInfo	ppi;
	float	time;
	float	time_attack;
	float	time_release;
};


