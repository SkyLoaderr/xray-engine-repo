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
#include "UIMultiTextStatic.h"
#include "UIColorAnimatorWrapper.h"

#include "../UIZoneMap.h"

#include "../actor.h"
#include "../weapon.h"
#include "../alife_space.h"

#include "xrXMLParser.h"

//////////////////////////////////////////////////////////////////////////

//для режима настройки HUD
extern int				g_bHudAdjustMode;
extern float			g_fHudAdjustValue;
extern int				g_bNewsDisable;

class					CUIPdaMsgListItem;

//////////////////////////////////////////////////////////////////////////

struct CUSTOM_TEXTURE
{
	CUSTOM_TEXTURE(CUIStaticItem* si, int left, int top, int right, int bottom, int priority = 0)
	{
		static_item		= si;
		x1				= left;
		y1				= top;
		x2				= right;
		y2				= bottom;
		texPriority		= priority;
	};
		
	CUIStaticItem*	static_item;
	int				x1, y1, x2, y2;
	int				texPriority;
};

//////////////////////////////////////////////////////////////////////////

DEFINE_VECTOR(CUSTOM_TEXTURE, CUSTOM_TEXTURE_VECTOR, CUSTOM_TEXTURE_IT);

//////////////////////////////////////////////////////////////////////////

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
	void ReceivePdaMessage(CInventoryOwner* pSender, EPdaMsg msg, INFO_INDEX info_index);
	
	CUIPdaMsgListItem * AddGameMessage	(LPCSTR message);
	void AddPersonalizedGameMessage		(CInventoryOwner* pSender, LPCSTR TextMessage);
	void AddIconedGameMessage			(LPCSTR textureName, RECT originalRect, LPCSTR message);
	void AddStaticItem					(CUIStaticItem* si, int left, int top, int right, int bottom, int priority = 0);
	// Функция для вывода служебных сообщений, таких как "здась спать нельзя",
	// "рюкзак переполнен", и т.д. Возвращаем указатель на добавленный элемент
	void AddInfoMessage	(LPCSTR message);

protected:
	void HideAll();
	void ShowAll();
	bool m_bShowHudInfo;
	bool m_bShowHudCrosshair;

	CUIStatic			UIStaticHealth;
	CUIStatic			UIStaticArmor;
	CUIStatic			UIStaticMapBack;
	CUIStatic			UIStaticBattery;

	// Статик контрол для отображения подсказок действий при наведении прицела на объект
	// Кнопка потому, что в статике еще нет функции выравнивания текста
	CUIButton			UIStaticQuickHelp;
	CUIStatic			UITextWound;
	CUIProgressBar		UIHealthBar;
	CUIProgressBar		UIArmorBar;
	CUIProgressBar		UIBatteryBar;

	CUIZoneMap			UIZoneMap;

	//иконка, показывающая количество активных PDA
	CUIStatic			UIPdaOnline;
	//список входящих сообщений PDA
	CUIListWnd			UIPdaMsgListWnd;
	// Список входящих информационных сообщений
	CUIListWnd			UIInfoMessages;
	
	//интервал показывания сообщения
	int					m_dwMaxShowTime;
	int					m_iInfosShowTime;

	//изображение оружия
	CUIStatic			UIWeaponBack;
	CUIStatic			UIWeaponSignAmmo;
	CUIStatic			UIWeaponIcon;

	// Для мультиплеера выводим денежки
	CUIMultiTextStatic	UIMoneyIndicator;
	
	//список текстур, задаваемых извне, которые будут отрисованы
	//на текущем кадре
	CUSTOM_TEXTURE_VECTOR m_CustomTextures;	
public:
	// Изменить индикатор текущего количества денег
	void ChangeTotalMoneyIndicator(shared_str newMoneyString);
	// Показать (с анимацией) помледнте заработанные/отняные денежки
	void DisplayMoneyChange(shared_str deltaMoney);
protected:

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

	// Енум перечисления возможных мигающих иконок
	enum EFlashingIcons
	{
		efiPdaTask	= 0,
		efiMail
	};
	
	// Вкл/выкл мигающую иконку
	void SetFlashIconState(EFlashingIcons type, bool enable);

	//
	void AnimateContacts();

	// Обработчик события получения новости
	void OnNewsReceived	(GAME_NEWS_DATA &news);
	
protected:

	void				InitFlashingIcons(CUIXml &node);
	void				DestroyFlashingIcons();
	void				UpdateFlashingIcons();

	// first - иконка, second - анимация
	typedef				std::pair<CUIStatic*, CUIColorAnimatorWrapper> IconInfo;
	DEF_MAP				(FlashingIcons, EFlashingIcons, IconInfo);
	FlashingIcons		m_FlashingIcons;

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
	void				RenderQuickInfos();
	// Просчитать анимационные параметры фейда для айтемов листа
	static void			FadeUpdate(CUIListWnd *pWnd, int fadeDuration);

	// для лога сообщений на экране устанавливаем время постепенного исчезновения надписи
	int					m_iPdaMessagesFade_mSec;
	int					m_iInfoMessagesFade_mSec;
	int					m_iChatMessagesFade_mSec;

	// Мигалка для контактов
	CUIColorAnimatorWrapper		UIContactsAnimation;

private:
	// Блок операций работы с текстурами-эффектами ударов когтей на экране(как в Doom 3)
	DEF_MAP(ClawsTexturesRepository, shared_str /*texture name*/, ref_shader /*actually shader*/);
	DEF_MAP(MonsterClawsTextures, shared_str /*monster name*/, ref_shader* /*effect texture*/);
	ClawsTexturesRepository		m_ClawsRepos;
	MonsterClawsTextures		m_ClawsTextures;

	// Static item for display claws texture
	CUIStaticItem				m_ClawsTexture;
	// Animation engine for claws
	CUIColorAnimatorWrapper		m_ClawsAnimation;
	// Update claws animation
	void						UpdateClawsAnimation();

public:
	// Инициализировать текстуры когтей для монстров
	void				AddMonsterClawsEffect(const shared_str &monsterName, const shared_str &textureName);
	// Проиграть анимацию текстуры когтей монстра
	void				PlayClawsAnimation(const shared_str &monsterName);
	// Установить позицию заряда батарейки
	void				SetBatteryCharge(float value);
	// Показать/спрятать батарейку
	void				ShowBattery(bool on);

	// Multiplayer chat log
	CUIListWnd			UIMPChatLog;
};