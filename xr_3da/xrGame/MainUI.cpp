#include "stdafx.h"
#include "MainUI.h"
#include "UI/UIDialogWnd.h"
#include "UICursor.h"
#include "HUDManager.h"
#include "../xr_IOConsole.h"
#include "../IGame_Level.h"
#include "xr_Level_controller.h"


extern CUICursor*	GetUICursor(){return UI()->GetUICursor();};
extern CMainUI*		UI(){return (CMainUI*)(g_pGamePersistent->m_pMainUI);};


CMainUI::CMainUI	()
{
	m_Flags.zero				();
	m_pFontManager				= xr_new<CFontManager>();
	m_pUICursor					= xr_new<CUICursor>();
	m_startDialog				= NULL;
	g_pGamePersistent->m_pMainUI= this;
	if (Device.bReady)			OnDeviceCreate();
}

CMainUI::~CMainUI	()
{
	xr_delete						(m_startDialog);
	xr_delete						(m_pUICursor);
	xr_delete						(m_pFontManager);
	g_pGamePersistent->m_pMainUI	= NULL;
}
void CMainUI::Activate	(bool bActivate)
{
	if(!!m_Flags.is(flActive) == bActivate)	return;
		
	if(bActivate){
		m_Flags.set					(flActive|flNeedChangeCapture,TRUE);
		DLL_Pure* dlg = NEW_INSTANCE (TEXT2CLSID("MAIN_MNU"));
		if(!dlg) {
			m_Flags.set				(flActive|flNeedChangeCapture,FALSE);
			return;
		}
		m_startDialog = smart_cast<CUIDialogWnd*>(dlg);
		VERIFY(m_startDialog);

		m_Flags.set					(flRestoreConsole,Console->bVisible);
		m_Flags.set					(flRestorePause,Device.Pause());
		Console->Hide				();

		if(!m_Flags.is(flRestorePause))
			Device.Pause			(TRUE);

		StartStopMenu				(m_startDialog,true);
		if(g_pGameLevel){
			Device.seqFrame.Remove	(g_pGameLevel);
			Device.seqRender.Remove	(g_pGameLevel);
		};
		Device.seqRender.Add		(this);
	}else{
		m_Flags.set					(flActive,FALSE);
		m_Flags.set					(flNeedChangeCapture,TRUE);

		Device.seqRender.Remove		(this);
		
		bool b = !!Console->bVisible;
		if(b){
			Console->Hide			();
		}

		IR_Release					();
		if(b){
			Console->Show			();
		}

		CleanInternals				();
		if(g_pGameLevel){
			Device.seqFrame.Add		(g_pGameLevel);
			Device.seqRender.Add	(g_pGameLevel);
		};
		if(m_Flags.is(flRestoreConsole))
			Console->Show			();

		if(!m_Flags.is(flRestorePause))
			Device.Pause(FALSE);

	}
}
bool CMainUI::IsActive	()
{
	return !!m_Flags.is(flActive);
}


//IInputReceiver
static int mouse_button_2_key []	=	{MOUSE_1,MOUSE_2,MOUSE_3};
void	CMainUI::IR_OnMousePress				(int btn)	
{	
	if(!IsActive()) return;
	IR_OnKeyboardPress(mouse_button_2_key[btn]);
};

void	CMainUI::IR_OnMouseRelease				(int btn)	
{
	if(!IsActive()) return;
	IR_OnKeyboardRelease(mouse_button_2_key[btn]);
};
void	CMainUI::IR_OnMouseHold					(int btn)	
{
	if(!IsActive()) return;
	IR_OnKeyboardHold(mouse_button_2_key[btn]);
};

void	CMainUI::IR_OnMouseMove					(int x, int y)
{
	if(!IsActive()) return;
		MainInputReceiver()->IR_OnMouseMove(x, y);
};

void	CMainUI::IR_OnMouseStop					(int x, int y)
{
	if(!IsActive()) return;
};

void	CMainUI::IR_OnKeyboardPress				(int dik)
{
	if(!IsActive()) return;
	MainInputReceiver()->IR_OnKeyboardPress( dik);
};

void	CMainUI::IR_OnKeyboardRelease			(int dik)
{
	if(!IsActive()) return;
	MainInputReceiver()->IR_OnKeyboardRelease(dik);
};

void	CMainUI::IR_OnKeyboardHold				(int dik)	
{
	if(!IsActive()) return;
};

