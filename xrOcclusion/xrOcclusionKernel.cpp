#include "stdafx.h"
#include "xrOcclusion.h"
#include "visibilitytester.h"

extern LPD3DXCONTEXT		g_pContext;
extern LPDIRECT3D7			g_pD3D;
extern LPDIRECT3DDEVICE7	g_pDevice;
extern LPDIRECTDRAWSURFACE7	g_pBB;
extern float				g_viewfar;

struct ORM_Model
{
	Fvector					C;
	float					R;
	LPDIRECT3DVERTEXBUFFER7	VB;
	DWORD					VB_Size;
	WORD*					IB;
	DWORD					IB_Size;
};

vector<ORM_Model*>			g_models;


ORM_API ORM_Vertex*		__cdecl ORM_Begin	(int vCount, int iCount, WORD* idx)
{
	ORM_Model*	M = new ORM_Model;
	g_models.push_back(M);

	M->VB_Size	= vCount;
	M->IB_Size	= iCount;
	M->IB		= idx;

    D3DVERTEXBUFFERDESC	vbdesc;

	ZeroMemory			(&vbdesc, sizeof(D3DVERTEXBUFFERDESC));
	vbdesc.dwSize        = sizeof(D3DVERTEXBUFFERDESC);
	vbdesc.dwCaps        = D3DVBCAPS_WRITEONLY | D3DVBCAPS_SYSTEMMEMORY;
	vbdesc.dwFVF         = ORM_FVF;
	vbdesc.dwNumVertices = vCount;

	g_pD3D->CreateVertexBuffer(&vbdesc, &M->VB, 0L );

	ORM_Vertex* pDest;
	M->VB->Lock(DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS,
		(void **)&pDest, NULL );

	return pDest;
}
ORM_API void			__cdecl ORM_End		(Fvector &C, float R)
{
	ORM_Model*	M	= g_models.back();
	M->VB->Unlock();
	M->VB->Optimize(g_pDevice,0);
	M->C.set(C);
	M->R = R;
}



//-----------------------------------------------------------------------------
#define V_MAXCOUNT		4096	// maximal number of visuals visible from the slot
#define V_INC			(65535/V_MAXCOUNT)
#define V_CAM_MAX		6

DWORD					CTable[V_MAXCOUNT];
struct {
	Fvector vAt;
	Fvector vUp;
} cam_def[V_CAM_MAX];

void BuildCTable()
{
	WORD	 r, g, b;
	DWORD	_r,_g,_b;
	WORD	c=0;
	
	// bb assumed to be 565
	for (int i=0; i<V_MAXCOUNT; i++)
	{
		c+= V_INC;
		r = (c>>(6+5))	& 31;
		g = (c>>(5))	& 63;
		b = (c)			& 31;
		_r= DWORD(r)<<3;
		_g= DWORD(g)<<2;
		_b= DWORD(b)<<3;
		CTable[i] = RGBA_MAKE(_r,_g,_b,0);
	}

	cam_def[0].vAt.set(0,0,1);	cam_def[0].vUp.set(0,1,0); // forward
	cam_def[1].vAt.set(0,0,-1); cam_def[1].vUp.set(0,1,0); // backward
	cam_def[2].vAt.set(-1,0,0); cam_def[2].vUp.set(0,1,0); // left
	cam_def[3].vAt.set(1,0,0);	cam_def[3].vUp.set(0,1,0); // right
	cam_def[4].vAt.set(0,1,0);	cam_def[4].vUp.set(0,0,1); // up
	cam_def[5].vAt.set(0,-1,0); cam_def[5].vUp.set(0,0,1); // down
}

CFrustumClipper	Frustum;

void KernelSetup()
{
	// projection matrix
	Fmatrix			P;
	P.build_projection(PI/2, 1.f, 0.20f, g_viewfar);
	g_pDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION,	P.d3d());

	// world matrix
	D3DXMatrixIdentity((D3DXMATRIX*)&P);
	g_pDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,		P.d3d());

	// frustum
	Frustum.Projection(90.f,0.20f, g_viewfar);

	// BuildCTable
	BuildCTable();
}
void KernelDestroy()
{
	for (int i=0; i<g_models.size(); i++)
		g_models[i]->VB->Release();
}

int Color2IDX(WORD c)
{
	return (c/V_INC)-1;
}

ORM_API void	__cdecl ORM_Process	(
		DWORD		Count,
		Fvector&	Pos,		// position of test point
		WORD*		ID,			// id's of tested models
		BOOL*		R			// boolean result, TRUE-visible
		)
{
	Fmatrix		mView;
	Fvector		from,vAt;
	from.set	(Pos);

	for (int cam=0; cam<6; cam++)
	{
		// setup camera
		vAt.add		(from, cam_def[cam].vAt);
		mView.build_camera(from, vAt, cam_def[cam].vUp );
		g_pDevice->SetTransform(D3DTRANSFORMSTATE_VIEW,	mView.d3d());

		// init frustum
		Frustum.BuildFrustum(from,cam_def[cam].vAt,cam_def[cam].vUp);

		// draw them out
	    g_pContext->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
		g_pDevice->BeginScene();
		for (DWORD it=0; it<Count; it++)
		{
			if (ID[it] >= g_models.size()) continue;
			ORM_Model*	pModel = g_models[ID[it]];
			if (Frustum.visibleSphere(pModel->C,pModel->R)) {
				g_pDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, CTable[it]);
				g_pDevice->DrawIndexedPrimitiveVB(
					D3DPT_TRIANGLELIST,
					pModel->VB,
					0, pModel->VB_Size,
					pModel->IB, pModel->IB_Size,
					0
					);
			}
		}
		g_pDevice->EndScene();
		
		// *** analyze results
		// lock
		DDSURFACEDESC2			picDesc;
		picDesc.dwSize = sizeof(picDesc);
		g_pBB->Lock(0,&picDesc,DDLOCK_WAIT|DDLOCK_READONLY|DDLOCK_NOSYSLOCK,0);

		// analyze
		DWORD*	ptr;
		int		idx;
		DWORD	pixel;
		for (int y=0; y<256; y++) {
			ptr = LPDWORD(LPWORD(picDesc.lpSurface)+y*256);
			for (int x=0; x<256/8; x++)
			{
				pixel	= *ptr++;
				idx		= (pixel&0xffff)/V_INC - 1;
				if (idx>=0 && idx<Count)			R[idx]= true;
				idx		= (pixel>>16)/V_INC - 1;
				if (idx>=0 && idx<Count)			R[idx]= true;
				
				pixel	= *ptr++;
				idx		= (pixel&0xffff)/V_INC - 1;
				if (idx>=0 && idx<Count)			R[idx]= true;
				idx		= (pixel>>16)/V_INC - 1;
				if (idx>=0 && idx<Count)			R[idx]= true;
				
				pixel	= *ptr++;
				idx		= (pixel&0xffff)/V_INC - 1;
				if (idx>=0 && idx<Count)			R[idx]= true;
				idx		= (pixel>>16)/V_INC - 1;
				if (idx>=0 && idx<Count)			R[idx]= true;
				
				pixel	= *ptr++;
				idx		= (pixel&0xffff)/V_INC - 1;
				if (idx>=0 && idx<Count)			R[idx]= true;
				idx		= (pixel>>16)/V_INC - 1;
				if (idx>=0 && idx<Count)			R[idx]= true;
			}
		}
		
		// unlock
		g_pBB->Unlock(0);
	}
}
