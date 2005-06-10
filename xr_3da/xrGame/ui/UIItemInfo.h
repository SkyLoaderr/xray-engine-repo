// UIItemInfo.h:  окошко, для отображения информации о вещи
// 
//////////////////////////////////////////////////////////////////////

#include "uiwindow.h"
#include "uistatic.h"
#include "UIListWnd.h"
#include "UIProgressBar.h"

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

	void		Init		(float x, float y, float width, float height, const char* xml_name);
	void		InitItem	(CInventoryItem* pInvItem);

	virtual void Draw();
	void		AlignRight(CUIStatic &Item, float offset);

	CUIStatic	UIName;
	CUIStatic	UIWeight;
	CUIStatic	UICost;
	CUIStatic	UICondition;
	CUIListWnd	UIDesc;
	CUIProgressBar	UICondProgresBar;

	//для изображения предмета крупным планом
	Fvector2	UIItemImageSize; 
	CUIStatic	UIItemImage;
};