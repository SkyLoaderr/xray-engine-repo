// WallmarksEngine.cpp: implementation of the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_creator.h"
#include "WallmarksEngine.h"
#include "frustum.h"
#include "xr_object.h"
#include "xr_effsun.h"

#include "fstaticrender.h"

#include "x_ray.h"
#include "xr_smallfont.h"

#define MAX_TRIS	1024
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
IC void AddTri(RAPID::tri* pTri,Fmatrix &mView,CWallmark	&W)
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

static vector<RAPID::tri*>	sml_processed;
static Fvector				sml_normal;
static sPoly				sml_poly_dest;
static sPoly				sml_poly_src;

IC void RecurseTri(RAPID::tri* T, Fmatrix &mView,CWallmark	&W,CFrustum &F)
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
			RAPID::tri* SML = T->adj[i];
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
	mView.mul2			(mScale);
}

void CWallmarksEngine::AddWallmark	(RAPID::tri* pTri, const Fvector &contact_point, Shader* hShader, float sz)
{
	// calc face normal
	Fvector	N;
	N.mknormal			(*pTri->verts[0],*pTri->verts[1],*pTri->verts[2]);
	sml_normal.set		(N);

	// build 3D ortho-frustum
	Fmatrix				mView;
	BuildMatrix			(mView,1/sz,contact_point);
	CFrustum			F;
	F.CreateFromMatrix	(mView);

	// create empty wallmark
	marks.push_back		(CWallmark());
	CWallmark	&W		= marks.back();

	W.Create			(hShader);
	RecurseTri			(pTri,mView,W,F);

	sml_processed.clear();

	if (W.verts.size()<3) marks.pop_back();
	else {
		// Calc sphere
		Fbox bb; bb.invalidate();

		CWallmark::Vertex* I=W.verts.begin	();
		CWallmark::Vertex* E=W.verts.end	();
		for (; I!=E; I++)	bb.modify(I->P);

		bb.getsphere(W.S.P,W.S.R);
	}
}

