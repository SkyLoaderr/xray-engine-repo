#include "stdafx.h"
#include "build.h"
#include "math.h"
#include "cl_defs.h"
#include "cl_intersect.h"
#include "std_classes.h"
#include "xrDXTC.h"
#include "tga.h"

void Jitter_Select(UVpoint* &Jitter, DWORD& Jcount)
{
	static UVpoint Jitter1[1] = {
		{0,0}
	};
	static UVpoint Jitter4[4] = {
		{-1,-1}, {1,-1}, {1,1}, {-1,1}
	};
	static UVpoint Jitter9[9] = {
		{-1,-1},	{0,-1},		{1,-1}, 
		{-1,0},		{0,0},		{1,0},
		{-1,1},		{0,1},		{1,1}
	};

	switch (g_params.m_lm_jitter_samples)
	{
		case 1:
			Jcount	= 1;
			Jitter	= Jitter1;
			break;
		case 9:
			Jcount	= 9;
			Jitter	= Jitter9;
			break;
		case 4:
		default:
			Jcount	= 4;
			Jitter	= Jitter4;
			break;
	}
}

IC void GET(
			b_texture &lm, 
			int x, int y, DWORD ref, DWORD &count, 
			DWORD &r, DWORD &g, DWORD &b)
{
	// wrap pixels
	if (x<0) return;
	else if (x>=(int)lm.dwWidth)	return;
	if (y<0) return;
	else if (y>=(int)lm.dwHeight)	return;

	// summarize
	DWORD pixel = lm.pSurface[y*lm.dwWidth + x];
	if (RGBA_GETALPHA(pixel)<=ref) return;

	r+=RGBA_GETRED	(pixel);
	g+=RGBA_GETGREEN(pixel);
	b+=RGBA_GETBLUE	(pixel);
	count++;
}
 
BOOL ApplyBorders(b_texture &lm, DWORD ref) 
{
	BOOL	bNeedContinue = FALSE;

	DWORD	result[512*512];

	R_ASSERT(lm.dwHeight<=512	);
	R_ASSERT(lm.dwWidth<=512	);

	CopyMemory(result,lm.pSurface,lm.dwHeight*lm.dwWidth*4);
	for (int y=0; y<(int)lm.dwHeight; y++) {
		for (int x=0; x<(int)lm.dwWidth; x++)
		{
			if (RGBA_GETALPHA(lm.pSurface[y*lm.dwWidth+x])==0) {
				
				DWORD C=0,r=0,g=0,b=0;
				GET(lm,x-1,y-1,ref,C,r,g,b);
				GET(lm,x  ,y-1,ref,C,r,g,b);
				GET(lm,x+1,y-1,ref,C,r,g,b);
				
				GET(lm,x-1,y  ,ref,C,r,g,b);
				GET(lm,x+1,y  ,ref,C,r,g,b);
				
				GET(lm,x-1,y+1,ref,C,r,g,b);
				GET(lm,x  ,y+1,ref,C,r,g,b);
				GET(lm,x+1,y+1,ref,C,r,g,b);
				
				if (C) {
					result[y*lm.dwWidth+x]=RGBA_MAKE(r/C,g/C,b/C,ref);
					bNeedContinue = TRUE;
				}
			}
		}
	}
	CopyMemory(lm.pSurface,result,lm.dwHeight*lm.dwWidth*4);
	return bNeedContinue;
}

float getLastRP_Scale(RAPID::XRCollide* DB, R_Light& L)
{
	DWORD	tris_count  = DB->GetRayContactCount();
	float	scale		= 1.f;
	Fvector B;

	for (DWORD I=0; I<tris_count; I++)
	{
		RAPID::raypick_info& rpinf = DB->RayContact[I];

		// Access to texture
		Face* F										= (Face*)(RCAST_Model.tris[rpinf.id].dummy);
		if (0==F)									continue;

		SH_ShaderDef&	SH							= F->Shader();
		if (!SH.C.bCastShadow)						continue;
		
		if (F->bOpaque)		{
			// Opaque poly - cache it
			L.tri[0].set	(rpinf.p[0]);
			L.tri[1].set	(rpinf.p[1]);
			L.tri[2].set	(rpinf.p[2]);
			return 0;
		}

		b_material& M	= pBuild->materials			[F->dwMaterial];
		b_texture&	T	= pBuild->textures			[M.surfidx[0]];

		// barycentric coords
		// note: W,U,V order
		B.set(1.0f - rpinf.u - rpinf.v,rpinf.u,rpinf.v);
		
		// calc UV
		_TCF	&C = F->tc[0];
		UVpoint uv;
		uv.u = C.uv[0].u*B.x + C.uv[1].u*B.y + C.uv[2].u*B.z;
		uv.v = C.uv[0].v*B.x + C.uv[1].v*B.y + C.uv[2].v*B.z;
		
		int U = iFloor(uv.u*float(T.dwWidth) + .5f);
		int V = iFloor(uv.v*float(T.dwHeight)+ .5f);
		U %= T.dwWidth;		if (U<0) U+=T.dwWidth;
		V %= T.dwHeight;	if (V<0) V+=T.dwHeight;
		
		DWORD pixel		= T.pSurface[V*T.dwWidth+U];
		DWORD pixel_a	= RGBA_GETALPHA(pixel);
		float opac		= 1.f - float(pixel_a)/255.f;
		scale			*= opac;
	}
	
	return scale;
}

