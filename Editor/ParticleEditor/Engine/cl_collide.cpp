#include "stdafx.h"
#pragma hdrstop

#include "cl_RAPID.h"
#include "cl_defs.h"

namespace RAPID {
	void XRCollide::add_collision(int id1, int id2)
	{
		collision_pair cp;
		cp.id1 = id1;
		cp.id2 = id2;
		ModelContact.push_back(cp);
	}
	
	void XRCollide::tri_contact(const box *b1, const box *b2)
	{
		// the vertices of the tris in b2 are in model1 C.S.  The vertices of
		// the other triangles are in model2 CS.  Use RAPID_mR, RAPID_mT, and
		// RAPID_ms to transform into model2 CS.
		
		Fvector p[3];
		int i,j;
		
		for(i=0; i<b1->num_tris; i++){
			int in1 = b1->tri_index[i];
			
			XR_mR.sMxVpV(p[0], XR_ms, *model1->tris[in1].verts[0], XR_mT);
			XR_mR.sMxVpV(p[1], XR_ms, *model1->tris[in1].verts[1], XR_mT);
			XR_mR.sMxVpV(p[2], XR_ms, *model1->tris[in1].verts[2], XR_mT);
			
			for(j=0; j<b2->num_tris; j++){
				int in2 = b2->tri_index[j];
				
				if (tri_contact(p, model2->tris[in2].verts))
					add_collision(in1, in2);
			}
		}
	}
	
	void XRCollide::collide_recursive(const box *b1, const box *b2, const Fmatrix33& R, const Fvector& T, float s)
	{
		Fvector d; // temp storage for scaled dimensions of box b2.
		
		if ((collide_flags & CL_ONLYFIRST) && (ModelContact.size() > 0)) return;
		
		// test top level
		int f1;
		d.mul(b2->d,s);
		f1 = obb_disjoint(R, T, b1->d, d);
		
		if (f1 != 0)  return;  // stop processing this test, go to top of loop
		
		// contact between boxes
		if (b1->leaf() && b2->leaf()){
			// it is a leaf pair - compare the polygons therein
			// tri_contact uses the model-to-model transforms stored in
			// RAPID_mR, RAPID_mT, RAPID_ms.
			// this will pass along any OUT_OF_MEMORY return codes which
			// may be generated.
			tri_contact(b1, b2);
			return;
		}
		
		Fvector		U;
		
		Fmatrix33	cR;
		Fvector		cT;
		float		cs;
		
		// Currently, the transform from model 2 to model 1 space is
		// given by [B T s], where y = [B T s].x = s.B.x + T.
		
		if (b2->leaf() || (!b1->leaf() && (b1->size() > b2->size()))){
			// here we descend to children of b1.  The transform from
			// a child of b1 to b1 is stored in [b1->N->pR,b1->N->pT],
			// but we will denote it [B1 T1 1] for short.  Notice that
			// children boxes always have same scaling as parent, so s=1
			// for such nested transforms.
			
			// Here, we compute [B1 T1 1]'[B T s] = [B1'B B1'(T-T1) s]
			// for each child, and store the transform into the collision
			// test queue.
			
			cR.MTxM(b1->N->pR, R);
			U.sub(T, b1->N->pT); b1->N->pR.MTxV(cT, U);
			cs = s;
			
			collide_recursive(b1->N, b2, cR, cT, cs);
			
			cR.MTxM(b1->P->pR, R);
			U.sub(T, b1->P->pT); b1->P->pR.MTxV(cT, U);
			cs = s;
			
			collide_recursive(b1->P, b2, cR, cT, cs);
		}else{
			// here we descend to the children of b2.  See comments for
			// other 'if' clause for explanation.
			
			cR.MxM(R, b2->N->pR); R.sMxVpV(cT, s, b2->N->pT, T); 
			cs = s;
			
			collide_recursive(b1, b2->N, cR, cT, cs);
			
			cR.MxM(R, b2->P->pR); R.sMxVpV(cT, s, b2->P->pT, T);
			cs = s;
			
			collide_recursive(b1, b2->P, cR, cT, cs);
		}
	}
	
	void XRCollide::Collide( const Fmatrix33& R1, const Fvector& T1, float s1, const Model *o1,
		const Fmatrix33& R2, const Fvector& T2, float s2, const Model *o2)
	{
		box *b1 = o1->b;
		box *b2 = o2->b;
		
		Fmatrix33 tR1, tR2, R;
		Fvector tT1, tT2, T, U;
		float s;
		
		// [R1,T1,s1] and [R2,T2,s2] are how the two triangle sets
		// (i.e. models) are positioned in world space.  [tR1,tT1,s1] and
		// [tR2,tT2,s2] are how the top level boxes are positioned in world
		// space
		
		tR1.MxM   (R1, b1->pR);               // tR1 = R1 * b1->pR;
		R1.sMxVpV (tT1,s1, b1->pT, T1);       // tT1 = s1 * R1 * b1->pT + T1;
		tR2.MxM   (R2, b2->pR);               // tR2 = R2 * b2->pR;
		R2.sMxVpV (tT2,s2, b2->pT, T2);       // tT2 = s2 * R2 * b2->pT + T2;
		
		// (R,T,s) is the placement of b2's top level box within
		// the coordinate system of b1's top level box.
		
		R.MTxM(tR1, tR2);                            // R = tR1.T()*tR2;
		U.sub(tT2, tT1);  tR1.sMTxV(T, 1.0f/s1, U);  // T = tR1.T()*(tT2-tT1)/s1;
		
		s = s2/s1;
		
		// To transform tri's from model1's CS to model2's CS use this:
		//    x2 = ms . mR . x1 + mT
		
		XR_mR.MTxM(R2, R1);
		U.sub(T1, T2);  R2.sMTxV(XR_mT, 1.0f/s2, U);
		XR_ms = s1/s2;
		
		// reset the report fields
		ModelContact.clear();
		model1 = o1;
		model2 = o2;
		
		// make the call
		collide_recursive(b1, b2, R, T, s);
	}
	
	
	
};
