// Occlusion.cpp : Defines the entry point for the console application.
//

#include	"stdafx.h"
#include	"occRasterizer.h"
#include	"ftimer.h"

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

const DWORD		scale	= 16;
const DWORD		size	= occ_dim_0*scale;
DWORD			buf		[size*size];

void pixel	(int x, int y, DWORD C=255<<16)
{
	if (x<0) return; else if (x>=(int)size)	return;
	if (y<0) return; else if (y>=(int)size)	return;
	buf [y*size+x]	= C;
}

void line	( int x1, int y1, int x2, int y2 )
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x2 >= x1 ? 1 : -1;
    int sy = y2 >= y1 ? 1 : -1;
	
    if ( dy <= dx ){
        int d  = ( dy << 1 ) - dx;
        int d1 = dy << 1;
        int d2 = ( dy - dx ) << 1;
		
		pixel(x1,y1);
		
        for  (int x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx){
            if ( d > 0){
                d += d2; y += sy;
            }else
                d += d1;
			pixel(x,y);
        }
    }else{
        int d  = ( dx << 1 ) - dy;
        int d1 = dx << 1;
        int d2 = ( dx - dy ) << 1;
		
		pixel(x1,y1);
        for  (int x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy ){
            if ( d > 0){
                d += d2; x += sx;
            }else
                d += d1;
			pixel(x,y);
        }
    }
}

float rad(float a) { return a*3.14159265358f / 180.f; }
const float p_c		= 32.7f;
const float p_r		= 25.4f;
const float p_r2	= 30.4f;
// const float p_a		= 1.f;
const int	offset	= 0;
#define ADJ_NONE	((occTri*)0xffffffff)

void edges(occTri& T)
{
	// draw edges
	line			(int(T.raster[0].x*scale),int(T.raster[0].y*scale),int(T.raster[1].x*scale),int(T.raster[1].y*scale));
	line			(int(T.raster[1].x*scale),int(T.raster[1].y*scale),int(T.raster[2].x*scale),int(T.raster[2].y*scale));
	line			(int(T.raster[2].x*scale),int(T.raster[2].y*scale),int(T.raster[0].x*scale),int(T.raster[0].y*scale));
}
extern	void i_edge ( occRasterizer* OCC, float x1, float y1, float x2, float y2);

CTimer	TM;

class	AAA
{
private:
	CTimer	TM;
public:	
	void test()
	{
		void*	p1		= &::TM;
		void*	p2		= &TM;
		
	}
};

extern void t_test	();
int __cdecl main	(int argc, char* argv[])
{
	CTimer			TM;
	AAA				A;
	
	A.test			();

	void*	p1		= &::TM;
	void*	p2		= &TM;
	

	u64				total=0;
	int				count=0;

	InitMath		();
	printf			("\n");
	t_test			();

	return			0;

//	Raster.clear	();
//	SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//	for (int p_a=-360; p_a<=360; p_a++)
//	{
//		float		a0	= rad(p_a);
//		float		a1	= rad(p_a + 60.f);
//		float		a2	= rad(p_a + 100.f);
//		float		a3	= rad(p_a + 30.f);
//		
//		// setup tri(s)
//		occTri	T1,T2;
//		T1.adjacent[0]	= &T2;
//		T1.adjacent[1]	= ADJ_NONE;
//		T1.adjacent[2]	= ADJ_NONE;
//		T1.raster[0].x	= p_c + p_r*cosf(a0);
//		T1.raster[0].y	= p_c + p_r*sinf(a0);
//		T1.raster[0].z	= 0.1f;
//		
//		T1.raster[1].x	= p_c + p_r*cosf(a1);
//		T1.raster[1].y	= p_c + p_r*sinf(a1);
//		T1.raster[1].z	= 0.1f;
//		
//		T1.raster[2].x	= p_c + p_r*cosf(a2);
//		T1.raster[2].y	= p_c + p_r*sinf(a2);
//		T1.raster[2].z	= 0.9f;
//		
//		T2 = T1;
//		T2.adjacent[0]	= &T1;
//		T2.adjacent[1]	= ADJ_NONE;
//		T2.adjacent[2]	= ADJ_NONE;
//		T2.raster[2].x	= p_c + p_r2*cosf(a3);
//		T2.raster[2].y	= p_c + p_r2*sinf(a3);
//		T2.raster[2].z	= 0.99f;
//		
//		// draw tri
//		TM.Start		();
//		for (int t=0; t<1000; t++)
//		{
//			Raster.rasterize	(&T1);
//			Raster.rasterize	(&T2);
//		}
//		total += TM.GetElapsed();
//		count += 2*1000;
//	}
//	SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
//	DWORD cycles_per_tri	= DWORD(u64(u64(total)/u64(count)));
//	DWORD tpms				= DWORD(u64(u64(CPU::cycles_per_second) / u64(cycles_per_tri)));
//	Msg("Cycles: %d\nTpMS:   %d\nTpS:    %d\n",cycles_per_tri,tpms/1000,tpms);
//	
//	// Propagade
//	Raster.propagade	();
//	
//	// copy into surface
//	for (int y=0; y<occ_dim_0; y++)
//	{
//		for (int x=0; x<occ_dim_0; x++)
//		{
//			float	A	= *(Raster.get_depth_level(0) + y*occ_dim_0 + x);	if (A<0) A=0; else if (A>1) A=1;
//			DWORD  gray	= iFloor(A*255.f);
//			DWORD  mask	= 0; //(*(Raster.get_frame() + y*occ_dim_0 + x)) ? 255 : 0;
//			DWORD  C	= (mask << 24) | (gray << 16) | (gray << 8) | (gray << 0);
//			
//			for (int by=0; by<scale; by++)
//				for (int bx=0; bx<scale; bx++)
//				{
//					DWORD _C = C;
//					if (by==0 && bx==0) _C = 255<<8;
//					buf[(y*scale+by)*size + x*scale+bx]	= _C;
//				}
//		}
//	}
//	
////	edges(T1);
////	edges(T2);
//	
//	// save
//	char name[256];
//	sprintf(name,"c:\\occ%2d.tga",0);
//	TGAdesc	desc;
//	desc.format		= IMG_32B;
//	desc.scanlenght	= size*4;
//	desc.width		= size;
//	desc.height		= size;
//	desc.data		= buf;
//	desc.maketga	(name);
//
//	return 0;
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
