#pragma once
#include "uiwindow.h"


class CInventoryItem;
class CUIStatic;
class CUIScrollView;
class CUIProgressBar;
class CUIWpnParams;

extern const char * const 		fieldsCaptionColor;

class CUIItemInfo: public CUIWindow
{
private:
	typedef CUIWindow inherited;
	CInventoryItem* m_pInvItem;
public:
					CUIItemInfo			();
	virtual			~CUIItemInfo		();

	void			Init				(float x, float y, float width, float height, LPCSTR xml_name);
	void			Init				(LPCSTR xml_name);
	void			InitItem			(CInventoryItem* pInvItem);
	void			TryAddWpnInfo		(LPCSTR wpn_section);

	virtual void	Draw				();
	bool			m_b_force_drawing;
	CUIStatic*		UIName;
	CUIStatic*		UIWeight;
	CUIStatic*		UICost;
	CUIStatic*		UICondition;
	CUIScrollView*	UIDesc;
	CUIProgressBar*	UICondProgresBar;
	CUIWpnParams*	UIWpnParams;

	//для изображения предмета крупным планом
	Fvector2	UIItemImageSize; 
	CUIStatic*	UIItemImage;
};