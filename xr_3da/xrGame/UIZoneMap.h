#ifndef __XR_UIZONEMAP_H__
#define __XR_UIZONEMAP_H__
#pragma once

#include "uistaticitem.h"
#include "uidynamicitem.h"

class CEntity;

class CUIZoneMap
{
	CUIStaticItem	landscape;

	CUIStaticItem	back;
	CUIStaticItem	compass;
	CUIDynamicItem	entity;
	CUIDynamicItem	entity_up;
	CUIDynamicItem	entity_down;
	float			heading;
	Ivector2		map_center;
	int				map_radius;

	float			m_fScale;
	
	float landscape_x1, landscape_y1;
	float landscape_x2, landscape_y2;
	float landscape_x3, landscape_y3;
	float landscape_x4, landscape_y4;

	Fbox level_box;

	void			EntityOut		(float diff, u32 color, const Ivector2& pt);
	void			ConvertToLocal	(const Fmatrix& LM, const Fvector& src, Ivector2& dest);
public:
					CUIZoneMap		();
	virtual			~CUIZoneMap		();
	void			Render			();
	void			SetHeading		(float angle);
	void			Init			();

	void			UpdateRadar		(CEntity* Actor);

	void			SetScale		(float scale)						{m_fScale = scale;}
	float			GetScale		()									{return m_fScale;}

	bool			ZoomIn			();
	bool			ZoomOut			();

};

#endif //__XR_UIZONEMAP_H__
