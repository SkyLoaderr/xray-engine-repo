// XR_Options.cpp: implementation of the CXR_Options class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ddutil.h"
#include "XR_Options.h"
#include "x_ray.h"
#include "xr_ini.h"
#include "xr_menu.h"
#include "xr_ioconsole.h"
#include "xr_level_controller.h"
#include "xr_sndman.h"
#include "xr_msg.h"

static const char *szBOptions	= "data\\maps\\menu\\options.bmp";
static const char *szBOK		= "data\\maps\\menu\\ok.bmp";
static const char *szBCursor	= "data\\maps\\menu\\cursor.bmp";
static const char *szML			= "data\\maps\\menu\\ml_h.bmp";
static const char *szMR			= "data\\maps\\menu\\mr_h.bmp";
static const char *szMP			= "data\\maps\\menu\\mp.bmp";
static const char *szMPC		= "data\\maps\\menu\\mpc.bmp";

#define CSIZE	16
#define CSIZE2	(CSIZE+2)

HFONT			op_font = 0;
LOGFONT			op_font_def = {
	CSIZE, 0, 0, 0, 0, 0,	0, 0,
	ANSI_CHARSET,
	OUT_RASTER_PRECIS|OUT_DEVICE_PRECIS,
	CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY,
	FIXED_PITCH,
	"Arial"
};

//////////////////////////////////////////////////////////////////////
char *_get_VK_name(char * name) {
	for (int i=0; keybind[i].name; i++) {
		if (_stricmp(keybind[i].name,name)==0) {
			for (int j=0; j<2048; j++) {
				if (key_binding[j]==keybind[i].DIK) {
					for (int k=0; keynames[k].name; k++) {
						if (keynames[k].DIK==j) return keynames[k].name;
					}
				}
			}
		}
	}
	return "???";
}
char *_get_DIK_name(int dik) {
	for (int k=0; keynames[k].name; k++) {
		if (keynames[k].DIK==dik) return keynames[k].name;
	}
	return " ";
}

class CDIK : public CController
{
	char cmd[64],aname[64];

	virtual void OnInputActivate()
	{
		strcpy(aname, xrMenu->pOptions->items[xrMenu->pOptions->GetItem()].cmd);
		strcpy(cmd,"unbind ");
		strcat(cmd,aname);
		pConsole->Execute(cmd);
	};
	virtual void OnKeyboardPress ( int key )
	{
		iRelease();
		strcpy(cmd,"bind ");
		strcat(cmd,aname);
		strcat(cmd," ");
		strcat(cmd,_get_DIK_name(key));
		pConsole->Execute(cmd);
		xrMenu->pOptions->bCursorShow = true;
	};
	virtual void OnMousePress	(int btn)
	{
		switch (btn) {
		case 0: OnKeyboardPress(MOUSE_1); break;
		case 1: OnKeyboardPress(MOUSE_2); break;
		}
	};
} sCDIK;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXR_Options::CXR_Options(char *section)
{
	// Font
	if (op_font==0) op_font = CreateFontIndirect(&op_font_def);

	sscanf(pSettings->ReadSTRING("menu","options_ok").c_str(),
		"%d %d %d %d",&rOK.left, &rOK.top, &rOK.right, &rOK.bottom);

	CInifile::Sect&	S		= pSettings->ReadSection("levels");
	for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
		_menu_command cmd;
		sscanf(I->second.c_str(),"%d \"%[^\"]\" \"%[^\"]\" \"%[^\"]\"",&cmd.type,&cmd.name,&cmd.cmd,&cmd.hint);
		items.Add(cmd);
	}

	string &C = pSettings->ReadSTRING(section,"OnExit");
	sscanf(C.c_str(),"\"%[^\"]\"",szOnExit);

	//--- Load bitmaps
	OnDeviceCreate();

	//--- Init mouse
	Cursor = &(xrMenu->Cursor);

	// registrators
	Device.seqRender2D.Add(this, REG_PRIORITY_LOW-1);
	Device.seqDevCreate.Add(this);
	Device.seqDevDestroy.Add(this);
	bOkHighlight = false;
	cur_mi	= -1;
	bCursorShow = true;
	bIsChanges = false;
	iCapture();
}

