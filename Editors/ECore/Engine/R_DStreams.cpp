#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"
#include "R_DStreams.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int		rsDVB_Size			= 512;
int		rsDIB_Size			= 512;

void _VertexStream::Create	()
{
	Device.Resources->Evict		();

	mSize					= rsDVB_Size*1024;
	R_CHK					(HW.pDevice->CreateVertexBuffer( mSize, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &pVB, NULL));
	R_ASSERT				(pVB);

	mPosition				= 0;
	mDiscardID				= 0;

	Msg("* DVB created: %dK", mSize/1024);
}

void _VertexStream::Destroy	()
{
	_RELEASE				(pVB);
	_clear					();
}

void _IndexStream::Create	()
{
	Device.Resources->Evict		();

	mSize					= rsDIB_Size*1024;
	R_CHK					(HW.pDevice->CreateIndexBuffer( mSize, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIB, NULL));
	R_ASSERT				(pIB);

	mPosition				= 0;
	mDiscardID				= 0;

	Msg("* DIB created: %dK", mSize/1024);
}

void _IndexStream::Destroy	()
{
	_RELEASE				(pIB);
	_clear					();
}

