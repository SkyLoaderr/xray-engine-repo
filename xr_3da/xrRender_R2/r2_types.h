#pragma once

#define	r2_RT_DEFFER	"$user$deffer"			// *MET
#define	r2_RT_P			"$user$position"		// MRT
#define	r2_RT_N_H		"$user$normal_hemi"		// MRT
#define	r2_RT_D_G		"$user$color_gloss"		// MRT
#define	r2_RT_accum		"$user$accum"			// ---
#define	r2_RT_generic	"$user$generic"			// ---
#define	r2_RT_bloom1	"$user$bloom1"			// ---
#define	r2_RT_bloom2	"$user$bloom2"			// ---
#define	r2_RT_smap_d	"$user$smap_d"			// ---
#define	r2_RT_smap_p	"$user$smap_p"			// ---

#define r2_material		"$user$material"		// ---
#define r2_ds2_fade		"$user$ds2_fade"		// ---

const u32				DSM_size			= 1024;
const float				DSM_distance_1		= 15.f;
const float				DSM_distance_2		= 100.f;
const float				DSM_d_range			= 100.f;

const u32				PSM_size			= 512;
const float				PSM_near_plane		= .07f;

const u32				TEX_material_LdotN	= 128;	// diffuse,		X
const u32				TEX_material_LdotH	= 256;	// specular,	Y
const u32				TEX_ds2_fade_size	= 256;
