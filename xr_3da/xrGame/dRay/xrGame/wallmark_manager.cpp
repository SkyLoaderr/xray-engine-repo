#include "stdafx.h"
#include "wallmark_manager.h"
#include "Level.h"
#include "GameMtlLib.h"
#include "CalculateTriangle.h"
#include "profiler.h"
#ifdef DEBUG
#include "phdebug.h"
#endif

CWalmarkManager::CWalmarkManager()
{
	m_wallmarks		=NULL		;
	m_trace_dist	=-dInfinity	;
	m_wallmark_size	=-dInfinity	;
}
CWalmarkManager::~CWalmarkManager()
{
	Clear();
}
void CWalmarkManager::Clear()
{
	if(m_wallmarks)m_wallmarks->clear();
	xr_delete(m_wallmarks);
}

void CWalmarkManager::AddWallmark(const Fvector& dir, const Fvector& start_pos, 
								  float range, float wallmark_size,
								  SHADER_VECTOR& wallmarks_vector,int t)
{
	CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+t;//result.element;
	SGameMtl*	pMaterial = GMLib.GetMaterialByIdx(pTri->material);

	if(pMaterial->Flags.is(SGameMtl::flBloodmark))
	{
		//вычислить нормаль к пораженной поверхности
		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();

		//вычислить точку попадания
		Fvector end_point;
		end_point.set(0,0,0);
		end_point.mad(start_pos, dir, range);

		ref_shader* pWallmarkShader = wallmarks_vector.empty()?NULL:
		&wallmarks_vector[::Random.randI(0,wallmarks_vector.size())];

		if (pWallmarkShader)
		{
			//добавить отметку на материале
			::Render->add_StaticWallmark(*pWallmarkShader, end_point,
				wallmark_size, pTri, pVerts);
		}
	}
}

void CWalmarkManager::PlaceWallmark(const Fvector& dir, const Fvector& start_pos, 
									  float trace_dist, float wallmark_size,
									  SHADER_VECTOR& wallmarks_vector,CObject* ignore_obj)
{
	collide::rq_result	result;
	BOOL				reach_wall = 
		Level().ObjectSpace.RayPick(
		start_pos,
		dir,
		trace_dist, 
		collide::rqtBoth,
		result,
		ignore_obj
		)
		&&
		!result.O;

	//если кровь долетела до статического объекта
	if(reach_wall)
	{
		AddWallmark(dir,start_pos,result.range,wallmark_size,wallmarks_vector,result.element);
	}
}
static const u32		max_wallmarks_count	=2;
void CWalmarkManager::PlaceWallmarks( const Fvector& start_pos, float trace_dist, float wallmark_size,SHADER_VECTOR& wallmarks_vector,CObject* ignore_obj)
{
START_PROFILE("Explosive wallmarks");
	///////////////////////////////////////////////////////////////////////////////////////////////
START_PROFILE("Explosive wallmarks/query");
	XRC.box_options                (0);
	XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),start_pos,Fvector().set(trace_dist,trace_dist,trace_dist));
