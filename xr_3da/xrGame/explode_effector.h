#include "../effector.h"

#define EXPLODE_EFFECTOR_TYPE_ID	7

class CExplodeEffector : public CEffector {
	typedef CEffector inherited;	

	float total;
	float max_amp;
	float period_number;
	float power;

public:
					CExplodeEffector		(float time, float amp, float periods, float power);
	virtual	BOOL	Process					(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);
};