void CMainUI::IR_OnMouseWheel(int direction)
{
	if(!IsActive()) return;
	MainInputReceiver()->IR_OnMouseWheel(direction);
}

//pureRender
void	CMainUI::OnRender		(void)
{
	DoRenderDialogs();

	m_pFontManager->Render();
	if(	GetUICursor()->IsVisible())
		GetUICursor()->Render();
}

//pureFrame
void	CMainUI::OnFrame		(void)
{
	if(!IsActive() && m_startDialog){
//		Device.seqFrame.Remove		(this);
//		Device.seqRender.Remove		(this);
//		IR_Release					();
		xr_delete					(m_startDialog);
	}
	if (m_Flags.is(flNeedChangeCapture)){
		m_Flags.set					(flNeedChangeCapture,FALSE);
		if (m_Flags.is(flActive))	IR_Capture();
		else						IR_Release();
	}
	CDialogHolder::OnFrame();
}






void CMainUI::OnDeviceCreate()
{
/*	if (Device.dwWidth!=UI_BASE_WIDTH)	SetScaleXY(float(Device.dwWidth)/float(UI_BASE_WIDTH), float(Device.dwHeight)/float(UI_BASE_HEIGHT));
	else								SetScaleXY(1.f,1.f);
*/
}
/*
void CMainUI::SetScaleXY(float x, float y){
	m_fDevScaleX			= x;
	m_fDevScaleY			= y;
}*/

void CMainUI::ClientToScreenScaled(Irect& r, u32 align)
{
	r.x1 = ClientToScreenScaledX(r.x1,align); 
	r.y1 = ClientToScreenScaledY(r.y1,align); 
	r.x2 = ClientToScreenScaledX(r.x2,align); 
	r.y2 = ClientToScreenScaledY(r.y2,align); 
}

void CMainUI::ClientToScreenScaled(Ivector2& dest, int left, int top, u32 align)
{
	dest.set(ClientToScreenScaledX(left,align),	ClientToScreenScaledY(top,align));
}

int CMainUI::ClientToScreenScaledX(int left, u32 align)
{
	return iFloor( left * GetScaleX() );

/*	if (align&alRight)	return iFloor(Device.dwWidth-UI_BASE_WIDTH*m_fDevScaleX + left*m_fDevScaleX);
	else				return iFloor(left*m_fDevScaleX);
*/
}

int CMainUI::ClientToScreenScaledY(int top, u32 align)
{
	return iFloor( top * GetScaleY() );
/*
	if (align&alBottom)	return iFloor(Device.dwHeight-UI_BASE_HEIGHT*m_fDevScaleY + top*m_fDevScaleY);
	else				return iFloor(top*m_fDevScaleY);
*/
}

void CMainUI::ClientToScreen(Ivector2& dest, int left, int top, u32 align)
{
	dest.set(ClientToScreenX(left,align),	ClientToScreenY(top,align));
}

void CMainUI::ClientToScreen(Irect& r, u32 align)
{
	r.x1 = ClientToScreenX(r.x1,align); 
	r.y1 = ClientToScreenY(r.y1,align); 
	r.x2 = ClientToScreenX(r.x2,align); 
	r.y2 = ClientToScreenY(r.y2,align); 
}

int CMainUI::ClientToScreenX(int left, u32 align)
{
	return left;
/*
	if (align&alRight)	return iFloor(Device.dwWidth-UI_BASE_WIDTH*m_fDevScaleX + left);
	else				return left;*/
}

int CMainUI::ClientToScreenY(int top, u32 align)
{
	return top;
/*
	if (align&alBottom)	return iFloor(Device.dwHeight-UI_BASE_HEIGHT*m_fDevScaleY + top);
	else				return top;*/
}

void CMainUI::OutText(CGameFont *pFont, Irect r, float x, float y, LPCSTR fmt, ...)
{
	if (r.in(static_cast<int>(x), static_cast<int>(y)))
	{
		R_ASSERT(pFont);
		va_list	lst;
		static string512 buf;
		::ZeroMemory(buf, 512);
		xr_string str;

		va_start(lst, fmt);
		vsprintf(buf, fmt, lst);
		str += buf;
		va_end(lst);

		// Rescale position in lower resolution
		if (x >= 1.0f && y >= 1.0f)
		{
			x *= GetScaleX();
			y *= GetScaleY();
		}

		pFont->Out(x, y, str.c_str());
	}
}

