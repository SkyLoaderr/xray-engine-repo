// TextureFIRE.cpp: implementation of the CTextureFIRE class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextureFIRE.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureFIRE::CTextureFIRE(void *params) : CTexture(params)
{
	pSurface				= NULL;
}

CTextureFIRE::~CTextureFIRE()
{
	VERIFY(pSurface==NULL);
	_FREE(pEmulSurface);
	_RELEASE(pPalette);
}


void BuildPal2Color(PALETTEENTRY *p, WORD *pal2color, D3DX_SURFACEFORMAT f)
{
	if (f==D3DX_SF_R5G6B5) {
		for (int i=0; i<256; i++, p++) {
			DWORD r = p->peRed	>>3;	// 5bit
			DWORD g = p->peGreen>>2;	// 6bit
			DWORD b = p->peBlue	>>3;	// 5bit
			DWORD color = (r<<(6+5))|(g<<(5))|(b);
			pal2color[i]=WORD(color&0xffff);
		}
	} else if (f==D3DX_SF_R5G5B5 || f==D3DX_SF_A1R5G5B5) {
		for (int i=0; i<256; i++, p++) {
			DWORD r = p->peRed	>>3;	// 5bit
			DWORD g = p->peGreen>>3;	// 6bit
			DWORD b = p->peBlue	>>3;	// 5bit
			DWORD color = (r<<(5+5))|(g<<(5))|(b);
			pal2color[i]=WORD(color&0xffff);
		}
	} else THROW;
}

void BuildFirePalette(PALETTEENTRY *palette) {
	palette[0].peRed=0;
	palette[0].peGreen=0;
	palette[0].peBlue=0;
	int BX = 0;
	do {
		int AX = BX * 255 / 85;
		unsigned char DL = (unsigned char) (AX&0xff);
		palette[BX+1].peRed		= DL;
		palette[BX+1].peGreen	= 0;
		palette[BX+1].peBlue	= 0;
		
		palette[BX+1+85].peRed		= 255;
		palette[BX+1+85].peGreen	= DL;
		palette[BX+1+85].peBlue		= 0;
		
		palette[BX+1+85+85].peRed	= 255;
		palette[BX+1+85+85].peGreen	= 255;
		palette[BX+1+85+85].peBlue	= DL;

		BX++;
	} while (BX<85);
	for (int i=0; i<256; i++) {
		palette[i].peFlags=0;
	}
}

void CTextureFIRE::Load()
{
	// Create palette
	PALETTEENTRY palette[256];
	BuildFirePalette(palette);
	CHK_DX(HW.pDDraw->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256,palette,&pPalette,NULL));

	// try to create palettized texture
	DWORD dwF = D3DX_TEXTURE_NOMIPMAP;
	DWORD dwSZ = 128;	fmt = D3DX_SF_PALETTE8;
	pSurface = TUCreateTexture(&dwF, &dwSZ, &dwSZ, &fmt, pPalette, NULL);
	if (fmt==D3DX_SF_PALETTE8) {
		pEmulSurface = 0;
	} else {
		_RELEASE(pPalette);
		BuildPal2Color(palette,pal2color,fmt);
		pEmulSurface = (char *)malloc(128*128);
		ZeroMemory(pEmulSurface,128*128);
	}
}

void CTextureFIRE::Unload()
{
	_FREE(pEmulSurface);
	_RELEASE(pPalette);
	_RELEASE(pSurface);
}

void CTextureFIRE::Activate(DWORD dwStage) 
{
	unsigned char *	psurf;

	DDSURFACEDESC2	ddsd;
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	CHK_DX(pSurface->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ));

	if (pEmulSurface) {
		psurf = (unsigned char *)pEmulSurface;
	} else {
		psurf = (unsigned char *)ddsd.lpSurface;
	}
	
	// random line
	for (int j=0; j<128; j++)
		psurf[128*126+j] = psurf[128*127+j] = 
		psurf[128*124+j] = psurf[128*125+j] = 
		char(127+rand()%127);
	for (DWORD i=4; i<127; i+=2) {
		DWORD DI = i*128;
		for (DWORD BX=2; BX<126; BX+=2) {
			DWORD AX =	(psurf[DI+BX-2]+psurf[DI+BX+2]+psurf[DI+BX]+
				psurf[DI+BX+256]) / 4;
			AX = (AX + psurf[DI+BX-256]) >> 1;
			unsigned char AL		= (unsigned char) (AX&0xff);
			if (AL > 128)		AL -= 3;
			else if (AL > 5)	AL -= 5;
			else				AL  = 0;
			
			psurf[DI+BX-128*4]		=
			psurf[DI+BX-128*4+1]	=
			psurf[DI+BX-128*3]		=
			psurf[DI+BX-128*3+1]	=AL;
		}
	}
	
	// We're done. Unlock the texture surface and return.
	if (pEmulSurface) {
		// transfer to video memory surface
		WORD *pDest = (WORD *) ddsd.lpSurface;
		unsigned char *pSrc = (unsigned char *)pEmulSurface;
		for (int i=0; i<128; i++) {
			for (int j=0; j<32; j++) {
				*pDest++ = pal2color[*pSrc++];
				*pDest++ = pal2color[*pSrc++];
				*pDest++ = pal2color[*pSrc++];
				*pDest++ = pal2color[*pSrc++];
			}
		}
	} else {
		// it was real texture memory access
		// so do nothing :)
	}

	CHK_DX(pSurface->Unlock(NULL));
	CHK_DX(HW.pDevice->SetTexture(dwStage, pSurface));
}
