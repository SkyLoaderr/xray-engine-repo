#include "stdafx.h"
#include "weaponsvu.h"

CWeaponSVU::CWeaponSVU(void) : CWeaponCustomPistol("SVU")
{
}

CWeaponSVU::~CWeaponSVU(void)
{
}

void CWeaponSVU::Load	(LPCSTR section)
{
	inherited::Load			(section);
	fMaxZoomFactor			= pSettings->r_float	(section,"max_zoom_factor");
}

void CWeaponSVU::Fire2Start () {
	inherited::Fire2Start();
	OnZoomIn();
	fZoomFactor = fMaxZoomFactor;
}
void CWeaponSVU::Fire2End () {
	inherited::Fire2End();
	OnZoomOut();
	fZoomFactor = DEFAULT_FOV;
}

bool CWeaponSVU::Action(s32 cmd, u32 flags) {
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) {
		case kWPN_ZOOM : {
			if(flags&CMD_START) Fire2Start();
			else Fire2End();
		} return true;
	}
	return false;
}
