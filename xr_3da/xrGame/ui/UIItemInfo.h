// UIItemInfo.h:  окошко, для отображения информации о вещи
// 
//////////////////////////////////////////////////////////////////////

#include "uiwindow.h"
#include "uistatic.h"
#include "UIListWnd.h"

#pragma once

//////////////////////////////////////////////////////////////////////////

class CInventoryItem;
extern const char * const 		fieldsCaptionColor;

//////////////////////////////////////////////////////////////////////////

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
	void		AlignRight(CUIStatic &Item, int offset);

	CUIStatic	UIName;
	CUIStatic	UIWeight;
	CUIStatic	UICost;
	CUIStatic	UICondition;
	CUIListWnd	UIDesc;

	//для изображения предмета крупным планом
	CUIStatic	UIItemImage;
};