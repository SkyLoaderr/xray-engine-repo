#pragma once

// r3xx code-path (MRT)
#define	r2_RT_depth			"$user$depth"			// MRT
#define	r2_RT_P				"$user$position"		// MRT
#define	r2_RT_N				"$user$normal"			// MRT
#define	r2_RT_D_G			"$user$color_gloss"		// MRT

// other
#define	r2_RT_accum			"$user$accum"			// ---	16 bit fp or 8 bit

#define	r2_RT_generic0		"$user$generic0"		// ---
#define	r2_RT_generic1		"$user$generic1"		// ---

#define	r2_RT_bloom1		"$user$bloom1"			// ---
#define	r2_RT_bloom2		"$user$bloom2"			// ---

#define r2_RT_luminance_tmp	"$user$lum_tmp"
#define r2_RT_luminance		"$user$lum"

#define	r2_RT_smap_d_surf	"$user$smap_d_surf"		// ---	directional
#define	r2_RT_smap_d_depth	"$user$smap_d_depth"	// ---	directional
#define	r2_RT_smap_p		"$user$smap_p"			// ---	point

#define r2_material			"$user$material"						// ---
#define r2_ncm				"$user$ncm"								// --- normal encode
#define r2_float2RG			"$user$float2rg"						// --- compression/encoding table
#define r2_float2B			"$user$float2b"							// --- compression/encoding table
#define r2_ds2_fade			"$user$ds2_fade"						// ---

const u32					DSM_size			= 1024;
const float					DSM_distance_1		= 15.f;
const float					DSM_distance_2		= 100.f;
const float					DSM_d_range			= 100.f;
const float					SSM_near_plane		= .1f;

const u32					PSM_size			= 512;
const float					PSM_near_plane		= .1f;

const u32					TEX_material_LdotN	= 128;	// diffuse,		X
const u32					TEX_material_LdotH	= 256;	// specular,	Y
const u32					TEX_ds2_fade_size	= 256;
const u32					TEX_float2rgb		= 256;
const u32					TEX_NCM				= 512;

const u32					BLOOM_size_X		= 256;
const u32					BLOOM_size_Y		= 256;
const u32					LUMINANCE_size		= 16;

const u32					occq_size			= 128;
