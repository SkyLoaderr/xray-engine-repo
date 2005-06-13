#include "stdafx.h"
#include "CarWeapon.h"
#include "PhysicsShell.h"
#include "PhysicsShellHolder.h"
#include "../skeletoncustom.h"
#include "object_broker.h"
#include "ai_sounds.h"
#include "weaponAmmo.h"
#include "xr_level_controller.h"
#include "game_object_space.h"

void __stdcall	CCarWeapon::BoneCallbackX		(CBoneInstance *B)
{
	CCarWeapon	*P = static_cast<CCarWeapon*>(B->Callback_Param);
	Fmatrix rX;		rX.rotateX		(P->m_cur_x_rot);
	B->mTransform.mulB(rX);
}

void __stdcall	CCarWeapon::BoneCallbackY		(CBoneInstance *B)
{
	CCarWeapon	*P = static_cast<CCarWeapon*>(B->Callback_Param);
	Fmatrix rY;		rY.rotateY		(P->m_cur_y_rot);
	B->mTransform.mulB(rY);
}

CCarWeapon::CCarWeapon(CPhysicsShellHolder* obj)
{
	m_object	= obj;
	m_Ammo		= xr_new<CCartridge>();

	CKinematics* K			= smart_cast<CKinematics*>(m_object->Visual());
	CInifile* pUserData		= K->LL_UserData(); 

	m_rotate_x_bone			= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","rotate_x_bone"));
	m_rotate_y_bone			= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","rotate_y_bone"));
	m_fire_bone				= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","fire_bone"));

	CBoneData& bdX			= K->LL_GetData(m_rotate_x_bone); //VERIFY(bdX.IK_data.type==jtJoint);
	m_lim_x_rot.set			(bdX.IK_data.limits[0].limit.x,bdX.IK_data.limits[0].limit.y);
	CBoneData& bdY			= K->LL_GetData(m_rotate_y_bone); //VERIFY(bdY.IK_data.type==jtJoint);
	m_lim_y_rot.set			(bdY.IK_data.limits[1].limit.x,bdY.IK_data.limits[1].limit.y);
	

	xr_vector<Fmatrix>					matrices;
	K->LL_GetBindTransform				(matrices);
	m_i_bind_x_xform.invert				(matrices[m_rotate_x_bone]);
	m_i_bind_y_xform.invert				(matrices[m_rotate_y_bone]);
	m_bind_x_rot						= matrices[m_rotate_x_bone].k.getP();
	m_bind_y_rot						= matrices[m_rotate_y_bone].k.getH();
	m_bind_x.set						(matrices[m_rotate_x_bone].c);
	m_bind_y.set						(matrices[m_rotate_y_bone].c);

	m_cur_x_rot							= m_bind_x_rot;
	m_cur_y_rot							= m_bind_y_rot;
	m_destEnemyDir.setHP				(m_bind_y_rot,m_bind_x_rot);
	m_object->XFORM().transform_dir		(m_destEnemyDir);

	inheritedShooting::Light_Create		();
//	Load								(pUserData->r_string("mounted_weapon_definition","wpn_section"));
	Load								("stationary_mgun");
	SetBoneCallbacks					();
	m_object->processing_activate		();
}

CCarWeapon::~CCarWeapon()
{
	delete_data(m_Ammo);
	m_object->processing_deactivate		();
}

void CCarWeapon::Load(LPCSTR section)
{
	inheritedShooting::Load(section);
	HUD_SOUND::LoadSound(section,"snd_shoot", m_sndShot, TRUE, SOUND_TYPE_WEAPON_SHOOTING);
	m_Ammo->Load(pSettings->r_string(section, "ammo_class"));
}

void CCarWeapon::UpdateCL()
{
	UpdateBarrelDir				();
	UpdateFire					();
}

void CCarWeapon::UpdateFire()
{
	fTime -= Device.fTimeDelta;

	inheritedShooting::UpdateFlameParticles();
	inheritedShooting::UpdateLight();
//	if(m_allow_fire){
//		FireStart();
//	}else
//		FireEnd();

	if(!IsWorking()){
		if(fTime<0) fTime = 0.f;
		return;
	}

	if(fTime<=0){
		OnShot();
		fTime += fTimeToFire;
	}
}

void CCarWeapon::SetBoneCallbacks()
{
	m_object->PPhysicsShell()->EnabledCallbacks(FALSE);
	
	CBoneInstance& biX		= smart_cast<CKinematics*>(m_object->Visual())->LL_GetBoneInstance(m_rotate_x_bone);	
	biX.set_callback		(bctCustom,BoneCallbackX,this);
	CBoneInstance& biY		= smart_cast<CKinematics*>(m_object->Visual())->LL_GetBoneInstance(m_rotate_y_bone);	
	biY.set_callback		(bctCustom,BoneCallbackY,this);
}

