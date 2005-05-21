#include "stdafx.h"
#include "MainUI.h"
#include "UI/UIDialogWnd.h"
#include "UICursor.h"
#include "HUDManager.h"
#include "../xr_IOConsole.h"
#include "../IGame_Level.h"
#include "xr_Level_controller.h"
#include <dinput.h>


extern CUICursor*	GetUICursor(){return UI()->GetUICursor();};
extern CMainUI*		UI(){return (CMainUI*)(g_pGamePersistent->m_pMainUI);};

//----------------------------------------------------------------------------------
void S2DVert::rotate_pt(const Fvector2& pivot, float cosA, float sinA)
{
	Fvector2 t		= pt;
	t.sub			(pivot);
	pt.x			= t.x*cosA+t.y*sinA;
	pt.y			= t.y*cosA-t.x*sinA;
	pt.add			(pivot);
}
void C2DFrustum::CreateFromRect(const Irect& rect)
{
	m_rect.set(float(rect.x1), float(rect.y1), float(rect.x2), float(rect.y2) );
	planes.resize	(4);
	planes[0].build(Fvector2().set(rect.lt.x,rect.lt.y),Fvector2().set(-1, 0));
	planes[1].build(Fvector2().set(rect.lt.x,rect.lt.y),Fvector2().set( 0,-1));
	planes[2].build(Fvector2().set(rect.rb.x,rect.rb.y),Fvector2().set(+1, 0));
	planes[3].build(Fvector2().set(rect.rb.x,rect.rb.y),Fvector2().set( 0,+1));
}

sPoly2D* C2DFrustum::ClipPoly(sPoly2D& S, sPoly2D& D) const
{

	bool bFullTest		= false;
	for (u32 j=0; j<S.size(); j++){
		if( !m_rect.in(S[j].pt) ){
			bFullTest	= true;
			break;
		}
	}

	sPoly2D*	src		= &D;
	sPoly2D*	dest	= &S;
	if(bFullTest){
		for (u32 i=0; i<planes.size(); i++){
			// cache plane and swap lists
			const Fplane2 &P= planes[i];
			std::swap		(src,dest);
			dest->clear		();

			// classify all points relative to plane #i
			float cls[UI_FRUSTUM_SAFE];
			for (u32 j=0; j<src->size(); j++) cls[j]=P.classify((*src)[j].pt);

			// clip everything to this plane
			cls[src->size()] = cls[0];
			src->push_back((*src)[0]);
			Fvector2 dir_pt,dir_uv; float denum,t;
			for (j=0; j<src->size()-1; j++){
				if ((*src)[j].pt.similar((*src)[j+1].pt,EPS_S)) continue;
				if (negative(cls[j])){
					dest->push_back((*src)[j]);
					if (positive(cls[j+1])){
						// segment intersects plane
						dir_pt.sub((*src)[j+1].pt,(*src)[j].pt);
						dir_uv.sub((*src)[j+1].uv,(*src)[j].uv);
						denum = P.n.dotproduct(dir_pt);
						if (denum!=0) {
							t = -cls[j]/denum; //VERIFY(t<=1.f && t>=0);
							dest->last().pt.mad((*src)[j].pt,dir_pt,t);
							dest->last().uv.mad((*src)[j].uv,dir_uv,t);
							dest->inc();
						}
					}
				} else {
					// J - outside
					if (negative(cls[j+1])){
						// J+1  - inside
						// segment intersects plane
						dir_pt.sub((*src)[j+1].pt,(*src)[j].pt);
						dir_uv.sub((*src)[j+1].uv,(*src)[j].uv);
						denum = P.n.dotproduct(dir_pt);
						if (denum!=0){
							t = -cls[j]/denum; //VERIFY(t<=1.f && t>=0);
							dest->last().pt.mad((*src)[j].pt,dir_pt,t);
							dest->last().uv.mad((*src)[j].uv,dir_uv,t);
							dest->inc();
						}
					}
				}
			}

			// here we end up with complete polygon in 'dest' which is inside plane #i
			if (dest->size()<3) return 0;
		}
	}
	return dest;
}

//----------------------------------------------------------------------------------
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

	ShaderMap_it it;
	for (it = m_shaders.begin(); it != m_shaders.end(); it++)
	{
		Msg("-- UI shader holder: freeing of shader %s", it->first.c_str());
		it->second.destroy();
	}
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

		m_Flags.set					(flRestoreCursor,GetUICursor()->IsVisible());

		if(!m_Flags.is(flRestorePause))
			Device.Pause			(TRUE);

		StartStopMenu				(m_startDialog,true);
		if(g_pGameLevel){
			Device.seqFrame.Remove	(g_pGameLevel);
			Device.seqRender.Remove	(g_pGameLevel);
		};
		Device.seqRender.Add		(this);
	}else{
		m_Flags.set					(flActive,				FALSE);
		m_Flags.set					(flNeedChangeCapture,	TRUE);

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
	
		if(m_Flags.is(flRestoreCursor))
			GetUICursor()->Show();
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

	if(key_binding[dik]== kCONSOLE){
		Console->Show();
		return;
	}
	if (DIK_F12 == dik){
		Render->Screenshot();
		return;
	}


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
	if(m_Flags.is(flGameSaveScreenshot)){
		return;
	};
	DoRenderDialogs();

	m_pFontManager->Render();
	if(	GetUICursor()->IsVisible())
		GetUICursor()->Render();
}

