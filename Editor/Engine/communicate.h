#ifndef _XR_COMM_
#define _XR_COMM_

#include "Light.h"
#include "xrShaderTypes.h"

#pragma pack(push,8)

const DWORD XR_MAX_UVMAPS		= 8;
const DWORD XR_MAX_TEXTURES		= 32;
const DWORD XR_MAX_PORTAL_VERTS	= 6;

// All types to interact with xrLC
typedef Fvector b_vertex;

struct b_uvmap
{
	float tu;
	float tv;
};
struct b_face
{
	DWORD	v[3];							// vertices
	b_uvmap t[XR_MAX_UVMAPS][3];			// TC
	WORD	dwMaterial;						// index of material
};
struct b_material
{
	WORD	dwTexCount;
	WORD	surfidx	[XR_MAX_TEXTURES];		// indices of texture surfaces
	WORD	shader;							// index of shader that combine them
	WORD	sector;							// ***
	DWORD	reserved;
};
struct b_shader
{
	sh_name name;
};
struct b_texture
{
	sh_name	name;
	DWORD	dwWidth;
	DWORD	dwHeight;
	BOOL	bHasAlpha;
	DWORD*	pSurface;
};
struct b_light : public xrLIGHT
{
	DWORD			s_count;
	WORD			s_sectors[XRLIGHT_MAX_SECTORS];

	float			shadowed_scale;		// by default - .05f	(5%)
};
struct b_glow
{
	Fvector			P;
	float			size;
	DWORD			dwMaterial;			// index of material
};
struct b_particle
{
	Fvector			P;
	Fvector			N;
	float			size;
	DWORD			dwMaterial;
};
struct b_occluder
{
	WORD			sector;			// ***
	WORD			vert_count;
	Fvector			vertices[XR_MAX_PORTAL_VERTS];
};
struct b_portal
{
	WORD			sector_front;
	WORD			sector_back;
	DWORD			vert_count;
	Fvector			vertices[XR_MAX_PORTAL_VERTS];
};

struct b_params
{
	// Tesselation
	BOOL		m_bTesselate;
	float		m_maxedge;			// for tesselation - by default: 32m

	// Normals & optimization
	float		m_sm_angle;			// normal smooth angle		- 89.0
	float		m_weld_distance;	// by default 0.005f		- 5mm

	// Vertex buffers
	DWORD		m_VB_maxSize;		// by default: 8mb
	DWORD		m_VB_maxVertices;	// by default: 64k

	// Subdivision & PVS
	float		m_SS_maxsize;		// max object size - by default: 48m
	float		m_SS_merge_coeff;	// perform addititional merge - 0..1
	int			m_SS_Low;			// spatial subdivisions low limit - 12
	int			m_SS_High;			// spatial subdivisions high limit - 2048
	int			m_SS_DedicateCached;// dedicate to cached pipeline if less than # faces, 32

	// Light maps
	BOOL		m_bLightMaps;
	BOOL		m_bRadiosity;		// radiosity vs direct lighting
	float		m_lm_split_angle;	// deflectors split angle	- 89.0
	float		m_lm_pixels_per_meter;// LM - by default: 4 ppm
	float		m_lm_dither;		// amount of distortion		- 0.02
	float		m_lm_jitter;		// jittering distortion		- 0.45
	int			m_lm_jitter_samples;// 1/4/9 - by default		- 4
	Fcolor		m_lm_amb_color;		// color of lightmaps ambinet lighting  (def: 1,1,1)
	float		m_lm_amb_fogness;	// in percents
	float		m_lm_rms;			// Resolution RootMeanSquare error 
	
	// Area(hemi-sphere) lighting
	Fcolor		areaDark,areaGround;//
	float		area_dispersion;	// Angle of 'sun' - our sun has around 5 degrees, recomended around 7.5 degrees
	float		area_energy_summary;// Amount of energy distributed across hemisphere
	int			area_quality;		//
	
