// XR_Menu.cpp: implementation of the CXR_Menu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "ddutil.h"
#include "xr_ioconsole.h"
#include "xr_options.h"
#include "XR_Menu.h"
#include "fdemoplay.h"
#include "ffileops.h"
#include "x_ray.h"
#include "xr_sndman.h"
#include "xr_msg.h"
#include "xr_tokens.h"

CXR_Menu * xrMenu = NULL;

xr_token							btn_hint						[ ]={
	{ "Previous user",				b_usr_up 						},
	{ "Next user",					b_usr_down 						},
	{ "New user",					b_usr_new 						},
	{ "Delete selected user",		b_usr_del 						},
	{ "Next level",					b_lev_down 						},
	{ "Previous level",				b_lev_up 						},
	{ "Exit",						b_exit	 						},
	{ "Start GAME",					b_ok 							},
	{ "Show options",				ba_options 						},
	{ "Users",						ba_users 						},
	{ "Levels",						ba_levels 						},
	{ "Last",						b_last 							},
	{ 0,							0								}
};

// constants
static const char *szBMain		= "data\\maps\\menu\\main.bmp";
static const char *szBLight		= "data\\maps\\menu\\light.bmp";
static const char *szBCursor	= "data\\maps\\menu\\cursor.bmp";
static const char *szBMask		= "data\\maps\\menu\\menu.mask";

#define CSIZE	18
#define CSIZE2	(CSIZE+2)
#define USR_X	316
#define USR_Y	60

HFONT			menu_font = 0;
LOGFONT			menu_font_def = {
	CSIZE, 0, 0, 0, 0, 0,	0, 0,
		ANSI_CHARSET,
		OUT_RASTER_PRECIS|OUT_DEVICE_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FIXED_PITCH,
		"Arial"
};

extern char		psUserName			[64];
extern BOOL		psUserLevelsDone	[];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CXR_Menu::AddToNames(char *name) {
	char n[64];
	strcpy(n,name);
	strlwr(n);
	n[0]=toupper(n[0]);
	char *p = strchr(n,'.');
	if (p) *p=0;
	Names.Add(strdup(n));
	if (stricmp(n,psUserName)==0) iCurrentUser = Names.count-1;
}

CXR_Menu::CXR_Menu()
{
	hFireTexture = Device.Texture.Create("FIRE:uv128","vengine");

	VERIFY(HW.pDDraw);

	//pSounds->PlayStatic(SND_MENU_SHOW);
	// Visual
	pMenuSurface	= LoadSurface(szBMain);
	pLightSurface	= LoadSurface(szBLight);
	DDSetColorKey(pLightSurface,0);
	pCursorSurface	= LoadSurface(szBCursor);

	Cursor.x = Cursor.y = 0;

	// Font
	if (menu_font==0)
		menu_font = CreateFontIndirect(&menu_font_def);

	// Users
	iCurrentUser		= 0;
	bEditor				= false;
	cur_btn				= b_none;
	cur_p				= -1;

	int					hFile;
	struct  _finddata_t c_file;
    if( (hFile = _findfirst( "users\\*.user", &c_file )) != -1 )
    {
		AddToNames(c_file.name);
		while( _findnext( hFile, &c_file ) == 0 )            {
			AddToNames(c_file.name);
		}
		_findclose( hFile );
	}

	// mask
	char *m = (char *)DownloadFile((char *)szBMask);
	CopyMemory(mask,m,sizeof(mask));
	_FREE(m);
	UpdateRects();

	// Levels
	iCurrentLevel		= 0;
	for (DWORD i=1; i<pApp->Levels.size(); i++)
	{
		Levels.Add(pApp->Levels[i].caption);
	}

	// Options
	Options.Add("Video");
	Options.Add("Audio");
	Options.Add("Game options");
	Options.Add("Controls");
	Options.Add("Debug");
	pOptions = NULL;

	// registrators
	Device.seqRender.Add(this, REG_PRIORITY_LOW-1111);
	Device.seqRender2D.Add(this, REG_PRIORITY_LOW-0);
	Device.seqDevCreate.Add(this);
	Device.seqDevDestroy.Add(this);
	iCapture();

	sscanf(pSettings->ReadSTRING("menu", "hint_pos").c_str(),"%d %d",&ptHint.x,&ptHint.y);
	cHint[0] = 0;

	pConsole->Execute("i_visible off");
	SelectUser(true);
}

