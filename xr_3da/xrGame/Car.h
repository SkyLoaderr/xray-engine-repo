#pragma once
#include "entity.h"

// refs
class ENGINE_API			CBoneInstance;

// defs
class CCar :				public CEntity
{
private:
	typedef CEntity			inherited;
private:
	CCameraBase*			camera;

	void					cam_Update			(float dt);

	bool					HUDview				( ) { return IsFocused(); }

	static void __stdcall	cb_WheelFL			(CBoneInstance* B);
	static void __stdcall	cb_WheelFR			(CBoneInstance* B);
	static void __stdcall	cb_WheelBL			(CBoneInstance* B);
	static void __stdcall	cb_WheelBR			(CBoneInstance* B);
public:
	// Core events
	virtual void			Load				( LPCSTR section );
	virtual BOOL			Spawn				( BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags );
	virtual void			Update				( DWORD T ); 
	virtual void			UpdateCL			( ); 
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

	// Hits
	virtual void			HitSignal			(int HitAmount, Fvector& local_dir, CEntity* who)		{};
	virtual void			HitImpulse			(Fvector& vWorldDir, Fvector& vLocalDir, float amount)	{};
	virtual void			Die					()														{};
	virtual void			g_fireParams		(Fvector& P, Fvector& D)								{};


	// HUD
	virtual void			OnHUDDraw			(CCustomHUD* hud);
public:
	CCar(void);
	virtual ~CCar(void);
};
