//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hModule, LPSTR, INT )
{
    CPracticalPSM d3dApp;

    g_hInstance = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CPracticalPSM::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch( uMsg )
    {
    case WM_CHAR:
        if (m_SelectedUI) m_SelectedUI->KeyDown((DWORD)wParam);
        HandleKey((DWORD)wParam, 0);
        break;

    case WM_KEYDOWN:
        if (m_SelectedUI) m_SelectedUI->KeyDown((DWORD)wParam);
        HandleKey((DWORD)wParam, 1);
        break;
        
    case WM_KEYUP:
        if (m_SelectedUI) m_SelectedUI->KeyUp((DWORD)wParam);
        break;

    case WM_MOUSEMOVE:
        if (m_SelectedUI) m_SelectedUI->OnMouseMove(LOWORD(lParam), HIWORD(lParam));
        break;
    
    case WM_LBUTTONDOWN:
        if (m_SelectedUI) m_SelectedUI->OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
        break;
    
    case WM_LBUTTONUP:
        if (m_SelectedUI) m_SelectedUI->OnLButtonUp(LOWORD(lParam), HIWORD(lParam));
        break;

    default:
        break;
    }

    // Pass remaining messages to default handler
    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CPracticalPSM::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
    static int nErrors = 0;     // use this to only show the very first error messagebox
    int nPrevErrors = nErrors;

    // check vertex shading support
    if(pCaps->VertexShaderVersion < D3DVS_VERSION(1,1))
        if (!nErrors++) 
            MessageBox(NULL, "Device does not support 1.1 vertex shaders!", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);

    // check pixel shader support
    if(pCaps->PixelShaderVersion < D3DPS_VERSION(1,1))
        if (!nErrors++) 
            MessageBox(NULL, "Device does not support 1.1 pixel shaders!", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
    
    m_bSupportsPixelShaders20 = (pCaps->PixelShaderVersion >= D3DPS_VERSION(2,0));
        

    if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_MIPMAP))
        if (!nErrors++) 
            MessageBox(NULL, "Device does not support mipmaps!", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
    
    if(pCaps->MaxSimultaneousTextures < 2)
        if (!nErrors++) 
            MessageBox(NULL, "Device does not support two simultaneous textures!", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);

    if(!(pCaps->RasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS))
        if (!nErrors++) 
            MessageBox(NULL, "Device does not support slope scale depth bias!", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);

    return (nErrors > nPrevErrors) ? E_FAIL : S_OK;
}