CXR_Menu::~CXR_Menu()
{
	iRelease();
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);
	Device.seqRender2D.Remove(this);
	Device.seqRender.Remove(this);

	// surfaces
	_RELEASE(pMenuSurface);
	_RELEASE(pLightSurface);
	_RELEASE(pCursorSurface);

	_DELETE	(pOptions);

	// users
	for (int i=0; i<Names.count; i++)
		_DELETE(Names[i]);

	pConsole->Execute("i_visible on");

	Device.Texture.Delete(hFireTexture);
}

void CXR_Menu::UpdateRects(void)
{
	for (int i=b_none; i<b_last; i++)
	{
		RECT r;
		r.left=99999;
		r.top=99999;
		r.right=-1;
		r.bottom=-1;
		for (int y=0; y<192; y++)
		{
			for (int x=0; x<256; x++)
			{
				_buttons b = (_buttons) mask[x][y];
				if (b==i) {
					int X=x*2;
					int Y=y*2;
					if (X<r.left)	r.left=X;
					if (X>r.right)	r.right=X;
					if (Y<r.top)	r.top=Y;
					if (Y>r.bottom)	r.bottom=Y;
				}
			}
		}
		//		r.left		= Device.dwWidth/2-512/2  + r.left;
		//		r.right		= Device.dwWidth/2-512/2  + r.right;
		//		r.top		= Device.dwHeight/2-384/2 + r.top;
		//		r.bottom	= Device.dwHeight/2-384/2 + r.bottom;
		bounds[i]=r;
		//		DWORDToFile
	}
}

_buttons CXR_Menu::GetButton()
{
	if (pOptions) return b_none;
	int x = int(X_TO_REAL(Cursor.x)) - int(Device.dwWidth/2-512/2);
	int y = int(Y_TO_REAL(Cursor.y)) - int(Device.dwHeight/2-384/2);
	if (x<0 || x>=512 || y<0 || y>=384) return b_none;
	return (_buttons) mask[x/2][y/2];
}
int CXR_Menu::GetH()
{
	if (GetButton()!=ba_options) return -1;
	int x = int(X_TO_REAL(Cursor.x));
	int y = int(Y_TO_REAL(Cursor.y));
	int ys= Device.dwHeight/2-384/2 + bounds[ba_options].top  + CSIZE2 - 5;

	int d = (y-ys)/CSIZE2;
	if (d<0 || d>=Options.count) return -1;
	return d;
}

BOOL CanPlay(int p) {
	int cnt=0;

	p++;
	if (p<0) return false;
	if (psUserLevelsDone[p]) return true; // Level already completed
	for (int i=p; i>0; i--)
	{
		if (!psUserLevelsDone[i]) cnt++;
	}
	if (cnt<3) return true;
	return false;
}

#define H_X(x)		(Device.dwWidth/2*((x)+1.f))
#define H_Y(y)		(Device.dwHeight/2*((y)+1.f))

void CXR_Menu::OnRender()
{
	FTLvertex shadow[4];
	FPcolor dwColor;
	dwColor.set(196,196,196,196);
	shadow[0].set(0,float(Device.dwHeight),0.95f,1.0f,dwColor,dwColor,						0, .9f);	// LB
	shadow[1].set(0,0,0.95f,1.0f,dwColor,dwColor,											0, 0);		// LT
	shadow[2].set(float(Device.dwWidth),float(Device.dwHeight),0.95f,1.0f,dwColor,dwColor,	1, .9f);	// RB
	shadow[3].set(float(Device.dwWidth),0,0.95f,1.0f,dwColor,dwColor,						1, 0);		// RT

	Device.SetMode(vAlpha);
	Device.Texture.Set(hFireTexture);
	Device.Draw.PrimitiveSP(D3DPT_TRIANGLESTRIP,FTLVERTEX,shadow, 4);
}

