#include "stdafx.h"
#include "fbasicvisual.h"
#include "fvisual.h"
#include "fhierrarhyvisual.h"
#include "ffileops.h"

static DWORD dwPositionPart[8] =
{
	0,	// no position
	3,	// x,y,z
	4,	// sx,sy,sz,rhw
	4,	// x,y,z,b1
	5,	// x,y,z,b1,b2
	6,	// x,y,z,b1,b2,b3
	7,	// x,y,z,b1,b2,b3,b4
	8	// x,y,z,b1,b2,b3,b4,b5
};

DWORD GetVertexSize(DWORD dwType)
{
	DWORD dwSize =
		dwPositionPart[(dwType&D3DFVF_POSITION_MASK)>>1]*sizeof(float);
	if (dwType&D3DFVF_NORMAL)		dwSize+=3*sizeof(float);
	if (dwType&D3DFVF_RESERVED1)	dwSize+=1*sizeof(DWORD);
	if (dwType&D3DFVF_DIFFUSE)		dwSize+=1*sizeof(DWORD);
	if (dwType&D3DFVF_SPECULAR)		dwSize+=1*sizeof(DWORD);
	dwSize +=
		((dwType&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT)*2*sizeof(float);
	return dwSize;
}

#define FAKES 0xffffffff
#define FAKEZ 0xfffffffe

void ConvertVertices(DWORD dwTypeDest, void *pDest, DWORD dwTypeSrc, void *pSource, DWORD dwCount)
// assuming that pDest is large enought to maintain all the data
{
	DWORD	TransferMask[64];
	DWORD	tmPos		= 0;
	DWORD	tmPosSrc	= 0;
	DWORD	dwSizeSrc	= GetVertexSize(dwTypeSrc)/4;
	DWORD	dwSizeDest	= GetVertexSize(dwTypeDest)/4;
	DWORD*	dest		= (DWORD*)pDest;
	DWORD*	src			= (DWORD*)pSource;

	// avoid redundant processing
	if (dwTypeDest==dwTypeSrc) {
		CopyMemory(pDest,pSource,dwSizeDest*dwCount*4);
		return;
	}

	// how many bytes to 'simple copy'
	DWORD dwPosDest	= (dwTypeDest&D3DFVF_POSITION_MASK)>>1;
	DWORD dwPosSrc	= (dwTypeSrc&D3DFVF_POSITION_MASK)>>1;
	if (dwPosDest==dwPosSrc) {
		DWORD cnt = dwPositionPart[dwPosSrc];
		for (DWORD i=0; i<cnt; i++) TransferMask[tmPos++]=i;
		tmPosSrc = tmPos;
	} else {
		VERIFY2(0,"Can't convert between different vertex positions");
	}

	// ---------------------- "Reserved" property
	if ((dwTypeDest&D3DFVF_RESERVED1) && (dwTypeSrc&D3DFVF_RESERVED1))
	{	// DEST & SRC
		TransferMask[tmPos++]=tmPosSrc++;
	}
	if ((dwTypeDest&D3DFVF_RESERVED1) && !(dwTypeSrc&D3DFVF_RESERVED1))
	{	// DEST & !SRC
		TransferMask[tmPos++]=FAKEZ;// fake data
	}
	if (!(dwTypeDest&D3DFVF_RESERVED1) && (dwTypeSrc&D3DFVF_RESERVED1))
	{	// !DEST & SRC
		tmPosSrc++;					// skip it
	}

	// ---------------------- "Normal" property
	if ((dwTypeDest&D3DFVF_NORMAL) && (dwTypeSrc&D3DFVF_NORMAL))
	{	// DEST & SRC
		TransferMask[tmPos++]=tmPosSrc++;
		TransferMask[tmPos++]=tmPosSrc++;
		TransferMask[tmPos++]=tmPosSrc++;
	}
	if ((dwTypeDest&D3DFVF_NORMAL) && !(dwTypeSrc&D3DFVF_NORMAL))
	{	// DEST & !SRC
		VERIFY2(0,"Source format doesn't have NORMAL but destination HAS");
	}
	if (!(dwTypeDest&D3DFVF_NORMAL) && (dwTypeSrc&D3DFVF_NORMAL))
	{	// !DEST & SRC
		tmPosSrc++;					// skip it
		tmPosSrc++;					// skip it
		tmPosSrc++;					// skip it
	}

	// ---------------------- "Diffuse" property
	if ((dwTypeDest&D3DFVF_DIFFUSE) && (dwTypeSrc&D3DFVF_DIFFUSE))
	{	// DEST & SRC
		TransferMask[tmPos++]=tmPosSrc++;
	}
	if ((dwTypeDest&D3DFVF_DIFFUSE) && !(dwTypeSrc&D3DFVF_DIFFUSE))
	{	// DEST & !SRC
		TransferMask[tmPos++]=FAKES;	// fake data - white
	}
	if (!(dwTypeDest&D3DFVF_DIFFUSE) && (dwTypeSrc&D3DFVF_DIFFUSE))
	{	// !DEST & SRC
		tmPosSrc++;						// skip it
	}

	// ---------------------- "Specular" property
	if ((dwTypeDest&D3DFVF_SPECULAR) && (dwTypeSrc&D3DFVF_SPECULAR))
	{	// DEST & SRC
		TransferMask[tmPos++]=tmPosSrc++;
	}
	if ((dwTypeDest&D3DFVF_SPECULAR) && !(dwTypeSrc&D3DFVF_SPECULAR))
	{	// DEST & !SRC
		TransferMask[tmPos++]=FAKES;	// fake data - white
	}
	if (!(dwTypeDest&D3DFVF_SPECULAR) && (dwTypeSrc&D3DFVF_SPECULAR))
	{	// !DEST & SRC
		tmPosSrc++;						// skip it
	}

	// ---------------------- "Texture coords" property
	DWORD dwTDest = ((dwTypeDest&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
	DWORD dwTSrc  = ((dwTypeSrc &D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
	if (dwTDest<=dwTSrc) {
		for (DWORD i=0; i<dwTDest; i++) {
			TransferMask[tmPos++]=tmPosSrc++;
			TransferMask[tmPos++]=tmPosSrc++;
		}
	} else {
		VERIFY2(0,"Destination format has more texture coordinate sets than source");
	}

	// ---------------------- REAL CONVERTION USING BUILDED MASK
	for (DWORD i=0; i<dwCount; i++) {
		// one vertex
		for (DWORD j=0; j<dwSizeDest; j++) {
			DWORD m = TransferMask[j];
			if (m == FAKES) dest[j]=0xffffffff;
			else if (m == FAKEZ) dest[j]=0;
			else dest[j]=src[m];
		}
		dest	+= dwSizeDest;
		src		+= dwSizeSrc;
	}
	return;
}
/*
_EF(FBasicVisual*	CreateVisualByType(DWORD type))
	FBasicVisual *pVisual = NULL;

	// Check types
	switch (type) {
	case MT_NORMAL:				// our base visual
		pVisual = new Fvisual;
		break;
	case MT_HIERRARHY:
		pVisual	= new FHierrarhyVisual;
		break;
	default:
		VERIFY2(0,"Unknown visual type");
		break;
	}
	VERIFY(pVisual);
	return pVisual;
_end;

_EF(FBasicVisual*	LoadVisual(char *name))
	FBasicVisual	*pVisual;
	FILE_NAME		fn;

	// Load data from MESHES or LEVEL
	strcpy(fn,MESHES_PATH);
	strcat(fn,name);
	Msg("* Loading visual '%s'...",name);

	// Actual loading
	CStream *data = new CStream(fn);
	ogf_header h;
	data->ReadChunk(OGF_HEADER,&h);
	pVisual = CreateVisualByType(h.type);
	pVisual->Load(data);
	_DELETE(data);

	return pVisual;
_end;
*/
