//-----------------------------------------------------------------------------
// Name: SetVertexShaderMatrices
// Desc: Sets the matrices for the effect file
//-----------------------------------------------------------------------------

HRESULT CPracticalPSM::SetVertexShaderMatrices(const D3DXMATRIX& worldMat, const D3DXMATRIX& viewProjMat, const D3DXMATRIX& texMat)
{
    D3DXMATRIX worldViewProjMat = worldMat * viewProjMat;

    D3DXMATRIX worldITMat;    
    D3DXMatrixInverse(&worldITMat, NULL, &worldMat);
    D3DXMatrixTranspose(&worldITMat, &worldITMat);
        
    m_pEffect->SetMatrix("WorldViewProj", &worldViewProjMat);
    m_pEffect->SetMatrix("WorldIT", &worldITMat);
    m_pEffect->SetMatrix("TexTransform", &texMat);
    
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Tick()
// Desc: Called once per frame, updates animation parameters, and other things
//-----------------------------------------------------------------------------
void CPracticalPSM::Tick( )
{
    static float time = 0.0f;
    static float lasttime = 0.f;
    static float phi = 0.f;
    static bool forward = true;

    // update time
    m_time = ::timeGetTime()*0.001f;
    if (m_frame == 0)
        m_startTime = lasttime = m_time;
    else if (m_time > m_startTime)
        m_fps = (float)m_frame / (m_time - m_startTime);

    // Update view and projection matrices
    if (m_SelectedUI) 
    {
        m_SelectedUI->UpdateKeyState();
        m_SelectedUI->Tick( m_time - lasttime );
        m_SelectedUI->GetViewMatrix(&m_View);
        lasttime = m_time;
    }

    D3DVIEWPORT9 viewport;
    m_pd3dDevice->GetViewport(&viewport);
    m_fAspect = float(viewport.Width) / float(viewport.Height);
    
    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);
    D3DXMatrixPerspectiveFovLH(&m_Projection, D3DXToRadian(60.f), m_fAspect, ZNEAR_MIN, ZFAR_MAX);

    // Update scene position
    D3DXMatrixIdentity(&m_World);

    if (!m_Paused)
    {
        if (forward)
            time += 30 / m_fps;
        else
            time -= 30 / m_fps;
    }
    if (time > m_pClawBot->m_NumMeshKeys - 1)
    {
        forward = false;
        time = (float)m_pClawBot->m_NumMeshKeys - 2.0f;
    }
    if (time < 1)
        forward = true;

    if (m_bLightAnimation)
    {
        phi = (m_time-m_startTime)/10.0f;
        phi = phi-floorf(phi);
        phi = 3.14159265f*phi;
    }
    
   
    m_lightDir.x = cosf(phi);
    m_lightDir.y = sinf(phi);
    m_lightDir.z = 0.0f;
    D3DXVec3Normalize(&m_lightDir, &m_lightDir);
    
    m_pEffect->SetValue("LightVec", (float*)&m_lightDir, sizeof(float)*3);

    m_pRockChunk->Update(time, &m_World);
    m_pClawBot->Update(time, &m_World);
    m_vClawBotLocalBBs.clear();
    m_vRockChunkLocalBBs.clear();
    GetSceneBoundingBox(&m_ClawBotLocalBB, &m_vClawBotLocalBBs, m_pClawBot);
    GetSceneBoundingBox(&m_RockChunkLocalBB, &m_vRockChunkLocalBBs, m_pRockChunk);
}



//-----------------------------------------------------------------------------
//  CreateQuad, DrawQuad
//    misc. functions for the big quad in the app
//-----------------------------------------------------------------------------

