// MovementControl.h: interface for the CMovementControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVEMENTCONTROL_H__B87EE816_FF37_4804_921B_C71A1FA397AF__INCLUDED_)
#define AFX_MOVEMENTCONTROL_H__B87EE816_FF37_4804_921B_C71A1FA397AF__INCLUDED_
#pragma once

class CMovementControl  
{
public:
	enum EEnvironment
	{
		peOnGround,
		peAtWall,
		peInAir
	};
private:
	void				CheckEnvironment();

	BOOL				bIsAffectedByGravity;
	//------------------------------
	CObject*			pObject;
	EEnvironment		eOldEnvironment;
	EEnvironment		eEnvironment;
	Fbox				aabb;
	Fbox				boxes	[4];
	Fvector				vFootCenter;			// задаются относительно vPosition
	Fvector				vFootExt;				//

	float				fAirFriction;
	float				fWallFriction;
	float				fGroundFriction;
	float				fFriction;				// Current friction

	float				fMass;
	float				fMinCrashSpeed;
	float				fMaxCrashSpeed;

	Fvector				vVelocity;
	Fvector				vPosition;

	Fvector				vLastMotion;
	float				fLastMotionMag;

	float				fActualVelocity;
public:
	Fvector				vExternalImpulse;
	BOOL				bSleep;

	BOOL				gcontact_Was;			// Приземление
	float				gcontact_Power;			// Насколько сильно ударились
	float				gcontact_HealthLost;	// Скоко здоровья потеряли


	void				dbg_Draw();


	void				SetFriction(float air, float wall, float ground)
	{
		fAirFriction	= air;
		fWallFriction	= wall;
		fGroundFriction	= ground;
//		fAirResistFactor= 1-a_res; clamp(fAirResistFactor,0.001f,1.f);
	}

	float				GetCurrentFriction()		{ return fFriction; }

	const Fvector&		GetVelocity		( )			{ return vVelocity;	}
	float				GetVelocityMagnitude()		{ return vVelocity.magnitude();	}
	float				GetVelocityActual	()		{ return fActualVelocity;	}
	void				SetVelocity		(float x, float y, float z)	{vVelocity.set(x,y,z);}
	void				SetVelocity		(const Fvector& v)	{vVelocity.set(v);}
	void				CalcMaximumVelocity	(Fvector& dest, Fvector& accel, float friction);
	void				CalcMaximumVelocity	(float& dest, float accel, float friction);

	void				ActivateBox		(DWORD id)	{ aabb.set(boxes[id]);	}

	const EEnvironment	Environment		( )			{ return eEnvironment; }
	const Fbox&			Box				( )			{ return aabb; }
	void				SetBox			(DWORD id, const Fbox &BB)	{ boxes[id].set(BB); aabb.set(BB); }

	void				SetParent		(CObject* P){ pObject = P; }

	void				SetMass			(float M)	{ fMass = M; }
	float				GetMass			()			{ return fMass;	}
	void				SetFoots		(Fvector& C, Fvector &E)
	{	vFootCenter.set	(C); vFootExt.set	(E); 	}
	void				SetCrashSpeeds	(float min, float max)
	{	fMinCrashSpeed	= min; 	fMaxCrashSpeed	= max; 	}
	void				SetPosition		(Fvector &P)
	{	vPosition.set	(P); 	}
	void				SetPosition		(float x, float y, float z)
	{	vPosition.set	(x,y,z); 	}
	void				GetPosition		(Fvector &P)
	{	P.set			(vPosition); }
	void				GetBoundingSphere(Fvector &P, float &R)
	{
		P.set			(vPosition);
		R =				aabb.getradius();
	}
	void				Calculate		(Fvector& vAccel, float ang_speed, float jump, float dt, bool bLight);
	void				Move			(Fvector& Dest, Fvector& Motion, BOOL bDynamic=FALSE);
	void				SetApplyGravity	(BOOL flag){ bIsAffectedByGravity=flag; }

	CMovementControl	();
	~CMovementControl	();
};

#endif // !defined(AFX_MOVEMENTCONTROL_H__B87EE816_FF37_4804_921B_C71A1FA397AF__INCLUDED_)
