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

void CUIFrameRect::Init(LPCSTR base_name, int x, int y, int sx, int sy, int tex_x, int tex_y, u32 align)
{
	rect.set		(x,y,x+sx,y+sy);
	// tile
	int	rem_x		= sx%tex_x;
	int rem_y		= sy%tex_y;
	int tile_x		= iFloor(float(sx)/tex_x); tile_x-=2; R_ASSERT(tile_x>=0);
	int tile_y		= iFloor(float(sy)/tex_y); tile_y-=2; R_ASSERT(tile_y>=0);
	// init graphics
	string256		buf;
	// back
	back.Init		(strconcat(buf,base_name,"_back"),	"hud\\default",	x+tex_x,	y+tex_y,	tex_x,	tex_y,	align);	back.SetTile(tile_x,tile_y,rem_x,rem_y);
	// frame
	frame[fmL].Init	(strconcat(buf,base_name,"_l"),		"hud\\default",	x,			y+tex_y,	tex_x,	tex_y,	align); frame[fmL].SetTile(1,tile_y,0,rem_y);
	frame[fmR].Init	(strconcat(buf,base_name,"_r"),		"hud\\default",	x+sx-tex_x,	y+tex_y,	tex_x,	tex_y,	align);	frame[fmR].SetTile(1,tile_y,0,rem_y);
	frame[fmT].Init	(strconcat(buf,base_name,"_t"),		"hud\\default",	x+tex_x,	y,			tex_x,	tex_y,	align);	frame[fmT].SetTile(tile_x,1,rem_x,0);
	frame[fmB].Init	(strconcat(buf,base_name,"_b"),		"hud\\default",	x+tex_x,	y+sy-tex_y,	tex_x,	tex_y,	align);	frame[fmB].SetTile(tile_x,1,rem_x,0);
	frame[fmLT].Init(strconcat(buf,base_name,"_lt"),	"hud\\default",	x,			y,			tex_x,	tex_y,	align);
	frame[fmRT].Init(strconcat(buf,base_name,"_rt"),	"hud\\default",	x+sx-tex_x,	y,			tex_x,	tex_y,	align);
	frame[fmRB].Init(strconcat(buf,base_name,"_rb"),	"hud\\default",	x+sx-tex_x,	y+sy-tex_y,	tex_x,	tex_y,	align);
	frame[fmLB].Init(strconcat(buf,base_name,"_lb"),	"hud\\default",	x,			y+sy-tex_y,	tex_x,	tex_y,	align);
}
//--------------------------------------------------------------------

void CUIFrameRect::Render()
{
	back.Render		();
	for (int k=0; k<fmMax; k++) 
		frame[k].Render	();
}
//--------------------------------------------------------------------

