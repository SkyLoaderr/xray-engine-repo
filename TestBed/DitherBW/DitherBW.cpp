// DitherBW.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tga.h"

static int magic4x4[4][4] =  
{
 	 0, 14,  3, 13,
	11,  5,  8,  6,
	12,  2, 15,  1,
	 7,  9,  4, 10
};

void bwdithermap	(int levels, int magic[16][16] )
{
	/* Get size of each step */
    float N = 255.0f / (levels - 1);    
	
	/*
	* Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
	* and doesn't give us full intensity range (only 17 sublevels).
	* 
	* magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
	* N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
	* pixels incremented to the next level (this is reserved for the
	* pixel value with mod N == 0 at the next level).
	*/
	
    float	magicfact = (N - 1) / 16;
    for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			for ( int k = 0; k < 4; k++ )
				for ( int l = 0; l < 4; l++ )
					magic[4*k+i][4*l+j] =
					(int)(0.5 + magic4x4[i][j] * magicfact +
					(magic4x4[k][l] / 16.) * magicfact);
}

void main(int argc, char* argv[])
{
	CImage		tex;
	tex.LoadTGA	("x:\\dbg\\logo.tga");
	
	int			magic[16][16];
	bwdithermap	(2,magic);

	for (DWORD y=0; y<tex.dwHeight; y++)
	{
		for (DWORD x=0; x<tex.dwWidth; x++)
		{
			DWORD c		= tex.GetPixel(x,y);
			DWORD new_c;

			int val		= c&255;
		    int row		= y % 16; 
			int col		= x % 16;
 			new_c		= val > magic[col][row] ? 255 : 0;
			
			tex.PutPixel(x,y,RGB(new_c,new_c,new_c));
		}
	}
	
	// Save
	TGAdesc		tga;
	tga.data	= tex.pData;
	tga.format	= IMG_24B;
	tga.height	= tex.dwHeight;
	tga.width	= tex.dwWidth;
	tga.scanlenght=tex.dwWidth*4;

	CFS_File	fs	("x:\\dbg\\out.tga");
	tga.maketga	(fs);
}

