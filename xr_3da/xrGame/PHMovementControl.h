#pragma once
#ifndef CPHMOVEMENT_CONTROL_H
#define CPHMOVEMENT_CONTROL_H

#include "PHCharacter.h"
//#include "MovementControl.h"
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

	CPHSimpleCharacter  m_character;
	
	float				m_fGroundDelayFactor;
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

	float				fLastMotionMag;

	float				fActualVelocity;
	float				fContactSpeed;
	float				fLastUpdateTime;
	Fvector				vLastUpdatePosition;
public:
	
	Fvector				vExternalImpulse;
	BOOL				bSleep;

	BOOL				gcontact_Was;			// Приземление
	float				gcontact_Power;			// Насколько сильно ударились
	float				gcontact_HealthLost;	// Скоко здоровья потеряли
	void				CreateCharacter()		{	
													dVector3 size={aabb.x2-aabb.x1,aabb.y2-aabb.y1,aabb.z2-aabb.z1};
													m_character.Create(size);
												}
	void				DestroyCharacter(){m_character.Destroy();}
	void				Load					(LPCSTR section);
#ifdef DEBUG
	void				dbg_Draw(){m_character.OnRender();};
#endif

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

	void				ActivateBox		(DWORD id)	{ aabb.set(boxes[id]);
													
														m_character.Destroy();
														CreateCharacter();	
														m_character.SetPosition(vPosition);	
													}

	EEnvironment	Environment		( )			{ return eEnvironment; }
	EEnvironment	OldEnvironment		( )		{ return eOldEnvironment; }
	const Fbox&			Box				( )			{ return aabb; }
	const Fbox*			Boxes			( )			{return boxes;}
	const Fvector&		FootExtent		( )			{return vFootExt;}
	void				SetBox			(DWORD id, const Fbox &BB)	{ boxes[id].set(BB); aabb.set(BB); }

	void				SetParent		(CObject* P){ pObject = P; }

	void				SetMass			(float M)	{ fMass = M;
													  m_character.SetMas(fMass);
													}
	float				GetMass			()			{ return fMass;	}

	void				SetFoots		(Fvector& C, Fvector &E)
	{	vFootCenter.set	(C); vFootExt.set	(E); 	}

	void				SetCrashSpeeds	(float min, float max)
	{	fMinCrashSpeed	= min; 	fMaxCrashSpeed	= max; 	}

	void				SetPosition		(Fvector &P)
	{	vPosition.set	(P); vLastUpdatePosition.set(P); m_character.SetPosition(vPosition);}

	void				SetPosition		(float x, float y, float z)
	{	vPosition.set	(x,y,z);m_character.SetPosition(vPosition); 	}

	void				GetPosition		(Fvector &P)
	{	P.set			(vPosition); }
	void				GetCharacterPosition(Fvector &P)
	{ P.set(m_character.GetPosition());}


	void				GetBoundingSphere(Fvector &P, float &R)
	{
		P.set			(vPosition);
		R =				aabb.getradius();
	}

	void				Calculate		(Fvector& vAccel, float ang_speed, float jump, float dt, bool bLight);
	void				Move			(Fvector& Dest, Fvector& Motion, BOOL bDynamic=FALSE){};
	void				SetApplyGravity	(BOOL flag){ bIsAffectedByGravity=flag; }
	void				GetDeathPosition(Fvector pos){pos.set( m_character.DeathPosition());}
	void SetEnvironment( int enviroment,int old_enviroment);
	void		ApplyImpulse(const Fvector& dir,const dReal P){m_character.ApplyImpulse(dir,P);};
	CPHMovementControl(void);
	~CPHMovementControl(void);
};
#endif