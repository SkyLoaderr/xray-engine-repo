#include "stdafx.h"
#include "WeaponFN2000.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponFN2000::CWeaponFN2000() : CWeaponMagazined("FN2000",SOUND_TYPE_WEAPON_SNIPERRIFLE)
{
	m_weight = 1.5f;
	m_slot = 2;
	fMaxZoomFactor		= 25.f;
}

CWeaponFN2000::~CWeaponFN2000()
{
}

void CWeaponFN2000::Load	(LPCSTR section)
{
	inherited::Load			(section);
	fMaxZoomFactor			= pSettings->r_float	(section,"max_zoom_factor");
}

void CWeaponFN2000::Fire2Start () {
	inherited::Fire2Start();
	OnZoomIn();
	fZoomFactor = fMaxZoomFactor;
}
void CWeaponFN2000::Fire2End () {
	inherited::Fire2End();
	OnZoomOut();
	fZoomFactor = DEFAULT_FOV;
}

bool CWeaponFN2000::Action(s32 cmd, u32 flags) {
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) {
		case kWPN_ZOOM : {
			if(flags&CMD_START) Fire2Start();
			else Fire2End();
		} return true;
	}
	return false;
}
