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

#include "../UIZoneMap.h"

#include "../actor.h"
#include "../weapon.h"
#include "../ai_alife_space.h"

//��� ������ ��������� HUD
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

	//��� ����������� ��������� ���������� �� PDA
	void ReceivePdaMessage(CInventoryOwner* pSender, EPdaMsg msg, int info_index);
	
	void AddGameMessage	(CInventoryOwner* pSender, LPCSTR TextMessage);

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
	// ������ ������� ��� ����������� ��������� �������� ��� ��������� ������� �� ������
	// ������ ������, ��� � ������� ��� ��� ������� ������������ ������
	CUIButton	UIStaticQuickHelp;
	
	CUIStatic	UITextWound;

	CUIProgressBar UIHealthBar;

	CUIZoneMap UIZoneMap;

	//������, ������������ ���������� �������� PDA
	CUIStatic			UIPdaOnline;
	//������ �������� ��������� PDA
	CUIListWnd			UIPdaMsgListWnd;
	
	//�������� ����������� ���������
	int					m_dwMaxShowTime;
	//����� ������� ������������ ����� ��������� PDA
	int					m_dwMsgShowingTime;

	//����������� ������
	CUIStatic			UIWeaponBack;
	CUIStatic			UIWeaponSignAmmo;
	CUIStatic			UIWeaponSignName;
	CUIStatic			UIWeaponIcon;
	CUIStatic			UIHealthString;
	//CUIDragDropItem	UIWeaponIcon;

	

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

	// ������������� ������ � �������. ����� ��� �� map, ��� ��� ���������� ���������� 1 ���,
	// � ����� ����� ������������� �����.
	std::map<int, ref_str>	m_strTips;

	// ��� ���� ��������� �� ������ ������������� ����� ������������ ������������ �������
	int					m_iFade_mSec;
	
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
	void OnNewsReceived(const ALife::SGameNews &newsItem);
	// ��������� �� ��������� �� ����� �����
	// Return:	true - ����� ������� ����
	bool CheckForNewNews();

	// ������ �������� ������ � �������������
	static const int						NEWS_CHECK_INTERVAL = 1000;
	ALife::_TIME_ID							m_iPrevTime;

};