float rayTrace(RAPID::XRCollide* DB, R_Light& L, Fvector& P, Fvector& D, float R)
{
	// 1. Check cached polygon
	float _u,_v,range;
	bool res = RAPID::TestRayTri(P,D,L.tri,_u,_v,range,false);
	if (res) {
		if (range>0 && range<R) return 0;
	}

	// 2. Polygon doesn't pick - real database query
	DB->RayPick(0,&RCAST_Model,P,D,R);
	if (0==DB->GetRayContactCount()) {
		return 1;
	} else {
		// analyze polygons and cache nearest if possible
		return getLastRP_Scale(DB,L);
	}
}

void LightPoint(RAPID::XRCollide* DB, Fcolor &C, Fvector &P, Fvector &N, R_Light* begin, R_Light* end)
{
	Fvector		Ldir,Pnew;
	Pnew.direct(P,N,0.01f);

	R_Light	*L = begin, *E=end;
	for (;L!=E; L++)
	{
		if (L->type==LT_DIRECT) {
			// Cos
			Ldir.invert	(L->direction);
			float D		= Ldir.dotproduct( N );
			if( D <=0 ) continue;

			// Trace Light
			float scale	= D*L->energy*rayTrace(DB,*L,Pnew,Ldir,1000.f);
			C.r += scale * L->diffuse.r; 
			C.g += scale * L->diffuse.g;
			C.b += scale * L->diffuse.b;
		} else {
			// Distance
			float sqD	= P.distance_to_sqr(L->position);
			if (sqD > L->range2) continue;
			
			// Dir
			Ldir.sub	(L->position,P);
			Ldir.normalize_safe();
			float D		= Ldir.dotproduct( N );
			if( D <=0 ) continue;
			
			// Trace Light
			float R		= sqrtf(sqD);
			float scale = D*L->energy*rayTrace(DB,*L,Pnew,Ldir,R);
			float A		= scale / (L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);
			
			C.r += A * L->diffuse.r;
			C.g += A * L->diffuse.g;
			C.b += A * L->diffuse.b;
		}
	}
}

