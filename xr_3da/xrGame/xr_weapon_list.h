#ifndef __XR_WEAPON_LIST_H__
#define __XR_WEAPON_LIST_H__

// refs
class ENGINE_API CInifile;
class CEntity;

#include "Weapon.h"
#include "WeaponHUD.h"

class				CWeaponList
{
	friend class	CWeapon;
private:
	DEFINE_VECTOR	(CWeapon*,WeaponVec,WeaponIt);

	CEntity*		m_pParent;

	WeaponVec		m_Weapons;
	int				m_iActiveWeapon;	// Current active weapon
	int				m_iSelectedWeapon;	// Candidate for change to
	BOOL			m_bZoomed;
	
	int				FindWeapon			(CLASS_ID cls);
	BOOL 			WeaponChange		(int idx);

public:
	int				m_iHUDboneL, m_iHUDboneR;
	int				m_iACTboneL, m_iACTboneR;

public:
					CWeaponList			(CEntity* p);
					~CWeaponList		();

	void			Init				(LPCSTR act_bone_r, LPCSTR act_bone_l);

	BOOL			ActivateWeaponPrev	(BOOL ignore_empty_weapon=true);
	BOOL			ActivateWeaponNext	(BOOL ignore_empty_weapon=true);
	BOOL			ActivateWeapon		(CLASS_ID cls);
	BOOL			ActivateWeaponID	(int id);
	void			Zoom				(BOOL bZoom);
	void			Reload				();
	IC BOOL			isZoomed			()	{ return m_bZoomed; }
	float			getZoomFactor		()	{ CWeapon* W = ActiveWeapon(); if (W&&isZoomed()) return W->GetZoomFactor();  else return DEFAULT_FOV; }
	int				getAmmoElapsed		()	{ CWeapon* W = ActiveWeapon(); if (W) return W->GetAmmoElapsed(); else return 0; }
	int				getAmmoCurrent		()	{ CWeapon* W = ActiveWeapon(); if (W) return W->GetAmmoCurrent(); else return 0; }
	int				getAmmoLimit		()	{ CWeapon* W = ActiveWeapon(); if (W) return W->GetAmmoLimit();	  else return 0; }
	int				getAmmoMagazineSize	()	{ CWeapon* W = ActiveWeapon(); if (W) return W->GetAmmoMagSize(); else return 0; }
	
	BOOL			TakeItem			(CLASS_ID cls, int iAmmoCount);
	void			LeaveWeapon			(CLASS_ID cls);

	void			FireStart			();
	void			FireEnd				();
	void			GetFireParams		(Fvector &fire_pos, Fvector &fire_dir);
	BOOL			isWorking			();

	void			Reset				(){;}

	void			Update				(float dt, BOOL bHUDView);

	// for soldiers
	IC	CWeapon*	GetWeaponByIndex	(int iIndex)	{ return ((iIndex > -1) && (iIndex < m_Weapons.size())) ? m_Weapons[iIndex] : 0; }

	//
	IC	CWeapon*	ActiveWeapon		()	{ return (m_iActiveWeapon==-1)?0:m_Weapons[m_iActiveWeapon]; }
	IC	int			ActiveWeaponID		()	{ return m_iActiveWeapon;	}
	IC	int			SelectedWeaponID	()	{ return m_iSelectedWeapon; }
	IC	int			WeaponCount			()	{ return m_Weapons.size();	}
};

#endif //__XR_OBJECT_LIST_H__
