// action_api.cpp
//
// Copyright 1997-1998 by David K. McAllister
//
// This file implements the action API calls by creating
// action class instances, which are either executed or
// added to an action list.
#include "stdafx.h"

#include "general.h"

extern void _pAddActionToList(ParticleAction *S, int size);
extern void _pCallActionList(ParticleAction *pa, int num_actions,
							 ParticleGroup *pg);

// Do not call this function.
void _pSendAction(ParticleAction *S, PActionEnum type, int size)
{
	_ParticleState &_ps = _GetPState();

	S->type = type;
	
	if(_ps.in_new_list)
	{
		_pAddActionToList(S, size);
	}
	else
	{
		// Immediate mode. Execute it.
		// This is a hack to give them local access to dt.
		S->dt = _ps.dt;
		_pCallActionList(S, 1, _ps.pgrp);
	}
}

PARTICLEDLL_API void __stdcall pAvoid(float magnitude, float epsilon, float look_ahead,
			 PDomainEnum dtype,
			 float a0, float a1, float a2,
			 float a3, float a4, float a5,
			 float a6, float a7, float a8, BOOL allow_transform)
{
	PAAvoid S;
	
	S.positionL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S.position		= S.positionL;
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.look_ahead	= look_ahead;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);

	_pSendAction(&S, PAAvoidID, sizeof(PAAvoid));
}

PARTICLEDLL_API void __stdcall pBounce(float friction, float resilience, float cutoff,
			 PDomainEnum dtype,
			 float a0, float a1, float a2,
			 float a3, float a4, float a5,
			 float a6, float a7, float a8, BOOL allow_transform)
{
	PABounce S;
	
	S.positionL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S.position		= S.positionL;
	S.oneMinusFriction = 1.0f - friction;
	S.resilience	= resilience;
	S.cutoffSqr		= _sqr(cutoff);
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PABounceID, sizeof(PABounce));
}

PARTICLEDLL_API void __stdcall pCopyVertexB(BOOL copy_pos, BOOL copy_vel)
{
	PACopyVertexB S;

	S.copy_pos		= copy_pos;
	S.copy_vel		= copy_vel;
	
	_pSendAction(&S, PACopyVertexBID, sizeof(PACopyVertexB));
}

PARTICLEDLL_API void __stdcall pDamping(float damping_x, float damping_y, float damping_z,
			 float vlow, float vhigh)
{
	PADamping S;
	
	S.damping		= pVector(damping_x, damping_y, damping_z);
	S.vlowSqr		= _sqr(vlow);
	S.vhighSqr		= _sqr(vhigh);
	
	_pSendAction(&S, PADampingID, sizeof(PADamping));
}

PARTICLEDLL_API void __stdcall pExplosion(float center_x, float center_y, float center_z, float velocity,
				float magnitude, float stdev, float epsilon, float age, BOOL allow_transform)
{
	PAExplosion S;

	S.centerL		= pVector(center_x, center_y, center_z);
	S.center		= S.centerL;
	S.velocity		= velocity;
	S.magnitude		= magnitude;
	S.stdev			= stdev;
	S.epsilon		= epsilon;
	S.age			= age;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	if(S.epsilon < 0.0f)
		S.epsilon = P_EPS;
	
	_pSendAction(&S, PAExplosionID, sizeof(PAExplosion));
}

PARTICLEDLL_API void __stdcall pFollow(float magnitude, float epsilon, float max_radius)
{
	PAFollow S;
	
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	
	_pSendAction(&S, PAFollowID, sizeof(PAFollow));
}

PARTICLEDLL_API void __stdcall pGravitate(float magnitude, float epsilon, float max_radius)
{
	PAGravitate S;
	
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	
	_pSendAction(&S, PAGravitateID, sizeof(PAGravitate));
}

PARTICLEDLL_API void __stdcall pGravity(float dir_x, float dir_y, float dir_z, BOOL allow_transform)
{
	PAGravity S;
	
	S.directionL	= pVector(dir_x, dir_y, dir_z);
	S.direction		= S.directionL;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PAGravityID, sizeof(PAGravity));
}

PARTICLEDLL_API void __stdcall pJet(float center_x, float center_y, float center_z,
		 float magnitude, float epsilon, float max_radius, BOOL allow_transform)
{
	_ParticleState &_ps = _GetPState();

	PAJet S;
	
	S.centerL		= pVector(center_x, center_y, center_z);
	S.center		= S.centerL;
	S.accL			= _ps.Vel;
	S.acc			= S.accL;
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PAJetID, sizeof(PAJet));
}

PARTICLEDLL_API void __stdcall pKillOld(float age_limit, BOOL kill_less_than)
{
	PAKillOld S;
	
	S.age_limit		= age_limit;
	S.kill_less_than = kill_less_than;
	
	_pSendAction(&S, PAKillOldID, sizeof(PAKillOld));
}

