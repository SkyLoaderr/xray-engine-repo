// file: targasaver.cpp
#include "stdafx.h"
#pragma hdrstop

#include "tga.h"

void TGAdesc::maketga( IWriter& fs ){
	R_ASSERT(data);
	R_ASSERT(width);
	R_ASSERT(height);

	tgaHeader hdr;
	ZeroMemory( &hdr, sizeof(hdr) );
	hdr.tgaImgType			= 2;
	hdr.tgaImgSpec.tgaXSize = u16(width);
	hdr.tgaImgSpec.tgaYSize = u16(height);

	if( format == IMG_24B ){
		hdr.tgaImgSpec.tgaDepth = 24;
		hdr.tgaImgSpec.tgaImgDesc = 32;			// flip
	}
	else{
		hdr.tgaImgSpec.tgaDepth = 32;
		hdr.tgaImgSpec.tgaImgDesc = 0x0f | 32;	// flip
	}

	fs.w(&hdr, sizeof(hdr) );

	if( format==IMG_24B ){
		BYTE ab_buffer[4]={0,0,0,0};
		int  real_sl = ((width*3)) & 3;
		int  ab_size = real_sl ? 4-real_sl : 0 ;
		for( int j=0; j<height; j++){
			BYTE *p = (LPBYTE)data + scanlenght*j;
			for( int i=0; i<width; i++){
				BYTE buffer[3] = {p[0],p[1],p[2]};
				fs.w(buffer, 3 );
				p+=4;
			}
			if(ab_size)
				fs.w(ab_buffer, ab_size );
		}
	}
	else{
		for( int j=0; j<height; j++){
			BYTE *p = (LPBYTE)data + scanlenght*j;
			for( int i=0; i<width; i++){
				BYTE buffer[4] = {p[0],p[1],p[2],p[3]};
				fs.w(buffer, 4 );
				p+=4;
			}
		}
	}
}
#ifdef XRLC
void TGAdesc::maketga( char *fname )
{
	string_path FN;
	sprintf(FN,(string(g_params.L_path)+"%s.tga").c_str(),fname);

	CFS_File fs	(FN);
	maketga	(fs);
}
#endif
