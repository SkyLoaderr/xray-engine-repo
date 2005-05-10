// LevelFogOfWar.cpp:  для карты уровня в одиночном режиме игры
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LevelFogOfWar.h"
#include "level.h"
#include "alife_registry_wrappers.h"
#include "ui/UIMapWnd.h"


#define FOG_CELL_SZ	(100.0f)

struct FindFogByLevelName{
	shared_str			level_name;
	FindFogByLevelName(const shared_str& name){level_name = name;}
	bool operator () (const CLevelFogOfWar& fog){
		return (level_name==fog.m_level_name);
	}
};

CFogOfWarMngr::CFogOfWarMngr()
{
	m_fogOfWarRegistry = xr_new<CFogOfWarWrapper>();
	m_fogOfWarRegistry->registry().init(0);
}

CLevelFogOfWar* CFogOfWarMngr::GetFogOfWar	(const shared_str& level_name)
{
	FOG_STORAGE_IT it = std::find_if(GetFogStorage().begin(),GetFogStorage().end(),FindFogByLevelName(level_name));
	if(it!=GetFogStorage().end())return &(*it);
	else { //create new or load...
		GetFogStorage().resize(GetFogStorage().size()+1);
		CLevelFogOfWar& F = GetFogStorage().back();
		F.Init(level_name);
		return &F;
	};
}

void CFogOfWarRegistry::save(IWriter &stream)
{

}

FOG_STORAGE_T& CFogOfWarMngr::GetFogStorage()
{
	VERIFY(m_fogOfWarRegistry);
	return m_fogOfWarRegistry->registry().objects();
};

CLevelFogOfWar::CLevelFogOfWar():m_rowNum(0), m_colNum(0)
{
	m_levelRect.set	(0.0f,0.0f,0.0f,0.0f);
	hShader.create	("hud\\default","ui\\ui_fog_of_war");
	hGeom.create	(FVF::F_TL, RCache.Vertex.Buffer(), 0);

};

void CLevelFogOfWar::Init	(const shared_str& level)
{
	m_level_name		= level;

	string256			gameLtxPath;
	FS.update_path		(gameLtxPath, "$game_config$", "game.ltx");
	CInifile gameLtx	(gameLtxPath);


	Fvector4 tmp;
	if( gameLtx.section_exist(m_level_name) )
		tmp		= gameLtx.r_fvector4(m_level_name,"bound_rect");
	else
		tmp.set(0.0f,0.0f,0.0f,0.0f);

	m_levelRect.set		(tmp.x, tmp.y, tmp.z, tmp.w);


	m_rowNum = iFloor((m_levelRect.height()+	FOG_CELL_SZ*0.5f)/FOG_CELL_SZ);
	m_colNum = iFloor((m_levelRect.width()+		FOG_CELL_SZ*0.5f)/FOG_CELL_SZ);
				
	m_cells.resize(m_rowNum*m_colNum, Flags8().assign(FOG_CLOSED));

}

void CLevelFogOfWar::Open	(Fvector2 pos)
{
	if(!m_rowNum ||!m_rowNum) return; //invalid map
	VERIFY2((pos.x>=m_levelRect.lt.x && pos.y>=m_levelRect.lt.y && pos.x<=m_levelRect.rb.x && pos.y<=m_levelRect.rb.y),"invalid position for opening FogOfWar map cell" );
	
	u32 col = iFloor( (pos.x - m_levelRect.lt.x)/FOG_CELL_SZ);
	u32 row = iFloor(  (m_levelRect.height() - (pos.y - m_levelRect.lt.y))/FOG_CELL_SZ);


	if (m_cells.at(row*m_colNum+col).flags==FOG_OPEN_TOTAL) return;
	Open(row+0,col+0,FOG_OPEN_TOTAL);
/*	Open(row-1,col-1,FOG_OPEN_RB);
	Open(row-1,col+0,FOG_OPEN_B);
	Open(row-1,col+1,FOG_OPEN_LB);
	Open(row+0,col+1,FOG_OPEN_L);
	Open(row+1,col+1,FOG_OPEN_LT);
	Open(row+1,col+0,FOG_OPEN_T);
	Open(row+1,col-1,FOG_OPEN_RT);
	Open(row+0,col-1,FOG_OPEN_R);*/
}

void CLevelFogOfWar::Open	(u32 row, u32 col, u8 mask)
{
	if(row>=m_rowNum || col>=m_colNum) return;

	m_cells.at(row*m_colNum+col).set(mask,TRUE);
}