HRESULT CPracticalPSM::CreateQuad(SMMeshInfo* mesh)
{    
    HRESULT hr = S_OK;

    hr = m_pd3dDevice->CreateVertexBuffer(4 * sizeof(SMVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &(mesh->pVB), NULL);
    if(FAILED(hr))
        return hr;

    SMVertex* pVData;
    hr = mesh->pVB->Lock(0, 0, (void**)&pVData, 0);
    if(FAILED(hr))
        return hr;
    float value = SMQUAD_SIZE;
    pVData[0].x  = -value; pVData[0].y  = -10.0f; pVData[0].z  = value;
    pVData[0].nx = 0.0f;  pVData[0].ny = 1.0f; pVData[0].nz = 0.0f;
    pVData[1].x  = value;  pVData[1].y  = -10.0f; pVData[1].z  = value;
    pVData[1].nx = 0.0f;  pVData[1].ny = 1.0f; pVData[1].nz = 0.0f;
    pVData[2].x  = -value; pVData[2].y  = -10.0f; pVData[2].z  = -value;
    pVData[2].nx = 0.0f;  pVData[2].ny = 1.0f; pVData[2].nz = 0.0f;
    pVData[3].x  = value;  pVData[3].y  = -10.0f; pVData[3].z  = -value;
    pVData[3].nx = 0.0f;  pVData[3].ny = 1.0f; pVData[3].nz = 0.0f;
    hr = mesh->pVB->Unlock();
    if(FAILED(hr))
        return hr;

    hr = m_pd3dDevice->CreateIndexBuffer(4 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &(mesh->pIB), NULL);
    if(FAILED(hr))
        return hr;

    WORD* pIData;
    hr = mesh->pIB->Lock(0, 0, (void**)&pIData, 0);
    if(FAILED(hr))
        return hr;
    //it's a strip
    pIData[0] = 0;
    pIData[1] = 1;
    pIData[2] = 2;
    pIData[3] = 3;
    hr = mesh->pIB->Unlock();

    mesh->dwNumFaces = 2;
    mesh->dwNumVerts = 4;
    mesh->primType = D3DPT_TRIANGLESTRIP;

    //quad doesn't get scaled / translated
    mesh->scaleVec = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
    mesh->transVec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    return S_OK;
}

HRESULT CPracticalPSM::DrawQuad( )
{
    //set vb
    HRESULT hr = S_OK;

    hr = m_pd3dDevice->SetStreamSource(0, m_smQuad.pVB, 0, sizeof(SMVertex));
    if (FAILED(hr))
        return hr;

    //set index buffer
    hr = m_pd3dDevice->SetIndices(m_smQuad.pIB);
    if(FAILED(hr))
        return hr;

    //render quad using HardwareShadowMapTechnique
    UINT uPasses;
    if (D3D_OK == m_pEffect->Begin(&uPasses, 0)) {  // The 0 specifies that ID3DXEffect::Begin and ID3DXEffect::End will save and restore all state modified by the effect.
        for (UINT uPass = 0; uPass < uPasses; uPass++) {
            // Set the state for a particular pass in a technique.
            m_pEffect->BeginPass(uPass);	//. OLES: 2K4 SDK

            // Draw it 
            if(FAILED(m_pd3dDevice->DrawIndexedPrimitive(m_smQuad.primType, 0, 0, m_smQuad.dwNumVerts, 0, m_smQuad.dwNumFaces)))
                return E_FAIL;
                
            m_pEffect->EndPass();			//. OLES: 2K4 SDK
        }
        m_pEffect->End();
    }
    return hr;
}


//-----------------------------------------------------------------------------
//  CPracticalPSM::SetRenderStates()
//     sets the correct render states for the current render pass
//-----------------------------------------------------------------------------
HRESULT CPracticalPSM::SetRenderStates( bool shadowPass )
{
    static D3DVIEWPORT9 oldViewport;
    if (shadowPass)
    {
        //  preserve old viewport
        m_pd3dDevice->GetViewport(&oldViewport);

        //set render target to shadow map surfaces
        if(FAILED(m_pd3dDevice->SetRenderTarget(0, m_pSMColorSurface)))
            return E_FAIL;

        //set depth stencil
        if(FAILED(m_pd3dDevice->SetDepthStencilSurface(m_pSMZSurface)))
            return E_FAIL;

        //  set new viewport for shadow map
        D3DVIEWPORT9 newViewport;
        newViewport.X = 0;
        newViewport.Y = 0;
        if (m_bSupportsPixelShaders20)
        {
            newViewport.Width  = TEXDEPTH_SIZE_20;	//. OLES:
            newViewport.Height = TEXDEPTH_SIZE_20;	//. OLES:
        }
        else
        {
            newViewport.Width  = TEXDEPTH_SIZE_11;
            newViewport.Height = TEXDEPTH_SIZE_11;
        }
        newViewport.MinZ = 0.0f;
        newViewport.MaxZ = 1.0f;
        m_pd3dDevice->SetViewport(&newViewport);
        
        float depthBias = float(m_iDepthBias) / 16777215.f;

        //  render color if it is going to be displayed -- otherwise don't
        m_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, (m_bDisplayShadowMap)?0xf : 0);

        //depth bias
        m_pd3dDevice->Clear(0, NULL, (m_bDisplayShadowMap?D3DCLEAR_TARGET:0) | D3DCLEAR_ZBUFFER, 0x00FFFFFF, 1.0f, 0L);	//. OLES:
        m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        m_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&depthBias);
        m_pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&m_fBiasSlope);

        m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
        m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED);

        m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);		//. OLES: perf problem?
    }
    else 
    {
        float fTemp = 0.0f;
        m_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fTemp);
        m_pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fTemp);
        // restore old viewport
        m_pd3dDevice->SetViewport(&oldViewport);
        m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        
        //set render target back to normal back buffer / depth buffer
        if(FAILED(m_pd3dDevice->SetRenderTarget(0, m_pBackBuffer)))
            return E_FAIL;
        if(FAILED(m_pd3dDevice->SetDepthStencilSurface(m_pZBuffer)))
            return E_FAIL;

        //reenable color writes
        m_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);
        m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 60, 60, 60), 1.0f, 0L);
        m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

        m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);		//. OLES: perf problem?
    }
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DrawScene()
// Desc: Draws an NVBScene
//-----------------------------------------------------------------------------
HRESULT CPracticalPSM::DrawScene(const NVBScene* scene, const D3DXMATRIX* model, const D3DXMATRIX* view, const D3DXMATRIX* project, const D3DXMATRIX* texture)
{
    unsigned int i;
    D3DXMATRIX viewProjMatrix;
    HRESULT hr = S_OK;
    
    D3DXMatrixMultiply(&viewProjMatrix, view, project);

    for (i = 0; i < scene->m_NumMeshes; ++i) 
    {
        const NVBScene::Mesh& mesh = scene->m_Meshes[i];

        D3DXMATRIX worldMatrix, texMatrix;
        
        D3DXMatrixMultiply(&worldMatrix, &mesh.m_Transform, model);
        D3DXMatrixMultiply(&texMatrix, &worldMatrix, texture);

        SetVertexShaderMatrices(worldMatrix, viewProjMatrix, texMatrix);

        // render mesh using HardwareShadowMapTechnique
        UINT uPasses;
        if (D3D_OK == m_pEffect->Begin(&uPasses, 0)) {  // The 0 specifies that ID3DXEffect::Begin and ID3DXEffect::End will save and restore all state modified by the effect.
            for (UINT uPass = 0; uPass < uPasses; uPass++) {
                // Set the state for a particular pass in a technique.
                m_pEffect->BeginPass(uPass);	//. OLES: 2K4 SDK

                // Draw it 
                if (FAILED(hr = mesh.Draw()))
                    return hr;
                    
				m_pEffect->EndPass	();         //. OLES: 2K4 SDK
            }
            m_pEffect->End();
        }
    }
    return hr;
}

