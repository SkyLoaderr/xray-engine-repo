// UIMapSpot.h:  п€тно на карте, 
// при наводке мыше посылает сообщение родителю
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "UIButton.h"

#include "../InfoPortion.h"

//////////////////////////////////////////////////////////////////////////

#define					MAP_ICON_WIDTH			32
#define					MAP_ICON_HEIGHT			32

#define					MAP_ICON_GRID_WIDTH		32
#define					MAP_ICON_GRID_HEIGHT	32

extern const int		ARROW_DIMENTIONS;

//////////////////////////////////////////////////////////////////////////

class CUIMapSpot: public CUIButton
{
private:
	typedef CUIButton inherited;
public:
						CUIMapSpot();
	virtual				~CUIMapSpot();

	virtual void		Draw();
	virtual void		Update();

	//возвращает позицию отметки на карте в мировых координатах
	virtual Fvector		MapPos();

	virtual void		SetObjectID(u16 id);

private:
	//id текущего уровн€
	ALife::_LEVEL_ID	m_our_level_id;
	//id объекта, который показываетс€
	//на карте, если такого нет то 0xffff
	u16					m_object_id;

public:
	//позици€ точки в мировой системе координат,
	//нужно если m_pObject NULL
	Fvector				m_vWorldPos;
	//им€ локации
	CUIString			m_sNameText;
	//если есть текст с описанием 
	CUIString			m_sDescText;
	// ¬кл/выкл стрелку-указатель
	bool				m_bArrowEnabled;
	// ѕризнак видимости стрелочки
	bool				m_bArrowVisible;
	//цвет стерлочки указател€ дл€ объекта
	u32					arrow_color;
	
	//центрирование иконки 
	enum EMapSpotAlign
	{
		eBottom, eCenter, eNone
	};
	EMapSpotAlign		m_eAlign;
	
	//направление иконки
	bool				m_bHeading;
	float				m_fHeading;
	// Cтрелочка - указатель
	CUIStaticItem		m_Arrow;
	// »м€ уровн€ которому пренадлежит спот
	ref_str				m_LevelName;
};
