// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BreakableObjectH
#define BreakableObjectH
#pragma once

#include "gameobject.h"

class CBreakableObject: public CGameObject {
	typedef	CGameObject		inherited;
private:
	float			fHealth;
public:
					CBreakableObject	();
	virtual			~CBreakableObject	();

	virtual void	Load				( LPCSTR section);
	virtual BOOL	net_Spawn			( LPVOID DC);
	virtual void	shedule_Update		( u32 dt);							// Called by sheduler
	virtual void	UpdateCL			( );								// Called each frame, so no need for dt
	virtual void	renderable_Render	( );

	virtual BOOL	renderable_ShadowGenerate	( ) { return TRUE;	}
	virtual BOOL	renderable_ShadowReceive	( ) { return TRUE;	}
	
	virtual	void	Hit					(float P,Fvector &dir, CObject* who,s16 element,
										Fvector p_in_object_space, float impulse, ALife::EHitType /**hit_type/**/);
	virtual void	net_Export			(NET_Packet& P);
	virtual void	net_Import			(NET_Packet& P);
	virtual BOOL	UsedAI_Locations	();
};

#endif //BreakableObjectH
