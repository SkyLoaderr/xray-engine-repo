#include "stdafx.h"

#include "nvMeshMender.h"

#include "r_shader.h"
#include "r_constants_cache.h"


enum COMBINE_MODE
{
	CM_NORMAL,
	CM_DBG_NORMALS,
	CM_DBG_ACCUMULATOR,
	CM_DBG_BASE
};
//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------

#define SHADOW_MAP_FORMAT	D3DFMT_R32F
#define SHADOW_MAP_SIZE		1024
#define OUT_WIDTH			800
#define OUT_HEIGHT			600
#define OVERLAY_SIZE		128
#define DEPTH_RANGE			4.0f

// 2D
struct TVERTEX
{
	D3DXVECTOR4 p;
	FLOAT       tu, tv;
};
#define TVERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_TEX1)
D3DVERTEXELEMENT9 decl_vert2D[] =
{
	{ 0, 0,		D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT,	0 },
	{ 0, 16,	D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
	D3DDECL_END()
};

// 3D
struct meshVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	FLOAT       tu, tv;
};
struct VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	FLOAT       tu, tv;
	D3DXVECTOR3	tangent;
	D3DXVECTOR3	binormal;
};
D3DVERTEXELEMENT9 decl_vert[] =
{
	{ 0, 0,		D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
	{ 0, 12,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0 },
	{ 0, 24,	D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
	{ 0, 32,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0 },
	{ 0, 44,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL,	0 },
	D3DDECL_END()
};

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Main class to run this application. Most functionality is inherited
//       from the CD3DApplication base class.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	//  ************************
	//	**** Generic stuff  ****
	//  ************************

	// Font for drawing text
	CD3DFont*						m_pFont;
	CD3DArcBall						m_ArcBall;

	// Model
	LPDIRECT3DVERTEXBUFFER9			m_pModelVB;
	LPDIRECT3DINDEXBUFFER9			m_pModelIB;
	DWORD							m_dwModelNumVerts;
	DWORD							m_dwModelNumFaces;
	FLOAT							m_fModelSize;

	LPDIRECT3DVERTEXBUFFER9			m_pFloorVB;
	LPDIRECT3DVERTEXBUFFER9			m_pQuadVB;
	LPDIRECT3DVERTEXBUFFER9			m_pOverlayVB;

	LPDIRECT3DVERTEXDECLARATION9	m_pDeclVert;
	LPDIRECT3DVERTEXDECLARATION9	m_pDeclVert2D;

	// xr2
	R_constants						cc;

	//  ************************
	//	**** Deferred shading **
	//  ************************

	// Surfaces
	LPDIRECT3DTEXTURE9				d_Position;		// 64bit, fat	(x,y,z,0)				(eye-space)
	LPDIRECT3DSURFACE9				d_Position_S;
	LPDIRECT3DTEXTURE9				d_Normal;		// 64bit, fat	(x,y,z,0)				(eye-space)
	LPDIRECT3DSURFACE9				d_Normal_S;
	LPDIRECT3DTEXTURE9				d_Color;		// 64bit, fat	(r,g,b,specular-gloss)
	LPDIRECT3DSURFACE9				d_Color_S;
	LPDIRECT3DTEXTURE9				d_Accumulator;	// 32bit		(r,g,b,specular)
	LPDIRECT3DSURFACE9				d_Accumulator_S;

	// Shadow map
	LPDIRECT3DTEXTURE9				sm_ShadowMap;
	LPDIRECT3DSURFACE9				sm_ShadowMap_S;
	LPDIRECT3DSURFACE9				sm_ShadowMap_Z;
	LPDIRECT3DVERTEXBUFFER9			sm_OverlayVB;

	// Special textures
	LPDIRECT3DTEXTURE9				t_SpecularPower_32;
	LPDIRECT3DCUBETEXTURE9			t_NCM;
	LPDIRECT3DTEXTURE9				t_Base;
	LPDIRECT3DTEXTURE9				t_Normals;

	// Matrices
	D3DXMATRIX						dm_model2world;
	D3DXMATRIX						dm_model2world2view;
	D3DXMATRIX						dm_model2world2view2projection;

	D3DXMATRIX						dm_2world;
	D3DXMATRIX						dm_2view;
	D3DXMATRIX						dm_2projection;

	D3DXMATRIX						dm_world2view2projection;

	D3DXVECTOR3						dv_LightDir;
	D3DXMATRIX						dm_model2world2view2projection_smap;
	D3DXMATRIX						dm_model2world2view2projection_light;

	// Shaders
	R_shader						s_Scene2fat_base;
	R_shader						s_Scene2fat_bump;
	R_shader						s_Scene2smap_direct;
	R_shader						s_Combine_Normal;
	R_shader						s_CombineDBG_Normals;
	R_shader						s_CombineDBG_Accumulator;
	R_shader						s_CombineDBG_Base;
	R_shader						s_Light_Direct;
	R_shader						s_Light_Direct_smap;

	//  ************************
	//	**** Shadow mapping ****
	//  ************************
public:
	CMyD3DApplication();

	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT Render			();
	HRESULT FrameMove		();
	HRESULT FinalCleanup	();
	HRESULT ConfirmDevice	(D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT Format);

	// Shadow mapping-base
	HRESULT RenderShadowMap	();
	HRESULT RenderScene		();
	HRESULT RenderOverlay	();

	HRESULT RenderFAT					();
	HRESULT RenderLight_Direct			();
	HRESULT RenderLight_Direct_smap		();
	HRESULT RenderCombine				(COMBINE_MODE M);
	HRESULT RenderCombine_Normal		();
	HRESULT RenderCombineDBG_Normals	();
	HRESULT RenderCombineDBG_Accumulator();
	HRESULT RenderCombineDBG_Base		();

	HRESULT UpdateTransform	();

	LRESULT MsgProc			(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

CMyD3DApplication g_d3dApp;

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	InitCommonControls	();
	InitMath			();
	if (FAILED(g_d3dApp.Create(hInst)))
		return 0;

	return g_d3dApp.Run();
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Constructor
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
	// Override base class members
	m_strWindowTitle					= _T	("xray2 : render");
	m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
	m_d3dEnumeration.AppMinStencilBits	= 1;
	// m_d3dEnumeration.AppRequiresFullscreen = TRUE;
	m_dwCreationWidth					= OUT_WIDTH;
	m_dwCreationHeight					= OUT_HEIGHT;

	m_pFont								= new CD3DFont(_T("Arial"), 12, D3DFONT_BOLD);

	m_pModelVB							= NULL;
	m_pModelIB							= NULL;
	m_pFloorVB							= NULL;

	sm_ShadowMap						= NULL;
	sm_ShadowMap_S						= NULL;
	sm_ShadowMap_Z						= NULL;

	m_pOverlayVB						= NULL;

	// d-render
	d_Position							= NULL;
	d_Position_S						= NULL;
	d_Normal							= NULL;
	d_Normal_S							= NULL;
	d_Color								= NULL;
	d_Color_S							= NULL;
	d_Accumulator						= NULL;
	d_Accumulator_S						= NULL;
}


HRESULT CMyD3DApplication::OneTimeSceneInit()	{ return S_OK; }

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove	()
{
	UpdateTransform	();
	return			S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render		()
{
	// Begin the scene
	if (SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
		// RenderShadowMap	();
		// RenderScene			();
		// RenderOverlay		();

		RenderFAT					();
		RenderShadowMap				();
		RenderLight_Direct			();
		RenderCombine				(CM_DBG_ACCUMULATOR);
		// RenderOverlay				();

		// Output statistics
		m_pFont->DrawText			(OVERLAY_SIZE + 12,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats);
		m_pFont->DrawText			(OVERLAY_SIZE + 12, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats);

		// End the scene.
		m_pd3dDevice->EndScene		();
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize device-dependent objects. This is the place to create mesh
//       and texture objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
	VERTEX*					pDst;
	TVERTEX*				pDstT;
	CD3DMesh                Mesh;
	FLOAT					fModelRad;

	// Initialize the font's internal textures
	m_pFont->InitDeviceObjects	(m_pd3dDevice);

	// Load model
	if (FAILED(Mesh.Create(m_pd3dDevice, _T("media\\star.x"))))			return D3DAPPERR_MEDIANOTFOUND;

	// Fix vertex contents
	Mesh.SetFVF			(m_pd3dDevice, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

	// ************************************* Perform mungle
	D3DXVECTOR3			vecModelCenter;
	{
		unsigned int i;

		// Retrieve vertices and texture coordinates from the mesh vertex buffer
		LPD3DXMESH gMesh	= Mesh.GetSysMemMesh();
		u32 numVertices		= gMesh->GetNumVertices();
		meshVERTEX*			vertexBuffer;
		std::vector<float>	position;
		std::vector<float>	texCoord;
		std::vector<float>	normals;
		gMesh->LockVertexBuffer	(D3DLOCK_READONLY, (VOID**)&vertexBuffer);
		for (i = 0; i < numVertices; ++i) {
			position.push_back	(vertexBuffer[i].p.x);
			position.push_back	(vertexBuffer[i].p.y);
			position.push_back	(vertexBuffer[i].p.z);
			normals.push_back	(vertexBuffer[i].n.x);
			normals.push_back	(vertexBuffer[i].n.y);
			normals.push_back	(vertexBuffer[i].n.z);
			texCoord.push_back	(vertexBuffer[i].tu);
			texCoord.push_back	(vertexBuffer[i].tv);
			texCoord.push_back	(0);
		}
		gMesh->UnlockVertexBuffer();

		// Retrieve triangle indices from the index buffer
		u32 numTriangles	= gMesh->GetNumFaces();
		WORD (*indexBuffer)[3];
		std::vector<int> index;
		HRESULT hr = gMesh->LockIndexBuffer(D3DLOCK_READONLY, (VOID**)&indexBuffer);
		for (i = 0; i < numTriangles; ++i) {
			index.push_back(indexBuffer[i][0]);
			index.push_back(indexBuffer[i][1]);
			index.push_back(indexBuffer[i][2]);
		}
		gMesh->UnlockIndexBuffer();

		// gMesh->Release();

		// Prepare the parameters to the mesh mender

		// Fill in the input to the mesh mender
		// Positions
		NVMeshMender::VertexAttribute positionAtt;
		positionAtt.Name_			= "position";
		positionAtt.floatVector_	= position;
		// Normals
		NVMeshMender::VertexAttribute normalAtt;
		normalAtt.Name_				= "normal";
		normalAtt.floatVector_		= normals;
		// Texture coordinates
		NVMeshMender::VertexAttribute texCoordAtt;
		texCoordAtt.Name_			= "tex0";
		texCoordAtt.floatVector_	= texCoord;
		// Indices
		NVMeshMender::VertexAttribute indexAtt;
		indexAtt.Name_				= "indices";
		indexAtt.intVector_			= index;

		// Fill in input list
		std::vector<NVMeshMender::VertexAttribute> inputAtts;
		inputAtts.push_back(positionAtt);
		inputAtts.push_back(normalAtt);
		inputAtts.push_back(indexAtt);
		inputAtts.push_back(texCoordAtt);

		// Specify the requested output
		// Tangents
		NVMeshMender::VertexAttribute tangentAtt;
		tangentAtt.Name_ = "tangent";
		// Binormals
		NVMeshMender::VertexAttribute binormalAtt;
		binormalAtt.Name_ = "binormal";
		// Fill in output list
		std::vector<NVMeshMender::VertexAttribute> outputAtts;
		unsigned int n = 0;
		outputAtts.push_back(positionAtt); ++n;
		outputAtts.push_back(indexAtt); ++n;
		outputAtts.push_back(texCoordAtt); ++n;
		outputAtts.push_back(tangentAtt); ++n;
		outputAtts.push_back(binormalAtt); ++n;
		outputAtts.push_back(normalAtt); ++n;

		// Mender!!!!
		NVMeshMender mender;
		if (!mender.Munge(
			inputAtts,									// input attributes
			outputAtts,									// outputs attributes
			3.141592654f / 3.0f,						// tangent space smooth angle
			0,											// no texture matrix applied to my texture coordinates
			NVMeshMender::FixTangents,					// fix degenerate bases & texture mirroring
			NVMeshMender::DontFixCylindricalTexGen,			// handle cylindrically mapped textures via vertex duplication
			NVMeshMender::DontWeightNormalsByFaceSize	// weigh vertex normals by the triangle's size
			))
		{
			fprintf(stderr, "NVMeshMender failed\n");
			exit(-1);
		}

		// Retrieve outputs
		--n; std::vector<float> normal = outputAtts[n].floatVector_;
		--n; std::vector<float> binormal = outputAtts[n].floatVector_;
		--n; std::vector<float> tangent = outputAtts[n].floatVector_;
		--n; texCoord = outputAtts[n].floatVector_;
		--n; index = outputAtts[n].intVector_;
		--n; position = outputAtts[n].floatVector_;

		// Create the new vertex buffer
		m_dwModelNumVerts = position.size() / 3;
		D3DXComputeBoundingSphere((D3DXVECTOR3*)&position.front(), m_dwModelNumVerts, 3*sizeof(float), &vecModelCenter, &fModelRad);
		m_fModelSize = fModelRad * 2.0f;

		u32 size = m_dwModelNumVerts * sizeof(VERTEX);
		m_pd3dDevice->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pModelVB, NULL);
		VERTEX* vertexBufferNew;
		m_pModelVB->Lock	(0, 0, (void**)&vertexBufferNew, 0);
		for (i = 0; i < m_dwModelNumVerts; ++i) {
			vertexBufferNew[i].p.x			= position[3 * i + 0];
			vertexBufferNew[i].p.y			= position[3 * i + 1];
			vertexBufferNew[i].p.z			= position[3 * i + 2];
			vertexBufferNew[i].tu			= texCoord[3 * i + 0];
			vertexBufferNew[i].tv			= texCoord[3 * i + 1];

			D3DXVECTOR3	N,T,B;
			N.x								= normal[3 * i + 0];
			N.y								= normal[3 * i + 1];
			N.z								= normal[3 * i + 2];
			D3DXVec3Normalize				(&N,&N);
			T.x								= tangent[3 * i + 0];
			T.y								= tangent[3 * i + 1];
			T.z								= tangent[3 * i + 2];
			D3DXVec3Normalize				(&T,&T);
			B.x								= binormal[3 * i + 0];
			B.y								= binormal[3 * i + 1];
			B.z								= binormal[3 * i + 2];
			D3DXVec3Normalize				(&B,&B);

			/*
			// ortho-normalize
//			D3DXVec3Cross					(&B,&T,&N);
//			D3DXVec3Normalize				(&B,&B);
//			D3DXVec3Cross					(&T,&N,&B);
//			D3DXVec3Normalize				(&T,&T);
			B = -B;
			D3DXVec3Cross					(&T,&B,&N);
			D3DXVec3Normalize				(&T,&T);
			D3DXVec3Cross					(&B,&N,&T);
			D3DXVec3Normalize				(&B,&B);
			*/

			vertexBufferNew[i].n.x			= N.x;
			vertexBufferNew[i].n.y			= N.y;
			vertexBufferNew[i].n.z			= N.z;
			vertexBufferNew[i].tangent.x	= T.x;
			vertexBufferNew[i].tangent.y	= T.y;
			vertexBufferNew[i].tangent.z	= T.z;
			vertexBufferNew[i].binormal.x	= B.x;
			vertexBufferNew[i].binormal.y	= B.y;
			vertexBufferNew[i].binormal.z	= B.z;
		}
		m_pModelVB->Unlock();

		// Create the new index buffer
		m_dwModelNumFaces	= index.size() / 3;
		size				= m_dwModelNumFaces * 3 * sizeof(WORD);
		m_pd3dDevice->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pModelIB, NULL);
		m_pModelIB->Lock	(0, 0, (void**)&indexBuffer, 0);
		for (i = 0; i < m_dwModelNumFaces; ++i) {
			indexBuffer[i][0] = index[3 * i + 0];
			indexBuffer[i][1] = index[3 * i + 1];
			indexBuffer[i][2] = index[3 * i + 2];
		}
		m_pModelIB->Unlock	();
	}

	// Create floor VB
	m_pd3dDevice->CreateVertexBuffer(4 * sizeof(VERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pFloorVB, NULL);
	m_pFloorVB->Lock(0, 0, (void**)&pDst, 0);
	pDst[0].p = D3DXVECTOR3(-10.0f, 0.0f, 10.0f);
	pDst[0].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pDst[0].tu = 0.0f;
	pDst[0].tv = 1.0f;
	pDst[1].p = D3DXVECTOR3(10.0f, 0.0f, 10.0f);
	pDst[1].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pDst[1].tu = 1.0f;
	pDst[1].tv = 1.0f;
	pDst[2].p = D3DXVECTOR3(-10.0f, 0.0f, -10.0f);
	pDst[2].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pDst[2].tu = 0.0f;
	pDst[2].tv = 0.0f;
	pDst[3].p = D3DXVECTOR3(10.0f, 0.0f, -10.0f);
	pDst[3].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pDst[3].tu = 1.0f;
	pDst[3].tv = 0.0f;
	m_pFloorVB->Unlock();

	// Create overlay VB
	m_pd3dDevice->CreateVertexBuffer(4 * sizeof(TVERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pOverlayVB, NULL);
	m_pOverlayVB->Lock(0, 0, (void**)&pDstT, 0);
	pDstT[0].p = D3DXVECTOR4(4.5f, 4.5f + OVERLAY_SIZE, 0.0f, 1.0f);
	pDstT[0].tu = 0.0f;
	pDstT[0].tv = 1.0f;
	pDstT[1].p = D3DXVECTOR4(4.5f + OVERLAY_SIZE, 4.5f + OVERLAY_SIZE, 0.0f, 1.0f);
	pDstT[1].tu = 1.0f;
	pDstT[1].tv = 1.0f;
	pDstT[2].p = D3DXVECTOR4(4.5f, 4.5f, 0.0f, 1.0f);
	pDstT[2].tu = 0.0f;
	pDstT[2].tv = 0.0f;
	pDstT[3].p = D3DXVECTOR4(4.5f + OVERLAY_SIZE, 4.5f, 0.0f, 1.0f);
	pDstT[3].tu = 1.0f;
	pDstT[3].tv = 0.0f;
	m_pOverlayVB->Unlock();

	// Create overlay VB
	{
		const float	 w	= float(m_d3dsdBackBuffer.Width),	h = float(m_d3dsdBackBuffer.Height);
		const float _w	= w-1, _h = h-1;
		m_pd3dDevice->CreateVertexBuffer	(4 * sizeof(TVERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pQuadVB, NULL);
		m_pQuadVB->Lock						(0, 0, (void**)&pDstT, 0);
		pDstT[0].p	= D3DXVECTOR4(0, _h,	0.001f, 1.0f);
		pDstT[0].tu = 0.0f;
		pDstT[0].tv = 1.0f;
		pDstT[1].p	= D3DXVECTOR4(_w, _h,	0.001f, 1.0f);
		pDstT[1].tu = 1.0f;
		pDstT[1].tv = 1.0f;
		pDstT[2].p	= D3DXVECTOR4(0, 0,		0.001f, 1.0f);
		pDstT[2].tu = 0.0f;
		pDstT[2].tv = 0.0f;
		pDstT[3].p	= D3DXVECTOR4(_w, 0,	0.001f, 1.0f);
		pDstT[3].tu = 1.0f;
		pDstT[3].tv = 0.0f;
		m_pQuadVB->Unlock					();
	}

	// Create shader declaration(s)
	if (FAILED(m_pd3dDevice->CreateVertexDeclaration(decl_vert, &m_pDeclVert)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateVertexDeclaration(decl_vert2D, &m_pDeclVert2D)))
		return E_FAIL;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Restore device-memory objects and state after a device is created or
//       resized.
//-----------------------------------------------------------------------------

HRESULT CreateRT(IDirect3DDevice9* D, DWORD w, DWORD h, D3DFORMAT f, LPDIRECT3DTEXTURE9* pT, LPDIRECT3DSURFACE9* pS)
{
	if (FAILED(D->CreateTexture(w,h, 1, D3DUSAGE_RENDERTARGET, f, D3DPOOL_DEFAULT, pT, NULL)))	return E_FAIL;
	if (FAILED((*pT)->GetSurfaceLevel(0, pS)))													return E_FAIL;
	return S_OK;
}

HRESULT CreateNCM(IDirect3DDevice9* D, DWORD w, LPDIRECT3DCUBETEXTURE9* pT)
{
	HRESULT hr;

	hr = D3DXCreateCubeTexture(D, w, 1, 0, D3DFMT_A16B16G16R16F, D3DPOOL_MANAGED, pT);
	if(FAILED(hr))
	{
		return hr;
	}
	LPDIRECT3DCUBETEXTURE9 m_pCubeTexture	= *pT;

	for (int i = 0; i < 6; i++)
	{
		D3DLOCKED_RECT	Locked;
		D3DXVECTOR3		Normal;
		float			w,h;
		D3DSURFACE_DESC ddsdDesc;

		m_pCubeTexture->GetLevelDesc(0, &ddsdDesc);

		m_pCubeTexture->LockRect	((D3DCUBEMAP_FACES)i, 0, &Locked, 0, 0);

		for (u32 y = 0; y < ddsdDesc.Height; y++)
		{
			h = (float)y / ((float)(ddsdDesc.Height - 1));
			h *= 2.0f;
			h -= 1.0f;

			for (u32 x = 0; x < ddsdDesc.Width; x++)
			{
				w = (float)x / ((float)(ddsdDesc.Width - 1));
				w *= 2.0f;
				w -= 1.0f;

				D3DXFLOAT16* pBits = (D3DXFLOAT16*)((BYTE*)Locked.pBits + (y * Locked.Pitch));
				pBits		+= 4*x;

				switch((D3DCUBEMAP_FACES)i)
				{
				case D3DCUBEMAP_FACE_POSITIVE_X:
					Normal = D3DXVECTOR3(1.0f, -h, -w);
					break;
				case D3DCUBEMAP_FACE_NEGATIVE_X:
					Normal = D3DXVECTOR3(-1.0f, -h, w);
					break;
				case D3DCUBEMAP_FACE_POSITIVE_Y:
					Normal = D3DXVECTOR3(w, 1.0f, h);
					break;
				case D3DCUBEMAP_FACE_NEGATIVE_Y:
					Normal = D3DXVECTOR3(w, -1.0f, -h);
					break;
				case D3DCUBEMAP_FACE_POSITIVE_Z:
					Normal = D3DXVECTOR3(w, -h, 1.0f);
					break;
				case D3DCUBEMAP_FACE_NEGATIVE_Z:
					Normal = D3DXVECTOR3(-w, -h, -1.0f);
					break;
				default:
					break;
				}

				// Normalize and store
				D3DXVec3Normalize	(&Normal, &Normal);
				D3DXVECTOR4	data	= D3DXVECTOR4(Normal.x,Normal.y,Normal.z,0);
				D3DXFloat32To16Array(pBits,&data.x,4);
			}
		}
		m_pCubeTexture->UnlockRect((D3DCUBEMAP_FACES)i, 0);
	}

	return S_OK;
}

HRESULT CreatePower(IDirect3DDevice9* D, DWORD size, float P, LPDIRECT3DTEXTURE9* pT)
{
	std::vector<float>		array;
	array.resize			(size);
	for (DWORD it=0; it<size; it++)
	{
		float	v		= float(it)/float(size - 1);
		array	[it]	= powf		(v,P);
	}
	D->CreateTexture				(size,1, 1, 0, D3DFMT_R16F, D3DPOOL_MANAGED, pT, NULL);
	D3DLOCKED_RECT					R;
	LPDIRECT3DTEXTURE9 T			= *pT;
	T->LockRect						(0,&R,0, 0);
	D3DXFloat32To16Array			((D3DXFLOAT16*)R.pBits,array.begin(),size);
	T->UnlockRect					(0);

	return S_OK;
}

HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	m_pFont->RestoreDeviceObjects();

	UpdateTransform		();

	// Create targets
	DWORD				w			= m_d3dsdBackBuffer.Width, h = m_d3dsdBackBuffer.Height;
	CreateRT						(m_pd3dDevice,w,h,D3DFMT_A16B16G16R16F,	&d_Position,&d_Position_S);
	CreateRT						(m_pd3dDevice,w,h,D3DFMT_A16B16G16R16F,	&d_Normal,&d_Normal_S);
	CreateRT						(m_pd3dDevice,w,h,D3DFMT_A16B16G16R16,	&d_Color,&d_Color_S);
	CreateRT						(m_pd3dDevice,w,h,D3DFMT_A8R8G8B8,		&d_Accumulator,&d_Accumulator_S);
	CreateRT						(m_pd3dDevice,SHADOW_MAP_SIZE,SHADOW_MAP_SIZE,D3DFMT_R32F,&sm_ShadowMap,&sm_ShadowMap_S);

	// Create depth buffer for shadow map rendering
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &sm_ShadowMap_Z, NULL)))
		return E_FAIL;

	// Create shaders
	s_Scene2fat_base.compile		(m_pd3dDevice,"shaders\\D\\fat_base.s");
	s_Scene2fat_bump.compile		(m_pd3dDevice,"shaders\\D\\fat_bump.s");
	s_Scene2smap_direct.compile		(m_pd3dDevice,"shaders\\D\\smap_direct.s");
	s_Combine_Normal.compile		(m_pd3dDevice,"shaders\\D\\cm_normal.s");
	s_CombineDBG_Normals.compile	(m_pd3dDevice,"shaders\\D\\cm_dbg_normals.s");
	s_CombineDBG_Accumulator.compile(m_pd3dDevice,"shaders\\D\\cm_dbg_accumulator.s");
	s_CombineDBG_Base.compile		(m_pd3dDevice,"shaders\\D\\cm_dbg_base.s");
	s_Light_Direct.compile			(m_pd3dDevice,"shaders\\D\\light_direct.s");
	s_Light_Direct_smap.compile		(m_pd3dDevice,"shaders\\D\\light_direct_smap.s");

	// Create special textures
	LPDIRECT3DTEXTURE9				height	= 0;
	HRESULT							hr		= 0;
	hr = D3DXCreateTextureFromFile		(m_pd3dDevice,"media\\shadowmap.tga",&t_Base);
	hr = D3DXCreateTextureFromFileEx	(m_pd3dDevice,"media\\shadowmap_height.tga",
		D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_SCRATCH,D3DX_DEFAULT,D3DX_DEFAULT,0,NULL,NULL,&height);
	hr = D3DXCreateTexture				(m_pd3dDevice,512,512,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED, &t_Normals);
	hr = D3DXComputeNormalMap			(t_Normals,height,0,0,D3DX_CHANNEL_RED,8.f);

	// Transfer gloss-map
	{
		LPDIRECT3DTEXTURE9			tDest	= t_Normals;
		LPDIRECT3DTEXTURE9			tSrc	= height;
		DWORD mips							= tDest->GetLevelCount();

		for (DWORD i = 0; i < mips; i++)
		{
			D3DLOCKED_RECT				Rsrc,Rdst;
			D3DSURFACE_DESC				desc;

			tDest->GetLevelDesc			(i, &desc);

			tSrc->LockRect				(i,&Rsrc,0,0);
			tDest->LockRect				(i,&Rdst,0,0);

			for (u32 y = 0; y < desc.Height; y++)
			{
				for (u32 x = 0; x < desc.Width; x++)
				{
					DWORD&	pSrc	= *(((DWORD*)((BYTE*)Rsrc.pBits + (y * Rsrc.Pitch)))+x);
					DWORD&	pDst	= *(((DWORD*)((BYTE*)Rdst.pBits + (y * Rdst.Pitch)))+x);

					DWORD	mask	= DWORD(0xff) << DWORD(24);
					
					pDst			= (pDst& (~mask)) | (pSrc&mask);
				}
			}

			tDest->UnlockRect			(i);
			tSrc->UnlockRect			(i);
		}
	}
	height->Release						();

	// Power and Normalization Cube-Map
	hr = CreatePower					(m_pd3dDevice,256,32.f,&t_SpecularPower_32);
	hr = CreateNCM						(m_pd3dDevice,64,&t_NCM);

	m_ArcBall.SetWindow					(m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 1.0f);
	m_ArcBall.SetRadius					(3.0f);
	m_ArcBall.SetRightHanded			(FALSE);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	m_pFont->InvalidateDeviceObjects();

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	m_pFont->DeleteDeviceObjects();

	SAFE_RELEASE(m_pModelVB);
	SAFE_RELEASE(m_pModelIB);
	SAFE_RELEASE(m_pFloorVB);
	SAFE_RELEASE(m_pDeclVert);
	SAFE_RELEASE(m_pDeclVert2D);

	SAFE_RELEASE(m_pOverlayVB);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
	SAFE_DELETE(m_pFont);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
										 D3DFORMAT Format )
{
	BOOL	bDebug	= FALSE;

	if (bDebug)
	{
		if (dwBehavior & D3DCREATE_PUREDEVICE)					return E_FAIL;
		if (pCaps->DeviceType==D3DDEVTYPE_HAL)					return E_FAIL;
		// if ((dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0) return E_FAIL;
		return S_OK;
	} else {
		if (dwBehavior & D3DCREATE_PUREDEVICE)					return E_FAIL;
		if (pCaps->PixelShaderVersion < D3DPS_VERSION(2, 0))	return E_FAIL;
		if (pCaps->VertexShaderVersion < D3DVS_VERSION(2, 0))	return E_FAIL;
	}

	/*
	if ((dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0)
	{
	return E_FAIL;
	}

	// If device doesn't support 1.1 vertex shaders in HW, switch to SWVP.
	if (pCaps->VertexShaderVersion < D3DVS_VERSION(1, 1))
	{
	if ((dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0)
	{
	return E_FAIL;
	}
	}
	*/

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderShadowMap()
// Desc: Called to render shadow map for the light. Outputs per-pixel distance
//       normalized to 0..1 range to the floating point buffer.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderShadowMap	()
{
	LPDIRECT3DSURFACE9	oldBackBuffer, oldZBuffer;
	D3DVIEWPORT9		oldViewport;

	// Save old render taget
	m_pd3dDevice->GetRenderTarget			(0, &oldBackBuffer);
	m_pd3dDevice->GetDepthStencilSurface	(&oldZBuffer);
	m_pd3dDevice->GetViewport				(&oldViewport);

	// Set new render target
	m_pd3dDevice->SetRenderTarget			(0, sm_ShadowMap_S);
	m_pd3dDevice->SetDepthStencilSurface	(sm_ShadowMap_Z);

	// Setup shadow map viewport
	D3DVIEWPORT9	shadowViewport;
	shadowViewport.X						= 0;
	shadowViewport.Y						= 0;
	shadowViewport.Width					= SHADOW_MAP_SIZE;
	shadowViewport.Height					= SHADOW_MAP_SIZE;
	shadowViewport.MinZ						= 0.0f;
	shadowViewport.MaxZ						= 1.0f;
	m_pd3dDevice->SetViewport				(&shadowViewport);

	// Clear viewport
	m_pd3dDevice->Clear						(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0L);
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE, D3DCULL_CW);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_Scene2smap_direct.ps);
	m_pd3dDevice->SetVertexShader			(s_Scene2smap_direct.vs);
	m_pd3dDevice->SetVertexDeclaration		(m_pDeclVert);
	cc.set									(s_Scene2smap_direct.constants.get("m_model2view2projection"),	*((Fmatrix*)&dm_model2world2view2projection_smap));
	cc.flush								(m_pd3dDevice);

	// Render model
	m_pd3dDevice->SetStreamSource			(0, m_pModelVB, 0, sizeof(VERTEX));
	m_pd3dDevice->SetIndices				(m_pModelIB);
	m_pd3dDevice->DrawIndexedPrimitive		(D3DPT_TRIANGLELIST, 0, 0, m_dwModelNumVerts, 0, m_dwModelNumFaces);

	// Restore old render target
	m_pd3dDevice->SetRenderTarget			(0, oldBackBuffer);
	m_pd3dDevice->SetDepthStencilSurface	(oldZBuffer);
	oldBackBuffer->Release					();
	oldZBuffer->Release						();

	// Restore old viewport
	m_pd3dDevice->SetViewport				(&oldViewport);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderFAT()
// Desc: Renders fat-buffer
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderFAT	()
{
	LPDIRECT3DSURFACE9						pBaseTarget;

	// Set new render targets
	m_pd3dDevice->GetRenderTarget			(0, &pBaseTarget	);
	m_pd3dDevice->SetRenderTarget			(0, d_Position_S	);
	m_pd3dDevice->SetRenderTarget			(1, d_Normal_S		);
	m_pd3dDevice->SetRenderTarget			(2, d_Color_S		);
	m_pd3dDevice->Clear						(0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00, 1.0f, 0L);

	// samplers and texture
	m_pd3dDevice->SetTexture				(0, t_Base);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MIPFILTER,	D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR);

	// normal-map
	m_pd3dDevice->SetTexture				(1, t_Normals);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MIPFILTER,	D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR);

	// NCM
	/*
	m_pd3dDevice->SetTexture				(2, t_NCM);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_MIPFILTER,	D3DTEXF_NONE);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);
	*/

	// Setup stencil and culling
	m_pd3dDevice->SetRenderState			( D3DRS_CULLMODE,			D3DCULL_CCW			);

	//
	m_pd3dDevice->SetRenderState			( D3DRS_TWOSIDEDSTENCILMODE,FALSE				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILENABLE,		TRUE				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILFUNC,		D3DCMP_ALWAYS		);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILREF,			0x1					);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILMASK,		0xff				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILWRITEMASK,	0xff				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILPASS,		D3DSTENCILOP_REPLACE);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	);

	// Shader and params
	R_shader&	S							= s_Scene2fat_bump;
	m_pd3dDevice->SetPixelShader			(S.ps);
	m_pd3dDevice->SetVertexShader			(S.vs);
	m_pd3dDevice->SetVertexDeclaration		(m_pDeclVert);
	cc.set									(S.constants.get("m_model2view"),				*((Fmatrix*)&dm_model2world2view));
	cc.set									(S.constants.get("m_model2view2projection"),	*((Fmatrix*)&dm_model2world2view2projection));
	cc.flush								(m_pd3dDevice);

	// Render model
	m_pd3dDevice->SetStreamSource			(0, m_pModelVB, 0, sizeof(VERTEX));
	m_pd3dDevice->SetIndices				(m_pModelIB);
	m_pd3dDevice->DrawIndexedPrimitive		(D3DPT_TRIANGLELIST, 0, 0, m_dwModelNumVerts, 0, m_dwModelNumFaces);

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);
	m_pd3dDevice->SetTexture				(1, NULL);
	m_pd3dDevice->SetTexture				(2, NULL);
	m_pd3dDevice->SetRenderTarget			(0, pBaseTarget	);
	m_pd3dDevice->SetRenderTarget			(1, 0	);
	m_pd3dDevice->SetRenderTarget			(2, 0	);

	m_pd3dDevice->SetRenderState			( D3DRS_STENCILENABLE,		FALSE				);

	return S_OK;
}

