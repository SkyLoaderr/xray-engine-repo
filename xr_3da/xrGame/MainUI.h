#pragma once

struct CFontManager;
class CUIWindow;
class CUIDialogWnd;
class CUICursor;
#include "../IInputReceiver.h"
#include "../IGame_Persistent.h"
#include "UIDialogHolder.h"
#include "UI.h"

class CMainUI :
	public IMainUI,
	public IInputReceiver,
	public pureRender,
	public CDialogHolder

{
	CFontManager*									m_pFontManager;
	CUICursor*										m_pUICursor;

	CUIDialogWnd*									m_startDialog;

//	float											m_fDevScaleX;
//	float											m_fDevScaleY;

	enum{
		flRestoreConsole	= (1<<0),
		flRestorePause		= (1<<1),
		flActive			= (1<<2),
		flNeedChangeCapture	= (1<<3),
	};
	Flags8											m_Flags;
	xr_stack<Irect> m_Scissors;
public:
					CMainUI							();
	virtual			~CMainUI						();
	CUICursor*		GetUICursor						()							{return m_pUICursor;}

	virtual void	Activate						(bool bActive); 
	virtual	bool	IsActive						(); 

	CFontManager*	Font							()							{return m_pFontManager;}
	//IInputReceiver
	virtual void	IR_OnMousePress					(int btn);
	virtual void	IR_OnMouseRelease				(int btn);
	virtual void	IR_OnMouseHold					(int btn);
	virtual void	IR_OnMouseMove					(int x, int y);
	virtual void	IR_OnMouseStop					(int x, int y);

	virtual void	IR_OnKeyboardPress				(int dik);
	virtual void	IR_OnKeyboardRelease			(int dik);
	virtual void	IR_OnKeyboardHold				(int dik);

	virtual void	IR_OnMouseWheel					(int direction)	;

	//pureRender
	virtual void	OnRender						(void);

	//pureFrame
	virtual void	OnFrame							(void);


	void			OutText							(CGameFont *pFont, Irect r, float x, float y, LPCSTR fmt, ...);

	void			OnDeviceCreate					();

	float			GetScaleX						()							{return float(Device.dwWidth)/float(UI_BASE_WIDTH);   }
	float			GetScaleY						()							{return float(Device.dwHeight)/float(UI_BASE_HEIGHT); }
//	void			SetScaleXY						(float f_x, float f_y);

	void			ClientToScreenScaled			(Irect& r, u32 align);
	void			ClientToScreenScaled			(Ivector2& dest, int left, int top, u32 align);
	int				ClientToScreenScaledX			(int left, u32 align);
	int				ClientToScreenScaledY			(int top, u32 align);
	void			ClientToScreen					(Irect& r, u32 align);
	void			ClientToScreen					(Ivector2& dest, int left, int top, u32 align);
	int				ClientToScreenX					(int left, u32 align);
	int				ClientToScreenY					(int top, u32 align);

	Irect			ScreenRect						();
	void			PushScissor						(const Irect& r, bool overlapped=false);
	void			PopScissor						();
};


extern CUICursor*	GetUICursor();
extern CMainUI*		UI();