PARTICLEDLL_API void __stdcall pMatchVelocity(float magnitude, float epsilon, float max_radius)
{
	PAMatchVelocity S;
	
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	
	_pSendAction(&S, PAMatchVelocityID, sizeof(PAMatchVelocity));
}

PARTICLEDLL_API void __stdcall pMove()
{
	PAMove S;
	
	_pSendAction(&S, PAMoveID, sizeof(PAMove));
}

PARTICLEDLL_API void __stdcall pOrbitLine(float p_x, float p_y, float p_z,
				float axis_x, float axis_y, float axis_z,
				float magnitude, float epsilon, float max_radius, BOOL allow_transform)
{
	PAOrbitLine S;
	
	S.pL			= pVector(p_x, p_y, p_z);
	S.p				= S.pL;
	S.axisL			= pVector(axis_x, axis_y, axis_z);
	S.axisL.normalize();
	S.axis			= S.axisL;
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PAOrbitLineID, sizeof(PAOrbitLine));
}

PARTICLEDLL_API void __stdcall pOrbitPoint(float center_x, float center_y, float center_z,
				 float magnitude, float epsilon, float max_radius, BOOL allow_transform)
{
	PAOrbitPoint S;
	
	S.centerL		= pVector(center_x, center_y, center_z);
	S.center		= S.centerL;
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PAOrbitPointID, sizeof(PAOrbitPoint));
}

PARTICLEDLL_API void __stdcall pRandomAccel(PDomainEnum dtype,
				 float a0, float a1, float a2,
				 float a3, float a4, float a5,
				 float a6, float a7, float a8, BOOL allow_transform)
{
	PARandomAccel S;
	
	S.gen_accL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S.gen_acc		= S.gen_accL;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PARandomAccelID, sizeof(PARandomAccel));
}

PARTICLEDLL_API void __stdcall pRandomDisplace(PDomainEnum dtype,
					 float a0, float a1, float a2,
					 float a3, float a4, float a5,
					 float a6, float a7, float a8, BOOL allow_transform)
{
	PARandomDisplace S;
	
	S.gen_dispL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S.gen_disp		= S.gen_dispL;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PARandomDisplaceID, sizeof(PARandomDisplace));
}

PARTICLEDLL_API void __stdcall pRandomVelocity(PDomainEnum dtype,
					 float a0, float a1, float a2,
					 float a3, float a4, float a5,
					 float a6, float a7, float a8, BOOL allow_transform)
{
	PARandomVelocity S;
	
	S.gen_velL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S.gen_vel		= S.gen_velL;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PARandomVelocityID, sizeof(PARandomVelocity));
}

PARTICLEDLL_API void __stdcall pRestore(float time_left)
{
	PARestore S;
	
	S.time_left		= time_left;
	
	_pSendAction(&S, PARestoreID, sizeof(PARestore));
}

PARTICLEDLL_API void __stdcall pSink(BOOL kill_inside, PDomainEnum dtype,
		  float a0, float a1, float a2,
		  float a3, float a4, float a5,
		  float a6, float a7, float a8, BOOL allow_transform)
{
	PASink S;
	
	S.kill_inside	= kill_inside;
	S.positionL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S.position		= S.positionL;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PASinkID, sizeof(PASink));
}

PARTICLEDLL_API void __stdcall pSinkVelocity(BOOL kill_inside, PDomainEnum dtype,
				  float a0, float a1, float a2,
				  float a3, float a4, float a5,
				  float a6, float a7, float a8, BOOL allow_transform)
{
	PASinkVelocity S;
	
	S.kill_inside	= kill_inside;
	S.velocityL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S.velocity		= S.velocityL;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PASinkVelocityID, sizeof(PASinkVelocity));
}

PARTICLEDLL_API void __stdcall pSource(float particle_rate, PDomainEnum dtype,
			 float a0, float a1, float a2,
			 float a3, float a4, float a5,
			 float a6, float a7, float a8, BOOL allow_transform)
{
	_ParticleState &_ps = _GetPState();

	PASource S;
	
	S.particle_rate = particle_rate;
	S.positionL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S.position		= S.positionL;
	S.velocityL		= _ps.Vel;
	S.velocity		= S.velocityL;
	S.size			= _ps.Size;
	S.rot			= _ps.Rot;
	S.color			= _ps.Color;
	S.alpha			= _ps.Alpha;
	S.age			= _ps.Age;
	S.age_sigma		= _ps.AgeSigma;
	S.vertexB_tracks= _ps.vertexB_tracks;
	S.flags.zero	();
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	S.parent_vel	= pVector(0.5f,0,0);

	_pSendAction(&S, PASourceID, sizeof(PASource));
}

