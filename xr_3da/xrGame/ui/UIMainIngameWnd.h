// UIMainIngameWnd.h:  окошки-информация в игре
// 
//////////////////////////////////////////////////////////////////////

#pragma once



#include "UIStatic.h"
#include "UIButton.h"
#include "UIDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"



#include "..\\UIZoneMap.h"
#include "..\\UIWeapon.h"



class CUIMainIngameWnd: public CUIWindow  
{
public:
	CUIMainIngameWnd();
	virtual ~CUIMainIngameWnd();

	virtual void Init();
	//virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void Draw();
	virtual void Update();

	bool OnKeyboardPress(int dik);

protected:

	CUIStatic	UIStaticHealth;
	CUIStatic	UIStaticMapBack;
	CUIStatic	UIStaticRadiationLow;
	CUIStatic	UIStaticRadiationMedium;
	CUIStatic	UIStaticRadiationHigh;
	CUIStatic	UIStaticWound;
	CUIStatic	UIStaticWeapon;

	CUIStatic	UITextWound;
	CUIStatic	UITextAmmo;
	CUIStatic	UITextWeaponName;

	CUIProgressBar UIHealthBar;

	CUIWeapon UIWeapon;
	CUIZoneMap UIZoneMap;

	//a test	
//	CUIStaticItem g_UIStaticItem;
	
};