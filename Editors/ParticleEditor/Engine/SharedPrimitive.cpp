#include "stdafx.h"
#pragma hdrstop

#include "SharedPrimitive.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int		rsDVB_Size			= 512;
int		rsDIB_Size			= 512;

void CVertexStream::Create	()
{
	mDiscardID				= 0;
	Device.Shader.Evict		();

	// Get vertex size
	mStride		= D3DXGetFVFVertexSize(mFVF);
	
	// Calc size
	R_ASSERT	(mCount);
	DWORD		dwMEM_Request	= mCount*mStride;
	DWORD		dwMEM_Designed	= rsDVB_Size*1024;
	DWORD		dwMEM_Alloc		= _max(dwMEM_Request,dwMEM_Designed);
	mSize		= dwMEM_Alloc/mStride;
	if ((dwMEM_Alloc-mSize*mStride)!=0)	mSize+=1;

	R_ASSERT	(mSize>=mCount);
	R_ASSERT	(mSize<=65535);
	
	// Create VB
	R_CHK		(HW.pDevice->CreateVertexBuffer( mSize*mStride,D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,mFVF,D3DPOOL_DEFAULT,&pVB));

	R_ASSERT	(pVB);
	mPosition	= 0;

	Msg("* DVB created: %dK, %d verts.", mSize*mStride/1024,mSize);
}

void CVertexStream::Destroy	()
{
	_RELEASE(pVB);
}

void CIndexStream::Create	()
{
	mDiscardID				= 0;
	Device.Shader.Evict		();
	
	// Calc size
	R_ASSERT	(mCount);
	DWORD		dwMEM_Request	= mCount*mStride;
	DWORD		dwMEM_Designed	= rsDIB_Size*1024;
	DWORD		dwMEM_Alloc		= _max(dwMEM_Request,dwMEM_Designed);
	mSize		= dwMEM_Alloc/mStride;
	if ((dwMEM_Alloc-mSize*mStride)!=0)	mSize+=1;
	
	// Create VB
	R_CHK		(HW.pDevice->CreateIndexBuffer( mSize*mStride, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIB));

	R_ASSERT	(pIB);
	mPosition	= 0;

	Msg("* DIB created: %dK, %d indices", mSize*mStride/1024,mSize);
}

void CIndexStream::Destroy	()
{
	_RELEASE(pIB);
}
