// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef HangingLampH
#define HangingLampH
#pragma once

#include "gameobject.h"

// refs
class CLAItem;
class CPhysicsElement;
class CSE_ALifeObjectHangingLamp;

class CHangingLamp: public CGameObject {
	typedef	CGameObject		inherited;
private:
	u16				guid_bone;
	IRender_Light*	light_render;
	CLAItem*		lanim;

	IRender_Glow*	glow_render;
	
	float			fHealth;
	float			fBrightness;

	void			CreateBody		(CSE_ALifeObjectHangingLamp	*lamp);
	bool			Alive			(){return fHealth>0.f;}

	void			TurnOn			();
	void			TurnOff			();
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
	
	virtual	void	Hit(float P,Fvector &dir, CObject* who,s16 element,
										Fvector p_in_object_space, float impulse, ALife::EHitType /**hit_type/**/);
	virtual void	net_Export		(NET_Packet& P);
	virtual void	net_Import		(NET_Packet& P);
	virtual BOOL	UsedAI_Locations();
};

#endif //HangingLampH
