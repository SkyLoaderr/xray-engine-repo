//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffectActions.h"

//using namespace PS;
//---------------------------------------------------------------------------
PAPI::ParticleAction* pCreateEAction(PAPI::PActionEnum type)
{
	PAPI::ParticleAction* pa	= 0;
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

PAPI::ParticleAction* pCreateEAction(PAPI::ParticleAction* src)
{
	VERIFY(src);
	PAPI::ParticleAction* pa		= pCreateEAction(src->type);
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


