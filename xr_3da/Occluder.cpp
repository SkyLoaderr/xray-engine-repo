// Occluder.cpp: implementation of the COccluder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Occluder.h"
//#include "fstaticrender.h"
#include "fbasicvisual.h"

// font
#include "x_ray.h"
#include "xr_smallfont.h"
#include "xr_input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const DWORD				 XR_MAX_PORTAL_VERTS	= 6;
struct b_occluder
{
	WORD				sector;				
	svector<Fvector,XR_MAX_PORTAL_VERTS>	vertices;
};
struct b_portal
{
	WORD				sector_front;
	WORD				sector_back;
	svector<Fvector,XR_MAX_PORTAL_VERTS>	vertices;
};
//////////////////////////////////////////////////////////////////////

void COccluderSystem::Load(CStream *fs)
{
	DWORD size	= fs->Length();
	if (0==size)	return;

	DWORD one	= sizeof(b_occluder);
	R_ASSERT(size%one==0);
	DWORD count = size/one;

	occluders.resize(count);
	for (DWORD I=0; I<count; I++)
	{
		b_occluder		L;
		fs->Read		(&L,one);

		// register
		SOccluderDef	def;
		for (u32 j=0; j<L.vertices.size(); j++)		def.verts.push_back	(L.vertices[j]);
		def.sphere.compute		(def.verts.begin(),def.verts.size());
		occluders.push_back	(def);
	}
}


float psOSSR = 0.003f;

IC	float	edge(Fvector &a, Fvector &b)
{	return sqrtf((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)); }
IC  float		triArea(Fvector &p0, Fvector &p1, Fvector &p2)
{
	float	e1 = edge(p0,p1);
	float	e2 = edge(p0,p2);
	float	e3 = edge(p1,p2);
	
	float	p  = (e1+e2+e3)/2.f;
	return	sqrtf( p*(p-e1)*(p-e2)*(p-e3) );
}

void COccluderSystem::Select(CFrustum& F)
{
	// Build base frustum
	baseF = &F;
	
	if (psDeviceFlags&rsOcclusion) {
		// Clip occluders
		svector<SOccluder,128>	inside;
		sPoly					poly_dest,poly_src;
		for (DWORD I=0; I<occluders.size(); I++)
		{
			SOccluderDef&		occ_def = occluders[I];
			if (!F.testSphere_dirty(occ_def.sphere.P,occ_def.sphere.R))	continue;
			
			svector<Fvector,8>	&verts	= occ_def.verts;
			
			poly_src.assign		(verts.begin(),verts.size());
			
			sPoly*		R = baseF->ClipPoly(poly_src,poly_dest);
			if (R) {
				// occluder occupies some part of view-frustum
				SOccluder& O = inside.last();
				O.id = I;
				
				// 1. transform
				sPoly&	S = *R;
				sPoly&	D = (R==&poly_src)?poly_dest:poly_src;	D.clear();
				for (DWORD i=0; i<S.size(); i++) {
					Device.mFullTransform.transform(D.last(),S[i]);
					D.inc();
				}
				VERIFY(D.size()==S.size());
				
				// 2. calc area
				O.area = 0;
				for (i=2; i<D.size(); i++) O.area += triArea(D[0],D[i-1],D[i]);
				if (O.area<psOSSR) continue;
				
				// 3. save it and build volume
				Fplane	P;
				P.build	(S[0],S[1],S[2]);
				if (P.classify(Device.vCameraPosition)<0) reverse(S.begin(),S.end());
				O.F.CreateOccluder(S.begin(),S.size(),*baseF);
				inside.inc();
			}
		}
		
		if (inside.size()) {
			// Sort them by screen-space area
			if (inside.size()>1) {
				std::sort(inside.begin(),inside.end());
				
				// Cull smallest against largest
				for (I=0; I<inside.size(); I++)
				{
					CFrustum &F = inside[I].F;
					for (DWORD j=I+1; j<inside.size(); j++)
					{
						svector<Fvector,8> &poly = occluders[inside[j].id].verts;
						if (F.testPolyInside(poly.begin(),poly.size()))
						{
							// erase occluder #j
							inside.erase(j);
							j--;
						}
					}
				}
			}
			
			// Transfer
			DWORD cnt = (inside.size()>7)?7:inside.size();
			selected.resize(cnt);
			for (I=0; I<cnt; I++) selected[I] = inside[I].F;
		} else {
			selected.clear();
		}
	} else {
		selected.clear();
	}
}

/*
struct OVert 
{
	Fvector p;
	DWORD	color;
};
#define OVert_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)

void COccluderSystem::DebugRender()
{
	Device.Primitive.Reset();
	Device.SetMode(vAlpha|vZWrite|vZTest);
	Device.Texture.Set(0);
	HW.pDevice->SetVertexShader(OVert_FVF);
	HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	OVert	V[4];
	WORD	I[6] = {0,1,2,2,3,0};

	DWORD	C = D3DCOLOR_RGBA(0,255,0,32);

	for (vPO_IT i=selected.begin(); i!=selected.end(); i++)
	{
		COccluder	&O = *(*i);
		V[0].p = O.p1; V[0].color = C;
		V[1].p = O.p2; V[0].color = C;
		V[2].p = O.p3; V[0].color = C;
		V[3].p = O.p4; V[0].color = C;
		HW.pDevice->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST,
			0, 4, 2, 
			I, D3DFMT_INDEX16, 
			V, sizeof(OVert));
	}

	HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
}
*/

EFC_Visible	COccluderSystem::visibleSphere	(CVisiCache &T, Fvector &C, float r)
{
	return baseF->testSphere(C,r,T.cache[0]);
}
EFC_Visible	COccluderSystem::visibleSphereNC(Fvector &C, float r)
{
	return baseF->testSphereDirty(C,r)?fcvFully:fcvNone;
}
EFC_Visible COccluderSystem::visibleVisual(CVisiCache &T, FBasicVisual* V)
{
	if (!selected.empty())
	{
		BOOL bPartial;
		switch (baseF->testSAABB(V->bv_Position,V->bv_Radius,V->bv_BBox.min,V->bv_BBox.max,T.cache[0]))
		{
		case fcvNone:	 return fcvNone;
		case fcvPartial: bPartial = true;	break;
		case fcvFully:	 bPartial = false;	break;
		default:		 NODEFAULT;
		}
		vector<CFrustum>::iterator I=selected.begin(),E=selected.end();
		BYTE* C = &T.cache[1];
		for (; I!=E; I++,C++)
		{
			if (0==*C) continue;
			switch (I->testSAABB(V->bv_Position,V->bv_Radius,V->bv_BBox.min,V->bv_BBox.max,*C))
			{
			case fcvNone:	 continue;						// means - fully visible
			case fcvPartial: bPartial = true;	continue;	// means - partial
			case fcvFully:	 return fcvNone;				// means - fully occluded
			default:		 NODEFAULT;
			}
		}
		return bPartial?fcvPartial:fcvFully;
	} else 
		return baseF->testSAABB(V->bv_Position,V->bv_Radius,V->bv_BBox.min,V->bv_BBox.max,T.cache[0]);
}

#pragma todo("Need curve poly by occluders")
sPoly*	COccluderSystem::clipPortal(sPoly& S, sPoly& D) 
{
	sPoly* P = baseF->ClipPoly(S,D);
	if (!P) return 0;
	if (selected.empty()) return P;

	vector<CFrustum>::iterator I=selected.begin(),E=selected.end();
	for (; I!=E; I++) {
		if (I->testPolyInside(P->begin(),P->size()))
			return 0;
	}
	return P;
}
