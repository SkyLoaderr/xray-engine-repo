// Occlusion.cpp : Defines the entry point for the console application.
//

#include	"stdafx.h"
#include	"occRasterizer.h"

#define IMG_24B 0
#define IMG_32B 1

class TGAdesc
{
public:
	int		format;
	int		scanlenght;
	int		width,height;
	void *	data;
public:
	TGAdesc()	{ data = 0; };
	~TGAdesc()	{};
	
	VOID maketga( char *fname );
};

int __cdecl main	(int argc, char* argv[])
{
	occRasterizer	occ;

	// setup tri
	occTri	T;
	T.raster[0].x	= 0.1f;
	T.raster[0].y	= 0.1f;
	T.raster[0].z	= 0.01f;
	
	T.raster[1].x	= 60.f;
	T.raster[1].y	= 10.f;
	T.raster[1].z	= 0.1f;
	
	T.raster[2].x	= 20.f;
	T.raster[2].y	= 50.f;
	T.raster[2].z	= 0.99f;

	// draw tri
	occ.clear		();
	occ.rasterize	(&T);
	occ.propagade	();

	// copy into surface
	const DWORD		S = 8;
	const DWORD		size = occ_dim0*S;
	DWORD			buf[size*size];
	for (int y=0; y<occ_dim0; y++)
	{
		for (int x=0; x<occ_dim0; x++)
		{
			float	A	= *(occ.dbg_depth() + y*occ_dim0 + x);
			DWORD  gray	= int(A*255.f);
			DWORD  mask	= (*(occ.dbg_frame() + y*occ_dim0 + x)) ? 255 : 0;
			DWORD  C	= (mask << 24) | (gray << 16) | (gray << 8) | (gray << 0);

			for (int by=0; by<S; by++)
				for (int bx=0; bx<S; bx++)
					buf[(y*S+by)*size + x*S+bx]	= C;
		}
	}

	// save
	TGAdesc	desc;
	desc.format		= IMG_32B;
	desc.scanlenght	= size*4;
	desc.width		= size;
	desc.height		= size;
	desc.data		= buf;
	desc.maketga	("c:\\occ.tga");

	return 0;
}

#pragma pack(push,1)
struct tgaImgSpecHeader{
	WORD tgaXOrigin;
	WORD tgaYOrigin;
	WORD tgaXSize;
	WORD tgaYSize;
	BYTE tgaDepth;
	BYTE tgaImgDesc;
};
struct tgaHeader
{
	BYTE tgaIDL;
	BYTE tgaMapType;
	BYTE tgaImgType;
	BYTE tgaClrMapSpec[5];
	tgaImgSpecHeader tgaImgSpec;
};
#pragma pack(pop)

VOID TGAdesc::maketga( char *fname )
{
	FILE* fs = fopen	(fname,"wb");

	tgaHeader	hdr;
	ZeroMemory	( &hdr, sizeof(hdr) );
	hdr.tgaImgType			= 2;
	hdr.tgaImgSpec.tgaXSize = width;
	hdr.tgaImgSpec.tgaYSize = height;
	
	if( format == IMG_24B ){
		hdr.tgaImgSpec.tgaDepth		= 24;
		hdr.tgaImgSpec.tgaImgDesc	= 32;			// flip
	}
	else{
		hdr.tgaImgSpec.tgaDepth		= 32;
		hdr.tgaImgSpec.tgaImgDesc	= 0x0f | 32;	// flip
	}
	
	fwrite	(&hdr, sizeof(hdr),1,fs);
	
	if( format==IMG_24B )
	{
		BYTE ab_buffer[4]={0,0,0,0};
		int  real_sl = ((width*3)) & 3;
		int  ab_size = real_sl ? 4-real_sl : 0 ;
		for( int j=0; j<height; j++){
			BYTE *p = (LPBYTE)data + scanlenght*j;
			for( int i=0; i<width; i++){
				BYTE buffer[3] = {p[0],p[1],p[2]};
				fwrite	(buffer, 3, 1, fs);
				p+=4;
			}
			if(ab_size)	fwrite(ab_buffer, ab_size, 1, fs);
		}
	}
	else
	{
		for( int j=0; j<height; j++){
			BYTE *p = (LPBYTE)data + scanlenght*j;
			for( int i=0; i<width; i++){
				BYTE buffer[4] = {p[0],p[1],p[2],p[3]};
				fwrite(buffer, 4, 1, fs );
				p+=4;
			}
		}
	}
	fclose (fs);
}
