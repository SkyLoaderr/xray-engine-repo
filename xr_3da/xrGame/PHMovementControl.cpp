#include "stdafx.h"
#include "..\cl_intersect.h"
#include "phmovementcontrol.h"
#include "entity.h"
#include "PHDynamicData.h"
#include "Physics.h"
#define GROUND_FRICTION	10.0f
#define AIR_FRICTION	0.01f
#define WALL_FRICTION	3.0f
//#define AIR_RESIST		0.001f
 
#define def_X_SIZE_2	0.35f
#define def_Y_SIZE_2	0.8f
#define def_Z_SIZE_2	0.35f

CPHMovementControl::CPHMovementControl(void)
{
	//m_character.Create();
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
	//m_character.Destroy();
}

//static Fvector old_pos={0,0,0};
//static bool bFirst=true;

void CPHMovementControl::Calculate(Fvector& vAccel,float ang_speed,float jump,float dt,bool bLight){

    vPosition=m_character.IPosition();
	vAccel.y+=jump;
	m_character.SetMaximumVelocity(vAccel.magnitude()/10.f);
	m_character.SetAcceleration(vAccel);
	

 vVelocity =m_character.GetVelocity(); 

	gcontact_Was=m_character.ContactWas();
	fContactSpeed=0.f;
	if(gcontact_Was){
		fContactSpeed=m_character.ContactVelocity();
		//m_character.ContactVelocity()=0.f;
		gcontact_Power				= fContactSpeed/fMaxCrashSpeed;

			gcontact_HealthLost			= 0;
			if (fContactSpeed>fMinCrashSpeed) 
			{
				//float k=10000.f/(B-A);
				//float dh=sqrtf((dv-A)*k);
				gcontact_HealthLost = 
					(100*(fContactSpeed-fMinCrashSpeed))/(fMaxCrashSpeed-fMinCrashSpeed);
			}
	}
	CheckEnvironment(vPosition);
	bSleep=false;
	
	
}

void CPHMovementControl::Load					(LPCSTR section){

	Fbox	bb;

	// Movement: BOX
	Fvector	vBOX0_center= pSettings->ReadVECTOR	(section,"ph_box0_center"	);
	Fvector	vBOX0_size	= pSettings->ReadVECTOR	(section,"ph_box0_size"		);
	bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
	SetBox		(0,bb);

	// Movement: BOX
	Fvector	vBOX1_center= pSettings->ReadVECTOR	(section,"ph_box1_center"	);
	Fvector	vBOX1_size	= pSettings->ReadVECTOR	(section,"ph_box1_size"		);
	bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	SetBox		(1,bb);

	// Movement: Foots
	Fvector	vFOOT_center= pSettings->ReadVECTOR	(section,"ph_foot_center"	);
	Fvector	vFOOT_size	= pSettings->ReadVECTOR	(section,"ph_foot_size"		);
	bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	SetFoots	(vFOOT_center,vFOOT_size);

	// Movement: Crash speed and mass
	float	cs_min		= pSettings->ReadFLOAT	(section,"ph_crash_speed_min"	);
	float	cs_max		= pSettings->ReadFLOAT	(section,"ph_crash_speed_max"	);
	float	mass		= pSettings->ReadFLOAT	(section,"ph_mass"				);
	SetCrashSpeeds	(cs_min,cs_max);
	SetMass		(mass);


	// Movement: Frictions
	float af, gf, wf;
	af					= pSettings->ReadFLOAT	(section,"ph_friction_air"	);
	gf					= pSettings->ReadFLOAT	(section,"ph_friction_ground");
	wf					= pSettings->ReadFLOAT	(section,"ph_friction_wall"	);
	SetFriction	(af,wf,gf);

	// BOX activate
	ActivateBox	(0);
}

void CPHMovementControl::CheckEnvironment(const Fvector &V){
eOldEnvironment=eEnvironment;
switch (m_character.CheckInvironment()){
case peOnGround : eEnvironment=peOnGround;break;
case peInAir :		eEnvironment=peInAir		;break;
case peAtWall : eEnvironment=peAtWall		;break;
}
}


void	CPHMovementControl::SetEnvironment( int enviroment,int old_enviroment){
	switch(enviroment){
	case 0: eEnvironment=peOnGround;
	break;
	case 1: eEnvironment=peAtWall;
	break;
	case 2: eEnvironment=peInAir;
	}
	switch(old_enviroment){
	case 0: eOldEnvironment=peOnGround;
	break;
	case 1: eOldEnvironment=peAtWall;
	break;
	case 2: eOldEnvironment=peInAir;
	}
}