//-----------------------------------------------------------------------------
// Name: WriteText()
// Desc: Draws an NVBScene
//-----------------------------------------------------------------------------
void CPracticalPSM::DrawStats()
{
    //draw text
    char buff[180];
    if (!m_bShowHelp)
    {
        sprintf (buff, "Press F1 for Help");
        m_pFont->DrawText(2, 2, D3DCOLOR_ARGB(255,255,255,0), buff);
    }
    else
    {
        sprintf (buff, "F1 - Toggle Help Screen                    F4 - Toggle Detailed Stats");
        m_pFont->DrawText(2, 478, D3DCOLOR_ARGB(255,255,255,0), buff);
        sprintf (buff, "WASD - Move        Mouse1 - Rotate     R - Randomize");
        m_pFont->DrawText(2, 498, D3DCOLOR_ARGB(255,255,255,0), buff);
        sprintf (buff, "PgUp/PgDn - Move Up/Down       Home - Reset Camera     Shift+Move - Run");
        m_pFont->DrawText(2, 518, D3DCOLOR_ARGB(255,255,255,0), buff);       
        sprintf (buff, "~ - Display Shadow Map                    SPACE - Play/Pause             L - Animate Light");
        m_pFont->DrawText(2, 538, D3DCOLOR_ARGB(255,255,255,0), buff);
        sprintf (buff, "[] :Depth Slope Bias        -+ :Depth Bias     :\" :InfinityZ SlideBack    <> :LiSPSM N Scale");	//. OLES: 
        m_pFont->DrawText(2, 558, D3DCOLOR_ARGB(255,255,255,0), buff);
        sprintf (buff, "P - Toggle Perspective/Ortho Shadow Map       C - Toggle Unit Cube Clipping         B - Toggle Infinity-Z Slide Back");
        m_pFont->DrawText(2, 578, D3DCOLOR_ARGB(255,255,255,0), buff);
    }
    
    //. OLES
    static const char* _types	[smap_count] = 
    {
    "Uniform",
    "PSM",
    "LiPSM",
    "TSM"
    };
    
    sprintf (buff, "type: %s  Depth Bias(%8d), Slope Scale Bias(%.2f), LiSPSM-Nscale(%.2f))", _types[m_smap_mode], m_iDepthBias, m_fBiasSlope, m_fBiasLiSPSM);			//. OLES:
    m_pFont->DrawText (2, 22, D3DCOLOR_ARGB(255,255,255,0), buff);
    sprintf (buff, "InfinityZ: %.2f", m_fMinInfinityZ);
    m_pFont->DrawText (2, 42, D3DCOLOR_ARGB(255,255,255,0), buff);

    if (m_bShowStats)
    {
        sprintf (buff, "PSM Projection: %s", (m_bShadowTestInverted)?"Inverted":"Regular");
        m_pFont->DrawText (2, 62, D3DCOLOR_ARGB(255,255,255,0),		buff);
        sprintf (buff, "Near=%.3f, Far=%.3f, ppNear=%.5f, ppFar=%.5f, slideBack=%.3f, LiSPSM(N)=%f", m_zNear, m_zFar, m_ppNear, m_ppFar, m_fSlideBack, m_fLiSPSM_N);	//. OLES:
        m_pFont->DrawText (2, 82, D3DCOLOR_ARGB(255,255,255,0),		buff);
        sprintf (buff, "Unit Cube Clipping:  %s", (m_bUnitCubeClip)?"Enabled":"Disabled");
        m_pFont->DrawText (2, 102, D3DCOLOR_ARGB(255,255,255,0),	buff);
        sprintf (buff, "Virtual Slide Back:  %s", (m_bSlideBack)?"Enabled":"Disabled");
        m_pFont->DrawText (2, 122, D3DCOLOR_ARGB(255,255,255,0),	buff);
    }
}