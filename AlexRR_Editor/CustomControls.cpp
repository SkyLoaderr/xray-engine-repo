//----------------------------------------------------
// file: CustomControls.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "UI_Main.h"
#include "CustomControls.h"
//----------------------------------------------------


IColor g_CheckHilightColor;
IColor g_ButtonForeColor;
IColor g_ButtonTextColor;
IColor g_HButtonForeColor;
IColor g_HButtonTextColor;
IColor g_StaticTextColor;
IColor g_CheckForeColor;
IColor g_CheckTextColor;
IColor g_HCheckForeColor;
IColor g_HCheckTextColor;
IColor g_BackColor;

HFONT g_StaticFont;
HFONT g_ButtonFont;

static void RepaintStatic( HWND hw )
{
	HDC hdc;
	RECT rect;
	HBRUSH brush;
	HFONT oldfont;
	char text[256];

	GetClientRect(hw,&rect);
	GetWindowText(hw,text,sizeof(text));
	hdc = GetDC( hw );
	brush = CreateSolidBrush(g_BackColor.colorref());
	FillRect(hdc,&rect,brush);
	DeleteObject(brush);
	if( text[0] ){
		oldfont = (HFONT)SelectObject( hdc, g_StaticFont );
		SetTextAlign( hdc, TA_BOTTOM|TA_CENTER );
		SetTextColor( hdc, g_StaticTextColor.colorref() );
		SetBkMode( hdc, TRANSPARENT );
		TextOut( hdc, (rect.right - rect.left)/2,
			rect.bottom - 1, text, strlen(text) );
		SelectObject( hdc, oldfont );
	}
	ReleaseDC(hw,hdc);
}

static void RepaintButton( HWND hw )
{
	HDC hdc;
	RECT rect;
	HBRUSH brush;
	HFONT oldfont;
	char text[256];
	HPEN toppen,botpen;
	HPEN oldpen;

	GetClientRect(hw,&rect);
	GetWindowText(hw,text,sizeof(text));
	bool hilight = !!(GetWindowLong(hw,GWL_USERDATA)&1);

	IColor topcolor,botcolor;
	topcolor.set(g_ButtonForeColor);
	botcolor.set(g_ButtonForeColor);

	if( hilight ){
		brush = CreateSolidBrush(g_HButtonForeColor.colorref());
		topcolor.scale( 0.75f );
		botcolor.scale( 1.25f );
	} else {
		brush = CreateSolidBrush(g_ButtonForeColor.colorref());
		topcolor.scale( 1.25f );
		botcolor.scale( 0.75f );
	}

	toppen = CreatePen(PS_SOLID,1,topcolor.colorref());
	botpen = CreatePen(PS_SOLID,1,botcolor.colorref());
	
	hdc = GetDC( hw );
	
	FillRect(hdc,&rect,brush);

	MoveToEx( hdc, rect.left, rect.bottom-1, 0);
	oldpen = (HPEN)SelectObject( hdc, toppen );
	LineTo( hdc, rect.left, rect.top );
	LineTo( hdc, rect.right-1, rect.top );
	SelectObject( hdc, botpen );
	LineTo( hdc, rect.right-1, rect.bottom-1 );
	LineTo( hdc, rect.left, rect.bottom-1 );
	
	if( text[0] ){
		oldfont = (HFONT)SelectObject( hdc, g_ButtonFont );
		SetTextAlign( hdc, TA_BOTTOM|TA_CENTER );
		SetTextColor( hdc, g_ButtonTextColor.colorref() );
		SetBkMode( hdc, TRANSPARENT );
		TextOut( hdc, (rect.right - rect.left)/2,
			rect.bottom - 1, text, strlen(text) );
		SelectObject( hdc, oldfont );
	}

	SelectObject( hdc, oldpen );
	ReleaseDC(hw,hdc);

	DeleteObject(brush);
	DeleteObject(toppen);
	DeleteObject(botpen);
}

