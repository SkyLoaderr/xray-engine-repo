#include "stdafx.h"
#pragma hdrstop

// For Windows DLL.
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call){
		case DLL_PROCESS_ATTACH:	break;
		case DLL_THREAD_ATTACH:		break;
		case DLL_THREAD_DETACH:		break;
		case DLL_PROCESS_DETACH:	break;
    }
    return TRUE;
}

namespace PAPI{

// static variables
float ParticleAction::dt;
_ParticleState::ParticleEffectVec	_ParticleState::effect_vec;
_ParticleState::LPParticleActionVec	_ParticleState::alist_vec;

// This is the global state.
_ParticleState 	__ps;
_ParticleState&	_GetPState		()						{return __ps;}

// Get a pointer to the particles in gp memory
ParticleEffect* _GetEffectPtr	(int p_effect_num)		{return __ps.GetEffectPtr(p_effect_num);}
PAVec*			_GetListPtr		(int action_list_num)	{return __ps.GetListPtr(action_list_num);}

_ParticleState::_ParticleState()
{
	in_call_list	= FALSE;
	in_new_list		= FALSE;
	
	dt				= 1.0f;
	
	effect_id		= -1;
	list_id			= -1;
	peff			= NULL;
	pact			= NULL;

	ResetState		();
}

void _ParticleState::ResetState()
{
	flags.set		(u32(PASource::flVertexB_tracks),TRUE);
	Size			= pDomain(PDPoint, 1.0f, 1.0f, 1.0f);
	Vel				= pDomain(PDPoint, 0.0f, 0.0f, 0.0f);
	VertexB			= pDomain(PDPoint, 0.0f, 0.0f, 0.0f);
	Color			= pDomain(PDPoint, 1.0f, 1.0f, 1.0f);
	Rot				= pDomain(PDPoint, 1.0f, 1.0f, 1.0f);
	Alpha			= 1.0f;
	Age				= 0.0f;
	AgeSigma		= 0.0f;
	parent_motion	= 0.f;
}

ParticleEffect *_ParticleState::GetEffectPtr(int p_effect_num)
{
	R_ASSERT(p_effect_num>=0&&p_effect_num<(int)effect_vec.size());
	return effect_vec[p_effect_num];
}

PAVec* _ParticleState::GetListPtr(int a_list_num)
{
	R_ASSERT(a_list_num>=0&&a_list_num<(int)alist_vec.size());
	return &alist_vec[a_list_num];
}

// Return an index into the list of particle effects where
// p_effect_count effects can be added.
int _ParticleState::GenerateEffects(int p_effect_count)
{
	int num_empty = 0;
	int first_empty = -1;
	
	for(int i=0; i<(int)effect_vec.size(); i++){
		if(effect_vec[i]){
			num_empty = 0;
			first_empty = -1;
		}else{
			if(first_empty < 0)
				first_empty = i;
			num_empty++;
			if(num_empty >= p_effect_count)
				return first_empty;
		}
	}
	
	// Couldn't find a big enough gap. Reallocate.
	first_empty = effect_vec.size();
    for (int k=0; k<p_effect_count; k++)
		effect_vec.push_back	(0);
	
	return first_empty;
}

// Return an index into the list of action lists where
// list_count lists can be added.
int _ParticleState::GenerateLists(int list_count)
{
	int num_empty = 0;
	int first_empty = -1;
	
	for(int i=0; i<(int)alist_vec.size(); i++)
	{
		if(!alist_vec[i].empty())
		{
			num_empty = 0;
			first_empty = -1;
		}
		else
		{
			if(first_empty < 0)
				first_empty = i;
			num_empty++;
			if(num_empty >= list_count)
				return first_empty;
		}
	}
	
	// Couldn't find a big enough gap. Allocate.
	first_empty = alist_vec.size();
    for (int k=0; k<list_count; k++)
		alist_vec.push_back(PAVec());
	
	return first_empty;
}

ParticleAction* pCreateAction(PActionEnum type)
{
	ParticleAction* pa			= 0;
    switch(type){
    case PAAvoidID:				pa = xr_new<PAAvoid>();				break;
    case PABounceID:    		pa = xr_new<PABounce>();			break;
    case PACallActionListID:    pa = xr_new<PACallActionList>();	break;
    case PACopyVertexBID:    	pa = xr_new<PACopyVertexB>();		break;
    case PADampingID:    		pa = xr_new<PADamping>();			break;
    case PAExplosionID:    		pa = xr_new<PAExplosion>();			break;
    case PAFollowID:    		pa = xr_new<PAFollow>();			break;
    case PAGravitateID:    		pa = xr_new<PAGravitate>();			break;
    case PAGravityID:    		pa = xr_new<PAGravity>();			break;
    case PAJetID:    			pa = xr_new<PAJet>();				break;
    case PAKillOldID:    		pa = xr_new<PAKillOld>();			break;
    case PAMatchVelocityID:    	pa = xr_new<PAMatchVelocity>();		break;
    case PAMoveID:    			pa = xr_new<PAMove>();				break;
    case PAOrbitLineID:    		pa = xr_new<PAOrbitLine>();			break;
    case PAOrbitPointID:    	pa = xr_new<PAOrbitPoint>();		break;
    case PARandomAccelID:    	pa = xr_new<PARandomAccel>();		break;
    case PARandomDisplaceID:    pa = xr_new<PARandomDisplace>();	break;
    case PARandomVelocityID:    pa = xr_new<PARandomVelocity>();	break;
    case PARestoreID:    		pa = xr_new<PARestore>();			break;
    case PASinkID:    			pa = xr_new<PASink>();				break;
    case PASinkVelocityID:    	pa = xr_new<PASinkVelocity>();		break;
    case PASourceID:    		pa = xr_new<PASource>();			break;
    case PASpeedLimitID:    	pa = xr_new<PASpeedLimit>();		break;
    case PATargetColorID:    	pa = xr_new<PATargetColor>();		break;
    case PATargetSizeID:    	pa = xr_new<PATargetSize>();		break;
    case PATargetRotateID:    	pa = xr_new<PATargetRotate>();		break;
    case PATargetRotateDID:    	pa = xr_new<PATargetRotate>();		break;
    case PATargetVelocityID:    pa = xr_new<PATargetVelocity>(); 	break;
    case PATargetVelocityDID:   pa = xr_new<PATargetVelocity>();	break;
    case PAVortexID:    		pa = xr_new<PAVortex>();			break;
    default: NODEFAULT;
    }
    pa->type					= type;
	return pa;
}

ParticleAction* pCreateAction(ParticleAction* src)
{
	VERIFY(src);
	ParticleAction* pa			= pCreateAction(src->type);
    switch(src->type){
    case PAAvoidID:				*dynamic_cast<PAAvoid*			>(pa) = *dynamic_cast<PAAvoid*			>(src);break;
    case PABounceID:    		*dynamic_cast<PABounce*			>(pa) = *dynamic_cast<PABounce*			>(src);break;
    case PACallActionListID:    *dynamic_cast<PACallActionList*	>(pa) = *dynamic_cast<PACallActionList*	>(src);break;
    case PACopyVertexBID:    	*dynamic_cast<PACopyVertexB*   	>(pa) = *dynamic_cast<PACopyVertexB*  	>(src);break;
    case PADampingID:    		*dynamic_cast<PADamping*	   	>(pa) = *dynamic_cast<PADamping*	  	>(src);break;
    case PAExplosionID:    		*dynamic_cast<PAExplosion*		>(pa) = *dynamic_cast<PAExplosion*		>(src);break;
    case PAFollowID:    		*dynamic_cast<PAFollow*			>(pa) = *dynamic_cast<PAFollow*			>(src);break;
    case PAGravitateID:    		*dynamic_cast<PAGravitate*		>(pa) = *dynamic_cast<PAGravitate*		>(src);break;
    case PAGravityID:    		*dynamic_cast<PAGravity*	   	>(pa) = *dynamic_cast<PAGravity*	  	>(src);break;
    case PAJetID:    			*dynamic_cast<PAJet*		   	>(pa) = *dynamic_cast<PAJet*		   	>(src);break;
    case PAKillOldID:    		*dynamic_cast<PAKillOld*	   	>(pa) = *dynamic_cast<PAKillOld*	  	>(src);break;
    case PAMatchVelocityID:    	*dynamic_cast<PAMatchVelocity*	>(pa) = *dynamic_cast<PAMatchVelocity*	>(src);break;
    case PAMoveID:    			*dynamic_cast<PAMove*		   	>(pa) = *dynamic_cast<PAMove*		  	>(src);break;
    case PAOrbitLineID:    		*dynamic_cast<PAOrbitLine*		>(pa) = *dynamic_cast<PAOrbitLine*		>(src);break;
    case PAOrbitPointID:    	*dynamic_cast<PAOrbitPoint*		>(pa) = *dynamic_cast<PAOrbitPoint*		>(src);break;
    case PARandomAccelID:    	*dynamic_cast<PARandomAccel*   	>(pa) = *dynamic_cast<PARandomAccel*   	>(src);break;
    case PARandomDisplaceID:    *dynamic_cast<PARandomDisplace*	>(pa) = *dynamic_cast<PARandomDisplace*	>(src);break;
    case PARandomVelocityID:    *dynamic_cast<PARandomVelocity*	>(pa) = *dynamic_cast<PARandomVelocity*	>(src);break;
    case PARestoreID:    		*dynamic_cast<PARestore*	   	>(pa) = *dynamic_cast<PARestore*	   	>(src);break;
    case PASinkID:    			*dynamic_cast<PASink*		   	>(pa) = *dynamic_cast<PASink*		   	>(src);break;
    case PASinkVelocityID:    	*dynamic_cast<PASinkVelocity*  	>(pa) = *dynamic_cast<PASinkVelocity* 	>(src);break;
    case PASourceID:    		*dynamic_cast<PASource*		   	>(pa) = *dynamic_cast<PASource*			>(src);break;
    case PASpeedLimitID:    	*dynamic_cast<PASpeedLimit*		>(pa) = *dynamic_cast<PASpeedLimit*		>(src);break;
    case PATargetColorID:    	*dynamic_cast<PATargetColor*	>(pa) = *dynamic_cast<PATargetColor*   	>(src);break;
    case PATargetSizeID:    	*dynamic_cast<PATargetSize*		>(pa) = *dynamic_cast<PATargetSize*		>(src);break;
    case PATargetRotateID:    	*dynamic_cast<PATargetRotate*	>(pa) = *dynamic_cast<PATargetRotate* 	>(src);break;
    case PATargetRotateDID:    	*dynamic_cast<PATargetRotate*	>(pa) = *dynamic_cast<PATargetRotate* 	>(src);break;
    case PATargetVelocityID:    *dynamic_cast<PATargetVelocity*	>(pa) = *dynamic_cast<PATargetVelocity*	>(src);break;
    case PATargetVelocityDID:   *dynamic_cast<PATargetVelocity*	>(pa) = *dynamic_cast<PATargetVelocity*	>(src);break;
    case PAVortexID:    		*dynamic_cast<PAVortex*			>(pa) = *dynamic_cast<PAVortex*			>(src);break;
    default: NODEFAULT;
    }
	return pa;
}
////////////////////////////////////////////////////////
// Auxiliary calls
void _pCallActionList(PAVec* pa, ParticleEffect *pe)
{
	// All these require a particle effect, so check for it.
	if(pe == NULL)
		return;
	
	// Step through all the actions in the action list.
	for(PAVecIt it=pa->begin(); it!=pa->end(); it++)
    	(*it)->Execute(pe);
}

void _pCallAction(ParticleAction* pa, ParticleEffect *pe)
{
	// All these require a particle effect, so check for it.
	if(pe == NULL)
		return;
	
	// Step through all the actions in the action list.
    pa->Execute(pe);
}

// Add the incoming action to the end of the current action list.
void _pAddActionToList(ParticleAction *S)
{
	_ParticleState &_ps		= _GetPState();

	if(!_ps.in_new_list)	return; // ERROR
	if(_ps.pact == NULL)   	return; // ERROR
	if(_ps.list_id < 0)		return; // ERROR

	_ps.pact->push_back		(S);
}

PARTICLEDLL_API void pAddActionToList(ParticleAction* S)
{
	_pAddActionToList(pCreateAction(S));
}

////////////////////////////////////////////////////////
// State setting calls
PARTICLEDLL_API void pResetState()
{
	_ParticleState &_ps = _GetPState();
	_ps.ResetState	();
}

PARTICLEDLL_API void pColor(float red, float green, float blue, float alpha)
{
	_ParticleState &_ps = _GetPState();

	_ps.Alpha = alpha;
	_ps.Color = pDomain(PDPoint, red, green, blue);
}

PARTICLEDLL_API void pColorD(float alpha, PDomainEnum dtype,
			 float a0, float a1, float a2,
			 float a3, float a4, float a5,
			 float a6, float a7, float a8)
{
	_ParticleState &_ps = _GetPState();

	_ps.Alpha = alpha;
	_ps.Color = pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
}

PARTICLEDLL_API void pVelocity(float x, float y, float z)
{
	_ParticleState &_ps = _GetPState();

	_ps.Vel = pDomain(PDPoint, x, y, z);
}

PARTICLEDLL_API void __cdecl pVelocityD(PDomainEnum dtype,
				float a0, float a1, float a2,
				float a3, float a4, float a5,
				float a6, float a7, float a8)
{
	_ParticleState &_ps = _GetPState();

	_ps.Vel = pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
}

PARTICLEDLL_API void pVertexB(float x, float y, float z)
{
	_ParticleState &_ps = _GetPState();

	_ps.VertexB = pDomain(PDPoint, x, y, z);
}

PARTICLEDLL_API void pVertexBD(PDomainEnum dtype,
			   float a0, float a1, float a2,
			   float a3, float a4, float a5,
			   float a6, float a7, float a8)
{
	_ParticleState &_ps = _GetPState();

	_ps.VertexB = pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
}


PARTICLEDLL_API void pVertexBTracks(BOOL trackVertex)
{
	_ParticleState &_ps = _GetPState();

	_ps.flags.set(u32(PASource::flVertexB_tracks),trackVertex);
}

PARTICLEDLL_API void pParentMotion(float scale)
{
	_ParticleState &_ps = _GetPState();

	_ps.parent_motion = scale;
}

PARTICLEDLL_API void pSize(float size_x, float size_y, float size_z)
{
	_ParticleState &_ps = _GetPState();

	_ps.Size = pDomain(PDPoint, size_x, size_y, size_z);
}

PARTICLEDLL_API void pSizeD(PDomainEnum dtype,
			   float a0, float a1, float a2,
			   float a3, float a4, float a5,
			   float a6, float a7, float a8)
{
	_ParticleState &_ps = _GetPState();

	_ps.Size = pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
}

PARTICLEDLL_API void pRotate(float x, float y, float z)
{
	_ParticleState &_ps = _GetPState();

	_ps.Rot = pDomain(PDPoint, x, y, z);
}

PARTICLEDLL_API void pRotateD(PDomainEnum dtype,
									  float a0, float a1, float a2,
									  float a3, float a4, float a5,
									  float a6, float a7, float a8)
{
	_ParticleState &_ps = _GetPState();

	_ps.Rot = pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
}

PARTICLEDLL_API void pStartingAge(float age, float sigma)
{
	_ParticleState &_ps = _GetPState();

	_ps.Age = age;
	_ps.AgeSigma = sigma;
}

PARTICLEDLL_API void pTimeStep(float newDT)
{
	_ParticleState &_ps = _GetPState();

	_ps.dt = newDT;
}

////////////////////////////////////////////////////////
// Action List Calls

PARTICLEDLL_API int pGenActionLists(int action_list_count)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return -1; // ERROR
	
