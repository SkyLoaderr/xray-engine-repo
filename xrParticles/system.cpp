// system.cpp
//
// Copyright 1998 by David K. McAllister.
//
// This file implements the API calls that are not particle actions.
#include "stdafx.h"
#pragma hdrstop

// For Windows DLL.
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: 
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

namespace PAPI{
float ParticleAction::dt;

_ParticleState::ParticleGroupVec	_ParticleState::group_vec;
_ParticleState::PAHeaderVec			_ParticleState::alist_vec;

// This AutoCall struct allows for static initialization of the above shared variables.
struct AutoCall
{
	AutoCall();
};

AutoCall::AutoCall()
{
	// The list of groups, etc.	
	/*
	_ParticleState::group_vec.reserve(128);
	_ParticleState::alist_vec.reserve(128);
	*/
}
static AutoCall AC;

// This is the global state.
_ParticleState __ps;
_ParticleState& _GetPState()
{
	// This is the global state.
//	extern _ParticleState __ps;

	return __ps;
}

// Get a pointer to the particles in gp memory
ParticleGroup* _GetGroupPtr(int p_group_num)
{
	return __ps.GetGroupPtr(p_group_num);
}
PAHeader* _GetListPtr(int action_list_num)
{
	return __ps.GetListPtr(action_list_num);
}

_ParticleState::_ParticleState()
{
	in_call_list	= FALSE;
	in_new_list		= FALSE;
	
	dt				= 1.0f;
	
	group_id		= -1;
	list_id			= -1;
	pgrp			= NULL;
	pact			= NULL;
	tid				= 0; // This will be filled in above if we're MP.

	ResetState		();
}

void _ParticleState::ResetState()
{
	flags.set		((u8)PASource::flVertexB_tracks,TRUE);
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

ParticleGroup *_ParticleState::GetGroupPtr(int p_group_num)
{
	R_ASSERT(p_group_num>=0&&p_group_num<(int)group_vec.size());
	return group_vec[p_group_num];
}

PAHeader *_ParticleState::GetListPtr(int a_list_num)
{
	R_ASSERT(a_list_num>=0&&a_list_num<(int)alist_vec.size());
	return alist_vec[a_list_num];
}

// Return an index into the list of particle groups where
// p_group_count groups can be added.
int _ParticleState::GenerateGroups(int p_group_count)
{
	int num_empty = 0;
	int first_empty = -1;
	
	for(int i=0; i<(int)group_vec.size(); i++){
		if(group_vec[i]){
			num_empty = 0;
			first_empty = -1;
		}else{
			if(first_empty < 0)
				first_empty = i;
			num_empty++;
			if(num_empty >= p_group_count)
				return first_empty;
		}
	}
	
	// Couldn't find a big enough gap. Reallocate.
	first_empty = group_vec.size();
	group_vec.push_back	(xr_alloc<ParticleGroup>(p_group_count));
	
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
		if(alist_vec[i])
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
	alist_vec.push_back(xr_alloc<PAHeader>(list_count));
	
	return first_empty;
}

////////////////////////////////////////////////////////
// Auxiliary calls
void _pCallActionList(ParticleAction *apa, int num_actions,
					  ParticleGroup *pg)
{
	// All these require a particle group, so check for it.
	if(pg == NULL)
		return;
	
	PAHeader *pa = (PAHeader *)apa;
	
	// Step through all the actions in the action list.
	for(int action = 0; action < num_actions; action++, pa++)
	{
		switch(pa->type)
		{
		case PAAvoidID:
			((PAAvoid *)pa)->Execute(pg);
			break;
		case PABounceID:
			((PABounce *)pa)->Execute(pg);
			break;
		case PACallActionListID:
			((PACallActionList *)pa)->Execute(pg);
			break;
		case PACopyVertexBID:
			((PACopyVertexB *)pa)->Execute(pg);
			break;
		case PADampingID:
			((PADamping *)pa)->Execute(pg);
			break;
		case PAExplosionID:
			((PAExplosion *)pa)->Execute(pg);
			break;
		case PAFollowID:
			((PAFollow *)pa)->Execute(pg);
			break;
		case PAGravitateID:
			((PAGravitate *)pa)->Execute(pg);
			break;
		case PAGravityID:
			((PAGravity *)pa)->Execute(pg);
			break;
		case PAJetID:
			((PAJet *)pa)->Execute(pg);
			break;
		case PAKillOldID:
			((PAKillOld *)pa)->Execute(pg);
			break;
		case PAMatchVelocityID:
			((PAMatchVelocity *)pa)->Execute(pg);
			break;
		case PAMoveID:
			((PAMove *)pa)->Execute(pg);
			break;
		case PAOrbitLineID:
			((PAOrbitLine *)pa)->Execute(pg);
			break;
		case PAOrbitPointID:
			((PAOrbitPoint *)pa)->Execute(pg);
			break;
		case PARandomAccelID:
			((PARandomAccel *)pa)->Execute(pg);
			break;
		case PARandomDisplaceID:
			((PARandomDisplace *)pa)->Execute(pg);
			break;
		case PARandomVelocityID:
			((PARandomVelocity *)pa)->Execute(pg);
			break;
		case PARestoreID:
			((PARestore *)pa)->Execute(pg);
			break;
		case PASinkID:
			((PASink *)pa)->Execute(pg);
			break;
		case PASinkVelocityID:
			((PASinkVelocity *)pa)->Execute(pg);
			break;
		case PASourceID:
			((PASource *)pa)->Execute(pg);
			break;
		case PASpeedLimitID:
			((PASpeedLimit *)pa)->Execute(pg);
			break;
		case PATargetColorID:
			((PATargetColor *)pa)->Execute(pg);
			break;
		case PATargetSizeID:
			((PATargetSize *)pa)->Execute(pg);
			break;
		case PATargetRotateID:
			((PATargetRotate *)pa)->Execute(pg);
			break;
		case PATargetRotateDID:
			((PATargetRotate *)pa)->Execute(pg);
			break;
		case PATargetVelocityID:
			((PATargetVelocity *)pa)->Execute(pg);
			break;
		case PATargetVelocityDID:
			((PATargetVelocity *)pa)->Execute(pg);
			break;
		case PAVortexID:
			((PAVortex *)pa)->Execute(pg);
			break;
		}
	}
}

// Add the incoming action to the end of the current action list.
void _pAddActionToList(ParticleAction *S, int size)
{
	_ParticleState &_ps = _GetPState();

	if(!_ps.in_new_list)
		return; // ERROR
	
	if(_ps.pact == NULL)
		return; // ERROR
	
	if(_ps.list_id < 0)
		return; // ERROR
	
	PAHeader *alist = _ps.pact;
	
	if(alist->actions_allocated <= alist->count)
	{
		// Must reallocate.
		int new_alloc = 16 + alist->actions_allocated;
		PAHeader *new_alist = xr_alloc<PAHeader>(new_alloc);
		Memory.mem_copy(new_alist, alist, alist->count * sizeof(PAHeader));
		
		xr_free(alist);
		_ps.alist_vec[_ps.list_id] = _ps.pact = alist = new_alist;
		
		alist->actions_allocated = new_alloc;
	}
	
	// Now add it in.
	Memory.mem_copy(&alist[alist->count], S, size);
	alist->count++;
}

PARTICLEDLL_API void pAddActionToList(PAHeader *S)
{
	_pAddActionToList(S,sizeof(PAHeader));
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

	_ps.flags.set((u8)PASource::flVertexB_tracks,trackVertex);
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
	{
		_ps.alist_vec[i] 					= xr_alloc<PAHeader>(8);
		_ps.alist_vec[i]->actions_allocated = 8;
		_ps.alist_vec[i]->type				= PAHeaderID;
		_ps.alist_vec[i]->count				= 1;
	}

	return ind;
}

PARTICLEDLL_API void pNewActionList(int action_list_num)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return; // ERROR
	
	_ps.pact = _ps.GetListPtr(action_list_num);
	
	if(_ps.pact == NULL)
		return; // ERROR
	
	_ps.list_id = action_list_num;
	_ps.in_new_list = TRUE;

	// Remove whatever used to be in the list.
	_ps.pact->count = 1;
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
		if(_ps.alist_vec[i])	xr_free(_ps.alist_vec[i]);
		else					return; // ERROR
	}
}