void CXR_Menu::OnRender2D()
{
	HDC		hDC;

	// Main screen
	DrawSurface(Device.dwWidth/2-512/2,Device.dwHeight/2-384/2,pMenuSurface,szBMain);

	// Lights
	int btn = GetButton();
	if (btn!=b_none && btn<ba_options){
		for (int i=0;; i++){
			if (!btn_hint[i].name) break;
			if (btn_hint[i].id==btn) ShowHint(LPSTR(btn_hint[i].name));
		}
		//if (btn!=cur_btn)	pApp->xrSndManager->PlayStatic(SND_MENU_HIGHLIGHT);
		HRESULT	hr;
		while (true) {
			hr = HW.pBB->BltFast(
				bounds[btn].left+Device.dwWidth/2-512/2,
				bounds[btn].top+Device.dwHeight/2-384/2,
				pLightSurface,
				&(bounds[btn]),
				DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT);
			if (hr == DDERR_SURFACELOST) {
				hr = pLightSurface->Restore();
				if (hr == DD_OK) DDReLoadBitmap(pLightSurface, szBLight);
			} else break;
		}
	}
	cur_btn = btn;

	///**************** TEXT OUT
	CHK_DX(HW.pBB->GetDC(&hDC));
	SetBkMode	( hDC, TRANSPARENT);
	SelectObject( hDC, menu_font);

	// Users list
	if (bEditor) iCurrentUser = Names.count-1;
	int		x = Device.dwWidth/2-512/2  + bounds[ba_users].left + CSIZE2;
	int		y = Device.dwHeight/2-384/2 + bounds[ba_users].top + CSIZE2 - 5;
	int		p;
	char *	str;

	for (p=iCurrentUser-2; p<=iCurrentUser+2; p++) {
		if (p==iCurrentUser)	SetTextColor( hDC, RGB(0,255,0));
		else					SetTextColor( hDC, RGB(0,96,0));
		str= (p>=Names.count)?" ": ((p<0)?" ":Names[p]);
		if (p==iCurrentUser && bEditor)
		{
			char buf[64];
			sprintf(buf,"[ %s ]",str);
			ExtTextOut	( hDC, x, y ,0, NULL, buf, strlen(buf), NULL );
		} else {
			ExtTextOut	( hDC, x, y ,0, NULL, str, strlen(str), NULL );
		}
		y+=CSIZE2;
	}

	// Levels list
	x = Device.dwWidth/2-512/2  + bounds[ba_levels].left + CSIZE2;
	y = Device.dwHeight/2-384/2 + bounds[ba_levels].top+1;
	for (p=iCurrentLevel-2; p<=iCurrentLevel+2; p++) {
		if (p>=0 && p<Levels.count) {
			BOOL bHL	= (p==iCurrentLevel);
			BOOL bDone	= psUserLevelsDone[p+1];
			if (bDone)	SetTextColor( hDC, RGB(0,0,bHL?255:96));	// blue
			else {
				if (CanPlay(p))	SetTextColor( hDC, RGB(0,bHL?255:96,0));	// green
				else { // grey
					if (bHL)	SetTextColor( hDC, RGB(200,200,200));
					else		SetTextColor( hDC, RGB(80,80,80));
				}
			}
			ExtTextOut	( hDC, x, y ,0, NULL, Levels[p], strlen(Levels[p]), NULL );
		}
		y+=CSIZE2-1;
	}

	// Options
	x = Device.dwWidth/2-512/2  + bounds[ba_options].left + CSIZE2;
	y = Device.dwHeight/2-384/2 + bounds[ba_options].top  + CSIZE2 - 5;
	int t = 0;
	for (p=0; p<Options.count; p++) {
		if (p==GetH()){
			ShowHint("Click to change this options");
			//if (p!=cur_p) pApp->xrSndManager->PlayStatic(SND_MENU_HIGHLIGHT);
			SetTextColor( hDC, RGB(0,255,0));
			cur_p = p;
		}else{
			t++;
			SetTextColor( hDC, RGB(0,96,0));
		}
		str= Options[p];
		ExtTextOut	( hDC, x, y ,0, NULL, str, strlen(str), NULL );
		y+=CSIZE2;
	}
	if (t==Options.count) cur_p = -1;

	// Hint
	SetTextColor( hDC, RGB(255,255,0));
	if (cHint[0]) ExtTextOut( hDC, H_X(ptHint.x), H_Y(ptHint.y), 0, NULL, cHint, strlen(cHint), NULL );
	cHint[0] = 0;

	CHK_DX(HW.pBB->ReleaseDC(hDC));

	// Cursor
	if (pOptions==NULL) DrawSurface(int(X_TO_REAL(Cursor.x)),int(Y_TO_REAL(Cursor.y)),pCursorSurface,szBCursor);
}

