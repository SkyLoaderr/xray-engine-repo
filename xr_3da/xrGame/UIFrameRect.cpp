#include "stdafx.h"
#include "uiFrameRect.h"
#include "hudmanager.h"
#include "ui\uitexturemaster.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFrameRect::CUIFrameRect()
{
	m_wndPos.set		(0,0);
	m_wndSize.set		(0,0);
	uFlags.zero		();
}
//--------------------------------------------------------------------

void CUIFrameRect::Init(LPCSTR texture, int x, int y, int w, int h)//, DWORD align)
{
	//SetPos			(x,y);
	//SetSize			(w,h);
	//SetAlign		(align);
	CUISimpleWindow::Init(x,y,w,h);
	InitTexture(texture);
}

void CUIFrameRect::InitTexture(const char* texture){
	string_path		fn,buf;
	strcpy			(buf,texture); if (strext(buf)) *strext(buf)=0;

	uFlags.set	(flSingleTex,TRUE);

	if (FS.exist(fn,"$game_textures$",buf,".ini")){
		Ivector4	v;
		//uFlags.set	(flSingleTex,TRUE);
		CInifile* ini= CInifile::Create(fn,TRUE);
		LPCSTR sh	= ini->r_string("frame","shader");
		frame[fmBK].CreateShader(texture,sh);
		frame[fmL].CreateShader	(texture,sh);
		frame[fmR].CreateShader	(texture,sh);
		frame[fmT].CreateShader	(texture,sh);
		frame[fmB].CreateShader	(texture,sh);
		frame[fmLT].CreateShader(texture,sh);
		frame[fmRT].CreateShader(texture,sh);
		frame[fmRB].CreateShader(texture,sh);
		frame[fmLB].CreateShader(texture,sh);
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
	}
	else
	{
		CUITextureMaster::InitTexture(strconcat(buf,texture,"_back"),	&frame[CUIFrameRect::fmBK]);
		CUITextureMaster::InitTexture(strconcat(buf,texture,"_l"),		&frame[CUIFrameRect::fmL]);
		CUITextureMaster::InitTexture(strconcat(buf,texture,"_r"),		&frame[CUIFrameRect::fmR]);
		CUITextureMaster::InitTexture(strconcat(buf,texture,"_t"),		&frame[CUIFrameRect::fmT]);
		CUITextureMaster::InitTexture(strconcat(buf,texture,"_b"),		&frame[CUIFrameRect::fmB]);
		CUITextureMaster::InitTexture(strconcat(buf,texture,"_lt"),		&frame[CUIFrameRect::fmLT]);
		CUITextureMaster::InitTexture(strconcat(buf,texture,"_rt"),		&frame[CUIFrameRect::fmRT]);
		CUITextureMaster::InitTexture(strconcat(buf,texture,"_rb"),		&frame[CUIFrameRect::fmRB]);
		CUITextureMaster::InitTexture(strconcat(buf,texture,"_lb"),		&frame[CUIFrameRect::fmLB]);		
	}
}

void CUIFrameRect::UpdateSize()
{
	VERIFY(g_bRendering);
	// texture size
	Ivector2  ts;
	int rem_x, rem_y, tile_x, tile_y;

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

	frame[fmLT].SetPos	(m_wndPos.x,				m_wndPos.y);	
	frame[fmRT].SetPos	(m_wndPos.x+m_wndSize.x-_rt.x,	m_wndPos.y);	
	frame[fmLB].SetPos	(m_wndPos.x,				m_wndPos.y+m_wndSize.y-_lb.y);
	frame[fmRB].SetPos	(m_wndPos.x+m_wndSize.x-_rb.x,	m_wndPos.y+m_wndSize.y-_rb.y);	

	int size_top	= m_wndSize.x - _lt.x - _rt.x;
	int size_bottom = m_wndSize.x - _lb.x - _rb.x;
	int size_left	= m_wndSize.y - _lt.y - _lb.y;
	int size_right  = m_wndSize.y - _rt.y - _rb.y;

	//Фон
	ts.set			(_bk.x,_bk.y);
	rem_x  			= size_top%ts.x;
	rem_y  			= size_left%ts.y;
	tile_x 			= iFloor(float(size_top)/ts.x);		tile_x=_max(tile_x, 0);
	tile_y 			= iFloor(float(size_left)/ts.y);	tile_y=_max(tile_y, 0);

	frame[fmBK].SetPos	(m_wndPos.x+_lt.x,m_wndPos.y+_lt.y);
	frame[fmBK].SetTile	(tile_x,tile_y,rem_x,rem_y);

	//Обрамление
	ts.set				(_t.x,_t.y);
	rem_x				= size_top%ts.x;
	tile_x				= iFloor(float(size_top)/ts.x); tile_x=_max(tile_x, 0);
	frame[fmT].SetPos	(m_wndPos.x+_lt.x,m_wndPos.y);	
	frame[fmT].SetTile	(tile_x,1,rem_x,0);

	ts.set				(_b.x,_b.y);
	rem_x				= size_bottom%ts.x;
	tile_x				= iFloor(float(size_bottom)/ts.x); tile_x=_max(tile_x, 0);
	frame[fmB].SetPos	(m_wndPos.x+_lb.x,m_wndPos.y+m_wndSize.y-ts.y);	
	frame[fmB].SetTile	(tile_x,1,rem_x,0);


	ts.set				(_l.x,_l.y);
	rem_y				= size_left%ts.y;
	tile_y				= iFloor(float(size_left)/ts.y); tile_y=_max(tile_y, 0);
	frame[fmL].SetPos	(m_wndPos.x, m_wndPos.y+_lt.y);	
	frame[fmL].SetTile	(1,tile_y,0,rem_y);

	ts.set				(_r.x,_r.y);
	rem_y				= size_right%ts.y;
	tile_y				= iFloor(float(size_right)/ts.y); tile_y=_max(tile_y, 0);
	frame[fmR].SetPos	(m_wndPos.x+m_wndSize.x-ts.x,m_wndPos.y+_rt.y);	
	frame[fmR].SetTile	(1,tile_y,0,rem_y);

	uFlags.set			(flValidSize,TRUE);
}

void CUIFrameRect::Draw()
{
	if (!uFlags.is(flValidSize)) 
		UpdateSize();

	for (int k=0; k<fmMax; ++k) 
		frame[k].Render	();
}

void CUIFrameRect::Update(){

}

void CUIFrameRect::Draw(int x, int y){
	int dx = m_wndPos.x - x;
	int dy = m_wndPos.y - y;
	if (dx != 0 || dy != 0)
	{
        uFlags.set(flValidSize, false);
		m_wndPos.set(x,y);
	}
	
	Draw();
}

void CUIFrameRect::SetTextureColor(u32 cl)
{
	for (int i = 0; i < fmMax; ++i)
		frame[i].SetColor(cl);
}