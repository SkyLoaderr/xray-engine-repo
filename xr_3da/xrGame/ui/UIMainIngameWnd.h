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


//#include "../UIZoneMap.h"

#include "../actor.h"
#include "../weapon.h"
#include "../alife_space.h"

#include "xrXMLParser.h"
#include "UICarPanel.h"
#include "UIMotionIcon.h"
//////////////////////////////////////////////////////////////////////////

//��� ������ ��������� HUD
extern int				g_bHudAdjustMode;
extern float			g_fHudAdjustValue;
extern int				g_bNewsDisable;

class					CUIPdaMsgListItem;
class					CLAItem;
class					CUIZoneMap;
class					CUIArtefactPanel;
//////////////////////////////////////////////////////////////////////////

struct CUSTOM_TEXTURE
{
	CUSTOM_TEXTURE(CUIStaticItem* si, float left, float top, float right, float bottom, int priority = 0)
	{
		static_item		= si;
		x1				= left;
		y1				= top;
		x2				= right;
		y2				= bottom;
		texPriority		= priority;
	};
		
	CUIStaticItem*	static_item;
	float			x1, y1, x2, y2;
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
	virtual void Draw();
//	virtual void DrawPdaMessages();
	virtual void Update();

	bool OnKeyboardPress(int dik);

	// PDA MESSAGES
		//��� ����������� ��������� ���������� �� PDA
	void ReceivePdaMessage(CInventoryOwner* pSender, EPdaMsg msg, INFO_ID info_id);

	bool SetDelayForPdaMessage          (int iValue, int iDelay);
	void AddGameMessage	(LPCSTR message, int iId = -1, int iDelay = 0);
	void AddPersonalizedGameMessage		(CInventoryOwner* pSender, LPCSTR TextMessage, int iId = -1, int iDelay = 0);
	void AddIconedGameMessage			(LPCSTR textureName, Frect originalRect, LPCSTR message, int iId = -1, int iDelay = 0);
protected:
	CUIPdaMsgListItem* AddMessageToList(LPCSTR message, CUIListWnd* pListWnd, int iId, int iDelay);

public:
	void AddStaticItem					(CUIStaticItem* si, float left, float top, float right, float bottom, int priority = 0);
	// ������� ��� ������ ��������� ���������, ����� ��� "����� ����� ������",
	// "������ ����������", � �.�. ���������� ��������� �� ����������� �������
	void AddInfoMessage	(LPCSTR message);

protected:
	void HideAll();
	void ShowAll();
	bool m_bShowHudInfo;
	bool m_bShowHudCrosshair;
	
	CUIStatic			UIStaticDiskIO;
	CUIStatic			UIStaticHealth;
	CUIStatic			UIStaticArmor;
	CUIStatic			UIStaticMapBack;
	CUIStatic			UIStaticBattery;
	CUIStatic			UIStaticPower;    // delete me ! :P
	// ������ ������� ��� ����������� ��������� �������� ��� ��������� ������� �� ������
	// ������ ������, ��� � ������� ��� ��� ������� ������������ ������
	CUIButton			UIStaticQuickHelp;
	CUIStatic			UITextWound;
	CUIProgressBar		UIHealthBar;
	CUIProgressBar		UIArmorBar;
	CUIProgressBar		UIBatteryBar;
	CUICarPanel			UICarPanel;
	CUIMotionIcon		UIMotionIcon;	
	CUIZoneMap*			UIZoneMap;
	CUIProgressBar		UILuminosityBar;
	CUIProgressBar		UISndNoiseBar;

	//������, ������������ ���������� �������� PDA
	CUIStatic			UIPdaOnline;
	//������ �������� ��������� PDA
//	CUIListWnd			UIPdaMsgListWnd2;	
//	CUIListWnd			UIPdaMsgListWnd2; // We will draw this list if MainInGameWnd is hided
	// ������ �������� �������������� ���������
	CUIListWnd			UIInfoMessages;
	
	//�������� ����������� ���������
//	int					m_dwMaxShowTime;
//	int					m_iInfosShowTime;

	//����������� ������
	CUIStatic			UIWeaponBack;
	CUIStatic			UIWeaponSignAmmo;
	CUIStatic			UIWeaponIcon;

	// ��� ������������ ������� �������
	CUIMultiTextStatic	UIMoneyIndicator;
	CUIStatic			UITeam1Sign, UITeam1Score;
	CUIStatic			UITeam2Sign, UITeam2Score;
	//  [7/27/2005]
	CUIStatic			UIRankIndicator;
	//  [7/27/2005]
	
	//������ �������, ���������� �����, ������� ����� ����������
	//�� ������� �����
	CUSTOM_TEXTURE_VECTOR m_CustomTextures;	
public:
	// �������� ��������� �������� ���������� �����
	void ChangeTotalMoneyIndicator(shared_str newMoneyString);
	// �������� (� ���������) ��������� ������������/������� �������
	void DisplayMoneyChange(shared_str deltaMoney);
	CUIStatic*	GetPDAOnline	() { return &UIPdaOnline; };
	void	UpdateTeamsScore	(int t1, int t2);
	void	SetRank				(int rank);
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
	CUIStatic			UIInvincibleIcon;
	CUIWindow*			m_pMPChatWnd;
	CUIWindow*			m_pMPLogWnd;
public:	
	CUIArtefactPanel*    m_artefactPanel;
	
public:
	
	// ����� �������������� ��������������� ������� 
	enum EWarningIcons
	{
		ewiAll				= 0,
		ewiWeaponJammed,
		ewiRadiation,
		ewiWound,
		ewiStarvation,
		ewiFatigue,
		ewiInvincible,
	};

	void SetMPChatLog(CUIWindow* pChat, CUIWindow* pLog);

	// ������ ���� ��������������� ������
	void SetWarningIconColor(EWarningIcons icon, const u32 cl);
	void TurnOffWarningIcon(EWarningIcons icon);

	// ������ ��������� ����� �����������, ����������� �� system.ltx
	typedef xr_map<EWarningIcons, xr_vector<float> >	Thresholds;
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
	CInventoryItem*		m_pItem;

	float				m_iWeaponIconX;
	float				m_iWeaponIconY;
	float				m_iWeaponIconWidth;
	float				m_iWeaponIconHeight;

	// ��������� ��� ��������� fadein/fadeout ������������ ���������
	float				fuzzyShowInfo;
	// ����������� ��������� ��� ��������� ������� �� ������
	void				RenderQuickInfos();
	// ���������� ������������ ��������� ����� ��� ������� �����
	void				FadeUpdate(CUIListWnd *pWnd);//, int fadeDuration);

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

public:
	// ���������������� �������� ������ ��� ��������
	void				AddMonsterClawsEffect(const shared_str &monsterName, const shared_str &textureName);
	// ��������� �������� �������� ������ �������
	void				PlayClawsAnimation(const shared_str &monsterName);
	// ���������� ������� ������ ���������
	void				SetBatteryCharge(float value);
	// ��������/�������� ���������
	void				ShowBattery(bool on);
	CUICarPanel&		CarPanel(){return UICarPanel;};
	CUIMotionIcon&		MotionIcon(){return UIMotionIcon;}
protected:
	CInventoryItem*		m_pPickUpItem;
	CUIStatic			UIPickUpItemIcon;

	float					m_iPickUpItemIconX;
	float					m_iPickUpItemIconY;
	float					m_iPickUpItemIconWidth;
	float					m_iPickUpItemIconHeight;

    void				UpdatePickUpItem();
public:
	void				SetPickUpItem	(CInventoryItem* PickUpItem);

};