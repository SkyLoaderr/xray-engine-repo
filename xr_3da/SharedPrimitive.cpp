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
	Device.Shader.Evict		();

	mSize					= rsDVB_Size*1024;
	R_CHK					(HW.pDevice->CreateVertexBuffer( mSize, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &pVB));
	R_ASSERT				(pVB);

	mPosition				= 0;
	mDiscardID				= 0;

	Msg("* DVB created: %dK", mSize/1024);
}

void CVertexStream::Destroy	()
{
	_RELEASE				(pVB);
}

void CIndexStream::Create	()
{
	Device.Shader.Evict		();
	
	mSize					= rsDIB_Size*1024;
	R_CHK					(HW.pDevice->CreateIndexBuffer( mSize, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIB));
	R_ASSERT				(pIB);
	
	mPosition				= 0;
	mDiscardID				= 0;

	Msg("* DIB created: %dK", mSize/1024);
}

void CIndexStream::Destroy	()
{
	_RELEASE				(pIB);
}
