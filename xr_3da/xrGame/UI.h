#pragma once

#include "UICursor.h"

//#include "ui\UIMainIngameWnd.h"
#include "UIDialogHolder.h"

#define UI_BASE_WIDTH	1024.0f
#define UI_BASE_HEIGHT	768.0f

// refs
class CHUDManager;
class CWeapon;
class CUIGameCustom;
class CUIMainIngameWnd;
class CUIMessagesWindow;

#define MAX_GROUPS		10
#define MAX_UIMESSAGES	7
#define LIFE_TIME		7.f		// sec
#define HIDE_TIME		2.5f	// sec 


class CDeviceResetNotifier :public pureDeviceReset
{
public:
						CDeviceResetNotifier					();
	virtual				~CDeviceResetNotifier					();
	virtual void		OnDeviceReset							()	{};

};



class CUI			: public CDialogHolder //ISheduled
{
	typedef ISheduled inherited;

	//whether to show main ingame indicators (health, weapon etc)
	bool					m_bShowIndicators;


	CUIGameCustom*			pUIGame;


	bool					m_bCrosshair;			//��� �� ������� ������-������ HUD ����� ������� ����


public:
	CHUDManager*			m_Parent;
	CUIMainIngameWnd*		UIMainIngameWnd;
	CUIMessagesWindow*		m_pMessagesWnd;
public:
							CUI						(CHUDManager* p);
	virtual					~CUI					();

	bool					Render					();
	void					UIOnFrame				();

	void					Load					();

	bool					IR_OnKeyboardHold		(int dik);
	bool					IR_OnKeyboardPress		(int dik);
	bool					IR_OnKeyboardRelease	(int dik);
	bool					IR_OnMouseMove			(int,int);
	bool					IR_OnMouseWheel			(int direction);

	CUIGameCustom*			UIGame					()					{return pUIGame;}


	void					ShowIndicators			()					{m_bShowIndicators = true;}
	void					HideIndicators			()					{m_bShowIndicators = false;}
	bool					IndicatorsHidden		()					{return m_bShowIndicators != true;};

	void					AddInfoMessage			(LPCSTR message);

	virtual void			StartStopMenu			(CUIDialogWnd* pDialog, bool bDoHideIndicators);
};