Ivector2 CLevelFogOfWar::ConvertRealToLocal(const Fvector2& src)
{
	Ivector2 res;
	res.x = iFloor( (src.x - m_levelRect.lt.x)/FOG_CELL_SZ);
	res.y = iFloor( (src.y - m_levelRect.lt.y)/FOG_CELL_SZ);

	return res;
}

Irect CLevelFogOfWar::ConvertRealToLocal	(const Frect& src)
{
	return Irect().set(ConvertRealToLocal(src.lt), ConvertRealToLocal(src.rb));
}

Fvector2 CLevelFogOfWar::ConvertLocalToReal(const Ivector2& src)
{
	return Fvector2().set(	m_levelRect.lt.x + src.x*FOG_CELL_SZ,
							m_levelRect.lt.y + src.y*FOG_CELL_SZ) ;
}

enum {
	tfLT = 0,
	tfRT,
	tfLB,
	tfRB
};

void CLevelFogOfWar::GetTexFrame(Ivector2& frame, u32 part, u32 col, u32 row)
{
	if (row>=m_rowNum || col>=m_colNum)				frame.set(1,0);
	else{
		Flags8 cell_mask							= m_cells[row*m_colNum+col];
		if (cell_mask.flags==FOG_OPEN_TOTAL)		frame.set(0,0);
		else if (cell_mask.flags==FOG_CLOSED)		frame.set(1,0);
		else{
			switch (part){
			case tfLT: 
				if (cell_mask.is(FOG_OPEN_L|FOG_OPEN_T))frame.set(3,2);
				else if (cell_mask.is(FOG_OPEN_L))		frame.set(0,1);
				else if (cell_mask.is(FOG_OPEN_T))		frame.set(0,2);
				else if (cell_mask.is(FOG_OPEN_LT))		frame.set(0,3);
				else									frame.set(1,0); // closed TOTAL
				break;
			case tfRT: 
				if (cell_mask.is(FOG_OPEN_R|FOG_OPEN_T))frame.set(2,2);
				else if (cell_mask.is(FOG_OPEN_R))		frame.set(1,1);
				else if (cell_mask.is(FOG_OPEN_T))		frame.set(0,2);
				else if (cell_mask.is(FOG_OPEN_RT))		frame.set(1,3);
				else									frame.set(1,0); // closed TOTAL
				break;
			case tfLB: 
				if (cell_mask.is(FOG_OPEN_L|FOG_OPEN_B))frame.set(3,1);
				else if (cell_mask.is(FOG_OPEN_L))		frame.set(0,1);
				else if (cell_mask.is(FOG_OPEN_B))		frame.set(1,2);
				else if (cell_mask.is(FOG_OPEN_LB))		frame.set(2,3);
				else									frame.set(1,0); // closed TOTAL
				break;
			case tfRB: 
				if (cell_mask.is(FOG_OPEN_R|FOG_OPEN_B))frame.set(2,1);
				else if (cell_mask.is(FOG_OPEN_R))		frame.set(1,1);
				else if (cell_mask.is(FOG_OPEN_B))		frame.set(1,2);
				else if (cell_mask.is(FOG_OPEN_RB))		frame.set(3,3);
				else									frame.set(1,0); // closed TOTAL
				break;
			}
		}
	}
}