static void RepaintCheck( HWND hw )
{
	HDC hdc;
	RECT rect;
	HBRUSH brush;
	HFONT oldfont;
	char text[256];
	HPEN toppen,botpen;
	HPEN oldpen;

	GetClientRect(hw,&rect);
	GetWindowText(hw,text,sizeof(text));
	bool hilight = !!(GetWindowLong(hw,GWL_USERDATA)&1);
	bool hilight2 = !!(GetWindowLong(hw,GWL_USERDATA)&2);

	IColor topcolor,botcolor;
	topcolor.set(g_ButtonForeColor);
	botcolor.set(g_ButtonForeColor);

	if( hilight ){
		brush = CreateSolidBrush(g_HButtonForeColor.colorref());
		topcolor.scale( 0.75f );
		botcolor.scale( 1.25f );
	} else {
		if( hilight2 ){
			brush = CreateSolidBrush(g_CheckHilightColor.colorref());
			topcolor.set(g_CheckHilightColor);
			botcolor.set(g_CheckHilightColor);
			topcolor.scale( 0.75f );
			botcolor.scale( 1.25f );
			/*topcolor.scale( 1.25f );
			botcolor.scale( 0.75f );*/
		} else {
			brush = CreateSolidBrush(g_ButtonForeColor.colorref());
			topcolor.scale( 1.25f );
			botcolor.scale( 0.75f );
		}
	}

	toppen = CreatePen(PS_SOLID,1,topcolor.colorref());
	botpen = CreatePen(PS_SOLID,1,botcolor.colorref());
	
	hdc = GetDC( hw );
	
	FillRect(hdc,&rect,brush);

	MoveToEx( hdc, rect.left, rect.bottom-1, 0);
	oldpen = (HPEN)SelectObject( hdc, toppen );
	LineTo( hdc, rect.left, rect.top );
	LineTo( hdc, rect.right-1, rect.top );
	SelectObject( hdc, botpen );
	LineTo( hdc, rect.right-1, rect.bottom-1 );
	LineTo( hdc, rect.left, rect.bottom-1 );
	
	if( text[0] ){
		oldfont = (HFONT)SelectObject( hdc, g_ButtonFont );
		SetTextAlign( hdc, TA_BOTTOM|TA_CENTER );
		SetTextColor( hdc, g_ButtonTextColor.colorref() );
		SetBkMode( hdc, TRANSPARENT );
		TextOut( hdc, (rect.right - rect.left)/2,
			rect.bottom - 1, text, strlen(text) );
		SelectObject( hdc, oldfont );
	}

	SelectObject( hdc, oldpen );
	ReleaseDC(hw,hdc);

	DeleteObject(brush);
	DeleteObject(toppen);
	DeleteObject(botpen);
}

static LRESULT CALLBACK ST2WindowProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){

	LRESULT l2 = 0;

	switch( msg ){
	case WM_PAINT:
		RepaintStatic( hw );
		ValidateRect( hw, 0 );
		break;
	case WM_ERASEBKGND:
		break;
	case WM_SETTEXT:
		l2 = DefWindowProc(hw,msg,wp,lp);
		RepaintStatic( hw );
		ValidateRect( hw, 0 );
		return l2;
	default:
		return DefWindowProc(hw,msg,wp,lp);
	}
	return 0l;
}


static LRESULT CALLBACK BT2WindowProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){

	LRESULT l2 = 0;

	switch( msg ){
	case WM_CREATE:
		SetWindowLong(hw,GWL_USERDATA,0);
		break;
	case WM_PAINT:
		RepaintButton( hw );
		ValidateRect( hw, 0 );
		break;
	case WM_ERASEBKGND:
		break;
	case WM_LBUTTONDOWN:
		if( !(GetWindowLong(hw,GWL_USERDATA)&1) ){
			SetWindowLong(hw,GWL_USERDATA,1);
			RepaintButton( hw );
			SetCapture( hw );
		}
		break;
	case WM_LBUTTONUP:
		if( (GetWindowLong(hw,GWL_USERDATA)&1) ){
			SetWindowLong(hw,GWL_USERDATA,0);
			RepaintButton( hw );
			ReleaseCapture();
			POINT pt;
			GetCursorPos(&pt);
			if(WindowFromPoint(pt)==hw)
				SendMessage(GetParent(hw),WM_COMMAND,
					GetWindowLong(hw,GWL_ID),0);
		}
		break;
	case WM_SETTEXT:
		l2 = DefWindowProc(hw,msg,wp,lp);
		RepaintButton( hw );
		ValidateRect( hw, 0 );
		return l2;
	default:
		return DefWindowProc(hw,msg,wp,lp);
	}
	return 0l;
}


