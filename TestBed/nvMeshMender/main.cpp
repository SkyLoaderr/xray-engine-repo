
/*
This program illustrates the use of NVMeshMender::Munge() (see NVMeshMender.h for a complete explanation of what this function does).
It starts by loading a mesh from an X file. The only attribute (other than its position) of any vertex of the mesh is its texture coordinates.
NVMeshMender::Munge() is then used to compute the tangent basis (tangent, binormal and normal) at each vertex of the mesh.
In so doing, it may have to add more vertices and/or change the texture coordinates to solve tricky discontinuity problems.
That's why an entire new vertex buffer is actually created after mending.

No error checking is performed on any of the D3D calls to keep code concise:
If you don't get a textured rotating tiger when you run the program, it means that something's not working;
use the debug version of Direct3D (DirectX control panel), step through the code and check any error output by the D3D calls.
*/

// Headers
#include "stdafx.h"
#pragma warning(disable : 4786)
#include "NVMeshMender.h"

// Functions
HWND InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitD3D(HWND);
void UninitD3D();
void Render();
void ComputeTangentBasis();
void CreateVectorBuffer(LPDIRECT3DVERTEXBUFFER8*, const std::vector<float>&, const std::vector<float>&);
void DrawVectorBuffer(LPDIRECT3DVERTEXBUFFER8, unsigned int);

// Variables
LPDIRECT3D8 gD3D; // Used to create D3DDevice
LPDIRECT3DDEVICE8 gD3DDevice; // The rendering device
typedef struct { // The vertex format for the original mesh
	D3DXVECTOR3 position;
	D3DXVECTOR2 texCoord;
} MeshVertex;
LPD3DXMESH gMesh; // The original mesh
typedef struct { // The vertex format for the mesh after mending
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texCoord;
} Vertex;
unsigned int gNumVertices; // The mesh number of vertices after mending
LPDIRECT3DVERTEXBUFFER8 gVertexBuffer; // The mesh vertices after mending
unsigned int gNumTriangles; // The mesh number of triangles after mending
LPDIRECT3DINDEXBUFFER8 gIndexBuffer; // The mesh indices after mending
LPDIRECT3DVERTEXBUFFER8 gTangentBuffer; // The tangents, computed during mending, vertex buffer (for display)
LPDIRECT3DVERTEXBUFFER8 gBinormalBuffer; // The binormals, computed during mending, vertex buffer (for display)
LPDIRECT3DVERTEXBUFFER8 gNormalBuffer; // The normals, computed during mending, vertex buffer (for display)
LPDIRECT3DTEXTURE8 gTexture;
bool gHidden; // True if the window is hidden (iconified or hidden by a full-screen window)
float gAngle; // Object yaw angle
float gTime; // Time
bool gPause; // Used to pause the animation
bool gDrawWireframe; // Toggle wireframe rendering mode
bool gDrawTangent; // Draw the tangents
bool gDrawBinormal; // Draw the binormals
bool gDrawNormal; // Draw the normals
ID3DXFont* gFont;
const LOGFONT gLogFont = {
	12,
	0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET,
	OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
	DEFAULT_PITCH | FF_DONTCARE, _T("Arial")
};
RECT gFontRect;

// Length of the segments representing the tangents, binormals and normals when displayed
#define VECTOR_LENGTH 0.25f
// The ratio of the circumference of a circle to its radius ;-)
#define TWO_PI ((float)(2 * 3.14159265358979323846264338327950288419716939937510582))

// Usage
const char gUsage[] = "\
esc: quit\n\
space: pause\n\
w: wireframe\n\
t: tangents\n\
b: binormals\n\
n: normals\n\
a: tangent basis\
";

