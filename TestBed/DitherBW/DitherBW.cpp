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

float dist2d(DWORD _x, DWORD _y, DWORD _x2, DWORD _y2)
{
	float	x=float(_x), y=float(_y), x2=float(_x2), y2=float(_y2);
	return	sqrtf((x-x2)*(x-x2)+(y-y2)*(y-y2));
}

void main(int argc, char* argv[])
{
	CImage		tex;
	tex.LoadTGA	("x:\\dbg\\logo.tga");
	
	int			magic[16][16];
	bwdithermap	(2,magic);

	float		p0=16;
	float		p1=0;
	float		p2=0;
	float		p3=64;

	float		f = float(tex.dwWidth);
	for (DWORD y=0; y<tex.dwHeight; y++)
	{
		for (DWORD x=0; x<tex.dwWidth; x++)
		{
//			DWORD c		= tex.GetPixel(x,y);
			DWORD new_c;

			float	fx	= float(x)/f; float ifx = 1.f-fx;
			float	fy	= float(y)/f; float ify = 1.f-fy;

			float	c01	= p0*ifx+p1*fx;
			float	c23	= p2*ifx+p3*fx;
			float	c02	= p0*ify+p2*fy;
			float	c13	= p1*ify+p3*fy;

			float	cx	= ify*c01 + fy*c23;
			float	cy	= ifx*c02 + fx*c13;

			new_c		= DWORD((cx+cy)/2);

			int row		= y % 16; 
			int col		= x % 16;
 			new_c		= new_c > magic[col][row] ? 255 : 0;

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

