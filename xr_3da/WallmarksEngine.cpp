// WallmarksEngine.cpp: implementation of the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_creator.h"
#include "WallmarksEngine.h"
#include "frustum.h"
#include "xr_object.h"
#include "xr_effsun.h"

#include "render.h"

#include "x_ray.h"
#include "xr_smallfont.h"

#define MAX_TRIS	512
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWallmarksEngine::CWallmarksEngine()
{
	VS = Device.Streams.Create(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,MAX_TRIS*3);
}

CWallmarksEngine::~CWallmarksEngine()
{
}

//--------------------------------------------------------------------------------
IC void AddTri(CDB::TRI* pTri,Fmatrix &mView,CWallmark	&W)
{
	Fvector				UV;
	CWallmark::Vertex	V;
	for (int i=0; i<3; i++)
	{
		mView.transform_tiny(UV, *pTri->verts[i]);
		V.P.set(*pTri->verts[i]);
		V.tu = UV.x;
		V.tv = UV.y;
		W.verts.push_back(V);
	}
}

static vector<CDB::TRI*>	sml_processed;
static Fvector				sml_normal;
static sPoly				sml_poly_dest;
static sPoly				sml_poly_src;

IC void RecurseTri(CDB::TRI* T, Fmatrix &mView,CWallmark	&W,CFrustum &F)
{
	// Check if triangle already processed
	if (find(sml_processed.begin(),sml_processed.end(),T)!=sml_processed.end())
		return;
	
	sml_processed.push_back(T);
	
	// Some vars
	sml_poly_src.clear	();
	sml_poly_src.push_back(*T->verts[0]);
	sml_poly_src.push_back(*T->verts[1]);
	sml_poly_src.push_back(*T->verts[2]);
	sml_poly_dest.clear	();
	
	sPoly* P = F.ClipPoly (sml_poly_src, sml_poly_dest);
	
	if (P) {
		// Create vertices and triangulate poly (tri-fan style triangulation)
		static CWallmark::Vertex	V0,V1,V2;
		static Fvector				UV;

		mView.transform_tiny(UV, (*P)[0]);
		V0.set((*P)[0],0,UV.x,UV.y);
		mView.transform_tiny(UV, (*P)[1]);
		V1.set((*P)[1],0,UV.x,UV.y);

		for (DWORD i=2; i<P->size(); i++)
		{
			mView.transform_tiny(UV, (*P)[i]);
			V2.set((*P)[i],0,UV.x,UV.y);
			W.verts.push_back(V0);
			W.verts.push_back(V1);
			W.verts.push_back(V2);
			V1 = V2;
		}
		
		// recurse
		for (i=0; i<3; i++)
		{
			CDB::TRI* SML = T->adj[i];
			if (SML)	{
				Fvector test_normal;
				test_normal.mknormal_non_normalized(*SML->verts[0],*SML->verts[1],*SML->verts[2]);
				float cosa = test_normal.dotproduct(sml_normal);
				if (cosa<0.01f) continue;
				RecurseTri(SML,mView,W,F);
			}
		}
	}
}

void CWallmarksEngine::BuildMatrix	(Fmatrix &mView, float invsz, const Fvector& from)
{
	// build projection
	Fmatrix		mScale;
    Fvector		at,up,right,y;
	at.sub		(from,sml_normal);
	y.set		(0,1,0);
	if (fabsf(sml_normal.y)>.99f) y.set(1,0,0);
	right.crossproduct	(y,sml_normal);
	up.crossproduct		(sml_normal,right);
	mView.build_camera	(from,at,up);
	mScale.scale		(invsz,invsz,invsz);
	mView.mulA			(mScale);
}

void CWallmarksEngine::AddWallmark	(CDB::TRI* pTri, const Fvector &contact_point, Shader* hShader, float sz)
{
	// calc face normal
	Fvector	N;
	N.mknormal			(*pTri->verts[0],*pTri->verts[1],*pTri->verts[2]);
	sml_normal.set		(N);

	// build 3D ortho-frustum
	Fmatrix				mView,mRot;
	BuildMatrix			(mView,1/sz,contact_point);
	mRot.rotateZ		(::Random.randF(0,PI_MUL_2));
	mView.mulA_43		(mRot);
	CFrustum			F;
	F.CreateFromMatrix	(mView,FRUSTUM_P_LRTB);

	// create empty wallmark
	marks.push_back		(CWallmark());
	CWallmark	&W		= marks.back();

	W.Create			(hShader);
	RecurseTri			(pTri,mView,W,F);

	sml_processed.clear();

	// Calc sphere
	if (W.verts.size()<3) { marks.pop_back(); return; }
	else {
		Fbox bb; bb.invalidate();

		CWallmark::Vertex* I=W.verts.begin	();
		CWallmark::Vertex* E=W.verts.end	();
		for (; I!=E; I++)	bb.modify(I->P);

		bb.getsphere(W.S.P,W.S.R);
	}

	// Search if similar wallmark exists
	for (deque<CWallmark>::iterator it=marks.begin(); it!=marks.end(); it++)
	{
		CWallmark& wm = *it;
		if (wm.hShader != hShader)	continue;
		if (&wm == &W)				continue;

		if (wm.S.P.similar(W.S.P,0.01f))	
		{
			// replace
			wm.Copy			(W);
			marks.pop_back	();
			return;
		}
	}
}

extern float g_fSCREEN;
void CWallmarksEngine::Render()
{
	if (marks.empty())	return;

	// Projection and xform
	float _43 = Device.mProject._43;
	Device.mProject._43 -= 0.001f; 
	Device.set_xform_world	(Fidentity);
	Device.set_xform_project(Device.mProject);

	Device.Statistic.RenderDUMP_WM.Begin	();

	DWORD				w_offset;
	CWallmark::Vertex*	w_verts = (CWallmark::Vertex*)VS->Lock	(MAX_TRIS*3,w_offset);
	CWallmark::Vertex*	w_start = w_verts;

	Shader*	w_S			= marks.front().hShader;
	for (DWORD i=0; i<marks.size(); i++)
	{
		CWallmark& W = marks[i];
		if (::Render->ViewBase.testSphere_dirty(W.S.P,W.S.R)) 
		{
			float dst = Device.vCameraPosition.distance_to_sqr(W.S.P);
			float ssa = g_fSCREEN * W.S.R * W.S.R / dst;
			if (ssa>=1)	{
				DWORD w_count	= w_verts-w_start;
				if (((w_count+W.verts.size())>=MAX_TRIS*3)||(w_S!=W.hShader))
				{
					// Flush stream
					VS->Unlock				(w_count);
					Device.Shader.set_Shader(w_S);
					Device.Primitive.Draw	(VS,w_count/3,w_offset);

					// Restart (re-lock/re-calc)
					w_verts		= (CWallmark::Vertex*) VS->Lock	(MAX_TRIS*3,w_offset);
					w_start		= w_verts;
					w_S			= W.hShader;
				}

				W.Draw	(w_verts);
			}
		}
		W.ttl -= Device.fTimeDelta;
	}

	// Flush stream
	DWORD w_count			= w_verts-w_start;
	VS->Unlock				(w_count);
	if (w_count)			{
		Device.Shader.set_Shader(w_S);
		Device.Primitive.Draw	(VS,w_count/3,w_offset);
	}

	// Remove last used wallmarks
	while (!marks.empty() && (marks.front().ttl<=EPS_S)) marks.pop_front();

	Device.Statistic.RenderDUMP_WM.End		();

	// Projection
	Device.mProject._43 = _43;
	Device.set_xform_project	(Device.mProject);
}
