// DitherBW.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tga.h"

void	make_square( double, int [256], int [256], int [16][16] );

static int magic4x4[4][4] =  {
 	 0, 14,  3, 13,
	11,  5,  8,  6,
	12,  2, 15,  1,
	 7,  9,  4, 10
};

/* basic dithering macro */
#define DMAP(v,x,y)	(modN[v]>magic[x][y] ? divN[v] + 1 : divN[v])


/*****************************************************************
 * TAG( dithermap )
 * 
 * Create a color dithering map with a specified number of intensity levels.
 * Inputs:
 * 	levels:		Intensity levels per primary.
 *	gamma:		Display gamma value.
 * Outputs:
 * 	rgbmap:		Generated color map.
 *	divN:		"div" function for dithering.
 *	modN:		"mod" function for dithering.
 * Assumptions:
 * 	rgbmap will hold levels^3 entries.
 * Algorithm:
 *	Compute gamma compensation map.
 *	N = 255.0 / (levels - 1) is number of pixel values per level.
 *	Compute rgbmap with red ramping fastest, green slower, and blue
 *	slowest (treat it as if it were rgbmap[levels][levels][levels][3]).
 *	Call make_square to get divN, modN, and magic
 *
 * Note:
 *	Call dithergb( x, y, r, g, b, levels, divN, modN, magic ) to get index
 *	into rgbmap for a given color/location pair, or use
 *	    row = y % 16; col = x % 16;
 *	    DMAP(v,col,row) =def (divN[v] + (modN[v]>magic[col][row] ? 1 : 0))
 *	    DMAP(r,col,row) + DMAP(g,col,row)*levels + DMAP(b,col,row)*levels^2
 *	if you don't want function call overhead.
 */
void dithermap( int levels, double gamma, int rgbmap[][3], int divN[256], int modN[256], int magic[16][16])
{
    double N;
    register int i;
    int levelsq, levelsc;
    int gammamap[256];
    
    for ( i = 0; i < 256; i++ )
	gammamap[i] = (int)(0.5 + 255 * pow( i / 255.0, 1.0/gamma ));

    levelsq = levels*levels;	/* squared */
    levelsc = levels*levelsq;	/* and cubed */

    N = 255.0 / (levels - 1);    /* Get size of each step */

    /* 
     * Set up the color map entries.
     */
    for(i = 0; i < levelsc; i++) {
	rgbmap[i][0] = gammamap[(int)(0.5 + (i%levels) * N)];
	rgbmap[i][1] = gammamap[(int)(0.5 + ((i/levels)%levels) * N)];
	rgbmap[i][2] = gammamap[(int)(0.5 + ((i/levelsq)%levels) * N)];
    }

    make_square( N, divN, modN, magic );
}

/*****************************************************************
 * TAG( bwdithermap )
 * 
 * Create a color dithering map with a specified number of intensity levels.
 * Inputs:
 * 	levels:		Intensity levels.
 *	gamma:		Display gamma value.
 * Outputs:
 * 	bwmap:		Generated black & white map.
 *	divN:		"div" function for dithering.
 *	modN:		"mod" function for dithering.
 * Assumptions:
 * 	bwmap will hold levels entries.
 * Algorithm:
 *	Compute gamma compensation map.
 *	N = 255.0 / (levels - 1) is number of pixel values per level.
 *	Compute bwmap for levels entries.
 *	Call make_square to get divN, modN, and magic.
 * Note:
 *	Call ditherbw( x, y, val, divN, modN, magic ) to get index into 
 *	bwmap for a given color/location pair, or use
 *	    row = y % 16; col = x % 16;
 *	    divN[val] + (modN[val]>magic[col][row] ? 1 : 0)
 *	if you don't want function call overhead.
 *	On a 1-bit display, use
 *	    divN[val] > magic[col][row] ? 1 : 0
 */
void bwdithermap	(int levels, int divN[256], int modN[256], int magic[16][16] )
{
    float N = 255.0f / (levels - 1);    /* Get size of each step */

    make_square( N, divN, modN, magic );
}


/*****************************************************************
 * TAG( make_square )
 * 
 * Build the magic square for a given number of levels.
 * Inputs:
 * 	N:		Pixel values per level (255.0 / levels).
 * Outputs:
 * 	divN:		Integer value of pixval / N
 *	modN:		Integer remainder between pixval and divN[pixval]*N
 *	magic:		Magic square for dithering to N sublevels.
 * Assumptions:
 * 	
 * Algorithm:
 *	divN[pixval] = (int)(pixval / N) maps pixval to its appropriate level.
 *	modN[pixval] = pixval - (int)(N * divN[pixval]) maps pixval to
 *	its sublevel, and is used in the dithering computation.
 *	The magic square is computed as the (modified) outer product of
 *	a 4x4 magic square with itself.
 *	magic[4*k + i][4*l + j] = (magic4x4[i][j] + magic4x4[k][l]/16.0)
 *	multiplied by an appropriate factor to get the correct dithering
 *	range.
 */
void make_square( double N, int divN[256], int modN[256], int magic[16][16])
{
    register int i, j, k, l;
    double magicfact;

    for ( i = 0; i < 256; i++ )
    {
		divN[i] = (int)(i / N);
		modN[i] = i - (int)(N * divN[i]);
    }
    modN[255] = 0;		/* always */

    /*
     * Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
     * and doesn't give us full intensity range (only 17 sublevels).
     * 
     * magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
     * N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
     * pixels incremented to the next level (this is reserved for the
     * pixel value with mod N == 0 at the next level).
     */
    magicfact = (N - 1) / 16.;
    for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	    for ( k = 0; k < 4; k++ )
		for ( l = 0; l < 4; l++ )
		    magic[4*k+i][4*l+j] =
			(int)(0.5 + magic4x4[i][j] * magicfact +
			      (magic4x4[k][l] / 16.) * magicfact);
}

void main(int argc, char* argv[])
{
	CImage		tex;
	tex.LoadTGA	("x:\\dbg\\logo.tga");
	
	int			divN [256];
	int			modN [256];
	int			magic[16][16];
	bwdithermap	(2,divN,modN,magic);

	for (DWORD y=0; y<tex.dwHeight; y++)
	{
		for (DWORD x=0; x<tex.dwWidth; x++)
		{
			DWORD c		= tex.GetPixel(x,y);
			int val		= c&255;

		    int row		= y % 16; 
			int col		= x % 16;
 			int new_c	= val > magic[col][row] ? 255 : 0;
			
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

