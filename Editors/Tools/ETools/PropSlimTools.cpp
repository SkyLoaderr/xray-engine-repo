#include "stdafx.h"
#include "PropSlimTools.h"
#include <MxStdModel.h>
#include <MxPropSlim.h>
#include <MxQSlim.h>

#pragma comment (lib,"x:/xrQSlim.lib")

#define MAX_DECIMATE_ERROR 0.1f
#define MAX_DECIMATE_ERROR_TOLERANCE 0.1f
#define COMPACTNESS_RATIO  0.5f
//0.000005f
namespace ETOOLS{
ETOOLS_API void ContractionClear		(QSContraction*& dst_conx)
{
	xr_delete				(dst_conx);
}

struct SCBParams{
	QSContraction*	contr;
	MxPropSlim*		slim;
	SCBParams(MxPropSlim*s, QSContraction* c):slim(s),contr(c){}
};

void contraction_callback(const MxPairContraction& conx, float err, void* cb_params)
{
	SCBParams* cb			= (SCBParams*)cb_params;

	// leave callback if change empty 
//	if ((0==conx.dead_faces.length())&&(0==(conx.delta_faces.length()-conx.delta_pivot))) return;

//	QSContractionItem* ci	= xr_new<QSContractionItem>(conx.dead_faces.length(),conx.delta_faces.length());
	QSContractionItem* ci	= 
		xr_new<QSContractionItem>(conx.dead_faces.length(),conx.delta_faces.length()-conx.delta_pivot);
	ci->v_kept				= conx.v1;
	ci->v_dead				= conx.v2;
	ci->error				= err;

	// dead faces
	const MxFaceList& N1	= conx.dead_faces;
	for (u32 f_idx=0; f_idx<(u32)N1.size(); f_idx++)
		ci->f_dead[f_idx]	= N1[f_idx];
	// changed faces
	const MxFaceList& N2	= conx.delta_faces;
	for (u32 f_idx=conx.delta_pivot; f_idx<(u32)N2.size(); f_idx++){
		ci->f_delta[f_idx-conx.delta_pivot]	= N2[f_idx];
//		info->heap_key(-10000.f);
//		cb->slim->model().vertex_mark_locked(F[0]);
//		cb->slim->model().vertex_mark_locked(F[1]);
//		cb->slim->model().vertex_mark_locked(F[2]);
	}
//	for (u32 f_idx=0; f_idx<(u32)N2.size(); f_idx++)
//		ci->f_delta[f_idx]	= N2[f_idx];

	cb->contr->AppendItem	(ci);
}

ETOOLS_API BOOL ContractionGenerate	(QSMesh* src_mesh, QSContraction*& dst_conx, u32 min_faces, float max_error)
{
	VERIFY					(src_mesh);
	VERIFY					(src_mesh->verts.size());
	VERIFY					(src_mesh->faces.size());

	// prepare model
	MxStdModel* mdl			= xr_new<MxStdModel>(src_mesh->verts.size(),src_mesh->faces.size());
	mdl->texcoord_binding	(MX_PERVERTEX);
	for (u32 v_idx=0; v_idx<src_mesh->verts.size(); v_idx++){
		QSVert& v			= src_mesh->verts[v_idx];
		mdl->add_vertex		(v.pt.x,v.pt.y,v.pt.z);
		mdl->add_texcoord	(v.uv.x,v.uv.y);
	}
	for (u32 f_idx=0; f_idx<src_mesh->faces.size(); f_idx++){
		QSFace& f			= src_mesh->faces[f_idx];
		mdl->add_face		(f.v[0],f.v[1],f.v[2]);
	}

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

	dst_conx				= xr_new<QSContraction>(src_mesh->verts.size());
	SCBParams				CBP(slim,dst_conx);

	// decimate
	slim->decimate			(0,MAX_DECIMATE_ERROR,&CBP);

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

	// -----
	xr_delete				(mdl);
	xr_delete				(slim);

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