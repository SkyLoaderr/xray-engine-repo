//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffectActions.h"
#include "folderlib.h"
#include "PropertiesListHelper.h"

using namespace PAPI;
//---------------------------------------------------------------------------
EParticleAction* pCreateEAction(PAPI::PActionEnum type)
{
	EParticleAction* pa	= 0;
    switch(type){
    case PAPI::PAAvoidID:			pa = xr_new<EPAAvoid>			();	break;
    case PAPI::PABounceID:    		pa = xr_new<EPABounce>			();	break;
    case PAPI::PACallActionListID:  pa = xr_new<EPACallActionList>	();	break;
    case PAPI::PACopyVertexBID:    	pa = xr_new<EPACopyVertexB>		();	break;
    case PAPI::PADampingID:    		pa = xr_new<EPADamping>			();	break;
    case PAPI::PAExplosionID:    	pa = xr_new<EPAExplosion>		();	break;
    case PAPI::PAFollowID:    		pa = xr_new<EPAFollow>			();	break;
    case PAPI::PAGravitateID:    	pa = xr_new<EPAGravitate>		();	break;
    case PAPI::PAGravityID:    		pa = xr_new<EPAGravity>			();	break;
    case PAPI::PAJetID:    			pa = xr_new<EPAJet>				();	break;
    case PAPI::PAKillOldID:    		pa = xr_new<EPAKillOld>			();	break;
    case PAPI::PAMatchVelocityID:   pa = xr_new<EPAMatchVelocity>	();	break;
    case PAPI::PAMoveID:    		pa = xr_new<EPAMove>		   	();	break;
    case PAPI::PAOrbitLineID:    	pa = xr_new<EPAOrbitLine>		();	break;
    case PAPI::PAOrbitPointID:    	pa = xr_new<EPAOrbitPoint>		();	break;
    case PAPI::PARandomAccelID:    	pa = xr_new<EPARandomAccel>		();	break;
    case PAPI::PARandomDisplaceID:  pa = xr_new<EPARandomDisplace>	();	break;
    case PAPI::PARandomVelocityID:  pa = xr_new<EPARandomVelocity>	();	break;
    case PAPI::PARestoreID:    		pa = xr_new<EPARestore>			();	break;
    case PAPI::PASinkID:    		pa = xr_new<EPASink>		   	();	break;
    case PAPI::PASinkVelocityID:    pa = xr_new<EPASinkVelocity>   	();	break;
    case PAPI::PASourceID:    		pa = xr_new<EPASource>			();	break;
    case PAPI::PASpeedLimitID:    	pa = xr_new<EPASpeedLimit>		();	break;
    case PAPI::PATargetColorID:    	pa = xr_new<EPATargetColor>		();	break;
    case PAPI::PATargetSizeID:    	pa = xr_new<EPATargetSize>		();	break;
    case PAPI::PATargetRotateID:    pa = xr_new<EPATargetRotate> 	();	break;
    case PAPI::PATargetRotateDID:   pa = xr_new<EPATargetRotate> 	();	break;
    case PAPI::PATargetVelocityID:	pa = xr_new<EPATargetVelocity>	();	break;
    case PAPI::PATargetVelocityDID: pa = xr_new<EPATargetVelocity>	();	break;
    case PAPI::PAVortexID:    		pa = xr_new<EPAVortex>			();	break;
    default: NODEFAULT;
    }
    pa->type						= type;
	return pa;
}

