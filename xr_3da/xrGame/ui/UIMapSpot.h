// UIMapSpot.h:  пятно на карте, 
// при наводке мыше посылает сообщение родителю
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "UIButton.h"

#include "../InfoPortion.h"

#define MAP_ICON_WIDTH  32
#define MAP_ICON_HEIGHT 32

extern const int			ARROW_DIMENTIONS;


class CUIMapSpot: public CUIButton
{
private:
	typedef CUIButton inherited;
public:
	CUIMapSpot();
	virtual ~CUIMapSpot();

	virtual void Draw();
	virtual void Update();
	
	//указатель на объект, который показывается
	//на карте,
	//если такого нет то NULL
	CObject* m_pObject;
	//позиция точки в мировой системе координат,
	//нужно если m_pObject NULL
	Fvector m_vWorldPos;
	//имя локации
	CUIString m_sNameText;
	//если есть текст с описанием 
	CUIString m_sDescText;
	// Вкл/выкл стрелку-указатель
	bool m_bArrowEnabled;
	// Признак видимости стрелочки
	bool m_bArrowVisible;
	
	//центрирование иконки 
	enum EMapSpotAlign {eBottom, eCenter, eNone};
	EMapSpotAlign m_eAlign;
	
	//направление иконки
	bool	m_bHeading;
	float	m_fHeading;

	// Cтрелочка - указатель
	CUIStaticItem m_Arrow;
};
