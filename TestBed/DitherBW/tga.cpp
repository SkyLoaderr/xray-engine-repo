// file: targasaver.cpp
#include "stdafx.h"
#include "tga.h"

VOID TGAdesc::maketga( CFS_Base& fs ){
	R_ASSERT(data);
	R_ASSERT(width);
	R_ASSERT(height);

	tgaHeader hdr;
	ZeroMemory( &hdr, sizeof(hdr) );
	hdr.tgaImgType = 2;
	hdr.tgaImgSpec.tgaXSize = width;
	hdr.tgaImgSpec.tgaYSize = height;

	if( format == IMG_24B ){
		hdr.tgaImgSpec.tgaDepth = 24;
		hdr.tgaImgSpec.tgaImgDesc = 32;			// flip
	}
	else{
		hdr.tgaImgSpec.tgaDepth = 32;
		hdr.tgaImgSpec.tgaImgDesc = 0x0f | 32;	// flip
	}

	fs.write(&hdr, sizeof(hdr) );

	if( format==IMG_24B ){
		BYTE ab_buffer[4]={0,0,0,0};
		int  real_sl = ((width*3)) & 3;
		int  ab_size = real_sl ? 4-real_sl : 0 ;
		for( int j=0; j<height; j++){
			BYTE *p = (LPBYTE)data + scanlenght*j;
			for( int i=0; i<width; i++){
				BYTE buffer[3] = {p[0],p[1],p[2]};
				fs.write(buffer, 3 );
				p+=4;
			}
			if(ab_size)
				fs.write(ab_buffer, ab_size );
		}
	}
	else{
		for( int j=0; j<height; j++){
			BYTE *p = (LPBYTE)data + scanlenght*j;
			for( int i=0; i<width; i++){
				BYTE buffer[4] = {p[0],p[1],p[2],p[3]};
				fs.write(buffer, 4 );
				p+=4;
			}
		}
	}
}

#ifdef ENGINE_BUILD
VOID TGAdesc::maketga( char *fname )
{
	static int sshotnum=0;
	char FN[_MAX_PATH];
	sprintf(FN,"%s%d.tga",fname,sshotnum++);

	CFS_File fs	(FN);
	maketga	(fs);

	Log("Screenshot saved.");
}
#else
#ifdef XRLC
VOID TGAdesc::maketga( char *fname )
{
	char FN[_MAX_PATH];
	sprintf(FN,(string(g_params.L_path)+"%s.tga").c_str(),fname);

	CFS_File fs	(FN);
	maketga	(fs);
}
#endif
#endif