// UIItemInfo.h:  окошко, для отображения информации о вещи
// 
//////////////////////////////////////////////////////////////////////

#include "uiwindow.h"
#include "uistatic.h"
#include "ui3dstatic.h"

#pragma once


class CInventoryItem;

class CUIItemInfo: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUIItemInfo();
	virtual ~CUIItemInfo();

	void Init(int x, int y, int width, int height, const char* xml_name);
	void InitItem(CInventoryItem* pInvItem);

	virtual void Draw();

	CUIStatic UIName;
	CUIStatic UIWeight;
	CUIStatic UIDesc;

	//для изображения предмета крупным планом
	CUI3dStatic			UI3dStatic;
protected:
	float y_rotate_angle;
};