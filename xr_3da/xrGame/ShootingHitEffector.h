#pragma once

#include "../effectorPP.h"
#include "../effector.h"
#include "../cameramanager.h"

struct SShootingEffector {
	SPPInfo	ppi;
	float	time;
	float	time_attack;
	float	time_release;

	// camera effects
	float	ce_time;
	float	ce_amplitude;
	float	ce_period_number;
	float	ce_power;
};

class CShootingHitEffectorPP : public CEffectorPP {
	typedef CEffectorPP inherited;	
	const SShootingEffector*	se;
public:
					CShootingHitEffectorPP	(SShootingEffector* eff);
	virtual	BOOL	Process					(SPPInfo& pp);
};
