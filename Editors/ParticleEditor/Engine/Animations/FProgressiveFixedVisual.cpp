// FProgressiveFixedVisual.cpp: implementation of the FProgressiveFixedVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "FProgressiveFixedVisual.h"
#include "fmesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FProgressiveFixedVisual::FProgressiveFixedVisual() : Fvisual()
{
	iRefresh = rand()%PM_REFRESH;
}

FProgressiveFixedVisual::~FProgressiveFixedVisual()
{
	_RELEASE	(pIndices);
}

void FProgressiveFixedVisual::Load(const char* N, CStream *data, u32 dwFlags)
{
	// load base visual data
	Fvisual::Load(N,data,dwFlags);

	CStream* fs = data->OpenChunk(OGF_P_MAP);

	// PMAP_HEADER
	R_ASSERT(fs->FindChunk(0x1));
	V_Current	= V_Minimal = fs->Rdword();
	I_Current	= fs->Rdword();
	FIX_Current	= 0;

	// PMAP_VSPLIT
	R_ASSERT(vCount>V_Minimal);
	vsplit = new Vsplit[vCount-V_Minimal];
	R_ASSERT(fs->ReadChunkSafe(0x2,vsplit,(vCount-V_Minimal)*sizeof(Vsplit)));

	// PMAP_FACES
	R_ASSERT(fs->FindChunk(0x3));
	u32 fCount = fs->Rdword();
	faces_affected = new WORD[fCount];
	fs->Read(faces_affected,fCount*sizeof(WORD));

	fs->Close();
}

void FProgressiveFixedVisual::Release()
{
	Fvisual::Release();

	delete [] faces_affected;
	delete [] vsplit;
}

void FProgressiveFixedVisual::SetLOD(float LOD)
{
	if (LOD<0)			return;
	VERIFY				(LOD<=1.0f);
	
	u32 dwCount = V_Minimal + iFloor(float(vCount-V_Minimal)*LOD+.4999f);
	
	VERIFY(dwCount>=V_Minimal);
	VERIFY(dwCount<=vCount);
	
	// First cycle - try to improve quality
	if (V_Current!=dwCount) {
		WORD*					Indices = 0;
		R_CHK					(pIndices->Lock	(0,0,(BYTE**)&Indices,0));
		
		while (V_Current<dwCount) {
			Vsplit&	S	=	vsplit[V_Current-V_Minimal];
			
			// fixup faces
			u32	dwEnd = u32(S.numFixFaces)+FIX_Current;
			WORD	V_Cur = WORD(V_Current);
			for (u32 I=FIX_Current; I<dwEnd; I++) {
				//				VERIFY(Indices[faces_affected[I]]==S.vsplitVert);
				Indices[faces_affected[I]]=V_Cur;
			};
			
			// correct numbers
			I_Current	+=	3*u32(S.numNewTriangles);
			V_Current	+=	1;
			FIX_Current	+=	S.numFixFaces;
		};
		
		// Second cycle - try to decrease quality
		while (V_Current>dwCount) {
			V_Current	-=	1;
			
			Vsplit&	S	=	vsplit[V_Current-V_Minimal];
			
			// fixup faces
			WORD V_New		= WORD(S.vsplitVert);
			u32 dwEnd		= FIX_Current;
			u32 dwStart	= dwEnd-S.numFixFaces;
			for (u32 I=dwStart; I<dwEnd; I++) {
				//				VERIFY(Indices[faces_affected[I]]==V_Current);
				Indices[faces_affected[I]]=V_New;
			};
			
			// correct numbers
			I_Current	-=	3*u32(S.numNewTriangles);
			FIX_Current	-=	S.numFixFaces;
		};

		R_CHK					(pIndices->Unlock());
	}
}

void FProgressiveFixedVisual::Render(float LOD)
{
	if (++iRefresh >= PM_REFRESH) {
		iRefresh = 0;
		SetLOD(LOD);
	}
	if (V_Current && I_Current) {
		Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,pVertices);
		Device.Primitive.setIndices		(vBase,	pIndices);
		Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,V_Current,iBase,I_Current/3);
	}
}

#define PCOPY(a)	a = pFrom->a

void	FProgressiveFixedVisual::pm_copy(CVisual *pSrc)
{
	FProgressiveFixedVisual	*pFrom = dynamic_cast<FProgressiveFixedVisual*>(pSrc);
	PCOPY(V_Minimal);
	PCOPY(V_Current);
	PCOPY(I_Current);
	PCOPY(FIX_Current);

	PCOPY(faces_affected);
	PCOPY(vsplit);

}
void	FProgressiveFixedVisual::Copy(CVisual *pFrom)
{
	Fvisual::Copy			(pFrom);
	pm_copy					(pFrom);

	FProgressiveFixedVisual* pSrc	= dynamic_cast<FProgressiveFixedVisual*>(pFrom);

	// note::	index buffer bust be explicitly duplicated 
	BYTE *d_src=0, *d_dst=0;
	D3DINDEXBUFFER_DESC		desc;
	R_CHK					(pSrc->pIndices->GetDesc	(&desc));
	R_CHK					(pSrc->pIndices->Lock		(0,0,&d_src,D3DLOCK_READONLY));
	R_CHK					(HW.pDevice->CreateIndexBuffer(iCount*2,desc.Usage,D3DFMT_INDEX16,desc.Pool,&pIndices));
	R_CHK					(pIndices->Lock				(0,0,&d_dst,0));
	PSGP.memCopy			(d_dst,d_src,desc.Size);
	R_CHK					(pIndices->Unlock());
	R_CHK					(pSrc->pIndices->Unlock());
}
