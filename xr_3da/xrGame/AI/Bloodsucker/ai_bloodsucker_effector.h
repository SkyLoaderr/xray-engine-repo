#pragma	once

#include "../../../effector.h"

#define BLOODSUCKER_EFFECTOR_TYPE_ID	6

class CBloodsuckerEffector : public CEffector {
	typedef CEffector inherited;	

	float total;
		
public:
					CBloodsuckerEffector		(float time);
	virtual	BOOL	Process						(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);
};

