//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ESceneWallmarkTools.h"

/*
void CWallmarksEngine::RecurseTri(u32 t, Fmatrix &mView, CWallmarksEngine::wallmark	&W)
{
	CDB::TRI*	T			= sml_collector.getT()+t;
	if (T->dummy)			return;
	T->dummy				= 0xffffffff;
	
	// Some vars
	u32*		v_ids		= T->verts;
	Fvector*	v_data		= sml_collector.getV();
	sml_poly_src.clear		();
	sml_poly_src.push_back	(v_data[v_ids[0]]);
	sml_poly_src.push_back	(v_data[v_ids[1]]);
	sml_poly_src.push_back	(v_data[v_ids[2]]);
	sml_poly_dest.clear		();
	
	sPoly* P = sml_clipper.ClipPoly	(sml_poly_src, sml_poly_dest);
	
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
			u32 adj					= sml_adjacency[3*t+i];
			if (0xffffffff==adj)	continue;
			CDB::TRI*	SML			= sml_collector.getT() + adj;
			v_ids					= SML->verts;

			Fvector test_normal;
			test_normal.mknormal	(v_data[v_ids[0]],v_data[v_ids[1]],v_data[v_ids[2]]);
			float cosa				= test_normal.dotproduct(sml_normal);
			if (cosa<EPS)			continue;
			RecurseTri				(adj,mView,W);
		}
	}
}

void CWallmarksEngine::BuildMatrix	(Fmatrix &mView, float invsz, const Fvector& from)
{
	// build projection
	Fmatrix				mScale;
    Fvector				at,up,right,y;
	at.sub				(from,sml_normal);
	y.set				(0,1,0);
	if (_abs(sml_normal.y)>.99f) y.set(1,0,0);
	right.crossproduct	(y,sml_normal);
	up.crossproduct		(sml_normal,right);
	mView.build_camera	(from,at,up);
	mScale.scale		(invsz,invsz,invsz);
	mView.mulA			(mScale);
}

void CWallmarksEngine::AddWallmark_internal	(CDB::TRI* pTri, const Fvector* pVerts, const Fvector &contact_point, ref_shader hShader, float sz)
{
	// query for polygons in bounding box
	// calculate adjacency
	{
		Fbox				bb_query;
		Fvector				bbc,bbd;
		bb_query.set		(contact_point,contact_point);
		bb_query.grow		(sz*2);
		bb_query.get_CD		(bbc,bbd);
		xrc.box_options		(CDB::OPT_FULL_TEST);
		xrc.box_query		(g_pGameLevel->ObjectSpace.GetStaticModel(),bbc,bbd);
		u32	triCount		= xrc.r_count	();
		if (0==triCount)	return;
		CDB::TRI* tris		= g_pGameLevel->ObjectSpace.GetStaticTris();
		sml_collector.clear	();
		sml_collector.add_face_packed_D	(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]],0);
		for (u32 t=0; t<triCount; t++)	{
			CDB::TRI*	T	= tris+xrc.r_begin()[t].id;
			if (T==pTri)	continue;
			sml_collector.add_face_packed_D		(pVerts[T->verts[0]],pVerts[T->verts[1]],pVerts[T->verts[2]],0);
		}
		sml_collector.calc_adjacency	(sml_adjacency);
	}

	// calc face normal
	Fvector	N;
	N.mknormal			(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
	sml_normal.set		(N);

	// build 3D ortho-frustum
	Fmatrix				mView,mRot;
	BuildMatrix			(mView,1/sz,contact_point);
	mRot.rotateZ		(::Random.randF(deg2rad(-20.f),deg2rad(20.f)));
	mView.mulA_43		(mRot);
	sml_clipper.CreateFromMatrix	(mView,FRUSTUM_P_LRTB);

	// create wallmark
	wallmark* W			= wm_allocate();
	RecurseTri			(0,mView,*W);

	// calc sphere
	if (W->verts.size()<3) { wm_destroy(W); return; }
	else {
		Fbox bb;	bb.invalidate();

		FVF::LIT* I=&*W->verts.begin	();
		FVF::LIT* E=&*W->verts.end		();
		for (; I!=E; I++)	bb.modify	(I->p);
		bb.getsphere					(W->bounds.P,W->bounds.R);
	}

	// search if similar wallmark exists
	wm_slot* slot	= FindSlot(hShader);
	if (slot){
		WMVecIt		it	= slot->items.begin	();
		WMVecIt		end	= slot->items.end	();
		for (; it!=end; it++)
		{
			wallmark* wm	= *it;
			if (wm->bounds.P.similar(W->bounds.P,0.02f)){ // replace
				wm_destroy	(wm);
				*it			= W;
				return;
			}
		}
	}else{
		slot		= AppendSlot(hShader);
	}

	// no similar - register _new_
	slot->items.push_back(W);
}
*/