	int ind = _ps.GenerateLists(action_list_count);
	
	for(int i=ind; i<ind+action_list_count; i++)
		_ps.alist_vec[i].reserve(8);

	return ind;
}

PARTICLEDLL_API void pNewActionList(int action_list_num)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list) return; // ERROR
	
	_ps.pact 			= _ps.GetListPtr(action_list_num);
	if(_ps.pact == NULL)return; // ERROR
	
	_ps.list_id 		= action_list_num;
	_ps.in_new_list 	= TRUE;

    for (PAVecIt it=_ps.pact->begin(); it!=_ps.pact->end(); it++) 
    	xr_delete		(*it);
    _ps.pact->clear		();
}

PARTICLEDLL_API void pEndActionList()
{
	_ParticleState &_ps = _GetPState();

	if(!_ps.in_new_list)
		return; // ERROR
	
	_ps.in_new_list = FALSE;
	
	_ps.pact = NULL;
	_ps.list_id = -1;
}

PARTICLEDLL_API void pDeleteActionLists(int action_list_num, int action_list_count)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return; // ERROR

	if(action_list_num < 0)
		return; // ERROR

	if(action_list_num + action_list_count > (int)_ps.alist_vec.size())
		return; // ERROR

	for(int i = action_list_num; i < action_list_num + action_list_count; i++)
	{
        PAVec& lst				= _ps.alist_vec[i];
	    for (PAVecIt it=lst.begin(); it!=lst.end(); it++)
            xr_delete			(*it);
	    lst.clear				();
	}
}