void _pSendAction(ParticleAction *S, PActionEnum type, int size);

PARTICLEDLL_API void pCallActionList(int action_list_num)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
	{
		// Add this call as an action to the current list.

		PACallActionList S;
		S.action_list_num = action_list_num;
		
		_pSendAction(&S, PACallActionListID, sizeof(PACallActionList));
	}
	else
	{
		// Execute the specified action list.
		PAHeader *pa = _ps.GetListPtr(action_list_num);
		
		if(pa == NULL)
			return; // ERRROR
		
		// XXX A temporary hack.
		pa->dt = _ps.dt;
		
		_ps.in_call_list = TRUE;
		
		_pCallActionList(pa+1, pa->count-1, _ps.pgrp);
		
		_ps.in_call_list = FALSE;
	}
}

PARTICLEDLL_API void pSetActionListParenting(int action_list_num, const Fmatrix& full, const Fvector& vel)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return; // ERROR

	// Execute the specified action list.
	PAHeader *pa	= _ps.GetListPtr(action_list_num);

	if(pa == NULL)
		return; // ERROR

	int num_act = pa->count-1; pa++;

	Fmatrix mR=full;	mR.c.set(0,0,0);
	Fmatrix mT;			mT.translate(full.c);

	// Step through all the actions in the action list.
	for(int act = 0; act < num_act; act++, pa++){
		BOOL t = pa->m_Flags.is(ParticleAction::ALLOW_TRANSLATE);
		BOOL r = pa->m_Flags.is(ParticleAction::ALLOW_ROTATE);
		if ((!t)&&(!r)) continue;
		const Fmatrix& m = t&&r?full:(t?mT:mR);
		switch(pa->type)
		{
		case PAAvoidID:
			((PAAvoid *)pa)->Transform(m);
			break;
		case PABounceID:
			((PABounce *)pa)->Transform(m);
			break;
		case PACallActionListID:			break;
		case PACopyVertexBID:				break;
		case PADampingID:					break;
		case PAExplosionID:
			((PAExplosion *)pa)->Transform(m);
			break;
		case PAFollowID:					break;
		case PAGravitateID:					break;
		case PAGravityID:					break;
		case PAJetID:
			((PAJet *)pa)->Transform(m);
			break;
		case PAKillOldID:					break;
		case PAMatchVelocityID:				break;
		case PAMoveID:						break;
		case PAOrbitLineID:
			((PAOrbitLine *)pa)->Transform(m);
			break;
		case PAOrbitPointID:
			((PAOrbitPoint *)pa)->Transform(m);
			break;
		case PARandomAccelID:
			((PARandomAccel *)pa)->Transform(m);
			break;
		case PARandomDisplaceID:
			((PARandomDisplace *)pa)->Transform(m);
			break;
		case PARandomVelocityID:
			((PARandomVelocity *)pa)->Transform(m);
			break;
		case PARestoreID:					break;
		case PASinkID:
			((PASink *)pa)->Transform(m);
			break;
		case PASinkVelocityID:
			((PASinkVelocity *)pa)->Transform(m);
			break;
		case PASourceID:
			((PASource *)pa)->Transform(m);
			((PASource *)pa)->parent_vel = pVector(vel.x,vel.y,vel.z)*((PASource *)pa)->parent_motion;
			break;
		case PASpeedLimitID:			break;
		case PATargetColorID:			break;
		case PATargetSizeID:			break;
		case PATargetRotateID:			break;
		case PATargetRotateDID:			break;
		case PATargetVelocityID:
			((PATargetVelocity *)pa)->Transform(m);
			break;
		case PATargetVelocityDID:
			((PATargetVelocity *)pa)->Transform(m);
			break;
		case PAVortexID:
			((PAVortex *)pa)->Transform(m);
			break;
		}
	}
}

