//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffectActions.h"
#include "PropertiesListHelper.h"

void PAPI::pDomain::FillProp	(PropItemVec& items, LPCSTR pref)
{
	
}
//---------------------------------------------------------------------------

void 	EPAAvoid::FillProp		(PropItemVec& items, LPCSTR pref)
{
	position.FillProp	(items,pref);
/*	

    PHelper.CreateFloat	(items,FHelper.PrepareKey(pref,"Look ahead"),);
    look_ahead		= F.r_float();
    magnitude		= F.r_float();
    epsilon			= F.r_float();
*/
}

void 	EPABounce::FillProp		(PropItemVec& items, LPCSTR pref)
{
/*
    F.r				(&positionL,sizeof(PAPI::pDomain));
    F.r				(&position,sizeof(PAPI::pDomain));
    oneMinusFriction= F.r_float();
    resilience		= F.r_float();
    cutoffSqr		= F.r_float();
*/
}

void 	EPACallActionList::FillProp(PropItemVec& items, LPCSTR pref)
{
/*	
    action_list_num	= F.r_s32	();
*/
}

void 	EPACopyVertexB::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
    copy_pos		= F.r_u32();
*/
}

void 	EPADamping::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
    F.r_fvector3	(damping);
    vlowSqr			= F.r_float	();
    vhighSqr		= F.r_float	();
*/
}

void 	EPAExplosion::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
	F.r_fvector3	(centerL);
	F.r_fvector3	(center);
	velocity		= F.r_float();
	magnitude		= F.r_float();
	stdev			= F.r_float();
	age				= F.r_float();
	epsilon			= F.r_float();
*/
}

void 	EPAFollow::FillProp		(PropItemVec& items, LPCSTR pref)
{
/*	
	magnitude		= F.r_float();
	epsilon			= F.r_float();
	max_radius		= F.r_float();
*/
}

void 	EPAGravitate::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
	magnitude		= F.r_float();
	epsilon			= F.r_float();
	max_radius		= F.r_float();
*/
}

void 	EPAGravity::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
	F.r_fvector3	(directionL);
	F.r_fvector3	(direction);
*/
}

void 	EPAJet::FillProp		(PropItemVec& items, LPCSTR pref)
{
/*	
	F.r_fvector3	(centerL);
	F.r				(&accL,sizeof(PAPI::pDomain));
	F.r_fvector3	(center);
	F.r				(&acc,sizeof(PAPI::pDomain));
	magnitude		= F.r_float();
	epsilon			= F.r_float();
	max_radius		= F.r_float();
*/
}

void 	EPAKillOld::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
    age_limit		= F.r_float	();
    kill_less_than	= F.r_u32	();
*/
}

void 	EPAMatchVelocity::FillProp(PropItemVec& items, LPCSTR pref)
{
/*	
	magnitude		= F.r_float();
	epsilon			= F.r_float();
	max_radius		= F.r_float();
*/
}

void 	EPAMove::FillProp		(PropItemVec& items, LPCSTR pref)
{
	
}

void 	EPAOrbitLine::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
	F.r_fvector3	(pL);
	F.r_fvector3	(axisL);
	F.r_fvector3	(p);
	F.r_fvector3	(axis);
	magnitude		= F.r_float();
	epsilon			= F.r_float();
	max_radius		= F.r_float();
*/
}

void 	EPAOrbitPoint::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
	F.r_fvector3	(centerL);
	F.r_fvector3	(center);
	magnitude		= F.r_float();
	epsilon			= F.r_float();
	max_radius		= F.r_float();
*/
}

void 	EPARandomAccel::FillProp(PropItemVec& items, LPCSTR pref)
{
/*	
    F.r				(&gen_accL,sizeof(PAPI::pDomain));
    F.r				(&gen_acc,sizeof(PAPI::pDomain));
*/
}

void 	EPARandomDisplace::FillProp(PropItemVec& items, LPCSTR pref)
{
/*	
    F.r				(&gen_dispL,sizeof(PAPI::pDomain));
    F.r				(&gen_disp,sizeof(PAPI::pDomain));
*/
}

void 	EPARandomVelocity::FillProp(PropItemVec& items, LPCSTR pref)
{
/*	
    F.r				(&gen_velL,sizeof(PAPI::pDomain));
    F.r				(&gen_vel,sizeof(PAPI::pDomain));
*/
}

void 	EPARestore::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
    time_left		= F.r_float();
*/
}

void 	EPASink::FillProp		(PropItemVec& items, LPCSTR pref)
{
/*	
	kill_inside		= F.r_u32();
    F.r				(&positionL,sizeof(PAPI::pDomain));
    F.r				(&position,sizeof(PAPI::pDomain));
*/
}

void 	EPASinkVelocity::FillProp(PropItemVec& items, LPCSTR pref)
{
/*	
	kill_inside		= F.r_u32();
    F.r				(&velocityL,sizeof(PAPI::pDomain));
    F.r				(&velocity,sizeof(PAPI::pDomain));
*/
}

void 	EPASpeedLimit::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
	min_speed		= F.r_float();
	max_speed		= F.r_float();
*/
}

void 	EPASource::FillProp		(PropItemVec& items, LPCSTR pref)
{
/*	
    F.r				(&positionL,sizeof(PAPI::pDomain));
    F.r				(&velocityL,sizeof(PAPI::pDomain));
    F.r				(&position,sizeof(PAPI::pDomain));
    F.r				(&velocity,sizeof(PAPI::pDomain));
    F.r				(&rot,sizeof(PAPI::pDomain));
    F.r				(&size,sizeof(PAPI::pDomain));
    F.r				(&color,sizeof(PAPI::pDomain));
	alpha			= F.r_float();
	particle_rate	= F.r_float();
	age				= F.r_float();
	age_sigma		= F.r_float();
    F.r_fvector3	(parent_vel);
    parent_motion	= F.r_float();
*/
}

void 	EPATargetColor::FillProp(PropItemVec& items, LPCSTR pref)
{
/*	
	F.r_fvector3	(color);
    alpha			= F.r_float();
	scale			= F.r_float();
*/
}

void 	EPATargetSize::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
	F.r_fvector3	(size);
	F.r_fvector3	(scale);
*/
}

void 	EPATargetRotate::FillProp(PropItemVec& items, LPCSTR pref)
{
/*	
	F.r_fvector3	(rot);
	scale			= F.r_float();
*/
}

void 	EPATargetVelocity::FillProp(PropItemVec& items, LPCSTR pref)
{
/*	
	F.r_fvector3	(velocityL);
	F.r_fvector3	(velocity);
	scale			= F.r_float();
*/
}

void 	EPAVelocityD::FillProp	(PropItemVec& items, LPCSTR pref)
{
/*	
    F.r				(&gen_velL,sizeof(PAPI::pDomain));
    F.r				(&gen_vel,sizeof(PAPI::pDomain));
*/
}

void 	EPAVortex::FillProp		(PropItemVec& items, LPCSTR pref)
{
/*	
	F.r_fvector3	(centerL);
	F.r_fvector3	(axisL);
	F.r_fvector3	(center);
	F.r_fvector3	(axis);
	magnitude		= F.r_float();
	epsilon			= F.r_float();
	max_radius		= F.r_float();
*/
}

 