void _pSendAction(ParticleAction *S);

PARTICLEDLL_API void pCallActionList(int action_list_num)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
	{
		// Add this call as an action to the current list.

		PACallActionList* S= dynamic_cast<PACallActionList*>(pCreateAction(PACallActionListID));
		S->action_list_num = action_list_num;
		
		_pSendAction(S);
	}
	else
	{
		// Execute the specified action list.
		PAVec* pa			= _ps.GetListPtr(action_list_num);
		
		if((pa==NULL)||pa->empty())	return; // ERROR
		
		// XXX A temporary hack.
        ParticleAction::dt	= _ps.dt;
		
		_ps.in_call_list 	= TRUE;

		_pCallActionList	(pa, _ps.peff);

		_ps.in_call_list 	= FALSE;
	}
}

PARTICLEDLL_API void pSetActionListParenting(int action_list_num, const Fmatrix& full, const Fvector& vel)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list) return; // ERROR

	// Execute the specified action list.
	PAVec* pa			= _ps.GetListPtr(action_list_num);

	if(pa == NULL)		return; // ERROR

	Fmatrix mR=full;	mR.c.set(0,0,0);
	Fmatrix mT;			mT.translate(full.c);

	// Step through all the actions in the action list.
	for(PAVecIt it=pa->begin(); it!=pa->end(); it++){
		BOOL t = (*it)->m_Flags.is(ParticleAction::ALLOW_TRANSLATE);
		BOOL r = (*it)->m_Flags.is(ParticleAction::ALLOW_ROTATE);
		if ((!t)&&(!r)) continue;
		const Fmatrix& m = t&&r?full:(t?mT:mR);
        (*it)->Transform(m);
		switch((*it)->type)
		{
		case PASourceID:
			dynamic_cast<PASource*>(*it)->parent_vel = pVector(vel.x,vel.y,vel.z)*dynamic_cast<PASource*>(*it)->parent_motion;
			break;
//.            
		}
	}
}