PARTICLEDLL_API void pStopPlaying(int action_list_num)
{
	_ParticleState &_ps = _GetPState();
	if(_ps.in_new_list) return; // ERROR
	// Execute the specified action list.
	PAHeader *pa		= _ps.GetListPtr(action_list_num);
	if(pa == NULL)		return; // ERROR
	int num_act = pa->count-1; pa++;
	// Step through all the actions in the action list.
	for(int act = 0; act < num_act; act++, pa++){
		switch(pa->type){
		case PASourceID:
			((PASource *)pa)->m_Flags.set(PASource::flSilent,TRUE);
			break;
		}
	}
}

PARTICLEDLL_API void pStartPlaying(int action_list_num)
{
	_ParticleState &_ps = _GetPState();
	if(_ps.in_new_list) return; // ERROR
	// Execute the specified action list.
	PAHeader *pa		= _ps.GetListPtr(action_list_num);
	if(pa == NULL)		return; // ERROR
	int num_act = pa->count-1; pa++;
	// Step through all the actions in the action list.
	for(int act = 0; act < num_act; act++, pa++){
		switch(pa->type){
		case PASourceID:
			((PASource *)pa)->m_Flags.set(PASource::flSilent,FALSE);
			break;
		}
	}
}

////////////////////////////////////////////////////////
// Particle Group Calls

