#ifndef __XR_UIZONEMAP_H__
#define __XR_UIZONEMAP_H__
#pragma once

#include "uistaticitem.h"
#include "uidynamicitem.h"

class CUIZoneMap
{
	CUIStaticItem	back;
	CUIStaticItem	compass;
	CUIDynamicItem	entity;
	float			heading;
	Fvector2		map_center;
	float			map_radius;

	void			ConvertToLocal	(const Fmatrix& LM, const Fvector& src, Fvector2& dest);
public:
					CUIZoneMap		();
					~CUIZoneMap		();
	void			Render			();
	void			SetHeading		(float angle);
	void			Init			();

	void			UpdateRadar		(CEntity* Actor, CTeam& Team);
};

#endif //__XR_UIZONEMAP_H__