void CCarWeapon::ResetBoneCallbacks()
{
	CBoneInstance& biX		= smart_cast<CKinematics*>(m_object->Visual())->LL_GetBoneInstance(m_rotate_x_bone);	
	biX.set_callback		(bctDummy,0,0);
	CBoneInstance& biY		= smart_cast<CKinematics*>(m_object->Visual())->LL_GetBoneInstance(m_rotate_y_bone);	
	biY.set_callback		(bctDummy,0,0);

	m_object->PPhysicsShell()->EnabledCallbacks(TRUE);
}

void CCarWeapon::UpdateBarrelDir()
{
	CKinematics* K		= smart_cast<CKinematics*>(m_object->Visual());
	m_fire_bone_xform	= K->LL_GetTransform(m_fire_bone);

	m_fire_bone_xform.mulA(m_object->XFORM());
	m_fire_pos.set(0,0,0); 
	m_fire_bone_xform.transform_tiny(m_fire_pos);
	m_fire_dir.set(0,0,1);
	m_fire_bone_xform.transform_dir(m_fire_dir);


	m_allow_fire		= true;
	Fmatrix XFi;
	XFi.invert			(m_object->XFORM());
	Fvector dep;
	XFi.transform_dir	(dep,m_destEnemyDir);
	{// x angle
		m_i_bind_x_xform.transform_dir(dep); dep.normalize();
		m_tgt_x_rot		= angle_normalize_signed(m_bind_x_rot-dep.getP());
		float sv_x		= m_tgt_x_rot;
		
		clamp			(m_tgt_x_rot,-m_lim_x_rot.y,-m_lim_x_rot.x);
		if (!fsimilar(sv_x,m_tgt_x_rot,EPS_L)) m_allow_fire=FALSE;
	}
	{// y angle
		m_i_bind_y_xform.transform_dir(dep); dep.normalize();
		m_tgt_y_rot		= angle_normalize_signed(m_bind_y_rot-dep.getH());
		float sv_y		= m_tgt_y_rot;
		clamp			(m_tgt_y_rot,-m_lim_y_rot.y,-m_lim_y_rot.x);
		if (!fsimilar(sv_y,m_tgt_y_rot,EPS_L)) m_allow_fire=FALSE;
	}
	static float f1 = 0.5f;
	static float f2 = 1.5f;
	static float f3 = PI_MUL_2;

	m_cur_x_rot		= angle_inertion_var(m_cur_x_rot,m_tgt_x_rot,f1,f2,f3,Device.fTimeDelta);
	m_cur_y_rot		= angle_inertion_var(m_cur_y_rot,m_tgt_y_rot,f1,f2,f3,Device.fTimeDelta);

#if (0)
	if(Device.dwFrame%200==0){
		Msg("m_cur_x_rot=[%f]",m_cur_x_rot);
		Msg("m_cur_y_rot=[%f]",m_cur_y_rot);
	}
#endif
}

const Fvector&	CCarWeapon::get_CurrentFirePoint()
{
	return m_fire_pos;
}

const Fmatrix&	CCarWeapon::get_ParticlesXFORM	()						
{
	return m_fire_bone_xform;
}

void CCarWeapon::FireStart()
{
	inheritedShooting::FireStart();
}

void CCarWeapon::FireEnd()	
{
	inheritedShooting::FireEnd();
	StopFlameParticles	();
}

void CCarWeapon::OnShot()
{
	FireBullet				(	m_fire_pos, m_fire_dir, fireDispersionBase, *m_Ammo, 
								m_object->ID(), m_object->ID(), SendHitAllowed(m_object));

	StartShotParticles		();
	
	if(m_bShotLight) 
		Light_Start			();

	StartFlameParticles		();
	StartSmokeParticles		(m_fire_pos, zero_vel);
//	OnShellDrop				(m_fire_pos, zero_vel);

	HUD_SOUND::PlaySound	(m_sndShot, m_fire_pos, m_object, false);
}

void CCarWeapon::Action				(int id, u32 flags)
{
	switch (id){
		case kWPN_FIRE:{
			if(flags==CMD_START)	FireStart	();
			else					FireEnd		();
		}break;
	}
}

void CCarWeapon::SetParam			(int id, Fvector2 val)
{
	switch (id){
		case DESIRED_DIR:
			m_destEnemyDir.setHP				(val.x,val.y);
		break;
	}
}

void CCarWeapon::SetParam			(int id, Fvector val)
{
	switch (id){
		case DESIRED_POS:
			m_destEnemyDir.sub(val,m_fire_pos).normalize_safe();
		break;
	}
}
