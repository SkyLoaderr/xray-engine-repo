#include "stdafx.h"
#include "wallmark_manager.h"
#include "Level.h"
#include "GameMtlLib.h"
#include "CalculateTriangle.h"

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

void CWalmarkManager::PlaceWallmarks( const Fvector& start_pos, float trace_dist, float wallmark_size,SHADER_VECTOR& wallmarks_vector,CObject* ignore_obj)
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	XRC.box_options                (0);
	XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),start_pos,Fvector().set(trace_dist,trace_dist,trace_dist));
	CDB::TRI			*T_array    = Level().ObjectSpace.GetStaticTris();
	Fvector				*V_array	= Level().ObjectSpace.GetStaticVerts();
	CDB::RESULT*    R_begin                         = XRC.r_begin()	;
	CDB::RESULT*    R_end                           = XRC.r_end()	;

	for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
	{
		SGameMtl*	pMaterial = GMLib.GetMaterialByIdx(Res->material);
		if(!pMaterial->Flags.is(SGameMtl::flBloodmark))continue;
		Triangle	tri;
		CalculateTriangle(T_array+Res->id,cast_fp(start_pos),tri);
		if(!TriPlaneContainPoint(&tri)||!TriContainPoint(&tri,cast_fp(start_pos)))	continue;


		ref_shader* pWallmarkShader = wallmarks_vector.empty()?NULL:
		&wallmarks_vector[::Random.randI(0,wallmarks_vector.size())];

		if(pWallmarkShader)
		{
			Fvector end_point;

			end_point.mad(start_pos, cast_fv(tri.norm), -tri.dist);	
			::Render->add_StaticWallmark(*pWallmarkShader, end_point,
				wallmark_size, tri.T,V_array);
		}
	}

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



