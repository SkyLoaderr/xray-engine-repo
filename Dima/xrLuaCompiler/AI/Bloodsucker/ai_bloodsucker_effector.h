#pragma	once

#include "..\\..\\..\\effectorPP.h"

#pragma todo("Dima to AlexMX : Move effector definition to engine headers, or move these headers to the xrGame to hold them together")
#define BLOODSUCKER_EFFECTOR_TYPE_ID	3

class CBloodsuckerEffector : public CEffectorPP {
protected:
	typedef CEffectorPP inherited;	
	float	time;
	float	max_past;
	SPPInfo max_power;

public:
					CBloodsuckerEffector		(float time);
	virtual			~CBloodsuckerEffector		();
	virtual	BOOL	Process						(SPPInfo& pp);
			void	Init						();
};

