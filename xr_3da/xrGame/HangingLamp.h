// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef HangingLampH
#define HangingLampH
#pragma once

#include "gameobject.h"

// refs
class CLAItem;

class CHangingLamp: public CGameObject
{
	typedef	CGameObject		inherited;
private:
	int				light_bone_idx;
	IRender_Light*	light_render;
	CLAItem*		lanim;
	CPhysicsShell*	body;
	void			AddElement		(CPhysicsElement* root_e, int id);
	void			CreateBody		();
public:
					CHangingLamp	();
	virtual			~CHangingLamp	();

	virtual void	Load			( LPCSTR section);
	virtual BOOL	net_Spawn		( LPVOID DC);
	virtual void	Update			( u32 dt);							// Called by sheduler
	virtual void	UpdateCL		( );									// Called each frame, so no need for dt
	virtual void	OnVisible		( );

	virtual BOOL	ShadowGenerate	( ) { return TRUE;	}
	virtual BOOL	ShadowReceive	( ) { return TRUE;	}
	
	virtual	void	Hit				(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse);
};

#endif //HangingLampH