PARTICLEDLL_API void __stdcall pSpeedLimit(float min_speed, float max_speed)
{
	PASpeedLimit S;

	S.min_speed = min_speed;
	S.max_speed = max_speed;

	_pSendAction(&S, PASpeedLimitID, sizeof(PASpeedLimit));
}

PARTICLEDLL_API void __stdcall pTargetColor(float color_x, float color_y, float color_z,
				 float alpha, float scale)
{
	PATargetColor S;
	
	S.color = pVector(color_x, color_y, color_z);
	S.alpha = alpha;
	S.scale = scale;
	
	_pSendAction(&S, PATargetColorID, sizeof(PATargetColor));
}

PARTICLEDLL_API void __stdcall pTargetSize(float size_x, float size_y, float size_z,
						 float scale_x, float scale_y, float scale_z)
{
	PATargetSize S;
	
	S.size = pVector(size_x, size_y, size_z);
	S.scale = pVector(scale_x, scale_y, scale_z);
	
	_pSendAction(&S, PATargetSizeID, sizeof(PATargetSize));
}

PARTICLEDLL_API void __stdcall pTargetRotate(float rot_x, float rot_y, float rot_z, float scale)
{
	PATargetRotate S;

	S.rot = pVector(rot_x, rot_y, rot_z);
	S.scale = scale;

	_pSendAction(&S, PATargetRotateID, sizeof(PATargetRotate));
}

PARTICLEDLL_API void __stdcall pTargetRotateD(float scale, PDomainEnum dtype,
											  float a0, float a1, float a2,
											  float a3, float a4, float a5,
											  float a6, float a7, float a8)
{ 
	// generate random target value
	pDomain Domain = pDomain(dtype,a0,a1,a2,a3,a4,a5,a6,a7,a8);
	pVector r;
	Domain.Generate(r);

	PATargetRotate S;

	S.rot = pVector(r.x, r.y, r.z);
	S.scale = scale;

	_pSendAction(&S, PATargetRotateDID, sizeof(PATargetRotate));
}

PARTICLEDLL_API void __stdcall pTargetVelocity(float vel_x, float vel_y, float vel_z, float scale, BOOL allow_transform)
{
	PATargetVelocity S;
	
	S.velocityL		= pVector(vel_x, vel_y, vel_z);
	S.velocity		= S.velocityL;
	S.scale			= scale;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PATargetVelocityID, sizeof(PATargetVelocity));
}

PARTICLEDLL_API void __stdcall pTargetVelocityD(float scale, PDomainEnum dtype,
												float a0, float a1, float a2,
												float a3, float a4, float a5,
												float a6, float a7, float a8, BOOL allow_transform)
{
	// generate random target velocity
	pVector v;
	pDomain Domain	= pDomain(dtype,a0,a1,a2,a3,a4,a5,a6,a7,a8);
	Domain.Generate(v);

	PATargetVelocity S;

	S.velocityL		= pVector(v.x, v.y, v.z);
	S.velocity		= S.velocityL;
	S.scale			= scale;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);

	_pSendAction(&S, PATargetVelocityDID, sizeof(PATargetVelocity));
}

// If in immediate mode, quickly add a vertex.
// If building an action list, call pSource.
PARTICLEDLL_API void __stdcall pVertex(float x, float y, float z)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
	{
		pSource(1, PDPoint, x, y, z);
		return;
	}
	
	// Immediate mode. Quickly add the vertex.
	if(_ps.pgrp == NULL)
		return;
	
	pVector pos(x, y, z);
	pVector siz, rot, vel, col, posB;
	if(_ps.vertexB_tracks)
		posB = pos;
	else
		_ps.VertexB.Generate(posB);
	_ps.Size.Generate(siz);
	_ps.Rot.Generate(rot);
	_ps.Vel.Generate(vel);
	_ps.Color.Generate(col);
	_ps.pgrp->Add(pos, posB, siz, rot, vel, col, _ps.Alpha, _ps.Age);
}

PARTICLEDLL_API void __stdcall pVortex(float center_x, float center_y, float center_z,
			float axis_x, float axis_y, float axis_z,
			float magnitude, float epsilon, float max_radius, BOOL allow_transform)
{
	PAVortex S;
	
	S.centerL		= pVector(center_x, center_y, center_z);
	S.center		= S.centerL;
	S.axisL			= pVector(axis_x, axis_y, axis_z);
	S.axisL.normalize();
	S.axis			= S.axisL;
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	S.flags.set		(ParticleAction::ALLOW_TRANSFORM,allow_transform);
	
	_pSendAction(&S, PAVortexID, sizeof(PAVortex));
}