STOP_PROFILE
	CDB::TRI			*T_array    = Level().ObjectSpace.GetStaticTris();
	Fvector				*V_array	= Level().ObjectSpace.GetStaticVerts();
	CDB::RESULT*    R_begin                         = XRC.r_begin()	;
	CDB::RESULT*    R_end                           = XRC.r_end()	;
	u32				wallmarks_count					=0				;
	Triangle		ntri											;
	float			ndist											=dInfinity;
	Fvector			npoint											;

	for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
	{

		if(wallmarks_count>max_wallmarks_count)return;
		SGameMtl*	pMaterial = GMLib.GetMaterialByIdx(Res->material);
		if(!pMaterial->Flags.is(SGameMtl::flBloodmark))continue;
		Triangle	tri;
		Fvector end_point;
		ETriDist c;
START_PROFILE("Explosive wallmarks/process tries");
		CalculateTriangle(T_array+Res->id,cast_fp(start_pos),tri);
		//if(!TriPlaneContainPoint(&tri)||!TriContainPoint(&tri,cast_fp(start_pos)))	continue;
		Fvector pdir;
		float dist=DistToTri(&tri,cast_fp(start_pos),cast_fp(pdir),cast_fp(end_point),c,V_array);
		if(c==tdBehind)	continue;
		float test=dist-EPS_L;
		if(test>0.f)
		{
			if(Level().ObjectSpace.RayTest(start_pos,pdir,test, collide::rqtStatic,NULL,ignore_obj))continue;
		}
		if(c!=tdPlane)
		{
			if(dist<ndist)
			{
				ndist=dist;
				npoint.set(end_point);
				ntri=tri;
			}
			continue;
		}

STOP_PROFILE
		ref_shader* pWallmarkShader = wallmarks_vector.empty()?NULL:
		&wallmarks_vector[::Random.randI(0,wallmarks_vector.size())];
		if(pWallmarkShader)
		{
			++wallmarks_count;

			//end_point.mad(start_pos, cast_fv(tri.norm), -tri.dist);
#ifdef DEBUG
			//DBG_OpenCashedDraw();
			//DBG_DrawPoint(end_point,0.01f,D3DCOLOR_XRGB(0,255,255));
			//DBG_DrawTri(tri.T,V_array,D3DCOLOR_XRGB(255,255,255));
			//DBG_ClosedCashedDraw(30000);
#endif
START_PROFILE("Explosive wallmarks/add wallmark")
			::Render->add_StaticWallmark(*pWallmarkShader, end_point,
				wallmark_size, tri.T,V_array);
STOP_PROFILE
			
		}

	}
	if(wallmarks_count==0&&ndist<=trace_dist)
	{
#ifdef DEBUG
		//DBG_OpenCashedDraw();
		//DBG_DrawPoint(npoint,0.01f,D3DCOLOR_XRGB(0,255,0));
		//DBG_DrawTri(ntri.T,V_array,D3DCOLOR_XRGB(255,255,255));
		//DBG_ClosedCashedDraw(30000);
#endif
		ref_shader* pWallmarkShader = wallmarks_vector.empty()?NULL:
		&wallmarks_vector[::Random.randI(0,wallmarks_vector.size())];
		if(pWallmarkShader)
		{
			START_PROFILE("Explosive wallmarks/add wallmark")
				::Render->add_StaticWallmark(*pWallmarkShader, npoint,
				wallmark_size, ntri.T,V_array);
			STOP_PROFILE
		}
	}
//Msg("wallmarks count %d",wallmarks_count);
STOP_PROFILE
}

void CWalmarkManager::PlaceWallmarks(const Fvector& start_pos,CObject* ignore_obj)
{
	if(m_wallmarks)
			PlaceWallmarks(start_pos,m_trace_dist,m_wallmark_size,*m_wallmarks,ignore_obj);
}
void CWalmarkManager::Load (CInifile	*ltx,	LPCSTR section)
{
	VERIFY					(0==m_wallmarks);
	m_trace_dist	=ltx->r_float(section,"dist");
	m_wallmark_size	=ltx->r_float(section,"size");
	
	m_wallmarks		= xr_new<SHADER_VECTOR>		();
	
	//кровавые отметки на стенах
	string256	tmp;
	LPCSTR wallmarks_name = ltx->r_string(section, "wallmarks"); 

	int cnt		=_GetItemCount(wallmarks_name);
	VERIFY		(cnt);
	ref_shader	s;
	for (int k=0; k<cnt; ++k)
	{
		s.create ("effects\\wallmark",_GetItem(wallmarks_name,k,tmp));
		m_wallmarks->push_back	(s);
	}


	//m_fBloodMarkSizeMin = pSettings->r_float(section, "min_size"); 
	//m_fBloodMarkSizeMax = pSettings->r_float(section, "max_size"); 
	//m_fBloodMarkDistance = pSettings->r_float(section, "dist"); 
	//m_fNominalHit = pSettings->r_float(section, "nominal_hit"); 




}



