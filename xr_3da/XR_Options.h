// XR_Options.h: interface for the CXR_Options class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XR_OPTIONS_H__FB219AE0_04F1_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_XR_OPTIONS_H__FB219AE0_04F1_11D4_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "FController.h"
#include "xr_list.h"

enum	_menu_item { mi_unknown=0, mi_cmd, mi_switch, mi_scroll, mi_list, mi_kbd };
struct	_menu_command {
	_menu_item		type;		// type
	char			name[96];	// menu item
	char			cmd	[96];	// console command
	char			hint[256];	// hint
};
struct _cursor {
	float x, y;
};

class CXR_Options :
	public pureDeviceCreate,
	public pureDeviceDestroy,
	public pureRender2D,
	public CController
{
	friend	class CDIK;
	LPDIRECTDRAWSURFACE7	pSurface;
	LPDIRECTDRAWSURFACE7	pMP;
	LPDIRECTDRAWSURFACE7	pMPC;
	LPDIRECTDRAWSURFACE7	pML;
	LPDIRECTDRAWSURFACE7	pMR;
	LPDIRECTDRAWSURFACE7	pOK;

	CList <_menu_command>	items;
	_cursor*				Cursor;
	int						GetItem	(void);
	BOOL					OnOK	(RECT &r);
	char					szOnExit[64];
	RECT					rOK;
	BOOL					bCursorShow;

	BOOL					bOkHighlight;
	int						cur_mi;

	BOOL					bIsChanges;
public:
	virtual void OnDeviceDestroy	(void);
	virtual void OnDeviceCreate		(void);
	virtual void OnRender2D			(void);
	virtual void OnKeyboardPress	(int dik);
	virtual	void OnMouseMove		(int dx, int dy);
	virtual void OnMousePress		(int btn);

	CXR_Options(char *section);
	~CXR_Options();
};

#endif // !defined(AFX_XR_OPTIONS_H__FB219AE0_04F1_11D4_B4E3_4854E82A090D__INCLUDED_)
