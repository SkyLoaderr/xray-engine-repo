#include "stdafx.h"
#include "PropSlimTools.h"
#include "object.h"
#include "object_sliding.h"

static Object*					g_pObject						= 0;
static ArbitraryList<MeshPt*>	g_ppTempPts						= 0;
static float					g_fSlidingWindowErrorTolerance	= 0.1f;
static BOOL						g_bOptimiseVertexOrder			= FALSE;
static u32						g_bMaxSlidingWindow				= u32(-1);
static VIPM_Result*				g_pResult						= 0;

ETOOLS_API void			VIPM_Init			()
{
	R_ASSERT2	(0==g_pObject,"VIPM already in use!");
	g_pObject							= xr_new<Object>();
	g_pResult							= xr_new<VIPM_Result>();
	g_pObject->iNumCollapses			= 0;
	g_pObject->iCurSlidingWindowLevel	= 0;
}

ETOOLS_API void			VIPM_AppendVertex	(const Fvector3& p, const Fvector2& uv)
{
	MeshPt* pt			= xr_new<MeshPt>	(&g_pObject->CurPtRoot);
	g_ppTempPts.push_back(pt);
	pt->mypt.vPos 		= p;
	pt->mypt.fU			= uv.x;
	pt->mypt.fV			= uv.y;
	pt->mypt.dwIndex	= g_ppTempPts.size()-1;
}

ETOOLS_API void			VIPM_AppendFace		(u16 v0, u16 v1, u16 v2)
{
	xr_new<MeshTri>(g_ppTempPts[v0],g_ppTempPts[v1],g_ppTempPts[v2], &g_pObject->CurTriRoot, &g_pObject->CurEdgeRoot );
}

void CalculateAllCollapses(Object* m_pObject, float m_fSlidingWindowErrorTolerance=1.f)
{
	m_pObject->BinEdgeCollapse();
	while (true){
		// Find the best collapse you can.
		// (how expensive is this? Ohhhh yes).
		float		fBestError			= 1.0e10f;
		MeshEdge	*pedgeBestError		= NULL;
		MeshPt		*pptBestError		= NULL;
		// NL = NewLevel - would force a new level.
		float		fBestErrorNL		= 1.0e10f;
		MeshEdge	*pedgeBestErrorNL	= NULL;
		MeshPt		*pptBestErrorNL		= NULL;
		MeshPt		*ppt;
		MeshEdge	*pedge;

		float		fAverage			= 0.0f;
		int			iAvCount			= 0;

		// Flush the cache, just in case.
		m_pObject->FindCollapseError		( NULL, NULL, FALSE );

		for ( ppt = m_pObject->CurPtRoot.ListNext(); ppt != NULL; ppt = ppt->ListNext() ){
			if (0==ppt->FirstEdge())	continue;
			// Disallow any pts that are on an edge - shouldn't be collapsing them.
			BOOL bAllowed = TRUE;
			for ( pedge = ppt->FirstEdge(); pedge != NULL; pedge = ppt->NextEdge() ){
				if ( ( pedge->pTri12 == NULL ) || ( pedge->pTri21 == NULL ) ){
					// This edge does not have two tris on it - disallow it.
					bAllowed = FALSE;
					break;
				}
			}
			if ( !bAllowed ) continue;

			BOOL bRequiresNewLevel = FALSE;
			if ( !m_pObject->CollapseAllowedForLevel ( ppt, m_pObject->iCurSlidingWindowLevel ) ){
				// This collapse would force a new level.
				bRequiresNewLevel = TRUE;
			}

			// collect error
			for ( pedge = ppt->FirstEdge(); pedge != NULL; pedge = ppt->NextEdge() ){
				float fErrorBin = m_pObject->FindCollapseError ( ppt, pedge, TRUE );
				iAvCount++;
				fAverage += fErrorBin;
				if ( bRequiresNewLevel ){
					if ( fBestErrorNL > fErrorBin ){
						fBestErrorNL = fErrorBin;
						pedgeBestErrorNL = pedge;
						pptBestErrorNL = ppt;
					}
				}else{
					if ( fBestError > fErrorBin ){
						fBestError = fErrorBin;
						pedgeBestError = pedge;
						pptBestError = ppt;
					}
				}
			}
		}
		fAverage /= (float)iAvCount;

		// Tweak up the NewLevel errors by a factor.
		if ( fBestError > ( fBestErrorNL + fAverage * m_fSlidingWindowErrorTolerance ) ){
			// Despite the boost, it's still the best,
			// so bite the bullet and do the collapse.
			fBestError = fBestErrorNL;
			pedgeBestError = pedgeBestErrorNL;
			pptBestError = pptBestErrorNL;
		}

		//-----------------------------------------------------------------------------------------------------------
		// Do we need to do any collapses?
		// Collapse auto-found edge.
		if ( ( pedgeBestError != NULL ) && ( pptBestError != NULL ) ){
			MeshPt *pKeptPt = pedgeBestError->OtherPt ( pptBestError ); 
			VERIFY ( pKeptPt != NULL );
			m_pObject->CreateEdgeCollapse ( pptBestError, pKeptPt );
		}else{
			break;
		}
	}
}

ETOOLS_API VIPM_Result*	VIPM_Convert		(u32 max_sliding_window, float error_tolerance, BOOL optimize_vertex_order)
{
	g_pObject->SetNewLevel	(0);
	CalculateAllCollapses	(g_pObject,error_tolerance);
	CalculateSW				(g_pObject,g_pResult);
	return g_pResult;
}

ETOOLS_API void			VIPM_Destroy		()
{
	xr_delete			(g_pResult);
	xr_delete			(g_pObject);
	g_ppTempPts.resize	(0);
}
/*
ETOOLS_API BOOL ContractionGenerate		(QSMesh* src_mesh, QSContraction*& dst_conx, u32 min_faces, float max_error)
{

	dst_conx				= xr_new<QSContraction>(src_mesh->verts.size());

	CalculateAllCollapses	(m_pObject);
	OMSlidingWindow* SM		= xr_new<OMSlidingWindow>(m_pObject);
	
	SM->Update				();

	// Find the record.
	int iLoD = 700;
	if ( iLoD < 0 )						iLoD = 0;
	else if ( iLoD > SM->iNumCollapses )iLoD = SM->iNumCollapses;
	SlidingWindowRecord *pswr = SM->swrRecords.item ( iLoD );

	// write SMF
	IWriter* W			= FS.w_open("x:\\import\\original.smf");
	string256 tmp;
	// vertices
	for (u32 v_idx=0; v_idx<SM->vertices.size(); v_idx++){
		ETOOLS::QSVert& v = SM->vertices[v_idx];
		sprintf			(tmp,"v %f %f %f",v.pt.x,v.pt.y,-v.pt.z);
		W->w_string		(tmp);
	}
	// transfer faces
	for (u32 f_idx=0; f_idx<pswr->wNumTris; f_idx++){
		ETOOLS::QSFace& F = SM->indices[f_idx+pswr->dwFirstIndexOffset/3];
		sprintf			(tmp,"f %d %d %d",F.v[2]+1,F.v[1]+1,F.v[0]+1);
		W->w_string		(tmp);
	}
	FS.w_close	(W);

	// -----
	xr_delete				(SM);
	xr_delete				(m_pObject);

	return TRUE;
}
*/