	// Light smoothing
	float		fuzzy_min;			// in meters - min dist 
	float		fuzzy_max;			// in meters - max dist
	int			fuzzy_samples;		// number of samples = 16;
	BOOL		fuzzy_enable;		// TRUE/FALSE - 'cause of slowness - FALSE is better :)

	// Progressive
	BOOL		m_bConvertProgressive;
	float		m_pm_uv;			// UV weight				- 0.5
	float		m_pm_pos;			// Position weight			- 1.f
	float		m_pm_curv;			// Curvature weight			- 1.f
	float		m_pm_borderH_angle;	// higher the angle - more critical borders - 150
	float		m_pm_borderH_distance;// higher the value (m) - less critical B	- 0.05f;
	float		m_pm_heuristic;		// Stripify vs Progressive	- 0.85f;

	// Strippifier
	BOOL		m_bStripify;		// true
	int			m_vCacheSize;		// Radeon = 16, Radeon2 = 24, GeForce(1/2/MX) = 16, GeForce3 = 24

	char		L_name[64];			// Level name
	char		L_path[256];		// Path to level. Like "x:\game\data\try\"

    void        Init(){
        m_bTesselate            = FALSE;
        m_maxedge               = 32;

        // Normals & optimization
        m_sm_angle              = 89.f;
        m_weld_distance         = 0.001f;

        // Vertex buffers
        m_VB_maxSize            = 8192*1024;
        m_VB_maxVertices        = 65535;

        // Subdivision & PVS
        m_SS_maxsize            = 48;
		m_SS_merge_coeff		= 0.5f;
		m_SS_Low				= 24;
		m_SS_High				= 2048;
		m_SS_DedicateCached		= 32;

        // Light maps
        m_bLightMaps            = TRUE;
		m_bRadiosity			= FALSE;
        m_lm_split_angle		= 87.f;
        m_lm_pixels_per_meter   = 4;
        m_lm_dither             = 0.02f;
        m_lm_jitter             = 0.45f;
		m_lm_jitter_samples		= 4;
        m_lm_amb_color.set      (1,1,1,0);
        m_lm_amb_fogness        = 0.01f;
		m_lm_rms				= 0.05f;	// 5%

		// Area(hemi-sphere) lighting
		areaDark.set			(1,0,0,0);
		areaGround.set			(0,1,0,0);
		area_dispersion			= 7.5f;
		area_energy_summary		= 0.5f;
		area_quality			= 1;

		// Light smoothing
		fuzzy_min				= 0.3f;
		fuzzy_max				= 1.f;
		fuzzy_samples			= 16;
		fuzzy_enable			= FALSE;

        // Progressive
        m_bConvertProgressive   = TRUE;
        m_pm_uv                 = 0.5f;
        m_pm_pos			    = 1.f;
        m_pm_curv			    = 1.f;
		m_pm_borderH_angle		= 150.f;
		m_pm_borderH_distance	= 0.04f;
		m_pm_heuristic			= 0.85f;

		// Strippifier
		m_bStripify				= TRUE;
		m_vCacheSize			= 16;

        L_name[0] = 0;
        L_path[0] = 0;
    }
};

struct b_transfer
{
	DWORD		_version;		// XRCL_CURRENT_VERSION (4)
	b_params	params;

	b_vertex*	vertices;
	DWORD		vertex_count;
	b_face*		faces;
	DWORD		face_count;
	b_material*	material;
	DWORD		mtl_count;
	b_shader*	shaders;
	DWORD		shader_count;
	b_texture*	textures;
	DWORD		tex_count;
	b_light*	lights;
	DWORD		light_count;
	Flight*		light_keys;
	DWORD		light_keys_count; // ***
	b_glow*		glows;
	DWORD		glow_count;
	b_particle*	particles;
	DWORD		particle_count;
	b_occluder*	occluders;
	DWORD		occluder_count;
	b_portal*	portals;		
	DWORD		portal_count;
};
#pragma pack(pop)

#endif
