#include "stdafx.h"
#pragma hdrstop

#include "cl_RAPID.h"
#include "cl_intersect.h"

namespace RAPID {
bool XRCollide::Intersect_RayTri(const ray& rp, Fvector** p, raypick_info& rp_inf){
	return TestRayTri(rp.C,rp.D,p,rp_inf.u,rp_inf.v,rp_inf.range,ray_flags&RAY_CULL);
}
bool XRCollide::Intersect_RayTri(const ray& rp, Fvector* p, raypick_info& rp_inf){
	return TestRayTri(rp.C,rp.D,p,rp_inf.u,rp_inf.v,rp_inf.range,ray_flags&RAY_CULL);
}

bool XRCollide::Intersect_BBoxTri(const bbox& bb,Fvector** p){
	return TestBBoxTri(bb,p,bbox_flags&BBOX_CULL);
}


//---------------------------------------------------------------------------
/*
static enum
{
    INTERSECTION,
    AXIS_N,
    AXIS_A0,
    AXIS_A1,
    AXIS_A2,
    AXIS_A0xE0,
    AXIS_A0xE1,
    AXIS_A0xE2,
    AXIS_A1xE0,
    AXIS_A1xE1,
    AXIS_A1xE2,
    AXIS_A2xE0,
    AXIS_A2xE1,
    AXIS_A2xE2
};

char* tr3bx3Axis[14] =
{
    "intersection",
    "N",
    "A0",
    "A1",
    "A2",
    "A0xE0",
    "A0xE1",
    "A0xE2",
    "A1xE0",
    "A1xE1",
    "A1xE2",
    "A2xE0",
    "A2xE1",
    "A2xE2"
};

//---------------------------------------------------------------------------
// macros for fast arithmetic
//---------------------------------------------------------------------------
#define COMBO(combo,p,t,q) \
{ \
    combo.m[0] = p.m[0]+t*q.m[0]; \
    combo.m[1] = p.m[1]+t*q.m[1]; \
    combo.m[2] = p.m[2]+t*q.m[2]; \
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// compare [-r,r] to [NdD+dt*NdW]
#define FIND0(NdD,R,t,N,W,tmax,type,side,axis) \
{ \
    float tmp, NdW; \
    if ( NdD > R ) \
    { \
        NdW = N.dotproduct(W); \
        if ( NdD+t*NdW > R ) \
            return axis; \
        tmp = t*(R-NdD)/NdW; \
        if ( tmp > tmax ) \
        { \
            tmax = tmp; \
            type = axis; \
            side = +1; \
        } \
    } \
    else if ( NdD < -R ) \
    { \
        NdW = N.dotproduct(W); \
        if ( NdD+t*NdW < -R ) \
            return axis; \
        tmp = -t*(R+NdD)/NdW; \
        if ( tmp > tmax ) \
        { \
            tmax = tmp; \
            type = axis; \
            side = -1; \
        } \
    } \
}
//---------------------------------------------------------------------------
// compare [-r,r] to [min{p,p+d0,p+d1},max{p,p+d0,p+d1}]+t*w where t >= 0
#define FIND1(p,t,w,d0,d1,r,tmax,type,extr,side,axis) \
{ \
    float tmp; \
    if ( (p) > (r) ) \
    { \
        float min; \
        if ( (d0) >= 0.0f ) \
        { \
            if ( (d1) >= 0.0f ) \
            { \
                if ( (p)+(t)*(w) > (r) ) \
                    return axis; \
                tmp = (t)*((r)-(p))/(w); \
                if ( tmp > tmax ) \
                { \
                    tmax = tmp; \
                    type = axis; \
                    extr = 0; \
                    side = +1; \
                } \
            } \
            else \
            { \
                min = (p)+(d1); \
                if ( min > (r) ) \
                { \
                    if ( min+(t)*(w) > (r) ) \
                        return axis; \
                    tmp = (t)*((r)-min)/(w); \
                    if ( tmp > tmax ) \
                    { \
                        tmax = tmp; \
                        type = axis; \
                        extr = 2; \
                        side = +1; \
                    } \
                } \
            } \
        } \
        else if ( (d1) <= (d0) ) \
        { \
            min = (p)+(d1); \
            if ( min > (r) ) \
            { \
                if ( min+(t)*(w) > (r) ) \
                    return axis; \
                tmp = (t)*((r)-min)/(w); \
                if ( tmp > tmax ) \
                { \
                    tmax = tmp; \
                    type = axis; \
                    extr = 2; \
                    side = +1; \
                } \
            } \
        } \
        else \
        { \
            min = (p)+(d0); \
            if ( min > (r) ) \
            { \
                if ( min+(t)*(w) > (r) ) \
                    return axis; \
                tmp = (t)*((r)-min)/(w); \
                if ( tmp > tmax ) \
                { \
                    tmax = tmp; \
                    type = axis; \
                    extr = 1; \
                    side = +1; \
                } \
            } \
        } \
    } \
    else if ( (p) < -(r) ) \
    { \
        float max; \
        if ( (d0) <= 0.0f ) \
        { \
            if ( (d1) <= 0.0f ) \
            { \
                if ( (p)+(t)*(w) < -(r) ) \
                    return axis; \
                tmp = -(t)*((r)+(p))/(w); \
                if ( tmp > tmax ) \
                { \
                    tmax = tmp; \
                    type = axis; \
                    extr = 0; \
                    side = -1; \
                } \
            } \
            else \
            { \
                max = (p)+(d1); \
                if ( max < -(r) ) \
                { \
                    if ( max+(t)*(w) < -(r) ) \
                        return axis; \
                    tmp = -(t)*((r)+max)/(w); \
                    if ( tmp > tmax ) \
                    { \
                        tmax = tmp; \
                        type = axis; \
                        extr = 2; \
                        side = -1; \
                    } \
                } \
            } \
        } \
        else if ( (d1) >= (d0) ) \
        { \
            max = (p)+(d1); \
            if ( max < -(r) ) \
            { \
                if ( max+(t)*(w) < -(r) ) \
                    return axis; \
                tmp = -(t)*((r)+max)/(w); \
                if ( tmp > tmax ) \
                { \
                    tmax = tmp; \
                    type = axis; \
                    extr = 2; \
                    side = -1; \
                } \
            } \
        } \
        else \
        { \
            max = (p)+(d0); \
            if ( max < -(r) ) \
            { \
                if ( max+(t)*(w) < -(r) ) \
                    return axis; \
                tmp = -(t)*((r)+max)/(w); \
                if ( tmp > tmax ) \
                { \
                    tmax = tmp; \
                    type = axis; \
                    extr = 1; \
                    side = -1; \
                } \
            } \
        } \
    } \
}
//---------------------------------------------------------------------------
// compare [-r,r] to [min{p,p+d},max{p,p+d}]+t*q where t >= 0
#define FIND2(p,t,w,d,r,tmax,type,extr,side,axis) \
{ \
    float tmp; \
    if ( (p) > (r) ) \
    { \
        if ( (d) >= 0.0f ) \
        { \
            if ( (p)+(t)*(w) > (r) ) \
                return axis; \
            tmp = (t)*((r)-(p))/(w); \
            if ( tmp > tmax ) \
            { \
                tmax = tmp; \
                type = axis; \
                extr = 0; \
                side = +1; \
            } \
        } \
        else \
        { \
            float min = (p)+(d); \
            if ( min > (r) ) \
            { \
                if ( min+(t)*(w) > (r) ) \
                    return axis; \
                tmp = (t)*((r)-min)/(w); \
                if ( tmp > tmax ) \
                { \
                    tmax = tmp; \
                    type = axis; \
                    extr = 1; \
                    side = +1; \
                } \
            } \
        } \
    } \
    else if ( (p) < -(r) ) \
    { \
        if ( (d) <= 0.0f ) \
        { \
            if ( (p)+(t)*(w) < -(r) ) \
                return axis; \
            tmp = -(t)*((r)+(p))/(w); \
            if ( tmp > tmax ) \
            { \
                tmax = tmp; \
                type = axis; \
                extr = 0; \
                side = -1; \
            } \
        } \
        else \
        { \
            float max = (p)+(d); \
            if ( max < -(r) ) \
            { \
                if ( max+(t)*(w) < -(r) ) \
                    return axis; \
                tmp = -(t)*((r)+max)/(w); \
                if ( tmp > tmax ) \
                { \
                    tmax = tmp; \
                    type = axis; \
                    extr = 1; \
                    side = -1; \
                } \
            } \
        } \
    } \
}
//---------------------------------------------------------------------------
#define GET_COEFF(coeff,sign0,sign1,side,cmat,ext) \
{ \
    if ( cmat > 0.0f ) \
    { \
        coeff = sign0##side*ext; \
    } \
    else if ( cmat < 0.0f ) \
    { \
        coeff = sign1##side*ext; \
    } \
    else \
    { \
        coeff = 0.0f; \
    } \
}
//---------------------------------------------------------------------------
	// = box.GetWorldCenter();
	// = box.GetWorldAxis(); 
#define GET_POINT(P,center,axis,T,V,x) \
{ \
    for (int k = 0; k < 3; k++) \
    { \
        P.m[k] = center.m[k] + T*V.m[k] + x[0]*axis.m[0][k] + \
            x[1]*axis.m[1][k] + x[2]*axis.m[2][k]; \
    } \
}
//---------------------------------------------------------------------------
DWORD XRCollide::Intersect_BBoxTri(const bbox& bb,const Fvector& p0,const Fvector& p1,const Fvector& p2, Fvector& P){
	float dt =0.1f;
	Fvector W;
	W.set(1,0,0);

	Fvector boxVel, triVel;
	boxVel.set(0,0,0);
	triVel.set(1,0,0);

    // convenience variables
	Fmatrix33 A;
	Fvector extA;

	A.transpose(bb.pR);
	extA.set(bb.d);

    // construct triangle normal, difference of center and vertex (18 ops)
    Fvector D, E[2], N;
	E[0].sub(p1,p0);
	E[1].sub(p2,p0);
	N.crossproduct(E[0],E[1]);

//	if ((bbox_flags&BBOX_CULL)&&(A.k.dotproduct(N)>0)) return false;

	D.sub(p0,bb.pT);

    // track maximum time of projection-intersection
    unsigned int type = INTERSECTION, extr = 0;
    int side = 0;
    float T = 0.f;

    // axis C+t*N
    float AN[3];
    AN[0] = A.i.dotproduct(N);
    AN[1] = A.j.dotproduct(N);
    AN[2] = A.k.dotproduct(N);
    float R = extA.x*fabsf(AN[0])+extA.y*fabsf(AN[1])+extA.z*fabsf(AN[2]);
    float NdD = N.dotproduct(D);
    FIND0(NdD,R,dt,N,W,T,type,side,AXIS_N);

    // axis C+t*A0
    float AD[3], AE[3][3];
    AD[0] = A.i.dotproduct(D);
    AE[0][0] = A.i.dotproduct(E[0]);
    AE[0][1] = A.i.dotproduct(E[1]);
    float A0dW = A.i.dotproduct(W);
    FIND1(AD[0],dt,A0dW,AE[0][0],AE[0][1],extA.x,T,type,extr,side,AXIS_A0);

    // axis C+t*A1
    AD[1] = A.j.dotproduct(D);
    AE[1][0] = A.j.dotproduct(E[0]);
    AE[1][1] = A.j.dotproduct(E[1]);
    float A1dW = A.j.dotproduct(W);
    FIND1(AD[1],dt,A1dW,AE[1][0],AE[1][1],extA.y,T,type,extr,side,AXIS_A1);

    // axis C+t*A2
    AD[2] = A.k.dotproduct(D);
    AE[2][0] = A.k.dotproduct(E[0]);
    AE[2][1] = A.k.dotproduct(E[1]);
    float A2dW = A.k.dotproduct(W);
    FIND1(AD[2],dt,A2dW,AE[2][0],AE[2][1],extA.z,T,type,extr,side,AXIS_A2);

    // axis C+t*A0xE0
    Fvector A0xE0;
    A0xE0.crossproduct(A.i,E[0]);
    float A0xE0dD = A0xE0.dotproduct(D);
    float A0xE0dW = A0xE0.dotproduct(W);
    R = extA.y*fabsf(AE[2][0])+extA.z*fabsf(AE[1][0]);
    FIND2(A0xE0dD,dt,A0xE0dW,AN[0],R,T,type,extr,side,AXIS_A0xE0);

    // axis C+t*A0xE1
    Fvector A0xE1;
    A0xE1.crossproduct(A.i,E[1]);
    float A0xE1dD = A0xE1.dotproduct(D);
    float A0xE1dW = A0xE1.dotproduct(W);
    R = extA.y*fabsf(AE[2][1])+extA.z*fabsf(AE[1][1]);
    FIND2(A0xE1dD,dt,A0xE1dW,-AN[0],R,T,type,extr,side,AXIS_A0xE1);

    // axis C+t*A0xE2
    AE[1][2] = AE[1][1]-AE[1][0];
    AE[2][2] = AE[2][1]-AE[2][0];
    float A0xE2dD = A0xE1dD-A0xE0dD;
    float A0xE2dW = A0xE1dW-A0xE0dW;
    R = extA.y*fabsf(AE[2][2])+extA.z*fabsf(AE[1][2]);
    FIND2(A0xE2dD,dt,A0xE2dW,-AN[0],R,T,type,extr,side,AXIS_A0xE2);

    // axis C+t*A1xE0
    Fvector A1xE0;
    A1xE0.crossproduct(A.j,E[0]);
    float A1xE0dD = A1xE0.dotproduct(D);
    float A1xE0dW = A1xE0.dotproduct(W);
    R = extA.x*fabsf(AE[2][0])+extA.z*fabsf(AE[0][0]);
    FIND2(A1xE0dD,dt,A1xE0dW,AN[1],R,T,type,extr,side,AXIS_A1xE0);

    // axis C+t*A1xE1
    Fvector A1xE1;
    A1xE1.crossproduct(A.j,E[1]);
    float A1xE1dD = A1xE1.dotproduct(D);
    float A1xE1dW = A1xE1.dotproduct(W);
    R = extA.x*fabsf(AE[2][1])+extA.z*fabsf(AE[0][1]);
    FIND2(A1xE1dD,dt,A1xE1dW,-AN[1],R,T,type,extr,side,AXIS_A1xE1);

    // axis C+t*A1xE2
    AE[0][2] = AE[0][1]-AE[0][0];
    float A1xE2dD = A1xE1dD-A1xE0dD;
    float A1xE2dW = A1xE1dW-A1xE0dW;
    R = extA.x*fabsf(AE[2][2])+extA.z*fabsf(AE[0][2]);
    FIND2(A1xE2dD,dt,A1xE2dW,-AN[1],R,T,type,extr,side,AXIS_A1xE2);

    // axis C+t*A2xE0
    Fvector A2xE0;
    A2xE0.crossproduct(A.k,E[0]);
    float A2xE0dD = A2xE0.dotproduct(D);
    float A2xE0dW = A2xE0.dotproduct(W);
    R = extA.x*fabsf(AE[1][0])+extA.y*fabsf(AE[0][0]);
    FIND2(A2xE0dD,dt,A2xE0dW,AN[2],R,T,type,extr,side,AXIS_A2xE0);

    // axis C+t*A2xE1
    Fvector A2xE1;
    A2xE1.crossproduct(A.k,E[1]);
    float A2xE1dD = A2xE1.dotproduct(D);
    float A2xE1dW = A2xE1.dotproduct(W);
    R = extA.x*fabsf(AE[1][1])+extA.y*fabsf(AE[0][1]);
    FIND2(A2xE1dD,dt,A2xE1dW,-AN[2],R,T,type,extr,side,AXIS_A2xE1);

    // axis C+t*A2xE2
    float A2xE2dD = A2xE1dD-A2xE0dD;
    float A2xE2dW = A2xE1dW-A2xE0dW;
    R = extA.x*fabsf(AE[1][2])+extA.y*fabsf(AE[0][2]);
    FIND2(A2xE2dD,dt,A2xE2dW,-AN[2],R,T,type,extr,side,AXIS_A2xE2);

    // determine the point of intersection
    int i;
    float x[3], numer, NDE[3], NAE[3][3];
    Fvector DxE[3], A0xE2, A1xE2, A2xE2;

    switch ( type )
    {
        case AXIS_N:
        {
            for (i = 0; i < 3; i++)
            {
                GET_COEFF(x[i],+,-,side,AN[i],extA.m[i]);
            }
            GET_POINT(P,bb.pT,A,T,boxVel,x);
            break;
        }
        case AXIS_A0:
        case AXIS_A1:
        case AXIS_A2:
        {
            if ( extr == 0 )
            {
                // y0 = 0, y1 = 0
                for (i = 0; i < 3; i++)
                    P.m[i] = p0.m[i]+T*triVel.m[i];
            }
            else if ( extr == 1 )
            {
                // y0 = 1, y1 = 0
                for (i = 0; i < 3; i++)
                    P.m[i] = p0.m[i]+T*triVel.m[i]+E[0].m[i];
            }
            else  // extr == 2
            {
                // y0 = 0, y1 = 1
                for (i = 0; i < 3; i++)
                    P.m[i] = p0.m[i]+T*triVel.m[i]+E[1].m[i];
            }
            break;
        }
        case AXIS_A0xE0:
        {
            GET_COEFF(x[1],-,+,side,AE[2][0],extA.y);
            GET_COEFF(x[2],+,-,side,AE[1][0],extA.z);
            COMBO(D,D,T,W);
            DxE[0].crossproduct(D,E[0]);
            NDE[0] = N.dotproduct(DxE[0]);
            NAE[0][0] = N.dotproduct(A0xE0);
            NAE[1][0] = N.dotproduct(A1xE0);
            NAE[2][0] = N.dotproduct(A2xE0);
            numer = NDE[0]-NAE[1][0]*x[1]-NAE[2][0]*x[2];
            if ( extr == 0 )
            {
                // y1 = 0
                x[0] = numer/NAE[0][0];
            }
            else  // extr == 1
            {
                // y1 = 1
                x[0] = (numer-N.dotproduct(N))/NAE[0][0];
            }
            GET_POINT(P,bb.pT,A,T,boxVel,x);
            break;
        }
        case AXIS_A0xE1:
        {
            GET_COEFF(x[1],-,+,side,AE[2][1],extA.y);
            GET_COEFF(x[2],+,-,side,AE[1][1],extA.z);
            COMBO(D,D,T,W);
            DxE[1].crossproduct(D,E[1]);
            NDE[1] = N.dotproduct(DxE[1]);
            NAE[0][1] = N.dotproduct(A0xE1);
            NAE[1][1] = N.dotproduct(A1xE1);
            NAE[2][1] = N.dotproduct(A2xE1);
            numer = NDE[1]-NAE[1][1]*x[1]-NAE[2][1]*x[2];
            if ( extr == 0 )
            {
                // y0 = 0
                x[0] = numer/NAE[0][1];
            }
            else  // extr == 1
            {
                // y0 = 1
                x[0] = (numer+N.dotproduct(N))/NAE[0][1];
            }
            GET_POINT(P,bb.pT,A,T,boxVel,x);
            break;
        }
        case AXIS_A0xE2:
        {
            E[2].sub(E[1],E[0]);
            A0xE2.crossproduct(A.i,E[2]);
            A1xE2.crossproduct(A.j,E[2]);
            A2xE2.crossproduct(A.k,E[2]);

            GET_COEFF(x[1],-,+,side,AE[2][2],extA.y);
            GET_COEFF(x[2],+,-,side,AE[1][2],extA.z);
            COMBO(D,D,T,W);
            DxE[2].crossproduct(D,E[2]);
            NDE[2] = N.dotproduct(DxE[2]);
            NAE[0][2] = N.dotproduct(A0xE2);
            NAE[1][2] = N.dotproduct(A1xE2);
            NAE[2][2] = N.dotproduct(A2xE2);
            numer = NDE[2]-NAE[1][2]*x[1]-NAE[2][2]*x[2];
            if ( extr == 0 )
            {
                // y0+y1 = 0
                x[0] = numer/NAE[0][2];
            }
            else  // extr == 1
            {
                // y0+y1 = 1
                x[0] = (numer+N.dotproduct(N))/NAE[0][2];
            }
            GET_POINT(P,bb.pT,A,T,boxVel,x);
            break;
        }
        case AXIS_A1xE0:
        {
            GET_COEFF(x[0],+,-,side,AE[2][0],extA.x);
            GET_COEFF(x[2],-,+,side,AE[0][0],extA.z);
            COMBO(D,D,T,W);
            DxE[0].crossproduct(D,E[0]);
            NDE[0] = N.dotproduct(DxE[0]);
            NAE[0][0] = N.dotproduct(A0xE0);
            NAE[1][0] = N.dotproduct(A1xE0);
            NAE[2][0] = N.dotproduct(A2xE0);
            numer = NDE[0]-NAE[0][0]*x[0]-NAE[2][0]*x[2];
            if ( extr == 0 )
            {
                // y1 = 0
                x[1] = numer/NAE[1][0];
            }
            else  // extr == 1
            {
                // y1 = 1
                x[1] = (numer-N.dotproduct(N))/NAE[1][0];
            }
            GET_POINT(P,bb.pT,A,T,boxVel,x);
            break;
        }
        case AXIS_A1xE1:
        {
            GET_COEFF(x[0],+,-,side,AE[2][1],extA.x);
            GET_COEFF(x[2],-,+,side,AE[0][1],extA.z);
            COMBO(D,D,T,W);
            DxE[1].crossproduct(D,E[1]);
            NDE[1] = N.dotproduct(DxE[1]);
            NAE[0][1] = N.dotproduct(A0xE1);
            NAE[1][1] = N.dotproduct(A1xE1);
            NAE[2][1] = N.dotproduct(A2xE1);
            numer = NDE[1]-NAE[0][1]*x[0]-NAE[2][1]*x[2];
            if ( extr == 0 )
            {
                // y0 = 0
                x[1] = numer/NAE[1][1];
            }
            else  // extr == 1
            {
                // y0 = 1
                x[1] = (numer+N.dotproduct(N))/NAE[1][1];
            }
            GET_POINT(P,bb.pT,A,T,boxVel,x);
            break;
        }
        case AXIS_A1xE2:
        {
            E[2].sub(E[1],E[0]);
            A0xE2.crossproduct(A.i,E[2]);
            A1xE2.crossproduct(A.j,E[2]);
            A2xE2.crossproduct(A.k,E[2]);

            GET_COEFF(x[0],+,-,side,AE[2][2],extA.x);
            GET_COEFF(x[2],-,+,side,AE[0][2],extA.z);
            COMBO(D,D,T,W);
            DxE[2].crossproduct(D,E[2]);
            NDE[2] = N.dotproduct(DxE[2]);
            NAE[0][2] = N.dotproduct(A0xE2);
            NAE[1][2] = N.dotproduct(A1xE2);
            NAE[2][2] = N.dotproduct(A2xE2);
            numer = NDE[2]-NAE[0][2]*x[0]-NAE[2][2]*x[2];
            if ( extr == 0 )
            {
                // y0+y1 = 0
                x[1] = numer/NAE[1][2];
            }
            else  // extr == 1
            {
                // y0+y1 = 1
                x[1] = (numer+N.dotproduct(N))/NAE[1][2];
            }
            GET_POINT(P,bb.pT,A,,T,boxVel,x);
            break;
        }
        case AXIS_A2xE0:
        {
            GET_COEFF(x[0],-,+,side,AE[1][0],extA.x);
            GET_COEFF(x[1],+,-,side,AE[0][0],extA.y);
            COMBO(D,D,T,W);
            DxE[0].crossproduct(D,E[0]);
            NDE[0] = N.dotproduct(DxE[0]);
            NAE[0][0] = N.dotproduct(A0xE0);
            NAE[1][0] = N.dotproduct(A1xE0);
            NAE[2][0] = N.dotproduct(A2xE0);
            numer = NDE[0]-NAE[0][0]*x[0]-NAE[1][0]*x[1];
            if ( extr == 0 )
            {
                // y1 = 0
                x[2] = numer/NAE[2][0];
            }
            else  // extr == 1
            {
                // y1 = 1
                x[2] = (numer-N.dotproduct(N))/NAE[2][0];
            }
            GET_POINT(P,bb.pT,A,,T,boxVel,x);
            break;
        }
        case AXIS_A2xE1:
        {
            GET_COEFF(x[0],-,+,side,AE[1][1],extA.x);
            GET_COEFF(x[1],+,-,side,AE[0][1],extA.y);
            COMBO(D,D,T,W);
            DxE[1].crossproduct(D,E[1]);
            NDE[1] = N.dotproduct(DxE[1]);
            NAE[0][1] = N.dotproduct(A0xE1);
            NAE[1][1] = N.dotproduct(A1xE1);
            NAE[2][1] = N.dotproduct(A2xE1);
            numer = NDE[1]-NAE[0][1]*x[0]-NAE[1][1]*x[1];
            if ( extr == 0 )
            {
                // y0 = 0
                x[2] = numer/NAE[2][1];
            }
            else  // extr == 1
            {
                // y0 = 1
                x[2] = (numer+N.dotproduct(N))/NAE[2][1];
            }
            GET_POINT(P,bb.pT,A,,T,boxVel,x);
            break;
        }
        case AXIS_A2xE2:
        {
            E[2].sub(E[1],E[0]);
            A0xE2.crossproduct(A.i,E[2]);
            A1xE2.crossproduct(A.j,E[2]);
            A2xE2.crossproduct(A.k,E[2]);

            GET_COEFF(x[0],-,+,side,AE[1][2],extA.x);
            GET_COEFF(x[1],+,-,side,AE[0][2],extA.y);
            COMBO(D,D,T,W);
            DxE[2].crossproduct(D,E[2]);
            NDE[2] = N.dotproduct(DxE[2]);
            NAE[0][2] = N.dotproduct(A0xE2);
            NAE[1][2] = N.dotproduct(A1xE2);
            NAE[2][2] = N.dotproduct(A2xE2);
            numer = NDE[2]-NAE[0][2]*x[0]-NAE[1][2]*x[1];
            if ( extr == 0 )
            {
                // y0+y1 = 0
                x[2] = numer/NAE[2][2];
            }
            else  // extr == 1
            {
                // y0+y1 = 1
                x[2] = (numer+N.dotproduct(N))/NAE[2][2];
            }
            GET_POINT(P,bb.pT,A,T,boxVel,x);
            break;
        }
    }

//    VERIFY( type != 0 );

    return 1;
}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
bool PickFace(  Fvector& start, Fvector& direction,
                Fvector& p0, Fvector& p1, Fvector& p2,
                float *distance );

bool PickFace(  Fvector& start, Fvector& direction,
                Fvector& p0, Fvector& p1, Fvector& p2,
                float *distance )
{
    Fvector facenormal;

    Fvector t1,t2;
    t1.sub(p1,p0);
    t2.sub(p2,p1);
    facenormal.crossproduct(t1,t2);
    facenormal.normalize();

	float clcheck = facenormal.dotproduct( direction );
	if( clcheck != 0.f ){

  		float d = - facenormal.dotproduct( p0 );
  		(*distance) = - ( (d + facenormal.dotproduct(start)) / clcheck );
		if( (*distance) <= 0 )
			return false;

  		Fvector hitpoint;
  		hitpoint.x = start.x + direction.x * (*distance);
  		hitpoint.y = start.y + direction.y * (*distance);
  		hitpoint.z = start.z + direction.z * (*distance);

  		Fvector np_from_p1;
  		np_from_p1.sub( hitpoint, p1 );
  		np_from_p1.normalize_safe();

  		Fvector tridir_10;
  		tridir_10.sub( p0, p1 );
  		tridir_10.normalize_safe();

  		Fvector tstn1;
  		tstn1.crossproduct( tridir_10, facenormal );

  		if( tstn1.dotproduct( np_from_p1 ) >= (-EPS) ){

  			Fvector np_from_p2;
  			np_from_p2.sub( hitpoint, p2 );
  			np_from_p2.normalize_safe();

  			Fvector tridir_21;
  			tridir_21.sub(p1,p2);
  			tridir_21.normalize_safe();

  			Fvector tstn2;
  			tstn2.crossproduct( tridir_21, facenormal );

  			if( tstn2.dotproduct( np_from_p2 ) >= (-EPS) ){

  				Fvector np_from_p0;
  				np_from_p0.sub( hitpoint, p0 );
  				np_from_p0.normalize_safe();

  				Fvector tridir_02;
  				tridir_02.sub(p2,p0);
  				tridir_02.normalize_safe();

  				Fvector tstn0;
  				tstn0.crossproduct( tridir_02, facenormal );

  				if( tstn0.dotproduct( np_from_p0 ) >= (-EPS) ){
  					return true;
  				}
  			}
  		}
	}
	return false;
};

int intersect_triangle_barycentric(
   float orig[3], float dir[3],
   float vert0[3], float vert1[3], float vert2[3],
   float planeq[4], int i1, int i2,
   float *t, float *alpha, float *beta)
{
   float	dot, dot2;
   float	point[2];
   float	u0, v0, u1, v1, u2, v2;

   // is ray parallel to plane?
   dot = dir[0] * planeq[0] + dir[1] * planeq[1] + dir[2] * planeq[2];
   if (dot < EPSILON && dot > -EPSILON)		// or use culling check
      return 0;

   // find distance to plane and intersection point
   dot2 = orig[0] * planeq[0] +
      orig[1] * planeq[1] + orig[2] * planeq[2];
   *t = -(planeq[3] + dot2 ) / dot;
   point[0] = orig[i1] + dir[i1] * *t;
   point[1] = orig[i2] + dir[i2] * *t;

   // begin barycentric intersection algorithm
   u0 = point[0] - vert0[i1];
   v0 = point[1] - vert0[i2];
   u1 = vert1[i1] - vert0[i1];
   u2 = vert2[i1] - vert0[i1];
   v1 = vert1[i2] - vert0[i2];
   v2 = vert2[i2] - vert0[i2];

   // calculate and compare barycentric coordinates
   if (u1 == 0) {		// uncommon case
      *beta = u0 / u2;
      if (*beta < 0 || *beta > 1)
	       return 0;
      *alpha = (v0 - *beta * v2) / v1;
   }
   else {			// common case, used for this analysis
      *beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
      if (*beta < 0 || *beta > 1)
	       return 0;
      *alpha = (u0 - *beta * u2) / u1;
   }

   if (*alpha < 0 || (*alpha + *beta) > 1.0)
      return 0;

   return 1;
}
*/




};
