#include "Bounding.h"
#include "Util.h"
#include "Common.h"

////////////////////////////////////////////////////////////////

void BoundingBox::Merge(const D3DXVECTOR3* vec)
{
    minPt.x = min(minPt.x, vec->x);
    minPt.y = min(minPt.y, vec->y);
    minPt.z = min(minPt.z, vec->z);
    maxPt.x = max(maxPt.x, vec->x);
    maxPt.y = max(maxPt.y, vec->y);
    maxPt.z = max(maxPt.z, vec->z);
}

BoundingCone::BoundingCone(const std::vector<BoundingBox>* boxes, const D3DXMATRIX* projection, const D3DXVECTOR3* _apex, const D3DXVECTOR3* _direction): apex(*_apex), direction(*_direction)
{
    const D3DXVECTOR3 yAxis(0.f, 1.f, 0.f);
    const D3DXVECTOR3 zAxis(0.f, 0.f, 1.f);
    D3DXVec3Normalize(&direction, &direction);
    
    D3DXVECTOR3 axis = yAxis;

    if ( fabsf(D3DXVec3Dot(&yAxis, &direction))>0.99f )
        axis = zAxis;
    
    D3DXMatrixLookAtLH(&m_LookAt, &apex, &(apex+direction), &axis);
    
    float maxx = 0.f, maxy = 0.f;
    fNear = 1e32f;
    fFar =  0.f;

    D3DXMATRIX concatMatrix;
    D3DXMatrixMultiply(&concatMatrix, projection, &m_LookAt);

    for (unsigned int i=0; i<boxes->size(); i++)
    {
        const BoundingBox& bbox = (*boxes)[i];
        for (int j=0; j<8; j++)
        {
            D3DXVECTOR3 vec( (j&1)?bbox.minPt.x : bbox.maxPt.x, (j&2)?bbox.minPt.y : bbox.maxPt.y, (j&4)?bbox.minPt.z : bbox.maxPt.z );
            D3DXVec3TransformCoord(&vec, &vec, &concatMatrix);
            maxx = max(maxx, fabsf(vec.x / vec.z));
            maxy = max(maxy, fabsf(vec.y / vec.z));
            fNear = min(fNear, vec.z);
            fFar  = max(fFar, vec.z);
        }
    }
    fovx = atanf(maxx);
    fovy = atanf(maxy);
}

BoundingCone::BoundingCone(const std::vector<BoundingBox>* boxes, const D3DXMATRIX* projection, const D3DXVECTOR3* _apex) : apex(*_apex)
{
    const D3DXVECTOR3 yAxis(0.f, 1.f, 0.f);
    const D3DXVECTOR3 zAxis(0.f, 0.f, 1.f);
    const D3DXVECTOR3 negZAxis(0.f, 0.f, -1.f);
    switch (boxes->size())
    {
    case 0: 
    {
        direction = negZAxis;
        fovx = 0.f;
        fovy = 0.f;
        D3DXMatrixIdentity(&m_LookAt);
        break;
    }
    default:
    {
        unsigned int i, j;


        //  compute a tight bounding sphere for the vertices of the bounding boxes.
        //  the vector from the apex to the center of the sphere is the optimized view direction
        //  start by xforming all points to post-projective space
        BoundingSphere bSphere;
        bSphere.radius = 0.f;

        std::vector<D3DXVECTOR3> ppPts;
        ppPts.reserve(boxes->size() * 8);

        for (i=0; i<boxes->size(); i++) 
        {
            const D3DXVECTOR3& minPt = (*boxes)[i].minPt;
            const D3DXVECTOR3& maxPt = (*boxes)[i].maxPt;

            D3DXVECTOR3 projectiveBBox[8];

            for (j=0; j<8; j++)
            {
                D3DXVECTOR3 tmp( (j&1)?minPt.x : maxPt.x, (j&2)?minPt.y : maxPt.y, (j&4)?minPt.z : maxPt.z);
                D3DXVec3TransformCoord(&tmp, &tmp, projection);

                //
                if ((tmp.z>1.f))
                    continue;

                ppPts.push_back(tmp);

                if (ppPts.size()==1)  // first point in the list -- initialize sphere 
                                      // (otherwise, 0,0,0 would be incorrectly included)
                {
                    bSphere.centerVec = tmp;
                }
                
                //  iteratively add points to the bounding sphere
                else
                {
                    D3DXVECTOR3 cVec = tmp - bSphere.centerVec;
                    float d = D3DXVec3Length(&cVec);
                    if ( d > bSphere.radius)
                    {
                        float r = 0.5f * (d + bSphere.radius);
                        float scale = (r-bSphere.radius) / d;
                        bSphere.centerVec = bSphere.centerVec + scale*cVec;
                        bSphere.radius = r;
                    }
                }
            }
        }

        float min_cosTheta = 1.f;
        
        direction = bSphere.centerVec - apex;
        D3DXVec3Normalize(&direction, &direction);

		// oles: break: smooth-direction movement
		// oles: actually - this doesn't work with fast direction changes
		/*
		static	D3DXVECTOR3 prev_direction	= D3DXVECTOR3(0,0,1);
		D3DXVECTOR3			new_direction	= .9f*prev_direction + .1f*direction;
		D3DXVec3Normalize	(&new_direction, &new_direction);
		prev_direction		= direction		= new_direction;
		*/
		/*
		D3DXVECTOR3			_center;
		D3DXVec3TransformCoord(&_center, &apex, projection);
		D3DXVECTOR3			new_direction	= .9f*prev_direction + .1f*direction;
		D3DXVec3Normalize	(&new_direction, &new_direction);
		prev_direction		= direction		= new_direction;
		*/

		// 
        D3DXVECTOR3 axis = yAxis;

        if ( fabsf(D3DXVec3Dot(&yAxis, &direction)) > 0.99f )
            axis = zAxis;

        D3DXMatrixLookAtLH(&m_LookAt, &apex, &(apex+direction), &axis);

        fNear = 1e32f;
        fFar = 0.f;

        float maxx=0.f, maxy=0.f;
        for (i=0; i<ppPts.size(); i++)
        {
            D3DXVECTOR3 tmp;
            D3DXVec3TransformCoord(&tmp, &ppPts[i], &m_LookAt);
            maxx	= max(maxx, fabsf(tmp.x / tmp.z));
            maxy	= max(maxy, fabsf(tmp.y / tmp.z));
            fNear	= min(fNear, tmp.z);
            fFar	= max(fFar, tmp.z);
        }

        fovx = atanf(maxx);
        fovy = atanf(maxy);
        break;
    }
    } // switch
}

