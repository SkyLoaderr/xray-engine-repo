//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalPSM
// File:  PracticalPSM.cpp
// 
// Copyright NVIDIA Corporation 2002-2003
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Note: This code uses the D3D Framework helper library.
// Link to: DX9SDKSampleFramework.lib d3dxof.lib dxguid.lib d3dx9dt.lib d3d9.lib winmm.lib comctl32.lib
//
//-----------------------------------------------------------------------------

#define STRICT

#include <Windows.h>
#include <commctrl.h>
#include <math.h>
#include <D3DX9.h>
#include <d3dx9math.h>
#include <d3dx9effect.h>
#include <d3dx9shader.h>
#include <DX9SDKSampleFramework/DX9SDKSampleFramework.h>
#include <shared/GetFilePath.h>
#pragma warning(disable : 4786)
#include <vector>
#pragma warning(disable : 4786)
#include <assert.h>
#include <shared/NVFileDialog.h>

#include "MouseUIs.h"
#include "PracticalPSM.h"
#include "Util.h"
#include "Bounding.h"
#include "common.h"

HINSTANCE g_hInstance = NULL;

//  misc. functions necessary to run the program, but not critical to the algorithm...

#include "FrameWork.inc"
#include "Helpers.inc"

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CPracticalPSM::Render()
{
    HRESULT hr;
    D3DXHANDLE hTechnique = NULL;

    //  update animation
    Tick();

    if (m_pd3dDevice->BeginScene() == D3D_OK)
    {

        //render into shadow map
        if(FAILED(RenderShadowMap()))
            return E_FAIL;

        if (FAILED(SetRenderStates(false)))
            return E_FAIL;

        if (FAILED(m_pEffect->SetTechnique("UseHardwareShadowMap")))
        {
            MessageBox(NULL, "Failed to set 'UseHardwareShadowMap' technique in effect file", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
            return E_FAIL;
        }

        //set depth map as texture
        if (FAILED(m_pEffect->SetTexture("ShadowColor", m_pSMColorTexture)))
            return E_FAIL;

        if(FAILED(m_pEffect->SetTexture("ShadowMap", m_pSMZTexture)))
            return E_FAIL;
        
        //set special texture matrix for shadow mapping
        float fOffsetX = 0.5f + (0.5f / (float)TEXDEPTH_WIDTH);
        float fOffsetY = 0.5f + (0.5f / (float)TEXDEPTH_HEIGHT);
        unsigned int range = 1;
        float fBias    = 0.0f;
        D3DXMATRIX texScaleBiasMat( 0.5f,     0.0f,     0.0f,         0.0f,
                                    0.0f,    -0.5f,     0.0f,         0.0f,
                                    0.0f,     0.0f,     (float)range, 0.0f,
                                    fOffsetX, fOffsetY, 0.0f,         1.0f );


        //  draw the clawbot and the rockchunk
        {
            //  the shadow map is projective (m_bDisplayShadowMap unsets this flag)
            m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
            m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED);        

            D3DXMATRIX textureMatrix;
            D3DXMatrixMultiply(&textureMatrix, &m_LightViewProj, &texScaleBiasMat);
          
            for (unsigned int i=0; i<m_ShadowReceiverObjects.size(); i++)
            {
                const ObjectInstance*& instance = m_ShadowReceiverObjects[i];

                D3DXMATRIX worldMatrix;
                D3DXMatrixTranslation(&worldMatrix, instance->translation.x, instance->translation.y, instance->translation.z);
                D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &m_World);
                if (FAILED(hr=DrawScene(instance->scene, &worldMatrix, &m_View, &m_Projection, &textureMatrix)))
                    return hr;
            }
            //  set up the clawbot translation*world matrix, and the texture projection matrix   
            D3DXMATRIX tempScaleMatrix;
            D3DXMATRIX viewProjMatrix;
            D3DXMATRIX scaledTextureMatrix;
            D3DXMatrixScaling(&tempScaleMatrix, m_smQuad.scaleVec.x, m_smQuad.scaleVec.y, m_smQuad.scaleVec.z);
            D3DXMatrixMultiply(&viewProjMatrix, &m_View, &m_Projection);
            D3DXMatrixMultiply(&scaledTextureMatrix, &tempScaleMatrix, &textureMatrix);

            SetVertexShaderMatrices(tempScaleMatrix, viewProjMatrix, scaledTextureMatrix);


            if (FAILED(DrawQuad()))
                return E_FAIL;
        }

        if (m_bDisplayShadowMap)
        {
            // if we display the shadow map, do it here
            m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
            m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
            m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

            m_pEffect->SetTechnique("DrawHardwareShadowMap");
            if (FAILED(DrawQuad()))
                return E_FAIL;
        }

        //  write out the interesting info about the rendering...
        DrawStats();

        m_pd3dDevice->EndScene();
    }

    m_frame++;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderShadowMap
