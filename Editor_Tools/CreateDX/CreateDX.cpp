// CreateDX.cpp : Defines the entry point for the DLL application.
//

#include	"stdafx.h"
#include	"d3dx.h"
#include	"dds.h"

__declspec(thread) HWND					m_hWnd		= 0;
__declspec(thread) ID3DXContext*        m_pD3DX;

struct strDX{
	LPDIRECT3D7				pD3D;
	LPDIRECT3DDEVICE7		pD3DDev;
	LPDIRECTDRAW7			pDD;
	LPDIRECTDRAWSURFACE7	pBackBuffer;
	DDSURFACEDESC2			BackDesc;
};

__declspec(thread) strDX*	m_dx;

__declspec(thread) bool		bInScene	= false;

HRESULT						InitRenderer();
HRESULT						HandleWindowedModeChanges();

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" __declspec(dllexport) 
HRESULT CreateTexture(LPDIRECTDRAWSURFACE7& pTexture, LPSTR pSrcName, LPSTR pAlternateSrcName, int* w, int* h, BOOL* bAlpha){
	HRESULT hr;

	DDS_HEADER HDR;
	try{
		CFileStream F(pSrcName);
		F.Rdword();
		F.Read(&HDR,sizeof(DDS_HEADER));
	}catch(...){
		return D3DERR_TEXTURE_LOAD_FAILED;
	}

	DWORD dwFlag = HDR.dwMipMapCount?0:D3DX_TEXTURE_NOMIPMAP;
	DWORD mip_num= 0;
	D3DX_SURFACEFORMAT fmt = D3DXMakeSurfaceFormat((DDPIXELFORMAT*)&HDR.ddspf); 	

	hr = D3DXCreateTexture(m_dx->pD3DDev,&dwFlag,LPDWORD(w),LPDWORD(h),&fmt,0,&pTexture,&mip_num);
	hr = D3DXLoadTextureFromFile(m_dx->pD3DDev,pTexture,  D3DX_DEFAULT, pSrcName,0,0,D3DX_FT_LINEAR);
	if (FAILED(hr)){
		hr = D3DXLoadTextureFromFile(m_dx->pD3DDev,pTexture,HDR.dwMipMapCount?D3DX_DEFAULT:0,pAlternateSrcName,0,0,D3DX_FT_LINEAR);
	}


//	hr=D3DXCreateTextureFromFile(	m_dx->pD3DDev, HDR.dwMipMapCount?0:&dwFlag, (DWORD*)w, (DWORD*)h, &fmt, 0, 
//										&pTexture, &mip_num, pSrcName, D3DX_FT_LINEAR );
	if (!FAILED(hr))
	{
		switch (fmt){
		case D3DX_SF_A8R8G8B8:
		case D3DX_SF_A1R5G5B5:
		case D3DX_SF_A4R4G4B4:
		case D3DX_SF_A8L8:
		case D3DX_SF_A8:
		case D3DX_SF_DXT3:
		case D3DX_SF_DXT5:
			*bAlpha = TRUE; break;
		default: *bAlpha=FALSE;
		}
	}

	return hr;
}

extern "C" __declspec(dllexport) 
HRESULT LoadTexture(LPDIRECTDRAWSURFACE7& pTexture, LPSTR pSrcName){
	HRESULT hr=D3DXLoadTextureFromFile(m_dx->pD3DDev, pTexture, D3DX_DEFAULT, pSrcName, 0, 0, D3DX_FT_DEFAULT);
	return hr;
}

extern "C" __declspec(dllexport) 
void ReleaseTexture(LPDIRECTDRAWSURFACE7& pTexture){
	_RELEASE(pTexture);
}

extern "C" __declspec(dllexport) LPCSTR InterpretError(HRESULT hr)
{
    static char errStr[1024];
    D3DXGetErrorString(hr, 1024, errStr );
	return errStr;
}

extern "C" __declspec(dllexport) HRESULT ReleaseD3DX()
{
    _RELEASE(m_dx->pDD);
    _RELEASE(m_dx->pD3D);
    _RELEASE(m_dx->pD3DDev);
    _RELEASE(m_pD3DX);
    _RELEASE(m_dx->pBackBuffer);
    D3DXUninitialize();
	delete(m_dx);
    return S_OK;
}

static DWORD m_Mode=D3DX_DEFAULT;

extern "C" __declspec(dllexport) HRESULT InitD3DX(HWND hWnd, strDX** dx, DWORD mode)
{
	m_dx				= new strDX;
	m_dx->pD3D			= NULL;
	m_dx->pD3DDev		= NULL;
	m_dx->pDD			= NULL;
	m_dx->pBackBuffer	= NULL;
	m_pD3DX				= NULL;

	m_hWnd = hWnd;

	m_Mode				= mode;

    HRESULT hr;
    if( FAILED(hr = D3DXInitialize()) )
        return hr;

    hr = D3DXCreateContext( m_Mode,	// D3DX handle
                            0,		// flags
                            m_hWnd,
                            D3DX_DEFAULT, // colorbits
                            D3DX_DEFAULT, // numdepthbits
                            &m_pD3DX // returned D3DX interface
                            );
    if( FAILED(hr) )        return hr;
	hr = InitRenderer();
    if( FAILED(hr) )        return hr;
	if (dx)(*dx) = m_dx;
    return S_OK;
}

