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

void CUIFrameRect::Init(LPCSTR base_name, int x, int y, int w, int h, DWORD align)
{
	int tex_x		= 128;
	int tex_y		= 128;

	rect.set		(x,y,x+w,y+h);
	// tile
	int	rem_x		= w%tex_x;
	int rem_y		= h%tex_y;
	int tile_x		= iFloor(float(w)/tex_x); tile_x-=2; R_ASSERT(tile_x>=0);
	int tile_y		= iFloor(float(h)/tex_y); tile_y-=2; R_ASSERT(tile_y>=0);
	// init graphics
	string256		buf;
	// back
	back.Init		(strconcat(buf,base_name,"_back"),	"hud\\default",	x+tex_x,	y+tex_y,	align);	back.SetTile(tile_x,tile_y,rem_x,rem_y);
	// frame
	frame[fmL].Init	(strconcat(buf,base_name,"_l"),		"hud\\default",	x,			y+tex_y,	align); frame[fmL].SetTile(1,tile_y,0,rem_y);
	frame[fmR].Init	(strconcat(buf,base_name,"_r"),		"hud\\default",	x+w-tex_x,	y+tex_y,	align);	frame[fmR].SetTile(1,tile_y,0,rem_y);
	frame[fmT].Init	(strconcat(buf,base_name,"_t"),		"hud\\default",	x+tex_x,	y,			align);	frame[fmT].SetTile(tile_x,1,rem_x,0);
	frame[fmB].Init	(strconcat(buf,base_name,"_b"),		"hud\\default",	x+tex_x,	y+h-tex_y,	align);	frame[fmB].SetTile(tile_x,1,rem_x,0);
	frame[fmLT].Init(strconcat(buf,base_name,"_lt"),	"hud\\default",	x,			y,			align);
	frame[fmRT].Init(strconcat(buf,base_name,"_rt"),	"hud\\default",	x+w-tex_x,	y,			align);
	frame[fmRB].Init(strconcat(buf,base_name,"_rb"),	"hud\\default",	x+w-tex_x,	y+h-tex_y,	align);
	frame[fmLB].Init(strconcat(buf,base_name,"_lb"),	"hud\\default",	x,			y+h-tex_y,	align);
}
//--------------------------------------------------------------------

void CUIFrameRect::Render()
{
	back.Render		();
	for (int k=0; k<fmMax; k++) 
		frame[k].Render	();
}
//--------------------------------------------------------------------

