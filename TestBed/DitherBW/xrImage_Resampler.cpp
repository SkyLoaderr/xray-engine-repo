/*
 *		Filtered Image Rescaling
 *
 *		  by Dale Schumacher
 */

#include "stdafx.h"
#pragma hdrstop

#include "xrImage_Resampler.h"

#ifndef RGBA_GETALPHA
#define RGBA_GETALPHA(rgb)      DWORD((rgb) >> 24)
#define RGBA_GETRED(rgb)        DWORD(((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      DWORD(((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       DWORD((rgb) & 0xff)
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define RGBA_MAKE(r,g,b,a)		D3DCOLOR_ARGB(a,r,g,b)
#endif


typedef	DWORD	Pixel;
struct Image 
{
	int		xsize;		/* horizontal size of the image in Pixels */
	int		ysize;		/* vertical size of the image in Pixels */
	Pixel *	data;		/* pointer to first scanline of image */
	int		span;		/* byte offset between two scanlines */
};

#define	WHITE_PIXEL		(255)
#define	BLACK_PIXEL		(0)

/*
 *	generic image access and i/o support routines
 */

Image *	new_image(int xsize, int ysize)		/* create a blank image */
{
	Image *image;

	if((image = (Image *)malloc(sizeof(Image)))
	&& (image->data = (Pixel *)calloc(ysize*xsize,sizeof(Pixel)))) 
	{
		image->xsize	= xsize;
		image->ysize	= ysize;
		image->span		= xsize;
	}
	return	(image);
}

void	free_image(Image* image)
{
	_FREE(image->data);
	_FREE(image);
}

Pixel	get_pixel	(Image* image, int x, int y)
{
	if((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize)) return 0;
	return image->data[(y * image->span) + x];
}

void	get_row		(Pixel* row, Image* image, int y)
{
	if((y < 0) || (y >= image->ysize)) return;
	CopyMemory(row,	image->data + (y * image->span), (sizeof(Pixel) * image->xsize));
}

void	get_column	(Pixel* column, Image* image, int x)
{
	int i, d;
	Pixel *p;

	if((x < 0) || (x >= image->xsize)) return;

	d	= image->span;
	for(i = image->ysize, p = image->data + x; i-- > 0; p += d) {
		*column++ = *p;
	}
}

Pixel	put_pixel	(Image* image, int x, int y, Pixel data)
{
	if((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize)) return 0;
	return	image->data[(y * image->span)+x] = data;
}


/*
 *	filter function definitions
 */

//
#define	filter_support		(1.0)
double	filter				(double t)
{
	/* f(t) = 2|t|^3 - 3|t|^2 + 1, -1 <= t <= 1 */
	if(t < 0.0) t = -t;
	if(t < 1.0) return((2.0 * t - 3.0) * t * t + 1.0);
	return(0.0);
}

//
#define	box_support			(0.5)
double	box_filter			(double t)
{
	if((t > -0.5) && (t <= 0.5)) return(1.0);
	return(0.0);
}

//
#define	triangle_support	(1.0)
double	triangle_filter		(double t)
{
	if(t < 0.0) t = -t;
	if(t < 1.0) return(1.0 - t);
	return(0.0);
}

// 
#define	bell_support		(1.5)
double	bell_filter			(double t)		/* box (*) box (*) box */
{
	if(t < 0) t = -t;
	if(t < .5) return(.75 - (t * t));
	if(t < 1.5) {
		t = (t - 1.5);
		return(.5 * (t * t));
	}
	return(0.0);
}

// 
#define	B_spline_support	(2.0)
double	B_spline_filter		(double t)	/* box (*) box (*) box (*) box */
{
	double tt;

	if(t < 0) t = -t;
	if(t < 1) {
		tt = t * t;
		return((.5 * tt * t) - tt + (2.0 / 3.0));
	} else if(t < 2) {
		t = 2 - t;
		return((1.0 / 6.0) * (t * t * t));
	}
	return(0.0);
}

// 
#define	Lanczos3_support	(3.0)
double	sinc				(double x)
{
	x *= 3.1415926f;
	if(x != 0) return(sin(x) / x);
	return(1.0);
}
double	Lanczos3_filter		(double t)
{
	if(t < 0) t = -t;
	if(t < 3.0) return(sinc(t) * sinc(t/3.0));
	return(0.0);
}

// 
#define	Mitchell_support	(2.0)
#define	B	(1.0 / 3.0)
#define	C	(1.0 / 3.0)