// Desc: Called once per frame, the function is responsible for building the
//       perspective shadow map projection matrix and rendering the actual
//       shadow map
//-----------------------------------------------------------------------------

HRESULT CPracticalPSM::RenderShadowMap()
{  
    D3DXMATRIX lightView, lightProj, virtualCameraViewProj, virtualCameraView, virtualCameraProj;

    const D3DXVECTOR3 yAxis  ( 0.f, 1.f, 0.f);
    const D3DXVECTOR3 zAxis  ( 0.f, 0.f, 1.f);

    //  update the virutal scene camera's bounding parameters
    ComputeVirtualCameraParameters( );

    if (m_bSlideBack)
    {
        //  clamp the view-cube to the slid back objects...
        const D3DXVECTOR3 eyePt(0.f, 0.f, 0.f);
        const D3DXVECTOR3 eyeDir(0.f, 0.f, 1.f);
        D3DXMatrixTranslation(&virtualCameraView, 0.f, 0.f, m_fSlideBack);
        BoundingCone bc(&m_ShadowReceiverPoints, &virtualCameraView, &eyePt, &eyeDir);
        D3DXMatrixPerspectiveFovLH( &virtualCameraProj, 2.f*bc.fovy, bc.fovx/bc.fovy, m_zNear, m_zFar);
    }
    else
    {
        D3DXMatrixIdentity( &virtualCameraView );
        D3DXMatrixPerspectiveFovLH( &virtualCameraProj, D3DXToRadian(60.f), m_fAspect, m_zNear, m_zFar);
    }

    D3DXMatrixMultiply(&virtualCameraViewProj, &m_View, &virtualCameraView);
    D3DXMatrixMultiply(&virtualCameraViewProj, &virtualCameraViewProj, &virtualCameraProj);
   
    D3DXMATRIX eyeToPostProjectiveVirtualCamera;
    D3DXMatrixMultiply(&eyeToPostProjectiveVirtualCamera, &virtualCameraView, &virtualCameraProj);

    D3DXVECTOR3 eyeLightDir;  D3DXVec3TransformNormal(&eyeLightDir, &m_lightDir, &m_View);

    //  directional light becomes a point on infinity plane in post-projective space
    D3DXVECTOR4 lightDirW (eyeLightDir.x, eyeLightDir.y, eyeLightDir.z, 0.f);   
    D3DXVECTOR4   ppLight;

    D3DXVec4Transform(&ppLight, &lightDirW, &virtualCameraProj);

    m_bShadowTestInverted = (ppLight.w < 0.f); // the light is coming from behind the eye
    
    //  compute the projection matrix...
    //  if the light is >= 1000 units away from the unit box, use an ortho matrix (standard shadow mapping)
    if ( fabsf(ppLight.w) <= W_EPSILON )  // orthographic matrix
    {
        D3DXVECTOR3 cubeCenter(0.f, 0.f, 0.5f);
        D3DXVECTOR3 ppLightDirection(ppLight.x, ppLight.y, ppLight.z);
        D3DXVec3Normalize(&ppLightDirection, &ppLightDirection);

        BoundingBox ppUnitBox;
        ppUnitBox.maxPt = D3DXVECTOR3(1, 1, 1);
        ppUnitBox.minPt = D3DXVECTOR3(-1, -1, 0);
        float t;       
        RayIntersectsUnitBox(&t, &cubeCenter, &ppLightDirection);
        D3DXVECTOR3 lightPos = cubeCenter + 2.f*t*ppLightDirection;
        D3DXVECTOR3 axis = yAxis;
        
        //  if the yAxis and the view direction are aligned, choose a different up vector, to avoid singularity
        //  artifacts
        if ( fabsf(D3DXVec3Dot(&ppLightDirection, &yAxis))>0.99f )
            axis = zAxis;

        D3DXMatrixLookAtLH(&lightView, &lightPos, &cubeCenter, &axis);
        XFormBoundingBox(&ppUnitBox, &ppUnitBox, &lightView);
        D3DXMatrixOrthoOffCenterLH(&lightProj, ppUnitBox.minPt.x, ppUnitBox.maxPt.x, ppUnitBox.minPt.y, ppUnitBox.maxPt.y, ppUnitBox.minPt.z, ppUnitBox.maxPt.z);
    }
    else  // otherwise, use perspective shadow mapping
    {
        D3DXVECTOR3 ppLightPos;
        float wRecip = 1.0f / ppLight.w;
        ppLightPos.x = ppLight.x * wRecip;
        ppLightPos.y = ppLight.y * wRecip;
        ppLightPos.z = ppLight.z * wRecip;

        D3DXMATRIX eyeToPostProjectiveLightView;

        const float			ppCubeRadius	= 1.5f;	//.
        const D3DXVECTOR3	ppCubeCenter	(0.f, 0.f, 0.5f);

        if (m_bShadowTestInverted)  // use the inverse projection matrix
        {
            BoundingCone viewCone;
            if (!m_bUnitCubeClip)
            {
                //  project the entire unit cube into the shadow map  
                std::vector<BoundingBox> justOneBox;
                BoundingBox unitCube;
                unitCube.minPt = D3DXVECTOR3(-1.f, -1.f, 0.f);
                unitCube.maxPt = D3DXVECTOR3( 1.f, 1.f, 1.f );
                justOneBox.push_back(unitCube);
                D3DXMATRIX tmpIdentity;
                D3DXMatrixIdentity(&tmpIdentity);
                viewCone = BoundingCone(&justOneBox, &tmpIdentity, &ppLightPos);               
            }
            else
            {
                //  clip the shadow map to just the used portions of the unit box.
                viewCone = BoundingCone(&m_ShadowReceiverPoints, &eyeToPostProjectiveVirtualCamera, &ppLightPos);
            }
            
            //  construct the inverse projection matrix -- clamp the fNear value for sanity (clamping at too low
            //  a value causes significant underflow in a 24-bit depth buffer)
            viewCone.fNear = max(0.001f, viewCone.fNear);
            m_ppNear = -viewCone.fNear;
            m_ppFar  = viewCone.fNear;
            lightView = viewCone.m_LookAt;
            D3DXMatrixPerspectiveFovLH(&lightProj, 2.f*viewCone.fovy, viewCone.fovx/viewCone.fovy, m_ppNear, m_ppFar);
        }
        else  // regular projection matrix
        {
            float fFovy, fAspect, fFar, fNear;
            if (!m_bUnitCubeClip)
            {
                D3DXVECTOR3 lookAt = ppCubeCenter - ppLightPos;
                D3DXVec3Normalize(&lookAt, &lookAt);
                D3DXVECTOR3 axis = yAxis;

                //  if the yAxis and the view direction are aligned, choose a different up vector, to avoid singularity
                //  artifacts
                if ( fabsf(D3DXVec3Dot(&yAxis, &lookAt))>0.99f )
                    axis = zAxis;

                //  this code is super-cheese.  treats the unit-box as a sphere
                //  lots of problems, looks like hell, and requires that MinInfinityZ >= 2
                D3DXMatrixLookAtLH(&lightView, &ppLightPos, &ppCubeCenter, &axis);
                float distance = D3DXVec3Length(&lookAt);
                fFovy = 2.f*atanf(ppCubeRadius/distance);
                fAspect = 1.f;
                fNear = max(0.001f, distance - 2.f*ppCubeRadius);
                fFar = distance + 2.f*ppCubeRadius;
                BoundingBox ppView;
                D3DXMatrixMultiply(&eyeToPostProjectiveLightView, &eyeToPostProjectiveVirtualCamera, &lightView);
            }
            else
            {
                //  unit cube clipping
                //  fit a cone to the bounding geometries of all shadow receivers (incl. terrain) in the scene
                BoundingCone bc(&m_ShadowReceiverPoints, &eyeToPostProjectiveVirtualCamera, &ppLightPos);
                lightView = bc.m_LookAt;
                D3DXMatrixMultiply(&eyeToPostProjectiveLightView, &eyeToPostProjectiveVirtualCamera, &lightView);
                float fDistance = D3DXVec3Length(&(ppLightPos-ppCubeCenter));
                fFovy = 2.f * bc.fovy;
                fAspect = bc.fovx / bc.fovy;
                fFar = bc.fFar;
                //  hack alert!  adjust the near-plane value a little bit, to avoid clamping problems
                fNear = max(0.001f, bc.fNear*0.8f);
            }
           
            fNear = max(0.001f, fNear);
            m_ppNear = fNear;
            m_ppFar = fFar;
            D3DXMatrixPerspectiveFovLH(&lightProj, fFovy, fAspect, m_ppNear, m_ppFar);
        }
    }

    //  build the composite matrix that transforms from world space into post-projective light space
    D3DXMatrixMultiply(&m_LightViewProj, &lightView, &lightProj);
    D3DXMatrixMultiply(&m_LightViewProj, &virtualCameraViewProj, &m_LightViewProj);
    
    if (FAILED(SetRenderStates(true)))
        return E_FAIL;

    //  either draw black into the shadow map (ps 1.x), or a greyscale value representing Z (ps 2.0)
    if (m_bSupportsPixelShaders20)
    {
        if (FAILED(m_pEffect->SetTechnique("GenHardwareShadowMap20")))
        {
            MessageBox(NULL, "Failed to set 'GenHardwareShadowMap20' technique in effect file", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
            return E_FAIL;
        }
    }
    else
    {
        if (FAILED(m_pEffect->SetTechnique("GenHardwareShadowMap11")))
        {
            MessageBox(NULL, "Failed to set 'GenHardwareShadowMap11' technique in effect file", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
            return E_FAIL;
        }
    }

    D3DXMATRIX tempIdentity;
    D3DXMATRIX clawBotWorldMatrix;
    D3DXMatrixIdentity(&tempIdentity);

    m_pd3dDevice->SetVertexDeclaration(m_pDeclaration);

    //  blast through cached shadow caster objects, and render them all
    for (unsigned int i=0; i<m_ShadowCasterObjects.size(); i++)
    {
        const ObjectInstance*& instance = m_ShadowCasterObjects[i];
        D3DXMATRIX worldMatrix;
        D3DXMatrixTranslation(&worldMatrix, instance->translation.x, instance->translation.y, instance->translation.z);
        D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &m_World);
        if (FAILED(DrawScene(instance->scene, &worldMatrix, &m_LightViewProj, &tempIdentity, &tempIdentity)))
            return E_FAIL;
    }

    return S_OK;


}


//-----------------------------------------------------------------------------
//  CPracticalPSM::ComputeVirtualCameraParameters( )
//    computes the near & far clip planes for the virtual camera based on the
//    scene.
//
//    bounds the field of view for the virtual camera based on a swept-sphere/frustum
//    intersection.  if the swept sphere representing the extrusion of an object's bounding
//    sphere along the light direction intersects the view frustum, the object is added to
//    a list of interesting shadow casters.  the field of view is the minimum cone containing
//    all eligible bounding spheres.
//-----------------------------------------------------------------------------

void CPracticalPSM::ComputeVirtualCameraParameters( )
{
    bool hit = false;

    //  frustum is in world space, so that bounding boxes are minimal size (xforming an AABB
    //  generally increases its volume).
    D3DXMATRIX modelView;
    D3DXMATRIX modelViewProjection;
    D3DXMatrixMultiply(&modelView, &m_World, &m_View);
    D3DXMatrixMultiply(&modelViewProjection, &modelView, &m_Projection);
    D3DXVECTOR3 sweepDir = -m_lightDir;

    Frustum sceneFrustum( &modelViewProjection );

    m_ShadowCasterPoints.clear();
    m_ShadowCasterObjects.clear();
    m_ShadowReceiverObjects.clear();
    m_ShadowReceiverPoints.clear();

    for (unsigned int i=0; i<m_Scenes.size(); i++)
    {
        const ObjectInstance* instance = m_Scenes[i];
        BoundingBox instanceBox = *instance->aabb;
        instanceBox.minPt += instance->translation;
        instanceBox.maxPt += instance->translation;
        int inFrustum = sceneFrustum.TestBox(&instanceBox);  //  0 = outside.  1 = inside.   2 = intersection
        bool shadowCaster	= (inFrustum!=0);
		hit					|= shadowCaster;
        if (!inFrustum)
        {
            //  test the swept bounding sphere against the view frustum, to detect external shadow casters
            //  can't test against just the ground vs frustum, since shadows are being cast onto all objects
            BoundingSphere instanceSphere(&instanceBox);
            shadowCaster = sceneFrustum.TestSweptSphere(&instanceSphere, &sweepDir);
        }
        if (shadowCaster)
        {
            XFormBoundingBox(&instanceBox, &instanceBox, &modelView);
            m_ShadowCasterPoints.push_back(instanceBox);
            m_ShadowCasterObjects.push_back(instance);
            if (inFrustum)
            {
                m_ShadowReceiverPoints.push_back(instanceBox);
                m_ShadowReceiverObjects.push_back(instance);
            }
        }
    }

    //  add the biggest shadow receiver -- the ground!
    GetTerrainBoundingBox(&m_ShadowReceiverPoints, &modelView, &sceneFrustum);

    //  these are the limits specified by the physical camera
    float lightDot = m_lightDir.x * m_View._13 +
                     m_lightDir.y * m_View._23 +
                     m_lightDir.z * m_View._33;

    float ZCLAMP;

    if (m_bSlideBack && m_ShadowCasterPoints.size())
    {
        //  compute a dynamic slideback, using the mean & std.deviation of the shadow caster Z's
        //  choose a near plane that is 1.5 standard deviations closer to the viewer than the average
        //  shadow caster depth.
        float z_mean = 0.f;
        const unsigned int nSize = (unsigned int)m_ShadowCasterPoints.size();
        float nSamples = 1.f / float(nSize);
        unsigned int i;

        //  compute average Z value relative to viewer
        for (i=0; i<nSize; i++)
        {
            float zavg = 0.5f*(m_ShadowCasterPoints[i].minPt.z + m_ShadowCasterPoints[i].maxPt.z);
            z_mean += zavg*nSamples;
        }

        //  compute variance
        float z_variance = 0.f;
        for (i=0; i<nSize; i++)
        {
            float zavg = 0.5f*(m_ShadowCasterPoints[i].minPt.z + m_ShadowCasterPoints[i].maxPt.z);
            float delta = z_mean - zavg;
            z_variance += (delta*delta)*nSamples;
        }

        float z_stddev = sqrtf(z_variance);
        ZCLAMP = max(ZNEAR_MIN, z_mean - m_fNumStdDevs*z_stddev);
    }
    else
        ZCLAMP = ZNEAR_MIN;
    
    if (!hit)
    {
        m_zNear			= ZNEAR_MIN;
        m_zFar			= ZFAR_MAX;
        m_fSlideBack	= 0.f;
    }
    else
    {
        float min_z		= 1e32f;
        float max_z		= 0.f;
        for (unsigned int i=0;i < m_ShadowReceiverPoints.size(); i++) 
        {
            min_z = min(min_z, m_ShadowReceiverPoints[i].minPt.z);
            max_z = max(max_z, m_ShadowReceiverPoints[i].maxPt.z);
        }

        if (m_bSlideBack && (min_z < ZCLAMP))
        {
            m_fSlideBack = ZCLAMP - min_z;
            m_zNear = ZCLAMP;
        }
        else 
        {
            m_zNear = (m_bSlideBack)?max(ZCLAMP, min_z) : ZCLAMP;
            m_fSlideBack = 0.f;
        }
    
        m_zFar = min( ZFAR_MAX, max_z ) + m_fSlideBack;

    }

    //  some additional slideback, to force some distance between the infinity plane and the view-box
    //  1.35-1.4 seems to work really well with the current bounding cone code.
    const float Z_EPSILON	=	0.0001f;
    float infinity			=	m_zFar/(m_zFar-m_zNear);
    if (infinity<=m_fMinInfinityZ)	{
        float additionalSlide = m_fMinInfinityZ*(m_zFar-m_zNear) - m_zFar + Z_EPSILON;
        m_fSlideBack += additionalSlide;
        m_zFar += additionalSlide;
        m_zNear += additionalSlide;
    }
}

//-----------------------------------------------------------------------------
// Name: CPracticalPSM()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CPracticalPSM::CPracticalPSM()
{
    m_time			= ::timeGetTime()*0.001f;
    m_startTime		= m_time;
    m_frame			= 0;
    m_fps			= 30;
    m_pEffect		= NULL;

    m_bShowHelp		= false;
    m_bShowStats	= false;
    m_pAttributes	= NULL;
    m_SelectedUI	= NULL;
    m_MouseFPSUI	= NULL;
    m_pBackBuffer	= NULL;
    m_pZBuffer		= NULL;
    m_pSMColorTexture = NULL;
    m_pSMZTexture = NULL;
    m_pSMColorSurface = NULL;
    m_pSMZSurface = NULL;
    m_bUnitCubeClip = true;
    m_bSlideBack = true;
    m_lightDir = D3DXVECTOR3(0.f, 0.f, 0.f);
    m_bitDepth = 24;
    m_Paused = false;
    m_bSupportsPixelShaders20 = false;
    m_bDisplayShadowMap = false;
    m_pDeclaration = NULL;
    m_iDepthBias = 4;
    m_fBiasSlope = 1.0f;
    m_fSlideBack = 0.f;
    m_pFont = NULL;
    m_pRockChunk = NULL;
    m_pClawBot = NULL;
    m_fNumStdDevs = 1.3f;
    m_fMinInfinityZ = 1.4f;

    D3DXMatrixIdentity(&m_World);
    D3DXMatrixIdentity(&m_View);
    D3DXMatrixIdentity(&m_Projection);

    //--------//

    m_strWindowTitle = _T("Hardware Perspective Shadow Maps 1.0 (DX9) (HLSL)");
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
    m_dwCreationWidth	= 800;       // default window width
    m_dwCreationHeight	= 600;       // default window height

    m_fAspect			= float(m_dwCreationWidth) / float(m_dwCreationHeight);
}