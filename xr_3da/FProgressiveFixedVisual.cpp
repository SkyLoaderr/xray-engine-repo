// FProgressiveFixedVisual.cpp: implementation of the FProgressiveFixedVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
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
	P.IB_Destroy();
}

void FProgressiveFixedVisual::Load(const char* N, CStream *data, DWORD dwFlags)
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
	R_ASSERT(dwVertCount>V_Minimal);
	vsplit = new Vsplit[dwVertCount-V_Minimal];
	R_ASSERT(fs->ReadChunk(0x2,vsplit));

	// PMAP_FACES
	R_ASSERT(fs->FindChunk(0x3));
	DWORD fCount = fs->Rdword();
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
	VERIFY	(LOD>=0.0f);
	VERIFY	(LOD<=1.0f);
	
	DWORD dwCount = V_Minimal + iFloor(float(dwVertCount-V_Minimal)*LOD+.4999f);
	
	VERIFY(dwCount>=V_Minimal);
	VERIFY(dwCount<=dwVertCount);
	
	// First cycle - try to improve quality
	if (V_Current!=dwCount) {
		WORD* Indices = P.IB_Lock();
		
		while (V_Current<dwCount) {
			Vsplit&	S	=	vsplit[V_Current-V_Minimal];
			
			// fixup faces
			DWORD	dwEnd = DWORD(S.numFixFaces)+FIX_Current;
			WORD	V_Cur = WORD(V_Current);
			for (DWORD I=FIX_Current; I<dwEnd; I++) {
				//				VERIFY(Indices[faces_affected[I]]==S.vsplitVert);
				Indices[faces_affected[I]]=V_Cur;
			};
			
			// correct numbers
			I_Current	+=	3*DWORD(S.numNewTriangles);
			V_Current	+=	1;
			FIX_Current	+=	S.numFixFaces;
		};
		
		// Second cycle - try to decrease quality
		while (V_Current>dwCount) {
			V_Current	-=	1;
			
			Vsplit&	S	=	vsplit[V_Current-V_Minimal];
			
			// fixup faces
			WORD V_New		= WORD(S.vsplitVert);
			DWORD dwEnd		= FIX_Current;
			DWORD dwStart	= dwEnd-S.numFixFaces;
			for (DWORD I=dwStart; I<dwEnd; I++) {
				//				VERIFY(Indices[faces_affected[I]]==V_Current);
				Indices[faces_affected[I]]=V_New;
			};
			
			// correct numbers
			I_Current	-=	3*DWORD(S.numNewTriangles);
			FIX_Current	-=	S.numFixFaces;
		};
		P.IB_Unlock();
	}
}

void FProgressiveFixedVisual::Render(float LOD)
{
	if (++iRefresh >= PM_REFRESH) {
		iRefresh = 0;
		SetLOD(LOD);
	}
	if (V_Current && I_Current)
		Device.Primitive.Draw	(P,V_Current,I_Current/3);
}

#define PCOPY(a)	a = pFrom->a

void	FProgressiveFixedVisual::pm_copy(FBasicVisual *pSrc)
{
	FProgressiveFixedVisual	*pFrom = (FProgressiveFixedVisual *)pSrc;
	PCOPY(V_Minimal);
	PCOPY(V_Current);
	PCOPY(I_Current);
	PCOPY(FIX_Current);

	PCOPY(faces_affected);
	PCOPY(vsplit);

}
void	FProgressiveFixedVisual::Copy(FBasicVisual *pSrc)
{
	Fvisual::Copy	(pSrc);

	pm_copy			(pSrc);

	// note::	index buffer bust be explicitly duplicated 
	CPrimitive PP = P;
	P.IB_Replicate(PP);
}