// Main function
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// Gauss filtering coeffs
	const int	n	= 7;		// kernel size
	const float r	= 10.3f;		// gaussian radius
	const float t	= 1;		// grid dimension
	const float bs	= 1.2f;		// bilinear interpolation (1=point sampling, 2=twice the kernel size - interpolated)

	float		w [2*n+1];	float* _w = w;
	float		u [2*n+1];	float* _u = u;
	for (int i=-n; i<=n; i++)
	{
		*_w++	= expf(-float(i*i)/(2*r*r));
		*_u++	= bs*float(i)/t;
	}
	float mag				= 0;
	float s_out				= 1.f;
	for (i=0; i<2*n+1; i++)	mag		+= w[i];
	for (i=0; i<2*n+1; i++)	w[i]	= s_out*w[i]/mag;

	// Create window
	HWND hWnd = InitInstance(hInstance, nCmdShow);
	assert(hWnd);

	// Initialize D3D
	InitD3D(hWnd);

	// Run main loop
	MSG msg;
	do {
		BOOL gotAMsg;
		if (gHidden)
			gotAMsg = GetMessage(&msg, 0, 0, 0);
		else
			gotAMsg = PeekMessage(&msg, 0, 0, 0, PM_REMOVE);
		if (gotAMsg) {
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
		else if (!gHidden)
			Render();
	} while (msg.message != WM_QUIT);

	// Unitinialize D3D
	UninitD3D();

	return (int)msg.wParam;
}

// Create a window.
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)"NVMeshMender";
	wcex.lpszClassName	= "NVMeshMender";
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);
	hWnd = CreateWindow("NVMeshMender", "NVMeshMender", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, hInstance, 0);
	if (hWnd) {
		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);
	}
	return hWnd;
}

// Process messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message) {
	case WM_SIZE:
		gHidden = (wParam == SIZE_MAXHIDE) || (wParam == SIZE_MINIMIZED);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Render();
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		if ((int)wParam == VK_ESCAPE)
			PostQuitMessage(0);
		if ((int)wParam == VK_SPACE)
			gPause = !gPause;
		else if ((int)wParam == 'W')
			gDrawWireframe = !gDrawWireframe;
		else if ((int)wParam == 'T')
			gDrawTangent = !gDrawTangent;
		else if ((int)wParam == 'B')
			gDrawBinormal = !gDrawBinormal;
		else if ((int)wParam == 'N')
			gDrawNormal = !gDrawNormal;
		else if ((int)wParam == 'A') {
			if (gDrawTangent && gDrawBinormal && gDrawNormal)
				gDrawTangent = gDrawBinormal = gDrawNormal = false;
			else
				gDrawTangent = gDrawBinormal = gDrawNormal = true;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// D3D initialization
void InitD3D(HWND hWnd)
{
	// Create the D3D object, which is needed to create the D3DDevice.
	gD3D = Direct3DCreate8(D3D_SDK_VERSION);
	assert(gD3D);

	// Get the current desktop display mode
	D3DDISPLAYMODE d3ddm;
	gD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	// Create the Direct3D device.
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.hDeviceWindow  = hWnd;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	gD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &gD3DDevice);

	// Create the geometry
	D3DXLoadMeshFromX("Tiger.x", D3DXMESH_SYSTEMMEM, gD3DDevice, 0, 0, 0, &gMesh);
	D3DXComputeNormals(gMesh, 0);
	ComputeTangentBasis();

	// Create the texture
	D3DXCreateTextureFromFile(gD3DDevice, "tiger.bmp", &gTexture);

	// Create light
	D3DLIGHT8 light;
	ZeroMemory(&light, sizeof(D3DLIGHT8));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Diffuse.a = 1.0f;
	gD3DDevice->SetLight(0, &light);
	gD3DDevice->LightEnable(0, TRUE);

	// Create material
	D3DMATERIAL8 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL8));
	mtrl.Diffuse.r = 0.75f;
	mtrl.Diffuse.g = 0.75f;
	mtrl.Diffuse.b = 0.8f;
	mtrl.Diffuse.a = 0.0f;
	gD3DDevice->SetMaterial(&mtrl);
	gD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

	// Init time
	gTime = (float)timeGetTime();

	// Init font
	HFONT font = CreateFontIndirect(&gLogFont);
	D3DXCreateFont(gD3DDevice, font, &gFont);
	DeleteObject(font);
    gFontRect.left = 0;
    gFontRect.top = 0;
    gFontRect.right = gFontRect.left + 100;
    gFontRect.bottom = gFontRect.top + 100;
}

// D3D un-initialization
void UninitD3D()
{
	gFont->Release();
	gTexture->Release();
	gMesh->Release();
	gD3DDevice->Release();
}

