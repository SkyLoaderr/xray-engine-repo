#include "stdafx.h"
#include "CustomZone.h"
#include "ZoneVisual.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "../SkeletonAnimated.h"
CVisualZone::CVisualZone						()
{
}
CVisualZone::~CVisualZone						()
{

}

BOOL CVisualZone::	net_Spawn						(LPVOID DC)
{
	BOOL ret					=	inherited::net_Spawn(DC);
	CSE_Abstract				*e = (CSE_Abstract*)(DC);
	CSE_ALifeZoneVisual		*Z = smart_cast<CSE_ALifeZoneVisual*>(e);
	smart_cast<CSkeletonAnimated*>(Visual())->PlayCycle(*Z->startup_animation);
	return ret;
}