CXR_Options::~CXR_Options()
{
	iRelease();
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);
	Device.seqRender2D.Remove(this);

	OnDeviceDestroy();
	if (bIsChanges && szOnExit[0]) pConsole->Execute(szOnExit);
}

void CXR_Options::OnDeviceCreate()
{
	pSurface	= LoadSurface(szBOptions);
	pMP			= LoadSurface(szMP);
	pMPC		= LoadSurface(szMPC);
	pML			= LoadSurface(szML);
	pMR			= LoadSurface(szMR);
	pOK			= LoadSurface(szBOK);
}

void CXR_Options::OnDeviceDestroy()
{
	_RELEASE(pSurface);
	_RELEASE(pMP);
	_RELEASE(pMPC);
	_RELEASE(pML);
	_RELEASE(pMR);
	_RELEASE(pOK);
}

#define SZ		15
#define M_X		(Device.dwWidth/2-256/2 + 20)
#define M_Y(a)	(Device.dwHeight/2-300/2 + 16 + SZ*(a))

void CXR_Options::OnRender2D()
{
	// Main screen
	DrawSurface(Device.dwWidth/2-256/2,Device.dwHeight/2-300/2,pSurface,szBOptions);

	RECT r;
	if (OnOK(r)){
		DrawSurface(r.left,r.top,pOK,szBOK);
		//if (!bOkHighlight) pSounds->PlayStatic(SND_MENU_HIGHLIGHT);
		bOkHighlight = true;
		xrMenu->ShowHint("Accept changes");
	}else{
		bOkHighlight = false;
	}

	for (int i=0; i<items.count; i++) {
		BOOL p;

		switch (items[i].type) {
		case mi_switch:
			p = pConsole->GetBool(items[i].cmd);
			DrawSurface(M_X+200,M_Y(i),p?pMPC:pMP,p?szMPC:szMP);
			break;
		case mi_scroll:
		case mi_list:
			DrawSurface(M_X+120,M_Y(i),pML,szML);
			DrawSurface(M_X+200,M_Y(i),pMR,szMR);
			break;
		case mi_kbd:

			break;
		}
	}

	// Strings
	int		hl	= GetItem();
	//if ((cur_mi!=hl)&&(hl>=0)) pSounds->PlayStatic(SND_MENU_HIGHLIGHT);
	cur_mi	= hl;
	if (cur_mi>=0) xrMenu->ShowHint(items[cur_mi].hint);
	HDC		hDC;
	CHK_DX(HW.pBB->GetDC(&hDC));
	SetBkMode	( hDC, TRANSPARENT);
	SelectObject( hDC, op_font);
	for (i=0; i<items.count; i++) {
		if (i==hl)	SetTextColor( hDC, RGB(0,255,0));
		else		SetTextColor( hDC, RGB(0,96,0));
		char *str = (char *)(&(items[i].name));
		ExtTextOut( hDC, M_X, M_Y(i) ,0, NULL, str, strlen(str), NULL );

		if (items[i].type==mi_scroll) {
			char buf[32];
			sprintf(buf,"%d%%",int(100.f*pConsole->GetFloat(items[i].cmd)));
			ExtTextOut( hDC, M_X+140, M_Y(i) ,0, NULL, buf, strlen(buf), NULL );
		}
		if (items[i].type==mi_list) {
			char *buf = pConsole->GetValue(items[i].cmd);
			ExtTextOut( hDC, M_X+140, M_Y(i) ,0, NULL, buf, strlen(buf), NULL );
		}
		if (items[i].type==mi_kbd) {
			char *buf = _get_VK_name(items[i].cmd);
			ExtTextOut( hDC, M_X+140, M_Y(i) ,0, NULL, buf, strlen(buf), NULL );
		}
	}
	CHK_DX(HW.pBB->ReleaseDC(hDC));
	if (bCursorShow) DrawSurface(int(X_TO_REAL(Cursor->x)),int(Y_TO_REAL(Cursor->y)),xrMenu->pCursorSurface,szBCursor);
}

