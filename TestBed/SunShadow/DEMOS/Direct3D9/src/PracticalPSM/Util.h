#ifndef __UTIL_H__included_
#define __UTIL_H__included_

// some utility math

#include <d3dx9.h>
#include <d3dx9math.h>
#include <math.h>
#include <vector>
#include <shared/NVBScene9.h>
#include "Bounding.h"

struct ObjectInstance
{
    NVBScene* scene;
    BoundingBox* aabb;                  // this forms a bounding box tree...
    std::vector<BoundingBox>* aaBoxen;  // ----
    D3DXVECTOR3 translation;
};

struct Frustum
{
    Frustum();
    Frustum( const D3DXMATRIX* matrix );

    bool TestSphere     ( const BoundingSphere* sphere ) const;
    int  TestBox        ( const BoundingBox* box ) const;
    bool TestSweptSphere( const BoundingSphere* sphere, const D3DXVECTOR3* sweepDir ) const;
    
    D3DXPLANE camPlanes[6];
    int nVertexLUT[6];
};

D3DXQUATERNION* QuaternionRotationAtoB(D3DXQUATERNION* quat, const D3DXVECTOR3* start, const D3DXVECTOR3* end);

bool RayIntersectsUnitBox( FLOAT* time, const D3DXVECTOR3* start, const D3DXVECTOR3* dir );

#endif