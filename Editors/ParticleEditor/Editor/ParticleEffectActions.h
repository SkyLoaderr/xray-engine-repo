//---------------------------------------------------------------------------
#ifndef ParticleEffectActionsH
#define ParticleEffectActionsH

struct EPAAvoid : public PAPI::PAAvoid
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};
 
struct EPABounce : public PAPI::PABounce
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPACallActionList : public PAPI::PACallActionList
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPACopyVertexB : public PAPI::PACopyVertexB
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPADamping : public PAPI::PADamping
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAExplosion : public PAPI::PAExplosion
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAFollow : public PAPI::PAFollow
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAGravitate : public PAPI::PAGravitate
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAGravity : public PAPI::PAGravity
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAJet : public PAPI::PAJet
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAKillOld : public PAPI::PAKillOld
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAMatchVelocity : public PAPI::PAMatchVelocity
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAMove : public PAPI::PAMove
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAOrbitLine : public PAPI::PAOrbitLine
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAOrbitPoint : public PAPI::PAOrbitPoint
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPARandomAccel : public PAPI::PARandomAccel
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPARandomDisplace : public PAPI::PARandomDisplace
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPARandomVelocity : public PAPI::PARandomVelocity
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPARestore : public PAPI::PARestore
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPASink : public PAPI::PASink
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPASinkVelocity : public PAPI::PASinkVelocity
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPASpeedLimit : public PAPI::PASpeedLimit
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPASource : public PAPI::PASource
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPATargetColor : public PAPI::PATargetColor
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPATargetSize : public PAPI::PATargetSize
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPATargetRotate : public PAPI::PATargetRotate
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPATargetVelocity : public PAPI::PATargetVelocity
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAVelocityD : public PAPI::PAVelocityD
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

struct EPAVortex : public PAPI::PAVortex
{
    virtual void 	Render		();
    virtual void 	FillProp	(PropItemVec& items, LPCSTR pref);
};

PAPI::ParticleAction* 	pCreateEAction(PAPI::PActionEnum type);
PAPI::ParticleAction* 	pCreateEAction(PAPI::ParticleAction* src);
//---------------------------------------------------------------------------
#endif