VOID CDeflector::Light()
{
	HASH	hash;

	// Surface
	lm.pSurface = 0;
	lm_rad		= 0;

	if (g_params.m_bRadiosity) {
		DWORD size = lm.dwWidth*lm.dwHeight*sizeof(Fvector);
		lm_rad = (Fvector*)malloc(size);
		ZeroMemory	(lm_rad,size);
	}

	{
		DWORD size = lm.dwWidth*lm.dwHeight*4;
		lm.pSurface = (DWORD *)malloc(size);
		ZeroMemory	(lm.pSurface,size);
	}

	// Filling it with new triangles
	Fbox bb; bb.invalidate	();
	Fbox2 bounds;
	Bounds_Summary			(bounds);
	hash.initialize			(bounds);

	for (DWORD fid=0; fid<tris.size(); fid++)
	{
		UVtri* T		= &(tris[fid]);
		Bounds			(fid,bounds);
		hash.add		(bounds,T);

		Face*	F		= T->owner;
		for (int i=0; i<3; i++)	bb.modify(F->v[i]->P);
	}
	bb.getsphere(Center,Radius);

	// Convert lights to local form
	{
		R_Light*	L = pBuild->lights_soften.begin();
		for (; L!=pBuild->lights_soften.end(); L++)
		{
			if (L->type==LT_POINT) {
				float dist = Center.distance_to(L->position);
				if (dist>(Radius+L->range)) continue;
			}
			LightsSelected.push_back		(*L);
		}
	}
	if (LightsSelected.empty()) return;

	if (g_params.m_bRadiosity)	L_Radiosity	(hash);
	else						L_Direct	(hash);
	LightsSelected.clear	();

	// Expand LMap with borders
	LPDWORD	old = lm.pSurface;
	DWORD	o_x = lm.dwWidth;
	DWORD	o_y = lm.dwHeight;
	DWORD	s_x = (o_x+2*BORDER);
	DWORD	s_y = (o_y+2*BORDER);
	DWORD size	= s_x*s_y*4;
	lm.pSurface	= LPDWORD(malloc(size));
	ZeroMemory	(lm.pSurface,size);
	blit		(lm.pSurface,s_x,s_y,old,o_x,o_y,BORDER,BORDER,1);
	_FREE		(old);

	// Calculate color of border pixels
	lm.dwWidth	= s_x;	lm.dwHeight	= s_y;
	for (DWORD ref=254; ref>0; ref--) if (!ApplyBorders(lm,ref)) break;
	lm.dwWidth	= o_x;	lm.dwHeight	= o_y;
	
	// Try to shrink lightmap in U & V direction to Zero-pixel LM (only border)
	{
		// Calculate average color
		DWORD _r=0, _g=0, _b=0, _count=0;
		{
			for (DWORD y=0; y<s_y; y++)
			{
				for (DWORD x=0; x<s_x; x++)
				{
					DWORD pixel			= lm.pSurface[y*s_x+x];
					if (RGBA_GETALPHA(pixel)>=254)	{
						_r		+= RGBA_GETRED	(pixel);
						_g		+= RGBA_GETGREEN	(pixel);
						_b		+= RGBA_GETBLUE	(pixel);
						_count	++;
					}
				}
			}
		}
		if (0==_count)	{
			Msg("* ERROR: Lightmap not calculated (T:%d)",tris.size());
			return;
		}
		_r	/= _count;	_g	/= _count;	_b	/= _count;
		
		// Test for equality
		DWORD	bCompress	= TRUE;
		const DWORD rms		= 4;
		{
			for (DWORD y=0; y<s_y; y++)
			{
				for (DWORD x=0; x<s_x; x++)
				{
					DWORD pixel	= lm.pSurface	[y*s_x+x];
					if (RGBA_GETALPHA(pixel)>=254)	{
						if (_abs(s32(RGBA_GETRED	(pixel))-s32(_r))>rms)	{ bCompress=FALSE; break; }
						if (_abs(s32(RGBA_GETGREEN	(pixel))-s32(_g))>rms)	{ bCompress=FALSE; break; }
						if (_abs(s32(RGBA_GETBLUE	(pixel))-s32(_b))>rms)	{ bCompress=FALSE; break; }
					}
				}
				if (!bCompress) break;
			}
		}

		// Compress if needed
		if (bCompress)
		{
			// Msg		("Compressing");
			DWORD	c_x			= BORDER*2;
			DWORD	c_y			= BORDER*2;
			DWORD   c_size		= c_x*c_y;
			LPDWORD	compressed	= LPDWORD(malloc(c_size*4));
			ZeroMemory			(compressed,c_size*4);
			DWORD	c_fill		= RGBA_MAKE		(_r,_g,_b,255-BORDER);
			for (DWORD p=0; p<c_size; p++)	compressed[p]=c_fill;

			_FREE				(lm.pSurface);
			lm.pSurface			= compressed;
			lm.dwHeight			= 0;	s_y=c_y;
			lm.dwWidth			= 0;	s_x=c_x;
		} else {
			// *** Try to bilinearly filter lightmap down and up
			// DWORD	filtered	[512*512];
		}
	}
	
	// Build span-buffer
	lm_spans	= (span)malloc(s_y*sizeof(span));
	for (DWORD y=0; y<s_y; y++)
	{
		span	spOne;
		int		x;

		// left
		for (x=0; x<int(s_x); x++)
		{
			DWORD pixel	= lm.pSurface	[y*s_x+x];
			if (RGBA_GETALPHA(pixel)>=(254-BORDER))	{
				spOne.first	= WORD(x);
				break;
			}
		}

		// right
		for (x=int(s_x-1); x>=0; x++)
		{
			DWORD pixel	= lm.pSurface	[y*s_x+x];
			if (RGBA_GETALPHA(pixel)>=(254-BORDER))	{
				spOne.second= WORD(x);
				break;
			}
		}

		lm_spans[y] = spOne;
	}
}

float gauss [7][7] =
{
	{	0,	0,	0,	5,	0,	0,	0	},
	{	0,	5,	18,	32,	18,	5,	0	},
	{	0,	18,	64,	100,64,	18,	0	},
	{	5,	32,	100,100,100,32,	5	},
	{	0,	18,	64,	100,64,	18,	0	},
	{	0,	5,	18,	32,	18,	5,	0	},
	{	0,	0,	0,	5,	0,	0,	0	}
};

