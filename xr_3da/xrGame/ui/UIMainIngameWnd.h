// UIMainIngameWnd.h:  окошки-информация в игре
// 
//////////////////////////////////////////////////////////////////////

#pragma once



#include "UIStatic.h"
#include "UIButton.h"
#include "UIDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIWpnDragDropItem.h"


#include "../UIZoneMap.h"
#include "../UIWeapon.h"

#include "../actor.h"
#include "../weapon.h"


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
	
	CUIStatic	UITextWound;

	CUIProgressBar UIHealthBar;

	CUIZoneMap UIZoneMap;

	//иконка, показывающая количество активных PDA
	CUIStatic	UIPdaOnline;

	//изображение оружия
	CUIStatic			UIWeaponBack;
	CUIStatic			UIWeaponSignAmmo;
	CUIStatic			UIWeaponSignName;
	CUIStatic			UIWeaponIcon;
	//CUIDragDropItem	UIWeaponIcon;

	//для текущего активного актера и оружия
	CActor*		m_pActor;	
	CWeapon*	m_pWeapon;
	int			m_iWeaponIconX;
	int			m_iWeaponIconY;
	int			m_iWeaponIconWidth;
	int			m_iWeaponIconHeight;
};