void CXR_Menu::OnDeviceDestroy(void)
{
	_RELEASE(pMenuSurface);
	_RELEASE(pLightSurface);
	_RELEASE(pCursorSurface);
}

void CXR_Menu::OnDeviceCreate (void)
{
	pMenuSurface	= LoadSurface(szBMain);
	pLightSurface	= LoadSurface(szBLight);
	DDSetColorKey(pLightSurface,0);
	pCursorSurface	= LoadSurface(szBCursor);
	//	UpdateRects();
}

void CXR_Menu::OnKeyboardPress(int dik)
{
	if (bEditor) {
		switch (dik) {
		case DIK_ESCAPE:
			iCurrentUser--;
			Names.DelIndex(Names.count-1);
			bEditor = false;
			SelectUser();
			break;
		case DIK_RETURN:
			{
				Names.DelIndex(Names.count-1);
				bEditor = false;
				AddToNames(cEditor);
				char buf[MAX_PATH];
				sprintf(buf,"users\\%s.user",cEditor);
				_close(_creat(buf,_S_IREAD | _S_IWRITE));
			}
			SelectUser();
			break;
		case DIK_BACK:
			if (strlen(cEditor)>0) cEditor[strlen(cEditor)-1]=0;
			break;
		case DIK_0:	strcat(cEditor,"0");	break;
		case DIK_1:	strcat(cEditor,"1");	break;
		case DIK_2:	strcat(cEditor,"2");	break;
		case DIK_3:	strcat(cEditor,"3");	break;
		case DIK_4:	strcat(cEditor,"4");	break;
		case DIK_5:	strcat(cEditor,"5");	break;
		case DIK_6:	strcat(cEditor,"6");	break;
		case DIK_7:	strcat(cEditor,"7");	break;
		case DIK_8:	strcat(cEditor,"8");	break;
		case DIK_9:	strcat(cEditor,"9");	break;
		case DIK_A:	strcat(cEditor,"a");	break;
		case DIK_B:	strcat(cEditor,"b");	break;
		case DIK_C:	strcat(cEditor,"c");	break;
		case DIK_D:	strcat(cEditor,"d");	break;
		case DIK_E:	strcat(cEditor,"e");	break;
		case DIK_F:	strcat(cEditor,"f");	break;
		case DIK_G:	strcat(cEditor,"g");	break;
		case DIK_H:	strcat(cEditor,"h");	break;
		case DIK_I:	strcat(cEditor,"i");	break;
		case DIK_J:	strcat(cEditor,"j");	break;
		case DIK_K:	strcat(cEditor,"k");	break;
		case DIK_L:	strcat(cEditor,"l");	break;
		case DIK_M:	strcat(cEditor,"m");	break;
		case DIK_N:	strcat(cEditor,"n");	break;
		case DIK_O:	strcat(cEditor,"o");	break;
		case DIK_P:	strcat(cEditor,"p");	break;
		case DIK_Q:	strcat(cEditor,"q");	break;
		case DIK_R:	strcat(cEditor,"r");	break;
		case DIK_S:	strcat(cEditor,"s");	break;
		case DIK_T:	strcat(cEditor,"t");	break;
		case DIK_U:	strcat(cEditor,"u");	break;
		case DIK_V:	strcat(cEditor,"v");	break;
		case DIK_W:	strcat(cEditor,"w");	break;
		case DIK_X:	strcat(cEditor,"x");	break;
		case DIK_Y:	strcat(cEditor,"y");	break;
		case DIK_Z:	strcat(cEditor,"z");	break;
		case DIK_SPACE:	strcat(cEditor," "); break;
		case DIK_COMMA:	strcat(cEditor,","); break;
		case DIK_PERIOD:strcat(cEditor,"."); break;
		case DIK_MINUS:	strcat(cEditor,"-"); break;
		default:
			break;
		}
		if (strlen(cEditor)>16) cEditor[16]=0;
	} else {
		switch (dik)
		{
		case DIK_ESCAPE: PutMessage(msg_hide_menu); break;
		case DIK_SPACE:
		case DIK_RETURN:
			Cursor.x=Cursor.y=0;
			OnMousePress(0);
			break;
		}
	}
}
void CXR_Menu::OnMouseMove	(int dx, int dy)
{
	Cursor.x+=float(dx)/320.0f;
	Cursor.y+=float(dy)/240.0f;
	if (Cursor.x<-1) Cursor.x=-1;
	if (Cursor.x> 1) Cursor.x=1;
	if (Cursor.y<-1) Cursor.y=-1;
	if (Cursor.y> 1) Cursor.y=1;
	//	DToF("Btn: ",DWORD(GetButton()));
}
void CXR_Menu::OnMousePress	(int btn)
{
	if (btn!=0) return;
	if (bEditor) return;
	switch (GetButton())
	{
	case b_usr_up:
		iCurrentUser--;
		if (iCurrentUser<0)  iCurrentUser = 0;
		SelectUser(true);
		//pApp->xrSndManager->PlayStatic(SND_MENU_SELECT);
		break;
	case b_usr_down:
		iCurrentUser++;
		if (iCurrentUser>=Names.count)  iCurrentUser = Names.count - 1;
		SelectUser(true);
		//pApp->xrSndManager->PlayStatic(SND_MENU_SELECT);
		break;
	case b_usr_new:
		cEditor[0]=0;
		Names.Add(cEditor);
		bEditor = true;
		SelectUser(true);
		//pApp->xrSndManager->PlayStatic(SND_MENU_SELECT);
		break;
	case b_usr_del:
		{
			char buf[MAX_PATH];
			sprintf(buf,"users\\%s.user",Names[iCurrentUser]);
			Names.DelIndex(iCurrentUser);
			iCurrentUser--;
			if (iCurrentUser<0)  iCurrentUser = 0;
			DeleteFile(buf);
			//pApp->xrSndManager->PlayStatic(SND_MENU_SELECT);
		}
		SelectUser(true);
		break;
	case b_lev_down:
		{
			int save = iCurrentLevel;
			iCurrentLevel++;
			if (iCurrentLevel>=Levels.count)  iCurrentLevel = Levels.count - 1;
			if (!CanPlay(iCurrentLevel)) iCurrentLevel=save;
			//pApp->xrSndManager->PlayStatic(SND_MENU_SELECT);
		}
		break;
	case b_lev_up:
		iCurrentLevel--;
		if (iCurrentLevel<0)  iCurrentLevel = 0;
		//pApp->xrSndManager->PlayStatic(SND_MENU_SELECT);
		break;
	case b_exit:
		SelectUser();
		pConsole->Execute("quit");
		//pApp->xrSndManager->PlayStatic(SND_MENU_SELECT);
		break;
	case b_ok:
		{
			SelectUser();
			if (CanPlay(iCurrentLevel)) {
				char buf[64];
				sprintf(buf,"level_load %d",int(iCurrentLevel+1));
				pConsole->Execute(buf);
			} else {
				// sound: Please select level only from available
			}
			//pApp->xrSndManager->PlayStatic(SND_MENU_SELECT);
		}
		break;
	case ba_options:
		{
			switch (GetH()) {
			case 0: // video
				pOptions = new CXR_Options("m_video");
				break;
			case 1: // audio
				pOptions = new CXR_Options("m_audio");
				break;
			case 2: // effects
				pOptions = new CXR_Options("m_effects");
				break;
			case 3: // controls
				pOptions = new CXR_Options("m_controls");
				break;
			case 4: // debug
				pOptions = new CXR_Options("m_debug");
				break;
			}
			//pApp->xrSndManager->PlayStatic(SND_MENU_SELECT);
		}
		break;
	case ba_users:
		break;
	case ba_levels:
		break;
	default:
		break;
	}
}

void  CXR_Menu::SelectUser(BOOL b)
{
	if (iCurrentUser>=Names.count) iCurrentUser=Names.count-1;
	if (iCurrentUser<0) iCurrentUser=0;
	if (iCurrentUser<Names.count) {
		char buf[128];
		sprintf(buf,"user %s",Names[iCurrentUser]);
		pConsole->Execute(buf);
	}
	if (b) {
		for (int i=0; i<Levels.count; i++) {
			if (CanPlay(i)) iCurrentLevel=i;
		}
	}
}
