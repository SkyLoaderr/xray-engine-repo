#include "stdafx.h"
#include "WeaponStatMgun.h"
#include "../skeletoncustom.h"
#include "PhysicsShell.h"
#include "weaponAmmo.h"
#include "object_broker.h"

void __stdcall	CWeaponStatMgun::BoneCallbackX		(CBoneInstance *B)
{
	CWeaponStatMgun	*P = static_cast<CWeaponStatMgun*>(B->Callback_Param);
}

void __stdcall	CWeaponStatMgun::BoneCallbackY		(CBoneInstance *B)
{
	CWeaponStatMgun	*P = static_cast<CWeaponStatMgun*>(B->Callback_Param);
}

CWeaponStatMgun::CWeaponStatMgun()
{
	m_Ammo = xr_new<CCartridge>();
}

CWeaponStatMgun::~CWeaponStatMgun()
{
	delete_data(m_Ammo);
}

void CWeaponStatMgun::SetBoneCallbacks()
{
	m_pPhysicsShell->EnabledCallbacks(FALSE);
	
	CBoneInstance& biX		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(rotate_x_bone);	
	biX.set_callback		(BoneCallbackX,this);
	CBoneInstance& biY		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(rotate_y_bone);	
	biY.set_callback		(BoneCallbackY,this);
}

void CWeaponStatMgun::ResetBoneCallbacks()
{
	CBoneInstance& biX		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(rotate_x_bone);	
	biX.set_callback		(0,0);
	CBoneInstance& biY		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(rotate_y_bone);	
	biY.set_callback		(0,0);

	m_pPhysicsShell->EnabledCallbacks(TRUE);
}

void CWeaponStatMgun::Load(LPCSTR section)
{
	inheritedPH::Load(section);
	inheritedShooting::Load	(section);

	m_Ammo->Load(pSettings->r_string(section, "ammo_class"));
}

BOOL CWeaponStatMgun::net_Spawn(CSE_Abstract* DC)
{
	if(!inheritedPH::net_Spawn	(DC)) return FALSE;



	CKinematics* K			= smart_cast<CKinematics*>(Visual());
	CInifile* pUserData		= K->LL_UserData(); 

	R_ASSERT2				(pUserData,"Empty WeaponStatMgun user data!");

	rotate_x_bone			= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","rotate_x_bone"));
	rotate_y_bone			= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","rotate_y_bone"));


	U16Vec fixed_bones;
	fixed_bones.push_back	(K->LL_GetBoneRoot());
	PPhysicsShell()			= P_build_Shell(this,false,fixed_bones);


	setVisible						(true);
	setEnabled						(true);
	return							TRUE;
}

void CWeaponStatMgun::net_Destroy()
{
	inheritedPH::net_Destroy		();
}

void CWeaponStatMgun::net_Export(NET_Packet& P)	// export to server
{
	inheritedPH::net_Export			(P);
}

void CWeaponStatMgun::net_Import			(NET_Packet& P)	// import from server
{
	inheritedPH::net_Import			(P);
}

void CWeaponStatMgun::UpdateCL()
{
	inheritedPH::UpdateCL			();
}