/* Un-optimized code to perform general image filtering
outputs to dst using a filter kernel in ker which must be a 2D float
array of size [2*k+1][2*k+1] */
void ip_BuildKernel	(float* dest, float* src, int DIM, float norm=1.f)
{
	float	*I,*E;

	float	sum		= 0;
	int		size	= 2*DIM+1;
	E				= src + (size*size);
	for (I=src; I!=E; I++) sum += *I;
	float	scale	= norm/sum;
	for (I=src; I!=E; I++) *dest++ = *I * scale;
}
void ip_ProcessKernel(Fvector* dest, Fvector* src, int w, int h, float* kern, int DIM)
{
	for (int y=0;y<h;y++)
	{
		for (int x=0;x<w;x++)
		{
			Fvector total; 
			total.set	(0,0,0);
			float *kp	= kern;
			for (int j=-DIM;j<=DIM;j++)
			for (int i=-DIM;i<=DIM;i++)
			{
				int x2=x+i,y2=y+j;

				// wrap pixels
				if (x2<0) x2+=w;
				else if (x2>=w) x2-=w;
				if (y2<0) y2+=h;
				else if (y2>=h) y2-=h;
				
				total.direct(total,src[y2*w+x2],*kp);
				kp++;
			}
			dest->set(total);
			dest++;
		}
	}
}

IC DWORD convert(float a)
{
	if (a<=0)		return 0;
	else if (a>=1)	return 255;
	else			return iFloor(a*255.f);
}
IC void pixel(int x, int y,  b_texture* T, DWORD C=RGBA_MAKE(0,255,0,0))
{
	// wrap pixels
	if (x<0) return;
	else if (x>=(int)T->dwWidth)	return;
	if (y<0) return;
	else if (y>=(int)T->dwHeight)	return;

	T->pSurface[y*T->dwWidth+x]	= C;
}

void line ( int x1, int y1, int x2, int y2, b_texture* T )
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x2 >= x1 ? 1 : -1;
    int sy = y2 >= y1 ? 1 : -1;

    if ( dy <= dx ){
        int d  = ( dy << 1 ) - dx;
        int d1 = dy << 1;
        int d2 = ( dy - dx ) << 1;

		pixel(x1,y1,T);

        for  (int x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx){
            if ( d > 0){
                d += d2; y += sy;
            }else
                d += d1;
			pixel(x,y,T);
        }
    }else{
        int d  = ( dx << 1 ) - dy;
        int d1 = dx << 1;
        int d2 = ( dx - dy ) << 1;

		pixel(x1,y1,T);
        for  (int x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy ){
            if ( d > 0){
                d += d2; x += sx;
            }else
                d += d1;
			pixel(x,y,T);
        }
    }
}

