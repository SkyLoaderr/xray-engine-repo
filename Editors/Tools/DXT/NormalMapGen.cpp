#include "stdafx.h"

#include "NV_Common.h"
#include "NVI_Convolution.h"
#include "NVI_Image.h"


enum KernelType
{
	KERNEL_4x,
	KERNEL_3x3,
	KERNEL_5x5,
	KERNEL_7x7,
	KERNEL_9x9,
	KERNEL_FORCEDWORD = 0xffffffff
};


//////////////////////////////////////////////////////////////////////
// Function prototypes

void MakeKernelElems( const float * pInWeightArray, int num_x, int num_y, ConvolutionKernelElement * pOutArray );
void RotateArrayCCW( const float * pInArray, int num_x, int num_y, float * pOutArray );


void ConvertAlphaToNormalMap_4x	( float scale, bool wrap );
void ConvertAlphaToNormalMap_3x3( float scale, bool wrap );
void ConvertAlphaToNormalMap_5x5( float scale, bool wrap );
void ConvertAlphaToNormalMap_7x7( float scale, bool wrap );
void ConvertAlphaToNormalMap_9x9( float scale, bool wrap );

//////////////////////////////////////////////////////////
u8		fpack		(float v)                               
{
	s32     _v      = iFloor        (((v+1)*.5f)*255.f + .5f);
	clamp   (_v,0,255);
	return  u8(_v);
}
Fvector vunpack		(s32 x, s32 y, s32 z)   
{
	Fvector pck;
	pck.x   = (float(x)/255.f - .5f)*2.f;
	pck.y   = (float(y)/255.f - .5f)*2.f;
	pck.z   = (float(z)/255.f - .5f)*2.f;
	return  pck;
}
Fvector vunpack		(Ivector src)                   
{
	return  vunpack (src.x,src.y,src.z);
}
Ivector	vpack      (Fvector src)
{
	src.normalize           ();
	Fvector                 _v;
	int bx                  = fpack (src.x);
	int by                  = fpack (src.y);
	int bz                  = fpack (src.z);
	// dumb test
	float   e_best  = flt_max;
	int             r=bx,g=by,b=bz;
	int             d=3;
	for (int x=_max(bx-d,0); x<=_min(bx+d,255); x++){
		for (int y=_max(by-d,0); y<=_min(by+d,255); y++){
			for (int z=_max(bz-d,0); z<=_min(bz+d,255); z++){
				_v                      = vunpack(x,y,z);
				float   m               = _v.magnitude();
				float   me              = _abs(m-1.f);
				if      (me>0.03f)      continue;
				_v.div  (m);
				float   e               = _abs(src.dotproduct(_v)-1.f);
				if (e<e_best)   {
					e_best          = e;
					r=x,g=y,b=z;
				}
			}
		}
	}
	Ivector         ipck;
	ipck.set        (r,g,b);
	return          ipck;
}

void 	CalculateNormalMap( NVI_Image* pSrc, ConvolutionKernel * pKernels, int num_kernels, float scale, bool wrap )
{
	// pKernels must be an array of at least two kernels
	// The first kernel extracts dh/dx  (change in height with respect to x )
	// the second extracts dh/dy  (change in height with respect to y )

	VERIFY(pKernels);
	VERIFY(num_kernels == 2);

	float			     results[2];

	////////////////////////////////////////////////////////
	// Set up the convolver & prepare image data

	Convolver	conv;

	conv.Initialize( &pSrc, pKernels, num_kernels, wrap );

	int size_y, size_x;

	size_x = (int) pSrc->GetWidth();
	size_y = (int) pSrc->GetHeight();
	DWORD * pArray = (DWORD*) pSrc->GetImageDataPointer();

	assert( pArray != NULL );

	/////////////////////////////////////////////////////
	// Now run the kernel over the source image area
	//  and write out the values.

	int				i,j;
	float			du, dv;
	unsigned long	nmap_color;
	char			height;

	// coordinates of source image (not padded)
	for( j=0; j < size_y; j++ )
	{
		for( i=0; i < size_x; i++ )
		{
			// apply kernels

			conv.Convolve_Alpha_At( i,j, results, 2 );

			du = results[0] * scale;
			dv = results[1] * scale;

			// det  | x  y  z |
			//      | 1  0 du |
			//      | 0  1 dv |
			//
			// cross product gives (-du, -dv, 1.0) as normal 

			float mag = du*du + dv*dv + 1.0f;
			mag = (float) _sqrt( mag );

			// Get alpha as height
			height = (char) ( pArray[ j * size_x + i ] ) >> 24;

//.			AlphaAndVectorToARGB( height, -du/mag, -dv/mag, 1.0f / mag, nmap_color );


			pArray[ j * size_x + i ] = nmap_color;
		}
	}
	xr_delete(pSrc);
}