// Render
void Render()
{
	if (gD3DDevice == 0)
		return;

	// Clear the backbuffer to a blue color
	gD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 200, 200, 200), 1.0f, 0);

	// Begin the scene
	gD3DDevice->BeginScene();

	// Set transforms

	// World
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	float newTime = (float)timeGetTime();
	if (!gPause)
		gAngle += 0.001f * (newTime - gTime);
	gTime = newTime;
	D3DXMatrixRotationY(&world, gAngle);
	gD3DDevice->SetTransform(D3DTS_WORLD, &world);

	// View
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0.0f, 0.0f, -8.0f),	// eye
							  &D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// at
							  &D3DXVECTOR3(0.0f, 1.0f, 0.0f));	// up
	gD3DDevice->SetTransform(D3DTS_VIEW, &view);

	// Projection
	D3DXMATRIX projection;
	D3DXMatrixPerspectiveFovLH(&projection, D3DX_PI/6.2f, 1.0f, 1.0f, 100.0f);
	gD3DDevice->SetTransform(D3DTS_PROJECTION, &projection);

	// Set shaders
	gD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
	gD3DDevice->SetPixelShader(0);

	// Set texture
	gD3DDevice->SetTexture(0, gTexture);
	gD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	// Set render states
	gD3DDevice->SetRenderState(D3DRS_FILLMODE, gDrawWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	// Draw mesh
	gD3DDevice->SetStreamSource(0, gVertexBuffer, sizeof(Vertex));
	gD3DDevice->SetIndices(gIndexBuffer, 0);
	gD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, gNumVertices, 0, gNumTriangles);

	// Draw tangent basis
	if (gDrawTangent)
		DrawVectorBuffer(gTangentBuffer, 0xffff0000);
	if (gDrawBinormal)
		DrawVectorBuffer(gBinormalBuffer, 0xff00ff00);
	if (gDrawNormal)
		DrawVectorBuffer(gNormalBuffer, 0xff0000ff);

	// Display help
	gFont->DrawText(gUsage, -1, &gFontRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK, D3DCOLOR_XRGB(0, 0, 0));

	// End the scene
	gD3DDevice->EndScene();

	// Present the backbuffer contents to the display
	gD3DDevice->Present(0, 0, 0, 0);
}

