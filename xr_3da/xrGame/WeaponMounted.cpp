#include "stdafx.h"
#pragma hdrstop

#include "WeaponMounted.h"
#include "xrServer_Objects_ALife.h"
#include "camerafirsteye.h"
#include "actor.h"
#include "weaponammo.h"


#include "actoreffector.h"
#include "effectorshot.h"
#include "ai_sounds.h"
#include "level.h"
#include "xr_level_controller.h"


//----------------------------------------------------------------------------------------

void __stdcall CWeaponMounted::BoneCallbackX(CBoneInstance *B)
{
	CWeaponMounted	*P = static_cast<CWeaponMounted*>(B->Callback_Param);

	if (P->Owner()){
		Fmatrix rX;		rX.rotateX		(P->camera->pitch);
		B->mTransform.mulB(rX);
	}
}

void __stdcall CWeaponMounted::BoneCallbackY(CBoneInstance *B)
{
	CWeaponMounted	*P = static_cast<CWeaponMounted*>(B->Callback_Param);

	if (P->Owner()){
		Fmatrix rY;		rY.rotateY		(P->camera->yaw);
		B->mTransform.mulB(rY);
	}
}
//----------------------------------------------------------------------------------------

CWeaponMounted::CWeaponMounted()
{
	camera		= xr_new<CCameraFirstEye>	(this, pSettings, "mounted_weapon_cam",	CCameraBase::flRelativeLink|CCameraBase::flPositionRigid|CCameraBase::flDirectionRigid); 
}

CWeaponMounted::~CWeaponMounted()
{
}

void	CWeaponMounted::Load(LPCSTR section)
{
	inherited::Load(section);
	CShootingObject::Load	(section);

	HUD_SOUND::LoadSound(section,"snd_shoot", sndShot, TRUE, SOUND_TYPE_WEAPON_SHOOTING);

	//тип используемых патронов
	m_sAmmoType = pSettings->r_string(section, "ammo_class");
	m_CurrentAmmo.Load(*m_sAmmoType);

	//подбрасывание камеры во время отдачи
	camMaxAngle			= pSettings->r_float		(section,"cam_max_angle"	); 
	camMaxAngle			= deg2rad					(camMaxAngle);
	camRelaxSpeed		= pSettings->r_float		(section,"cam_relax_speed"	); 
	camRelaxSpeed		= deg2rad					(camRelaxSpeed);

}

BOOL	CWeaponMounted::net_Spawn(LPVOID DC)
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMountedWeapon	*mw	= smart_cast<CSE_ALifeMountedWeapon*>(e);
	R_ASSERT				(mw);

	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	R_ASSERT				(Visual() && smart_cast<CKinematics*>(Visual()));

	CKinematics* K			= smart_cast<CKinematics*>(Visual());
	CInifile* pUserData		= K->LL_UserData(); 

	R_ASSERT3				(pUserData,"Empty MountedWeapon user data!",mw->get_visual());

	fire_bone				= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","fire_bone"));
	actor_bone				= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","actor_bone"));
	rotate_x_bone			= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","rotate_x_bone"));
	rotate_y_bone			= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","rotate_y_bone"));
	camera_bone				= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","camera_bone"));

	CBoneData& bdX			= K->LL_GetData(rotate_x_bone); VERIFY(bdX.IK_data.type==jtJoint);
	camera->lim_pitch.set	(bdX.IK_data.limits[0].limit.x,bdX.IK_data.limits[0].limit.y);
	CBoneData& bdY			= K->LL_GetData(rotate_y_bone); VERIFY(bdY.IK_data.type==jtJoint);
	camera->lim_yaw.set		(bdY.IK_data.limits[1].limit.x,bdY.IK_data.limits[1].limit.y);

	U16Vec fixed_bones;
	fixed_bones.push_back	(K->LL_GetBoneRoot());
	PPhysicsShell()			= P_build_Shell(this,false,fixed_bones);


	CShootingObject::Light_Create();

	setVisible	(true);
	setEnabled	(true);



	return TRUE;
}

void	CWeaponMounted::net_Destroy()
{
	CShootingObject::Light_Destroy();

	inherited::net_Destroy();
	xr_delete(m_pPhysicsShell);
}

void	CWeaponMounted::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);
}

void	CWeaponMounted::net_Import(NET_Packet& P)
{
	inherited::net_Import(P);
}

void	CWeaponMounted::UpdateCL()
{
	inherited::UpdateCL	();
	if (Owner()){
		CKinematics* K		= smart_cast<CKinematics*>(Visual());
		K->CalculateBones	();
		// update fire pos & fire_dir
		fire_bone_xform		= K->LL_GetTransform(fire_bone);
		fire_bone_xform.mulA(XFORM());
		fire_pos.set		(0,0,0); 
		fire_bone_xform.transform_tiny	(fire_pos);
		fire_dir.set		(0,0,1); 
		fire_bone_xform.transform_dir	(fire_dir);

		UpdateFire			();
	}
}

void	CWeaponMounted::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
}

void	CWeaponMounted::renderable_Render()
{
	//нарисовать подсветку
	RenderLight();

	inherited::renderable_Render	();
}

