#include "Util.h"
#include "Common.h"

Frustum::Frustum() 
{
    for (int i=0; i<6; i++)
        camPlanes[i] = D3DXPLANE(0.f, 0.f, 0.f, 0.f);
}

//  build a frustum from a camera (projection, or viewProjection) matrix
Frustum::Frustum(const D3DXMATRIX* matrix)
{
    //  build a view frustum based on the current view & projection matrices...
    D3DXVECTOR4 column4( matrix->_14, matrix->_24, matrix->_34, matrix->_44 );
    D3DXVECTOR4 column1( matrix->_11, matrix->_21, matrix->_31, matrix->_41 );
    D3DXVECTOR4 column2( matrix->_12, matrix->_22, matrix->_32, matrix->_42 );
    D3DXVECTOR4 column3( matrix->_13, matrix->_23, matrix->_33, matrix->_43 );

    D3DXVECTOR4 planes[6];
    planes[0] = column4 - column1;  // left
    planes[1] = column4 + column1;  // right
    planes[2] = column4 - column2;  // bottom
    planes[3] = column4 + column2;  // top
    planes[4] = column4 - column3;  // near
    planes[5] = column4 + column3;  // far
    // ignore near & far plane
    
    int p;

    for (p=0; p<6; p++)  // normalize the planes
    {
        float dot = planes[p].x*planes[p].x + planes[p].y*planes[p].y + planes[p].z*planes[p].z;
        dot = 1.f / sqrtf(dot);
        planes[p] = planes[p] * dot;
    }

    for (p=0; p<6; p++)
        camPlanes[p] = D3DXPLANE( planes[p].x, planes[p].y, planes[p].z, planes[p].w );

    //  build a bit-field that will tell us the indices for the nearest and farthest vertices from each plane...
    for (int i=0; i<6; i++)
        nVertexLUT[i] = ((planes[i].x<0.f)?1:0) | ((planes[i].y<0.f)?2:0) | ((planes[i].z<0.f)?4:0);
}

//  test if a sphere is within the view frustum
bool Frustum::TestSphere(const BoundingSphere* sphere) const
{
    bool inside = true;
    float radius = sphere->radius;

    for (int i=0; (i<6) && inside; i++)
        inside &= ((D3DXPlaneDotCoord(&camPlanes[i], &sphere->centerVec) + radius) >= 0.f);

    return inside;
}

//  Tests if an AABB is inside/intersecting the view frustum
int Frustum::TestBox( const BoundingBox* box ) const
{
    bool intersect = false;

    for (int i=0; i<6; i++)
    {
        int nV = nVertexLUT[i];
        // pVertex is diagonally opposed to nVertex
        D3DXVECTOR3 nVertex( (nV&1)?box->minPt.x:box->maxPt.x, (nV&2)?box->minPt.y:box->maxPt.y, (nV&4)?box->minPt.z:box->maxPt.z );
        D3DXVECTOR3 pVertex( (nV&1)?box->maxPt.x:box->minPt.x, (nV&2)?box->maxPt.y:box->minPt.y, (nV&4)?box->maxPt.z:box->minPt.z );

        if ( D3DXPlaneDotCoord(&camPlanes[i], &nVertex) < 0.f )
            return 0;
        if ( D3DXPlaneDotCoord(&camPlanes[i], &pVertex) < 0.f )
            intersect = true;
    }

    return (intersect)?2 : 1;
}

//  this function tests if the projection of a bounding sphere along the light direction intersects
//  the view frustum 

bool SweptSpherePlaneIntersect(float& t0, float& t1, const D3DXPLANE* plane, const BoundingSphere* sphere, const D3DXVECTOR3* sweepDir)
{
    float b_dot_n = D3DXPlaneDotCoord(plane, &sphere->centerVec);
    float d_dot_n = D3DXPlaneDotNormal(plane, sweepDir);

    if (d_dot_n == 0.f)
    {
        if (b_dot_n <= sphere->radius)
        {
            //  effectively infinity
            t0 = 0.f;
            t1 = 1e32f;
            return true;
        }
        else
            return false;
    }
    else
    {
        float tmp0 = ( sphere->radius - b_dot_n) / d_dot_n;
        float tmp1 = (-sphere->radius - b_dot_n) / d_dot_n;
        t0 = min(tmp0, tmp1);
        t1 = max(tmp0, tmp1);
        return true;
    }
}