// Create particle groups, each with max_particles allocated.
PARTICLEDLL_API int pGenParticleGroups(int p_group_count, int max_particles)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return -1; // ERROR

	int ind = _ps.GenerateGroups(p_group_count);
	
	for(int i=ind; i<ind+p_group_count; i++)
	{
		_ps.group_vec[i] = (ParticleGroup *)xr_alloc<Particle>(max_particles + 2);
		_ps.group_vec[i]->max_particles = max_particles;
		_ps.group_vec[i]->particles_allocated = max_particles;
		_ps.group_vec[i]->p_count = 0;
	}
	
	return ind;
}

PARTICLEDLL_API void pDeleteParticleGroups(int p_group_num, int p_group_count)
{
	if(p_group_num < 0)
		return; // ERROR

	_ParticleState &_ps = _GetPState();

	if(p_group_num + p_group_count > (int)_ps.group_vec.size())
		return; // ERROR
	
	for(int i = p_group_num; i < p_group_num + p_group_count; i++)
	{
		if(_ps.group_vec[i])	xr_free(_ps.group_vec[i]);
		else					return; // ERROR
	}
}

// Change which group is current.
PARTICLEDLL_API void pCurrentGroup(int p_group_num)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return; // ERROR
	
	_ps.pgrp = _ps.GetGroupPtr(p_group_num);
	if(_ps.pgrp)
		_ps.group_id = p_group_num;
	else
		_ps.group_id = -1;
}

