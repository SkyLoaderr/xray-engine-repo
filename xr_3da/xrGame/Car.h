#pragma once
#include "entity.h"
#include "PHDynamicData.h"
#include "Physics.h"
// refs
class ENGINE_API			CBoneInstance;

// defs
class CCar :				public CEntity,public CPHObject
{
private:
	typedef CEntity			inherited;
private:
	CPHJeep					m_jeep;
	bool					m_repairing;
	CCameraBase*			camera[3];
	CCameraBase*			active_camera;
	Fvector					m_vCamDeltaHP;

	void					OnCameraChange		(int type);
	
	void					cam_Update			(float dt);

	sound					snd_engine;

	bool					HUDview				( ) { return IsFocused(); }

	static void __stdcall	cb_WheelFL			(CBoneInstance* B);
	static void __stdcall	cb_WheelFR			(CBoneInstance* B);
	static void __stdcall	cb_WheelBL			(CBoneInstance* B);
	static void __stdcall	cb_WheelBR			(CBoneInstance* B);
	virtual void Hit(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space);
	virtual void PhDataUpdate(dReal step);
	virtual void PhTune(dReal step);
	virtual void InitContact(dContact*c){};
	virtual void StepFrameUpdate(dReal step){};
public:
	// Core events
	virtual void			Load				( LPCSTR section );
	virtual BOOL			net_Spawn			( LPVOID DC );
	virtual void			Update				( u32 T ); 
	virtual void			UpdateCL			( ); 
	virtual void			OnVisible			( ); 
	virtual void			OnDeviceCreate		( );

	// Network
	virtual void			net_Export			(NET_Packet& P);				// export to server
	virtual void			net_Import			(NET_Packet& P);				// import from server
	virtual BOOL			net_Relevant		()	{ return getLocal(); };		// relevant for export to server

	// Input
	virtual void			OnMouseMove			(int x, int y);
	virtual void			OnKeyboardPress		(int dik);
	virtual void			OnKeyboardRelease	(int dik);
	virtual void			OnKeyboardHold		(int dik);

	// Hits
	virtual void			HitSignal			(float HitAmount,	Fvector& local_dir, CObject* who, s16 element)	{};
	virtual void			HitImpulse			(float amount,		Fvector& vWorldDir, Fvector& vLocalDir)			{};
	virtual void			Die					()																	{};
	virtual void			g_fireParams		(Fvector& P, Fvector& D)											{};

	// HUD
	virtual void			OnHUDDraw			(CCustomHUD* hud);
public:
	CCar(void);
	virtual ~CCar(void);
};
