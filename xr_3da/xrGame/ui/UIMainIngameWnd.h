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

#include "../UIZoneMap.h"

#include "../actor.h"
#include "../weapon.h"
#include "../alife_space.h"

//��� ������ ��������� HUD
extern int				g_bHudAdjustMode;
extern float			g_fHudAdjustValue;
extern int				g_bNewsDisable;

struct CUSTOM_TEXTURE
{
	CUSTOM_TEXTURE(CUIStaticItem* si, int left, int top, int right, int bottom)
	{
		static_item = si;
		x1 = left;
		y1 = top;
		x2 = right;
		y2 = bottom;	
	};
		
	CUIStaticItem* static_item;
	int x1, y1, x2, y2;
};

DEFINE_VECTOR(CUSTOM_TEXTURE, CUSTOM_TEXTURE_VECTOR, CUSTOM_TEXTURE_IT);


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
	
	void AddGameMessage	(CInventoryOwner* pSender, LPCSTR TextMessage);
	void AddStaticItem	(CUIStaticItem* si, int left, int top, int right, int bottom);
	// ������� ��� ������ ��������� ���������, ����� ��� "����� ����� ������",
	// "������ ����������", � �.�.
	void AddInfoMessage	(LPCSTR message);

protected:
	void HideAll();
	void ShowAll();
	bool m_bShowHudInfo;
	bool m_bShowHudCrosshair;

	CUIStatic			UIStaticHealth;
	CUIStatic			UIStaticArmor;
	CUIStatic			UIStaticMapBack;

	// ������ ������� ��� ����������� ��������� �������� ��� ��������� ������� �� ������
	// ������ ������, ��� � ������� ��� ��� ������� ������������ ������
	CUIButton			UIStaticQuickHelp;
	
	CUIStatic			UITextWound;

	CUIProgressBar		UIHealthBar;
	CUIProgressBar		UIArmorBar;

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
	void ChangeTotalMoneyIndicator(ref_str newMoneyString);
	// �������� (� ���������) ��������� ������������/������� �������
	void DisplayMoneyChange(ref_str deltaMoney);
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

protected:

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
	void RenderQuickInfos();
	// ���������� ������������ ��������� ����� ��� ������� �����
	void FadeUpdate(CUIListWnd *pWnd, int fadeDuration);

	// ��� ���� ��������� �� ������ ������������� ����� ������������ ������������ �������
	int					m_iPdaMessagesFade_mSec;
	int					m_iInfoMessagesFade_mSec;
	
	//-----------------------------------------------------------------------------/
	//	News related routines
	//-----------------------------------------------------------------------------/

	// ������ ��������� ����������� ��������
	void LoadNewsTemplates();

	// ���������� ����������� ����
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

	// ���������� ������� ��������� �������
	void OnNewsReceived(const CALifeNews &newsItem);
	// ��������� �� ��������� �� ����� �����
	// Return:	true - ����� ������� ����
	bool CheckForNewNews();

	// ������ �������� ������ � �������������
	static const int						NEWS_CHECK_INTERVAL = 1000;
	ALife::_TIME_ID							m_iPrevTime;
};