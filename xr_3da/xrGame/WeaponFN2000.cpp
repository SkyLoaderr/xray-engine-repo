#include "stdafx.h"
#include "WeaponFN2000.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponFN2000::CWeaponFN2000() : CWeaponMagazined("FN2000",SOUND_TYPE_WEAPON_SNIPERRIFLE)
{
	fMaxZoomFactor		= 25.f;
}

CWeaponFN2000::~CWeaponFN2000()
{
}

void CWeaponFN2000::Load	(LPCSTR section)
{
	inherited::Load			(section);
	fMaxZoomFactor			= pSettings->ReadFLOAT	(section,"max_zoom_factor");
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
