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
	
	CWeapon*		LoadOne				(CLASS_ID cls);
	int				FindWeapon			(CLASS_ID cls);
	BOOL 			WeaponChange		(int idx);

public:
	int				m_iHUDboneL, m_iHUDboneR;
	int				m_iACTboneL, m_iACTboneR;

public:
					CWeaponList			(CEntity* p);
					~CWeaponList		();

	void			Init				(LPSTR act_bone_r, LPSTR act_bone_l);

	BOOL			ActivateWeaponPrev	(BOOL ignore_empty_weapon=true);
	BOOL			ActivateWeaponNext	(BOOL ignore_empty_weapon=true);
	BOOL			ActivateWeapon		(CLASS_ID cls);
	BOOL			ActivateWeaponID	(int id);
	void			Zoom				(BOOL bZoom);
	IC BOOL			isZoomed			()	{ return m_bZoomed; }

	BOOL			TakeItem			(CLASS_ID cls, int iAmmoCount);
	void			LeaveWeapon			(CLASS_ID cls);

	void			FireStart			();
	void			FireEnd				();
	void			GetFireParams		(Fvector &fire_pos, Fvector &fire_dir);
	BOOL			isWorking			();

	void			Reset				(){;}

	void			Update				(float dt, BOOL bHUDView);
	void			OnRender			(BOOL bHUDView);

	IC CWeapon*		ActiveWeapon		()	{ return (m_iActiveWeapon==-1)?0:m_Weapons[m_iActiveWeapon]; }
	IC	int			ActiveWeaponID		()	{ return m_iActiveWeapon;	}
	IC	int			SelectedWeaponID	()	{ return m_iSelectedWeapon; }
};

#endif //__XR_OBJECT_LIST_H__
