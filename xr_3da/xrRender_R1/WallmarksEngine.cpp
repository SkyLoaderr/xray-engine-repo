// WallmarksEngine.cpp: implementation of the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WallmarksEngine.h"

#include "..\xr_object.h"
#include "..\x_ray.h"
#include "..\GameFont.h"

// #include "xr_effsun.h"

const float W_TTL			= 30.f;
const float W_DIST_FADE		= 15.f;
const float	W_DIST_FADE_SQR	= W_DIST_FADE*W_DIST_FADE;
const float I_DIST_FADE_SQR	= 1.f/W_DIST_FADE_SQR;
const int	MAX_TRIS		= 512;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWallmarksEngine::CWallmarksEngine	()
{
	pool.reserve	(256);
	marks.reserve	(256);

	hGeom	= Device.Shader.CreateGeom	(FVF::F_LIT, RCache.Vertex.Buffer(), NULL);
}

CWallmarksEngine::~CWallmarksEngine	()
{
	{
		for (u32 it=0; it<marks.size(); it++)
			wm_destroy	(marks[it]);
		marks.clear	();
	}
	{
		for (u32 it=0; it<pool.size(); it++)
			xr_delete	(pool[it]);
		pool.clear	();
	}

	Device.Shader.DeleteGeom			(hGeom);
}

// allocate
CWallmarksEngine::wallmark*	CWallmarksEngine::wm_allocate		(Shader*	S	)
{
	wallmark*			W = 0;
	if (pool.empty())	W = xr_new<wallmark> ();
	else				{ W = pool.back(); pool.pop_back(); }

	W->shader			= S;
	W->ttl				= W_TTL;
	W->verts.clear		();
	return W;
}
// destroy
void		CWallmarksEngine::wm_destroy		(wallmark*	W	)
{
	pool.push_back		(W);
}
// render
void		CWallmarksEngine::wm_render			(wallmark*	W, FVF::LIT* &V)
{
	float		a	= 1-(W->ttl/W_TTL);
	int			aC	= iFloor	( a * 255.f);	clamp	(aC,0,255);
	u32		C	= D3DCOLOR_RGBA(128,128,128,aC);

	FVF::LIT*	S	= &*W->verts.begin	();
	FVF::LIT*	E	= &*W->verts.end	();
	for (; S!=E; S++, V++)
	{
		V->p.set	(S->p);
		V->color	= C;
		V->t.set	(S->t);
	}
}

//--------------------------------------------------------------------------------
void AddTri(CDB::TRI* pTri, Fmatrix &mView, CWallmarksEngine::wallmark	&W)
{
	Fvector				UV;
	FVF::LIT			V;
	for (int i=0; i<3; i++)
	{
		mView.transform_tiny	(UV, *pTri->verts[i]);
		V.p.set					(*pTri->verts[i]);
		V.t.set					(UV.x,UV.y);
		W.verts.push_back		(V);
	}
}