// Change the maximum number of particles in the current group.
PARTICLEDLL_API int pSetMaxParticlesG(int group_num, int max_count)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return 0; // ERROR
	
	ParticleGroup *pg = _ps.GetGroupPtr(group_num);
	if(pg == NULL)
		return 0; // ERROR
	
	if(max_count < 0)
		return 0; // ERROR

	// Reducing max.
	if(pg->particles_allocated >= max_count)
	{
		pg->max_particles = max_count;

		// May have to kill particles.
		if(pg->p_count > pg->max_particles)
			pg->p_count = pg->max_particles;

		return max_count;
	}

	// Allocate particles.
	ParticleGroup *pg2 =(ParticleGroup *)xr_alloc<Particle>(max_count + 2);
	if(pg2 == NULL)
	{
		// Not enough memory. Just give all we've got.
		// ERROR
		pg->max_particles = pg->particles_allocated;
		
		return pg->max_particles;
	}
	
	Memory.mem_copy(pg2, pg, (pg->p_count + 2) * sizeof(Particle));
	
	xr_free(pg);
	
	_ps.group_vec[_ps.group_id] = _ps.pgrp = pg2;
	pg2->max_particles			= max_count;
	pg2->particles_allocated	= max_count;

	return max_count;
}

PARTICLEDLL_API int pSetMaxParticles(int max_count)
{
	_ParticleState &_ps = _GetPState();
	return pSetMaxParticlesG(_ps.group_id,max_count);
}

// Copy from the specified group to the current group.
PARTICLEDLL_API void pCopyGroup(int p_src_group_num, int index, int copy_count)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return; // ERROR
	
	ParticleGroup *srcgrp = _ps.GetGroupPtr(p_src_group_num);
	if(srcgrp == NULL)
		return; // ERROR

	ParticleGroup *destgrp = _ps.pgrp;
	if(destgrp == NULL)
		return; // ERROR

	// Find out exactly how many to copy.
	int ccount = copy_count;
	if(ccount > srcgrp->p_count - index)
		ccount = srcgrp->p_count - index;
	if(ccount > destgrp->max_particles - destgrp->p_count)
		ccount = destgrp->max_particles - destgrp->p_count;

	// #pragma critical
	// cerr << p_src_group_num << ": " << ccount << " " << srcgrp->p_count << " " << index << endl;

	if(ccount<0)
	  ccount = 0;

	// Directly copy the particles to the current list.
	for(int i=0; i<ccount; i++)
	{
		destgrp->list[destgrp->p_count+i] =
			srcgrp->list[index+i];
	}
	destgrp->p_count += ccount;
}

// Copy from the current group to application memory.
PARTICLEDLL_API int pGetParticles(int index, int count, float *verts,
				  float *color, float *vel, float *size, float *age)
{
	_ParticleState &_ps = _GetPState();

	// XXX I should think about whether color means color3, color4, or what.
	// For now, it means color4.

	if(_ps.in_new_list)
		return -1; // ERROR
		
	ParticleGroup *pg = _ps.pgrp;
	if(pg == NULL)
		return -2; // ERROR

	if(index < 0 || count < 0)
		return -3; // ERROR

	if(index + count > pg->p_count)
	  {
	    count = pg->p_count - index;
	    if(count <= 0)
	      return -4; // ERROR index out of bounds.
	  }

	int vi = 0, ci = 0, li = 0, si = 0, ai = 0;

	// This could be optimized.
	for(int i=0; i<count; i++)
	{
		Particle &m = pg->list[index + i];

		if(verts)
		{
			verts[vi++] = m.pos.x;
			verts[vi++] = m.pos.y;
			verts[vi++] = m.pos.z;
		}

		if(color)
		{
			color[ci++] = m.color.x;
			color[ci++] = m.color.y;
			color[ci++] = m.color.z;
			color[ci++] = m.alpha;
		}

		if(vel)
		{
			vel[li++] = m.vel.x;
			vel[li++] = m.vel.y;
			vel[li++] = m.vel.z;
		}

		if(size)
		{
			size[si++] = m.size.x;
			size[si++] = m.size.y;
			size[si++] = m.size.z;
		}

		if(age)
		{
			age[ai++] = m.age;
		}
	}

	return count;
}

// Returns the number of particles currently in the group.
PARTICLEDLL_API int pGetGroupCount()
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
		return 0; // ERROR
	
	if(_ps.pgrp == NULL)
		return 0; // ERROR

	return _ps.pgrp->p_count;
}
}