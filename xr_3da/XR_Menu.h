// XR_Menu.h: interface for the CXR_Menu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XR_MENU_H__5F7D4300_FDBC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_XR_MENU_H__5F7D4300_FDBC_11D3_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "fcontroller.h"
#include "xr_options.h"
#include "xr_list.h"

enum _buttons { b_none=0, b_usr_up, b_usr_down, b_usr_new, b_usr_del, b_lev_down, b_lev_up, b_exit, b_ok, ba_options, ba_users, ba_levels, b_last };

class CXR_Menu  :
	public pureRender,
	public pureRender2D,
	public pureDeviceCreate,
	public pureDeviceDestroy,
	public CController
{
	friend class CXR_Options;
	friend class CDIK;

	int						hFireTexture;
	LPDIRECTDRAWSURFACE7	pMenuSurface;
	LPDIRECTDRAWSURFACE7	pLightSurface;
	LPDIRECTDRAWSURFACE7	pCursorSurface;

	RECT					bounds[b_last];

	_cursor					Cursor;
	int						iCurrentUser;
	int						iCurrentLevel;
	BOOL					bEditor;
	char					cEditor[64];

	char					cHint[64];
	Fpoint					ptHint;

	int						cur_btn;
	int						cur_p;

	CList <char *>			Names;
	CList <char *>			Levels;
	CList <char *>			Options;
	char					mask[256][192];
	CXR_Options*			pOptions;

	void			AddToNames		(char *name);
	void			UpdateRects		(void);
	_buttons		GetButton		(void);
	int				GetH			(void);
	void			SelectUser		(BOOL b=false);

	void			ShowHint(LPSTR str) {strcpy(cHint,str);}
public:
	virtual void	OnDeviceDestroy	(void);
	virtual void	OnDeviceCreate	(void);
	virtual void	OnRender		(void);
	virtual void	OnRender2D		(void);
	virtual void	OnKeyboardPress	(int dik);
	virtual	void	OnMouseMove		(int dx, int dy);
	virtual void	OnMousePress	(int btn);

	void	HideOptions				(void)				{ _DELETE(pOptions); };

	CXR_Menu();
	~CXR_Menu();
};

extern CXR_Menu * xrMenu;

#endif // !defined(AFX_XR_MENU_H__5F7D4300_FDBC_11D3_B4E3_4854E82A090D__INCLUDED_)
