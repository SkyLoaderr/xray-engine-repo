#include "stdafx.h"
#include "weaponsvd.h"

CWeaponSVD::CWeaponSVD(void) : CWeaponCustomPistol("SVD")
{
}

CWeaponSVD::~CWeaponSVD(void)
{
}

void CWeaponSVD::Load	(LPCSTR section)
{
	inherited::Load			(section);
	fMaxZoomFactor			= pSettings->ReadFLOAT	(section,"max_zoom_factor");
}

void CWeaponSVD::Fire2Start () {
	inherited::Fire2Start();
	OnZoomIn();
	fZoomFactor = fMaxZoomFactor;
}
void CWeaponSVD::Fire2End () {
	inherited::Fire2End();
	OnZoomOut();
	fZoomFactor = DEFAULT_FOV;
}