void ConvertAlphaToNormalMap_4x( NVI_Image* pSrc, float scale, bool wrap )
{
	// Do the conversion using a 4 sample nearest neighbor
	//  pattern

	// d height / du kernel:
	//  0		0		0
	//	-1/2	0		1/2
	//	0		0		0

	// d height / dv kernel:
	//	0	1/2		0
	//	0	0		0
	//	0	-1/2	0

	int numelem;				// num elements in each kernel


	numelem = 2;
	ConvolutionKernelElement	du_elem[] =	{
		{-1, 0, -1.0f/2.0f}, { 1, 0, 1.0f/2.0f}			};


	ConvolutionKernelElement	dv_elem[] = {
			{ 0,  1,  1.0f/2.0f},
			{ 0, -1, -1.0f/2.0f}			 };

	
	int num_kernels = 2;
	ConvolutionKernel	 kernels[2];

	kernels[0].SetElements( numelem, du_elem );
	kernels[1].SetElements( numelem, dv_elem );


	///////////////////////////////////////////////////////
	//  Calc ARGB normal map & write to the "in." file

	CalculateNormalMap( pSrc, kernels, num_kernels, scale, wrap );


}


void ConvertAlphaToNormalMap_3x3( NVI_Image* pSrc, float scale, bool wrap )
{
	// Uses Anders' 3x3 kernels for transforming height 
	//  into a normal map vector.
	// Either wraps or does not wrap.
	// The convolver class memcopies to larger surface (width + kernel width-1, 
	//		height + kernel heigh - 1 ) to make wrap code easy for arbitrary
	//		kernels.  Edge texels are duplicated into the border regions or
	//      copied from the other side of the source image if wrapping is on.


	int numelem;				// num elements in each kernel

	float wt = 1.0f / 6.0f;

	// Kernel for change of height in u axis:
	//  -1/6	0	1/6
	//  -1/6	0	1/6
	//  -1/6	0	1/6

	numelem = 6;
	ConvolutionKernelElement	du_elem[] =	{
		{-1, 1, -wt}, { 1, 1, wt},
		{-1, 0, -wt}, { 1, 0, wt},
		{-1,-1, -wt}, { 1,-1, wt}	};

	// Kernel for change of height in v axis:
	//						 1,1
	//   1/6	 1/6	 1/6
	//     0	   0	   0
	//  -1/6	-1/6	-1/6
	// 0,0

	ConvolutionKernelElement	dv_elem[] = {
		{-1, 1,  wt}, { 0, 1, wt}, { 1, 1,  wt },
		{-1,-1, -wt}, { 0,-1,-wt}, { 1,-1, -wt } };

	
	int num_kernels = 2;
	ConvolutionKernel	 kernels[2];

	kernels[0].SetElements( numelem, du_elem );
	kernels[1].SetElements( numelem, dv_elem );


	///////////////////////////////////////////////////////
	//  Calc ARGB normal map & write to the "in." file

	CalculateNormalMap( pSrc, kernels, num_kernels, scale, wrap );

}



