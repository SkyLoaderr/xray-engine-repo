#pragma once

#include "../effectorPP.h"


class CMonsterEffector : public CEffectorPP {
	typedef CEffectorPP inherited;	
	
	SPPInfo state;			// current state
	float	m_total;		// total PP time
	float	m_attack;		// attack time in percents	[0..1]
	float	m_release;		// release time in percents	[0..1]

public:
					CMonsterEffector		(const SPPInfo &ppi, float life_time, float attack_time = 0.0f, float release_time = 0.0f);
	virtual	BOOL	Process					(SPPInfo& pp);
};

