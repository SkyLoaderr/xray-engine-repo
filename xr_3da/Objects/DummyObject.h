// DummyObject.h: interface for the CDummyObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUMMYOBJECT_H__3A3BE489_045B_4229_AD0F_83CACEA62145__INCLUDED_)
#define AFX_DUMMYOBJECT_H__3A3BE489_045B_4229_AD0F_83CACEA62145__INCLUDED_
#pragma once

#include "gameobject.h"
#include "PhysicsShell.h"
#include "..\psruntime.h"

// refs
class ENGINE_API		CObjectAnimator;

// t-defs
class CDummyObject		: public CGameObject
{
	typedef	CGameObject		inherited;

private:
	enum SStyle{
		esAnimated			=1<<0,	
		esModel				=1<<1, 
		esParticles			=1<<2, 
		esSound				=1<<3,
		esRelativePosition	=1<<4
	};
	u8									style;
private:
	CObjectAnimator*					s_animator;
	CVisual*							s_particles;
	sound								s_sound;
	PS::SEmitter						s_emitter;
	CPhysicsShell*						m_pPhysicsShell;

	Fmatrix								relation;
public:
	virtual void						Load			( LPCSTR section);
	virtual BOOL						net_Spawn		( LPVOID DC);
	virtual void						Update			( DWORD dt);							// Called by sheduler
	virtual void						UpdateCL		( );									// Called each frame, so no need for dt
	virtual void						OnVisible		( );
	virtual void						OnDeviceCreate	( );

	virtual BOOL						ShadowGenerate	( ) { return FALSE;	}
	virtual BOOL						ShadowReceive	( ) { return FALSE;	}

	void				PlayDemo		( LPCSTR N );

	CDummyObject		();
	virtual ~CDummyObject();
};

#endif // !defined(AFX_DUMMYOBJECT_H__3A3BE489_045B_4229_AD0F_83CACEA62145__INCLUDED_)
