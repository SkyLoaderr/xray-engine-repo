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

void* _VertexStream::Lock	( u32 vl_Count, u32 Stride, u32& vOffset )
{
#ifdef DEBUG
	PGO					(Msg("PGO:VB_LOCK:%d",vl_Count));
	VERIFY				(0==dbg_lock);
	dbg_lock			++;
#endif

	// Ensure there is enough space in the VB for this data
	u32	bytes_need		= vl_Count*Stride;
	R_ASSERT			((bytes_need<=mSize) && vl_Count);

	// Vertex-local info
	u32 vl_mSize		= mSize/Stride;
	u32 vl_mPosition	= mPosition/Stride + 1;

	// Check if there is need to flush and perform lock
	BYTE* pData			= 0;
	if ((vl_Count+vl_mPosition) >= vl_mSize)
	{
		// FLUSH-LOCK
		mPosition			= 0;
		vOffset				= 0;
		mDiscardID			++;

		pVB->Lock			( mPosition, bytes_need, (void**)&pData, LOCKFLAGS_FLUSH);
	} else {
		// APPEND-LOCK
		mPosition			= vl_mPosition*Stride;
		vOffset				= vl_mPosition;

		pVB->Lock			( mPosition, bytes_need, (void**)&pData, LOCKFLAGS_APPEND);
	}
	VERIFY				( pData );

	return LPVOID		( pData );
}

void	_VertexStream::Unlock		( u32 Count, u32 Stride)
{
#ifdef DEBUG
	PGO					(Msg("PGO:VB_UNLOCK:%d",Count));
	VERIFY				(1==dbg_lock);
	dbg_lock			--;
#endif
	mPosition			+=	Count*Stride;

	VERIFY				(pVB);
	pVB->Unlock			();
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