HRESULT CMyD3DApplication::RenderCombine	(COMBINE_MODE M)
{
	if (M==CM_NORMAL)						return RenderCombine_Normal			();
	if (M==CM_DBG_NORMALS)					return RenderCombineDBG_Normals		();
	if (M==CM_DBG_ACCUMULATOR)				return RenderCombineDBG_Accumulator	();
	if (M==CM_DBG_BASE)						return RenderCombineDBG_Base		();

	return E_FAIL;
}

//-----------------------------------------------------------------------------
// Name: RenderLight_Direct				()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderLight_Direct	()
{
	LPDIRECT3DSURFACE9						pBaseTarget;

	// Set new render targets
	m_pd3dDevice->GetRenderTarget			(0, &pBaseTarget	);
	m_pd3dDevice->SetRenderTarget			(0, d_Accumulator_S	);
	m_pd3dDevice->Clear						(0L, NULL, D3DCLEAR_TARGET, 0x00, 1.0f, 0L);

	// samplers and texture (POS)
	m_pd3dDevice->SetTexture				(0, d_Position);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// samplers and texture (NORM)
	m_pd3dDevice->SetTexture				(1, d_Normal);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// samplers and texture (Power32)
	m_pd3dDevice->SetTexture				(2, t_SpecularPower_32);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR);

	// samplers and texture (NCM)
	/*
	m_pd3dDevice->SetTexture				(3, t_NCM);
	m_pd3dDevice->SetSamplerState			(3, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(3, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(3, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(3, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);
	*/

	// Set up the stencil states
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILENABLE,		TRUE				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILREF,			0x01				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILMASK,		0xff				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILWRITEMASK,	0x00				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_Light_Direct.ps);
	m_pd3dDevice->SetVertexShader			(s_Light_Direct.vs);
	m_pd3dDevice->SetFVF					(TVERTEX_FVF);
	D3DXVECTOR3 vLightDir					= dv_LightDir;
	D3DXMATRIX	mInvView;
	D3DXVec3Normalize						(&vLightDir, &vLightDir);
	D3DXMatrixInverse						(&mInvView,0,&dm_2view);
	D3DXVec3TransformNormal					(&vLightDir, &vLightDir,&mInvView);
	D3DXVec3Normalize						(&vLightDir, &vLightDir);
	cc.set									(s_Light_Direct.constants.get("light_direction"),	vLightDir.x,vLightDir.y,vLightDir.z,0	);
	cc.set									(s_Light_Direct.constants.get("light_color"),		.9f,		.9f,		1.,			1.0	);
	cc.flush								(m_pd3dDevice);

	// Blend mode - directional light comes first - means no blending
	m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, FALSE);

	// Render Quad
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE,	D3DCULL_NONE);
	m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

	// Second light
	m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState			(D3DRS_SRCBLEND,	D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState			(D3DRS_DESTBLEND,	D3DBLEND_ONE);
	cc.set									(s_Light_Direct.constants.get("light_direction"),	-vLightDir.x,-vLightDir.y,-vLightDir.z,0	);
	cc.set									(s_Light_Direct.constants.get("light_color"),		.9f,		.3f,		.0,			.7		);
	cc.flush								(m_pd3dDevice);
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);
	m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, FALSE);

	/*
	// Third light
	m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState			(D3DRS_SRCBLEND,	D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState			(D3DRS_DESTBLEND,	D3DBLEND_ONE);
	cc.set									(s_Light_Direct.constants.get("light_direction"),	vLightDir.x,-vLightDir.y,vLightDir.z,0	);
	cc.set									(s_Light_Direct.constants.get("light_color"),		.1f,		.9f,		.1,			.7		);
	cc.flush								(m_pd3dDevice);
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);
	m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, FALSE);
	*/

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);
	m_pd3dDevice->SetTexture				(1, NULL);
	m_pd3dDevice->SetTexture				(2, NULL);
	m_pd3dDevice->SetTexture				(3, NULL);
	m_pd3dDevice->SetRenderTarget			(0, pBaseTarget	);
	pBaseTarget->Release					();

	m_pd3dDevice->SetRenderState			( D3DRS_STENCILENABLE,		FALSE				);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderLight_Direct				()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderLight_Direct_smap	()
{
	LPDIRECT3DSURFACE9						pBaseTarget;

	// Set new render targets
	m_pd3dDevice->GetRenderTarget			(0, &pBaseTarget	);
	m_pd3dDevice->SetRenderTarget			(0, d_Accumulator_S	);
	m_pd3dDevice->Clear						(0L, NULL, D3DCLEAR_TARGET, 0x10101010, 1.0f, 0L);

	// samplers and texture (POS)
	m_pd3dDevice->SetTexture				(0, d_Position);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// samplers and texture (NORM)
	m_pd3dDevice->SetTexture				(1, d_Normal);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// samplers and texture (Power32)
	m_pd3dDevice->SetTexture				(2, t_SpecularPower_32);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR);

	// samplers and texture (SMAP)
	m_pd3dDevice->SetTexture				(3, sm_ShadowMap);
	m_pd3dDevice->SetSamplerState			(3, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(3, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(3, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(3, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// Set up the stencil states
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILENABLE,		TRUE				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILREF,			0x01				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILMASK,		0xff				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILWRITEMASK,	0x00				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	);

	m_pd3dDevice->SetRenderState			( D3DRS_CULLMODE,	D3DCULL_NONE);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_Light_Direct_smap.ps);
	m_pd3dDevice->SetVertexShader			(s_Light_Direct_smap.vs);
	m_pd3dDevice->SetFVF					(TVERTEX_FVF);
	D3DXVECTOR3 vLightDir					= dv_LightDir;
	D3DXMATRIX	mInvView;
	D3DXVec3Normalize						(&vLightDir, &vLightDir);
	D3DXMatrixInverse						(&mInvView,0,&dm_2view);
	D3DXVec3TransformNormal					(&vLightDir, &vLightDir,&mInvView);
	D3DXVec3Normalize						(&vLightDir, &vLightDir);

	cc.set									(s_Light_Direct_smap.constants.get("light_direction"),	vLightDir.x,vLightDir.y,vLightDir.z,0	);
	cc.set									(s_Light_Direct_smap.constants.get("light_xform"),		*(Fmatrix*)&dm_model2world2view2projection_light	);
	R_constant*	C							= s_Light_Direct_smap.constants.get("jitter");

	if (1)
	{
		cc.set									(s_Light_Direct_smap.constants.get("light_color"),		.3f,		.3f,		1.,			.9);

		Fvector4 J; float scale					= (3.f / SHADOW_MAP_SIZE)/11.f;
		J.set(11, 0,  0);		J.sub(11); J.mul(scale);	cc.seta	(C,0,J.x,J.y,-J.y,-J.x);
		J.set(19, 3,  0);		J.sub(11); J.mul(scale);	cc.seta	(C,1,J.x,J.y,-J.y,-J.x);
		J.set(22, 11, 0);		J.sub(11); J.mul(scale);	cc.seta	(C,2,J.x,J.y,-J.y,-J.x);
		J.set(19, 19, 0);		J.sub(11); J.mul(scale);	cc.seta	(C,3,J.x,J.y,-J.y,-J.x);

		J.set(9,  7,  15, 9);	J.sub(11); J.mul(scale);	cc.seta	(C,4,J.x,J.y,J.w,J.z);
		J.set(13, 15, 7,  13);	J.sub(11); J.mul(scale);	cc.seta	(C,5,J.x,J.y,J.w,J.z);

		// Blend mode - directional light comes first - means no blending
		m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, FALSE);

		// Render Quad
		m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
		m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);
	} else {
		cc.set									(s_Light_Direct_smap.constants.get("light_color"),		.3f/2,		.3f/2,		1./2,		.9/2);

		Fvector4 J; float scale					= (3.f / SHADOW_MAP_SIZE)/27.f;
		J.set(21, 2,  33, 2 );	J.sub(27); J.mul(scale); cc.seta	(C,0,J.x,J.y,J.w,J.z);
		J.set(9,  9,  45, 9 );	J.sub(27); J.mul(scale); cc.seta	(C,1,J.x,J.y,J.w,J.z);
		J.set(20, 12, 34, 12);	J.sub(27); J.mul(scale); cc.seta	(C,2,J.x,J.y,J.w,J.z);
		J.set(12, 20, 27, 20);	J.sub(27); J.mul(scale); cc.seta	(C,3,J.x,J.y,J.w,J.z);
		J.set(42, 20, 2,  21);	J.sub(27); J.mul(scale); cc.seta	(C,4,J.x,J.y,J.w,J.z);
		J.set(52, 21, 20, 27);	J.sub(27); J.mul(scale); cc.seta	(C,5,J.x,J.y,J.w,J.z);
		cc.flush								(m_pd3dDevice);

		// Blend mode - directional light comes first - means no blending
		m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, FALSE);

		// Render Quad
		m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
		m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

		// Second part
		J.set(34, 27, 2,  33);	J.sub(27); J.mul(scale); cc.seta	(C,0,J.x,J.y,J.w,J.z);
		J.set(52, 33, 12, 34);	J.sub(27); J.mul(scale); cc.seta	(C,1,J.x,J.y,J.w,J.z);
		J.set(27, 34, 42, 34);	J.sub(27); J.mul(scale); cc.seta	(C,2,J.x,J.y,J.w,J.z);
		J.set(20, 42, 34, 42);	J.sub(27); J.mul(scale); cc.seta	(C,3,J.x,J.y,J.w,J.z);
		J.set(9,  45, 45, 45);	J.sub(27); J.mul(scale); cc.seta	(C,4,J.x,J.y,J.w,J.z);
		J.set(21, 52, 33, 52);	J.sub(27); J.mul(scale); cc.seta	(C,5,J.x,J.y,J.w,J.z);
		cc.flush								(m_pd3dDevice);

		// Blend mode - directional light comes first - means no blending
		m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pd3dDevice->SetRenderState			(D3DRS_SRCBLEND,	D3DBLEND_ONE);
		m_pd3dDevice->SetRenderState			(D3DRS_DESTBLEND,	D3DBLEND_ONE);

		// Render Quad
		m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
		m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);
		m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, FALSE);
	}

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);
	m_pd3dDevice->SetTexture				(1, NULL);
	m_pd3dDevice->SetTexture				(2, NULL);
	m_pd3dDevice->SetTexture				(4, NULL);
	m_pd3dDevice->SetRenderTarget			(0, pBaseTarget	);
	pBaseTarget->Release					();

	m_pd3dDevice->SetRenderState			( D3DRS_STENCILENABLE,		FALSE				);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderCombineDBG_Normals			()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderCombineDBG_Normals	()
{
	// samplers and texture
	m_pd3dDevice->SetTexture				(0, d_Normal);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_CombineDBG_Normals.ps);
	m_pd3dDevice->SetVertexShader			(s_CombineDBG_Normals.vs);
	m_pd3dDevice->SetFVF					(TVERTEX_FVF);
	cc.flush								(m_pd3dDevice);

	// Render Quad
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE,	D3DCULL_NONE);
	m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderCombineDBG_Base			()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderCombineDBG_Base	()
{
	// samplers and texture
	m_pd3dDevice->SetTexture				(0, d_Color);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_CombineDBG_Base.ps);
	m_pd3dDevice->SetVertexShader			(s_CombineDBG_Base.vs);
	m_pd3dDevice->SetFVF					(TVERTEX_FVF);
	cc.flush								(m_pd3dDevice);

	// Render Quad
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE,	D3DCULL_NONE);
	m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderCombineDBG_Accumulator			()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderCombineDBG_Accumulator	()
{
	// samplers and texture (diffuse + gloss)
	m_pd3dDevice->SetTexture				(0, d_Color);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// samplers and texture
	m_pd3dDevice->SetTexture				(1, d_Accumulator);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_CombineDBG_Accumulator.ps);
	m_pd3dDevice->SetVertexShader			(s_CombineDBG_Accumulator.vs);
	m_pd3dDevice->SetFVF					(TVERTEX_FVF);
	cc.flush								(m_pd3dDevice);

	// Render Quad
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE,	D3DCULL_NONE);
	m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);
	m_pd3dDevice->SetTexture				(1, NULL);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderCombine_Normal			()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderCombine_Normal	()
{
	// samplers and texture (diffuse + gloss)
	m_pd3dDevice->SetTexture				(0, d_Color);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// samplers and texture
	m_pd3dDevice->SetTexture				(1, d_Accumulator);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_Combine_Normal.ps);
	m_pd3dDevice->SetVertexShader			(s_Combine_Normal.vs);
	m_pd3dDevice->SetFVF					(TVERTEX_FVF);
	cc.flush								(m_pd3dDevice);

	// Render Quad
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE,	D3DCULL_NONE);
	m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);
	m_pd3dDevice->SetTexture				(1, NULL);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderOverlay()