EParticleAction* pCreateEAction(PAPI::ParticleAction* src)
{
	VERIFY(src);
	EParticleAction* pa				= pCreateEAction(src->type);
    switch(pa->type){
    case PAPI::PAAvoidID:			*dynamic_cast<PAPI::PAAvoid*			>(pa) = *dynamic_cast<PAPI::PAAvoid*			>(src);break;
    case PAPI::PABounceID:    		*dynamic_cast<PAPI::PABounce*			>(pa) = *dynamic_cast<PAPI::PABounce*			>(src);break;
    case PAPI::PACallActionListID:  *dynamic_cast<PAPI::PACallActionList*	>(pa) = *dynamic_cast<PAPI::PACallActionList*	>(src);break;
    case PAPI::PACopyVertexBID:    	*dynamic_cast<PAPI::PACopyVertexB*		>(pa) = *dynamic_cast<PAPI::PACopyVertexB*		>(src);break;
    case PAPI::PADampingID:    		*dynamic_cast<PAPI::PADamping*			>(pa) = *dynamic_cast<PAPI::PADamping*			>(src);break;
    case PAPI::PAExplosionID:    	*dynamic_cast<PAPI::PAExplosion*		>(pa) = *dynamic_cast<PAPI::PAExplosion*		>(src);break;
    case PAPI::PAFollowID:    		*dynamic_cast<PAPI::PAFollow*			>(pa) = *dynamic_cast<PAPI::PAFollow*			>(src);break;
    case PAPI::PAGravitateID:    	*dynamic_cast<PAPI::PAGravitate*		>(pa) = *dynamic_cast<PAPI::PAGravitate*		>(src);break;
    case PAPI::PAGravityID:    		*dynamic_cast<PAPI::PAGravity*			>(pa) = *dynamic_cast<PAPI::PAGravity*			>(src);break;
    case PAPI::PAJetID:    			*dynamic_cast<PAPI::PAJet*				>(pa) = *dynamic_cast<PAPI::PAJet*				>(src);break;
    case PAPI::PAKillOldID:    		*dynamic_cast<PAPI::PAKillOld*			>(pa) = *dynamic_cast<PAPI::PAKillOld*			>(src);break;
    case PAPI::PAMatchVelocityID:   *dynamic_cast<PAPI::PAMatchVelocity*	>(pa) = *dynamic_cast<PAPI::PAMatchVelocity*	>(src);break;
    case PAPI::PAMoveID:    		*dynamic_cast<PAPI::PAMove*				>(pa) = *dynamic_cast<PAPI::PAMove*				>(src);break;
    case PAPI::PAOrbitLineID:    	*dynamic_cast<PAPI::PAOrbitLine*		>(pa) = *dynamic_cast<PAPI::PAOrbitLine*		>(src);break;
    case PAPI::PAOrbitPointID:    	*dynamic_cast<PAPI::PAOrbitPoint*		>(pa) = *dynamic_cast<PAPI::PAOrbitPoint*		>(src);break;
    case PAPI::PARandomAccelID:    	*dynamic_cast<PAPI::PARandomAccel*		>(pa) = *dynamic_cast<PAPI::PARandomAccel*		>(src);break;
    case PAPI::PARandomDisplaceID:  *dynamic_cast<PAPI::PARandomDisplace*	>(pa) = *dynamic_cast<PAPI::PARandomDisplace*	>(src);break;
    case PAPI::PARandomVelocityID:  *dynamic_cast<PAPI::PARandomVelocity*	>(pa) = *dynamic_cast<PAPI::PARandomVelocity*	>(src);break;
    case PAPI::PARestoreID:    		*dynamic_cast<PAPI::PARestore*			>(pa) = *dynamic_cast<PAPI::PARestore*			>(src);break;
    case PAPI::PASinkID:    		*dynamic_cast<PAPI::PASink*				>(pa) = *dynamic_cast<PAPI::PASink*				>(src);break;
    case PAPI::PASinkVelocityID:    *dynamic_cast<PAPI::PASinkVelocity*		>(pa) = *dynamic_cast<PAPI::PASinkVelocity*		>(src);break;
    case PAPI::PASourceID:    		*dynamic_cast<PAPI::PASource*			>(pa) = *dynamic_cast<PAPI::PASource*			>(src);break;
    case PAPI::PASpeedLimitID:    	*dynamic_cast<PAPI::PASpeedLimit*		>(pa) = *dynamic_cast<PAPI::PASpeedLimit*		>(src);break;
    case PAPI::PATargetColorID:    	*dynamic_cast<PAPI::PATargetColor*		>(pa) = *dynamic_cast<PAPI::PATargetColor*		>(src);break;
    case PAPI::PATargetSizeID:    	*dynamic_cast<PAPI::PATargetSize*		>(pa) = *dynamic_cast<PAPI::PATargetSize*		>(src);break;
    case PAPI::PATargetRotateID:    *dynamic_cast<PAPI::PATargetRotate*		>(pa) = *dynamic_cast<PAPI::PATargetRotate*		>(src);break;
    case PAPI::PATargetRotateDID:   *dynamic_cast<PAPI::PATargetRotate*		>(pa) = *dynamic_cast<PAPI::PATargetRotate*		>(src);break;
    case PAPI::PATargetVelocityID:	*dynamic_cast<PAPI::PATargetVelocity*	>(pa) = *dynamic_cast<PAPI::PATargetVelocity*	>(src);break;
    case PAPI::PATargetVelocityDID: *dynamic_cast<PAPI::PATargetVelocity*	>(pa) = *dynamic_cast<PAPI::PATargetVelocity*	>(src);break;
    case PAPI::PAVortexID:    		*dynamic_cast<PAPI::PAVortex*			>(pa) = *dynamic_cast<PAPI::PAVortex*			>(src);break;
    default: NODEFAULT;
    }
	return pa;
}
//---------------------------------------------------------------------------
void 	EParticleAction::Render		()
{
	if (flags.is(flDrawDomain)&&flags.is(flEnabled)){
    	u32 clr						= 0xffffffff;
		for (PDomainVecIt it=domains.begin(); it!=domains.end(); it++)
        	it->Render				(clr);
    }
}
void 	EParticleAction::Load		(IReader& F)
{
	flags.set		(F.r_u32());
    for (PFloatVecIt f_it=floats.begin(); f_it!=floats.end(); f_it++)	f_it->val	= F.r_float();
    for (PDomainVecIt d_it=domains.begin(); d_it!=domains.end(); d_it++)F.r			(&(*d_it),sizeof(PDomain));
    for (BOOLIt b_it=bools.begin(); b_it!=bools.end(); b_it++)			*b_it		= F.r_u8();
}
void 	EParticleAction::Save		(IWriter& F)
{
	F.w_u32			(flags.get());
    for (PFloatVecIt f_it=floats.begin(); f_it!=floats.end(); f_it++)	F.w_float	(f_it->val);
    for (PDomainVecIt d_it=domains.begin(); d_it!=domains.end(); d_it++)F.w			(&(*d_it),sizeof(PDomain));
    for (BOOLIt b_it=bools.begin(); b_it!=bools.end(); b_it++)			F.w_u8		(*b_it);
}
void 	EParticleAction::FillProp	(PropItemVec& items, LPCSTR pref)
{
	PHelper.CreateRText				(items,FHelper.PrepareKey(pref,"Name"),&actionName);
    for (u32 d_idx=0; d_idx<domains.size(); d_idx++)
        domains[d_idx].FillProp		(items, FHelper.PrepareKey(pref,*domainNames[d_idx]).c_str());
    for (u32 f_idx=0; f_idx<floats.size(); f_idx++)
    	PHelper.CreateFloat			(items,	FHelper.PrepareKey(pref,*floatNames[f_idx]).c_str(), &floats[f_idx].val, floats[f_idx].mn, floats[f_idx].mx, 0.001f, 3);
    for (u32 b_idx=0; b_idx<bools.size(); b_idx++)
    	PHelper.CreateBOOL			(items,	FHelper.PrepareKey(pref,*boolNames[f_idx]).c_str(), &bools[f_idx]);
    PHelper.CreateFlag32			(items,	FHelper.PrepareKey(pref,"Enabled").c_str(), 		&flags, flEnabled);
    if (!domains.empty())
    	PHelper.CreateFlag32		(items,	FHelper.PrepareKey(pref,"Draw Domains").c_str(), 	&flags, flDrawDomain);
}
void EParticleAction::appendFloat	(LPCSTR name, float v, float mn, float mx)
{
	floatNames.push_back			(name);
	floats.push_back				(PFloat(v,mn,mx));
}
void EParticleAction::appendDomain	(LPCSTR name, PDomain v)
{
	domainNames.push_back			(name);
	domains.push_back				(v);
}

