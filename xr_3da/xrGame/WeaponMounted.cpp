#include "stdafx.h"
#pragma hdrstop

#include "WeaponMounted.h"
#include "xrServer_Objects_ALife.h"
#include "camerafirsteye.h"
#include "actor.h"
//----------------------------------------------------------------------------------------

void __stdcall CWeaponMounted::BoneCallbackX(CBoneInstance *B)
{
	CWeaponMounted	*P = dynamic_cast<CWeaponMounted*> (static_cast<CObject*>(B->Callback_Param));

	if (P->Owner()){
//		CKinematics* K	= PKinematics	(P->Visual());
		Fmatrix rX;		rX.rotateX		(P->camera->pitch);
		B->mTransform.mulB(rX);
	}
}
void __stdcall CWeaponMounted::BoneCallbackY(CBoneInstance *B)
{
	CWeaponMounted	*P = dynamic_cast<CWeaponMounted*> (static_cast<CObject*>(B->Callback_Param));

	if (P->Owner()){
//		CKinematics* K	= PKinematics	(P->Visual());
		Fmatrix rY;		rY.rotateY		(P->camera->yaw);
		B->mTransform.mulB(rY);
	}
}
//----------------------------------------------------------------------------------------

CWeaponMounted::CWeaponMounted()
{
	camera		= xr_new<CCameraFirstEye>	(this, pSettings, "mounted_weapon_cam",	CCameraBase::flRelativeLink|CCameraBase::flPositionRigid|CCameraBase::flDirectionRigid); 
	m_bFiring	= false;
}

CWeaponMounted::~CWeaponMounted()
{
}

void	CWeaponMounted::Load(LPCSTR section)
{
	inherited::Load(section);

	CShootingObject::Load	(section);
}

BOOL	CWeaponMounted::net_Spawn(LPVOID DC)
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMountedWeapon	*mw	= dynamic_cast<CSE_ALifeMountedWeapon*>(e);
	R_ASSERT				(mw);

	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	R_ASSERT				(Visual() && PKinematics(Visual()));

	CKinematics* K			= PKinematics(Visual());
	CInifile* pUserData		= K->LL_UserData(); 

	R_ASSERT3				(pUserData,"Empty MountedWeapon user data!",mw->get_visual());

	fire_bone				= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","fire_bone"));
	actor_bone				= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","actor_bone"));
	rotate_x_bone			= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","rotate_x_bone"));
	rotate_y_bone			= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","rotate_y_bone"));
	camera_bone				= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","camera_bone"));

	CBoneData& bdX			= K->LL_GetData(rotate_x_bone); VERIFY(bdX.IK_data.type==jtJoint);
	camera->lim_pitch.set	(bdX.IK_data.limits[0].limit.x,bdX.IK_data.limits[0].limit.y);
	CBoneInstance& biX		= PKinematics(Visual())->LL_GetBoneInstance(rotate_x_bone);	
	biX.set_callback		(BoneCallbackX,this);
	CBoneData& bdY			= K->LL_GetData(rotate_y_bone); VERIFY(bdY.IK_data.type==jtJoint);
	camera->lim_yaw.set		(bdY.IK_data.limits[1].limit.x,bdY.IK_data.limits[1].limit.y);
	camera->lim_yaw.set		(0,0);
	CBoneInstance& biY		= PKinematics(Visual())->LL_GetBoneInstance(rotate_y_bone);	
	biY.set_callback		(BoneCallbackY,this);

	//P_BuildShell

	setVisible	(true);
	setEnabled	(true);

	return TRUE;
}

void	CWeaponMounted::net_Destroy()
{
}

void	CWeaponMounted::net_Export(NET_Packet& P)
{
}

void	CWeaponMounted::net_Import(NET_Packet& P)
{
}

void	CWeaponMounted::UpdateCL()
{
	inherited::UpdateCL	();
	if (Owner()){
		CKinematics* K	= PKinematics(Visual());
		K->Calculate	();
		// update fire pos & fire_dir
		Fmatrix mFP		= K->LL_GetTransform(fire_bone);
		mFP.mulB		(XFORM());
		fire_pos.set	(0,0,0); mFP.transform_tiny	(fire_pos);
		fire_dir.set	(0,0,1); mFP.transform_dir	(fire_dir);
	}
}

void	CWeaponMounted::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
}

void	CWeaponMounted::renderable_Render()
{
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
		m_bFiring	= true;
		break;
	};

}
void	CWeaponMounted::OnKeyboardRelease	(int dik)
{
	if (Remote())							return;
	switch (dik)	
	{
	case kWPN_FIRE:
		m_bFiring	= false;
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

	CKinematics* K					= PKinematics(Visual());
	K->Calculate					();
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
	CKinematics* K		= PKinematics(Visual());
	// убрать оружие из рук	
	// деактивировать шелл на пушке
	// destroy actor character
	// set actor to mounted position
	const Fmatrix& A	= K->LL_GetTransform(actor_bone);
	Fvector ap;
	XFORM().transform_tiny	(ap,A.c);
	Fmatrix AP; AP.translate(ap);
	Owner()->SetPhPosition	(AP);
	return true;
}
void	CWeaponMounted::detach_Actor		()
{
	CHolderCustom::detach_Actor();
}

Fvector	CWeaponMounted::ExitPosition		()
{
	return XFORM().c;
}

CCameraBase*	CWeaponMounted::Camera				()
{
	return camera;
}