void ConvertAlphaToNormalMap_5x5( NVI_Image* pSrc, float scale, bool wrap )
{
	// Either wraps or does not wrap.
	// The convolver class memcopies to larger surface (width + kernel width-1, 
	//		height + kernel heigh - 1 ) to make wrap code easy for arbitrary
	//		kernels.  Edge texels are duplicated into the border regions or
	//      copied from the other side of the source image if wrapping is on.

	int numelem;				// num elements in each kernel


	float wt1 = 1.0f / 6.0f;
	float wt2 = 1.0f / 48.0f;

/*
	// Kernel for change of height in u axis:
	// The are cubic coefs for interpolation with sample 
	//  points at -2, -1, 1, and 2, hence the 0 coefs along
	//  center axis.  Resulting pattern is undesirable for 
	//  outstanding points in the height field.  These become
	//  a region of 4 mounds, when they should instead become a 
	//  smooth blob.

	//	1/48	-1/6	0	1/6		-1/48
	//	1/48	-1/6	0	1/6		-1/48
	//	0		0		0	0		0
	//	1/48	-1/6	0	1/6		-1/48
	//	1/48	-1/6	0	1/6		-1/48


	numelem = 16;
	ConvolutionKernelElement	du_elem[] =	{
		{-2, 2, wt2}, {-1,2,-wt1},    {1,2,wt1}, {2,2,-wt2},
		{-2, 1, wt2}, {-1,1,-wt1},    {1,1,wt1}, {2,1,-wt2},

		{-2,-1, wt2}, {-1,-1,-wt1},   {1,-1,wt1}, {2,-1,-wt2},
		{-2,-2, wt2}, {-1,-2,-wt1},   {1,-2,wt1}, {2,-2,-wt2}   };

	// Kernel for change of height in v axis:
	// This is same as u kernel above - just rotated 90 degrees

	ConvolutionKernelElement	dv_elem[] = {
		{-2, 2,-wt2}, {-1,2,-wt2},    {1,2,-wt2}, {2,2,-wt2},
		{-2, 1, wt1}, {-1,1, wt1},    {1,1, wt1}, {2,1, wt1},

		{-2,-1,-wt1}, {-1,-1,-wt1},   {1,-1,-wt1}, {2,-1,-wt1},
		{-2,-2, wt2}, {-1,-2, wt2},   {1,-2, wt2}, {2,-2, wt2}   };
*/


	

	numelem = 20;
	float wt22 = 1.0f/16.0f;
	float wt12 = 1.0f/10.0f;
	float wt02 = 1.0f/8.0f;
	float wt11 = 1.0f/2.8f;

	// Kernels using slope based on distance of that point from the 0,0
	// This is not from math derivation, but makes nice result

	ConvolutionKernelElement	du_elem[] =	{
		{-2, 2,-wt22 }, {-1, 2,-wt12 },   {1, 2, wt12 }, {2, 2, wt22 },
		{-2, 1,-wt12 }, {-1, 1,-wt11 },   {1, 1, wt11 }, {2, 1, wt12 },

		{-2, 0,-wt02 }, {-1, 0,-1.0f/2.0f },   {1, 0, 1.0f/2.0f }, {2, 0, wt02 },

		{-2,-1,-wt12 }, {-1,-1,-wt11 },   {1,-1, wt11 }, {2,-1, wt12 },
		{-2,-2,-wt22 }, {-1,-2,-wt12 },   {1,-2, wt12 }, {2,-2, wt22 }   };

	ConvolutionKernelElement	dv_elem[] = {
		{-2, 2, wt22 }, {-1, 2, wt12 }, {0, 2, 1.0f/4.0f },  {1, 2, wt12 }, {2, 2, wt22 },
		{-2, 1, wt12 }, {-1, 1, wt11 }, {0, 1, 1.0f/2.0f },  {1, 1, wt11 }, {2, 1, wt12 },

		{-2,-1,-wt12 }, {-1,-1,-wt11 }, {0,-1,-1.0f/2.0f },  {1,-1,-wt11 }, {2,-1,-wt12 },
		{-2,-2,-wt22 }, {-1,-2,-wt12 }, {0,-2,-1.0f/4.0f },  {1,-2,-wt12 }, {2,-2,-wt22 }   };


	// normalize the kernel so abs of all 
	// weights add to one
		
	int i;
	float usum, vsum;
	usum = vsum = 0.0f;
	for( i=0; i < numelem; i++ )
	{
		usum += (float) _abs( du_elem[i].weight );
		vsum += (float) _abs( dv_elem[i].weight );
	}
	for( i=0; i < numelem; i++ )
	{
		du_elem[i].weight /= usum;
		dv_elem[i].weight /= vsum;
	}



	int num_kernels = 2;
	ConvolutionKernel	 kernels[2];

	kernels[0].SetElements( numelem, du_elem );
	kernels[1].SetElements( numelem, dv_elem );


	///////////////////////////////////////////////////////
	//  Calc ARGB normal map & write to the "in." file

	CalculateNormalMap( pSrc, kernels, num_kernels, scale, wrap );

}


