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
#include "../alife_space.h"

//для режима настройки HUD
extern int				g_bHudAdjustMode;
extern float			g_fHudAdjustValue;
extern int				g_bNewsDisable;

class CUIMainIngameWnd: public CUIWindow  
{
public:
	CUIMainIngameWnd();
	virtual ~CUIMainIngameWnd();

	virtual void Init();
//	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void Draw();
	virtual void Update();

	bool OnKeyboardPress(int dik);

	//для отображения сообщения пришедшего по PDA
	void ReceivePdaMessage(CInventoryOwner* pSender, EPdaMsg msg, int info_index);
	
	void AddGameMessage	(CInventoryOwner* pSender, LPCSTR TextMessage);

protected:
	void HideAll();
	void ShowAll();
	bool m_bShowHudInfo;
	bool m_bShowHudCrosshair;

	CUIStatic	UIStaticHealth;
	CUIStatic	UIStaticArmor;
	CUIStatic	UIStaticMapBack;
//	CUIStatic	UIStaticRadiationLow;
//	CUIStatic	UIStaticRadiationMedium;
//	CUIStatic	UIStaticRadiationHigh;
//	CUIStatic	UIStaticWound;
	// Статик контрол для отображения подсказок действий при наведении прицела на объект
	// Кнопка потому, что в статике еще нет функции выравнивания текста
	CUIButton	UIStaticQuickHelp;
	
	CUIStatic	UITextWound;

	CUIProgressBar UIHealthBar;
	CUIProgressBar UIArmorBar;

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
	CUIStatic			UIWeaponIcon;

	// Надписи armor & health
//	CUIStatic			UIHealthString;
//	CUIStatic			UIArmorString;
	//CUIDragDropItem	UIWeaponIcon;

	// 5 статиков для отображения иконок:
	// - сломанного оружия
	// - радиации
	// - ранения
	// - голода
	// - усталости
	CUIStatic			UIWeaponJammedIcon;
	CUIStatic			UIRadiaitionIcon;
	CUIStatic			UIWoundIcon;
	CUIStatic			UIStarvationIcon;
	CUIStatic			UIFatigueIcon;

public:
	
	// Енумы соответсвующие предупреждающим иконкам 
	enum EWarningIcons
	{
		ewiAll				= 0,
		ewiWeaponJammed,
		ewiRadiation,
		ewiWound,
		ewiStarvation,
		ewiFatigue
	};

	// Задаем цвет соответствующей иконке
	void SetWarningIconColor(EWarningIcons icon, const u32 cl);
	void TurnOffWarningIcon(EWarningIcons icon);

	// Пороги изменения цвета индикаторов, загружаемые из system.ltx
	typedef std::map<EWarningIcons, xr_vector<float> >	Thresholds;
	typedef Thresholds::iterator						Thresholds_it;
	Thresholds	m_Thresholds;

protected:

	//для текущего активного актера и оружия
	CActor*				m_pActor;	
	CWeapon*			m_pWeapon;
	int					m_iWeaponIconX;
	int					m_iWeaponIconY;
	int					m_iWeaponIconWidth;
	int					m_iWeaponIconHeight;

	// Добавлено для поддержки fadein/fadeout реалтаймовых подсказок
	float				fuzzyShowInfo;
	// Отображение подсказок при наведении прицела на объект
	void RenderQuickInfos();

	// ассоциативный массив с типсами. Выбор пал на map, так как заполнение произойдет 1 раз,
	// а поиск будет производиться часто.
	std::map<int, ref_str>	m_strTips;

	// для лога сообщений на экране устанавливаем время постепенного исчезновения надписи
	int					m_iFade_mSec;
	
	//-----------------------------------------------------------------------------/
	//	News related routines
	//-----------------------------------------------------------------------------/

	// Читаем заготовки стандартных собщений
	void LoadNewsTemplates();

	// Структурка описывающая ньюс
	typedef struct tagNewsTemplate
	{
		ref_str		str;
		bool		ignore;
		///////////////////
		tagNewsTemplate(): ignore(false) {}

	} SNewsTemplate;

	// Array of news templates
	typedef std::map<u32, SNewsTemplate>	NewsTemplates;
	NewsTemplates							m_NewsTemplates;

	// Обработчик события получения новости
	void OnNewsReceived(const CALifeNews &newsItem);
	// Проверяем не появились ли новые ньюсы
	// Return:	true - новые новости есть
	bool CheckForNewNews();

	// Период проверки ньюсов в моллисекундах
	static const int						NEWS_CHECK_INTERVAL = 1000;
	ALife::_TIME_ID							m_iPrevTime;

};