void ComputeTangentBasis()
{
	unsigned int i;

	// Retrieve vertices and texture coordinates from the mesh vertex buffer
	assert(gMesh->GetFVF() == (D3DFVF_XYZ | D3DFVF_TEX1));
	DWORD numVertices = gMesh->GetNumVertices();
	MeshVertex* vertexBuffer;
	std::vector<float> position;
	std::vector<float> texCoord;
	gMesh->LockVertexBuffer(D3DLOCK_READONLY, (BYTE**)&vertexBuffer);
	for (i = 0; i < numVertices; ++i) {
		position.push_back(vertexBuffer[i].position.x);
		position.push_back(vertexBuffer[i].position.y);
		position.push_back(vertexBuffer[i].position.z);
		texCoord.push_back(vertexBuffer[i].texCoord.x);
		texCoord.push_back(vertexBuffer[i].texCoord.y);
		texCoord.push_back(0);
	}
	gMesh->UnlockVertexBuffer();

	// Retrieve triangle indices from the index buffer
	DWORD numTriangles = gMesh->GetNumFaces();
	WORD (*indexBuffer)[3];
	std::vector<int> index;
	HRESULT hr = gMesh->LockIndexBuffer(D3DLOCK_READONLY, (BYTE**)&indexBuffer);
	for (i = 0; i < numTriangles; ++i) {
		index.push_back(indexBuffer[i][0]);
		index.push_back(indexBuffer[i][1]);
		index.push_back(indexBuffer[i][2]);
	}
	gMesh->UnlockIndexBuffer();

	// Prepare the parameters to the mesh mender

	// Fill in the input to the mesh mender
	// Positions
	NVMeshMender::VertexAttribute positionAtt;
	positionAtt.Name_ = "position";
	positionAtt.floatVector_ = position;
	// Indices
	NVMeshMender::VertexAttribute indexAtt;
	indexAtt.Name_ = "indices";
	indexAtt.intVector_ = index;
	// Texture coordinates
	NVMeshMender::VertexAttribute texCoordAtt;
	texCoordAtt.Name_ = "tex0";
	texCoordAtt.floatVector_ = texCoord;
	// Fill in input list
	std::vector<NVMeshMender::VertexAttribute> inputAtts;
	inputAtts.push_back(positionAtt);
	inputAtts.push_back(indexAtt);
	inputAtts.push_back(texCoordAtt);
	
	// Specify the requested output
	// Tangents
	NVMeshMender::VertexAttribute tangentAtt;
	tangentAtt.Name_ = "tangent";
	// Binormals
	NVMeshMender::VertexAttribute binormalAtt;
	binormalAtt.Name_ = "binormal";
	// Normals
	NVMeshMender::VertexAttribute normalAtt;
	normalAtt.Name_ = "normal";
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
					inputAtts,								// input attributes
					outputAtts,								// outputs attributes
					3.141592654f / 3.0f,					// tangent space smooth angle
					0,										// no texture matrix applied to my texture coordinates
					NVMeshMender::FixTangents,				// fix degenerate bases & texture mirroring
					NVMeshMender::FixCylindricalTexGen,		// handle cylindrically mapped textures via vertex duplication
					NVMeshMender::WeightNormalsByFaceSize	// weigh vertex normals by the triangle's size
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
	gNumVertices = position.size() / 3;
	unsigned int size = gNumVertices * sizeof(Vertex);
	gD3DDevice->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &gVertexBuffer);
	Vertex* vertexBufferNew;
	gVertexBuffer->Lock(0, size, (BYTE**)&vertexBufferNew, 0);
	for (i = 0; i < gNumVertices; ++i) {
		vertexBufferNew[i].position.x = position[3 * i + 0];
		vertexBufferNew[i].position.y = position[3 * i + 1];
		vertexBufferNew[i].position.z = position[3 * i + 2];
		vertexBufferNew[i].texCoord.x = texCoord[3 * i + 0];
		vertexBufferNew[i].texCoord.y = texCoord[3 * i + 1];
		vertexBufferNew[i].normal.x = normal[3 * i + 0];
		vertexBufferNew[i].normal.y = normal[3 * i + 1];
		vertexBufferNew[i].normal.z = normal[3 * i + 2];
	}
	gVertexBuffer->Unlock();

	// Create the new index buffer
	gNumTriangles = index.size() / 3;
	size = index.size() * sizeof(unsigned int);
	gD3DDevice->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &gIndexBuffer);
	gIndexBuffer->Lock(0, size, (BYTE**)&indexBuffer, 0);
	for (i = 0; i < gNumTriangles; ++i) {
		indexBuffer[i][0] = index[3 * i + 0];
		indexBuffer[i][1] = index[3 * i + 1];
		indexBuffer[i][2] = index[3 * i + 2];
	}
	gIndexBuffer->Unlock();

	// Create a normal vertex buffer for display
	CreateVectorBuffer(&gTangentBuffer, position, tangent);
	CreateVectorBuffer(&gBinormalBuffer, position, binormal);
	CreateVectorBuffer(&gNormalBuffer, position, normal);
}

void CreateVectorBuffer(LPDIRECT3DVERTEXBUFFER8* vectorBuffer, const std::vector<float>& position, const std::vector<float>& data)
{
	unsigned int size = gNumVertices * sizeof(D3DXVECTOR3[2]);
	gD3DDevice->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, vectorBuffer);
	D3DXVECTOR3 (*vector)[2];
	(*vectorBuffer)->Lock(0, size, (BYTE**)&vector, 0);
	for (unsigned int i = 0; i < gNumVertices; ++i) {
		vector[i][0].x = position[3 * i + 0];
		vector[i][0].y = position[3 * i + 1];
		vector[i][0].z = position[3 * i + 2];
		vector[i][1].x = vector[i][0].x + VECTOR_LENGTH * data[3 * i + 0];
		vector[i][1].y = vector[i][0].y + VECTOR_LENGTH * data[3 * i + 1];
		vector[i][1].z = vector[i][0].z + VECTOR_LENGTH * data[3 * i + 2];
	}
	(*vectorBuffer)->Unlock();
}

void DrawVectorBuffer(LPDIRECT3DVERTEXBUFFER8 vectorBuffer, unsigned int color)
{
	gD3DDevice->SetTexture(0, 0);
	gD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	gD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	gD3DDevice->SetVertexShader(D3DFVF_XYZ);
	gD3DDevice->SetPixelShader(0);
	gD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, color);
	gD3DDevice->SetStreamSource(0, vectorBuffer, sizeof(D3DXVECTOR3));
	gD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, gMesh->GetNumVertices());
	gD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
}