extern float g_fSCREEN;
void CWallmarksEngine::Render()
{
	if (shadow_casters.empty() && marks.empty())	return;

	// Projection and xform
	float _43 = Device.mProject._43;
	Device.mProject._43 -= 0.001f; 
	CHK_DX(HW.pDevice->SetTransform	 ( D3DTS_WORLD,			precalc_identity.d3d() ));
	CHK_DX(HW.pDevice->SetTransform	 ( D3DTS_PROJECTION,	Device.mProject.d3d() ));

	if (!marks.empty()) 
	{
		Device.Statistic.RenderDUMP_WM.Begin	();

		DWORD				w_offset;
		CWallmark::Vertex*	w_verts = (CWallmark::Vertex*)VS->Lock	(MAX_TRIS*3,w_offset);
		CWallmark::Vertex*	w_start = w_verts;

		Shader*	w_S			= marks.front().hShader;
		for (DWORD i=0; i<marks.size(); i++)
		{
			CWallmark& W = marks[i];
			if (::Render.ViewBase.testSphereDirty(W.S.P,W.S.R)) 
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
	}

	if (!shadow_casters.empty()) 
	{
		Device.Statistic.RenderDUMP_Shads.Begin	();
		Fvector sDir	= pCreator->Environment.Suns[0]->Direction();
		sDir.y -= 4;	sDir.normalize();
		sml_normal.invert	(sDir);
		for (DWORD i=0; i<shadow_casters.size(); i++)
		{
			// Take params from the entity
			CObject* E			= shadow_casters[i];
			
			Shader* Shader		= E->shadowShader();
			float	Size		= E->shadowSize  ();
			
			Fvector				sPOS;
			E->clCenter			(sPOS);
			sPOS.direct			(sPOS,sml_normal,Size);
			
			// Build 3D ortho-frustum
			Fmatrix				mView;
			BuildMatrix			(mView,1/Size,sPOS);
			CFrustum			F;
			F.CreateFromMatrix	(mView);
			
			// Build box & xform
			Fbox				BB; 
			Fmatrix				M_bbox;
			float				range	= W_DIST_FADE;
			{
				Fvector				D,N,R;
				D.set				(sDir);
				if(1-fabsf(D.y)<EPS)N.set(1,0,0);
				else            	N.set(0,1,0);
				R.crossproduct		(N,D);
				N.crossproduct		(D,R);
				M_bbox.set			(R,D,N,sPOS);
				float Ra			= Size*2.f;
				BB.set				(-Ra,0,-Ra,Ra,range,Ra);
			}
			
			pCreator->ObjectSpace.BoxQuery(BB,M_bbox,clGET_TRIS|clQUERY_STATIC|clCOARSE);
			CList<clQueryTri>& L = pCreator->ObjectSpace.q_result.tris;
			if (L.size()>0) {
				Fvector& cPOS = Device.vCameraPosition;
				Fvector& cDIR = Device.vCameraDirection;
				
				DWORD				vOffset;
				CWallmark::Vertex*	VB	= (CWallmark::Vertex*)VS->Lock(MAX_TRIS*3,vOffset);
				CWallmark::Vertex*	B	= VB;
				for (DWORD i=0; i<L.size(); i++)
				{
					clQueryTri& T	= L[i];
					
					// 1. Culling
					Fvector	TN;
					TN.mknormal(T.p[0],T.p[1],T.p[2]);
					if (TN.dotproduct(sDir)>=0)		continue;
					if (TN.dotproduct(cDIR)>0.3f)	continue;
					
					// 2. Clipping
					sml_poly_src.clear		();
					sml_poly_src.push_back	(T.p[0]);
					sml_poly_src.push_back	(T.p[1]);
					sml_poly_src.push_back	(T.p[2]);
					sml_poly_dest.clear		();
					sPoly* P = F.ClipPoly	(sml_poly_src, sml_poly_dest);
					if (0==P) continue;
					
					// 3. XForm and VB-fill
					// Create vertices and triangulate poly (tri-fan style triangulation)
					static CWallmark::Vertex	V0,V1,V2;
					static Fvector				UV;
					
					{	// 0
						Fvector&	V	= (*P)[0];
						float		a1	= sPOS.distance_to_sqr(V)*I_DIST_FADE_SQR;
						float		a2	= cPOS.distance_to_sqr(V)*I_DIST_FADE_SQR;
						int			a	= iFloor(255.f*(a1+a2)); clamp(a,0,255);
						mView.transform_tiny(UV,V);
						V0.set			(V,D3DCOLOR_RGBA(a,a,a,a),UV.x,UV.y);
					}
					{	// 1
						Fvector&	V	= (*P)[1];
						float		a1	= sPOS.distance_to_sqr(V)*I_DIST_FADE_SQR;
						float		a2	= cPOS.distance_to_sqr(V)*I_DIST_FADE_SQR;
						int			a	= iFloor(255.f*(a1+a2)); clamp(a,0,255);
						mView.transform_tiny(UV,V);
						V1.set			(V,D3DCOLOR_RGBA(a,a,a,a),UV.x,UV.y);
					}
					
					// 2..n-1
					for (DWORD j=2; j<P->size(); j++)
					{
						Fvector&	V = (*P)[j];
						float		a1	= sPOS.distance_to_sqr(V)*I_DIST_FADE_SQR;
						float		a2	= cPOS.distance_to_sqr(V)*I_DIST_FADE_SQR;
						int			a	= iFloor(255.f*(a1+a2)); clamp(a,0,255);
						mView.transform_tiny(UV,V);
						V2.set			(V,D3DCOLOR_RGBA(a,a,a,a),UV.x,UV.y);
						
						*VB++			= V0;
						*VB++			= V1;
						*VB++			= V2;
						
						V1 = V2;
					}
				}
				DWORD vCount = VB-B;
				VS->Unlock	(vCount);
				if (vCount) {
					Device.Shader.set_Shader(Shader);
					Device.Primitive.Draw	(VS,vCount/3,vOffset);
				}
			}
		}
		shadow_casters.clear();
		Device.Statistic.RenderDUMP_Shads.End	();
	}
	
	// Projection
	Device.mProject._43 = _43;
	CHK_DX(HW.pDevice->SetTransform	 ( D3DTS_PROJECTION,Device.mProject.d3d() ));
}

void CWallmarksEngine::AddShadow(CObject* E)
{ 
	if (pCreator->Environment.Suns.empty()) return;
	if (E->Position().distance_to(Device.vCameraPosition)+E->Radius() < W_DIST_FADE)
		shadow_casters.push_back(E);
};
