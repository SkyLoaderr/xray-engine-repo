#include "stdafx.h"
#include "uiFragList.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFragList::CUIFragList()
{
}
//--------------------------------------------------------------------

void CUIFragList::Init()
{
	DWORD align		= alLeft|alTop;	
	frame.Init		("ui\\ui_hud_frame",150,150,400,300,64,64,align);
	list_rect.set	(frame.rect);
	list_rect.shrink(40,40);
	Level().HUD()->ClientToScreen(list_rect, align);
}
//--------------------------------------------------------------------

void CUIFragList::Render()
{
	frame.Render	();
}
//--------------------------------------------------------------------

void CUIFragList::OnFrame()
{
	CFontHUD* H			= Level().HUD()->pHUDFont;
	H->OutSet			(list_rect.lt.x,list_rect.lt.y);
	H->Color			(0xb0a0a0a0);
	for (int k=1; k<=24; k++){
		H->OutNext		("%3d: %-20s %-5d",k,"karma",k*10);
	}
}
//--------------------------------------------------------------------