///////////////////////////////////////////////////////////////////

void GetSceneBoundingBox( BoundingBox* box, std::vector<BoundingBox>* modelBoxen, const NVBScene* scene )
{
    BoundingBox tmp;
    box->minPt = D3DXVECTOR3( 3.3e33f,  3.3e33f,  3.3e33f );
    box->maxPt = D3DXVECTOR3(-3.3e33f, -3.3e33f, -3.3e33f );

    for (unsigned int i=0; i<scene->m_NumMeshes; i++)
    {
        GetModelBoundingBox(&tmp, &scene->m_Meshes[i]);
        modelBoxen->push_back(tmp);
        box->Merge(&tmp.minPt);
        box->Merge(&tmp.maxPt);
    }
}

void GetModelBoundingBox( BoundingBox* box, const NVBScene::Mesh* mesh )
{
    BoundingBox tmp;
    tmp.minPt = D3DXVECTOR3( 3.3e33f,  3.3e33f,  3.3e33f);
    tmp.maxPt = D3DXVECTOR3(-3.3e33f, -3.3e33f, -3.3e33f);

    for (unsigned int i=0; i<mesh->m_NumVertices; i++)
        tmp.Merge(&mesh->m_Vertices[i].m_Position);

    XFormBoundingBox( box, &tmp, &mesh->m_Transform );
}

//  Transform an axis-aligned bounding box by the specified matrix, and compute a new axis-aligned bounding box
void XFormBoundingBox( BoundingBox* result, const BoundingBox* src, const D3DXMATRIX* matrix )
{
    D3DXVECTOR3  pts[8];
    pts[0] = D3DXVECTOR3( src->minPt.x, src->minPt.y, src->minPt.z );
    pts[1] = D3DXVECTOR3( src->minPt.x, src->minPt.y, src->maxPt.z );
    pts[2] = D3DXVECTOR3( src->minPt.x, src->maxPt.y, src->minPt.z );
    pts[3] = D3DXVECTOR3( src->minPt.x, src->maxPt.y, src->maxPt.z );
    pts[4] = D3DXVECTOR3( src->maxPt.x, src->minPt.y, src->minPt.z );
    pts[5] = D3DXVECTOR3( src->maxPt.x, src->minPt.y, src->maxPt.z );
    pts[6] = D3DXVECTOR3( src->maxPt.x, src->maxPt.y, src->minPt.z );
    pts[7] = D3DXVECTOR3( src->maxPt.x, src->maxPt.y, src->maxPt.z );

    result->minPt = D3DXVECTOR3(3.3e33f, 3.3e33f, 3.3e33f);
    result->maxPt = D3DXVECTOR3(-3.3e33f, -3.3e33f, -3.3e33f);

    for (int i=0; i<8; i++)
    {
        D3DXVECTOR3 tmp;
        D3DXVec3TransformCoord(&tmp, &pts[i], matrix);
        result->minPt.x = min(result->minPt.x, tmp.x);
        result->minPt.y = min(result->minPt.y, tmp.y);
        result->minPt.z = min(result->minPt.z, tmp.z);
        result->maxPt.x = max(result->maxPt.x, tmp.x);
        result->maxPt.y = max(result->maxPt.y, tmp.y);
        result->maxPt.z = max(result->maxPt.z, tmp.z);
    }
}

