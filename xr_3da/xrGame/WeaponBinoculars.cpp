#include "stdafx.h"
#include "WeaponBinoculars.h"

#include "xr_level_controller.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponBinoculars::CWeaponBinoculars() : CWeaponCustomPistol("BINOCULARS")
{

	m_eSoundShow		= ESoundTypes(SOUND_TYPE_ITEM_TAKING | SOUND_TYPE_ITEM_USING);
	m_eSoundHide		= ESoundTypes(SOUND_TYPE_ITEM_HIDING | SOUND_TYPE_ITEM_USING);
}

CWeaponBinoculars::~CWeaponBinoculars()
{
	sndShow.destroy();
	sndHide.destroy();	
	sndGyro.destroy();
	sndZoomIn.destroy();
	sndZoomOut.destroy();
}

void CWeaponBinoculars::Load	(LPCSTR section)
{
	inherited::Load(section);

	// Sounds
	sndShow.create(TRUE, pSettings->r_string(section,	"snd_draw"), m_eSoundShow);
	sndHide.create(TRUE, pSettings->r_string(section,	"snd_holster"), m_eSoundHide);
	sndGyro.create(TRUE, pSettings->r_string(section,	"snd_gyro"), st_SourceType);
	sndZoomIn.create(TRUE, pSettings->r_string(section,	"snd_zoomin"), st_SourceType);
	sndZoomOut.create(TRUE, pSettings->r_string(section,"snd_zoomout"), st_SourceType);
}

void CWeaponBinoculars::Hide		()
{
	SwitchState(eHidden);
}
void CWeaponBinoculars::Show		()
{
	SwitchState(eShowing);
}

bool CWeaponBinoculars::Action(s32 cmd, u32 flags) 
{
	switch(cmd) 
	{
	case kWPN_FIRE : 
		return inherited::Action(kWPN_ZOOM, flags);
	}

	return inherited::Action(cmd, flags);
}
