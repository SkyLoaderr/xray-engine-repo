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
	int				m_iActiveWeapon;
	
	CWeapon*		LoadOne				(CLASS_ID cls);
	int				FindWeapon			(CLASS_ID cls);
	bool 			WeaponChange		(int idx);

public:
	int				m_iHUDboneL, m_iHUDboneR;
	int				m_iACTboneL, m_iACTboneR;

public:
					CWeaponList			(CEntity* p);
					~CWeaponList		();

	void			Init				();

	bool			ActivateWeaponPrev	(bool ignore_empty_weapon=true);
	bool			ActivateWeaponNext	(bool ignore_empty_weapon=true);
	bool			ActivateWeapon		(CLASS_ID cls);
	bool			ActivateWeaponID	(int id);

	bool			TakeItem			(CLASS_ID cls, int iAmmoCount);
	void			LeaveWeapon			(CLASS_ID cls);

	void			FireStart			();
	void			FireEnd				();
	void			GetFireParams		(Fvector &fire_pos, Fvector &fire_dir);
	BOOL			isWorking			();

	void			Reset				(){;}

	void			Update				(float dt, bool bHUDView);
	void			OnRender			(bool bHUDView);

	const char*		ActiveWeaponName		();
	int				ActiveWeaponAmmoElapsed	();
	int				ActiveWeaponAmmoLimit	();
	int				ActiveWeaponID			();
};

#endif //__XR_OBJECT_LIST_H__
