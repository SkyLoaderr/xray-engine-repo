#include "stdafx.h"
#include "..\cl_intersect.h"
#include "phmovementcontrol.h"
#include "entity.h"

#define GROUND_FRICTION	10.0f
#define AIR_FRICTION	0.01f
#define WALL_FRICTION	3.0f
//#define AIR_RESIST		0.001f

#define def_X_SIZE_2	0.35f
#define def_Y_SIZE_2	0.8f
#define def_Z_SIZE_2	0.35f

CPHMovementControl::CPHMovementControl(void)
{
	m_character.Create();
		pObject	=			NULL;
	eOldEnvironment =	peInAir;
	eEnvironment =		peInAir;
	aabb.set			(-def_X_SIZE_2,0,-def_Z_SIZE_2, def_X_SIZE_2, def_Y_SIZE_2*2, def_Z_SIZE_2);
	vFootCenter.set		(0,0,0);
	vFootExt.set		(0,0,0);
	fMass				= 100;
	fMinCrashSpeed		= 12.0f;
	fMaxCrashSpeed		= 25.0f;
	vVelocity.set		(0,0,0);
	vPosition.set		(0,0,0);
	vExternalImpulse.set(0,0,0);
	fLastMotionMag		= 1.f;
	fAirFriction		= AIR_FRICTION;
	fWallFriction		= WALL_FRICTION;
	fGroundFriction		= GROUND_FRICTION;
	fFriction			= fAirFriction;
	bIsAffectedByGravity= TRUE;
	fActualVelocity		= 0;
	m_fGroundDelayFactor= 1.f;
	gcontact_HealthLost = 0;
	fContactSpeed		= 0.f;
}

CPHMovementControl::~CPHMovementControl(void)
{
	m_character.Destroy();
}

void CPHMovementControl::Calculate(Fvector& vAccel,float ang_speed,float jump,float dt,bool bLight){
	m_character.SetAcceleration(vAccel);
	vPosition=m_character.GetPosition();
	vVelocity=m_character.GetVelocity();
}