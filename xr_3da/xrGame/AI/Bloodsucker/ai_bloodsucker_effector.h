#pragma	once

class CEffectorPP;

#define BLOODSUCKER_EFFECTOR_TYPE_ID	3

class CBloodsuckerEffector : public CEffectorPP {
	typedef inherited CEffectorPP;	
	float	fLifeTime;
	
public:
					CBloodsuckerEffector		(float time);
	virtual			~CBloodsuckerEffector		();
	virtual	BOOL	Process						(SPPInfo& pp);
			void	Init						();
};

