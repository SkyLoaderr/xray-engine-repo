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
	CUIDynamicItem	entity_up;
	CUIDynamicItem	entity_down;
	float			heading;
	Ivector2		map_center;
	int				map_radius;

	void			EntityOut		(float diff, u32 color, const Ivector2& pt);
	void			ConvertToLocal	(const Fmatrix& LM, const Fvector& src, Ivector2& dest);
public:
					CUIZoneMap		();
					~CUIZoneMap		();
	void			Render			();
	void			SetHeading		(float angle);
	void			Init			();

	void			UpdateRadar		(CEntity* Actor, CTeam& Team);
};

#endif //__XR_UIZONEMAP_H__
