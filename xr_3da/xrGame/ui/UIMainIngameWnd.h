// UIMainIngameWnd.h:  ������-���������� � ����
// 
//////////////////////////////////////////////////////////////////////

#pragma once



#include "UIStatic.h"
#include "UIButton.h"
#include "UIDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"



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
	CUIStatic	UIStaticWeapon;

	CUIStatic	UITextWound;
	CUIStatic	UITextAmmo;
	CUIStatic	UITextWeaponName;

	CUIProgressBar UIHealthBar;

	CUIWeapon UIWeapon;
	CUIZoneMap UIZoneMap;

	CUIStatic	UIWeapon1;
	CUIStatic	UIWeapon2;

	//��� �������� ��������� ������ � ������
	CActor*		m_pActor;	
	CWeapon*	m_pWeapon;
};