void CXR_Options::OnKeyboardPress(int dik)
{
	switch (dik)
	{
	case DIK_ESCAPE:
	case DIK_RETURN:
	case DIK_SPACE:
		PutMessage(msg_hide_options); break;
	}
}

void CXR_Options::OnMouseMove	(int dx, int dy)
{
	Cursor->x+=float(dx)/320.0f;
	Cursor->y+=float(dy)/240.0f;
	if (Cursor->x<-1) Cursor->x=-1;
	if (Cursor->x> 1) Cursor->x=1;
	if (Cursor->y<-1) Cursor->y=-1;
	if (Cursor->y> 1) Cursor->y=1;
	//	DToF("Btn: ",DWORD(GetButton()));
}

void CXR_Options::OnMousePress(int btn)
{
	if (btn!=0) return;

	RECT r;
	if (OnOK(r)) {
		OnKeyboardPress(DIK_RETURN);
		//pSounds->PlayStatic(SND_MENU_UP_LEVEL);
		return;
	}

	int itm = GetItem();
	if (itm<0)	return;
	switch (items[itm].type)
	{
	case mi_cmd:
		pConsole->Execute(items[itm].cmd);
		bIsChanges = true;
		break;
	case mi_switch:
		{
			char buf[64];
			BOOL b = !pConsole->GetBool(items[itm].cmd);
			sprintf(buf,"%s %s",items[itm].cmd,b?"on":"off");
			pConsole->Execute(buf);
			//pSounds->PlayStatic(SND_MENU_SWITCH);
			bIsChanges = true;
		}
		break;
	case mi_list:
		{
			char buf[64];
			DWORD cx = int(X_TO_REAL(Cursor->x));
			char *v  = pConsole->GetValue(items[itm].cmd);
			if (cx>(M_X+120) && cx<(M_X+140)) {
				v = pConsole->GetPrevValue(items[itm].cmd);
			}
			if (cx>(M_X+200) && cx<(M_X+220)) {
				v = pConsole->GetNextValue(items[itm].cmd);
			}
			sprintf(buf,"%s %s",items[itm].cmd,v);
			pConsole->Execute(buf);
			//pSounds->PlayStatic(SND_MENU_SWITCH);
			bIsChanges = true;
		}
		break;
	case mi_scroll:
		{
			char buf[64];
			DWORD cx = int(X_TO_REAL(Cursor->x));
			float v = pConsole->GetFloat(items[itm].cmd);
			if (cx>(M_X+120) && cx<(M_X+140)) {
				v-=.05f;	if (v<0) v=0;
			}
			if (cx>(M_X+200) && cx<(M_X+220)) {
				v+=.05f;	if (v>1) v=1;
			}
			sprintf(buf,"%s %3.3f",items[itm].cmd,v);
			pConsole->Execute(buf);
			//pSounds->PlayStatic(SND_MENU_INC_DEC_SLIDER);
			bIsChanges = true;
		}
		break;
	case mi_kbd:
//			Items[iCommand]->SetText2("???");
			bCursorShow = false;
			sCDIK.iCapture();
			break;
		break;
	}
}

int CXR_Options::GetItem()
{
	DWORD cx = int(X_TO_REAL(Cursor->x));
	DWORD cy = int(Y_TO_REAL(Cursor->y));

	if ((cx<M_X)||(cx>M_X+230)) return -1;
	for (int i=0; i<items.count; i++) {
		DWORD mi	= M_Y(i);
		DWORD mip	= M_Y(i+1);
		if (
			(cy >= mi   ) &&
			(cy <  mip  )
		) return i;
	}
	return -1;
}
BOOL CXR_Options::OnOK(RECT &r)
{
	r.left  = rOK.left  + (Device.dwWidth/2-256/2);
	r.right = rOK.right + (Device.dwWidth/2-256/2);
	r.top	= rOK.top   + (Device.dwHeight/2-300/2);
	r.bottom= rOK.bottom+ (Device.dwHeight/2-300/2);

	int cx = int(X_TO_REAL(Cursor->x));
	int cy = int(Y_TO_REAL(Cursor->y));
	if (cx>=r.left && cx<=r.right && cy>=r.top && cy<=r.bottom) return true;
	return false;
}




