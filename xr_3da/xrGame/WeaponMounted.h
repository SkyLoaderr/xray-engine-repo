#ifndef WeaponMountedH
#define WeaponMountedH
#pragma once

#include "holder_custom.h"
#include "shootingobject.h"

#include "hudsound.h"
#include "weaponammo.h"
#include "physicsshellholder.h"

class CWeaponMounted :	public CPhysicsShellHolder, 
						public CHolderCustom,
						public CShootingObject
{
private:
	//////////////////////////////////////////////////////////////////////////
	//  General
	//////////////////////////////////////////////////////////////////////////
	typedef CPhysicsShellHolder inherited;
	CCameraBase*			camera;
	u16						fire_bone;
	u16						actor_bone;
	u16						rotate_x_bone;
	u16						rotate_y_bone;
	u16						camera_bone;

	Fvector					fire_pos, fire_dir;
	Fmatrix					fire_bone_xform;

	static void __stdcall	BoneCallbackX		(CBoneInstance *B);
	static void __stdcall	BoneCallbackY		(CBoneInstance *B);
public:
							CWeaponMounted		();
	virtual					~CWeaponMounted		();

	// for shooting object
	virtual const Fmatrix&	ParticlesXFORM() const;
	virtual IRender_Sector*	Sector()				{return CGameObject::Sector();}
	virtual const Fvector&	CurrentFirePoint()		{return fire_pos;}


	//////////////////////////////////////////////////
	// непосредственно обработка стрельбы
	//////////////////////////////////////////////////

protected:
	virtual	void			FireStart	();
	virtual	void			FireEnd		();
	virtual	void			UpdateFire	();
	virtual	void			OnShot		();
	virtual void			AddShotEffector		();
	virtual void			RemoveShotEffector	();
protected:
	shared_str					m_sAmmoType;
	CCartridge				m_CurrentAmmo;

	//звук стрельбы
	HUD_SOUND				sndShot;

	//для отдачи
	float					camRelaxSpeed;
	float					camMaxAngle;


	/////////////////////////////////////////////////
	// Generic
	/////////////////////////////////////////////////
public:
	virtual void			Load				(LPCSTR section);

	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(NET_Packet& P);	// export to server
	virtual void			net_Import			(NET_Packet& P);	// import from server

	virtual void			UpdateCL			();
	virtual void			shedule_Update		(u32 dt);

	virtual void			renderable_Render	();

	virtual	BOOL			UsedAI_Locations	(){return FALSE;}

	// control functions
	virtual void			OnMouseMove			(int x, int y);
	virtual void			OnKeyboardPress		(int dik);
	virtual void			OnKeyboardRelease	(int dik);
	virtual void			OnKeyboardHold		(int dik);

	virtual CInventory*		GetInventory		(){return 0;}

	virtual void			cam_Update			(float dt);

	virtual bool			Use					(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos);
	virtual bool			attach_Actor		(CActor* actor);
	virtual void			detach_Actor		();

	virtual Fvector			ExitPosition		();

	virtual CCameraBase*	Camera				();
};
#endif // WeaponMountedH