//pureFrame
void	CMainUI::OnFrame		(void)
{
	m_2DFrustum.CreateFromRect	(Irect().set(0,0,Device.dwWidth,Device.dwHeight));
	if(!IsActive() && m_startDialog){
		xr_delete					(m_startDialog);
	}
	if (m_Flags.is(flNeedChangeCapture)){
		m_Flags.set					(flNeedChangeCapture,FALSE);
		if (m_Flags.is(flActive))	IR_Capture();
		else						IR_Release();
	}
	CDialogHolder::OnFrame		();


	//screenshot stuff
	if(m_Flags.is(flGameSaveScreenshot) && Device.dwFrame > m_screenshotFrame  ){
		m_Flags.set					(flGameSaveScreenshot,FALSE);
		::Render->Screenshot		(IRender_interface::SM_FOR_GAMESAVE, m_screenshot_name);
		
		if(g_pGameLevel && m_Flags.is(flActive)){
			Device.seqFrame.Remove	(g_pGameLevel);
			Device.seqRender.Remove	(g_pGameLevel);
		};

		if(m_Flags.is(flRestoreConsole))
			Console->Show			();
	}

}

void CMainUI::OnDeviceCreate()
{
}

void CMainUI::ClientToScreenScaled(Fvector2& dest, float left, float top, u32 align)
{
	dest.set(ClientToScreenScaledX(left,align),	ClientToScreenScaledY(top,align));
}

float CMainUI::ClientToScreenScaledX(float left, u32 align)
{
	return left * GetScaleX();
}

float CMainUI::ClientToScreenScaledY(float top, u32 align)
{
	return top * GetScaleY();
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

Irect CMainUI::ScreenRect()
{
	return Irect().set(0,0,UI_BASE_WIDTH,UI_BASE_HEIGHT);
}

void CMainUI::PushScissor(const Irect& r_tgt, bool overlapped)
{
	Irect r_top			= ScreenRect();
	Irect result		= r_tgt;
	if (!m_Scissors.empty()&&!overlapped){
		r_top			= m_Scissors.top();
	}
	if (!result.intersection(r_top,r_tgt))
			result.set	(0,0,0,0);

	VERIFY(result.x1>=0&&result.y1>=0&&result.x2<=UI_BASE_WIDTH&&result.y2<=UI_BASE_HEIGHT);
	m_Scissors.push		(result);

	result.lt.x 		= iFloor(ClientToScreenScaledX(float(result.lt.x),0));
	result.lt.y 		= iFloor(ClientToScreenScaledY(float(result.lt.y),0));
	result.rb.x 		= iFloor(ClientToScreenScaledX(float(result.rb.x),0));
	result.rb.y 		= iFloor(ClientToScreenScaledY(float(result.rb.y),0));

	VERIFY(result.x1>=0&&result.y1>=0&&(result.x2<=UI_BASE_WIDTH*GetScaleX())&&(result.y2<=UI_BASE_HEIGHT*GetScaleY()));
	RCache.set_Scissor	(&result);
}

void CMainUI::PopScissor()
{
	VERIFY(!m_Scissors.empty());
	m_Scissors.pop		();
	
	if(m_Scissors.empty())
		RCache.set_Scissor(NULL);
	else{
		const Irect& top= m_Scissors.top();
		Irect tgt;
		tgt.lt.x 		= iFloor(ClientToScreenScaledX(float(top.lt.x),0));
		tgt.lt.y 		= iFloor(ClientToScreenScaledY(float(top.lt.y),0));
		tgt.rb.x 		= iFloor(ClientToScreenScaledX(float(top.rb.x),0));
		tgt.rb.y 		= iFloor(ClientToScreenScaledY(float(top.rb.y),0));
		RCache.set_Scissor(&tgt);
	}
}

void CMainUI::Screenshot						(IRender_interface::ScreenshotMode mode, LPCSTR name)
{
	if(mode != IRender_interface::SM_FOR_GAMESAVE){
		::Render->Screenshot		(mode,name);
	}else{
		m_Flags.set					(flGameSaveScreenshot, TRUE);
		strcpy(m_screenshot_name,name);
		if(g_pGameLevel && m_Flags.is(flActive)){
			Device.seqFrame.Add		(g_pGameLevel);
			Device.seqRender.Add	(g_pGameLevel);
		};
		m_screenshotFrame			= Device.dwFrame+1;
		m_Flags.set					(flRestoreConsole,		Console->bVisible);
		Console->Hide				();
	}
}