PARTICLEDLL_API void pStopPlaying(int action_list_num)
{
	_ParticleState &_ps = _GetPState();
	if(_ps.in_new_list) return; // ERROR
	// Execute the specified action list.
	PAVec* pa			= _ps.GetListPtr(action_list_num);
	if(pa == NULL)		return; // ERROR
	// Step through all the actions in the action list.
	for(PAVecIt it=pa->begin(); it!=pa->end(); it++){
		switch((*it)->type){
		case PASourceID:
			dynamic_cast<PASource*>(*it)->m_Flags.set(PASource::flSilent,TRUE);
			break;
		}
	}
}

PARTICLEDLL_API void pStartPlaying(int action_list_num)
{
	_ParticleState &_ps = _GetPState();
	if(_ps.in_new_list) return; // ERROR
	// Execute the specified action list.
	PAVec* pa			= _ps.GetListPtr(action_list_num);
	if(pa == NULL)		return; // ERROR
	// Step through all the actions in the action list.
	for(PAVecIt it=pa->begin(); it!=pa->end(); it++){
		switch((*it)->type){
		case PASourceID:
			dynamic_cast<PASource*>(*it)->m_Flags.set(PASource::flSilent,FALSE);
			break;
		}
	}
}

////////////////////////////////////////////////////////
// Particle Effect Calls

