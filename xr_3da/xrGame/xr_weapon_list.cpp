#include "stdafx.h"
#include "..\render.h"
#include "..\bodyinstance.h"
#include "..\fmesh.h"

#include "xr_weapon_list.h"
#include "entity.h"

// Factory
#include "WeaponM134.h"
#include "WeaponFN2000.h"
#include "WeaponAK74.h"
#include "WeaponLR300.h"
#include "WeaponHPSA.h"

class fClassEQ {
	CLASS_ID cls;
public:
	fClassEQ(CLASS_ID C) : cls(C) {};
	IC BOOL operator() (CWeapon* W) { return cls==W->SUB_CLS_ID; }
};

//--------------------------------------------------------------------------
CWeaponList::CWeaponList	(CEntity* p)
{
	m_pParent			= p;
	R_ASSERT(m_pParent->Visual()->Type==MT_SKELETON);

	m_iActiveWeapon		= -1;
	m_iSelectedWeapon	= -1;
	m_iHUDboneL			= -1;
	m_iHUDboneR			= -1;
	m_iACTboneL			= -1;
	m_iACTboneR			= -1;
	m_bZoomed			= FALSE;
}

CWeaponList::~CWeaponList	( )
{
	for (WeaponIt w_it=m_Weapons.begin(); w_it!=m_Weapons.end(); w_it++)
		_DELETE(*w_it);
	m_Weapons.clear();
}

#define HUD_BONE_NAME_R "bip01_r_hand"
#define HUD_BONE_NAME_L "bip01_l_finger1"
#define ACT_BONE_NAME_R "bip01_r_hand"
#define ACT_BONE_NAME_L "bip01_l_finger1"

void CWeaponList::Init(LPCSTR act_bone_r, LPCSTR act_bone_l)
{
	m_iHUDboneL		= -1;
	m_iHUDboneR		= -1;
	CKinematics* V	= PKinematics(m_pParent->Visual());
	m_iACTboneL		= V->LL_BoneID(act_bone_l);
	m_iACTboneR		= V->LL_BoneID(act_bone_r);
}

void CWeaponList::FireStart()
{ 
	if (m_iActiveWeapon==-1) return;
	CWeapon* W = m_Weapons[m_iActiveWeapon];
	W->FireStart();
}

void CWeaponList::FireEnd()
{ 
	if (m_iActiveWeapon==-1) return;
	CWeapon* W = m_Weapons[m_iActiveWeapon];
	W->FireEnd();
}

BOOL CWeaponList::isWorking()
{
	if (m_iActiveWeapon==-1) return false;
	CWeapon* W = m_Weapons[m_iActiveWeapon];
	return W->IsWorking();
}

BOOL CWeaponList::WeaponChange(int idx)
{
	// Analyze desired ID
	if (idx==m_iActiveWeapon)											return false;
	if ((idx<0) || (idx>=int(m_Weapons.size())) || (0==m_Weapons[idx]))	return false;

	// Stop current weapon
	if (m_iActiveWeapon>=0)	m_Weapons[m_iActiveWeapon]->Hide	();

	// Select new
	m_iSelectedWeapon	= idx;
	Zoom				(FALSE);
	return true;
}

BOOL CWeaponList::ActivateWeaponNext(BOOL ignore)
{
	if (m_Weapons.size()>1){
		int wpn_count=m_Weapons.size();
		for (int Q=0, cur_wpn = m_iActiveWeapon+1;cur_wpn<=wpn_count; cur_wpn++, Q++){
			if (cur_wpn>=wpn_count)cur_wpn=0;
			if (ignore||m_Weapons[cur_wpn]->IsValid()) break;
			if (Q>=wpn_count) return false;
		}
		return WeaponChange(cur_wpn);	
	}
	return false;
}

int	CWeaponList::FindWeapon(CLASS_ID cls)
{
	WeaponIt W=find_if(m_Weapons.begin(),m_Weapons.end(),fClassEQ(cls));
	if (W!=m_Weapons.end()) return W-m_Weapons.begin();
	return -1;
}

BOOL CWeaponList::ActivateWeapon(CLASS_ID cls)
{
	if (!m_Weapons.empty()){
		int idx = FindWeapon(cls);
		if (idx>=0){
			return WeaponChange(idx);
		}
	}
	return false;
}

BOOL CWeaponList::ActivateWeaponID(int id)
{
	return WeaponChange(id);
}

