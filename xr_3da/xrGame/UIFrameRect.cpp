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
	// texture size
	CTexture* T;
	Ivector2  ts;
	int rem_x, rem_y, tile_x, tile_y;

	float fScale	= UI()->GetScale();
	// center align
	if (GetAlign()&alCenter)
		iPos.set	(iFloor((Device.dwWidth-fScale*iSize.x)*.5f),iFloor((Device.dwHeight-fScale*iSize.y)*.5f));

	list_rect.set	(iPos.x,iPos.y,iFloor(iPos.x+fScale*iSize.x),iFloor(iPos.y+fScale*iSize.y));
	list_rect.shrink(32,32);


	RCache.set_Shader(frame[fmLT].GetShader());
	T = RCache.get_ActiveTexture(0);
	int lt_width = (int)T->get_Width();
	int lt_height = (int)T->get_Height();
	frame[fmLT].SetPos	(iPos.x, iPos.y);	

	RCache.set_Shader(frame[fmRT].GetShader());
	T = RCache.get_ActiveTexture(0);
	int rt_width = (int)T->get_Width();
	int rt_height = (int)T->get_Height();
	frame[fmRT].SetPos	(iPos.x+iSize.x-rt_width,iPos.y);	

	RCache.set_Shader(frame[fmLB].GetShader());
	T = RCache.get_ActiveTexture(0);
	int lb_width = (int)T->get_Width();
	int lb_height = (int)T->get_Height();
	frame[fmLB].SetPos	(iPos.x, iPos.y+iSize.y-lb_height);

	RCache.set_Shader(frame[fmRB].GetShader());
	T = RCache.get_ActiveTexture(0);
	int rb_width = (int)T->get_Width();
	int rb_height = (int)T->get_Height();
	frame[fmRB].SetPos	(iPos.x+iSize.x-rb_width, iPos.y+iSize.y-rb_height);	


	int size_top	= iSize.x - lt_width - rt_width;
	int size_bottom = iSize.x - lb_width - rb_width;
	int size_left	= iSize.y - lt_height - lb_height;
	int size_right  = iSize.y - rt_height - rb_height;


	//Фон
	RCache.set_Shader(back.GetShader());
	T = RCache.get_ActiveTexture(0);
	ts.set ((int)T->get_Width(),(int)T->get_Height());
	rem_x  = size_top%ts.x;
	rem_y  = size_left%ts.y;
	tile_x = iFloor(float(size_top)/ts.x); R_ASSERT(tile_x>=0);
	tile_y = iFloor(float(size_left)/ts.y);  R_ASSERT(tile_y>=0);

	back.SetPos(iPos.x+lt_width,iPos.y+lt_height);
	back.SetTile(tile_x,tile_y,rem_x,rem_y);


	//Обрамление
	RCache.set_Shader(frame[fmT].GetShader());
	T = RCache.get_ActiveTexture(0);
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	rem_x		= size_top%ts.x;
	tile_x		= iFloor(float(size_top)/ts.x); R_ASSERT(tile_x>=0);
	frame[fmT].SetPos	(iPos.x+lt_width,iPos.y);	
	frame[fmT].SetTile	(tile_x,1,rem_x,0);

	RCache.set_Shader(frame[fmB].GetShader());
	T		= RCache.get_ActiveTexture(0);
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	rem_x		= size_bottom%ts.x;
	tile_x		= iFloor(float(size_bottom)/ts.x); R_ASSERT(tile_x>=0);
	frame[fmB].SetPos	(iPos.x+lb_width,iPos.y+iSize.y-ts.y);	
	frame[fmB].SetTile	(tile_x,1,rem_x,0);


	RCache.set_Shader(frame[fmL].GetShader());
	T		= RCache.get_ActiveTexture(0);
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	rem_y		= size_left%ts.y;
	tile_y		= iFloor(float(size_left)/ts.y); R_ASSERT(tile_y>=0);
	frame[fmL].SetPos	(iPos.x, iPos.y+lt_height);	
	frame[fmL].SetTile	(1,tile_y,0,rem_y);


	RCache.set_Shader(frame[fmR].GetShader());
	T		= RCache.get_ActiveTexture(0);
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	rem_y		= size_right%ts.y;
	tile_y		= iFloor(float(size_right)/ts.y); R_ASSERT(tile_y>=0);
	frame[fmR].SetPos	(iPos.x+iSize.x-ts.x,iPos.y+rt_height);	
	frame[fmR].SetTile	(1,tile_y,0,rem_y);


	uFlags			|= flValidSize;
}
//--------------------------------------------------------------------

void CUIFrameRect::Render()
{
	if (!(uFlags&flValidSize)) UpdateSize();
	back.Render		();
	for (int k=0; k<fmMax; ++k) 
		frame[k].Render	();
}
//--------------------------------------------------------------------

void CUIFrameRect::SetColor(u32 cl)
{
	for (int i = 0; i < fmMax; ++i)
		frame[i].SetColor(cl);
	back.SetColor(cl);
}