void CLevelFogOfWar::Draw	(CUICustomMap* m)
{

	Frect	tgt;
	Irect	clip_rect				= m->GetClipperRect();

	Ivector2	map_abs_pos			= m->GetAbsolutePos();
	Irect	vis_rect;
	vis_rect.set(	clip_rect.lt.x-map_abs_pos.x,
					clip_rect.lt.y-map_abs_pos.y,
					clip_rect.rb.x-map_abs_pos.x,
					clip_rect.rb.y-map_abs_pos.y);// vis_rect now in pixels

	
	tgt.set(float(vis_rect.x1), float(vis_rect.y1), float(vis_rect.x2), float(vis_rect.y2) ); 
	tgt.div(m->GetCurrentZoom(), m->GetCurrentZoom());
	tgt.add(m_levelRect.lt.x, m_levelRect.lt.y);
	tgt.rb.add(Fvector2().set(FOG_CELL_SZ-EPS_L,FOG_CELL_SZ-EPS_L));
	Irect		cells			= ConvertRealToLocal(tgt);

	Fvector2	realCellsPosLT	= ConvertLocalToReal(cells.lt);

	realCellsPosLT.sub			(m_levelRect.lt).mul(m->GetCurrentZoom());
	
	Ivector2	drawLT;
	drawLT.set					(realCellsPosLT.x + map_abs_pos.x, realCellsPosLT.y + map_abs_pos.y);

	float		fw				= FOG_CELL_SZ*m->GetCurrentZoom()*UI()->GetScaleX();
	float		fh				= FOG_CELL_SZ*m->GetCurrentZoom()*UI()->GetScaleY();
	float		ft				= 1.f/4.f;

	const Fvector2	pts[9]=	{	{0.0f,0.0f},{0.5f,0.0f},{1.0f,0.0f},
								{0.0f,0.5f},{0.5f,0.5f},{1.0f,0.5f},
								{0.0f,1.0f},{0.5f,1.0f},{1.0f,1.0f}		};
	const u32 idx[4][2][3]	={	{{0,4,3},{0,1,4}}, {{1,5,4},{1,2,5}},
								{{3,7,6},{3,4,7}}, {{4,8,7},{4,5,8}}	};
//	const Fvector2	ts[16]=	{	{0.0f,0.0f},{0.5f,0.0f},{1.0f,0.0f},
	const float eps = .01f;
	const Fvector2 uvs[2][3]={	{{0.0f,0.0f},{1.0f-eps,1.0f-eps},{0.0f,1.0f-eps}},
								{{0.0f,0.0f},{1.0f-eps,0.0f},{1.0f-eps,1.0f-eps}}};
	RCache.set_Shader			(hShader);
	CTexture* T					= RCache.get_ActiveTexture(0);
	Fvector2					hp;
	hp.set						(0.5f/float(T->get_Width()),0.5f/float(T->get_Height()));
//	hp.set(.0f,.0f);

	CRandom Random1(0);

	u32 vOffset					= 0;
	FVF::TL* start_pv			= (FVF::TL*)RCache.Vertex.Lock	(cells.width()*cells.height()*24,hGeom.stride(),vOffset);
	FVF::TL* pv					= start_pv;
	for (u32 x=0; x<cells.width(); ++x){
		for (u32 y=0; y<cells.height(); ++y){
			for(u32 c=0; c<4; ++c){
				Ivector2 frame;
				GetTexFrame		(frame,c,cells.x1+x,cells.y1+y);
				Fvector2 tp;
				tp.set			(frame.x*ft,frame.y*ft);
				for(u32 f=0; f<2; ++f){
					for(u32 v=0; v<3;++v,pv++){
						const Fvector2& p = pts[idx[c][f][v]];
						const Fvector2& uv= uvs[f][v];
						pv->set(drawLT.x + p.x*fw +fw*x, drawLT.y + p.y*fh +fh*y, 0xFFFFFFFF,tp.x+uv.x*ft+hp.x,tp.y+uv.y*ft+hp.y);
					}
				}
			}
		}
	}

	std::ptrdiff_t p_cnt		= (pv-start_pv)/3;
	RCache.Vertex.Unlock		(u32(pv-start_pv),hGeom.stride());
	// set scissor
	UI()->PushScissor			(clip_rect);
	// set geom
	if (p_cnt!=0){
		RCache.set_Geometry		(hGeom);
//		RCache.set_CullMode		(CULL_NONE);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,u32(p_cnt));
	}
	UI()->PopScissor			();




/*
	if(0&&!(Device.dwFrame%300) ){
		Msg("clip_rect=[%d][%d]-[%d][%d]",clip_rect.lt.x, clip_rect.lt.y, clip_rect.rb.x, clip_rect.rb.y);
		Msg("vis_rect =[%d][%d]-[%d][%d]",vis_rect.lt.x, vis_rect.lt.y, vis_rect.rb.x, vis_rect.rb.y);
		Msg("rect_real(tgt) =[%f][%f]-[%f][%f]",tgt.lt.x, tgt.lt.y, tgt.rb.x, tgt.rb.y);

		Msg("cell_pos       =[%f][%f]-[%f][%f]",realCellsPosLT.x, realCellsPosLT.y, realCellsPosRB.x, realCellsPosRB.y);
		Msg("rect_draw      =[%d][%d]-[%d][%d]",drawLT.x, drawLT.y, drawRB.x, drawRB.y);


		Msg("cells=[%d][%d]-[%d][%d]",cells.lt.x, cells.lt.y, cells.rb.x, cells.rb.y);
		Msg("------------------------");

		CUIStatic	tmp;
		tmp.Init	("test",drawLT.x,drawLT.y,  
									drawRB.x - drawLT.x,
									drawRB.y - drawLT.y);
		tmp.SetStretchTexture(true);
		tmp.Draw();
	}*/
}
void CLevelFogOfWar::save	(IWriter &stream)
{
}

void CLevelFogOfWar::load	(IReader &stream)
{
}
