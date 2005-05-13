#pragma once

#include "holder_custom.h"
#include "shootingobject.h"
#include "physicsshellholder.h"
class CCartridge;

class CWeaponStatMgun:	public CPhysicsShellHolder, 
						public CHolderCustom,
						public CShootingObject
{
private:
	typedef CPhysicsShellHolder inheritedPH;
	typedef CHolderCustom		inheritedHolder;
	typedef CShootingObject		inheritedShooting;

private:
	// 
	static void __stdcall	BoneCallbackX		(CBoneInstance *B);
	static void __stdcall	BoneCallbackY		(CBoneInstance *B);
	void					SetBoneCallbacks	();
	void					ResetBoneCallbacks	();
//casts
public:
	virtual CHolderCustom	*cast_holder_custom	()				{return this;}

//general
public:
							CWeaponStatMgun		();
	virtual					~CWeaponStatMgun	();

	virtual void			Load				(LPCSTR section);

	virtual BOOL			net_Spawn			(CSE_Abstract* DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(NET_Packet& P);	// export to server
	virtual void			net_Import			(NET_Packet& P);	// import from server

	virtual void			UpdateCL			();

//shooting
private:
	u16						rotate_x_bone, rotate_y_bone;
	CCartridge*				m_Ammo;

protected:
	virtual const Fvector&	get_CurrentFirePoint()						{return Fvector().set(0.0f,0.0f,0.0f);};
	virtual const Fmatrix&	get_ParticlesXFORM	()						{return XFORM();};

	virtual	void			FireStart			()						{};
	virtual	void			FireEnd				()						{};
	virtual	void			UpdateFire			()						{};
	virtual	void			OnShot				()						{};

//HolderCustom
public:
	virtual bool			Use					(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos) {return false;};
	virtual void			OnMouseMove			(int x, int y)			{};
	virtual void			OnKeyboardPress		(int dik)				{};
	virtual void			OnKeyboardRelease	(int dik)				{};
	virtual void			OnKeyboardHold		(int dik)				{};
	virtual CInventory*		GetInventory		()						{return NULL;};
	virtual void			cam_Update			(float dt, float fov=90.0f)				{};

	virtual bool			attach_Actor		(CActor* actor)			{return true;};
	virtual void			detach_Actor		()						{};
	virtual bool			allowWeapon			()	const				{return false;};
	virtual bool			HUDView				()	const				{return true;};
	virtual Fvector			ExitPosition		()						{return Fvector().set(0.0f,0.0f,0.0f);};

	virtual CCameraBase*	Camera				()						{return NULL;};




};