void	CWeaponMounted::OnMouseMove			(int dx, int dy)
{
	if (Remote())	return;

	CCameraBase* C	= camera;
	float scale		= (C->f_fov/DEFAULT_FOV)*psMouseSens * psMouseSensScale/50.f;
	if (dx){
		float d		= float(dx)*scale;
		C->Move		((d<0)?kLEFT:kRIGHT, _abs(d));
	}
	if (dy){
		float d		= ((psMouseInvert.test(1))?-1:1)*float(dy)*scale*3.f/4.f;
		C->Move		((d>0)?kUP:kDOWN, _abs(d));
	}
}
void	CWeaponMounted::OnKeyboardPress		(int dik)
{
	if (Remote())							return;

	switch (dik)	
	{
	case kWPN_FIRE:					
		FireStart();
		break;
	};

}
void	CWeaponMounted::OnKeyboardRelease	(int dik)
{
	if (Remote())							return;
	switch (dik)	
	{
	case kWPN_FIRE:
		FireEnd();
		break;
	};
}
void	CWeaponMounted::OnKeyboardHold		(int dik)
{
	if (Remote())							return;

//	switch(dik)
//	{
//	}
}

void	CWeaponMounted::cam_Update			(float dt)
{
	Fvector							P,Da;
	Da.set							(0,0,0);
	if(Owner())	Owner()->setEnabled	(false);

	CKinematics* K					= smart_cast<CKinematics*>(Visual());
	K->CalculateBones				();
	const Fmatrix& C				= K->LL_GetTransform(camera_bone);
	XFORM().transform_tiny			(P,C.c);

	// rotate head
	Owner()->Orientation().yaw		= -camera->yaw;
	Owner()->Orientation().pitch	= -camera->pitch;

	camera->Update					(P,Da);
	Level().Cameras.Update			(camera);

	if(Owner())	Owner()->setEnabled	(true);
}

bool	CWeaponMounted::Use					(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos)
{
	return !Owner();
}
bool	CWeaponMounted::attach_Actor		(CActor* actor)
{
	CHolderCustom::attach_Actor(actor);
	CKinematics* K		= smart_cast<CKinematics*>(Visual());
	// убрать оружие из рук	
	// disable shell callback
	m_pPhysicsShell->EnabledCallbacks(FALSE);
	// enable actor rotate callback
	CBoneInstance& biX		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(rotate_x_bone);	
	biX.set_callback		(BoneCallbackX,this);
	CBoneInstance& biY		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(rotate_y_bone);	
	biY.set_callback		(BoneCallbackY,this);
	// set actor to mounted position
	const Fmatrix& A	= K->LL_GetTransform(actor_bone);
	Fvector ap;
	XFORM().transform_tiny	(ap,A.c);
	Fmatrix AP; AP.translate(ap);
	Owner()->SetPhPosition	(AP);
	processing_activate		();
	return true;
}
void	CWeaponMounted::detach_Actor		()
{
	CHolderCustom::detach_Actor();
	// disable actor rotate callback
	CBoneInstance& biX		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(rotate_x_bone);	
	biX.set_callback		(0,0);
	CBoneInstance& biY		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(rotate_y_bone);	
	biY.set_callback		(0,0);
	// enable shell callback
	m_pPhysicsShell->EnabledCallbacks(TRUE);
	
	//закончить стрельбу
	FireEnd();

	processing_deactivate		();
}

Fvector	CWeaponMounted::ExitPosition		()
{
	return XFORM().c;
}

CCameraBase*	CWeaponMounted::Camera				()
{
	return camera;
}


void CWeaponMounted::FireStart()
{
	CShootingObject::FireStart();
}
void CWeaponMounted::FireEnd()
{
	CShootingObject::FireEnd();
	StopFlameParticles	();
	RemoveShotEffector ();
}


void CWeaponMounted::OnShot		()
{
	VERIFY(Owner());

	FireBullet(CurrentFirePoint(),fire_dir, 
		fireDispersionBase,
		m_CurrentAmmo, Owner()->ID(),ID());

	StartShotParticles	();
	if(m_bShotLight) Light_Start();

	StartFlameParticles();
	StartSmokeParticles(fire_pos, zero_vel);
	OnShellDrop(fire_pos, zero_vel);

	bool hud_mode = (Level().CurrentEntity() == smart_cast<CObject*>(Owner()));
	HUD_SOUND::PlaySound(sndShot, fire_pos, Owner(), hud_mode);

	//добавить эффектор стрельбы
	AddShotEffector		();
}

void CWeaponMounted::UpdateFire()
{
	fTime -= Device.fTimeDelta;
	

	CShootingObject::UpdateFlameParticles();
	CShootingObject::UpdateLight();

	if(!IsWorking()) 
	{
		if(fTime<0) fTime = 0.f;
		return;
	}

	if(fTime<=0)
	{
		OnShot();
		fTime += fTimeToFire;
	}

}

const Fmatrix&	 CWeaponMounted::ParticlesXFORM() const	
{
	return fire_bone_xform;
}

void CWeaponMounted::AddShotEffector		()
{
	if(Owner())
	{
		CEffectorShot* S		= smart_cast<CEffectorShot*>	(Owner()->EffectorManager().GetEffector(eCEShot)); 
		if (!S)	S				= (CEffectorShot*)Owner()->EffectorManager().AddEffector(xr_new<CEffectorShot> (camMaxAngle,camRelaxSpeed, 0.25f, 0.01f));
		R_ASSERT				(S);
		S->MountedWeaponShot	();
	}
}

void  CWeaponMounted::RemoveShotEffector	()
{
	if(Owner())
		Owner()->EffectorManager().RemoveEffector	(eCEShot);
}