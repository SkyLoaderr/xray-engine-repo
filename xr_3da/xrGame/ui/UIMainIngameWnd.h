// UIMainIngameWnd.h:  ������-���������� � ����
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

//��� ������ ��������� HUD
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

	//��� ����������� ��������� ���������� �� PDA
	void ReceivePdaMessage(CInventoryOwner* pSender, EPdaMsg msg, INFO_INDEX info_index);
	
	CUIPdaMsgListItem * AddGameMessage	(LPCSTR message);
	void AddPersonalizedGameMessage		(CInventoryOwner* pSender, LPCSTR TextMessage);
	void AddIconedGameMessage			(LPCSTR textureName, RECT originalRect, LPCSTR message);
	void AddStaticItem					(CUIStaticItem* si, int left, int top, int right, int bottom, int priority = 0);
	// ������� ��� ������ ��������� ���������, ����� ��� "����� ����� ������",
	// "������ ����������", � �.�. ���������� ��������� �� ����������� �������
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

	// ������ ������� ��� ����������� ��������� �������� ��� ��������� ������� �� ������
	// ������ ������, ��� � ������� ��� ��� ������� ������������ ������
	CUIButton			UIStaticQuickHelp;
	CUIStatic			UITextWound;
	CUIProgressBar		UIHealthBar;
	CUIProgressBar		UIArmorBar;
	CUIProgressBar		UIBatteryBar;

	CUIZoneMap			UIZoneMap;

	//������, ������������ ���������� �������� PDA
	CUIStatic			UIPdaOnline;
	//������ �������� ��������� PDA
	CUIListWnd			UIPdaMsgListWnd;
	// ������ �������� �������������� ���������
	CUIListWnd			UIInfoMessages;
	
	//�������� ����������� ���������
	int					m_dwMaxShowTime;
	int					m_iInfosShowTime;

	//����������� ������
	CUIStatic			UIWeaponBack;
	CUIStatic			UIWeaponSignAmmo;
	CUIStatic			UIWeaponIcon;

	// ��� ������������ ������� �������
	CUIMultiTextStatic	UIMoneyIndicator;
	
	//������ �������, ���������� �����, ������� ����� ����������
	//�� ������� �����
	CUSTOM_TEXTURE_VECTOR m_CustomTextures;	
public:
	// �������� ��������� �������� ���������� �����
	void ChangeTotalMoneyIndicator(shared_str newMoneyString);
	// �������� (� ���������) ��������� ������������/������� �������
	void DisplayMoneyChange(shared_str deltaMoney);
protected:

	// ������� armor & health
//	CUIStatic			UIHealthString;
//	CUIStatic			UIArmorString;
	//CUIDragDropItem	UIWeaponIcon;

	// 5 �������� ��� ����������� ������:
	// - ���������� ������
	// - ��������
	// - �������
	// - ������
	// - ���������
	CUIStatic			UIWeaponJammedIcon;
	CUIStatic			UIRadiaitionIcon;
	CUIStatic			UIWoundIcon;
	CUIStatic			UIStarvationIcon;
	CUIStatic			UIFatigueIcon;

public:
	
	// ����� �������������� ��������������� ������� 
	enum EWarningIcons
	{
		ewiAll				= 0,
		ewiWeaponJammed,
		ewiRadiation,
		ewiWound,
		ewiStarvation,
		ewiFatigue
	};

	// ������ ���� ��������������� ������
	void SetWarningIconColor(EWarningIcons icon, const u32 cl);
	void TurnOffWarningIcon(EWarningIcons icon);

	// ������ ��������� ����� �����������, ����������� �� system.ltx
	typedef std::map<EWarningIcons, xr_vector<float> >	Thresholds;
	typedef Thresholds::iterator						Thresholds_it;
	Thresholds	m_Thresholds;

	// ���� ������������ ��������� �������� ������
	enum EFlashingIcons
	{
		efiPdaTask	= 0,
		efiMail
	};
	
	// ���/���� �������� ������
	void SetFlashIconState(EFlashingIcons type, bool enable);

	//
	void AnimateContacts();

	// ���������� ������� ��������� �������
	void OnNewsReceived	(GAME_NEWS_DATA &news);
	
protected:

	void				InitFlashingIcons(CUIXml &node);
	void				DestroyFlashingIcons();
	void				UpdateFlashingIcons();

	// first - ������, second - ��������
	typedef				std::pair<CUIStatic*, CUIColorAnimatorWrapper> IconInfo;
	DEF_MAP				(FlashingIcons, EFlashingIcons, IconInfo);
	FlashingIcons		m_FlashingIcons;

	//��� �������� ��������� ������ � ������
	CActor*				m_pActor;	
	CWeapon*			m_pWeapon;
	int					m_iWeaponIconX;
	int					m_iWeaponIconY;
	int					m_iWeaponIconWidth;
	int					m_iWeaponIconHeight;

	// ��������� ��� ��������� fadein/fadeout ������������ ���������
	float				fuzzyShowInfo;
	// ����������� ��������� ��� ��������� ������� �� ������
	void				RenderQuickInfos();
	// ���������� ������������ ��������� ����� ��� ������� �����
	static void			FadeUpdate(CUIListWnd *pWnd, int fadeDuration);

	// ��� ���� ��������� �� ������ ������������� ����� ������������ ������������ �������
	int					m_iPdaMessagesFade_mSec;
	int					m_iInfoMessagesFade_mSec;
	int					m_iChatMessagesFade_mSec;

	// ������� ��� ���������
	CUIColorAnimatorWrapper		UIContactsAnimation;

private:
	// ���� �������� ������ � ����������-��������� ������ ������ �� ������(��� � Doom 3)
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
	// ���������������� �������� ������ ��� ��������
	void				AddMonsterClawsEffect(const shared_str &monsterName, const shared_str &textureName);
	// ��������� �������� �������� ������ �������
	void				PlayClawsAnimation(const shared_str &monsterName);
	// ���������� ������� ������ ���������
	void				SetBatteryCharge(float value);
	// ��������/�������� ���������
	void				ShowBattery(bool on);

	// Multiplayer chat log
	CUIListWnd			UIMPChatLog;
};