// Desc: Displays the content of the shadow map.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderOverlay()
{
	m_pd3dDevice->SetSamplerState		(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState		(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetRenderState		(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pd3dDevice->SetFVF				(TVERTEX_FVF);
	m_pd3dDevice->SetStreamSource		(0, m_pOverlayVB, 0, sizeof(TVERTEX));
	m_pd3dDevice->SetPixelShader		(0);
	m_pd3dDevice->SetTexture			(0, sm_ShadowMap);
	m_pd3dDevice->SetTextureStageState	(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState	(0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1);
	m_pd3dDevice->DrawPrimitive			(D3DPT_TRIANGLESTRIP, 0, 2);
	m_pd3dDevice->SetTexture			(0, NULL);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: UpdateTransform()
// Desc: Re-computes all transform parameters for shadow mapping.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::UpdateTransform()
{
	// Model offset
	D3DXVECTOR3 vModelOffs		= D3DXVECTOR3(0.0f, 2.0f, 0.0f);

	// Set the transform matrices
	D3DXVECTOR3 vEyePt			= D3DXVECTOR3(0.0f, 2.0f, -4.0f);
	D3DXVECTOR3 vLookatPt		= D3DXVECTOR3(0.0f, 2.0f,  0.0f);
	D3DXVECTOR3 vUpVec			= D3DXVECTOR3(0.0f, 1.0f,  0.0f);
	FLOAT       fAspect			= (FLOAT)m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;

	D3DXMATRIX	matWorldModel,	matWorldFloor, matView, matProj, mat;
	D3DXMATRIX	matShadowView,	matShadowProj, mat2;

	D3DXMatrixLookAtLH			(&matView, &vEyePt, &vLookatPt, &vUpVec);
	D3DXMatrixPerspectiveFovLH	(&matProj, D3DX_PI / 3, fAspect, 1.0f, 100.0f);
	D3DXMatrixMultiply			(&mat, &matView, &matProj);

	dm_2view					= matView;
	dm_2projection				= matProj;
	dm_world2view2projection	= mat;

	D3DXMATRIX matTranslate;
	D3DXMatrixTranslation		(&matTranslate, vModelOffs.x, vModelOffs.y, vModelOffs.z);
	D3DXMatrixMultiply			(&matWorldModel, m_ArcBall.GetRotationMatrix(), &matTranslate);
	dm_model2world				= matWorldModel;

	D3DXMatrixMultiply			(&dm_model2world2view,				&dm_model2world, &dm_2view);
	D3DXMatrixMultiply			(&dm_model2world2view2projection,	&dm_model2world, &dm_world2view2projection);

	// Light direction
	dv_LightDir					= D3DXVECTOR3(-1.0f, -1.0f, 1.0);
	D3DXVec3Normalize			(&dv_LightDir, &dv_LightDir);

	// Setup shadow map transform
	vLookatPt					= vModelOffs;
	vEyePt						= vLookatPt - dv_LightDir * (m_fModelSize / 2.0f);
	D3DXMatrixLookAtLH			(&matView, &vEyePt, &vLookatPt, &vUpVec);

	// Projection for directional light
	D3DXMatrixOrthoLH			(&matProj, 5.0f, 5.0f, 0.0f, DEPTH_RANGE);

	D3DXMATRIX matShadowModelMVP;
	D3DXMatrixMultiply			(&mat, &matView, &matProj);
	D3DXMatrixMultiply			(&matShadowModelMVP,	&matWorldModel, &mat);

	dm_model2world2view2projection_smap	= matShadowModelMVP;

	// Now, take positions from eye-space to world space
	D3DXMATRIX matView2World,matView2World2Light;
	D3DXMatrixInverse			(&matView2World,0,&dm_2view);
	D3DXMatrixMultiply			(&matView2World2Light,&matView2World,&mat);

	// Texture adjustment matrix
	FLOAT fTexelOffs			= (.5f / SHADOW_MAP_SIZE);
	D3DXMATRIX matTexAdj	(	0.5f,      0.0f,        0.0f,        0.0f,
		0.0f,     -0.5f,        0.0f,        0.0f,
		0.0f,      0.0f,        1.0f,        0.0f,
		0.5f + fTexelOffs,	0.5f + fTexelOffs,	0.0f, 1.0f);
	D3DXMatrixMultiply			(&dm_model2world2view2projection_light, &matView2World2Light, &matTexAdj);

	// ***** Verify results
	D3DXVECTOR3		vecPoint	= vEyePt + dv_LightDir*1.f;
	D3DXVECTOR3		vecP_smap, vecP_light;
	D3DXVECTOR4		vtP_smap,  vtP_light;
	D3DXMATRIX		invModel;
	D3DXMatrixInverse			(&invModel,		0, &matWorldModel);
	D3DXVec3TransformCoord		(&vecP_smap,	&vecPoint, &invModel);
	D3DXVec3TransformCoord		(&vecP_light,	&vecPoint, &dm_2view);

	D3DXVec3Transform			(&vtP_smap,		&vecP_smap,	&dm_model2world2view2projection_smap	);
	D3DXVec3Transform			(&vtP_light,	&vecP_light,&dm_model2world2view2projection_light	);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_ArcBall.HandleMouseMessages(hWnd, uMsg, wParam, lParam);

	return CD3DApplication::MsgProc(hWnd, uMsg, wParam, lParam);
}
