#include "stdafx.h"
#include "uiFrameRect.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFrameRect::CUIFrameRect()
{
}
//--------------------------------------------------------------------

void CUIFrameRect::Init(LPCSTR base_name, int x, int y, int sx, int sy, int tex_x, int tex_y, DWORD align)
{
	rect.set		(x,y,x+sx,y+sy);
	// tile
	Fvector2		tile;
	int	rem_x		= sx%tex_x;
	int rem_y		= sy%tex_y;
	int tile_x		= iFloor(float(sx)/tex_x); tile_x-=2; R_ASSERT(tile_x>=0);
	int tile_y		= iFloor(float(sy)/tex_y); tile_y-=2; R_ASSERT(tile_y>=0);
	// init graphics
	string256		buf;
	// back
	strconcat		(buf,base_name,"_back");
	back.Init		(buf, "hud\\default",	x+tex_x,	y+tex_y,	tex_x,	tex_y,	align);	back.SetTile(tile_x,tile_y,rem_x,rem_y);
	// frame
	strconcat		(buf,base_name,"_l");
	frame[fmL].Init	(buf, "hud\\default",	x,			y+tex_y,	tex_x,	tex_y,	align); frame[fmL].SetTile(1,tile_y,0,rem_y);
	strconcat		(buf,base_name,"_r");
	frame[fmR].Init	(buf, "hud\\default",	x+sx-tex_x,	y+tex_y,	tex_x,	tex_y,	align);	frame[fmR].SetTile(1,tile_y,0,rem_y);
	strconcat		(buf,base_name,"_t");
	frame[fmT].Init	(buf, "hud\\default",	x+tex_x,	y,			tex_x,	tex_y,	align);	frame[fmT].SetTile(tile_x,1,rem_x,0);
	strconcat		(buf,base_name,"_b");
	frame[fmB].Init	(buf, "hud\\default",	x+tex_x,	y+sy-tex_y,	tex_x,	tex_y,	align);	frame[fmB].SetTile(tile_x,1,rem_x,0);
	strconcat		(buf,base_name,"_lt");							
	frame[fmLT].Init(buf,"hud\\default",	x,			y,			tex_x,	tex_y,	align);
	strconcat		(buf,base_name,"_rt");
	frame[fmRT].Init(buf,"hud\\default",	x+sx-tex_x,	y,			tex_x,	tex_y,	align);
	strconcat		(buf,base_name,"_rb");							
	frame[fmRB].Init(buf,"hud\\default",	x+sx-tex_x,	y+sy-tex_y,	tex_x,	tex_y,	align);
	strconcat		(buf,base_name,"_lb");
	frame[fmLB].Init(buf,"hud\\default",	x,			y+sy-tex_y,	tex_x,	tex_y,	align);
}
//--------------------------------------------------------------------

void CUIFrameRect::Render()
{
	back.Render		();
	for (int k=0; k<fmMax; k++) frame[k].Render	();
}
//--------------------------------------------------------------------

