#include "stdafx.h"
#pragma hdrstop

#include "cl_RAPID.h"
#include "cl_obb.h"

int XRCollide::add_collision(int id1, int id2)
{
    collision_pair cp;
    cp.id1 = id1;
    cp.id2 = id2;
    ModelContact.Add(cp);
    return RAPID_OK;
}

int XRCollide::tri_contact(box *b1, box *b2)
{
  // the vertices of the tris in b2 are in model1 C.S.  The vertices of
  // the other triangles are in model2 CS.  Use RAPID_mR, RAPID_mT, and
  // RAPID_ms to transform into model2 CS.

  Fvector i1;
  Fvector i2;
  Fvector i3;
  int rc;  // return code
  int i,j;

  for(i=0; i<b1->num_tris; i++){
      int in1 = b1->tri_index[i];

      i1.sMxVpV(XR_ms, XR_mR, model1->tris[in1].p1, XR_mT);
      i2.sMxVpV(XR_ms, XR_mR, model1->tris[in1].p2, XR_mT);
      i3.sMxVpV(XR_ms, XR_mR, model1->tris[in1].p3, XR_mT);

      for(j=0; j<b2->num_tris; j++){
		  int in2 = b2->tri_index[j];

		  num_tri_tests++;

		  int f = tri_contact(i1, i2, i3,
			  model2->tris[in2].p1,
			  model2->tris[in2].p2,
			  model2->tris[in2].p3);

		  if (f){
			  // add_collision may be unable to allocate enough memory,
			  // so be prepared to pass along an OUT_OF_MEMORY return code.
//	  	      if ((rc = add_collision(RAPID_test_model1->tris[in1].id,
//	  				      RAPID_test_model2->tris[in2].id)) != RAPID_OK)
			  if ((rc = add_collision(in1, in2)) != RAPID_OK)
				  return rc;
		  }
	  }
  }

  return RAPID_OK;
}

int XRCollide::collide_recursive(box *b1, box *b2, Fmatrix33& R, Fvector& T, float s)
{
    Fvector d; // temp storage for scaled dimensions of box b2.
    int rc;    // return codes

    if (bFirstContact && (ModelContact.count > 0)) return RAPID_OK;

    // test top level

    num_box_tests++;

    int f1;
    d.mul(b2->d,s);
    f1 = obb_disjoint(R, T, b1->d, d);

    if (f1 != 0)  return RAPID_OK;  // stop processing this test, go to top of loop

    // contact between boxes
    if (b1->leaf() && b2->leaf()){
        // it is a leaf pair - compare the polygons therein
        // tri_contact uses the model-to-model transforms stored in
        // RAPID_mR, RAPID_mT, RAPID_ms.
        // this will pass along any OUT_OF_MEMORY return codes which
        // may be generated.
        return tri_contact(b1, b2);
    }

    Fvector U;

    Fmatrix33 cR;
    Fvector cT;
    float cs;

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
        U.sub(T, b1->N->pT); cT.MTxV(b1->N->pR, U);
        cs = s;

        if ((rc = collide_recursive(b1->N, b2, cR, cT, cs)) != RAPID_OK)
            return rc;

        cR.MTxM(b1->P->pR, R);
        U.sub(T, b1->P->pT); cT.MTxV(b1->P->pR, U);
        cs = s;

        if ((rc = collide_recursive(b1->P, b2, cR, cT, cs)) != RAPID_OK)
            return rc;

        return RAPID_OK;
    }else{
        // here we descend to the children of b2.  See comments for
        // other 'if' clause for explanation.

        cR.MxM(R, b2->N->pR); cT.sMxVpV(s, R, b2->N->pT, T);
        cs = s;

        if ((rc = collide_recursive(b1, b2->N, cR, cT, cs)) != RAPID_OK)
        return rc;

        cR.MxM(R, b2->P->pR); cT.sMxVpV(s, R, b2->P->pT, T);
        cs = s;

        if ((rc = collide_recursive(b1, b2->P, cR, cT, cs)) != RAPID_OK)
        return rc;
    }

    return RAPID_OK;
}

int XRCollide::Collide( Fmatrix33& R1, Fvector& T1, float s1, RAPID_model *o1,
	                    Fmatrix33& R2, Fvector& T2, float s2, RAPID_model *o2,
	                    bool flag)
{
    if (o1->build_state != RAPID_BUILD_STATE_PROCESSED)
        return RAPID_ERR_UNPROCESSED_MODEL;

    if (o2->build_state != RAPID_BUILD_STATE_PROCESSED)
        return RAPID_ERR_UNPROCESSED_MODEL;

    box *b1 = o1->b;
    box *b2 = o2->b;

    bFirstContact = flag;

    Fmatrix33 tR1, tR2, R;
    Fvector tT1, tT2, T, U;
    float s;

  // [R1,T1,s1] and [R2,T2,s2] are how the two triangle sets
  // (i.e. models) are positioned in world space.  [tR1,tT1,s1] and
  // [tR2,tT2,s2] are how the top level boxes are positioned in world
  // space

    tR1.MxM   (R1, b1->pR);               // tR1 = R1 * b1->pR;
    tT1.sMxVpV(s1, R1, b1->pT, T1);       // tT1 = s1 * R1 * b1->pT + T1;
    tR2.MxM   (R2, b2->pR);               // tR2 = R2 * b2->pR;
    tT2.sMxVpV(s2, R2, b2->pT, T2);       // tT2 = s2 * R2 * b2->pT + T2;

  // (R,T,s) is the placement of b2's top level box within
  // the coordinate system of b1's top level box.

    R.MTxM(tR1, tR2);                            // R = tR1.T()*tR2;
    U.sub(tT2, tT1);  T.sMTxV(1.0f/s1, tR1, U);  // T = tR1.T()*(tT2-tT1)/s1;

    s = s2/s1;

  // To transform tri's from model1's CS to model2's CS use this:
  //    x2 = ms . mR . x1 + mT

    XR_mR.MTxM(R2, R1);
    U.sub(T1, T2);  XR_mT.sMTxV(1.0f/s2, R2, U);
    XR_ms = s1/s2;

    // reset the report fields
    num_box_tests = 0;
    num_tri_tests = 0;
    ModelContact.Clear();
    model1 = o1;
    model2 = o2;

  // make the call
    return collide_recursive(b1, b2, R, T, s);
}


