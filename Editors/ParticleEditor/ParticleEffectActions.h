//---------------------------------------------------------------------------
#ifndef ParticleEffectActionsH
#define ParticleEffectActionsH

#include "pDomain.h"

struct EParticleAction
{
	struct PFloat{
    	float		val;
        float		mn;
        float		mx;
        PFloat		()	{val=0.f;mn=0.f;mx=0.f;}
        PFloat		(float _val, float _mn, float _mx):val(_val),mn(_mn),mx(_mx){;}
    };
	DEFINE_VECTOR(PDomain,PDomainVec,PDomainVecIt);
	DEFINE_VECTOR(PFloat,PFloatVec,PFloatVecIt);

	ref_str 		actionName;
	ref_str 		actionType;
    enum{
    	flEnabled	= (1<<0),
    	flDrawDomain= (1<<1),
    };
	Flags32				flags;
    PAPI::PActionEnum	type;

    RStrVec			floatNames;
	RStrVec			domainNames;
	RStrVec			boolNames;

	PFloatVec		floats;
	PDomainVec		domains;
	BOOLVec			bools;

    EParticleAction	(PAPI::PActionEnum	_type)
    {
    	flags.set	(flEnabled);
        type		= _type;
    }
protected:
	void			appendFloat	(LPCSTR name, float v, float mn=flt_min, float mx=flt_max);
	void			appendDomain(LPCSTR name, PDomain v);
	void			appendBool	(LPCSTR name, BOOL b);
public:    
    virtual void	Execute		()=0;
    virtual void	WriteCode	()=0;
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);

    virtual void 	Load		(IReader& F);
    virtual void 	Save		(IWriter& F);
    virtual void 	Render		();
};

struct EPAAvoid : public EParticleAction
{
					EPAAvoid	();
    virtual void	Execute		();
    virtual void	WriteCode	();
};
 
struct EPABounce : public EParticleAction
{
					EPABounce	();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPACallActionList : public EParticleAction
{
					EPACallActionList();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPACopyVertexB : public EParticleAction
{
					EPACopyVertexB();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPADamping : public EParticleAction
{
					EPADamping	();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAExplosion : public EParticleAction
{
					EPAExplosion();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAFollow : public EParticleAction
{
					EPAFollow	();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAGravitate : public EParticleAction
{
					EPAGravitate();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAGravity : public EParticleAction
{
					EPAGravity	();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAJet : public EParticleAction
{
					EPAJet		();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAKillOld : public EParticleAction
{
					EPAKillOld	();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAMatchVelocity : public EParticleAction
{
					EPAMatchVelocity();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAMove : public EParticleAction
{
					EPAMove		();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAOrbitLine : public EParticleAction
{
					EPAOrbitLine();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAOrbitPoint : public EParticleAction
{
					EPAOrbitPoint();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPARandomAccel : public EParticleAction
{
					EPARandomAccel();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPARandomDisplace : public EParticleAction
{
					EPARandomDisplace();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPARandomVelocity : public EParticleAction
{
					EPARandomVelocity();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPARestore : public EParticleAction
{
					EPARestore	();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPASink : public EParticleAction
{
					EPASink		();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPASinkVelocity : public EParticleAction
{
					EPASinkVelocity();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPASpeedLimit : public EParticleAction
{
					EPASpeedLimit();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPASource : public EParticleAction
{
					EPASource	();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPATargetColor : public EParticleAction
{
					EPATargetColor();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPATargetSize : public EParticleAction
{
					EPATargetSize();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPATargetRotate : public EParticleAction
{
					EPATargetRotate();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPATargetVelocity : public EParticleAction
{
					EPATargetVelocity();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

struct EPAVortex : public EParticleAction
{
					EPAVortex	();
    virtual void	Execute		();
    virtual void	WriteCode	();
};

EParticleAction* 	pCreateEAction(PAPI::PActionEnum type);
EParticleAction* 	pCreateEAction(PAPI::ParticleAction* src);
//---------------------------------------------------------------------------
#endif

/*

	 void pTargetVelocityD(float scale, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	 void pVertex(float x, float y, float z);

}
*/
