#pragma once
#include "entity.h"
#include "PHDynamicData.h"

#include "PHJeep.h"

// refs
class ENGINE_API			CBoneInstance;
class						CActor;

// defs
class CCar :				public CEntity,public CPHObject
{
public:
	
private:
	typedef CEntity			inherited;
private:
	CPHJeep					m_jeep;
	bool					m_repairing;
	CCameraBase*			camera[3];
	CCameraBase*			active_camera;
	CActor*					m_owner;
	Fmatrix					fmPosDriver;
	Fvector					m_vCamDeltaHP;
////////////////////////////////////////////////////
bool  Breaks;
int   DriveDirection;
float DriveVelocity;
float DriveForce;
float VelocityRate;
CPhysicsJoint* weels[4];
int	  m_doors_ids[2];
int	  m_exhaust_ids[2];
CPGObject* m_pExhaustPG1;
CPGObject* m_pExhaustPG2;
////////////////////////////////////////////////////
void Steer(const char& steering);
float GetSteerAngle();
void LimitWeels();
void Drive(const char& velocity,dReal force=500.f);
void Drive();
void NeutralDrive();
void JointTune(dReal step);
void Revert();
void SetStartPosition(Fvector pos){}
void SetPosition(Fvector pos){}
void SetRotation(dReal* R){}

////////////////////////////////////////////////////

	void					OnCameraChange		(int type);
	


	sound					snd_engine;

	bool					HUDview				( ) { return IsFocused(); }
	
	void					ActivateJeep		();
	void					CreateShell			();

	static void __stdcall	cb_WheelFL			(CBoneInstance* B);
	static void __stdcall	cb_WheelFR			(CBoneInstance* B);
	static void __stdcall	cb_WheelBL			(CBoneInstance* B);
	static void __stdcall	cb_WheelBR			(CBoneInstance* B);
	static void __stdcall	cb_Steer			(CBoneInstance* B);
	virtual void Hit(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse);
	virtual void PhDataUpdate(dReal step);
	virtual void PhTune(dReal step);
	virtual void InitContact(dContact*c){};
	virtual void StepFrameUpdate(dReal step){};

public:
	void					GetVelocity			(Fvector& vel)	{vel.set(m_jeep.GetVelocity());}
	void					cam_Update			(float dt);
	void					detach_Actor		();
	bool					attach_Actor		(CActor* actor);
	bool					is_Door				(int id);
	// Core events
	virtual void			Load				( LPCSTR section );
	virtual BOOL			net_Spawn			( LPVOID DC );
	virtual void			Update				( u32 T ); 
	virtual void			UpdateCL			( ); 
	virtual void			OnVisible			( ); 
	
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
