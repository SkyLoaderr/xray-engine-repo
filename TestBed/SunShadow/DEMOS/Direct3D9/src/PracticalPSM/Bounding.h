#ifndef __BOUNDING_H_included_
#define __BOUNDING_H_included_

#include <d3dx9.h>
#include <d3dx9math.h>
#include <math.h>
#include <vector>
#include <shared/NVBScene9.h>

struct Frustum;

struct BoundingBox
{
    D3DXVECTOR3 minPt;
    D3DXVECTOR3 maxPt;

    BoundingBox(): minPt(), maxPt() { }
    BoundingBox( const BoundingBox& other ): minPt(other.minPt), maxPt(other.maxPt) { }
    
    explicit BoundingBox( const std::vector<BoundingBox>* boxes ): minPt(3.3e33f, 3.3e33f, 3.3e33f), maxPt(-3.3e33f, -3.3e33f, -3.3e33f) 
    {
        for (unsigned int i=0; i<boxes->size(); i++) 
        {
            Merge( &(*boxes)[i].maxPt );
            Merge( &(*boxes)[i].minPt );
        }
    }
    void Centroid( D3DXVECTOR3* vec) const { *vec = 0.5f*(minPt+maxPt); }
    void Merge( const D3DXVECTOR3* vec );

	//. OLES:
	D3DXVECTOR3	point(int id)	const 
	{		
		switch (id)
		{
		case 0:		return D3DXVECTOR3(minPt.x,minPt.y,minPt.z);
		case 1:		return D3DXVECTOR3(minPt.x,minPt.y,maxPt.z);
		case 2:		return D3DXVECTOR3(minPt.x,maxPt.y,maxPt.z);
		case 3:		return D3DXVECTOR3(minPt.x,maxPt.y,minPt.z);
		case 4:		return D3DXVECTOR3(maxPt.x,maxPt.y,maxPt.z);
		case 5:		return D3DXVECTOR3(maxPt.x,maxPt.y,minPt.z);
		case 6:		return D3DXVECTOR3(maxPt.x,minPt.y,maxPt.z);
		case 7:		return D3DXVECTOR3(maxPt.x,minPt.y,minPt.z);
		default:	__assume(0);
		}
	}
};

struct BoundingSphere
{
    D3DXVECTOR3 centerVec;
    float       radius;
    
    BoundingSphere(): centerVec(), radius(0.f) { }
    BoundingSphere( const BoundingSphere& other ): centerVec(other.centerVec), radius(other.radius) { }
    explicit BoundingSphere( const BoundingBox* box )
    {
        D3DXVECTOR3 radiusVec;
        centerVec = 0.5f * (box->maxPt + box->minPt);
        radiusVec = box->maxPt - centerVec;
        float len = D3DXVec3Length(&radiusVec);
        radius = len;
    }
};

struct BoundingCone
{
    D3DXVECTOR3 direction;
    D3DXVECTOR3 apex;
    float       fovy;
    float       fovx;
    float       fNear;
    float       fFar;
    D3DXMATRIX  m_LookAt;

    BoundingCone(): direction(0.f, 0.f, 1.f), apex(0.f, 0.f, 0.f), fovx(0.f), fovy(0.f), fNear(0.001f), fFar(1.f) { }
    BoundingCone(const std::vector<BoundingBox>* boxes, const D3DXMATRIX* projection, const D3DXVECTOR3* _apex);
    BoundingCone(const std::vector<BoundingBox>* boxes, const D3DXMATRIX* projection, const D3DXVECTOR3* _apex, const D3DXVECTOR3* _direction);
};

void GetTerrainBoundingBox( std::vector<BoundingBox>* shadowReceivers, const D3DXMATRIX* modelView, const Frustum* sceneFrustum );
void GetSceneBoundingBox( BoundingBox* box, std::vector<BoundingBox>* modelBoxen, const NVBScene* scene );
void GetModelBoundingBox( BoundingBox* box, const NVBScene::Mesh* mesh );
void XFormBoundingBox( BoundingBox* result, const BoundingBox* src, const D3DXMATRIX* matrix );


#endif