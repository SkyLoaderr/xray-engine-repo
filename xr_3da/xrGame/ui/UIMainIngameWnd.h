// UIMainIngameWnd.h:  окошки-информация в игре
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIButton.h"
#include "UIFrameWindow.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIWpnDragDropItem.h"
#include "UIListWnd.h"
#include "UIMultiTextStatic.h"
#include "UIColorAnimatorWrapper.h"

#include "../actor.h"
#include "../weapon.h"
#include "../alife_space.h"

#include "UICarPanel.h"
#include "UIMotionIcon.h"
#include "../hudsound.h"
//для режима настройки HUD
extern int				g_bHudAdjustMode;
extern float			g_fHudAdjustValue;
extern int				g_bNewsDisable;

class					CUIPdaMsgListItem;
class					CLAItem;
class					CUIZoneMap;
class					CUIArtefactPanel;
class					CUIMoneyIndicator;
class					CUIScrollView;

class CUIMainIngameWnd: public CUIWindow  
{
public:
	CUIMainIngameWnd();
	virtual ~CUIMainIngameWnd();

	virtual void Init();
	virtual void Draw();
	virtual void Update();

	bool OnKeyboardPress(int dik);

public:
	// Функция для вывода служебных сообщений, таких как "здась спать нельзя",
	// "рюкзак переполнен", и т.д. Возвращаем указатель на добавленный элемент
	void AddInfoMessage					(LPCSTR message);

protected:
	void				HideAll						();
	void				ShowAll						();
	bool				m_bShowHudCrosshair;
	
	CUIStatic			UIStaticDiskIO;
	CUIStatic			UIStaticHealth;
	CUIStatic			UIStaticArmor;
	CUIStatic			UIStaticQuickHelp;
	CUIProgressBar		UIHealthBar;
	CUIProgressBar		UIArmorBar;
	CUICarPanel			UICarPanel;
	CUIMotionIcon		UIMotionIcon;	
	CUIZoneMap*			UIZoneMap;

	//иконка, показывающая количество активных PDA
	CUIStatic			UIPdaOnline;
	// Список входящих информационных сообщений
	CUIListWnd			UIInfoMessages;
	
	//изображение оружия
	CUIStatic			UIWeaponBack;
	CUIStatic			UIWeaponSignAmmo;
	CUIStatic			UIWeaponIcon;

	// Для мультиплеера выводим денежки
	CUIMoneyIndicator*	m_pMoneyIndicator;
	CUIStatic			UITeam1Sign, UITeam1Score;
	CUIStatic			UITeam2Sign, UITeam2Score;
	//  [7/27/2005]
	CUIStatic			UIRankIndicator;
	//  [7/27/2005]
	
public:
	// Изменить индикатор текущего количества денег
	void				ChangeTotalMoneyIndicator		(shared_str newMoneyString);
	// Показать (с анимацией) помледнте заработанные/отняные денежки
	void				DisplayMoneyChange				(shared_str deltaMoney);
	void				DisplayMoneyBonus				(shared_str bonus);
	CUIStatic*			GetPDAOnline					() { return &UIPdaOnline; };
	void				UpdateTeamsScore				(int t1, int t2);
	void				SetRank							(int rank);
protected:


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
	CUIStatic			UIPsyHealthIcon;
	CUIStatic			UIInvincibleIcon;
	CUIStatic			UISleepIcon;
	CUIStatic			UIArtefactIcon;

	CUIScrollView*		m_UIIcons;
	CUIWindow*			m_pMPChatWnd;
	CUIWindow*			m_pMPLogWnd;
public:	
	CUIArtefactPanel*    m_artefactPanel;
	
public:
	
	// Енумы соответсвующие предупреждающим иконкам 
	enum EWarningIcons
	{
		ewiAll				= 0,
		ewiWeaponJammed,
		ewiRadiation,
		ewiWound,
		ewiStarvation,
		ewiPsyHealth,
		ewiInvincible,
		ewiSleep,
		ewiArtefact,
	};

	void				SetMPChatLog					(CUIWindow* pChat, CUIWindow* pLog);

	// Задаем цвет соответствующей иконке
	void				SetWarningIconColor				(EWarningIcons icon, const u32 cl);
	void				TurnOffWarningIcon				(EWarningIcons icon);

	// Пороги изменения цвета индикаторов, загружаемые из system.ltx
	typedef				xr_map<EWarningIcons, xr_vector<float> >	Thresholds;
	typedef				Thresholds::iterator						Thresholds_it;
	Thresholds			m_Thresholds;

	// Енум перечисления возможных мигающих иконок
	enum EFlashingIcons
	{
		efiPdaTask	= 0,
		efiMail
	};
	
	void				SetFlashIconState_				(EFlashingIcons type, bool enable);

	void				AnimateContacts					(bool b_snd);
	HUD_SOUND			m_contactSnd;

	void				ReceiveNews						(GAME_NEWS_DATA &news);
	
protected:
	void				SetWarningIconColor				(CUIStatic* s, const u32 cl);
	void				InitFlashingIcons				(CUIXml* node);
	void				DestroyFlashingIcons			();
	void				UpdateFlashingIcons				();

	// first - иконка, second - анимация
	DEF_MAP				(FlashingIcons, EFlashingIcons, CUIStatic*);
	FlashingIcons		m_FlashingIcons;

	//для текущего активного актера и оружия
	CActor*				m_pActor;	
	CWeapon*			m_pWeapon;
	CInventoryItem*		m_pItem;

	float				m_iWeaponIconX;
	float				m_iWeaponIconY;
	float				m_iWeaponIconWidth;
	float				m_iWeaponIconHeight;

	// Отображение подсказок при наведении прицела на объект
	void				RenderQuickInfos();
	// Просчитать анимационные параметры фейда для айтемов листа
	void				FadeUpdate(CUIListWnd *pWnd);//, int fadeDuration);

public:
	CUICarPanel&		CarPanel							(){return UICarPanel;};
	CUIMotionIcon&		MotionIcon							(){return UIMotionIcon;}
protected:
	CInventoryItem*		m_pPickUpItem;
	CUIStatic			UIPickUpItemIcon;

	float				m_iPickUpItemIconX;
	float				m_iPickUpItemIconY;
	float				m_iPickUpItemIconWidth;
	float				m_iPickUpItemIconHeight;

	void				UpdatePickUpItem();
public:
	void				SetPickUpItem	(CInventoryItem* PickUpItem);

};