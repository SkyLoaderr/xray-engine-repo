#include "stdafx.h"
#pragma hdrstop

#include "cl_RAPID.h"
#include "cl_obb.h"
#include "cl_intersect.h"

// add rapick info to list
int XRCollide::add_raypick(raypick_info& rp_inf)
{
	RayContact.Add(rp_inf);
	if (min_raypick_id>=0){
		VERIFY(min_raypick_id<RayContact.count);
		if (RayContact[min_raypick_id].range>rp_inf.range) min_raypick_id = RayContact.count-1;
	}else{
		min_raypick_id = RayContact.count-1;
	}
	return RAPID_OK;
}

int XRCollide::ray_contact(box *b1, ray *rp)
{
	// the vertices of the tris in b2 are in model1 C.S.  The vertices of
	// the other triangles are in model2 CS.  Use RAPID_mR, RAPID_mT, and
	// RAPID_ms to transform into model2 CS.
	for(int i=0; i<b1->num_tris; i++){
        raypick_info rp_inf;
		rp_inf.id = b1->tri_index[i];
        rp_inf.range = 0;
		rp_inf.p1.sMxVpV(XR_ms, XR_mR, model1->tris[rp_inf.id].p1, XR_mT);
		rp_inf.p2.sMxVpV(XR_ms, XR_mR, model1->tris[rp_inf.id].p2, XR_mT);
		rp_inf.p3.sMxVpV(XR_ms, XR_mR, model1->tris[rp_inf.id].p3, XR_mT);
		num_tri_tests++;
//		if (PickFace(&range,c,d,N,i1,i2,i3)) add_raypick(in1, range);
		if (PickFace2(rp->C,rp->D,rp_inf.p1,rp_inf.p2,rp_inf.p3,&rp_inf.range,&rp_inf.u,&rp_inf.v,true)){
            add_raypick(rp_inf);
        }
	}
	return RAPID_OK;
}

int XRCollide::raypick_recursive(box *b1, ray *rp, Fmatrix33& R, Fvector& T, float s)
{
    Fvector d;
    int rc;      // return codes
    if (bFirstContact && (RayContact.count > 0)) return RAPID_OK;
    // test top level
    num_box_tests++;
    int f1;
    d.mul(rp->d,s);
    f1 = obb_disjoint(R, T, b1->d, d);
    if (f1 != 0)  return RAPID_OK;  // stop processing this test, go to top of loop
    // contact between boxes
    if (b1->leaf()){
	  // it is a leaf pair - compare the polygons therein
          // tri_contact uses the model-to-model transforms stored in
	  // RAPID_mR, RAPID_mT, RAPID_ms.
	  // this will pass along any OUT_OF_MEMORY return codes which
	  // may be generated.
	    return ray_contact(b1, rp);
	}

    Fvector U;
    Fmatrix33 cR;
    float cs;
    Fvector cT;

    // Currently, the transform from model 2 to model 1 space is
    // given by [B T s], where y = [B T s].x = s.B.x + T.
    // here we descend to children of b1.  The transform from
    // a child of b1 to b1 is stored in [b1->N->pR,b1->N->pT],
    // but we will denote it [B1 T1 1] for short.  Notice that
    // children boxes always have same scaling as parent, so s=1
    // for such nested transforms.
    // Here, we compute [B1 T1 1]'[B T s] = [B1'B B1'(T-T1) s]
    // for each child, and store the transform into the collision
    // test queue.

    cR.MTxM(b1->N->pR, R);
    U.sub(T, b1->N->pT); cT.MTxV(b1->N->pR, U);
    cs = s;

    if ((rc = raypick_recursive(b1->N, rp, cR, cT, cs)) != RAPID_OK) return rc;

    cR.MTxM(b1->P->pR, R);
    U.sub(T, b1->P->pT); cT.MTxV(b1->P->pR, U);
    cs = s;

    if ((rc = raypick_recursive(b1->P, rp, cR, cT, cs)) != RAPID_OK) return rc;

    return RAPID_OK;
}

int XRCollide::RayPick( Fmatrix& parent, float s1, RAPID_model *o1,
				        Fvector& C, Fvector& D, float max_range, bool flag)
{
	if (o1->build_state != RAPID_BUILD_STATE_PROCESSED)
		return RAPID_ERR_UNPROCESSED_MODEL;

	Fmatrix33   		R1, R2;
    Fvector             T1, T2;
	float s2 = 1;
	ray r;

    parent.get_rapid    (R1);
    parent.get_translate(T1);

	R2.identity();	// все в мировых координатах (ед. матрица)
    T2.set(0,0,0);

	r.set_ray(C,D,max_range);
//---------------------------------------------------
	box *b1 = o1->b;
	ray *rp = &r;

    bFirstContact = flag;

	Fmatrix33 tR1, tR2, R;
	Fvector tT1, tT2, U;
	float s;
    Fvector T;

	// [R1,T1,s1] and [R2,T2,s2] are how the two triangle sets
	// (i.e. models) are positioned in world space.  [tR1,tT1,s1] and
	// [tR2,tT2,s2] are how the top level boxes are positioned in world
	// space

    tR1.MxM   (R1, b1->pR);               // tR1 = R1 * b1->pR;
    tT1.sMxVpV(s1, R1, b1->pT, T1);       // tT1 = s1 * R1 * b1->pT + T1;
    tR2.MxM   (R2, rp->pR);               // tR2 = R2 * b2->pR;
    tT2.sMxVpV(s2, R2, rp->pT, T2);       // tT2 = s2 * R2 * b2->pT + T2;

	// (R,T,s) is the placement of b2's top level box within
	// the coordinate system of b1's top level box.

	R.MTxM(tR1, tR2);							// R = tR1.T()*tR2;
	U.sub(tT2, tT1);  T.sMTxV(1.0f/s1, tR1, U);	// T = tR1.T()*(tT2-tT1)/s1;

	s = s2/s1;

	// To transform tri's from model1's CS to ray(world) CS use this:
	//    x2 = ms . mR . x1 + mT

    XR_mR.MTxM(R2, R1);
    U.sub(T1, T2);  XR_mT.sMTxV(1.0f/s2, R2, U);
    XR_ms = s1/s2;

	// reset the report fields
	num_box_tests = 0;
	num_tri_tests = 0;
    min_raypick_id = -1;
	model1 = o1;
	RayContact.Clear();

	// make the call
	return raypick_recursive(b1, rp, R, T, s);
}



