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
