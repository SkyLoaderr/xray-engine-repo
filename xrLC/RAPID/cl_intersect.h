//---------------------------------------------------------------------------
#ifndef intersectH
#define intersectH
//---------------------------------------------------------------------------
/*
bool FrustumPickFace(
	ICullPlane *planes, // four planes !
	Fvector& p0, Fvector& p1, Fvector& p2 );
*/

bool PickFace(  Fvector& start, Fvector& direction,
                Fvector& p0, Fvector& p1, Fvector& p2,
                float *distance );

bool __forceinline PickFace2( Fvector& orig, Fvector& dir,
                Fvector& v0, Fvector& v1, Fvector& v2,
                float *t, float *u, float *v, bool bCULL=true);

bool __forceinline PickFace2( Fvector& orig, Fvector& dir,
                Fvector& v0, Fvector& v1, Fvector& v2,
                float *t, bool bCULL=true)
{
    float u, v;
    return PickFace2(orig, dir, v0, v1, v2, t, &u, &v, bCULL);
}

bool __forceinline PickFace2(Fvector& orig, Fvector& dir,
               Fvector& v0, Fvector& v1, Fvector& v2,
               float *t, float *u, float *v, bool bCULL)
{
    Fvector edge1, edge2, tvec, pvec, qvec;
    float det,inv_det;
    // find vectors for two edges sharing vert0
    edge1.sub(v1, v0);
    edge2.sub(v2, v0);
    // begin calculating determinant - also used to calculate U parameter
    pvec.crossproduct(dir, edge2);
    // if determinant is near zero, ray lies in plane of triangle
    det = edge1.dotproduct(pvec);
    if (bCULL){           // define TEST_CULL if culling is desired
        if (det < EPS)  return false;
        tvec.sub(orig, v0);           // calculate distance from vert0 to ray origin
        *u = tvec.dotproduct(pvec);      // calculate U parameter and test bounds
        if (*u < 0.0 || *u > det) return false;
        qvec.crossproduct(tvec, edge1);  // prepare to test V parameter
        *v = dir.dotproduct(qvec);  // calculate V parameter and test bounds
        if (*v < 0.0 || *u + *v > det) return false;
        *t = edge2.dotproduct(qvec);     // calculate t, scale parameters, ray intersects triangle
        inv_det = 1.0 / det;
        *t *= inv_det;
        *u *= inv_det;
        *v *= inv_det;
    }else{                    // the non-culling branch
        if (det > -EPS && det < EPS) return false;
        inv_det = 1.0 / det;
        tvec.sub(orig, v0);           // calculate distance from vert0 to ray origin
        *u = tvec.dotproduct(pvec)*inv_det;// calculate U parameter and test bounds
        if (*u < 0.0 || *u > 1.0)    return false;
        qvec.crossproduct(tvec, edge1);  // prepare to test V parameter
        *v = dir.dotproduct(qvec)*inv_det; // calculate V parameter and test bounds
        if (*v < 0.0 || *u + *v > 1.0) return false;
        *t = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects triangle
    }
    return true;
}
//---------------------------------------------------------------------------
#endif
