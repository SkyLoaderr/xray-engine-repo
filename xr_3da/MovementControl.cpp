// MovementControl.cpp: implementation of the CMovementControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\collide\cl_intersect.h"

#include "MovementControl.h"
#include "entity.h"

#define GROUND_FRICTION	10.0f
#define AIR_FRICTION	0.01f
#define WALL_FRICTION	3.0f
//#define AIR_RESIST		0.001f

#define def_X_SIZE_2	0.35f
#define def_Y_SIZE_2	0.8f
#define def_Z_SIZE_2	0.35f

void CMovementControl::dbg_Draw()
{
	Fvector P2; P2.add(vPosition,vVelocity);
	Device.Primitive.dbg_DrawLINE(precalc_identity,vPosition,P2,D3DCOLOR_RGBA(255,255,255,255));
	P2.add(vPosition,vLastMotion);
	Device.Primitive.dbg_DrawLINE(precalc_identity,vPosition,P2,D3DCOLOR_RGBA(0,255,0,255));

	Fvector sz,C; aabb.getsize(sz); sz.div(2); aabb.getcenter(C);
	Fmatrix	M = pObject->svTransform;
	M.c.add (C);
	Device.Primitive.dbg_DrawOBB (M,sz,D3DCOLOR_RGBA(0,0,255,255));

	Fvector fct;
	pObject->svTransform.transform_tiny(fct,vFootCenter);
	Fmatrix fm;
	fm.translate(fct);
	Device.Primitive.dbg_DrawOBB (fm,vFootExt,D3DCOLOR_RGBA(255,0,255,255));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMovementControl::CMovementControl()
{
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
	vLastMotion.set		(0,0,0);
	vExternalImpulse.set(0,0,0);
	fLastMotionMag		= 1.f;
	fAirFriction		= AIR_FRICTION;
	fWallFriction		= WALL_FRICTION;
	fGroundFriction		= GROUND_FRICTION;
	fFriction			= fAirFriction;
	bIsAffectedByGravity= TRUE;
	fActualVelocity		= 0;
}

CMovementControl::~CMovementControl()
{

}

float	psSqueezeVelocity;
int		psPhysicsFPS=100;

// Velocity+Friction(dumping), Movement
void Integrate(float& v, float &s, float& a, float dt, float f)
{
	float	Q		= 1.f/float(psPhysicsFPS);
	int		steps	= iFloor(dt/Q);	// note: round-towards-zero
	float	v0		= v;
	float	QmulF	= Q*f;
	float	Qdiv2	= Q*0.5f;
	for (int i=0; i<steps; i++) 
	{
		// velocity
		v0 = v;
		v += a*Q - (v0 + a*Q)*QmulF;
		// motion
		s += (v0 + v)*Qdiv2;
	}
	Q = dt-float(steps)*Q;
	QmulF	= Q*f;
	Qdiv2	= Q*0.5f;

	// velocity
	v0 = v;
	v += a*Q - (v0 + a*Q)*QmulF;

	// motion
	s += (v0 + v)*Qdiv2;
}

void Integrate(Fvector& v, Fvector &s, Fvector& a, float dt, float f)
{
	float	Q		= 1.f/float(psPhysicsFPS);
	int		steps	= iFloor(dt/Q);	// note: round-towards-zero
	Fvector	v0		= v;
	float	QmulF	= Q*f;
	float	Qdiv2	= Q*0.5f;
	for (int i=0; i<steps; i++) 
	{
		// velocity
		v0.set(v);
		v.x += a.x*Q - (v0.x + a.x*Q)*QmulF;
		v.y += a.y*Q - (v0.y + a.y*Q)*QmulF;
		v.z += a.z*Q - (v0.z + a.z*Q)*QmulF;

		// motion
		s.x += (v0.x + v.x)*Qdiv2;
		s.y += (v0.y + v.y)*Qdiv2;
		s.z += (v0.z + v.z)*Qdiv2;
	}
	Q = dt-float(steps)*Q;
	QmulF	= Q*f;
	Qdiv2	= Q*0.5f;

	// velocity
	v0.set	(v);
	v.x += a.x*Q - (v0.x + a.x*Q)*QmulF;
	v.y += a.y*Q - (v0.y + a.y*Q)*QmulF;
	v.z += a.z*Q - (v0.z + a.z*Q)*QmulF;

	// motion
	s.x += (v0.x + v.x)*Qdiv2;
	s.y += (v0.y + v.y)*Qdiv2;
	s.z += (v0.z + v.z)*Qdiv2;
}

void CMovementControl::CalcMaximumVelocity	(float& v, float a, float friction){
	// first approximation
	float Q		= 5;
	float QmulF = Q*friction;
	v = a*Q - a*Q*QmulF;

	// sample iterations
	float		S; S=0;
	Integrate	(v,S,a,100,friction);
}

void CMovementControl::CalcMaximumVelocity(Fvector& v, Fvector& a, float friction)
{
	// first approximation
	float Q		= 5;
	float QmulF = Q*friction;
	v.x = a.x*Q - a.x*Q*QmulF;
	v.y = a.y*Q - a.y*Q*QmulF;
	v.z = a.z*Q - a.z*Q*QmulF;

	// sample iterations
	Fvector		S; S.set(0,0,0);
	Integrate	(v,S,a,100,friction);
}

void CMovementControl::Calculate(Fvector &_Accel, float ang_speed, float jump, float dt, bool bLight)
{
	Fvector motion,vAccel;
//	Fvector vOldVelocity	= vVelocity;
	float	fOldFriction	= fFriction;
	float	fOldActVelocity	= fActualVelocity;

	vExternalImpulse.div(dt);
	vAccel.add			(vExternalImpulse,_Accel);

	// Analyze jump
	bool bJump = (fabsf(jump)>EPS) && (eEnvironment==peOnGround);

	// Check for Sleep-State
	bSleep = FALSE;
	float a_speed = fabsf(ang_speed);
	switch (eEnvironment)
	{
	case peOnGround:
		bSleep = (vAccel.magnitude()<EPS) && (fLastMotionMag<EPS_L) && (!bJump) && (a_speed<EPS_S);
		break;
	default:
		bSleep = (vAccel.magnitude()<EPS) && (!bIsAffectedByGravity) && (fLastMotionMag<EPS_L) && (!bJump) && (a_speed<EPS_S);
		break;
	}
	if (bSleep)	{
		fLastMotionMag		= 0;
		fActualVelocity		= 0;
		return;
	}

	Device.Statistic.Physics.Begin();

	// Calculate gravity
	float gravity;
	gravity = psGravity;
	if (eEnvironment==peOnGround && !bJump) gravity *= 0.5f;
	vAccel.y -= bIsAffectedByGravity?gravity:0;

	// Physics integration
	vExternalImpulse.set(0,0,0);
	motion.set			(0,0,0);
	if (bJump)			{
		vVelocity.y = 0; 
		vAccel.y	= vAccel.y+psGravity+jump/dt;
		fFriction	= fAirFriction;
	}
	Integrate		(vVelocity,motion,vAccel,dt,fFriction);

	// Calculate collisions
	Fvector	final_pos,final_vel;
	
	Fbox bbf;
	bbf.set(-vFootExt.x, -vFootExt.y, -vFootExt.z, +vFootExt.x, +vFootExt.y, +vFootExt.z );
	bbf.add(vFootCenter);
	pCreator->ObjectSpace.cl_Move( 
		pObject->CFORM(), pObject->svTransform, 
		motion, aabb, bbf, final_pos, final_vel, EPS);
	
	// Velocity stuff
	float s_calc	= motion.magnitude();	// length of motion - dS - requested
	motion.sub		(final_pos,vPosition);	// motion - resulting
	vLastMotion.set	(motion);
	float s_res		= motion.magnitude();	// length of motion - dS - resulting

	float src		= 20*dt;	clamp(src,0.f,1.f);
	float dst		= 1-src;
	fLastMotionMag			= dst*fLastMotionMag + src*s_res;
	float	fAvgVelocity	= fLastMotionMag/dt;
	fActualVelocity			= dst*fActualVelocity+ src*fAvgVelocity;

	//	Don't allow new velocity to go against original velocity unless told otherwise
	Fvector vel_dir;
	final_vel.normalize_safe	();
	vel_dir.normalize_safe		(vVelocity);
	if ((final_vel.dotproduct	(vel_dir)<0.f)||((s_res/s_calc)<0.001f)){ 
		vVelocity.set(0,0,0);
		final_pos.set(vPosition);
	} else {
		vPosition.set(final_pos);
	}
	
	// Environment
	if (fLastMotionMag>EPS_S)
		CheckEnvironment	();
 
	// Set movement friction
	switch(eEnvironment)
	{
		case peInAir:		fFriction = fAirFriction;		break;
		case peAtWall:		fFriction = fWallFriction;		break;
		case peOnGround:	fFriction = fGroundFriction;	break;
		default: NODEFAULT;
	}

	// если определили приземление или резко уиеньшилась скорость (порядок fMinCrashSpeed)
	// определим возможное повреждение
	gcontact_Was		= FALSE;
	if (
		((eOldEnvironment==peInAir || eOldEnvironment==peAtWall) && (eEnvironment==peOnGround)) // ||
//		((fabsf(jump)>EPS) && (eOldEnvironment==peOnGround) && (eEnvironment==peOnGround))
		)
	{
		gcontact_Was		= TRUE;
		gcontact_Power		= 0;
		gcontact_HealthLost = 0;
		if (s_res<s_calc)	
		{
			float		dt_x	= dt*(s_res/s_calc);
			float		dummy_s = 0;
			float		a		= vAccel.magnitude();
			Integrate	(fOldActVelocity,dummy_s,a,dt_x,fOldFriction);
//			Integrate	(vOldVelocity,dummy,,dt_x,fOldFriction);

			// now vOldVelocity - average velocity at contact time
			float		contact_speed	= fOldActVelocity;

			// contact with ground
			gcontact_Power	= contact_speed/fMaxCrashSpeed;

			gcontact_HealthLost = 0;
			if (contact_speed>fMinCrashSpeed) 
			{
				//float k=10000.f/(B-A);
				//float dh=sqrtf((dv-A)*k);
				gcontact_HealthLost = 
					(100*(contact_speed-fMinCrashSpeed))/(fMaxCrashSpeed-fMinCrashSpeed);
				
				Msg	("sres: %2.1f, scalc: %2.1f, cspeed: %2.1f (min: %2.1f)",
					s_res,s_calc,contact_speed,fMinCrashSpeed);
			}
		}
	}

	Device.Statistic.Physics.End();
}
void CMovementControl::Move(Fvector& Dest, Fvector& Motion, BOOL bDynamic)
{
	Fbox	bbf;
	bbf.set(-vFootExt.x, -vFootExt.y, -vFootExt.z, +vFootExt.x, +vFootExt.y, +vFootExt.z );
	bbf.add(vFootCenter);
	Fvector final_vel;
	pCreator->ObjectSpace.cl_Move( 
		pObject->CFORM(), pObject->svTransform, 
		Motion, aabb, bbf, Dest, final_vel, EPS, bDynamic);
}

void CMovementControl::CheckEnvironment()
{
	// verify surface and update Player State
	int cp_cnt=pCreator->ObjectSpace.q_result.tris.size();
	eOldEnvironment	= eEnvironment;
	eEnvironment	= peInAir;
	if (cp_cnt)
	{
		Fmatrix33	A; A.set(pObject->Rotation());
		Fvector		C; 
		pObject->svTransform.transform_tiny(C,vFootCenter);
		for(int i=0; i<cp_cnt; i++)
		{
			clQueryTri& T=pCreator->ObjectSpace.q_result.tris[i];
			// все что ниже пояса
			if (RAPID::TestBBoxTri(A,C,vFootExt,&T.p[0],false))
			{
				Fvector N; 
				N.mknormal(T.p[0],T.p[1],T.p[2]);
				if ((N.y>.7f))
				{
					eEnvironment=peOnGround;
					return;
				} else  {
					if((N.y>=0)&&(N.y<=0.5f))
					{
						eEnvironment=peAtWall;
					}
				}
			}
		}
	}
}
