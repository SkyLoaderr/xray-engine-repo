// UIItemInfo.h:  окошко, для отображения информации о вещи
#pragma once

#include "uiwindow.h"
//#include "uistatic.h"
//#include "UIListWnd.h"
//#include "UIProgressBar.h"
//#include "UIScrollView.h"



class CInventoryItem;
class CUIStatic;
class CUIScrollView;
class CUIProgressBar;

extern const char * const 		fieldsCaptionColor;

class CUIItemInfo: public CUIWindow
{
private:
	typedef CUIWindow inherited;
	CInventoryItem* m_pInvItem;
public:
					CUIItemInfo			();
	virtual			~CUIItemInfo		();

	void			Init				(float x, float y, float width, float height, const char* xml_name);
	void			InitItem			(CInventoryItem* pInvItem);

	virtual void	Draw				();

	CUIStatic*		UIName;
	CUIStatic*		UIWeight;
	CUIStatic*		UICost;
	CUIStatic*		UICondition;
	CUIScrollView*	UIDesc;
	CUIProgressBar*	UICondProgresBar;

	//для изображения предмета крупным планом
	Fvector2	UIItemImageSize; 
	CUIStatic*	UIItemImage;
};