// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef HangingLampH
#define HangingLampH
#pragma once

#include "gameobject.h"

class CHangingLamp: public CGameObject
{
	typedef	CGameObject		inherited;
private:
public:
	virtual void	Load			( LPCSTR section);
	virtual BOOL	net_Spawn		( LPVOID DC);
	virtual void	Update			( u32 dt);							// Called by sheduler
	virtual void	UpdateCL		( );									// Called each frame, so no need for dt
	virtual void	OnVisible		( );

	virtual BOOL	ShadowGenerate	( ) { return TRUE;	}
	virtual BOOL	ShadowReceive	( ) { return TRUE;	}
					CHangingLamp		();
	virtual ~CHangingLamp();
};

#endif //HangingLampH