void CWallmarksEngine::RecurseTri(CDB::TRI* T, Fmatrix &mView, CWallmarksEngine::wallmark	&W, CFrustum &F)
{
	// Check if triangle already processed
	if (std::find(sml_processed.begin(),sml_processed.end(),T)!=sml_processed.end())
		return;
	
	sml_processed.push_back(T);
	
	// Some vars
	sml_poly_src.clear		();
	sml_poly_src.push_back	(*T->verts[0]);
	sml_poly_src.push_back	(*T->verts[1]);
	sml_poly_src.push_back	(*T->verts[2]);
	sml_poly_dest.clear		();
	
	sPoly* P = F.ClipPoly	(sml_poly_src, sml_poly_dest);
	
	if (P) {
		// Create vertices and triangulate poly (tri-fan style triangulation)
		FVF::LIT			V0,V1,V2;
		Fvector				UV;

		mView.transform_tiny(UV, (*P)[0]);
		V0.set				((*P)[0],0,(1+UV.x)*.5f,(1-UV.y)*.5f);
		mView.transform_tiny(UV, (*P)[1]);
		V1.set				((*P)[1],0,(1+UV.x)*.5f,(1-UV.y)*.5f);

		for (u32 i=2; i<P->size(); i++)
		{
			mView.transform_tiny(UV, (*P)[i]);
			V2.set				((*P)[i],0,(1+UV.x)*.5f,(1-UV.y)*.5f);
			W.verts.push_back	(V0);
			W.verts.push_back	(V1);
			W.verts.push_back	(V2);
			V1					= V2;
		}
		
		// recurse
		for (i=0; i<3; i++)
		{
			CDB::TRI* SML = T->adj[i];
			if (SML)	{
				Fvector test_normal;
				test_normal.mknormal	(*SML->verts[0],*SML->verts[1],*SML->verts[2]);
				float cosa				= test_normal.dotproduct(sml_normal);
				if (cosa<EPS)			continue;
				RecurseTri				(SML,mView,W,F);
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
	if (_abs(sml_normal.y)>.99f) y.set(1,0,0);
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

	// create wallmark
	wallmark* W			= wm_allocate(hShader);
	RecurseTri			(pTri,mView,*W,F);
	sml_processed.clear	();

	// calc sphere
	if (W->verts.size()<3) { wm_destroy(W); return; }
	else {
		Fbox bb; bb.invalidate();

		FVF::LIT* I=&*W->verts.begin	();
		FVF::LIT* E=&*W->verts.end		();
		for (; I!=E; I++)	bb.modify	(I->p);
		bb.getsphere					(W->bounds.P,W->bounds.R);
	}

	// search if similar wallmark exists
	xr_vector<wallmark*>::iterator		it	=marks.begin();
	xr_vector<wallmark*>::iterator		end	=marks.end	();
	for (; it!=end; it++)
	{
		wallmark* wm	= *it;
		if (wm->shader != W->shader)	continue;
		if (wm->bounds.P.similar(W->bounds.P,0.02f))
		{
			// replace
			wm_destroy		(wm);
			*it				= W;
			return;
		}
	}

	// no similar - register _new_
	marks.push_back			(W);
}

extern float r_ssaDISCARD;
void CWallmarksEngine::Render()
{
	if (marks.empty())			return;

	// Projection and xform
	float _43					= Device.mProject._43;
	Device.mProject._43			-= 0.001f; 
	RCache.set_xform_world		(Fidentity);
	RCache.set_xform_project	(Device.mProject);

	Device.Statistic.RenderDUMP_WM.Begin	();

	u32					w_offset= 0;
	FVF::LIT*			w_verts = (FVF::LIT*)	RCache.Vertex.Lock	(MAX_TRIS*3,hGeom->vb_stride,w_offset);
	FVF::LIT*			w_start = w_verts;

	Shader*	w_S			= marks.front()->shader;
	float	ssaCLIP		= r_ssaDISCARD/4;
	for (u32 i=0; i<marks.size(); i++)
	{
		wallmark* W		= marks	[i];
		if (RImplementation.ViewBase.testSphere_dirty(W->bounds.P,W->bounds.R)) 
		{
			float dst = Device.vCameraPosition.distance_to_sqr(W->bounds.P);
			float ssa = W->bounds.R * W->bounds.R / dst;
			if (ssa>=ssaCLIP)	
			{
				u32 w_count	= w_verts-w_start;
				if (((w_count+W->verts.size())>=(MAX_TRIS*3))||(w_S!=W->shader))
				{
					if (w_count)	
					{
						// Flush stream
						RCache.Vertex.Unlock	(w_count,hGeom->vb_stride);
						RCache.set_Shader		(w_S);
						RCache.set_Geometry		(hGeom);
						RCache.Render			(D3DPT_TRIANGLELIST,w_offset,w_count/3);

						// Restart (re-lock/re-calc)
						w_verts		= (FVF::LIT*)	RCache.Vertex.Lock	(MAX_TRIS*3,hGeom->vb_stride,w_offset);
						w_start		= w_verts;
						w_S			= W->shader;
					}
				}

				wm_render	(W,w_verts);
			}
		}
		W->ttl -= Device.fTimeDelta;
	}

	// Flush stream
	u32 w_count			= w_verts-w_start;
	RCache.Vertex.Unlock	(w_count,hGeom->vb_stride);
	if (w_count)			
	{
		RCache.set_Shader		(w_S);
		RCache.set_Geometry		(hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,w_offset,w_count/3);
	}

	// Remove last used wallmarks
	if (marks.front()->ttl<=EPS)	
	{
		wm_destroy	(marks.front());
		marks.erase	(marks.begin());
	}
	Device.Statistic.RenderDUMP_WM.End		();

	// Projection
	Device.mProject._43			= _43;
	RCache.set_xform_project	(Device.mProject);
}