static LRESULT CALLBACK ZCheckWindowProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){

	LONG flags;

	switch( msg ){
	case WM_CREATE:
		SetWindowLong(hw,GWL_USERDATA,0);
		break;
	case WM_PAINT:
		RepaintCheck( hw );
		ValidateRect( hw, 0 );
		break;
	case WM_ERASEBKGND:
		break;
	case WM_LBUTTONDOWN:
		flags = GetWindowLong(hw,GWL_USERDATA);
		if( !(flags&1) ){
			SetWindowLong(hw,GWL_USERDATA,(flags|1));
			RepaintCheck( hw );
			SetCapture( hw );
		}
		break;

	case WM_LBUTTONUP:
		flags = GetWindowLong(hw,GWL_USERDATA);
		if( (flags&1) ){
			flags &= ~0x00000001;

			POINT pt;
			GetCursorPos(&pt);
			if(WindowFromPoint(pt)==hw){
				
				if( (flags&2) ){
					flags &= ~0x00000002; }
				else {
					flags |= 2; }

				SetWindowLong(hw,GWL_USERDATA,flags);
				SendMessage(GetParent(hw),WM_COMMAND,
					GetWindowLong(hw,GWL_ID),0);
			} else {
				SetWindowLong(hw,GWL_USERDATA,flags);
			}

			RepaintCheck( hw );
			ReleaseCapture();
		}
		break;

	case BM_SETCHECK:
		flags = GetWindowLong(hw,GWL_USERDATA);
		if( wp ){
			flags |= 2;
			SetWindowLong(hw,GWL_USERDATA,flags);
		} else {
			flags &= ~0x00000002;
			SetWindowLong(hw,GWL_USERDATA,flags);
		}
		RepaintCheck( hw );
		return 0l;

	case BM_GETCHECK:
		flags = GetWindowLong(hw,GWL_USERDATA);
		return (flags&2) ? 1l : 0l;
		return 0l;

	default:
		return DefWindowProc(hw,msg,wp,lp);
	}
	return 0l;
}


void ClearColorSheme(){
	DeleteObject( g_StaticFont );
	DeleteObject( g_ButtonFont );
}

void InitColorSheme(){

	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.hCursor = LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW));
	wc.hInstance = UI.inst();
	wc.lpfnWndProc = ST2WindowProc;
	wc.lpszClassName = "STATIC";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	memset(&wc,0,sizeof(wc));
	wc.hCursor = LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW));
	wc.hInstance = UI.inst();
	wc.lpfnWndProc = BT2WindowProc;
	wc.lpszClassName = "ZBUTTON";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	memset(&wc,0,sizeof(wc));
	wc.hCursor = LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW));
	wc.hInstance = UI.inst();
	wc.lpfnWndProc = ZCheckWindowProc;
	wc.lpszClassName = "ZCHECK";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	g_StaticFont = CreateFont( -12, 0,0,0,0,0,0,0,0,0,0,0,0, "Arial" );
	g_ButtonFont = CreateFont( -10, 0,0,0,0,0,0,0,0,0,0,0,0, "Arial" );
	
	g_CheckHilightColor	 .set( 177, 150, 177 );
	g_ButtonForeColor    .set( 127, 150, 127 );
	g_ButtonTextColor    .set(   0,   0,   0 );
	g_HButtonForeColor   .set( 127, 150, 150 );
	g_HButtonTextColor   .set(   0,   0,   0 );
	g_StaticTextColor    .set( 191, 191, 191 );
	g_CheckForeColor     .set( 100, 127, 127 );
	g_CheckTextColor     .set(   0,   0,   0 );
	g_HCheckForeColor    .set( 127, 140, 180 );
	g_HCheckTextColor    .set(   0,   0,   0 );
	g_BackColor          .set( 127, 127, 150 );
}


int IsChecked( HWND hwDialog, int idControl ){
	return SendDlgItemMessage(hwDialog,idControl,BM_GETCHECK,0,0);
}

void SetCheck( HWND hwDialog, int idControl, int bCheck ){
	SendDlgItemMessage(hwDialog,idControl,BM_SETCHECK,bCheck,0);
}

