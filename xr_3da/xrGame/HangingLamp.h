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
	
	float			fHealth;
	float			fBrightness;

	void			AddElement		(CPhysicsElement* root_e, int id);
	void			CreateBody		(float mass);
	bool			Alive			(){return fHealth>0.f;}
public:
					CHangingLamp	();
	virtual			~CHangingLamp	();

	virtual void	Load			( LPCSTR section);
	virtual BOOL	net_Spawn		( LPVOID DC);
	virtual void	shedule_Update	( u32 dt);							// Called by sheduler
	virtual void	UpdateCL		( );								// Called each frame, so no need for dt
	virtual void	renderable_Render		( );

	virtual BOOL	renderable_ShadowGenerate	( ) { return TRUE;	}
	virtual BOOL	renderable_ShadowReceive	( ) { return TRUE;	}
	
	virtual	void	Hit				(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse);
	virtual void	net_Export		(NET_Packet& P);
	virtual void	net_Import		(NET_Packet& P);
};

#endif //HangingLampH