void GetTerrainBoundingBox( std::vector<BoundingBox>* shadowReceivers, const D3DXMATRIX* modelView, const Frustum* sceneFrustum )
{
    D3DXVECTOR3 smq_start(-SMQUAD_SIZE, -10.f, -SMQUAD_SIZE);
    D3DXVECTOR3 smq_width(2.f*SMQUAD_SIZE, 0.f, 0.f);
    D3DXVECTOR3 smq_height(0.f, 0.f, 2.f*SMQUAD_SIZE);

    for (int k=0; k<4*4; k++)
    {
        float kx = float(k&0x3);
        float ky = float((k>>2)&0x3);
        BoundingBox hugeBox;
        hugeBox.minPt = smq_start + (kx/4.f)*smq_width + (ky/4.f)*smq_height;
        hugeBox.maxPt = smq_start + ((kx+1.f)/4.f)*smq_width + ((ky+1.f)/4.f)*smq_height;
        int hugeResult = sceneFrustum->TestBox(&hugeBox);
        if ( hugeResult !=2 )  //  2 requires more testing...  0 is fast reject, 1 is fast accept
        {
            if ( hugeResult == 1 )
            {
                XFormBoundingBox(&hugeBox, &hugeBox, modelView);
                shadowReceivers->push_back(hugeBox);
            }
            continue;
        }


        for (int j=0; j<4*4; j++)
        {
            float jx = kx*4.f + float(j&0x3);
            float jy = ky*4.f + float((j>>2)&0x3);
            BoundingBox bigBox;
            bigBox.minPt = smq_start + (jx/16.f)*smq_width + (jy/16.f)*smq_height;
            bigBox.maxPt = smq_start + ((jx+1.f)/16.f)*smq_width + ((jy+1.f)/16.f)*smq_height;
            int bigResult = sceneFrustum->TestBox(&bigBox);
            if ( bigResult != 2 )
            {
                if ( bigResult == 1 )
                {
                    XFormBoundingBox(&bigBox, &bigBox, modelView);
                    shadowReceivers->push_back(bigBox);
                }
                continue;
            }

            int stack = 0;

            for (int q=0; q<4; q++)
            {
                float iy = jy*4.f + float(q);
                int stack = 0;

                for (int r=0; r<4; r++)
                {
                    float ix = jx*4.f + float(r);
                    BoundingBox smallBox;
                    smallBox.minPt = smq_start + (ix/64.f)*smq_width + (iy/64.f)*smq_height;
                    smallBox.maxPt = smq_start + ((ix+1.f)/64.f)*smq_width + ((iy+1.f)/64.f)*smq_height;
                    if (sceneFrustum->TestBox(&smallBox))
                    {
                        stack |= (1 << r);
                    }
                }

                if (stack)
                {
                    float firstX, lastX;
                    int i;
                    for (i=0; i<4; i++)
                    {
                        if ( (stack&(1<<i)) != 0)
                        {
                            firstX = float(i);
                            break;
                        }
                    }
                    for (i=3; i>=0; i--)
                    {
                        if ( (stack&(1<<i)) !=0)
                        {
                            lastX = float(i);
                            break;
                        }
                    }
                    firstX += jx*4.f;
                    lastX  += jx*4.f;

                    BoundingBox coalescedBox;
                    coalescedBox.minPt = smq_start + (firstX/64.f)*smq_width + (iy/64.f)*smq_height;
                    coalescedBox.maxPt = smq_start + ((lastX+1.f)/64.f)*smq_width + ((iy+1.f)/64.f)*smq_height;
                    XFormBoundingBox(&coalescedBox, &coalescedBox, modelView);
                    shadowReceivers->push_back(coalescedBox);
                }
            }
        }
    }

}