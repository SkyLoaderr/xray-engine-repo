#include "stdafx.h"
#include "weaponshotgun.h"

CWeaponShotgun::CWeaponShotgun(void) : CWeaponCustomPistol("TOZ34")
{
	m_reload = true;
}

CWeaponShotgun::~CWeaponShotgun(void)
{
}

void CWeaponShotgun::OnAnimationEnd	() {
	__super::OnAnimationEnd();
	if(false && m_reload) {
		m_reload = false;
		Reload();
	}
}
