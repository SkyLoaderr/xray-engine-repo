#include "stdafx.h"

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

#define FAKES 0xffffffff
#define FAKEZ 0xfffffffe

void ConvertVertices(DWORD dwTypeDest, void *pDest, DWORD dwTypeSrc, void *pSource, DWORD dwCount)
// assuming that pDest is large enought to maintain all the data
{
	DWORD	TransferMask[64];
	DWORD	tmPos		= 0;
	DWORD	tmPosSrc	= 0;
	DWORD	dwSizeSrc	= D3DXGetFVFVertexSize(dwTypeSrc)/4;
	DWORD	dwSizeDest	= D3DXGetFVFVertexSize(dwTypeDest)/4;
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
	if ((dwTypeDest&D3DFVF_PSIZE) && (dwTypeSrc&D3DFVF_PSIZE))
	{	// DEST & SRC
		TransferMask[tmPos++]=tmPosSrc++;
	}
	if ((dwTypeDest&D3DFVF_PSIZE) && !(dwTypeSrc&D3DFVF_PSIZE))
	{	// DEST & !SRC
		TransferMask[tmPos++]=FAKEZ;// fake data
	}
	if (!(dwTypeDest&D3DFVF_PSIZE) && (dwTypeSrc&D3DFVF_PSIZE))
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
		if (dwTSrc==0) {
			R_ASSERT(0=="Source vertex format doesn't has texture coords at all");
		}
		// Copy real TC
		DWORD dwStage0TC = tmPosSrc;
		for (DWORD i=0; i<dwTSrc; i++) {
			TransferMask[tmPos++]=tmPosSrc++;
			TransferMask[tmPos++]=tmPosSrc++;
		}
		// Duplicate stage0 TC
		for (i=dwTSrc; i<dwTDest; i++) {
			TransferMask[tmPos++]=dwStage0TC;
			TransferMask[tmPos++]=dwStage0TC+1;
		}
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