// Create particle effects, each with max_particles allocated.
PARTICLEDLL_API int pGenParticleEffects(int p_effect_count, int max_particles)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return -1; // ERROR

	int ind = _ps.GenerateEffects(p_effect_count);
	
	for(int i=ind; i<ind+p_effect_count; i++)
	{
		_ps.effect_vec[i] = (ParticleEffect *)xr_alloc<Particle>(max_particles + 2);
		_ps.effect_vec[i]->max_particles = max_particles;
		_ps.effect_vec[i]->particles_allocated = max_particles;
		_ps.effect_vec[i]->p_count = 0;
	}
	
	return ind;
}

PARTICLEDLL_API void pDeleteParticleEffects(int p_effect_num, int p_effect_count)
{
	if(p_effect_num < 0)
		return; // ERROR

	_ParticleState &_ps = _GetPState();

	if(p_effect_num + p_effect_count > (int)_ps.effect_vec.size())
		return; // ERROR
	
	for(int i = p_effect_num; i < p_effect_num + p_effect_count; i++)
	{
		if(_ps.effect_vec[i])	xr_free(_ps.effect_vec[i]);
		else					return; // ERROR
	}
}

// Change which effect is current.
PARTICLEDLL_API void pCurrentEffect(int p_effect_num)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return; // ERROR
	
	_ps.peff = _ps.GetEffectPtr(p_effect_num);
	if(_ps.peff)
		_ps.effect_id = p_effect_num;
	else
		_ps.effect_id = -1;
}

