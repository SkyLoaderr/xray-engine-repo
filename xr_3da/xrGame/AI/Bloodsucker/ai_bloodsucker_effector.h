#pragma	once

#include "..\\..\\..\\effectorPP.h"


#define BLOODSUCKER_EFFECTOR_TYPE_ID	3

class CBloodsuckerEffector : public CEffectorPP {
	typedef CEffectorPP inherited;	
	float	fLifeTime;
	float	time;
	
	float	max_past;
	SPPInfo max_power;

public:
					CBloodsuckerEffector		(float time);
	virtual			~CBloodsuckerEffector		();
	virtual	BOOL	Process						(SPPInfo& pp);
			void	Init						();
};

