#include "stdafx.h"
#include "build.h"
#include "cl_intersect.h"
#include "std_classes.h"
#include "xrImage_Resampler.h"

#define rms_zero	((4+g_params.m_lm_rms_zero)/2)	//. float
#define rms_shrink	((8+g_params.m_lm_rms)/2)		//. float
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

void GET			(lm_layer &lm, int x, int y, u32 ref, u32 &count,  base_color_c& dst)
{
	// wrap pixels
	if (x<0) return;
	else if (x>=(int)lm.width)	return;
	if (y<0) return;
	else if (y>=(int)lm.height)	return;

	// summarize
	u32		id	= y*lm.width + x;
	if (lm.marker[id]<=ref)		return;

	base_color_c		C;
	lm.surface[id]._get	(C);
	dst.add				(C);
	count				++;
}

BOOL ApplyBorders	(lm_layer &lm, u32 ref) 
{
	BOOL			bNeedContinue = FALSE;

	try {
		lm_layer	result	= lm;

		for (int y=0; y<(int)lm.height; y++) {
			for (int x=0; x<(int)lm.width; x++)
			{
				if (lm.marker[y*lm.width+x]==0) 
				{
					base_color_c	clr;
					u32			C	=0;
					GET(lm,x-1,y-1,ref,C,clr);
					GET(lm,x  ,y-1,ref,C,clr);
					GET(lm,x+1,y-1,ref,C,clr);

					GET(lm,x-1,y  ,ref,C,clr);
					GET(lm,x+1,y  ,ref,C,clr);

					GET(lm,x-1,y+1,ref,C,clr);
					GET(lm,x  ,y+1,ref,C,clr);
					GET(lm,x+1,y+1,ref,C,clr);

					if (C) {
						clr.scale			(C);
						result.surface		[y*lm.width+x]._set(clr);
						result.marker		[y*lm.width+x]=u8(ref);
						bNeedContinue		= TRUE;
					}
				}
			}
		}
		lm	= result;
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
			CDB::TRI& clT										= MDL->get_tris()[rpinf.id];
			base_Face* F										= (base_Face*) clT.dummy;
			if (0==F)											continue;
			if (skip==F)										continue;
			if (bUseFaceDisable && F->flags.bDisableShadowCast)	continue;

			Shader_xrLC&	SH									= F->Shader();
			if (!SH.flags.bLIGHT_CastShadow)					continue;

			if (F->flags.bOpaque)	{
				// Opaque poly - cache it
				L.tri[0].set	(rpinf.verts[0]);
				L.tri[1].set	(rpinf.verts[1]);
				L.tri[2].set	(rpinf.verts[2]);
				return 0;
			}

			b_material& M	= pBuild->materials			[F->dwMaterial];
			b_texture&	T	= pBuild->textures			[M.surfidx];

			if (0==T.pSurface)	{
				F->flags.bOpaque	= true;
				clMsg			("* ERROR: RAY-TRACE: Strange face detected... Has alpha without texture...");
				return 0;
			}

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
			float opac		= 1.f - _sqr(float(pixel_a)/255.f);
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

void LightPoint(CDB::COLLIDER* DB, CDB::MODEL* MDL, base_color_c &C, Fvector &P, Fvector &N, base_lighting& lights, u32 flags, Face* skip)
{
	Fvector		Ldir,Pnew;
	Pnew.mad	(P,N,0.01f);

	BOOL		bUseFaceDisable	= flags&LP_UseFaceDisable;

	if (0==(flags&LP_dont_rgb))
	{
		R_Light	*L	= &*lights.rgb.begin(), *E = &*lights.rgb.end();
		for (;L!=E; L++)
		{
			switch (L->type)
			{
			case LT_DIRECT:
				{
					// Cos
					Ldir.invert	(L->direction);
					float D		= Ldir.dotproduct( N );
					if( D <=0 ) continue;

					// Trace Light
					float scale	=	D*L->energy*rayTrace(DB,MDL, *L,Pnew,Ldir,1000.f,skip,bUseFaceDisable);
					C.rgb.x		+=	scale * L->diffuse.x; 
					C.rgb.y		+=	scale * L->diffuse.y;
					C.rgb.z		+=	scale * L->diffuse.z;
				}
				break;
			case LT_POINT:
				{
					// Distance
					float sqD	=	P.distance_to_sqr	(L->position);
					if (sqD > L->range2) continue;

					// Dir
					Ldir.sub			(L->position,P);
					Ldir.normalize_safe	();
					float D				= Ldir.dotproduct( N );
					if( D <=0 )			continue;

					// Trace Light
					float R		= _sqrt(sqD);
					float scale = D*L->energy*rayTrace(DB,MDL, *L,Pnew,Ldir,R,skip,bUseFaceDisable);
					float A		= scale / (L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);

					C.rgb.x += A * L->diffuse.x;
					C.rgb.y += A * L->diffuse.y;
					C.rgb.z += A * L->diffuse.z;
				}
				break;
			case LT_SECONDARY:
				{
					// Distance
					float sqD	=	P.distance_to_sqr	(L->position);
					if (sqD > L->range2) continue;

					// Dir
					Ldir.sub	(L->position,P);
					Ldir.normalize_safe();
					float	D	=	Ldir.dotproduct		( N );
					if( D <=0 ) continue;
							D	*=	-Ldir.dotproduct	(L->direction);
					if( D <=0 ) continue;

					// Jitter + trace light -> monte-carlo method
					Fvector	Psave	= L->position, Pdir;
					L->position.mad	(Pdir.random_dir(L->direction,PI_DIV_4),.05f);
					float R			= _sqrt(sqD);
					float scale		= powf(D, 1.f/8.f)*L->energy*rayTrace(DB,MDL, *L,Pnew,Ldir,R,skip,bUseFaceDisable);
					float A			= scale * (1-R/L->range);
					L->position		= Psave;

					C.rgb.x += A * L->diffuse.x;
					C.rgb.y += A * L->diffuse.y;
					C.rgb.z += A * L->diffuse.z;
				}
				break;
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
				float scale	=	L->energy*rayTrace(DB,MDL, *L,Pnew,Ldir,1000.f,skip,bUseFaceDisable);
				C.sun		+=	scale;
			} else {
				// Distance
				float sqD	=	P.distance_to_sqr(L->position);
				if (sqD > L->range2) continue;

				// Dir
				Ldir.sub			(L->position,P);
				Ldir.normalize_safe	();
				float D				= Ldir.dotproduct( N );
				if( D <=0 )			continue;

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
				if( D <-0.5f )	continue;	// allow some wrap

				// Trace Light
				Fvector		PMoved;	PMoved.mad	(Pnew,Ldir,0.01f);
				float scale	=	L->energy*rayTrace(DB,MDL, *L,PMoved,Ldir,1000.f,skip,bUseFaceDisable);
				C.hemi		+=	scale;
			} else {
				// Distance
				float sqD	=	P.distance_to_sqr(L->position);
				if (sqD > L->range2) continue;

				// Dir
				Ldir.sub			(L->position,P);
				Ldir.normalize_safe	();
				float D		=	Ldir.dotproduct( N );
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

BOOL	__stdcall rms_test	(lm_layer& lm, u32 w, u32 h, u32 rms)
{
	if ((w<=1) || (h<=1))	return FALSE;

	// scale down(lanczos3) and up (bilinear, as video board)
	//.?????????
	xr_vector<u32>			pOriginal;		lm.Pack			(pOriginal);
	xr_vector<u32>			pOriginal_hemi;	lm.Pack_hemi	(pOriginal);

	xr_vector<base_color>	pScaled;	pScaled.resize	(w*h);
	xr_vector<base_color>	pScaled_hemi;	pScaled.resize	(w*h);

	xr_vector<base_color>	pRestored;	pRestored.resize(lm.width*lm.height);
	xr_vector<base_color>	pRestored_hemi;	pRestored.resize(lm.width*lm.height);

	try {
		// 5-channel -> r,g,b,sun,hemi
		imf_Process	(&*pScaled.begin(),		w,			h,			&*pOriginal.begin(),	lm.width,lm.height,imf_lanczos3	);
		imf_Process	(&*pRestored.begin(),	lm.width,	lm.height,	&*pScaled.begin(),		w,h,imf_filter					);

		// hemi ??????????
		imf_Process	(&*pScaled_hemi.begin(),		w,			h,			&*pOriginal.begin(),	lm.width,lm.height,imf_lanczos3	);
		imf_Process	(&*pRestored.begin(),	lm.width,	lm.height,	&*pScaled.begin(),		w,h,imf_filter					);
	} catch (...)
	{
		clMsg	("* ERROR: imf_Process");
		return	FALSE;
	}

	// compare them
	const u32 limit = 254-BORDER;
	for (u32 y=0; y<lm.height; y++)
	{
		u32		offset		= y*lm.width;
		u8*		scan_mark	= (u8*)	&*(lm.marker.begin()+offset);	//.
		u32*	scan_lmap	= (u32*)&*(pOriginal.begin()+offset);	//.
		u32*	scan_rest	= (u32*)&*(pRestored.begin()+offset);	//.
		for (u32 x=0; x<lm.width; x++)
		{
			if (scan_mark[x]>=limit)	
			{
				u32 pixel	= scan_lmap[x];
				u32 pixel_r	= scan_rest[x];
				if (rms_diff(color_get_R(pixel_r),color_get_R(pixel))>rms)	return FALSE;
				if (rms_diff(color_get_G(pixel_r),color_get_G(pixel))>rms)	return FALSE;
				if (rms_diff(color_get_B(pixel_r),color_get_B(pixel))>rms)	return FALSE;
				if (rms_diff(color_get_A(pixel_r),color_get_A(pixel))>rms)	return FALSE;

				//.??? if (rms_diff(color_get_!!hemi!!!(pixel_r),color_get_A(pixel))>rms)	return FALSE;
			}
		}
	}
	return	TRUE;
}

BOOL	__stdcall rms_test	(lm_layer&	lm, u32 _r, u32 _g, u32 _b, u32 _a, u32 rms)
{
	u32 x,y;
	for (y=0; y<lm.height; y++)
	{
		for (x=0; x<lm.width; x++)
		{
			u32	offset	= y*lm.width+x;
			if (lm.marker[offset]>=254)	{
				u32 pixel	= lm.Pixel(offset);
				if (rms_diff(_r, color_get_R(pixel))>rms)	return FALSE;
				if (rms_diff(_g, color_get_G(pixel))>rms)	return FALSE;
				if (rms_diff(_b, color_get_B(pixel))>rms)	return FALSE;
				if (rms_diff(_a, color_get_A(pixel))>rms)	return FALSE;

				//.??? if (rms_diff(color_get_!!hemi!!!(pixel_r),color_get_A(pixel))>rms)	return FALSE;
			}
		}
	}
	return TRUE;
}

u32	__stdcall rms_average	(lm_layer& lm, base_color_c& C)
{
	u32 x,y,_count=0;

	for (y=0; y<lm.height; y++)
	{
		for (x=0; x<lm.width; x++)
		{
			u32	offset	= y*lm.width+x;
			if (lm.marker[offset]>=254)	
			{
				base_color_c	cc;
				lm.surface		[offset]._get (cc);
				C.add			(cc);
				_count			++;
			}
		}
	}
	return	_count;
}

BOOL	compress_Zero		(lm_layer& lm, u32 rms)
{
	// Average color
	base_color_c	_c;
	u32				_count	= rms_average(lm,_c);

	if (0==_count)	{
		clMsg	("* ERROR: Lightmap not calculated (T:%d)");
		return	FALSE;
	} else		_c.scale(_count);

	// Compress if needed
	//.???
	u8	_r	= u8_clr	(_c.rgb.x	);
	u8	_g	= u8_clr	(_c.rgb.y	);
	u8	_b	= u8_clr	(_c.rgb.z	);
	u8	_a	= u8_clr	(_c.sun		);
	if (rms_test(lm,_r,_g,_b,_a,rms))
	{
		u32		c_x			= BORDER*2;
		u32		c_y			= BORDER*2;
		base_color ccc;		ccc._set(_c);
		lm.surface.assign	(c_x*c_y,ccc);
		lm.marker.assign	(c_x*c_y,255);
		lm.height			= 0;
		lm.width			= 0;
		return TRUE;
	}
	return FALSE;
}

BOOL	compress_RMS		(lm_layer& lm, u32 rms, u32& w, u32& h)
{
	// *** Try to bilinearly filter lightmap down and up
	w=0, h=0;
	if (lm.width>=2)	{
		w = lm.width/2;
		if (!rms_test(lm,w,lm.height,rms))	{
			// 3/4
			w = (lm.width*3)/4;
			if (!rms_test(lm,w,lm.height,rms))	w = 0;
		} else {
			// 1/4
			u32 nw = (lm.width*1)/4;
			if (rms_test(lm,nw,lm.height,rms))	w = nw;
		}
	}
	if (lm.height>=2)	{
		h = lm.height/2;
		if (!rms_test(lm,lm.width,h,rms))		{
			// 3/4
			h = (lm.height*3)/4;
			if (!rms_test(lm,lm.width,h,rms))		h = 0;
		} else {
			// 1/4
			u32 nh = (lm.height*1)/4;
			if (rms_test(lm,lm.width,nh,rms))		h = nh;
		}
	}
	if (w || h)	{
		if (0==w)	w = lm.width;
		if (0==h)	h = lm.height;
		//		clMsg	("* RMS: [%d,%d] => [%d,%d]",lm.width,lm.height,w,h);
		return TRUE;
	}
	return FALSE;
}

void CDeflector::L_Calculate(CDB::COLLIDER* DB, base_lighting* LightsSelected, HASH& H)
{
	try {
		lm_layer&		lm	= layer;

		// UV & HASH
		RemapUV			(0,0,lm.width,lm.height,lm.width,lm.height,FALSE);
		Fbox2			bounds;
		Bounds_Summary	(bounds);
		H.initialize	(bounds,(u32)UVpolys.size());
		for (u32 fid=0; fid<UVpolys.size(); fid++)	{
			UVtri* T	= &(UVpolys[fid]);
			Bounds		(fid,bounds);
			H.add		(bounds,T);
		}

		// Calculate
		R_ASSERT		(lm.width	<=(c_LMAP_size-2*BORDER));
		R_ASSERT		(lm.height	<=(c_LMAP_size-2*BORDER));
		lm.create		(lm.width,lm.height);
		L_Direct		(DB,LightsSelected,H);
	} catch (...)
	{
		clMsg("* ERROR: CDeflector::L_Calculate");
	}
}

void CDeflector::Light(CDB::COLLIDER* DB, base_lighting* LightsSelected, HASH& H)
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

	// Convert lights to local form
	LightsSelected->select(pBuild->L_static,Sphere.P,Sphere.R);

	// Calculate and fill borders
	L_Calculate			(DB,LightsSelected,H);
	for (u32 ref=254; ref>0; ref--) if (!ApplyBorders(layer,ref)) break;

	// Compression
	try {
		u32	w,h;
		if (compress_Zero(layer,rms_zero))	return;		// already with borders
		else if (compress_RMS(layer,rms_shrink,w,h))	
		{
			// Reacalculate lightmap at lower resolution
			layer.create	(w,h);
			L_Calculate		(DB,LightsSelected,H);
		}
	} catch (...)
	{
		clMsg("* ERROR: CDeflector::Light - Compression");
	}

	// Expand with borders
	try {
		if (layer.width==1)	
		{
			// Horizontal ZERO - vertical line
			lm_layer		T;
			T.create		(2*BORDER,layer.height+2*BORDER);

			// Transfer
			for (u32 y=0; y<T.height; y++)
			{
				int			py		= int(y)-BORDER;
				clamp				(py,0,int(layer.height-1));
				base_color	C		= layer.surface[py];
				T.surface[y*2+0]	= C; 
				T.marker [y*2+0]	= 255;
				T.surface[y*2+1]	= C;
				T.marker [y*2+1]	= 255;
			}

			// Exchange
			T.width			= 0;
			T.height		= layer.height;
			layer			= T;
		} else if (layer.height==1) 
		{
			// Vertical ZERO - horizontal line
			lm_layer		T;
			T.create		(layer.width+2*BORDER, 2*BORDER);

			// Transfer
			for (u32 x=0; x<T.width; x++)
			{
				int			px			= int(x)-BORDER;
				clamp					(px,0,int(layer.width-1));
				base_color	C			= layer.surface[px];
				T.surface	[0*T.width+x]	= C;
				T.marker	[0*T.width+x]	= 255;
				T.surface	[1*T.width+x]	= C;
				T.marker	[1*T.width+x]	= 255;
			}

			// Exchange
			T.width			= layer.width;
			T.height		= 0;
			layer			= T;
		} else {
			// Generic blit
			lm_layer		lm_old	= layer;
			lm_layer		lm_new;
			lm_new.create			(lm_old.width+2*BORDER,lm_old.height+2*BORDER);
			lblit					(lm_new,lm_old,BORDER,BORDER,255-BORDER);
			layer					= lm_new;
			ApplyBorders			(layer,254);
			ApplyBorders			(layer,253);
			ApplyBorders			(layer,252);
			ApplyBorders			(layer,251);
			for	(u32 ref=250; ref>0; ref--) if (!ApplyBorders(layer,ref)) break;
			layer.width				= lm_old.width;
			layer.height			= lm_old.height;
		}
	} catch (...)
	{
		clMsg("* ERROR: CDeflector::Light - BorderExpansion");
	}
}
