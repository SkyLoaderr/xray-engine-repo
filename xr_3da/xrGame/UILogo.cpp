#include "stdafx.h"
#include "uilogo.h"
#include "hudmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUILogo::CUILogo()
{    
}
//--------------------------------------------------------------------

CUILogo::~CUILogo()
{    
}

void CUILogo::Init(int left, int top, LPCSTR tex_name, DWORD align)
{
	back.Init	(tex_name,	"hud\\default",left,top,256,256,align);
//	back.SetRect(0,0,153,148);
}
//--------------------------------------------------------------------

void CUILogo::Render()
{
	back.Render		();
}
//--------------------------------------------------------------------
