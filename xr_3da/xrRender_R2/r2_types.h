#pragma once

// r3xx code-path (MRT)
#define		r2_RT_depth			"$user$depth"			// MRT
#define		r2_RT_P				"$user$position"		// MRT
#define		r2_RT_N				"$user$normal"			// MRT
#define		r2_RT_albedo		"$user$albedo"			// MRT

// other
#define		r2_RT_accum			"$user$accum"			// ---	16 bit fp or 16 bit fx

#define		r2_RT_generic0		"$user$generic0"		// ---
#define		r2_RT_generic1		"$user$generic1"		// ---

#define		r2_RT_bloom1		"$user$bloom1"			// ---
#define		r2_RT_bloom2		"$user$bloom2"			// ---

#define		r2_RT_luminance_t64	"$user$lum_t64"			// --- temp
#define		r2_RT_luminance_t8	"$user$lum_t8"			// --- temp
#define		r2_RT_luminance		"$user$tonemap"			// --- result

#define		r2_RT_smap_surf		"$user$smap_surf"		// ---	directional
#define		r2_RT_smap_depth	"$user$smap_depth"		// ---	directional

#define		r2_material			"$user$material"		// ---
#define		r2_ds2_fade			"$user$ds2_fade"		// ---

#define		r2_jitter			"$user$jitter_"			// --- dither

#define		JITTER(a) r2_jitter #a

const		float				SMAP_near_plane		= .1f	;

const		u32					SMAP_adapt_min		= 32	;
const		u32					SMAP_adapt_optimal	= 512	;
const		u32					SMAP_adapt_max		= 1024	;

const		u32					TEX_material_LdotN	= 128	;	// diffuse,		X, almost linear = small res
const		u32					TEX_material_LdotH	= 256	;	// specular,	Y
const		u32					TEX_jitter			= 64	;
const		u32					TEX_jitter_count	= 2		;

const		u32					BLOOM_size_X		= 256	;
const		u32					BLOOM_size_Y		= 256	;
const		u32					LUMINANCE_size		= 16	;

const		u32					occq_size			= 128	;	// queue for occlusion queries

// spot
#define		SE_L_FILL			0
#define		SE_L_UNSHADOWED		1
#define		SE_L_NORMAL			2	// typical, scaled
#define		SE_L_FULLSIZE		3	// full texture coverage
#define		SE_L_TRANSLUENT		4	// with opacity/color mask

// mask
#define		SE_MASK_SPOT		0
#define		SE_MASK_POINT		1
#define		SE_MASK_DIRECT		2

IC	float	u_diffuse2s	(float x, float y, float z)	{ float	v = (x+y+z)/3.f;	return (v<1)?sqrt(v):v; }
IC	float	u_diffuse2s	(Fvector3& c)				{ return u_diffuse2s(c.x,c.y,c.z);					}