void MakeKernelElems( const float * pInWeightArray, int num_x, int num_y,
						ConvolutionKernelElement * pOutArray )
{
	// This makes coordinates for an array of weights, assumed to 
	//  be a rectangle.
	//
	// You must allocate pOutArray outside the function!
	// num_x and num_y should be odd
	//
	// Specify elems from  upper left corner (-num_x/2, num_y/2) to
	//   lower right corner. 
	// This generates the coordinates of the samples for you
	// For example:
	//  elem_array[] = { 00, 11, 22, 33, 44, 55, 66, 77, 88 }
	//  MakeKernelsElems( elem_array, 3, 3 )
	//  would make:
	//
	//   { -1, 1, 00 }  { 0, 1, 11 }  { 1, 1, 22 }
	//   { -1, 0, 33 }  { 0, 0, 44 }  { 1, 0, 55 }
	//   { -1, -1, 66}  ..etc 
	//
	//  As the array of kernel elements written to pOutArray
	//
	
	assert( pOutArray != NULL );
	assert( pInWeightArray != NULL );

	int i,j;
	int ind;

	for( j=0; j < num_y; j++ )
	{
		for( i=0; i < num_x; i++ )
		{
			ind = i + j * num_x;
			
			assert( ind < num_x * num_y );

			pOutArray[ ind ].x_offset =  (int)( i - num_x / 2 );
			pOutArray[ ind ].y_offset =  (int)( num_y / 2 - j );
			pOutArray[ ind ].weight   = pInWeightArray[ind];
		}
	}
}

void 	RotateArrayCCW( float * pInArray, int num_x, int num_y, float * pOutArray )
{
	// rotate an array of floats 90 deg CCW, so
	//    1, 0
	//    2, 3
	//  becomes
	//    0, 3
	//    1, 2

	assert( pOutArray != NULL );
	assert( pInArray != NULL );

	int i,j;
	int newi, newj;

	float * pSrc;

	////////////////////////////////////////////////////
	// If arrays are same, copy source to new temp array

	if( pInArray == pOutArray )
	{
		pSrc = new float[ num_x * num_y ];
		assert( pSrc != NULL );

		for( i=0; i < num_x * num_y; i++ )
		{
			pSrc[i] = pInArray[ i ];
		}
	}
	else
	{
		pSrc = pInArray;
	}
	////////////////////////////////////////////////////

	for( j=0; j < num_y; j++ )
	{
		for( i=0; i < num_x; i++ )
		{
			newj = num_x - i - 1;
			newi = j;

			// rotate dims of array too ==>  j * num_y

			pOutArray[ newi + newj * num_y ] = pSrc[ i + j * num_x ];
		}
	}


	if( pInArray == pOutArray )
	{
		SAFE_ARRAY_DELETE( pSrc );
	}	
}


void ConvertAlphaToNormalMap_7x7( NVI_Image* pSrc, float scale, bool wrap )
{
	// Either wraps or does not wrap.
	// The convolver class memcopies to larger surface (width + kernel width-1, 
	//		height + kernel heigh - 1 ) to make wrap code easy for arbitrary
	//		kernels.  Edge texels are duplicated into the border regions or
	//      copied from the other side of the source image if wrapping is on.

	int numelem;				// num elements in each kernel


	/////////////////////////////////////////
	// Kernel for change of height in u axis:
	// A Sobel filter kernel

	numelem = 49;


	float du_f[] = {
			-1, -2, -3,  0,  3, 2, 1,
			-2, -3, -4,  0,  4, 3, 2,
			-3, -4, -5,  0,  5, 4, 3,
			-4, -5, -6,  0,  6, 5, 4,
			-3, -4, -5,  0,  5, 4, 3,
			-2, -3, -4,  0,  4, 3, 2,
			-1, -2, -3,  0,  3, 2,	1   };



	ConvolutionKernelElement	du_elem[49];
	
	MakeKernelElems( du_f, 7, 7, & (du_elem[0] ) );

	/////////////////////////////////////////////
	// Kernel for change of height in v axis:

	float dv_f[49];
	
	RotateArrayCCW( &( du_f[0] ), 7, 7,  &( dv_f[0] ) );

	ConvolutionKernelElement	dv_elem[49];
	
	MakeKernelElems( dv_f, 7, 7, & ( dv_elem[0] ) );

	/////////////////////////////////////////////

	///////////////////////////////////////////
	// normalize the kernels so abs of all 
	// weights add to one
		
	int i;
	float usum, vsum;
	usum = vsum = 0.0f;

	for( i=0; i < numelem; i++ )
	{
		usum += (float) _abs( du_elem[i].weight );
		vsum += (float) _abs( dv_elem[i].weight );
	}

	for( i=0; i < numelem; i++ )
	{
		du_elem[i].weight /= usum;
		dv_elem[i].weight /= vsum;
	}

	///////////////////////////////////////////
	int num_kernels = 2;
	ConvolutionKernel	 kernels[2];

	kernels[0].SetElements( numelem, du_elem );
	kernels[1].SetElements( numelem, dv_elem );


	///////////////////////////////////////////////////////
	//  Calc ARGB normal map & write to the "in." file

	CalculateNormalMap( pSrc, kernels, num_kernels, scale, wrap );

}