void CWeaponList::Zoom(BOOL bZoom)
{
	if (bZoom)	{
		CWeapon*	W = ActiveWeapon();
		if (W && W->HasOpticalAim())	m_bZoomed = TRUE;
	} else {
		m_bZoomed	= FALSE;
	}
}
void CWeaponList::Reload()
{
	CWeapon*	W = ActiveWeapon();
	if (W)		W->Reload		();
}

CWeapon* CWeaponList::LoadOne( CLASS_ID cls )
{
	string128	sect_name;
	CWeapon*	pWeapon	= NULL;
	switch (cls)
	{
	case CLSID_OBJECT_W_M134:
		pWeapon = new CWeaponM134(); 
		strcpy(sect_name,"wpn_m134");
		break;
	case CLSID_OBJECT_W_M134_en:
		pWeapon = new CWeaponM134(); 
		strcpy(sect_name,"wpn_m134en");
		break;
	case CLSID_OBJECT_W_FN2000:
		pWeapon = new CWeaponFN2000	(); 
		strcpy(sect_name,"wpn_fn2000");
		break;
	case CLSID_OBJECT_W_AK74:
		pWeapon = new CWeaponAK74	(); 
		strcpy(sect_name,"wpn_ak74");
		break;
	case CLSID_OBJECT_W_LR300:
		pWeapon = new CWeaponLR300	(); 
		strcpy(sect_name,"wpn_lr300");
		break;
	case CLSID_OBJECT_W_HPSA:
		pWeapon = new CWeaponHPSA	(); 
		strcpy(sect_name,"wpn_HPSA");
		break;
	}

	// load weapon
	R_ASSERT			(pWeapon);
	pWeapon->SUB_CLS_ID = cls;
	pWeapon->Load		(pSettings,sect_name);
	pWeapon->SetParent	(m_pParent,this);
	m_Weapons.push_back	(pWeapon);
	return pWeapon;
}

BOOL CWeaponList::TakeItem(CLASS_ID cls, int iAmmoCount)
{
	int idx				= -1;
	BOOL bTakeWeapon	= false;
	switch (cls)
	{
	case CLSID_OBJECT_A_M134:		idx = FindWeapon(CLSID_OBJECT_W_M134);		break;
	case CLSID_OBJECT_A_FN2000:		idx = FindWeapon(CLSID_OBJECT_W_FN2000);	break;
	case CLSID_OBJECT_A_AK74:		idx = FindWeapon(CLSID_OBJECT_W_AK74);		break;
	case CLSID_OBJECT_A_LR300:		idx = FindWeapon(CLSID_OBJECT_W_LR300);		break;
	case CLSID_OBJECT_A_HPSA:		idx = FindWeapon(CLSID_OBJECT_W_HPSA);		break;

	case CLSID_OBJECT_W_M134:	
	case CLSID_OBJECT_W_FN2000:	
	case CLSID_OBJECT_W_AK74:
	case CLSID_OBJECT_W_LR300:	
	case CLSID_OBJECT_W_M134_en:	
	case CLSID_OBJECT_W_HPSA:		idx = FindWeapon(cls); bTakeWeapon = true; break;
	}
	if (idx>=0)
	{
		// add ammo
		return m_Weapons[idx]->AddAmmo(iAmmoCount);
	}else{
		if (bTakeWeapon){
			// add weapon
			CWeapon* wpn = LoadOne(cls);
			wpn->AddAmmo(iAmmoCount);
			if (m_Weapons.size()==1) ActivateWeapon(cls);
			return true;
		}
	}
	return false;
}

void CWeaponList::LeaveWeapon(CLASS_ID cls)
{
}

void CWeaponList::Update(float dt, BOOL bHUDView)
{
	// Change weapon if needed and can be done
	if (m_iSelectedWeapon>=0)	
	{
		if ((m_iActiveWeapon<0) || (!m_Weapons[m_iActiveWeapon]->IsVisible()))
		{
			m_iActiveWeapon		= m_iSelectedWeapon;
			m_Weapons[m_iActiveWeapon]->Show	();
		}
	}

	// Update all needed weapons
	for (DWORD it=0; it<m_Weapons.size(); it++)
		if (m_Weapons[it]->IsUpdating())	m_Weapons[it]->Update(dt,bHUDView);
}

void CWeaponList::OnRender(BOOL bHUDView)
{
	if (m_iActiveWeapon==-1) return;
	m_Weapons[m_iActiveWeapon]->Render(bHUDView);
}

void CWeaponList::GetFireParams(Fvector &fire_pos, Fvector &fire_dir){
	if (m_iActiveWeapon==-1) return;
	CWeapon* W		= m_Weapons[m_iActiveWeapon];
	fire_pos.set	(W->Position());
	fire_dir.set	(Device.vCameraDirection);
}
