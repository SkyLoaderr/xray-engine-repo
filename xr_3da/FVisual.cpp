// FVisual.cpp: implementation of the FVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fvisual.h"
#include "fstaticrender.h"
#include "fmesh.h"

FBasicVisual*	Fvisual::CreateInstance(void)
{	return new Fvisual;	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fvisual::Fvisual()  : FBasicVisual()
{
}

Fvisual::~Fvisual()
{
}

void Fvisual::Release()
{
	FBasicVisual::Release();
	P.Release();
}
void Fvisual::Load(const char* N, CStream *data, DWORD dwFlags)
{
	FBasicVisual::Load(N,data,dwFlags);

	// read vertices
	DWORD dwVertStart=0;
	if ((dwFlags&VLOAD_NOVERTICES)==0) {
		if (data->FindChunk(OGF_VCONTAINER)) {
			DWORD ID		= data->Rdword();
			dwVertStart		= data->Rdword();
			dwVertCount		= data->Rdword();
			P.VB_Attach		(::Render.FVF[ID],::Render.VB[ID]);
		} else {
			R_ASSERT(data->FindChunk(OGF_VERTICES));
			
			DWORD dwVertType;
			
			dwVertStart	= 0;
			dwVertType	= data->Rdword();
			dwVertCount	= data->Rdword();
			
			// But texture coords must be expanded
			P.VB_Create(dwVertType,dwVertCount,
				D3DUSAGE_WRITEONLY | ((dwFlags&VLOAD_FORCESOFTWARE)?D3DUSAGE_SOFTWAREPROCESSING:0),
				dwVertType,data->Pointer());
		}
	}
	P.IB_SetBase(dwVertStart);

	// indices
	dwPrimsCount = 0;
	if ((dwFlags&VLOAD_NOINDICES)==0) {
		R_ASSERT(data->FindChunk(OGF_INDICES));
		DWORD dwCount = data->Rdword();
		R_ASSERT(dwCount%3 == 0);
		
		P.IB_Create(dwVertStart,dwCount,
			D3DUSAGE_WRITEONLY | ((dwFlags&VLOAD_FORCESOFTWARE)?D3DUSAGE_SOFTWAREPROCESSING:0),
			data->Pointer());
		dwPrimsCount = dwCount/3;
	}

//	Msg("   %d vertices, %d polys.",dwVertCount,dwPrimsCount);
}

void Fvisual::Render(float LOD)
{
	Device.Primitive.Draw	(P,dwVertCount,dwPrimsCount);
}

#define PCOPY(a)	a = pFrom->a
void	Fvisual::Copy(FBasicVisual *pSrc)
{
	FBasicVisual::Copy(pSrc);

	Fvisual	*pFrom = (Fvisual *)pSrc;
	PCOPY(P);
	PCOPY(dwVertCount);
	PCOPY(dwPrimsCount);
}
