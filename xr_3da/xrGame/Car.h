#pragma once
#include "entity.h"

class CCar :				public CEntity
{
private:
	CCameraBase*			camera;

	void					cam_Update			(float dt);

	bool					HUDview				( ) { return IsFocused(); }
public:
	// Core events
	virtual void			Load				( CInifile* ini, const char *section );
	virtual BOOL			Spawn				( BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags );
	virtual void			Update				( DWORD T ); 
	virtual void			OnVisible			( ); 

	// Network
	virtual void			net_Export			(NET_Packet* P);				// export to server
	virtual void			net_Import			(NET_Packet* P);				// import from server
	virtual BOOL			net_Relevant		()	{ return net_Local; };		// relevant for export to server

	// Input
	virtual void			OnMouseMove			(int x, int y);
	virtual void			OnKeyboardPress		(int dik);
	virtual void			OnKeyboardRelease	(int dik);
	virtual void			OnKeyboardHold		(int dik);

	// HUD
	virtual void			OnHUDDraw			(CCustomHUD* hud);
public:
	CCar(void);
	virtual ~CCar(void);
};
