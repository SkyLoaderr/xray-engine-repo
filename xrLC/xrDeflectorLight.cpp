#include "stdafx.h"
#include "build.h"
#include "cl_intersect.h"
#include "std_classes.h"
#include "xrImage_Resampler.h"

#define rms_zero	((4+g_params.m_lm_rms_zero)/2)
#define rms_shrink	((8+g_params.m_lm_rms)/2)
const	u32	rms_discard		= 8;

void Jitter_Select(Fvector2* &Jitter, u32& Jcount)
{
	static Fvector2 Jitter1[1] = {
		{0,0}
	};
	static Fvector2 Jitter4[4] = {
		{-1,-1}, {1,-1}, {1,1}, {-1,1}
	};
	static Fvector2 Jitter9[9] = {
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
			int x, int y, u32 ref, u32 &count, 
			u32 &r, u32 &g, u32 &b)
{
	// wrap pixels
	if (x<0) return;
	else if (x>=(int)lm.dwWidth)	return;
	if (y<0) return;
	else if (y>=(int)lm.dwHeight)	return;
	
	// summarize
	u32 pixel = lm.pSurface[y*lm.dwWidth + x];
	if (color_get_A(pixel)<=ref) return;
	
	r	+=	color_get_R	(pixel);
	g	+=	color_get_G	(pixel);
	b	+=	color_get_B	(pixel);
	count++;
}

BOOL ApplyBorders(b_texture &lm, u32 ref) 
{
	BOOL	bNeedContinue = FALSE;
	
	try {
		u32	result[lmap_size*lmap_size];
		
		R_ASSERT(lm.dwHeight<=lmap_size	);
		R_ASSERT(lm.dwWidth<=lmap_size	);
		
		CopyMemory(result,lm.pSurface,lm.dwHeight*lm.dwWidth*4);
		for (int y=0; y<(int)lm.dwHeight; y++) {
			for (int x=0; x<(int)lm.dwWidth; x++)
			{
				if (color_get_A(lm.pSurface[y*lm.dwWidth+x])==0) {
					
					u32 C=0,r=0,g=0,b=0;
					GET(lm,x-1,y-1,ref,C,r,g,b);
					GET(lm,x  ,y-1,ref,C,r,g,b);
					GET(lm,x+1,y-1,ref,C,r,g,b);
					
					GET(lm,x-1,y  ,ref,C,r,g,b);
					GET(lm,x+1,y  ,ref,C,r,g,b);
					
					GET(lm,x-1,y+1,ref,C,r,g,b);
					GET(lm,x  ,y+1,ref,C,r,g,b);
					GET(lm,x+1,y+1,ref,C,r,g,b);
					
					if (C) {
						result[y*lm.dwWidth+x]=color_rgba(r/C,g/C,b/C,ref);
						bNeedContinue = TRUE;
					}
				}
			}
		}
		CopyMemory(lm.pSurface,result,lm.dwHeight*lm.dwWidth*4);
	} catch (...)
	{
		clMsg("* ERROR: ApplyBorders");
	}
	return bNeedContinue;
}

float getLastRP_Scale(CDB::COLLIDER* DB, CDB::MODEL* MDL, R_Light& L, Face* skip, BOOL bUseFaceDisable)
{
	u32		tris_count	= DB->r_count();
	float	scale		= 1.f;
	Fvector B;

	X_TRY 
	{
		for (u32 I=0; I<tris_count; I++)
		{
			CDB::RESULT& rpinf = DB->r_begin()[I];
			
			// Access to texture
			CDB::TRI& clT									= MDL->get_tris()[rpinf.id];
			base_Face* F									= (base_Face*) clT.dummy;
			if (0==F)										continue;
			if (skip==F)									continue;
			if (bUseFaceDisable && F->bDisableShadowCast)	continue;
			
			Shader_xrLC&	SH								= F->Shader();
			if (!SH.flags.bLIGHT_CastShadow)				continue;
			
			if (F->bOpaque){
				// Opaque poly - cache it
				L.tri[0].set	(*clT.verts[0]);
				L.tri[1].set	(*clT.verts[1]);
				L.tri[2].set	(*clT.verts[2]);
				return 0;
			}
			
			b_material& M	= pBuild->materials			[F->dwMaterial];
			b_texture&	T	= pBuild->textures			[M.surfidx];
			
			// barycentric coords
			// note: W,U,V order
			B.set	(1.0f - rpinf.u - rpinf.v,rpinf.u,rpinf.v);
			
			// calc UV
			Fvector2*	cuv = F->getTC0					();
			Fvector2	uv;
			uv.x = cuv[0].x*B.x + cuv[1].x*B.y + cuv[2].x*B.z;
			uv.y = cuv[0].y*B.x + cuv[1].y*B.y + cuv[2].y*B.z;
			
			int U = iFloor(uv.x*float(T.dwWidth) + .5f);
			int V = iFloor(uv.y*float(T.dwHeight)+ .5f);
			U %= T.dwWidth;		if (U<0) U+=T.dwWidth;
			V %= T.dwHeight;	if (V<0) V+=T.dwHeight;
			
			u32 pixel		= T.pSurface[V*T.dwWidth+U];
			u32 pixel_a		= color_get_A(pixel);
			float opac		= 1.f - float(pixel_a)/255.f;
			scale			*= opac;
		}
	} 
	X_CATCH
	{
		clMsg("* ERROR: getLastRP_Scale");
	}
	
	return scale;
}

float rayTrace	(CDB::COLLIDER* DB, CDB::MODEL* MDL, R_Light& L, Fvector& P, Fvector& D, float R, Face* skip, BOOL bUseFaceDisable)
{
	R_ASSERT	(DB);
	
	// 1. Check cached polygon
	float _u,_v,range;
	bool res = CDB::TestRayTri(P,D,L.tri,_u,_v,range,false);
	if (res) {
		if (range>0 && range<R) return 0;
	}
	
	// 2. Polygon doesn't pick - real database query
	DB->ray_query	(MDL,P,D,R);
	
	// 3. Analyze polygons and cache nearest if possible
	if (0==DB->r_count()) {
		return 1;
	} else {
		return getLastRP_Scale(DB,MDL, L,skip,bUseFaceDisable);
	}
	return 0;
}

void LightPoint(CDB::COLLIDER* DB, CDB::MODEL* MDL, base_color &C, Fvector &P, Fvector &N, base_lighting& lights, u32 flags, Face* skip)
{
	Fvector		Ldir,Pnew;
	Pnew.mad	(P,N,0.01f);

	BOOL		bUseFaceDisable	= flags&LP_UseFaceDisable;

	if (0==(flags&LP_dont_rgb))
	{
		R_Light	*L	= &*lights.rgb.begin(), *E = &*lights.rgb.end();
		for (;L!=E; L++)
		{
			if (L->type==LT_DIRECT) {
				// Cos
				Ldir.invert	(L->direction);
				float D		= Ldir.dotproduct( N );
				if( D <=0 ) continue;

				// Trace Light
				float scale	= D*L->energy*rayTrace(DB,MDL, *L,Pnew,Ldir,1000.f,skip,bUseFaceDisable);
				C.rgb.x += scale * L->diffuse.r; 
				C.rgb.y += scale * L->diffuse.g;
				C.rgb.z += scale * L->diffuse.b;
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
				float R		= _sqrt(sqD);
				float scale = D*L->energy*rayTrace(DB,MDL, *L,Pnew,Ldir,R,skip,bUseFaceDisable);
				float A		= scale / (L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);

				C.rgb.x += A * L->diffuse.r;
				C.rgb.y += A * L->diffuse.g;
				C.rgb.z += A * L->diffuse.b;
			}
		}
	}
	if (0==(flags&LP_dont_sun))
	{
		R_Light	*L	= &*(lights.sun.begin()), *E = &*(lights.sun.end());
		for (;L!=E; L++)
		{
			if (L->type==LT_DIRECT) {
				// Cos
				Ldir.invert	(L->direction);
				float D		= Ldir.dotproduct( N );
				if( D <=0 ) continue;

				// Trace Light
				float scale	=	D*L->energy*rayTrace(DB,MDL, *L,Pnew,Ldir,1000.f,skip,bUseFaceDisable);
				C.sun		+=	scale;
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
				float R		=	_sqrt(sqD);
				float scale =	D*L->energy*rayTrace(DB,MDL, *L,Pnew,Ldir,R,skip,bUseFaceDisable);
				float A		=	scale / (L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);

				C.sun		+=	A;
			}
		}
	}
	if (0==(flags&LP_dont_hemi))
	{
		R_Light	*L	= &*lights.hemi.begin(), *E = &*lights.hemi.end();
		for (;L!=E; L++)
		{
			if (L->type==LT_DIRECT) {
				// Cos
				Ldir.invert	(L->direction);
				float D		= Ldir.dotproduct( N );
				if( D <=0 ) continue;

				// Trace Light
				float scale	=	D*L->energy*rayTrace(DB,MDL, *L,Pnew,Ldir,1000.f,skip,bUseFaceDisable);
				C.hemi		+=	scale;
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
				float R		=	_sqrt(sqD);
				float scale =	D*L->energy*rayTrace(DB,MDL, *L,Pnew,Ldir,R,skip,bUseFaceDisable);
				float A		=	scale / (L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);

				C.hemi		+=	A;
			}
		}
	}
}

IC u32	rms_diff	(u32 a, u32 b)
{
	if (a>b)	return a-b;
	else		return b-a;
}
BOOL	__stdcall rms_test	(b_texture& lm, u32 w, u32 h, u32 rms)
{
	if ((w<=1) || (h<=1))	return FALSE;

	// scale down(lanczos3) and up (bilinear, as video board)
	u32*	pScaled		= (u32*)(xr_malloc(w*h*4));
	u32*	pRestored	= (u32*)(xr_malloc(lm.dwWidth*lm.dwHeight*4));
	try {
		imf_Process	(pScaled,	w,h,(u32*)lm.pSurface,lm.dwWidth,lm.dwHeight,imf_lanczos3	);
		imf_Process	(pRestored,	lm.dwWidth,lm.dwHeight,pScaled,w,h,imf_filter				);
	} catch (...)
	{
		clMsg	("* ERROR: imf_Process");
		xr_free	(pScaled);
		xr_free	(pRestored);
		return	FALSE;
	}
	xr_free		(pScaled);

	// compare them
	const u32 limit = 254-BORDER;
	for (u32 y=0; y<lm.dwHeight; y++)
	{
		u32*	scan_lmap	= lm.pSurface+y*lm.dwWidth;
		u32*	scan_rest	= (u32*)(pRestored)+y*lm.dwWidth;
		for (u32 x=0; x<lm.dwWidth; x++)
		{
			u32 pixel	= scan_lmap[x];
			if (color_get_A(pixel)>=limit)	
			{
				u32 pixel_r	= scan_rest[x];
				if (rms_diff(color_get_R(pixel_r),color_get_R(pixel))>rms)		goto fail;
				if (rms_diff(color_get_G(pixel_r),color_get_G(pixel))>rms)	goto fail;
				if (rms_diff(color_get_B(pixel_r),color_get_B(pixel))>rms)	goto fail;
			}
		}
	}
	xr_free	(pRestored);
	return	TRUE;

fail:
	xr_free	(pRestored);
	return	FALSE;
}

BOOL	__stdcall rms_test	(b_texture&	lm, u32 _r, u32 _g, u32 _b, u32 rms)
{
	u32 x,y;
	for (y=0; y<lm.dwHeight; y++)
	{
		for (x=0; x<lm.dwWidth; x++)
		{
			u32 pixel	= lm.pSurface	[y*lm.dwWidth+x];
			if (color_get_A(pixel)>=254)	{
				if (rms_diff(_r, color_get_R(pixel))>rms)	return FALSE;
				if (rms_diff(_g, color_get_G(pixel))>rms)	return FALSE;
				if (rms_diff(_b, color_get_B(pixel))>rms)	return FALSE;
			}
		}
	}
	return TRUE;
}

u32	__stdcall rms_average	(b_texture& lm, u32& _r, u32& _g, u32& _b)
{
	u32 x,y,_count;
	_r=0, _g=0, _b=0, _count=0;
	
	for (y=0; y<lm.dwHeight; y++)
	{
		for (x=0; x<lm.dwWidth; x++)
		{
			u32 pixel	= lm.pSurface[y*lm.dwWidth+x];
			if ((color_get_A(pixel))>=254)	
			{
				_r		+= color_get_R	(pixel);
				_g		+= color_get_G	(pixel);
				_b		+= color_get_B	(pixel);
				_count	++;
			}
		}
	}
	return	_count;
}

BOOL	compress_Zero			(b_texture& lm, u32 rms)
{
	u32 _r, _g, _b, _count;
	
	// Average color
	_count	= rms_average(lm,_r,_g,_b);
	
	if (0==_count)	{
		clMsg("* ERROR: Lightmap not calculated (T:%d)");
		return FALSE;
	} else {
		_r	/= _count;	_g	/= _count;	_b	/= _count;
	}
	
	// Compress if needed
	if (rms_test(lm,_r,_g,_b,rms))
	{
		xr_free	(lm.pSurface);		// release OLD
		u32		c_x			= BORDER*2;
		u32		c_y			= BORDER*2;
		u32		c_size		= c_x*c_y;
		u32*	compressed	= (u32*)(xr_malloc(c_size*4));
		u32	c_fill			= color_rgba	(_r,_g,_b,255);
		for (u32 p=0; p<c_size; p++)	compressed[p]=c_fill;
		
		lm.pSurface			= compressed;
		lm.dwHeight			= 0;
		lm.dwWidth			= 0;
		return TRUE;
	}
	return FALSE;
}
BOOL	compress_RMS			(b_texture& lm, u32 rms, u32& w, u32& h)
{
	// *** Try to bilinearly filter lightmap down and up
	w=0, h=0;
	if (lm.dwWidth>=2)	{
		w = lm.dwWidth/2;
		if (!rms_test(lm,w,lm.dwHeight,rms))	{
			// 3/4
			w = (lm.dwWidth*3)/4;
			if (!rms_test(lm,w,lm.dwHeight,rms))	w = 0;
		} else {
			// 1/4
			u32 nw = (lm.dwWidth*1)/4;
			if (rms_test(lm,nw,lm.dwHeight,rms))	w = nw;
		}
	}
	if (lm.dwHeight>=2)	{
		h = lm.dwHeight/2;
		if (!rms_test(lm,lm.dwWidth,h,rms))		{
			// 3/4
			h = (lm.dwHeight*3)/4;
			if (!rms_test(lm,lm.dwWidth,h,rms))		h = 0;
		} else {
			// 1/4
			u32 nh = (lm.dwHeight*1)/4;
			if (rms_test(lm,lm.dwWidth,nh,rms))		h = nh;
		}
	}
	if (w || h)	{
		if (0==w)	w = lm.dwWidth;
		if (0==h)	h = lm.dwHeight;
//		clMsg	("* RMS: [%d,%d] => [%d,%d]",lm.dwWidth,lm.dwHeight,w,h);
		return TRUE;
	}
	return FALSE;
}

VOID CDeflector::L_Calculate(CDB::COLLIDER* DB, base_lighting* LightsSelected, HASH& H)
{
	try {
		b_texture&		lm = layers.back().lm;
		
		// UV & HASH
		RemapUV			(0,0,lm.dwWidth,lm.dwHeight,lm.dwWidth,lm.dwHeight,FALSE);
		Fbox2			bounds;
		Bounds_Summary	(bounds);
		H.initialize	(bounds,(u32)UVpolys.size());
		for (u32 fid=0; fid<UVpolys.size(); fid++)
		{
			UVtri* T	= &(UVpolys[fid]);
			Bounds		(fid,bounds);
			H.add		(bounds,T);
		}
		
		// Calculate
		{
			R_ASSERT	(lm.dwWidth<=(lmap_size-2*BORDER));
			R_ASSERT	(lm.dwHeight<=(lmap_size-2*BORDER));
			u32 size = lm.dwWidth*lm.dwHeight*4;
			lm.pSurface = (u32 *)xr_malloc(size);
			ZeroMemory	(lm.pSurface,size);
		}
		L_Direct		(DB,LightsSelected,H);
	} catch (...)
	{
		clMsg("* ERROR: CDeflector::L_Calculate");
	}
}

VOID CDeflector::Light(CDB::COLLIDER* DB, base_lighting* LightsSelected, HASH& H)
{
	// Geometrical bounds
	Fbox bb;		bb.invalidate	();
	try {
		for (u32 fid=0; fid<UVpolys.size(); fid++)
		{
			Face*	F		= UVpolys[fid].owner;
			for (int i=0; i<3; i++)	bb.modify(F->v[i]->P);
		}
		bb.getsphere(Sphere.P,Sphere.R);
	} catch (...)
	{
		clMsg("* ERROR: CDeflector::Light - sphere calc");
	}

	// Iterate on layers
	for (R_Layer* layer=&*pBuild->L_layers.begin(); layer!=&*pBuild->L_layers.end(); layer++)
	{
		// Convert lights to local form
		try {
			LightsSelected->clear	();
			R_Light*	L			= &*layer->lights.begin();
			for (; L!=&*layer->lights.end(); L++)
			{
				if (L->type==LT_POINT) {
					float dist						= Sphere.P.distance_to(L->position);
					if (dist>(Sphere.R+L->range))	continue;
				}
				LightsSelected->push_back(*L);
			}
			if ((layer!=&*pBuild->L_layers.begin()) && LightsSelected->empty())	continue;	// empty, non-base layer
		} catch (...)
		{
			clMsg("* ERROR: CDeflector::Light - LocalSelect (L:%d)",layer-&*pBuild->L_layers.begin());
		}

		// Register _new layer
		layers.push_back	(Layer());
		Layer&	layer_data	= layers.back();
		layer_data.base_id	= (u32)(layer - &*pBuild->L_layers.begin());
		b_texture& lm		= layer_data.lm;
		lm.dwWidth			= dwWidth;
		lm.dwHeight			= dwHeight;

		// Calculate and fill borders
		L_Calculate			(DB,LightsSelected,H);
		for (u32 ref=254; ref>0; ref--) if (!ApplyBorders(lm,ref)) break;

		// Compression
		try {
			u32	w,h;
			if (compress_Zero(lm,rms_zero))	return;		// already with borders
			else if (compress_RMS(lm,rms_shrink,w,h))	
			{
				// Reacalculate lightmap at lower resolution
				lm.dwWidth	= w;
				lm.dwHeight	= h;
				xr_free		(lm.pSurface);
				L_Calculate	(DB,LightsSelected,H);
			}
		} catch (...)
		{
			clMsg("* ERROR: CDeflector::Light - Compression (L:%d)", layer-&*pBuild->L_layers.begin());
		}

		// Expand with borders
		try {
			if (lm.dwWidth==1)	
			{
				// Horizontal ZERO - vertical line
				b_texture		T;
				T.dwWidth		= 2*BORDER;
				T.dwHeight		= lm.dwHeight+2*BORDER;
				u32 size		= T.dwWidth*T.dwHeight*4;
				T.pSurface		= (u32*)(xr_malloc(size));
				ZeroMemory		(T.pSurface,size);

				// Transfer
				for (u32 y=0; y<T.dwHeight; y++)
				{
					int		py			= int(y)-BORDER;
					clamp	(py,0,int(lm.dwHeight-1));
					u32	C			= lm.pSurface[py];
					T.pSurface[y*2+0]	= C; 
					T.pSurface[y*2+1]	= C;
				}

				// Exchange
				xr_free			(lm.pSurface);
				T.dwWidth		= 0;
				T.dwHeight		= lm.dwHeight;
				lm				= T;
			} else if (lm.dwHeight==1) 
			{
				// Vertical ZERO - horizontal line
				b_texture		T;
				T.dwWidth		= lm.dwWidth+2*BORDER;
				T.dwHeight		= 2*BORDER;
				u32 size		= T.dwWidth*T.dwHeight*4;
				T.pSurface		= (u32*)(xr_malloc(size));
				ZeroMemory		(T.pSurface,size);

				// Transfer
				for (u32 x=0; x<T.dwWidth; x++)
				{
					int		px			= int(x)-BORDER;
					clamp	(px,0,int(lm.dwWidth-1));
					u32	C			= lm.pSurface[px];
					T.pSurface[0*T.dwWidth+x]	= C;
					T.pSurface[1*T.dwWidth+x]	= C;
				}

				// Exchange
				xr_free			(lm.pSurface);
				T.dwWidth		= lm.dwWidth;
				T.dwHeight		= 0;
				lm				= T;
			} else {
				// Generic blit
				b_texture		lm_old	= lm;
				b_texture		lm_new;
				lm_new.dwWidth	= (lm_old.dwWidth+2*BORDER);
				lm_new.dwHeight	= (lm_old.dwHeight+2*BORDER);
				u32 size		= lm_new.dwWidth*lm_new.dwHeight*4;
				lm_new.pSurface	= (u32*)(xr_malloc(size));
				ZeroMemory		(lm_new.pSurface,size);
				blit			(lm_new.pSurface,lm_new.dwWidth,lm_new.dwHeight,lm_old.pSurface,lm_old.dwWidth,lm_old.dwHeight,BORDER,BORDER,255-BORDER);
				xr_free			(lm_old.pSurface);
				lm				= lm_new;
				ApplyBorders	(lm,254);
				ApplyBorders	(lm,253);
				ApplyBorders	(lm,252);
				ApplyBorders	(lm,251);
				for	(ref=250; ref>0; ref--) if (!ApplyBorders(lm,ref)) break;
				lm.dwWidth		= lm_old.dwWidth;
				lm.dwHeight		= lm_old.dwHeight;
			}
		} catch (...)
		{
			clMsg("* ERROR: CDeflector::Light - BorderExpansion (L:%d)", layer-&*pBuild->L_layers.begin());
		}

		// Test if layer really needed 
		{
			if (layer==&*pBuild->L_layers.begin())			continue;	// base, ambient layer - always present
			BOOL			bSkip	= TRUE;
			u32			size	= (lm.dwWidth+2*BORDER)*(lm.dwHeight+2*BORDER);
			for (u32 pix=0; pix<size; pix++)	{
				u32 pixel	= lm.pSurface	[pix];
				if (color_get_R(pixel)>rms_discard)			{ bSkip=FALSE; break; }
				if (color_get_G(pixel)>rms_discard)		{ bSkip=FALSE; break; }
				if (color_get_B(pixel)>rms_discard)		{ bSkip=FALSE; break; }
			}
			if (bSkip)		{
				xr_free			(lm.pSurface);
				layers.pop_back	();
			}
		}
	}
}