void CDeflector::Save()
{
	static int		deflNameID = 0; ++deflNameID;

	if (g_params.m_bRadiosity) {
		DWORD count	= lm.dwWidth*lm.dwHeight;

		// Blur data
		Fvector	blured[512*512];
		float	kernel[7*7];
		ip_BuildKernel	(kernel, (float*)gauss, 3, 1/3.5f);
		ip_ProcessKernel(blured,lm_rad,lm.dwWidth,lm.dwHeight,kernel,3);
		CopyMemory		(lm_rad,blured,count*sizeof(Fvector));

		// Convert to '32bpp' format - for debugging
		DWORD		surf[512*512];
		for (DWORD I=0; I<count; I++)
		{
			Fvector&	Pixel	= *(lm_rad+I);
			DWORD&		Dest	= *(surf+I);

			Fcolor R;
			R.r		= Pixel.x; clamp(R.r,0.f,1.f);
			R.g		= Pixel.y; clamp(R.g,0.f,1.f);
			R.b		= Pixel.z; clamp(R.b,0.f,1.f);
			R.a		= 1.f;
			Dest	= R.get();
		}

		// Saving
		sprintf			(lm.name,"L#%d_rad",deflNameID);
		TGAdesc			p;
		p.format		= IMG_24B;
		p.scanlenght	= lm.dwWidth*4;
		p.width			= lm.dwWidth;
		p.height		= lm.dwHeight;
		p.data			= surf;
		p.maketga		(lm.name);

		// Adding radiosity to direct lighting model
		for (I=0; I<count; I++)
		{
			Fvector&	TexelRad	= *(lm_rad+I);
			DWORD&		Texel		= *(lm.pSurface+I);

			if (!RGBA_GETALPHA(Texel)) continue;
			
			Fcolor R, Lumel;

			R.r = (float(RGBA_GETRED	(Texel))/255.f+TexelRad.x);
			R.g = (float(RGBA_GETGREEN	(Texel))/255.f+TexelRad.y);
			R.b = (float(RGBA_GETBLUE	(Texel))/255.f+TexelRad.z);
			Lumel.lerp(R,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
			
			// Make 32bpp color
			Texel = RGBA_MAKE(convert(R.r*255.f),convert(R.g*255.f),convert(R.b*255.f),255);
		}
		_FREE(lm_rad);
	}

	// DEBUG: Lines
	/*
	{
		// 5x expand
		b_texture		temp;
		temp.dwHeight	= lm.dwHeight*5;
		temp.dwWidth	= lm.dwWidth *5;
		temp.pSurface	= LPDWORD(malloc(temp.dwHeight*temp.dwWidth*4));
		for (DWORD y=0; y<lm.dwHeight; y++)
		{
			for (DWORD x=0; x<lm.dwWidth; x++)
			{
				DWORD C = lm.pSurface[y*lm.dwWidth+x];
				if (RGBA_GETALPHA(C)!=255)	C=0;
				for (DWORD p_y=0; p_y<5; p_y++)
					for (DWORD p_x=0; p_x<5; p_x++)
						pixel(x*5+p_x,y*5+p_y,&temp,C);
			}
		}

		// Render polygons
		for (DWORD t=0; t<tris.size(); t++)
		{
			UVtri&		T	= tris[t];
			UVpoint&	p1	= T.uv[0]; int x1=iFloor(p1.u*float(temp.dwWidth)); int y1=iFloor(p1.v*float(temp.dwHeight));
			UVpoint&	p2	= T.uv[1]; int x2=iFloor(p2.u*float(temp.dwWidth)); int y2=iFloor(p2.v*float(temp.dwHeight));
			UVpoint&	p3	= T.uv[2]; int x3=iFloor(p3.u*float(temp.dwWidth)); int y3=iFloor(p3.v*float(temp.dwHeight));
			line		(x1,y1,x2,y2,&temp);
			line		(x2,y2,x3,y3,&temp);
			line		(x3,y3,x1,y1,&temp);
		}

		// Save picture
		sprintf			(temp.name,"L#%d_debug",deflNameID);
		TGAdesc			p;
		p.format		= IMG_32B;
		p.scanlenght	= temp.dwWidth*4;
		p.width			= temp.dwWidth;
		p.height		= temp.dwHeight;
		p.data			= temp.pSurface;
		p.maketga		(temp.name);
		_FREE			(temp.pSurface);
	}
	*/
	
	// Borders correction
	for (DWORD _y=0; _y<512; _y++)
	{
		for (DWORD _x=0; _x<512; _x++)
		{
			DWORD pixel = lm.pSurface[_y*512+_x];
			if (RGBA_GETALPHA(pixel)>=(254-BORDER))	pixel = (pixel&RGBA_MAKE(255,255,255,0))|RGBA_MAKE(0,0,0,255);
			else									pixel = (pixel&RGBA_MAKE(255,255,255,0));
		}
	}
	for (DWORD ref=254; ref>0; ref--) ApplyBorders(lm,ref);

	// DEBUG: Saving	(32b.tga)
	char	FN[_MAX_PATH];
	{
		sprintf			(lm.name,"L#%d",deflNameID);
		TGAdesc			p;
		p.format		= IMG_32B;
		p.scanlenght	= lm.dwWidth*4;
		p.width			= lm.dwWidth;
		p.height		= lm.dwHeight;
		p.data			= lm.pSurface;
		p.maketga		(lm.name);
	}

	// Saving			(16b.dds)
	{
		sprintf	(lm.name,"L#%d",deflNameID				);
		sprintf	(FN,"%s%sQ.dds",g_params.L_path,lm.name	);
		BYTE*	raw_data		= LPBYTE(lm.pSurface);
		DWORD	w				= lm.dwWidth;
		DWORD	h				= lm.dwHeight;
		DWORD	pitch			= w*4;

		STextureParams fmt;
		fmt.fmt					= STextureParams::tf565;
		fmt.flag				= EF_DITHER | EF_NO_MIPMAP;
		DXTCompress				(FN,raw_data,w,h,pitch,&fmt,4);
	}

	// Saving			(DXT1.dds)
	sprintf	(lm.name,"L#%d",deflNameID);
	sprintf	(FN,"%s%s.dds",g_params.L_path,lm.name);
	R_ASSERT(xrDXTC_Compress(FN,eDXT1,FALSE,lm.pSurface,lm.dwWidth,lm.dwHeight,0xff));

	_FREE			(lm.pSurface);
}
