#include "stdafx.h"
#include "xr_weapon_list.h"
#include "entity.h"

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
	/*
	for (WeaponIt w_it=m_Weapons.begin(); w_it!=m_Weapons.end(); w_it++)
		_DELETE(*w_it);
	m_Weapons.clear();
	*/
}

#define HUD_BONE_NAME_R "bip01_r_hand"
#define HUD_BONE_NAME_L "bip01_l_finger1"
#define ACT_BONE_NAME_R "bip01_r_hand"
#define ACT_BONE_NAME_L "bip01_l_finger1"

void CWeaponList::Init	(LPCSTR act_bone_r, LPCSTR act_bone_l)
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

	// Zoom - off
	Zoom				(FALSE);

	// Stop current weapon
	if (m_iActiveWeapon>=0)	m_Weapons[m_iActiveWeapon]->Hide	();

	// Select new
	m_iSelectedWeapon	= idx;
	return true;
}

int	CWeaponList::weapon_add			(CWeapon* W)		// add, return index
{
	m_Weapons.push_back				(W);
	return m_Weapons.size()-1;
}
int	CWeaponList::weapon_remove		(CWeapon* W)		// remove, return last
{
	m_iActiveWeapon					= -1;
	m_iSelectedWeapon				= -1;
	WeaponVec::iterator	it			= find(m_Weapons.begin(),m_Weapons.end(),W);
	m_Weapons.erase					(it);
	return it-m_Weapons.begin();
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

BOOL CWeaponList::ActivateWeaponID(int id)
{
	return WeaponChange(id);
}

void CWeaponList::Zoom(BOOL bZoom)
{
	CWeapon*	W = ActiveWeapon();
	if (W && W->HasOpticalAim()){
		if (bZoom)	{
			m_bZoomed	= TRUE;
			W->OnZoomIn	();
		} else {
			m_bZoomed	= FALSE;
			W->OnZoomOut();
		}
	}
}
void CWeaponList::Reload()
{
	CWeapon*	W = ActiveWeapon();
	if (W)		W->Reload		();
}

/*
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
	case CLSID_OBJECT_A_PM:			idx = FindWeapon(CLSID_OBJECT_W_PM);		break;
	case CLSID_OBJECT_A_FORT:		idx = FindWeapon(CLSID_OBJECT_W_FORT);		break;

	case CLSID_OBJECT_W_M134:	
	case CLSID_OBJECT_W_FN2000:	
	case CLSID_OBJECT_W_AK74:
	case CLSID_OBJECT_W_LR300:	
	case CLSID_OBJECT_W_M134_en:	
	case CLSID_OBJECT_W_HPSA:		
	case CLSID_OBJECT_W_PM:		
	case CLSID_OBJECT_W_FORT:		
	case CLSID_OBJECT_W_BINOCULAR:	idx = FindWeapon(cls); bTakeWeapon = true; break;
	}
	if (idx>=0)
	{
		// add ammo
		return m_Weapons[idx]->AddAmmo(iAmmoCount);
	}else{
		if (bTakeWeapon){
			// add weapon
			// wpn->AddAmmo(iAmmoCount);
			if (m_Weapons.size()==1) ActivateWeapon(cls);
			return true;
		}
	}
	return false;
}

void CWeaponList::LeaveWeapon(CLASS_ID cls)
{
}
*/

#include <typeinfo.h>

CWeapon*	CWeaponList::getWeaponByWeapon(CWeapon* W)
{
	for (u32 it=0; it<m_Weapons.size(); it++)
	{
		CWeapon* T											= m_Weapons[it];
		if (typeid(*W).raw_name() == typeid(*T).raw_name())	return	T;
	}
	return 0;
}

void CWeaponList::Update	(float dt, BOOL bHUDView)
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

	// Setup HUD mode
	if (m_iActiveWeapon>=0)		m_Weapons[m_iActiveWeapon]->SetHUDmode(bHUDView);

	// Update all needed weapons
	/*
	for (DWORD it=0; it<m_Weapons.size(); it++)
		if (m_Weapons[it]->IsUpdating())	m_Weapons[it]->Update();
	*/
}

void CWeaponList::GetFireParams(Fvector &fire_pos, Fvector &fire_dir){
	if (m_iActiveWeapon==-1) return;
	CWeapon* W		= m_Weapons[m_iActiveWeapon];
	fire_pos.set	(W->Position());
	fire_dir.set	(Device.vCameraDirection);
}
