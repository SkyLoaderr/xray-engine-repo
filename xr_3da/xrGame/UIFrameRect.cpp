#include "stdafx.h"
#include "uiFrameRect.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFrameRect::CUIFrameRect()
{
	iPos.set		(0,0);
	iSize.set		(0,0);
	uFlags			= 0;
}
//--------------------------------------------------------------------

void CUIFrameRect::Init(LPCSTR base_name, int x, int y, int w, int h, DWORD align)
{
	SetPos			(x,y);
	SetSize			(w,h);
	SetAlign		(align);
	// init graphics
	string256		buf;
	// back
	back.CreateShader(strconcat(buf,base_name,"_back"),	"hud\\default");			//back.SetAlign		(align);
	// frame
	frame[fmL].CreateShader	(strconcat(buf,base_name,"_l"),		"hud\\default");	//frame[fmL].SetAlign	(align); 
	frame[fmR].CreateShader	(strconcat(buf,base_name,"_r"),		"hud\\default");	//frame[fmR].SetAlign	(align);	
	frame[fmT].CreateShader	(strconcat(buf,base_name,"_t"),		"hud\\default");	//frame[fmT].SetAlign	(align);	
	frame[fmB].CreateShader	(strconcat(buf,base_name,"_b"),		"hud\\default");	//frame[fmB].SetAlign	(align);	
	frame[fmLT].CreateShader(strconcat(buf,base_name,"_lt"),	"hud\\default");	//frame[fmLT].SetAlign(align);
	frame[fmRT].CreateShader(strconcat(buf,base_name,"_rt"),	"hud\\default");	//frame[fmRT].SetAlign(align);
	frame[fmRB].CreateShader(strconcat(buf,base_name,"_rb"),	"hud\\default");	//frame[fmRB].SetAlign(align);
	frame[fmLB].CreateShader(strconcat(buf,base_name,"_lb"),	"hud\\default");	//frame[fmLB].SetAlign(align);
}
//--------------------------------------------------------------------

void CUIFrameRect::UpdateSize()
{
	Device.Shader.set_Shader(back.GetShader());
	// texture size
	CTexture* T		= Device.Shader.get_ActiveTexture(0);
	Ivector2		ts;
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	// tile
	int	rem_x		= iSize.x%ts.x;
	int rem_y		= iSize.y%ts.y;
	int tile_x		= iFloor(float(iSize.x)/ts.x); tile_x-=2; R_ASSERT(tile_x>=0);
	int tile_y		= iFloor(float(iSize.y)/ts.y); tile_y-=2; R_ASSERT(tile_y>=0);

	float fScale	= Level().HUD()->GetScale();
	// center align
	if (GetAlign()&alCenter)
		iPos.set	(iFloor((Device.dwWidth-fScale*iSize.x)*.5f),iFloor((Device.dwHeight-fScale*iSize.y)*.5f));

	list_rect.set	(iPos.x,iPos.y,iFloor(iPos.x+fScale*iSize.x),iFloor(iPos.y+fScale*iSize.y));
	list_rect.shrink(32,32);

	back.SetPos			(iPos.x+ts.x,			iPos.y+ts.y			);	back.SetTile		(tile_x,tile_y,rem_x,rem_y);
	frame[fmL].SetPos	(iPos.x,				iPos.y+ts.y			);	frame[fmL].SetTile	(1,tile_y,0,rem_y);
	frame[fmR].SetPos	(iPos.x+iSize.x-ts.x,	iPos.y+ts.y			);	frame[fmR].SetTile	(1,tile_y,0,rem_y);
	frame[fmT].SetPos	(iPos.x+ts.x,			iPos.y				);	frame[fmT].SetTile	(tile_x,1,rem_x,0);
	frame[fmB].SetPos	(iPos.x+ts.x,			iPos.y+iSize.y-ts.y	);	frame[fmB].SetTile	(tile_x,1,rem_x,0);
	frame[fmLT].SetPos	(iPos.x,				iPos.y				);	
	frame[fmRT].SetPos	(iPos.x+iSize.x-ts.x,	iPos.y				);	
	frame[fmRB].SetPos	(iPos.x+iSize.x-ts.x,	iPos.y+iSize.y-ts.y	);	
	frame[fmLB].SetPos	(iPos.x,				iPos.y+iSize.y-ts.y	);	

	uFlags			|= flValidSize;
}
//--------------------------------------------------------------------

void CUIFrameRect::Render()
{
	if (!(uFlags&flValidSize)) UpdateSize();
	back.Render		();
	for (int k=0; k<fmMax; k++) 
		frame[k].Render	();
}
//--------------------------------------------------------------------

