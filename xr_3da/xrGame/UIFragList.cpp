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
//s	Level().HUD()->ClientToScreen(list_rect.lt, list_rect.lt.x, list_rect.lt.y, align);
//s	Level().HUD()->ClientToScreen(list_rect.rb, list_rect.rb.x, list_rect.rb.y, align);
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
	for (int k=0; k<44; k++){
//		H->OutNext		("%3d: %-20s %-5d",k,"karma",k*10);
	}
}
//--------------------------------------------------------------------
