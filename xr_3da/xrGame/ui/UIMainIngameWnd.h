// UIMainIngameWnd.h:  окошки-информация в игре
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "UIButton.h"
#include "UIFrameWindow.h"
#include "UIDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIWpnDragDropItem.h"
#include "UIListWnd.h"

#include "../UIZoneMap.h"

#include "../actor.h"
#include "../weapon.h"

//для режима настройки HUD
extern int				g_bHudAdjustMode;
extern float			g_fHudAdjustValue;

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

	//для отображения сообщения пришедшего по PDA
	void ReceivePdaMessage(CInventoryOwner* pSender, EPdaMsg msg, int info_index);

protected:
	void HideAll();
	void ShowAll();
	bool m_bShowHudInfo;
	bool m_bShowHudCrosshair;

	CUIStatic	UIStaticHealth;
	CUIStatic	UIStaticMapBack;
	CUIStatic	UIStaticRadiationLow;
	CUIStatic	UIStaticRadiationMedium;
	CUIStatic	UIStaticRadiationHigh;
	CUIStatic	UIStaticWound;
	// Статик контрол для отображения подсказок действий при наведении прицела на объект
	// Кнопка потому, что в статике еще нет функции выравнивания текста
	CUIButton	UIStaticQuickHelp;
	
	CUIStatic	UITextWound;

	CUIProgressBar UIHealthBar;

	CUIZoneMap UIZoneMap;

	//иконка, показывающая количество активных PDA
	CUIStatic			UIPdaOnline;
	//список входящих сообщений PDA
	CUIListWnd			UIPdaMsgListWnd;
	
	//интервал показывания сообщения
	int					m_dwMaxShowTime;
	//время сколько показывается новое сообщение PDA
	int					m_dwMsgShowingTime;

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

	// Добавлено для поддержки fadein/fadeout реалтаймовых подсказок
	float					fuzzyShowInfo;
	// Отображение подсказок при наведении прицела на объект
	// Эта процедура почти полная копия куска процедуры CHUDCursor::Render().
	// Возможно, необходимо сделать таки 1 процедуру отрисовки.
	void RenderQuickInfos();

	// ассоциативный массив с типсами. Выбор пал на map, так как заполнение произойдет 1 раз,
	// а поиск будет производиться часто.
	std::map<int, ref_str>	m_strTips;
};