#include "stdafx.h"
#pragma hdrstop

#include "cl_RAPID.h"
#include "cl_defs.h"

#ifdef ENGINE_BUILD
#define B_BEGIN Device.Statistic.clBOX.Begin()
#define B_END Device.Statistic.clBOX.End()
#else
#define B_BEGIN
#define B_END
#endif

// add rapick info to list
namespace RAPID {
	void XRCollide::add_bboxcollide(int id)
	{
		bbox_collide bb_inf;
		bb_inf.id = id;
		BBoxContact.push_back(bb_inf);
	}
	
	void XRCollide::bbox_contact(const box *b, const bbox *bb)
	{
		// the vertices of the tris in b2 are in model1 C.S.  The vertices of
		// the other triangles are in model2 CS.  Use XR_mR, XR_mT, and XR_ms
		// to transform into model2 CS.
		Fvector p[3];
		for(int i=0; i<b->num_tris; i++){
			int id = b->tri_index[i];
			if(bbox_flags&BBOX_TRITEST) {
				XR_mR.sMxVpV(p[0], XR_ms, *(model1->tris[id].verts[0]), XR_mT);
				XR_mR.sMxVpV(p[1], XR_ms, *(model1->tris[id].verts[1]), XR_mT);
				XR_mR.sMxVpV(p[2], XR_ms, *(model1->tris[id].verts[2]), XR_mT);
				
				Fvector*	PTR[3] = { p+0, p+1, p+2 };
				if (Intersect_BBoxTri(*bb,PTR))
					add_bboxcollide(id);
			}else
				add_bboxcollide(id);
		}
	}
	
	void XRCollide::bbox_recursive(const box *b, const bbox *bb, const Fmatrix33& R, const Fvector& T, float s)
	{
		Fvector d;
		if ((bbox_flags&BBOX_ONLYFIRST) && (BBoxContact.size()>0)) return;

		// test top level
		int f1;
		d.mul(bb->d,s);
		f1 = obb_disjoint(R, T, b->d, d);
		if (f1 != 0)  return;	// stop processing this test, go to top of loop
		// contact between boxes
		if (b->leaf()){
			// it is a leaf pair - compare the polygons therein
			// tri_contact uses the model-to-model transforms stored in
			// RAPID_mR, RAPID_mT, RAPID_ms.
			// this will pass along any OUT_OF_MEMORY return codes which
			// may be generated.
			bbox_contact(b, bb);
			return;
		}
		
		Fvector U;
		Fmatrix33 cR;
		float cs;
		Fvector cT;
		
		// Currently, the transform from model 2 to model 1 space is
		// given by [B T s], where y = [B T s].x = s.B.x + T.
		// here we descend to children of b.  The transform from
		// a child of b1 to b1 is stored in [b1->N->pR,b1->N->pT],
		// but we will denote it [B1 T1 1] for short.  Notice that
		// children boxes always have same scaling as parent, so s=1
		// for such nested transforms.
		// Here, we compute [B1 T1 1]'[B T s] = [B1'B B1'(T-T1) s]
		// for each child, and store the transform into the collision
		// test queue.
		
		cR.MTxM(b->N->pR, R);
		U.sub(T, b->N->pT); b->N->pR.MTxV(cT, U);
		cs = s;
		
		bbox_recursive(b->N, bb, cR, cT, cs);
		
		cR.MTxM(b->P->pR, R);
		U.sub(T, b->P->pT); b->P->pR.MTxV(cT, U);
		cs = s;
		
		bbox_recursive(b->P, bb, cR, cT, cs);
	}
	
	void XRCollide::BBoxCollide( const Fmatrix& P_object, const Model *o,
		const Fmatrix& P_bbox,	 const Fvector& bbox_center, const Fvector& bbox_radius )
	{
		B_BEGIN;

		bbox bb;
		
		Fmatrix33   		R1;
		Fvector             T1;
		
		P_object.get_rapid    (R1);
		P_object.get_translate(T1);
		
		bb.set_bbox			(P_bbox,bbox_center,bbox_radius);
		
		//---------------------------------------------------
		box *b = o->b;
		
		Fmatrix33 tR1, R;
		Fvector tT1, U;
		Fvector T;
		
		// [R1,T1,s1] and [R2,T2,s2] are how the two triangle sets
		// (i.e. models) are positioned in world space.  [tR1,tT1,s1] and
		// [tR2,tT2,s2] are how the top level boxes are positioned in world
		// space
		
		tR1.MxM   (R1, b->pR);			// tR1 = R1 * b->pR;
		R1.MxVpV  (tT1, b->pT, T1);		// tT1 = s1 * R1 * b->pT + T1;
		
		// (R,T,s) is the placement of b2's top level box within
		// the coordinate system of b's top level box.
		
		R.MTxM(tR1, bb.pR);					// R = tR1.T()*tR2;
		U.sub(bb.pT, tT1);  tR1.MTxV(T, U);	// T = tR1.T()*(tT2-tT1)/s1;
		
		// To transform tri's from model1's CS to ray(world) CS use this:
		//    x2 = ms . mR . x1 + mT
		
		XR_mR.set(R1);
		XR_mT.set(T1);
		XR_ms = 1.f;
		
		// reset the report fields
		model1 = o;
		BBoxContact.clear();
		
		// make the call
		bbox_recursive(b, &bb, R, T, 1.f);

		B_END;
	}
};
