#include "stdafx.h"
#pragma hdrstop

#include "SharedPrimitive.h"

void	CSharedStreams::OnDeviceCreate	()
{
	// Create Quad-IB
	{
		const u32 dwTriCount	= 4*1024;
		const u32 dwIdxCount	= dwTriCount*2*3;
		WORD	*Indices		= 0;
		u32	dwUsage			= D3DUSAGE_WRITEONLY;
		if (HW.Caps.vertex.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
		R_CHK(HW.pDevice->CreateIndexBuffer(dwIdxCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&QuadIB));
		R_CHK(QuadIB->Lock(0,0,(BYTE**)&Indices,0));
		{
			int		Cnt = 0;
			int		ICnt= 0;
			for (int i=0; i<dwTriCount; i++)
			{
				Indices[ICnt++]=u16(Cnt+0);
				Indices[ICnt++]=u16(Cnt+1);
				Indices[ICnt++]=u16(Cnt+2);

				Indices[ICnt++]=u16(Cnt+3);
				Indices[ICnt++]=u16(Cnt+2);
				Indices[ICnt++]=u16(Cnt+1);

				Cnt+=4;
			}
		}
		R_CHK(QuadIB->Unlock());
	}

	// streams
	Vertex.Create		();
	Index.Create		();
}
void	CSharedStreams::OnDeviceDestroy	()
{
	// streams
	Index.Destroy		();
	Vertex.Destroy		();

	// Quad
	_RELEASE			(QuadIB);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int		rsDVB_Size			= 512;
int		rsDIB_Size			= 512;

void _VertexStream::Create	()
{
	Device.Shader.Evict		();

	mSize					= rsDVB_Size*1024;
	R_CHK					(HW.pDevice->CreateVertexBuffer( mSize, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &pVB));
	R_ASSERT				(pVB);

	mPosition				= 0;
	mDiscardID				= 0;

	Msg("* DVB created: %dK", mSize/1024);
}

void _VertexStream::Destroy	()
{
	_RELEASE				(pVB);
}

void _IndexStream::Create	()
{
	Device.Shader.Evict		();
	
	mSize					= rsDIB_Size*1024;
	R_CHK					(HW.pDevice->CreateIndexBuffer( mSize, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIB));
	R_ASSERT				(pIB);
	
	mPosition				= 0;
	mDiscardID				= 0;

	Msg("* DIB created: %dK", mSize/1024);
}

void _IndexStream::Destroy	()
{
	_RELEASE				(pIB);
}
