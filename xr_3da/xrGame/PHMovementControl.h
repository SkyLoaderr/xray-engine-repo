#pragma once
#ifndef CPHMOVEMENT_CONTROL_H
#define CPHMOVEMENT_CONTROL_H

#include "PHCharacter.h"

class CPHMovementControl
{

public:
		enum EEnvironment
	{
		peOnGround,
		peAtWall,
		peInAir
	};
private:
	void				CheckEnvironment	(const Fvector& V);

	float				m_fGroundDelayFactor;
	BOOL				bIsAffectedByGravity;
	//------------------------------
	CObject*			pObject;
	CPHSimpleCharacter  m_character;
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

	float				fLastMotionMag;

	float				fActualVelocity;
	float				fContactSpeed;
public:
	Fvector				vExternalImpulse;
	BOOL				bSleep;

	BOOL				gcontact_Was;			// Приземление
	float				gcontact_Power;			// Насколько сильно ударились
	float				gcontact_HealthLost;	// Скоко здоровья потеряли


	void				dbg_Draw(){};


	void				SetFriction(float air, float wall, float ground)
	{
		fAirFriction	= air;
		fWallFriction	= wall;
		fGroundFriction	= ground;
	}

	float				GetCurrentFriction()		{ return fFriction; }

	const Fvector&		GetVelocity		( )			{ return vVelocity;	}
	float				GetVelocityMagnitude()		{ return vVelocity.magnitude();	}
	float				GetVelocityActual	()		{ return fActualVelocity;	}
	float				GetContactSpeed	()			{ return fContactSpeed; }
	void				SetVelocity		(float x, float y, float z)	{vVelocity.set(x,y,z);m_character.SetVelocity(vVelocity);}
	void				SetVelocity		(const Fvector& v)	{vVelocity.set(v);m_character.SetVelocity(vVelocity);}

	void				CalcMaximumVelocity	(Fvector& dest, Fvector& accel, float friction){};
	void				CalcMaximumVelocity	(float& dest, float accel, float friction){};

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
	{	vPosition.set	(P); m_character.SetPosition(vPosition);}

	void				SetPosition		(float x, float y, float z)
	{	vPosition.set	(x,y,z);m_character.SetPosition(vPosition); 	}

	void				GetPosition		(Fvector &P)
	{	P.set			(vPosition); }

	void				GetBoundingSphere(Fvector &P, float &R)
	{
		P.set			(vPosition);
		R =				aabb.getradius();
	}

	void				Calculate		(Fvector& vAccel, float ang_speed, float jump, float dt, bool bLight);
	void				Move			(Fvector& Dest, Fvector& Motion, BOOL bDynamic=FALSE){};
	void				SetApplyGravity	(BOOL flag){ bIsAffectedByGravity=flag; }



	CPHMovementControl(void);
	~CPHMovementControl(void);
};
#endif