#pragma once

//#include "uistaticitem.h"
//#include "uidynamicitem.h"


#include "ui/UIStatic.h"

class CActor;
class CUICustomMap;
//////////////////////////////////////////////////////////////////////////


class CUIZoneMap
{
	CUICustomMap*				m_activeMap;
	float						m_fScale;

	CUIStatic					m_background;
	CUIStatic					m_center;
	CUIStatic					m_compass;
	CUIStatic					m_clipFrame;

public:
								CUIZoneMap		();
	virtual						~CUIZoneMap		();

	void						SetHeading		(float angle);
	void						Init			();

	void						Render			();
	void						UpdateRadar		(CActor* Actor);

	void						SetScale		(float s)							{m_fScale = s;}
	float						GetScale		()									{return m_fScale;}

	bool						ZoomIn			();
	bool						ZoomOut			();

};


/*
// Название ключа имени текстуры карты уровня
extern const char * const	mapTextureKey;
// Аналогично её координаты на глобальной карте
extern const char * const	mapLocalCoordinatesKey;
class CUIZoneMap
{
	CUIStaticItem	landscape;

	CUIStaticItem	back;
	CUIStaticItem	compass;
	CUIDynamicItem	entity;
	//для вывода сюжетных локаций
	CUIDynamicItem	entity_story;
	CUIDynamicItem	entity_arrow;
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

	void			EntityOut		(float diff, u32 color, const Ivector2& pt);
	void			ConvertToLocal	(const Fmatrix& LM, const Fvector& src, Ivector2& dest, int border_radius, bool& on_border);
	void			ConvertToLocalWithoutTransform(const Fmatrix& LM, const Fvector& src, Fvector2 &Pt);
public:
					CUIZoneMap		();
	virtual			~CUIZoneMap		();
	void			Render			();
	void			SetHeading		(float angle);
	void			Init			();

	void			UpdateRadar		(CActor* Actor);

	void			SetScale		(float s)							{m_fScale = s;}
	float			GetScale		()									{return m_fScale;}

	bool			ZoomIn			();
	bool			ZoomOut			();

};
*/