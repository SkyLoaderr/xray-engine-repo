#include "stdafx.h"
#include "build.h"
#include "math.h"
#include "cl_defs.h"
#include "cl_intersect.h"
#include "std_classes.h"
#include "xrImage_Resampler.h"

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

IC DWORD	rms_diff	(DWORD a, DWORD b)
{
	if (a>b)	return a-b;
	else		return b-a;
}
IC BOOL		rms_test	(b_texture& lm, DWORD w, DWORD h, DWORD rms)
{
	if ((0==w) || (0==h))	return FALSE;

	// scale down(lanczos3) and up (bilinear, as video board)
	LPDWORD	pScaled		= LPDWORD(malloc(w*h*4));
	LPDWORD	pRestored	= LPDWORD(malloc(lm.dwWidth*lm.dwHeight*4));
	imf_Process	(pScaled,	w,h,lm.pSurface,lm.dwWidth,lm.dwHeight,imf_lanczos3	);
	imf_Process	(pRestored,	lm.dwWidth,lm.dwHeight,pScaled,w,h,imf_filter		);
	_FREE		(pScaled);

	// compare them
	const DWORD limit = 255-BORDER;
	for (DWORD y=0; y<lm.dwHeight; y++)
	{
		LPDWORD	scan_lmap	= lm.pSurface+y*lm.dwWidth;
		LPDWORD	scan_rest	= pRestored+y*lm.dwWidth;
		for (DWORD x=0; x<lm.dwWidth; x++)
		{
			DWORD pixel	= scan_lmap[x];
			if (RGBA_GETALPHA(pixel)>=limit)	
			{
				DWORD pixel_r	= scan_rest[x];
				if (rms_diff(RGBA_GETRED(pixel_r),RGBA_GETRED(pixel))>rms)		goto fail;
				if (rms_diff(RGBA_GETGREEN(pixel_r),RGBA_GETGREEN(pixel))>rms)	goto fail;
				if (rms_diff(RGBA_GETBLUE(pixel_r),RGBA_GETBLUE(pixel))>rms)	goto fail;
			}
		}
	}
	_FREE	(pRestored);
	return	TRUE;

fail:
	_FREE	(pRestored);
	return	FALSE;
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
	DWORD			ref;
	b_texture		lm_old	= lm;
	b_texture		lm_new;
	lm_new.dwWidth	= (lm_old.dwWidth+2*BORDER);
	lm_new.dwHeight	= (lm_old.dwHeight+2*BORDER);
	DWORD size		= lm_new.dwWidth*lm_new.dwHeight*4;
	lm.pSurface		= LPDWORD(malloc(size));
	ZeroMemory		(lm_new.pSurface,size);
	blit			(lm_new.pSurface,lm_new.dwWidth,lm_new.dwHeight,lm_old.pSurface,lm_old.dwWidth,lm_old.dwHeight,BORDER,BORDER,0);
	for (ref=254; ref>0; ref--) if (!ApplyBorders(lm_new,ref))	break;	// new
	for (ref=254; ref>0; ref--) if (!ApplyBorders(lm,ref))		break;	// old

	// Try to shrink lightmap in U & V direction to Zero-pixel LM (only border)
	{
		const DWORD rms		= 4;
		DWORD _r=0, _g=0, _b=0, _count=0, x,y, bCompress=TRUE;

		// Calculate average color
		for (y=0; y<lm_new.dwHeight; y++)
		{
			for (x=0; x<lm_new.dwWidth; x++)
			{
				DWORD pixel			= lm_new.pSurface[y*lm_new.dwWidth+x];
				if ((RGBA_GETALPHA(pixel))>=254)	{
					_r		+= RGBA_GETRED	(pixel);
					_g		+= RGBA_GETGREEN(pixel);
					_b		+= RGBA_GETBLUE	(pixel);
					_count	++;
				}
			}
		}
		if (0==_count)	{
			Msg("* ERROR: Lightmap not calculated (T:%d)",tris.size());
			return;
		} else {
			_r	/= _count;	_g	/= _count;	_b	/= _count;
			Msg("* avarage: %d,%d,%d",_r,_g,_b);
		}
		
		// Test for equality
		for (y=0; y<lm_new.dwHeight; y++)
		{
			for (x=0; x<lm_new.dwWidth; x++)
			{
				DWORD pixel	= lm_new.pSurface	[y*lm_new.dwWidth+x];
				if (RGBA_GETALPHA(pixel)>=254)	{
					if (rms_diff(_r, RGBA_GETRED(pixel))>rms)	{ bCompress=FALSE; break; }
					if (rms_diff(_g, RGBA_GETGREEN(pixel))>rms)	{ bCompress=FALSE; break; }
					if (rms_diff(_b, RGBA_GETBLUE(pixel))>rms)	{ bCompress=FALSE; break; }
				}
			}
			if (!bCompress) break;
		}
		
		// Compress if needed
		if (bCompress)
		{
			Msg		("Compressing");
			_FREE	(lm.pSurface);		// release OLD
			_FREE	(lm_new.pSurface);	// ... and new
			DWORD	c_x			= BORDER*2;
			DWORD	c_y			= BORDER*2;
			DWORD   c_size		= c_x*c_y;
			LPDWORD	compressed	= LPDWORD(malloc(c_size*4));
			DWORD	c_fill		= RGBA_MAKE	(_r,_g,_b,255-BORDER);
			for (DWORD p=0; p<c_size; p++)	compressed[p]=c_fill;

			lm.pSurface			= compressed;
			lm.dwHeight			= 0;
			lm.dwWidth			= 0;
		} else {
			// *** Try to bilinearly filter lightmap down and up
			// b_texture			
		}
	}
}

