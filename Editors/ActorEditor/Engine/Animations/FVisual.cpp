// FVisual.cpp: implementation of the FVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "fvisual.h"
#include "fmesh.h"
#ifndef _EDITOR
	#include "render.h"
#endif

CVisual*	Fvisual::CreateInstance(void)
{	return new Fvisual;	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fvisual::Fvisual()  : CVisual()
{
}

Fvisual::~Fvisual()
{
}

void Fvisual::Release()
{
	CVisual::Release();
	P.Release();
}
void Fvisual::Load(const char* N, CStream *data, DWORD dwFlags)
{
	CVisual::Load(N,data,dwFlags);

	// read vertices
	DWORD dwVertStart=0;
	if ((dwFlags&VLOAD_NOVERTICES)==0) {
		if (data->FindChunk(OGF_VCONTAINER)) {
#ifndef _EDITOR
			DWORD ID		= data->Rdword();
			dwVertStart		= data->Rdword();
			dwVertCount		= data->Rdword();
			P.VB_Attach		(::Render->getFVF(ID),::Render->getVB(ID));
#endif
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
	P.IB_SetBase	(dwVertStart);

	// indices
	dwPrimsCount = 0;
	if ((dwFlags&VLOAD_NOINDICES)==0) {
		if (data->FindChunk(OGF_ICONTAINER)) {
			/*
			DWORD ID		= data->Rdword();
			P.IB_Attach		()
			*/
		} else {
			R_ASSERT(data->FindChunk(OGF_INDICES));
			DWORD dwCount = data->Rdword();
			R_ASSERT(dwCount%3 == 0);

			P.IB_Create(dwVertStart,dwCount,
				D3DUSAGE_WRITEONLY | ((dwFlags&VLOAD_FORCESOFTWARE)?D3DUSAGE_SOFTWAREPROCESSING:0),
				data->Pointer());
			dwPrimsCount = dwCount/3;
		}
	}

//	Msg("   %d vertices, %d polys.",dwVertCount,dwPrimsCount);
}

void Fvisual::Render(float LOD)
{
	Device.Primitive.Draw	(P,dwVertCount,dwPrimsCount);
}

#define PCOPY(a)	a = pFrom->a
void	Fvisual::Copy(CVisual *pSrc)
{
	CVisual::Copy(pSrc);

	Fvisual	*pFrom = (Fvisual *)pSrc;
	PCOPY(P);
	PCOPY(dwVertCount);
	PCOPY(dwPrimsCount);
}