bool Frustum::TestSweptSphere(const BoundingSphere *sphere, const D3DXVECTOR3 *sweepDir) const
{
    //  algorithm -- get all 12 intersection points of the swept sphere with the view frustum
    //  for all points >0, displace sphere along the sweep driection.  if the displaced sphere
    //  is inside the frustum, return TRUE.  else, return FALSE
    float displacements[12];
    int cnt = 0;
    float a, b;
    bool inFrustum = false;

    for (int i=0; i<6; i++)
    {
        if (SweptSpherePlaneIntersect(a, b, &camPlanes[i], sphere, sweepDir))
        {
            if (a>=0)
                displacements[cnt++] = a;
            if (b>=0)
                displacements[cnt++] = b;
        }
    }

    for (int i=0; i<cnt; i++)
    {
        BoundingSphere displacedSphere(*sphere);
        displacedSphere.centerVec += (*sweepDir)*displacements[i];
        displacedSphere.radius *= 1.1f;
        inFrustum |= TestSphere(&displacedSphere);
    }
    return inFrustum;
}

//  build a quaternion representing the rotation from START to END
D3DXQUATERNION* QuaternionRotationAtoB(D3DXQUATERNION* quat, const D3DXVECTOR3* start, const D3DXVECTOR3* end)
{
    D3DXVECTOR3 xAxis;
    D3DXVECTOR3 yAxis;
    D3DXVECTOR3 zAxis;
    D3DXVECTOR3 vAxis;

    D3DXVec3Normalize( &vAxis, end );
    D3DXVec3Normalize( &xAxis, start );

    if ( fabsf(D3DXVec3Dot(&vAxis, &xAxis))<0.0001f )  // degenerate case...
    {
        quat->x = 0;
        quat->y = 0;
        quat->z = 0;
        quat->w = 1;
        return quat;
    }
    else
    {
        D3DXVec3Cross( &zAxis, start, end );  D3DXVec3Normalize(&zAxis, &zAxis);
        D3DXVec3Cross( &yAxis, &zAxis, &xAxis );

        float x_c = D3DXVec3Dot( &vAxis, &xAxis );
        float y_c = D3DXVec3Dot( &vAxis, &yAxis );
        float theta = atan2(y_c, x_c);
        float l = sinf(theta * 0.5f);
        D3DXQuaternionRotationAxis(quat, &zAxis, theta);
        return quat;
    }
}


bool RayIntersectsUnitBox(FLOAT* time, const D3DXVECTOR3* start, const D3DXVECTOR3* dir )
{
    D3DXPLANE sides[6] = { D3DXPLANE( 0,  1,  0, 1),
                           D3DXPLANE( 0, -1,  0, 1),
                           D3DXPLANE( 1,  0,  0, 1),
                           D3DXPLANE(-1,  0,  0, 1),
                           D3DXPLANE( 0,  0,  1, 0),
                           D3DXPLANE( 0,  0, -1, 1) };

    bool hit = false;
    
    *time = 3.3e33f;

    for (int i=0; i<6; i++)
    {
        float d_dot_n = D3DXPlaneDotNormal(&sides[i], dir);
        float b_dot_n = D3DXPlaneDotCoord(&sides[i], start);
        if (b_dot_n==0.f)
        {
            *time = 0.f;
            return true;
        }
        float t = -b_dot_n / d_dot_n;

        D3DXVECTOR3 intersectPt = t*(*dir) + (*start);
        switch (i)
        {
        case 0:
        case 1:
            {
                if ( fabsf(intersectPt.x)<1.0f && (intersectPt.z<1.0f && intersectPt.z>=0.f) && t>0 )
                {
                    hit = true;
                    *time = min(*time, t);
                }
                break;
            }
        case 2:
        case 3:
            {
                if ( fabsf(intersectPt.y)<1.0f && (intersectPt.z<1.0f && intersectPt.z>=0.f) && t>0 )
                {
                    hit = true;
                    *time = min(*time, t);
                }
                break;
            }
        case 4:
        case 5:
            {
                if ( fabsf(intersectPt.x)<1.0f && fabsf(intersectPt.y)<1.0f && t>0)
                {
                    hit = true;
                    *time = min(*time, t);
                }
                break;
            }
       }
    }

    if (!hit)
        *time = 0.01f;
    return hit;
}