double	Mitchell_filter		(double t)
{
	double tt;

	tt = t * t;
	if(t < 0) t = -t;
	if(t < 1.0) {
		t = (((12.0 - 9.0 * B - 6.0 * C) * (t * tt))
		   + ((-18.0 + 12.0 * B + 6.0 * C) * tt)
		   + (6.0 - 2 * B));
		return(t / 6.0);
	} else if(t < 2.0) {
		t = (((-1.0 * B - 6.0 * C) * (t * tt))
		   + ((6.0 * B + 30.0 * C) * tt)
		   + ((-12.0 * B - 48.0 * C) * t)
		   + (8.0 * B + 24 * C));
		return(t / 6.0);
	}
	return(0.0);
}

/*
 *	image rescaling routine
 */

struct CONTRIB	
{
	int		pixel;
	double	weight;
};

struct CLIST 
{
	int		n;					/* number of contributors */
	CONTRIB	*p;					/* pointer to list of contributions */
};

DWORD	CC	(double a)
{
	int	p		= iFloor(float(a));
	if	(p<0)	return 0; else if (p>255) return 255;
	return p;
}

void	imf_Process	(LPDWORD dstI, DWORD dstW, DWORD dstH, LPDWORD srcI, DWORD srcW, DWORD srcH, EIMF_Type FILTER)
{
	// SRC & DST images
	Image		src;	src.xsize	= srcW;	src.ysize	= srcH;	src.data	= srcI;	src.span	= srcW;
	Image		dst;	dst.xsize	= dstW;	dst.ysize	= dstH;	dst.data	= dstI;	dst.span	= dstW;

	// Select filter
	double		(*filterf)(double);	filterf		= 0;
	double		fwidth	= 0;
	switch		(FILTER)
	{
	case imf_filter:	filterf=filter;				fwidth = filter_support;	break;
	case imf_box:		filterf=box_filter;			fwidth = box_support;		break;
	case imf_triangle:	filterf=triangle_filter;	fwidth = triangle_support;	break;
	case imf_bell:		filterf=bell_filter;		fwidth = bell_support;		break;
	case imf_b_spline:	filterf=B_spline_filter;	fwidth = B_spline_support;	break;
	case imf_lanczos3:	filterf=Lanczos3_filter;	fwidth = Lanczos3_support;	break;
	case imf_mitchell:	filterf=Mitchell_filter;	fwidth = Mitchell_support;	break;
	}


	//
	Image	*tmp;					/* intermediate image */
	double	xscale, yscale;			/* zoom scale factors */
	int		i, j, k;				/* loop variables */
	int		n;						/* pixel number */
	double	center, left,	right;	/* filter calculation variables */
	double	width,	fscale, weight;	/* filter calculation variables */
	Pixel	*raster;				/* a row or column of pixels */
	CLIST	*contrib;				/* array of contribution lists */

	/* create intermediate image to hold horizontal zoom */
	tmp		= new_image	(dst.xsize, src.ysize);
	xscale	= double	(dst.xsize) / double(src.xsize);
	yscale	= double	(dst.ysize) / double(src.ysize);

	/* pre-calculate filter contributions for a row */
	contrib = (CLIST *)calloc	(dst.xsize, sizeof(CLIST));
	if(xscale < 1.0) {
		width	= fwidth / xscale;
		fscale	= 1.0 / xscale;
		for(i = 0; i < dst.xsize; ++i) 
		{
			contrib[i].n	= 0;
			contrib[i].p	= (CONTRIB *)calloc((int) (width * 2 + 1),	sizeof(CONTRIB));
			center			= double(i) / xscale;
			left			= ceil	(center - width);
			right			= floor	(center + width);
			for(j = int(left); j <= int(right); ++j) 
			{
				weight	= center - double(j);
				weight	= filterf(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= src.xsize) {
					n = (src.xsize - j) + src.xsize - 1;
				} else {
					n = j;
				}
				k		= contrib[i].n++;
				contrib[i].p[k].pixel	= (n<src.xsize)?n:(src.xsize-1);
				contrib[i].p[k].weight	= weight;
			}
		}
	} else {
		for(i = 0; i < dst.xsize; ++i) 
		{
			contrib[i].n	= 0;
			contrib[i].p	= (CONTRIB *)calloc((int) (fwidth * 2 + 1),	sizeof(CONTRIB));
			center			= double(i) / xscale;
			left			= ceil	(center - fwidth);
			right			= floor	(center + fwidth);
			for(j = int(left); j <= int(right); ++j) 
			{
				weight	= center - (double) j;
				weight	= (*filterf)(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= src.xsize) {
					n = (src.xsize - j) + src.xsize - 1;
				} else {
					n = j;
				}
				k		= contrib[i].n++;
				contrib[i].p[k].pixel	= (n<src.xsize)?n:(src.xsize-1);
				contrib[i].p[k].weight	= weight;
			}
		}
	}

	/* apply filter to zoom horizontally from src to tmp */
	raster	= (Pixel *)calloc(src.xsize, sizeof(Pixel));
	for	(k = 0; k < tmp->ysize; ++k) 
	{
		get_row	(raster, &src, k);
		for(i = 0; i < tmp->xsize; ++i) 
		{
			double	w_r	= 0., w_g = 0., w_b	= 0., w_a = 0.;

			for	(j = 0; j < contrib[i].n; ++j) 
			{
				double	W	=	contrib[i].p[j].weight;
				Pixel	P	=	raster[contrib[i].p[j].pixel];
				w_r			+=	W*double(RGBA_GETRED(P));
				w_g			+=	W*double(RGBA_GETGREEN(P));
				w_b			+=	W*double(RGBA_GETBLUE(P));
				w_a			+=	W*double(RGBA_GETALPHA(P));
			}
			put_pixel(tmp, i, k, RGBA_MAKE(CC(w_r),CC(w_g),CC(w_b),CC(w_a+.5)));
		}
	}
	_FREE(raster);

	/* _FREE the memory allocated for horizontal filter weights */
	for(i = 0; i < tmp->xsize; ++i) _FREE(contrib[i].p);
	_FREE(contrib);

	/* pre-calculate filter contributions for a column */
	contrib = (CLIST *)calloc(dst.ysize, sizeof(CLIST));
	if(yscale < 1.0) {
		width	= fwidth / yscale;
		fscale	= 1.0 / yscale;
		for	(i = 0; i < dst.ysize; ++i) 
		{
			contrib[i].n	= 0;
			contrib[i].p	= (CONTRIB *)calloc((int) (width * 2 + 1),sizeof(CONTRIB));
			center			= (double) i / yscale;
			left			= ceil	(center - width);
			right			= floor	(center + width);
			for(j = int(left); j <= int(right); ++j) 
			{
				weight	= center - (double) j;
				weight	= filterf(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= tmp->ysize) {
					n = (tmp->ysize - j) + tmp->ysize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel	= (n<tmp->ysize)?n:(tmp->ysize-1);
				contrib[i].p[k].weight	= weight;
			}
		}
	} else {
		for(i = 0; i < dst.ysize; ++i) 
		{
			contrib[i].n	= 0;
			contrib[i].p	= (CONTRIB *)calloc((int) (fwidth * 2 + 1),sizeof(CONTRIB));
			center			= (double) i / yscale;
			left			= ceil	(center - fwidth);
			right			= floor	(center + fwidth);
			for(j = int(left); j <= int(right); ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= tmp->ysize) {
					n = (tmp->ysize - j) + tmp->ysize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel	= (n<tmp->ysize)?n:(tmp->ysize-1);
				contrib[i].p[k].weight	= weight;
			}
		}
	}

	/* apply filter to zoom vertically from tmp to dst */
	raster = (Pixel *)calloc(tmp->ysize, sizeof(Pixel));
	for(k = 0; k < dst.xsize; ++k) 
	{
		get_column	(raster, tmp, k);
		for(i = 0; i < dst.ysize; ++i) 
		{
			double	w_r	= 0., w_g = 0., w_b	= 0., w_a = 0.;

			for	(j = 0; j < contrib[i].n; ++j) 
			{
				double	W	=	contrib[i].p[j].weight;
				Pixel	P	=	raster[contrib[i].p[j].pixel];
				w_r			+=	W*double(RGBA_GETRED(P));
				w_g			+=	W*double(RGBA_GETGREEN(P));
				w_b			+=	W*double(RGBA_GETBLUE(P));
				w_a			+=	W*double(RGBA_GETALPHA(P));
			}
			put_pixel(&dst, k, i, RGBA_MAKE(CC(w_r),CC(w_g),CC(w_b),CC(w_a+.5)));
		}

	}
	_FREE(raster);

	/* _FREE the memory allocated for vertical filter weights */
	for	(i = 0; i < dst.ysize; ++i) _FREE(contrib[i].p);
	_FREE(contrib);

	free_image(tmp);
}

