#include "stdafx.h"
#include "weaponvintorez.h"

CWeaponVintorez::CWeaponVintorez(void) : CWeaponMagazined("VINTOREZ",SOUND_TYPE_WEAPON_SNIPERRIFLE)
{
	m_weight = 1.5f;
	m_slot = 2;
	fMaxZoomFactor		= 25.f;
}

CWeaponVintorez::~CWeaponVintorez(void)
{
}

void CWeaponVintorez::Load	(LPCSTR section)
{
	inherited::Load			(section);
	fMaxZoomFactor			= pSettings->r_float	(section,"max_zoom_factor");
}

void CWeaponVintorez::Fire2Start () {
	inherited::Fire2Start();
	OnZoomIn();
	fZoomFactor = fMaxZoomFactor;
}
void CWeaponVintorez::Fire2End () {
	inherited::Fire2End();
	OnZoomOut();
	fZoomFactor = DEFAULT_FOV;
}

bool CWeaponVintorez::Action(s32 cmd, u32 flags) {
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) {
		case kWPN_ZOOM : {
			if(flags&CMD_START) Fire2Start();
			else Fire2End();
		} return true;
	}
	return false;
}
