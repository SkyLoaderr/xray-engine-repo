#include "stdafx.h"
#include "PropSlimTools.h"
#include "object.h"

namespace ETOOLS{
ETOOLS_API void ContractionClear		(QSContraction*& dst_conx)
{
	xr_delete				(dst_conx);
}

void CalculateAllCollapses(Object* m_pObject, float m_fSlidingWindowErrorTolerance=0.1f)
{
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

ETOOLS_API BOOL ContractionGenerate		(QSMesh* src_mesh, QSContraction*& dst_conx, u32 min_faces, float max_error)
{
	VERIFY					(src_mesh);
	VERIFY					(src_mesh->verts.size());
	VERIFY					(src_mesh->faces.size());

	Object*	m_pObject		= xr_new<Object>();
	// prepare model
	// The de-index list.
	MeshPt **ppPts			= xr_alloc<MeshPt*>(src_mesh->verts.size());
	for (u32 v_idx=0; v_idx<src_mesh->verts.size(); v_idx++){
		QSVert& v			= src_mesh->verts[v_idx];
		MeshPt * pt			= xr_new<MeshPt>( &m_pObject->CurPtRoot );
		ppPts[v_idx]		= pt;
		pt->mypt.vPos 		= v.pt;
		pt->mypt.fU			= 1.f;//float(rand())/32767.f;
		pt->mypt.fV			= 1.f;//float(rand())/32767.f;
		pt->mypt.dwIndex	= v_idx;
	}
	for (u32 f_idx=0; f_idx<src_mesh->faces.size(); f_idx++){
		QSFace& f			= src_mesh->faces[f_idx];
		MeshTri *ptri		= xr_new<MeshTri>(ppPts[f.v[0]],ppPts[f.v[1]],ppPts[f.v[2]], &m_pObject->CurTriRoot, &m_pObject->CurEdgeRoot );
		ptri->mytri.dwIndex	= f_idx;
	}
	xr_free(ppPts);

	dst_conx				= xr_new<QSContraction>(src_mesh->verts.size());

	CalculateAllCollapses	(m_pObject);


/*
	// write SMF
	IWriter* W			= FS.w_open("x:\\import\\original.smf");
	string256 tmp;
	// vertices
	for (u32 v_idx=0; v_idx<mdl->vert_count(); v_idx++){
		sprintf			(tmp,"v %f %f %f",mdl->vertex(v_idx)[0],mdl->vertex(v_idx)[1],-mdl->vertex(v_idx)[2]);
		W->w_string		(tmp);
	}
	// transfer faces
	for (u32 f_idx=0; f_idx<mdl->face_count(); f_idx++){
		if (mdl->face_is_valid(f_idx)){
			MxFace& F		= mdl->face(f_idx);
			sprintf			(tmp,"f %d %d %d",F.v[2]+1,F.v[1]+1,F.v[0]+1);
			W->w_string		(tmp);
		}
	}
	FS.w_close	(W);
*/
	// -----
	xr_delete				(m_pObject);

	return TRUE;
}


/*

void CMyD3DApplication::CollapseAll()
{
// prepare model
MxStdModel* mdl			= xr_new<MxStdModel>(m_pObject->iFullNumPts,m_pObject->iFullNumTris);
MeshPt*	pt;
mdl->texcoord_binding	(MX_PERVERTEX);
for (pt=m_pObject->PermPtRoot.ListNext(); pt!=NULL; pt=pt->ListNext()){
mdl->add_vertex		(pt->mypt.vPos.x,pt->mypt.vPos.y,pt->mypt.vPos.z);
mdl->add_texcoord	(pt->mypt.fU,pt->mypt.fV);
}
MeshTri* tri;
for (tri=m_pObject->PermTriRoot.ListNext(); tri!=NULL; tri=tri->ListNext())
mdl->add_face		(tri->pPt1->mypt.dwIndex,tri->pPt2->mypt.dwIndex,tri->pPt3->mypt.dwIndex);


m_pObject->iCurSlidingWindowLevel = 0;
m_pObject->SetNewLevel ( m_pObject->iCurSlidingWindowLevel );


// create slim and set params
MxPropSlim* slim		= xr_new<MxPropSlim>(mdl);
slim->boundary_weight	= 1000.f;
slim->compactness_ratio	= COMPACTNESS_RATIO;
slim->meshing_penalty	= 1000.f;
slim->placement_policy	= MX_PLACE_ENDPOINTS;
slim->weighting_policy	= MX_WEIGHT_AREA_AVG;
slim->contraction_callback = contraction_callback;

// initialiez slim
slim->initialize		();

// collect edges
slim->collect_edges		();

// decimate
slim->decimate			(0,MAX_DECIMATE_ERROR,m_pObject);
//	mdl->compact_vertices	();

xr_delete				(m_pObject);
m_pObject				= xr_new<Object>();

// rebuild
MeshPt **ppPts = new MeshPt*[mdl->face_count()];
for (u32 i_idx=0; i_idx<mdl->vert_count(); i_idx++){
MxVertex& V				= mdl->vertex(i_idx);
ppPts[i_idx] 			= new MeshPt ( &m_pObject->PermPtRoot );
ppPts[i_idx]->mypt.vPos	= D3DXVECTOR3 (V[0],V[1],V[2]);
}
for (u32 f_idx=0; f_idx<mdl->face_count(); f_idx++){
if (mdl->face_is_valid(f_idx)){
MxFace& F		= mdl->face(f_idx);
MeshTri *ptri	= new MeshTri (ppPts[F[0]], ppPts[F[1]], ppPts[F[2]], &m_pObject->PermTriRoot, &m_pObject->PermEdgeRoot );
}
}
delete [] ppPts;

m_pObject->iFullNumTris = 0;
m_pObject->iFullNumPts	= 0;
pt = m_pObject->PermPtRoot.ListNext();
while ( pt != NULL )
{
// All the pts had better be the same material.
pt = pt->ListNext();
m_pObject->iFullNumPts++;
}
tri = m_pObject->PermTriRoot.ListNext();
while ( tri != NULL )
{
// All the pts had better be the same material.
tri = tri->ListNext();
m_pObject->iFullNumTris++;
}

MeshEdge *edge = m_pObject->PermEdgeRoot.ListNext();
while ( edge != NULL )
{
edge = edge->ListNext();
}

m_pObject->iCurSlidingWindowLevel = 0;
m_pObject->SetNewLevel ( m_pObject->iCurSlidingWindowLevel );

m_pObject->MakeCurrentObjectFromPerm();



xr_delete				(slim);
xr_delete				(mdl);
*/
}