HRESULT InitRenderer()
{
    HRESULT hr;

    m_dx->pD3DDev = m_pD3DX->GetD3DDevice();
    if( m_dx->pD3DDev == NULL )
        return E_FAIL;

	m_dx->pD3D = m_pD3DX->GetD3D();
    if( m_dx->pD3D == NULL )
        return E_FAIL;

    m_dx->pDD = m_pD3DX->GetDD();
    if( m_dx->pDD == NULL )
        return E_FAIL;

	m_dx->pDD->SetCooperativeLevel(m_hWnd,DDSCL_FPUPRESERVE|DDSCL_MULTITHREADED|DDSCL_NORMAL);

	m_dx->pBackBuffer = m_pD3DX->GetBackBuffer(0);
    if( m_dx->pBackBuffer == NULL )
        return E_FAIL;

	memset( &m_dx->BackDesc, 0, sizeof(m_dx->BackDesc) );
	m_dx->BackDesc.dwSize = sizeof(m_dx->BackDesc);
	hr = m_dx->pBackBuffer->GetSurfaceDesc( &m_dx->BackDesc );
    if ( FAILED(hr) )
        return hr;

    hr = m_dx->pD3DDev->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
    if ( FAILED(hr) )
        return hr;

    hr = m_dx->pD3DDev->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_CCW );
    if ( FAILED(hr) )
        return hr;

    // let's make the background non-black.
    hr = m_pD3DX->SetClearColor(0x00555555);
    if( FAILED(hr) )
        return hr;

    hr = m_pD3DX->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
    if ( FAILED(hr) )
        return hr;

    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT BeginDraw()
{
	HRESULT hr = 0;

    hr = m_dx->pD3DDev->BeginScene();
    if( SUCCEEDED( hr ) )
    {
		bInScene = true;
        m_pD3DX->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
    }
	return hr;
}

extern "C" __declspec(dllexport) HRESULT EndDraw()
{
	if (bInScene){
		HRESULT hr = 0;
		hr = m_dx->pD3DDev->EndScene();

		bInScene = false;
		hr = m_pD3DX->UpdateFrame( 0 );
		if ( hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY ){
			hr = HandleWindowedModeChanges();
			InterpretError(hr);
		}
		return hr;
	}else return E_FAIL;
}


HRESULT HandleWindowedModeChanges()
{
    HRESULT hr;
    hr = m_dx->pDD->TestCooperativeLevel();

    if( SUCCEEDED( hr ) )
    {
        // This means that mode changes had taken place, surfaces
        // were lost but still we are in the original mode, so we
        // simply restore all surfaces and keep going.
        if( FAILED( m_dx->pDD->RestoreAllSurfaces() ) )
            return hr;
    }
    else if( hr == DDERR_WRONGMODE )
    {
        // This means that the desktop mode has changed
        // we can destroy and recreate everything back again.
        if(FAILED(hr = ReleaseD3DX()))
            return hr;
        if(FAILED(hr = InitD3DX(m_hWnd, 0, m_Mode)))
            return hr;
    }
    else if( hr == DDERR_EXCLUSIVEMODEALREADYSET )
    {
        // This means that some app took exclusive mode access
        // we need to sit in a loop till we get back to the right mode.
        do
        {
            Sleep( 500 );
        } while( DDERR_EXCLUSIVEMODEALREADYSET == 
                 (hr = m_dx->pDD->TestCooperativeLevel()) );
        if( SUCCEEDED( hr ) )
        {
            // This means that the exclusive mode app relinquished its 
            // control and we are back to the safe mode, so simply restore
            if( FAILED( m_dx->pDD->RestoreAllSurfaces() ) )
                return hr;
        }
        else if( DDERR_WRONGMODE == hr )
        {
            // This means that the exclusive mode app relinquished its 
            // control BUT we are back to some strange mode, so destroy
            // and recreate.
            if(FAILED(hr = ReleaseD3DX()))
                return hr;
			if(FAILED(hr = InitD3DX(m_hWnd, 0, m_Mode)))
                return hr;
        }
        else
        {
            // Busted!!
            return hr;
        }
    }
    else
    {
        // Busted!!
        return hr;
    }
    return S_OK;
}


extern "C" __declspec(dllexport) HRESULT ResizeD3DX(int X, int Y)
{
    _RELEASE(m_dx->pDD);
    _RELEASE(m_dx->pD3D);
    _RELEASE(m_dx->pD3DDev);
    _RELEASE(m_dx->pBackBuffer);
	HRESULT hr;
    if (FAILED(hr = m_pD3DX->Resize(X,Y))) return hr;
	hr = InitRenderer();
	return hr;
}

// misc
__declspec( dllimport ) bool WINAPI FSColorPickerDoModal(unsigned int * currentColor, unsigned int * originalColor, const int initialExpansionState);
extern "C" __declspec(dllexport) bool FSColorPickerExecute(LPDWORD currentColor, LPDWORD originalColor, const int initialExpansionState){
	return FSColorPickerDoModal((unsigned int*)currentColor, (unsigned int*)originalColor, initialExpansionState);
}