//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.  Nothing DX9-related should be allocated
//       or created yet.
//-----------------------------------------------------------------------------
HRESULT CPracticalPSM::OneTimeSceneInit()
{
   
    // IMPORTANT: you must set this if your demo will be calling GetRenderState() at any point!  
    // Otherwise, the D3D app framework will create a pure device and GetRenderState will fail.
    //m_d3dSettings.Windowed_VertexProcessingType = HARDWARE_VP;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.  Clean up anything allocated/created in
//       OneTimeSceneInit() here.
//-----------------------------------------------------------------------------
HRESULT CPracticalPSM::FinalCleanup()
{
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.  Ideally, you want to initialize all DX9
//   objects that don't get lost on a device reset, here (and put the rest in
//   RestoreDeviceObjects()).  You can be lazy, though, and just put everything
//   in RestoreDeviceObjects().
//-----------------------------------------------------------------------------
HRESULT CPracticalPSM::InitDeviceObjects()
{
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  Basically,
//       here, clean up anything you created in InitDeviceObjects().
//-----------------------------------------------------------------------------
HRESULT CPracticalPSM::DeleteDeviceObjects()
{
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//  The device exists, but was just lost or reset, and is now being restored.  
//  Resources in D3DPOOL_DEFAULT and any other device state that persists during 
//  rendering should be set here.  Render states, matrices, textures, etc., that 
//  don't change during rendering can be set once here to avoid redundant state 
//  setting during Render(). 
//-----------------------------------------------------------------------------
HRESULT CPracticalPSM::RestoreDeviceObjects()
{
    HRESULT hr;

    assert(m_pd3dDevice);

    D3DFORMAT zFormat = D3DFMT_D24S8;
    m_bitDepth = 24;

    if(FAILED(CheckResourceFormatSupport(zFormat, D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL)))
    {
        MessageBox(NULL, "Device/driver does not support hardware shadow maps!", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        return E_FAIL;
    }

    //setup buffers
    if(FAILED(m_pd3dDevice->GetRenderTarget(0, &m_pBackBuffer)))
        return E_FAIL;
    if(FAILED(m_pd3dDevice->GetDepthStencilSurface(&m_pZBuffer)))
        return E_FAIL;

    if(!m_pBackBuffer || !m_pZBuffer)
        return E_FAIL;


    D3DFORMAT colorFormat = D3DFMT_A8R8G8B8;

    if(FAILED(m_pd3dDevice->CreateTexture(TEXDEPTH_WIDTH, TEXDEPTH_HEIGHT, 1, D3DUSAGE_RENDERTARGET, colorFormat, 
        D3DPOOL_DEFAULT, &m_pSMColorTexture, NULL)))
        return E_FAIL;
    if(FAILED(m_pd3dDevice->CreateTexture(TEXDEPTH_WIDTH, TEXDEPTH_HEIGHT, 1, D3DUSAGE_DEPTHSTENCIL, zFormat, 
        D3DPOOL_DEFAULT, &m_pSMZTexture, NULL)))
        return E_FAIL;
    if(!m_pSMColorTexture || !m_pSMZTexture )
        return E_FAIL;

    // Retrieve top-level surfaces of our shadow buffer (need these for use with SetRenderTarget)
    if(FAILED(m_pSMColorTexture->GetSurfaceLevel(0, &m_pSMColorSurface)))
        return E_FAIL;
    if(FAILED(m_pSMZTexture->GetSurfaceLevel(0, &m_pSMZSurface)))
        return E_FAIL;
    if(!m_pSMColorSurface || !m_pSMZSurface)
        return E_FAIL;

    //font setup
    m_pFont = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_pFont->InitDeviceObjects (m_pd3dDevice);
    m_pFont->RestoreDeviceObjects();

    // Assign registers to the relevant vertex attributes
    D3DVERTEXELEMENT9 declaration[] =
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, 
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },  
        D3DDECL_END()
    };

    m_pd3dDevice->CreateVertexDeclaration(declaration, &m_pDeclaration);

    const char* profileOpts[] = 
    {
        "-profileopts", "dcls", NULL,
    };

    DWORD tempFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX0;

    hr = CreateQuad(&m_smQuad);
    std::string fileNameRock("SambavaRockChunk.nvb");
    std::string fileNameClawBot("ClawBot.nvb");

    m_pRockChunk = new NVBScene;
    hr = m_pRockChunk->Load(fileNameRock, m_pd3dDevice, GetFilePath::GetFilePath);
    if(FAILED(hr))
        return hr;
    m_pClawBot = new NVBScene;
    
    if (!m_pRockChunk->m_VertexHasNormal) {
        MessageBox(NULL, "Model does not have normals", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        return E_FAIL;
    }

    hr = m_pClawBot->Load(fileNameClawBot, m_pd3dDevice, GetFilePath::GetFilePath);
    if(FAILED(hr))
        return hr;

    if (!m_pClawBot->m_VertexHasNormal) {
        MessageBox(NULL, "Model does not have normals", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        return E_FAIL;
    }
  
    //set render states
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
     
    // Load our Effect file
    // note: path is relative to MEDIA\ dir
    hr = D3DXCreateEffectFromFile(m_pd3dDevice, GetFilePath::GetFilePath("programs\\PracticalPSM\\PracticalPSM.fx").c_str(),
        NULL, NULL, 0, NULL, &m_pEffect, NULL);
    if (FAILED(hr))
    {
        MessageBox(NULL, "Failed to load effect file", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        return hr;
    }

    // Mouse UI
    RECT rect;
    rect.left = rect.top = 0;

    m_MouseFPSUI = new MouseFPSUI(m_hWnd);
    m_SelectedUI = m_MouseFPSUI;

    // Set view matrix (m_View) and apply to m_UICamera
    ResetCamera();
    RandomizeObjects();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc:
//  The device exists, but is about to be Reset(), so we should release some things. 
//  Resources in D3DPOOL_DEFAULT and any other device state that persists during 
//  rendering should be set here. Render states, matrices, textures, etc., that 
//  don't change during rendering can be set once here to avoid redundant state 
//  setting during Render(). 
//-----------------------------------------------------------------------------
HRESULT CPracticalPSM::InvalidateDeviceObjects()
{
    if (m_pFont)
    {
        m_pFont->InvalidateDeviceObjects();
        m_pFont->DeleteDeviceObjects();
    }
    SAFE_DELETE(m_pFont);

    SAFE_RELEASE(m_smQuad.pVB);
    SAFE_RELEASE(m_smQuad.pIB);

    SAFE_RELEASE(m_pSMColorTexture);
    SAFE_RELEASE(m_pSMZTexture);
    SAFE_RELEASE(m_pSMColorSurface);
    SAFE_RELEASE(m_pSMZSurface);

    SAFE_RELEASE(m_pBackBuffer);
    SAFE_RELEASE(m_pZBuffer);

    SAFE_RELEASE(m_pDeclaration);

    SAFE_DELETE_ARRAY(m_pAttributes);

    SAFE_DELETE(m_MouseFPSUI);

    SAFE_DELETE(m_pRockChunk);
    SAFE_DELETE(m_pClawBot);
    SAFE_RELEASE(m_pEffect);
    
    for (unsigned int i=0; i<m_Scenes.size(); i++)
    {
        delete m_Scenes[i];
    }
    m_Scenes.clear();

    // ...and CD3DApplication will release m_pd3dDevice for us.

    return S_OK;
}

HRESULT CPracticalPSM::CheckResourceFormatSupport(D3DFORMAT fmt, D3DRESOURCETYPE resType, DWORD dwUsage)
{
    HRESULT hr = S_OK;
    IDirect3D9* tempD3D = NULL;
    m_pd3dDevice->GetDirect3D(&tempD3D);
    D3DCAPS9 devCaps;
    m_pd3dDevice->GetDeviceCaps(&devCaps);
    
    D3DDISPLAYMODE displayMode;
    tempD3D->GetAdapterDisplayMode(devCaps.AdapterOrdinal, &displayMode);
    
    hr = tempD3D->CheckDeviceFormat(devCaps.AdapterOrdinal, devCaps.DeviceType, displayMode.Format, dwUsage, resType, fmt);
    
    tempD3D->Release(), tempD3D = NULL;
    
    return hr;
}


void CPracticalPSM::HandleKey(DWORD wParam, int bIsVirtualKey)
{  
    if (!bIsVirtualKey)
    {
        // regular alphanumeric keys; case is preserved, too
        switch(wParam)
        {
        case 'r':
        case 'R':
            RandomizeObjects();
            break;
            
        case 'c':
        case 'C':
            m_bUnitCubeClip = !m_bUnitCubeClip;
            break;            
            
        case 'b':
        case 'B':
            m_bSlideBack = !m_bSlideBack;
            break;
            
        case 'l':
        case 'L':
            m_bLightAnimation = !m_bLightAnimation;
            break;            

        case '`':
        case '~':
            m_bDisplayShadowMap = !m_bDisplayShadowMap;
            break;
            
        case ';':
        case ':':
            m_fMinInfinityZ -= 0.01f;
            break;
            
        case '\'':
        case '\"':
            m_fMinInfinityZ += .01f;
            break;
            
        case ',':
        case '<':
            m_fNumStdDevs -= 0.05f;
            break;
            
        case '.':
        case '>':
            m_fNumStdDevs += 0.05f;
            break;
            
        case '[':
        case '{':
            m_fBiasSlope -= 0.01f;
            break;
            
        case ']':
        case '}':
            m_fBiasSlope += 0.01f;
            break;
            
        case '-':
        case '_':
            m_iDepthBias -= 1;
            break;
            
        case '=':
        case '+':
            m_iDepthBias += 1;
            break;
        }
    }
    else
    {
        // non-alphanumeric keys; use virtual-key codes only.
        switch(wParam)
        {
        case VK_F1:
            m_bShowHelp = !m_bShowHelp;
            break;
        
        case VK_F4:
            m_bShowStats = !m_bShowStats;
            break;
        
        case VK_ESCAPE:
            if (!m_bWindowed) 
                ToggleFullscreen();
            else
                PostMessage(m_hWnd, WM_CLOSE, 0, 0);    
            break;

        case VK_HOME :          // reset scene/settings
            {
                ResetCamera();
                m_iDepthBias = 1;
                m_fBiasSlope = 1.0f;
            }
            break;

        case VK_SPACE:
            m_Paused = !m_Paused;
            break;
        }
    }
}

HRESULT CPracticalPSM::ResetCamera()
{
    if (!m_MouseFPSUI) return E_FAIL;
    
    m_MouseFPSUI->Reset();
    m_MouseFPSUI->SetTranslationalSensitivityFactor(9.5f);
    m_MouseFPSUI->SetTranslate(0.f, -8.f, 0.f);
    return S_OK;
}

void CPracticalPSM::RandomizeObjects()
{
    unsigned int i;
    for (i=0; i<m_Scenes.size(); i++)
    {
        delete m_Scenes[i];
    }
    m_Scenes.clear();
       
    for (i=0; i<NUM_OBJECTS; i++)
    {
        int objectType = rand() & 1;
        ObjectInstance* cbInstance = new ObjectInstance;
        cbInstance->scene = (objectType)?m_pClawBot : m_pRockChunk;
        cbInstance->translation.x = (float)(rand()%1600)-800;
        cbInstance->translation.z = (float)(rand()%1600)-800;
        cbInstance->translation.y = (float)((objectType)?-10:-5);
        cbInstance->aabb = (objectType)?&m_ClawBotLocalBB : &m_RockChunkLocalBB;
        m_Scenes.push_back(cbInstance);
    }
}