#pragma once

#include "UICursor.h"

#include "ui\UIMainIngameWnd.h"
#include "UIDialogHolder.h"

#define UI_BASE_WIDTH	1024
#define UI_BASE_HEIGHT	768

// refs
class CHUDManager;
class CWeapon;
class CUIGameCustom;
//class CUIDialogWnd;

#define MAX_GROUPS		10
#define MAX_UIMESSAGES	7
#define LIFE_TIME		7.f		// sec
#define HIDE_TIME		2.5f	// sec 

struct SUIMessage
{
	LPSTR	sender;
	LPSTR	msg;
	u32		color;
	int		life_time;
	SUIMessage(LPCSTR S, LPCSTR M, u32 clr, float lt)
	{
		sender		= xr_strdup(	S);
		msg			= xr_strdup	(M);
		life_time	= iFloor	(lt*1000);
		color		= clr;
	}
	~SUIMessage()
	{
		xr_free(sender);
		xr_free(msg);
	}
};

DEFINE_SVECTOR(SUIMessage*,MAX_UIMESSAGES,UIMsgSVec,UIMsgIt);

class CUI			: public CDialogHolder //ISheduled
{
	typedef ISheduled inherited;

//	CUICursor				UICursor;


	//whether to show main ingame indicators (health, weapon etc)
	bool					m_bShowIndicators;


	CUIGameCustom*			pUIGame;

	// messages
	float					msgs_offs;
	float					menu_offs;
	UIMsgSVec				messages;

//	xr_stack<CUIDialogWnd*> m_input_receivers;
//	xr_vector<CUIWindow*>	m_dialogsToRender;
//	xr_vector<CUIWindow*>	m_dialogsToErase;
	bool					m_bCrosshair;			//��� �� ������� ������-������ HUD ����� ������� ����

//	void					StartMenu				(CUIDialogWnd* pDialog, bool bDoHideIndicators);
//	void					StopMenu				(CUIDialogWnd* pDialog, bool bDoHideIndicators);
//	void					SetMainInputReceiver	(CUIDialogWnd* ir);

public:
	CHUDManager*			m_Parent;

	CUIMainIngameWnd		UIMainIngameWnd;
public:
							CUI						(CHUDManager* p);
	virtual					~CUI					();

	bool					Render					();
	void					OnFrame					();

	void					Load					();

	bool					IR_OnKeyboardPress		(int dik);
	bool					IR_OnKeyboardRelease	(int dik);
	bool					IR_OnMouseMove			(int,int);

	CUIGameCustom*			UIGame					()					{return pUIGame;}
	// --- depends on game type
	// frag		(g_fraglimit)
	// time		(g_timelimit)
	// frag-list.....
	

//	void					ShowCursor				()					{UICursor.Show();}
//	void					HideCursor				()					{UICursor.Hide();}
//	CUICursor*				GetCursor				()					{return &UICursor;}

	void					ShowIndicators			()					{m_bShowIndicators = true;}
	void					HideIndicators			()					{m_bShowIndicators = false;}

	void					AddMessage				(LPCSTR S, LPCSTR M, u32 C=0xffffffff, float life_time=LIFE_TIME);

//	CUIDialogWnd*			MainInputReceiver		();
	virtual void			StartStopMenu			(CUIDialogWnd* pDialog, bool bDoHideIndicators);
//	void					AddDialogToRender		(CUIWindow* pDialog);
//	void					RemoveDialogToRender	(CUIWindow* pDialog);
//	virtual	float			shedule_Scale			();
//	virtual	void			shedule_Update			(u32 dt);

};