void EParticleAction::appendBool	(LPCSTR name, BOOL v)
{
	boolNames.push_back				(name);
	bools.push_back					(v);
}

//------------------------------------------------------------------------------
#define EXPAND_DOMAIN(D)			D.type,\
									D.f[0], D.f[1], D.f[2],\
									D.f[3], D.f[4], D.f[5],\
						            D.f[6], D.f[7], D.f[9]

EPAAvoid::EPAAvoid					():EParticleAction(PAPI::PAAvoidID)
{
	actionType						= "Avoid";
	actionName						= actionType;
    appendFloat						("magnitude",	0.f);
    appendFloat						("epsilon",		0.f);
    appendFloat						("look ahead",	0.f);
    appendDomain					("domain",PDomain());
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPAAvoid::Execute			()
{
	if (flags.is(flEnabled))
		pAvoid(floats[0].val, floats[1].val, floats[2].val, EXPAND_DOMAIN(domains[0]), bools[0], bools[1]);
}
void	EPAAvoid::WriteCode			()
{
}

EPABounce::EPABounce				():EParticleAction(PAPI::PABounceID)
{
	actionType						= "Bounce";
	actionName						= actionType;
    appendFloat						("friction",0.f);
    appendFloat						("resilience",0.f);
    appendFloat						("cutoff",0.f);
    appendDomain					("domain",PDomain());
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPABounce::Execute			()
{
	if (flags.is(flEnabled))
	 	pBounce(floats[0].val, floats[1].val, floats[2].val, EXPAND_DOMAIN(domains[0]), bools[0], bools[1]);
}
void	EPABounce::WriteCode	  	()
{
}

EPACallActionList::EPACallActionList():EParticleAction(PAPI::PACallActionListID)
{
	actionType						= "CallActionList";
	actionName						= actionType;
}
void	EPACallActionList::Execute 	()
{
//.	if (flags.is(flEnabled))
//.		pCallActionList				(bools[0]);
}
void	EPACallActionList::WriteCode()
{
}

EPACopyVertexB::EPACopyVertexB  	():EParticleAction(PAPI::PACopyVertexBID)
{
	actionType						= "CopyVertexB";
	actionName						= actionType;
    appendBool						("copy position", TRUE);
}
void	EPACopyVertexB::Execute	   	()
{
	if (flags.is(flEnabled))
		pCopyVertexB(bools[0]);
}
void	EPACopyVertexB::WriteCode 	()
{
}

EPADamping::EPADamping				():EParticleAction(PAPI::PADampingID)
{
	actionType						= "Damping";
	actionName						= actionType;
    appendFloat						("damping_x",0.f);
    appendFloat						("damping_y",0.f);
    appendFloat						("damping_z",0.f);
    appendFloat						("vlow",0.f);
    appendFloat						("vhigh",P_MAXFLOAT);
}
void	EPADamping::Execute			()
{
	if (flags.is(flEnabled))
		pDamping(floats[0].val, floats[1].val, floats[2].val, floats[3].val, floats[4].val);
}
void	EPADamping::WriteCode	   	()
{
}

EPAExplosion::EPAExplosion			():EParticleAction(PAPI::PAExplosionID)
{
	actionType						= "Explosion";
	actionName						= actionType;
    appendFloat						("center_x",0.f);
    appendFloat						("center_y",0.f);
    appendFloat						("center_z",0.f);
    appendFloat						("velocity",0.f);
    appendFloat						("magnitude",0.f);
    appendFloat						("stdev",0.f);
    appendFloat						("epsilon",P_EPS);
    appendFloat						("age",0.f);
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPAExplosion::Execute	  	()
{
	if (flags.is(flEnabled))
    	pExplosion(floats[0].val, floats[1].val, floats[2].val, floats[3].val, floats[4].val, floats[5].val, floats[6].val, floats[7].val, bools[0], bools[1]);
}
void	EPAExplosion::WriteCode	  	()
{
}

EPAFollow::EPAFollow				():EParticleAction(PAPI::PAFollowID)
{
	actionType						= "Follow";
	actionName						= actionType;
    appendFloat						("magnitude",0.f);
    appendFloat						("epsilon",P_EPS);
    appendFloat						("max_radius",P_MAXFLOAT);
}
void	EPAFollow::Execute			()
{
	if (flags.is(flEnabled))
		pFollow(floats[0].val, floats[1].val, floats[2].val);
}
void	EPAFollow::WriteCode	  	()
{
}

EPAGravitate::EPAGravitate			():EParticleAction(PAPI::PAGravitateID)
{
	actionType						= "Gravitate";
	actionName						= actionType;
    appendFloat						("magnitude",0.f);
    appendFloat						("epsilon",P_EPS);
    appendFloat						("max_radius",P_MAXFLOAT);
}
void	EPAGravitate::Execute	   	()
{
	if (flags.is(flEnabled))
		pGravitate(floats[0].val, floats[1].val, floats[2].val);
}
void	EPAGravitate::WriteCode	  	()
{
}

EPAGravity::EPAGravity				():EParticleAction(PAPI::PAGravityID)
{
	actionType						= "Gravity";
	actionName						= actionType;
    appendFloat						("dir_x",0.f);
    appendFloat						("dir_y",0.f);
    appendFloat						("dir_z",0.f);
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPAGravity::Execute			()
{
	if (flags.is(flEnabled))
		pGravity(floats[0].val, floats[1].val, floats[2].val, bools[0], bools[1]);
}
void	EPAGravity::WriteCode	   	()
{
}

EPAJet::EPAJet						():EParticleAction(PAPI::PAJetID)
{
	actionType						= "Jet";
	actionName						= actionType;
    appendFloat						("center_x",0.f);
    appendFloat						("center_y",0.f);
    appendFloat						("center_z",0.f);
    appendFloat						("magnitude",0.f);
    appendFloat						("epsilon",P_EPS);
    appendFloat						("max_radius",P_MAXFLOAT);
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPAJet::Execute				()
{
	if (flags.is(flEnabled))
	 	pJet(floats[0].val, floats[1].val, floats[2].val, floats[3].val, floats[4].val, floats[5].val, bools[0], bools[1]);
}
void	EPAJet::WriteCode			()
{
}

EPAKillOld::EPAKillOld				():EParticleAction(PAPI::PAKillOldID)
{
	actionType						= "KillOld";
	actionName						= actionType;
    appendFloat						("age_limit",		0.f);
    appendBool						("kill_less_than",	FALSE);
}
void	EPAKillOld::Execute			()
{
	if (flags.is(flEnabled))
		pKillOld(floats[0].val, bools[0]);
}
void	EPAKillOld::WriteCode	  	()
{
}

EPAMatchVelocity::EPAMatchVelocity	():EParticleAction(PAPI::PAMatchVelocityID)
{
	actionType						= "MatchVelocity";
	actionName						= actionType;
    appendFloat						("magnitude",0.f);
    appendFloat						("epsilon",P_EPS);
    appendFloat						("max_radius",P_MAXFLOAT);
}
void	EPAMatchVelocity::Execute 	()
{
	if (flags.is(flEnabled))
        pMatchVelocity(floats[0].val, floats[1].val, floats[2].val);
}
void	EPAMatchVelocity::WriteCode	()
{
}

EPAMove::EPAMove					():EParticleAction(PAPI::PAMoveID)
{
	actionType						= "Move";
	actionName						= actionType;
}
void	EPAMove::Execute			()
{
	if (flags.is(flEnabled))
		pMove();
}
void	EPAMove::WriteCode			()
{
}

EPAOrbitLine::EPAOrbitLine			():EParticleAction(PAPI::PAOrbitLineID)
{
	actionType						= "OrbitLine";
	actionName						= actionType;
    appendFloat						("p_x",				0.f);
    appendFloat						("p_y",				0.f);
    appendFloat						("p_z",				0.f);
    appendFloat						("axis_x",			0.f);
    appendFloat						("axis_y",			0.f);
    appendFloat						("axis_z",			0.f);
    appendFloat						("magnitude",		1.f);
    appendFloat						("epsilon",			P_EPS);
    appendFloat						("max_radius",		P_MAXFLOAT);
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPAOrbitLine::Execute	 	()
{
	if (flags.is(flEnabled))
		pOrbitLine(floats[0].val, floats[1].val, floats[2].val, floats[3].val, floats[4].val, floats[5].val, floats[6].val, floats[7].val, floats[8].val, bools[0], bools[1]);
}
void	EPAOrbitLine::WriteCode	   	()
{
}

EPAOrbitPoint::EPAOrbitPoint		():EParticleAction(PAPI::PAOrbitPointID)
{
	actionType						= "OrbitPoint";
	actionName						= actionType;
    appendFloat						("center_x",		0.f);
    appendFloat						("center_y",		0.f);
    appendFloat						("center_z",		0.f);
    appendFloat						("magnitude",		1.f);
    appendFloat						("epsilon",			P_EPS);
    appendFloat						("max_radius",		P_MAXFLOAT);
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPAOrbitPoint::Execute	   	()
{
	if (flags.is(flEnabled))
		pOrbitPoint(floats[0].val, floats[1].val, floats[2].val, floats[3].val, floats[4].val, floats[5].val, bools[0], bools[1]);
}
void	EPAOrbitPoint::WriteCode	()
{
}

EPARandomAccel::EPARandomAccel		():EParticleAction(PAPI::PARandomAccelID)
{
	actionType						= "RandomAccel";
	actionName						= actionType;
    appendDomain					("domain",PDomain());
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPARandomAccel::Execute	   	()
{
	if (flags.is(flEnabled))
		pRandomAccel(EXPAND_DOMAIN(domains[0]), bools[0], bools[1]);
}
void	EPARandomAccel::WriteCode	()
{
}

EPARandomDisplace::EPARandomDisplace():EParticleAction(PAPI::PARandomDisplaceID)
{
	actionType						= "RandomDisplace";
	actionName						= actionType;
    appendDomain					("domain",PDomain());
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPARandomDisplace::Execute 	()
{
	if (flags.is(flEnabled))
		pRandomDisplace(EXPAND_DOMAIN(domains[0]), bools[0], bools[1]);

}
void	EPARandomDisplace::WriteCode()
{
}

EPARandomVelocity::EPARandomVelocity():EParticleAction(PAPI::PARandomVelocityID)
{
	actionType						= "RandomVelocity";
	actionName						= actionType;
    appendDomain					("domain",PDomain());
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPARandomVelocity::Execute 	()
{
	if (flags.is(flEnabled))
		pRandomVelocity(EXPAND_DOMAIN(domains[0]), bools[0], bools[1]);

}
void	EPARandomVelocity::WriteCode()
{
}

EPARestore::EPARestore				():EParticleAction(PAPI::PARestoreID)
{
	actionType						= "Restore";
	actionName						= actionType;
    appendFloat						("time",			0.f);
}
void	EPARestore::Execute			()
{
	if (flags.is(flEnabled))
		pRestore(floats[0].val);
}
void	EPARestore::WriteCode	  	()
{
}

EPASink::EPASink					():EParticleAction(PAPI::PASinkID)
{
	actionType						= "Sink";
	actionName						= actionType;
    appendBool						("kill inside",		TRUE);
    appendDomain					("domain",			PDomain());
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPASink::Execute			()
{
	if (flags.is(flEnabled))
		pSink(bools[0], EXPAND_DOMAIN(domains[0]), bools[1], bools[2]);
}
void	EPASink::WriteCode			()
{
}

EPASinkVelocity::EPASinkVelocity	():EParticleAction(PAPI::PASinkVelocityID)
{
	actionType						= "SinkVelocity";
	actionName						= actionType;
    appendBool						("kill inside",		TRUE);
    appendDomain					("domain",PDomain());
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPASinkVelocity::Execute   	()
{
	if (flags.is(flEnabled))
		pSinkVelocity(bools[0], EXPAND_DOMAIN(domains[0]), bools[1], bools[2]);
}
void	EPASinkVelocity::WriteCode 	()
{
}

EPASource::EPASource				():EParticleAction(PAPI::PASourceID)
{
	actionType						= "Source";
	actionName						= actionType;
	appendFloat						("rate",			10);
	appendDomain					("domain",			PDomain());
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPASource::Execute			()
{
	if (flags.is(flEnabled))
		pSource(floats[0].val, EXPAND_DOMAIN(domains[0]), bools[0], bools[1]);
}
void	EPASource::WriteCode		()
{
}

EPASpeedLimit::EPASpeedLimit		():EParticleAction(PAPI::PASpeedLimitID)
{
	actionType						= "SpeedLimit";
	actionName						= actionType;
    appendFloat						("min_speed",			0.f);
    appendFloat						("max_speed",			P_MAXFLOAT);
}
void	EPASpeedLimit::Execute	 	()
{
	if (flags.is(flEnabled))
		pSpeedLimit(floats[0].val, floats[1].val);
}
void	EPASpeedLimit::WriteCode  	()
{
}

EPATargetColor::EPATargetColor		():EParticleAction(PAPI::PATargetColorID)
{
	actionType						= "TargetColor";
	actionName						= actionType;
    appendFloat						("color_r",			1.f);
    appendFloat						("color_g",			1.f);
    appendFloat						("color_b",			1.f);
    appendFloat						("color_a",			1.f);
    appendFloat						("scale",			1.f);
}
void	EPATargetColor::Execute	  	()
{
	if (flags.is(flEnabled))
		pTargetColor(floats[0].val, floats[1].val, floats[2].val, floats[3].val, floats[4].val);
}
void	EPATargetColor::WriteCode  	()
{
}

EPATargetSize::EPATargetSize		():EParticleAction(PAPI::PATargetSizeID)
{
	actionType						= "TargetSize";
	actionName						= actionType;
    appendFloat						("size x",			1.f);
    appendFloat						("size y",			1.f);
    appendFloat						("size z",			1.f);
    appendFloat						("scale x",			0.f);
    appendFloat						("scale y",			0.f);
    appendFloat						("scale z",			0.f);
}
void	EPATargetSize::Execute	  	()
{
	if (flags.is(flEnabled))
		pTargetSize(floats[0].val, floats[1].val, floats[2].val, floats[3].val, floats[4].val, floats[5].val);
}
void	EPATargetSize::WriteCode  	()
{
}

EPATargetRotate::EPATargetRotate	():EParticleAction(PAPI::PATargetRotateID)
{
	actionType						= "TargetRotate";
	actionName						= actionType;
    appendFloat						("rot x",			0.f);
    appendFloat						("rot y",			0.f);
    appendFloat						("rot z",			0.f);
    appendFloat						("scale",			0.f);
}
void	EPATargetRotate::Execute   	()
{
	if (flags.is(flEnabled))
		pTargetRotate(floats[0].val, floats[1].val, floats[2].val, floats[3].val);
}
void	EPATargetRotate::WriteCode 	()
{
}

EPATargetVelocity::EPATargetVelocity():EParticleAction(PAPI::PATargetVelocityID)
{
	actionType						= "TargetVelocity";
	actionName						= actionType;
    appendFloat						("vel x",			0.f);
    appendFloat						("vel x",			0.f);
    appendFloat						("vel x",			0.f);
    appendFloat						("scale",			0.f);
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPATargetVelocity::Execute	()
{
	if (flags.is(flEnabled))
		pTargetVelocity(floats[0].val, floats[1].val, floats[2].val, floats[3].val, bools[0], bools[1]);
}
void	EPATargetVelocity::WriteCode()
{
}

EPAVortex::EPAVortex				():EParticleAction(PAPI::PAVortexID)
{
	actionType						= "Vortex";
	actionName						= actionType;
    appendFloat						("center_x",		0.f);
    appendFloat						("center_y",		0.f);
    appendFloat						("center_z",		0.f);
    appendFloat						("axis_x",			0.f);
    appendFloat						("axis_y",			0.f);
    appendFloat						("axis_z",			0.f);
    appendFloat						("magnitude",		1.f);
    appendFloat						("epsilon",			P_EPS);
    appendFloat						("max_radius",		P_MAXFLOAT);
    appendBool						("allow translate",	TRUE);
    appendBool						("allow rotate",	TRUE);
}
void	EPAVortex::Execute			()
{
	if (flags.is(flEnabled))
         pVortex(floats[0].val, floats[1].val, floats[2].val, floats[3].val, floats[4].val, floats[5].val, floats[6].val, floats[7].val, floats[8].val, bools[0], bools[1]);
}
void	EPAVortex::WriteCode 		()
{
}