// Change the maximum number of particles in the current effect.
PARTICLEDLL_API int pSetMaxParticlesG(int effect_num, int max_count)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return 0; // ERROR
	
	ParticleEffect *pe = _ps.GetEffectPtr(effect_num);
	if(pe == NULL)
		return 0; // ERROR
	
	if(max_count < 0)
		return 0; // ERROR

	// Reducing max.
	if(pe->particles_allocated >= max_count)
	{
		pe->max_particles = max_count;

		// May have to kill particles.
		if(pe->p_count > pe->max_particles)
			pe->p_count = pe->max_particles;

		return max_count;
	}

	// Allocate particles.
	ParticleEffect *pe2 =(ParticleEffect *)xr_alloc<Particle>(max_count + 2);
	if(pe2 == NULL)
	{
		// Not enough memory. Just give all we've got.
		// ERROR
		pe->max_particles = pe->particles_allocated;
		
		return pe->max_particles;
	}
	
	Memory.mem_copy(pe2, pe, (pe->p_count + 2) * sizeof(Particle));
	
	xr_free(pe);
	
	_ps.effect_vec[_ps.effect_id] = _ps.peff = pe2;
	pe2->max_particles			= max_count;
	pe2->particles_allocated	= max_count;

	return max_count;
}

PARTICLEDLL_API int pSetMaxParticles(int max_count)
{
	_ParticleState &_ps = _GetPState();
	return pSetMaxParticlesG(_ps.effect_id,max_count);
}

// Copy from the specified effect to the current effect.
PARTICLEDLL_API void pCopyEffect(int p_src_effect_num, int index, int copy_count)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return; // ERROR
	
	ParticleEffect *srceff = _ps.GetEffectPtr(p_src_effect_num);
	if(srceff == NULL)
		return; // ERROR

	ParticleEffect *desteff = _ps.peff;
	if(desteff == NULL)
		return; // ERROR

	// Find out exactly how many to copy.
	int ccount = copy_count;
	if(ccount > srceff->p_count - index)
		ccount = srceff->p_count - index;
	if(ccount > desteff->max_particles - desteff->p_count)
		ccount = desteff->max_particles - desteff->p_count;

	// #pragma critical
	// cerr << p_src_effect_num << ": " << ccount << " " << srceff->p_count << " " << index << endl;

	if(ccount<0)
	  ccount = 0;

	// Directly copy the particles to the current list.
	for(int i=0; i<ccount; i++)
	{
		desteff->list[desteff->p_count+i] =
			srceff->list[index+i];
	}
	desteff->p_count += ccount;
}

// Returns the number of particles currently in the effect.
PARTICLEDLL_API int pGetEffectCount()
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return 0; // ERROR
	
	if(_ps.peff == NULL)
		return 0; // ERROR

	return _ps.peff->p_count;
}
}