void ConvertAlphaToNormalMap_9x9( NVI_Image* pSrc, float scale, bool wrap )
{
	// Either wraps or does not wrap.
	// The convolver class memcopies to larger surface (width + kernel width-1, 
	//		height + kernel heigh - 1 ) to make wrap code easy for arbitrary
	//		kernels.  Edge texels are duplicated into the border regions or
	//      copied from the other side of the source image if wrapping is on.
	//

	int numelem;				// num elements in each kernel


	/////////////////////////////////////////
	// Kernel for change of height in u axis:
	// A Sobel filter kernel

	numelem = 81;

	float du_f[] = {
			-1, -2, -3, -4,  0,  4, 3, 2, 1,
			-2, -3, -4, -5,  0,  5, 4, 3, 2,
			-3, -4, -5, -6,  0,  6, 5, 4, 3,
			-4, -5, -6, -7,  0,  7, 6, 5, 4,
			-5, -6, -7, -8,  0,  8, 7, 6, 5,
			-4, -5, -6, -7,  0,  7, 6, 5, 4,
			-3, -4, -5, -6,  0,  6, 5, 4, 3,
			-2, -3, -4, -5,  0,  5, 4, 3, 2,
			-1, -2, -3, -4,  0,  4, 3, 2, 1		};


	ConvolutionKernelElement	du_elem[81];
	
	MakeKernelElems( du_f, 9, 9, & (du_elem[0] ) );

	/////////////////////////////////////////////
	// Kernel for change of height in v axis:

	float dv_f[81];
	
	RotateArrayCCW( &( du_f[0] ), 9, 9,  &( dv_f[0] ) );

	ConvolutionKernelElement	dv_elem[81];
	
	MakeKernelElems( dv_f, 9, 9, & ( dv_elem[0] ) );

	/////////////////////////////////////////////

	///////////////////////////////////////////
	// normalize the kernels so abs of all 
	// weights add to one
		
	int i;
	float usum, vsum;
	usum = vsum = 0.0f;

	for( i=0; i < numelem; i++ )
	{
		usum += (float) _abs( du_elem[i].weight );
		vsum += (float) _abs( dv_elem[i].weight );
	}
	for( i=0; i < numelem; i++ )
	{
		du_elem[i].weight /= usum;
		dv_elem[i].weight /= vsum;
	}

	///////////////////////////////////////////
	int num_kernels = 2;
	ConvolutionKernel	 kernels[2];

	kernels[0].SetElements( numelem, du_elem );
	kernels[1].SetElements( numelem, dv_elem );


	///////////////////////////////////////////////////////
	//  Calc ARGB normal map & write to the "in." file

	CalculateNormalMap( pSrc, kernels, num_kernels, scale, wrap );

}

void ConvertToNormalMap(u8* src_data, int src_width, int src_height, KernelType kt, float scale)
{
	NVI_Image* pSrc		= xr_new<NVI_Image>();
	pSrc->Initialize	(src_width,src_height,NVI_A8_R8_G8_B8,src_data);
	pSrc->AverageRGBToAlpha();

	switch (kt){
	case KERNEL_4x:		ConvertAlphaToNormalMap_4x	(pSrc,scale,true);	break;
	case KERNEL_3x3:	ConvertAlphaToNormalMap_3x3	(pSrc,scale,true);	break;
	case KERNEL_5x5:	ConvertAlphaToNormalMap_5x5	(pSrc,scale,true);	break;
	case KERNEL_7x7:	ConvertAlphaToNormalMap_7x7	(pSrc,scale,true);	break;
	case KERNEL_9x9:	ConvertAlphaToNormalMap_9x9	(pSrc,scale,true);	break;
	default: NODEFAULT;
	}

	xr_delete(pSrc);
}
