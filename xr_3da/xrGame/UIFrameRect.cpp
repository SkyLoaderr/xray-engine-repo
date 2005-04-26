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
	uFlags.zero		();
}
//--------------------------------------------------------------------

void CUIFrameRect::Init(LPCSTR base_name, int x, int y, int w, int h, DWORD align)
{
	SetPos			(x,y);
	SetSize			(w,h);
	SetAlign		(align);

	string_path		fn,buf;
	strcpy			(buf,base_name); if (strext(buf)) *strext(buf)=0;
	if (FS.exist(fn,"$game_textures$",buf,".ini")){
		Ivector4	v;
		uFlags.set	(flSingleTex,TRUE);
		CInifile* ini= CInifile::Create(fn,TRUE);
		LPCSTR sh	= ini->r_string("frame","shader");
		frame[fmBK].CreateShader(base_name,sh);
		frame[fmL].CreateShader	(base_name,sh);
		frame[fmR].CreateShader	(base_name,sh);
		frame[fmT].CreateShader	(base_name,sh);
		frame[fmB].CreateShader	(base_name,sh);
		frame[fmLT].CreateShader(base_name,sh);
		frame[fmRT].CreateShader(base_name,sh);
		frame[fmRB].CreateShader(base_name,sh);
		frame[fmLB].CreateShader(base_name,sh);
		v			= ini->r_ivector4("frame","back");	frame[fmBK].SetOriginalRect	(v.x,v.y,v.z,v.w);	frame[fmBK].SetRect	(0,0,v.z,v.w);
		v			= ini->r_ivector4("frame","l");		frame[fmL].SetOriginalRect	(v.x,v.y,v.z,v.w);	frame[fmL].SetRect	(0,0,v.z,v.w);
		v			= ini->r_ivector4("frame","r");		frame[fmR].SetOriginalRect	(v.x,v.y,v.z,v.w);	frame[fmR].SetRect	(0,0,v.z,v.w);
		v			= ini->r_ivector4("frame","t");		frame[fmT].SetOriginalRect	(v.x,v.y,v.z,v.w);	frame[fmT].SetRect	(0,0,v.z,v.w);
		v			= ini->r_ivector4("frame","b");		frame[fmB].SetOriginalRect	(v.x,v.y,v.z,v.w);	frame[fmB].SetRect	(0,0,v.z,v.w);
		v			= ini->r_ivector4("frame","lt");	frame[fmLT].SetOriginalRect	(v.x,v.y,v.z,v.w);	frame[fmLT].SetRect	(0,0,v.z,v.w);
		v			= ini->r_ivector4("frame","rt");	frame[fmRT].SetOriginalRect	(v.x,v.y,v.z,v.w);	frame[fmRT].SetRect	(0,0,v.z,v.w);
		v			= ini->r_ivector4("frame","rb");	frame[fmRB].SetOriginalRect	(v.x,v.y,v.z,v.w);	frame[fmRB].SetRect	(0,0,v.z,v.w);
		v			= ini->r_ivector4("frame","lb");	frame[fmLB].SetOriginalRect	(v.x,v.y,v.z,v.w);	frame[fmLB].SetRect	(0,0,v.z,v.w);
		CInifile::Destroy(ini);
	}else{
		// back
		frame[fmBK].CreateShader(strconcat(buf,base_name,"_back"),	"hud\\default");	//back.SetAlign		(align);
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
}
//--------------------------------------------------------------------

void CUIFrameRect::UpdateSize()
{
	VERIFY(g_bRendering);
	// texture size
	Ivector2  ts;
	int rem_x, rem_y, tile_x, tile_y;

	float fScaleX	= UI()->GetScaleX();
	float fScaleY	= UI()->GetScaleY();
	// center align
	if (GetAlign()&alCenter)
		iPos.set	(iFloor((Device.dwWidth-fScaleX*iSize.x)*.5f),iFloor((Device.dwHeight-fScaleY*iSize.y)*.5f));

	Ivector2 _bk, _lt,_lb,_rb,_rt, _l,_r,_t,_b;

	if (uFlags.is(flSingleTex)){
		_bk.set		(frame[fmBK].GetOriginalRect().width(),	frame[fmBK].GetOriginalRect().height());
		_lt.set		(frame[fmLT].GetOriginalRect().width(), frame[fmLT].GetOriginalRect().height());
		_lb.set		(frame[fmLB].GetOriginalRect().width(), frame[fmLB].GetOriginalRect().height());
		_rb.set		(frame[fmRB].GetOriginalRect().width(), frame[fmRB].GetOriginalRect().height());
		_rt.set		(frame[fmRT].GetOriginalRect().width(), frame[fmRT].GetOriginalRect().height());
		_l.set		(frame[fmL].GetOriginalRect().width(),	frame[fmL].GetOriginalRect().height());
		_r.set		(frame[fmR].GetOriginalRect().width(),	frame[fmR].GetOriginalRect().height());
		_t.set		(frame[fmT].GetOriginalRect().width(),	frame[fmT].GetOriginalRect().height());
		_b.set		(frame[fmB].GetOriginalRect().width(),	frame[fmB].GetOriginalRect().height());
	}else{
		CTexture*	T;
		RCache.set_Shader	(frame[fmLT].GetShader());
		T					= RCache.get_ActiveTexture(0);
		_lt.set				((int)T->get_Width(),(int)T->get_Height());

		RCache.set_Shader	(frame[fmRT].GetShader());
		T					= RCache.get_ActiveTexture(0);
		_rt.set				((int)T->get_Width(),(int)T->get_Height());

		RCache.set_Shader	(frame[fmLB].GetShader());
		T					= RCache.get_ActiveTexture(0);
		_lb.set				((int)T->get_Width(),(int)T->get_Height());

		RCache.set_Shader	(frame[fmRB].GetShader());
		T					= RCache.get_ActiveTexture(0);
		_rb.set				((int)T->get_Width(),(int)T->get_Height());

		RCache.set_Shader	(frame[fmBK].GetShader());
		T					= RCache.get_ActiveTexture(0);
		_bk.set				((int)T->get_Width(),(int)T->get_Height());
		
		RCache.set_Shader	(frame[fmT].GetShader());
		T					= RCache.get_ActiveTexture(0);
		_t.set				((int)T->get_Width(),(int)T->get_Height());

		RCache.set_Shader	(frame[fmB].GetShader());
		T					= RCache.get_ActiveTexture(0);
		_b.set				((int)T->get_Width(),(int)T->get_Height());

		RCache.set_Shader(frame[fmL].GetShader());
		T					= RCache.get_ActiveTexture(0);
		_l.set				((int)T->get_Width(),(int)T->get_Height());

		RCache.set_Shader	(frame[fmR].GetShader());
		T					= RCache.get_ActiveTexture(0);
		_r.set				((int)T->get_Width(),(int)T->get_Height());
	}

	frame[fmLT].SetPos	(iPos.x,				iPos.y);	
	frame[fmRT].SetPos	(iPos.x+iSize.x-_rt.x,	iPos.y);	
	frame[fmLB].SetPos	(iPos.x,				iPos.y+iSize.y-_lb.y);
	frame[fmRB].SetPos	(iPos.x+iSize.x-_rb.x,	iPos.y+iSize.y-_rb.y);	

	int size_top	= iSize.x - _lt.x - _rt.x;
	int size_bottom = iSize.x - _lb.x - _rb.x;
	int size_left	= iSize.y - _lt.y - _lb.y;
	int size_right  = iSize.y - _rt.y - _rb.y;

	//Фон
	ts.set			(_bk.x,_bk.y);
	rem_x  			= size_top%ts.x;
	rem_y  			= size_left%ts.y;
	tile_x 			= iFloor(float(size_top)/ts.x);		tile_x=_max(tile_x, 0);
	tile_y 			= iFloor(float(size_left)/ts.y);	tile_y=_max(tile_y, 0);

	frame[fmBK].SetPos	(iPos.x+_lt.x,iPos.y+_lt.y);
	frame[fmBK].SetTile	(tile_x,tile_y,rem_x,rem_y);

	//Обрамление
	ts.set				(_t.x,_t.y);
	rem_x				= size_top%ts.x;
	tile_x				= iFloor(float(size_top)/ts.x); tile_x=_max(tile_x, 0);
	frame[fmT].SetPos	(iPos.x+_lt.x,iPos.y);	
	frame[fmT].SetTile	(tile_x,1,rem_x,0);

	ts.set				(_b.x,_b.y);
	rem_x				= size_bottom%ts.x;
	tile_x				= iFloor(float(size_bottom)/ts.x); tile_x=_max(tile_x, 0);
	frame[fmB].SetPos	(iPos.x+_lb.x,iPos.y+iSize.y-ts.y);	
	frame[fmB].SetTile	(tile_x,1,rem_x,0);


	ts.set				(_l.x,_l.y);
	rem_y				= size_left%ts.y;
	tile_y				= iFloor(float(size_left)/ts.y); tile_y=_max(tile_y, 0);
	frame[fmL].SetPos	(iPos.x, iPos.y+_lt.y);	
	frame[fmL].SetTile	(1,tile_y,0,rem_y);

	ts.set				(_r.x,_r.y);
	rem_y				= size_right%ts.y;
	tile_y				= iFloor(float(size_right)/ts.y); tile_y=_max(tile_y, 0);
	frame[fmR].SetPos	(iPos.x+iSize.x-ts.x,iPos.y+_rt.y);	
	frame[fmR].SetTile	(1,tile_y,0,rem_y);

	uFlags.set			(flValidSize,TRUE);
}
//--------------------------------------------------------------------

void CUIFrameRect::Render()
{
	if (!uFlags.is(flValidSize)) UpdateSize();
	for (int k=0; k<fmMax; ++k) 
		frame[k].Render	();
}
//--------------------------------------------------------------------

void CUIFrameRect::SetColor(u32 cl)
{
	for (int i = 0; i < fmMax; ++i)
		frame[i].SetColor(cl);
}