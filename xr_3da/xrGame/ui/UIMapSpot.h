// UIMapSpot.h:  пятно на карте, 
// при наводке мыше посылает сообщение родителю
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "UIButton.h"


#define MAP_ICON_WIDTH  64
#define MAP_ICON_HEIGHT 64



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
	
	//центрирование иконки (вкл/выкл)
	bool m_bCenter;
};
