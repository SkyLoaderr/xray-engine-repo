#include "stdafx.h"

namespace PAPI{

extern void 			_pAddActionToList	(ParticleAction *S);
extern void 			_pCallActionList	(PAVec* pa, ParticleEffect *pe);

// Do not call this function.
void _pSendAction(ParticleAction *S)
{
	_ParticleState &_ps = _GetPState();

	VERIFY(_ps.in_new_list);

	_pAddActionToList	(S);
}

PARTICLEDLL_API void pAvoid(float magnitude, float epsilon, float look_ahead,
			 PDomainEnum dtype,
			 float a0, float a1, float a2,
			 float a3, float a4, float a5,
			 float a6, float a7, float a8, BOOL allow_translate, BOOL allow_rotate)
{
	PAAvoid* S		= dynamic_cast<PAAvoid*>(pCreateAction(PAAvoidID));
	
	S->positionL	= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S->position		= S->positionL;
	S->magnitude	= magnitude;
	S->epsilon		= epsilon;
	S->look_ahead	= look_ahead;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);

	_pSendAction	(S);
}

PARTICLEDLL_API void pBounce(float friction, float resilience, float cutoff,
			 PDomainEnum dtype,
			 float a0, float a1, float a2,
			 float a3, float a4, float a5,
			 float a6, float a7, float a8, BOOL allow_translate, BOOL allow_rotate)
{
	PABounce* S		= dynamic_cast<PABounce*>(pCreateAction(PABounceID));
	
	S->positionL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S->position		= S->positionL;
	S->oneMinusFriction = 1.0f - friction;
	S->resilience	= resilience;
	S->cutoffSqr		= _sqr(cutoff);
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pCopyVertexB(BOOL copy_pos)
{
	PACopyVertexB* S= dynamic_cast<PACopyVertexB*>(pCreateAction(PACopyVertexBID));

	S->copy_pos		= copy_pos;
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pDamping(float damping_x, float damping_y, float damping_z,
			 float vlow, float vhigh)
{
	PADamping* S	= dynamic_cast<PADamping*>(pCreateAction(PADampingID));
	
	S->damping		= pVector(damping_x, damping_y, damping_z);
	S->vlowSqr		= _sqr(vlow);
	S->vhighSqr		= _sqr(vhigh);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pExplosion(float center_x, float center_y, float center_z, float velocity,
				float magnitude, float stdev, float epsilon, float age, BOOL allow_translate, BOOL allow_rotate)
{
	PAExplosion* S	= dynamic_cast<PAExplosion*>(pCreateAction(PAExplosionID));

	S->centerL		= pVector(center_x, center_y, center_z);
	S->center		= S->centerL;
	S->velocity		= velocity;
	S->magnitude		= magnitude;
	S->stdev			= stdev;
	S->epsilon		= epsilon;
	S->age			= age;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	if(S->epsilon < 0.0f)
		S->epsilon = P_EPS;
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pFollow(float magnitude, float epsilon, float max_radius)
{
	PAFollow* S		= dynamic_cast<PAFollow*>(pCreateAction(PAFollowID));
	
	S->magnitude		= magnitude;
	S->epsilon		= epsilon;
	S->max_radius	= max_radius;
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pGravitate(float magnitude, float epsilon, float max_radius)
{
	PAGravitate* S	= dynamic_cast<PAGravitate*>(pCreateAction(PAGravitateID));
	
	S->magnitude		= magnitude;
	S->epsilon		= epsilon;
	S->max_radius	= max_radius;
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pGravity(float dir_x, float dir_y, float dir_z, BOOL allow_translate, BOOL allow_rotate)
{
	PAGravity* S	= dynamic_cast<PAGravity*>(pCreateAction(PAGravityID));
	
	S->directionL	= pVector(dir_x, dir_y, dir_z);
	S->direction		= S->directionL;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);

	_pSendAction	(S);
}

PARTICLEDLL_API void pJet(float center_x, float center_y, float center_z,
		 float magnitude, float epsilon, float max_radius, BOOL allow_translate, BOOL allow_rotate)
{
	_ParticleState &_ps = _GetPState();

	PAJet* S		= dynamic_cast<PAJet*>(pCreateAction(PAJetID));
	
	S->centerL		= pVector(center_x, center_y, center_z);
	S->center		= S->centerL;
	S->accL			= _ps.Vel;
	S->acc			= S->accL;
	S->magnitude		= magnitude;
	S->epsilon		= epsilon;
	S->max_radius	= max_radius;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pKillOld(float age_limit, BOOL kill_less_than)
{
	PAKillOld* S	= dynamic_cast<PAKillOld*>(pCreateAction(PAKillOldID));
	
	S->age_limit		= age_limit;
	S->kill_less_than = kill_less_than;
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pMatchVelocity(float magnitude, float epsilon, float max_radius)
{
	PAMatchVelocity* S	= dynamic_cast<PAMatchVelocity*>(pCreateAction(PAMatchVelocityID));
	
	S->magnitude		= magnitude;
	S->epsilon		= epsilon;
	S->max_radius	= max_radius;
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pMove()
{
	PAMove* S		= dynamic_cast<PAMove*>(pCreateAction(PAMoveID));
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pOrbitLine(float p_x, float p_y, float p_z,
				float axis_x, float axis_y, float axis_z,
				float magnitude, float epsilon, float max_radius, BOOL allow_translate, BOOL allow_rotate)
{
	PAOrbitLine* S	= dynamic_cast<PAOrbitLine*>(pCreateAction(PAOrbitLineID));
	
	S->pL			= pVector(p_x, p_y, p_z);
	S->p			= S->pL;
	S->axisL		= pVector(axis_x, axis_y, axis_z);
	S->axisL.normalize();
	S->axis			= S->axisL;
	S->magnitude	= magnitude;
	S->epsilon		= epsilon;
	S->max_radius	= max_radius;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pOrbitPoint(float center_x, float center_y, float center_z,
				 float magnitude, float epsilon, float max_radius, BOOL allow_translate, BOOL allow_rotate)
{
	PAOrbitPoint* S	= dynamic_cast<PAOrbitPoint*>(pCreateAction(PAOrbitPointID));
	
	S->centerL		= pVector(center_x, center_y, center_z);
	S->center		= S->centerL;
	S->magnitude		= magnitude;
	S->epsilon		= epsilon;
	S->max_radius	= max_radius;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pRandomAccel(PDomainEnum dtype,
				 float a0, float a1, float a2,
				 float a3, float a4, float a5,
				 float a6, float a7, float a8, BOOL allow_translate, BOOL allow_rotate)
{
	PARandomAccel* S= dynamic_cast<PARandomAccel*>(pCreateAction(PARandomAccelID));
	
	S->gen_accL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S->gen_acc		= S->gen_accL;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pRandomDisplace(PDomainEnum dtype,
					 float a0, float a1, float a2,
					 float a3, float a4, float a5,
					 float a6, float a7, float a8, BOOL allow_translate, BOOL allow_rotate)
{
	PARandomDisplace* S	= dynamic_cast<PARandomDisplace*>(pCreateAction(PARandomDisplaceID));
	
	S->gen_dispL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S->gen_disp		= S->gen_dispL;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pRandomVelocity(PDomainEnum dtype,
					 float a0, float a1, float a2,
					 float a3, float a4, float a5,
					 float a6, float a7, float a8, BOOL allow_translate, BOOL allow_rotate)
{
	PARandomVelocity* S	= dynamic_cast<PARandomVelocity*>(pCreateAction(PARandomVelocityID));
	
	S->gen_velL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S->gen_vel		= S->gen_velL;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pRestore(float time_left)
{
	PARestore* S	= dynamic_cast<PARestore*>(pCreateAction(PARestoreID));
	
	S->time_left		= time_left;
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pSink(BOOL kill_inside, PDomainEnum dtype,
		  float a0, float a1, float a2,
		  float a3, float a4, float a5,
		  float a6, float a7, float a8, BOOL allow_translate, BOOL allow_rotate)
{
	PASink* S		= dynamic_cast<PASink*>(pCreateAction(PASinkID));
	
	S->kill_inside	= kill_inside;
	S->positionL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S->position		= S->positionL;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pSinkVelocity(BOOL kill_inside, PDomainEnum dtype,
				  float a0, float a1, float a2,
				  float a3, float a4, float a5,
				  float a6, float a7, float a8, BOOL allow_translate, BOOL allow_rotate)
{
	PASinkVelocity* S= dynamic_cast<PASinkVelocity*>(pCreateAction(PASinkVelocityID));
	
	S->kill_inside	= kill_inside;
	S->velocityL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S->velocity		= S->velocityL;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pSource(float particle_rate, PDomainEnum dtype,
			 float a0, float a1, float a2,
			 float a3, float a4, float a5,
			 float a6, float a7, float a8, BOOL allow_translate, BOOL allow_rotate)
{
	_ParticleState &_ps = _GetPState();

	PASource* S		= dynamic_cast<PASource*>(pCreateAction(PASourceID));
	
	S->particle_rate = particle_rate;
	S->positionL		= pDomain(dtype, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	S->position		= S->positionL;
	S->velocityL		= _ps.Vel;
	S->velocity		= S->velocityL;
	S->size			= _ps.Size;
	S->rot			= _ps.Rot;
	S->color			= _ps.Color;
	S->alpha			= _ps.Alpha;
	S->age			= _ps.Age;
	S->age_sigma		= _ps.AgeSigma;
	S->m_Flags		= _ps.flags;
	S->parent_vel	= pVector(0,0,0);
	S->parent_motion	= _ps.parent_motion;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);

	_pSendAction	(S);
}

PARTICLEDLL_API void pSpeedLimit(float min_speed, float max_speed)
{
	PASpeedLimit* S	= dynamic_cast<PASpeedLimit*>(pCreateAction(PASpeedLimitID));

	S->min_speed = min_speed;
	S->max_speed = max_speed;

	_pSendAction	(S);
}

PARTICLEDLL_API void pTargetColor(float color_x, float color_y, float color_z,
				 float alpha, float scale)
{
	PATargetColor* S	= dynamic_cast<PATargetColor*>(pCreateAction(PATargetColorID));
	
	S->color = pVector(color_x, color_y, color_z);
	S->alpha = alpha;
	S->scale = scale;
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pTargetSize(float size_x, float size_y, float size_z,
						 float scale_x, float scale_y, float scale_z)
{
	PATargetSize* S	= dynamic_cast<PATargetSize*>(pCreateAction(PATargetSizeID));
	
	S->size = pVector(size_x, size_y, size_z);
	S->scale = pVector(scale_x, scale_y, scale_z);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pTargetRotate(float rot_x, float rot_y, float rot_z, float scale)
{
	PATargetRotate* S	= dynamic_cast<PATargetRotate*>(pCreateAction(PATargetRotateID));

	S->rot = pVector(rot_x, rot_y, rot_z);
	S->scale = scale;

	_pSendAction	(S);
}

PARTICLEDLL_API void pTargetRotateD(float scale, PDomainEnum dtype,
											  float a0, float a1, float a2,
											  float a3, float a4, float a5,
											  float a6, float a7, float a8)
{ 
	// generate random target value
	pDomain Domain = pDomain(dtype,a0,a1,a2,a3,a4,a5,a6,a7,a8);
	pVector r;
	Domain.Generate(r);

	PATargetRotate* S= dynamic_cast<PATargetRotate*>(pCreateAction(PATargetRotateID));

	S->rot = pVector(r.x, r.y, r.z);
	S->scale = scale;

	_pSendAction	(S);
}

PARTICLEDLL_API void pTargetVelocity(float vel_x, float vel_y, float vel_z, float scale, BOOL allow_translate, BOOL allow_rotate)
{
	PATargetVelocity* S	= dynamic_cast<PATargetVelocity*>(pCreateAction(PATargetVelocityID));
	
	S->velocityL		= pVector(vel_x, vel_y, vel_z);
	S->velocity		= S->velocityL;
	S->scale			= scale;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}

PARTICLEDLL_API void pTargetVelocityD(float scale, PDomainEnum dtype,
												float a0, float a1, float a2,
												float a3, float a4, float a5,
												float a6, float a7, float a8, BOOL allow_translate, BOOL allow_rotate)
{
	// generate random target velocity
	pVector v;
	pDomain Domain	= pDomain(dtype,a0,a1,a2,a3,a4,a5,a6,a7,a8);
	Domain.Generate(v);

	PATargetVelocity* S	= dynamic_cast<PATargetVelocity*>(pCreateAction(PATargetVelocityID));

	S->velocityL		= pVector(v.x, v.y, v.z);
	S->velocity		= S->velocityL;
	S->scale			= scale;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);

	_pSendAction	(S);
}

// If in immediate mode, quickly add a vertex.
// If building an action list, call pSource.
PARTICLEDLL_API void pVertex(float x, float y, float z)
{
	_ParticleState &_ps = _GetPState();

	if(_ps.in_new_list)
	{
		pSource(1, PDPoint, x, y, z);
		return;
	}
	
	// Immediate mode. Quickly add the vertex.
	if(_ps.peff == NULL)
		return;
	
	pVector pos(x, y, z);
	pVector siz, rot, vel, col, posB;
	if(_ps.flags.is(u32(PASource::flVertexB_tracks)))
		posB = pos;
	else
		_ps.VertexB.Generate(posB);
	_ps.Size.Generate(siz);
	_ps.Rot.Generate(rot);
	_ps.Vel.Generate(vel);
	_ps.Color.Generate(col);
	_ps.peff->Add(pos, posB, siz, rot, vel, color_argb_f(_ps.Alpha, col.x, col.y, col.z), _ps.Age);
}

PARTICLEDLL_API void pVortex(float center_x, float center_y, float center_z,
			float axis_x, float axis_y, float axis_z,
			float magnitude, float epsilon, float max_radius, BOOL allow_translate, BOOL allow_rotate)
{
	PAVortex* S		= dynamic_cast<PAVortex*>(pCreateAction(PAVortexID));
	
	S->centerL		= pVector(center_x, center_y, center_z);
	S->center		= S->centerL;
	S->axisL			= pVector(axis_x, axis_y, axis_z);
	S->axisL.normalize();
	S->axis			= S->axisL;
	S->magnitude		= magnitude;
	S->epsilon		= epsilon;
	S->max_radius	= max_radius;
	S->m_Flags.set	(ParticleAction::ALLOW_TRANSLATE,allow_translate);
	S->m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	_pSendAction	(S);
}
}
