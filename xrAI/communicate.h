#ifndef _XR_COMM_
#define _XR_COMM_

#pragma pack(push,4)

const DWORD XR_MAX_TEXTURES		= 32;
const DWORD XR_MAX_PORTAL_VERTS	= 6;

// All types to interact with xrLC
typedef Fvector			b_vertex;

struct b_face
{
	DWORD				v[3];				// vertices
	Fvector2			t[3];				// TC
	WORD				dwMaterial;			// index of material
};

struct b_material
{
	WORD				surfidx;			// indices of texture surface
	WORD				shader;				// index of shader that combine them
	WORD				shader_xrlc;		// compiler options
	WORD				sector;				// ***
	WORD				lod_id;				// WORD(-1) = no lod, just static geometry
	WORD				reserved;			// 
};

struct b_shader
{
	string128			name;
};

struct b_texture
{
	string128			name;
	DWORD				dwWidth;
	DWORD				dwHeight;
	BOOL				bHasAlpha;
	DWORD*				pSurface;
};

struct b_light_control						// controller or "layer", 30fps
{
	string64			name;
	DWORD				count;
	// DWORD			data[];
};

struct b_light_static						// For static lighting
{
	DWORD				controller_ID;		// DWORD(-1) = no controller
	Flight				data;
};

struct b_light_dynamic						// For dynamic models
{
	DWORD				controller_ID;		// DWORD(-1) = no controller
	Flight				data;
	svector<WORD,16>	sectors;
};

struct b_glow
{
	Fvector				P;
	float				size;
	DWORD				flags;				// 0x01 = non scalable
	DWORD				dwMaterial;			// index of material
};

struct b_portal
{
	WORD				sector_front;
	WORD				sector_back;
	svector<Fvector,XR_MAX_PORTAL_VERTS>	vertices;
};

struct b_lod_face
{
	Fvector				v		[4];
	Fvector2			t		[4];
};

struct b_lod
{
	b_lod_face			faces	[4];
	DWORD				dwMaterial;
};

struct b_params
{
	// Normals & optimization
	float		m_sm_angle;			// normal smooth angle		- 89.0
	float		m_weld_distance;	// by default 0.005f		- 5mm

	// Vertex buffers
	DWORD		m_VB_maxSize;		// by default: 8mb
	DWORD		m_VB_maxVertices;	// by default: 64k

	// Subdivision & PVS
	float		m_SS_maxsize;		// max object size - by default: 48m
	float		m_SS_merge_coeff;	// perform addititional merge - 0..1
	int			m_SS_Low;			// spatial subdivisions low limit - 32
	int			m_SS_High;			// spatial subdivisions high limit - 2048
	int			m_SS_DedicateCached;// dedicate to cached pipeline if less than # faces, 16

	// Light maps
	BOOL		m_bLightMaps;
	float		m_lm_split_angle;	// deflectors split angle	- 89.0
	float		m_lm_pixels_per_meter;// LM - by default: 4 ppm
	float		m_lm_dither;		// amount of distortion		- 0.02
	float		m_lm_jitter;		// jittering distortion		- 0.45
	int			m_lm_jitter_samples;// 1/4/9 - by default		- 4
	Fcolor		m_lm_amb_color;		// color of lightmaps ambinet lighting  (def: 1,1,1)
	float		m_lm_amb_fogness;	// in percents
	DWORD		m_lm_rms_zero;		// RMS - after what the lightmap will be shrinked to ZERO pixels
	DWORD		m_lm_rms;			// RMS - shrink and recalc
	
	// Area(hemi-sphere) lighting
	Fcolor		area_color;			//
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
	int			m_vCacheSize;		// R200 = 10, R300 = 12, GeForce(1/2/MX) = 16/12, GeForce3 = 24/16

	string128	L_name;				// Level name

    void        Init()
	{
        // Normals & optimization
        m_sm_angle              = 89.f;
        m_weld_distance         = 0.005f;

        // Vertex buffers
        m_VB_maxSize            = 8192*1024;
        m_VB_maxVertices        = 65535;

        // Subdivision & PVS
        m_SS_maxsize            = 32;
		m_SS_merge_coeff		= 0.5f;
		m_SS_Low				= 32;
		m_SS_High				= 2048;
		m_SS_DedicateCached		= 4;

        // Light maps
        m_bLightMaps            = TRUE;
        m_lm_split_angle		= 87.f;
        m_lm_pixels_per_meter   = 14;
        m_lm_dither             = 0.02f;
        m_lm_jitter             = 0.49f;
		m_lm_jitter_samples		= 4;
        m_lm_amb_color.set      (1,1,1,0);
        m_lm_amb_fogness        = 0.05f;
		m_lm_rms_zero			= 4;
		m_lm_rms				= 4;

		// Area(hemi-sphere) lighting
		area_color.set			(1,1,1,1);
		area_dispersion			= 7.f;
		area_energy_summary		= 1.f;
		area_quality			= 2;

		// Light smoothing
		fuzzy_min				= 0.2f;
		fuzzy_max				= 0.5f;
		fuzzy_samples			= 12;
		fuzzy_enable			= FALSE;

        // Progressive
        m_bConvertProgressive   = FALSE;
        m_pm_uv                 = 0.5f;
        m_pm_pos			    = 1.f;
        m_pm_curv			    = 1.f;
		m_pm_borderH_angle		= 150.f;
		m_pm_borderH_distance	= 0.04f;
		m_pm_heuristic			= 0.85f;

		// Strippifier
		m_bStripify				= TRUE;
		m_vCacheSize			= 16;

        L_name[0]				= 0;
    }
	void		setDebug()
	{
		m_bConvertProgressive	= FALSE;
		m_lm_pixels_per_meter	= 4;
		m_lm_jitter_samples		= 4;
		area_quality			= 1;
		fuzzy_enable			= FALSE;
	}
	void		setRelease()
	{
		m_bConvertProgressive	= FALSE;
		m_lm_pixels_per_meter	= 14;
		m_lm_jitter_samples		= 9;
		area_quality			= 2;
		fuzzy_enable			= FALSE;
	}
};
#pragma pack(pop)

enum EBUILD_CHUNKS
{
	EB_Version			= 0,	// XRCLC_CURRENT_VERSION
	EB_Parameters,
	EB_Vertices,
	EB_Faces,
	EB_Materials,
	EB_Shaders_Render,
	EB_Shaders_Compile,
	EB_Textures,
	EB_Glows,
	EB_Portals,
	EB_Light_control,
	EB_Light_static,
	EB_Light_dynamic,
	EB_LOD_models,

	EB_FORCE_DWORD = DWORD(-1)
};

#endif
