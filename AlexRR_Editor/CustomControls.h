//----------------------------------------------------
// file: CustomControls.h
//----------------------------------------------------

#ifndef _INCDEF_CustomControls_H_
#define _INCDEF_CustomControls_H_

//----------------------------------------------------

extern IColor g_ButtonForeColor;
extern IColor g_ButtonTextColor;
extern IColor g_HButtonForeColor;
extern IColor g_HButtonTextColor;
extern IColor g_StaticTextColor;
extern IColor g_CheckForeColor;
extern IColor g_CheckTextColor;
extern IColor g_HCheckForeColor;
extern IColor g_HCheckTextColor;
extern IColor g_BackColor;

extern HFONT g_StaticFont;
extern HFONT g_ButtonFont;

void InitColorSheme();
void ClearColorSheme();

int IsChecked( HWND hwDialog, int idControl );
void SetCheck( HWND hwDialog, int idControl, int bCheck );

//----------------------------------------------------
#endif /*_INCDEF_CustomControls_H_*/
