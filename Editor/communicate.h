#ifndef _XR_COMM_
#define _XR_COMM_

#pragma pack(push,8)

#define XR_MAX_TEXTURES	16

typedef unsigned __int64	T_CLSID;

// All types to interact with BuilderDLL

typedef Fvector b_vertex;

struct b_uvmap
{
	float u;
	float v;
};
struct b_face
{
	DWORD   v[3];					// vertices
	b_uvmap t[XR_MAX_TEXTURES][3];	// TC
	DWORD   dwRMode;				// OGF-render mode
	DWORD   dwMaterial;				// index of material
};
struct b_material
{
	DWORD	dwTexCount;
	DWORD	surfidx	[16];			// indices of texture surfaces
	DWORD	shader;					// index of shader that combine them
	T_CLSID	tclass	[16];			// class id of textures
	DWORD	script	[16];			// scripting(names) applyed to them
};
struct b_shader
{
	char	name[64];				// name of the shader
};
struct b_script
{
	char	script[64];				// name of the texture-transform script
};
struct b_texture
{
	char	name[64];
	DWORD	dwWidth;
	DWORD	dwHeight;
	BOOL	bHasAlpha;
	DWORD*	pSurface;
};
struct b_light
{
    DWORD           type;				// Type of light source
    Fcolor          diffuse;			// Diffuse color of light
    Fcolor          specular;			// Specular color of light
    Fcolor          ambient;			// Ambient color of light
    Fvector         position;			// Position in world space
    Fvector         direction;			// Direction in world space
    float		    range;				// Cutoff range
    float	        falloff;			// Falloff
    float	        attenuation0;		// Constant attenuation
    float	        attenuation1;		// Linear attenuation
    float	        attenuation2;		// Quadratic attenuation
    float	        theta;				// Inner angle of spotlight cone
    float	        phi;				// Outer angle of spotlight cone
};

struct b_transfer
{
	b_vertex*	vertices;
	DWORD		vertex_count;
	b_face*		faces;
	DWORD		face_count;
	b_material*	material;
	DWORD		mtl_count;
	b_shader*	shaders;
	DWORD		shader_count;
	b_script*	scripts;
	DWORD		script_count;
	b_texture*	textures;
	DWORD		tex_count;
	b_light*	lights;
	DWORD		light_count;

	BOOL		m_bTesselate;
	BOOL		m_bConvertProgressive;
	BOOL		m_bLightMaps;

	float		m_maxedge;			// for tesselation - by default: 1m
	DWORD		m_VB_maxSize;		// by default: 64k
	DWORD		m_VB_maxVertices;	// by default: 4096
	float		m_pixels_per_meter;	// LM - by default: 20 ppm
	float		m_maxsize;			// max object size - by default: 16m
	float		m_relevance;		// dimension of VIS slot - by default: 10m
	float		m_viewdist;			// view distance - by default: 100m
};
// if add new build options:
//  - add to b_transfer struct
//  - add to frmSceneProperties (component, action init/ok)
//  - add to Scene class (add variable, reset in globals, load globals, save globals)
//  - copy from scene class to TSData in builder
#pragma pack(pop)

#define MK_CLSID(a,b,c,d,e,f,g,h) \
	T_CLSID(	((T_CLSID(a)<<24)|(T_CLSID(b)<<16)|(T_CLSID(c)<<8)|(T_CLSID(d)))<<32 | \
				((T_CLSID(e)<<24)|(T_CLSID(f)<<16)|(T_CLSID(g)<<8)|(T_CLSID(h))) )

void CLSID2TEXT(T_CLSID id, char *text) {
	text[8]=0;
	for (int i=7; i>=0; i--) { text[i]=char(id&0xff); id>>=8; }
}

T_CLSID TEXT2CLSID(char *text) {
	char buf[10];
	strncpy(buf,text,9);
	int need = 8-strlen(buf);
	while (need) { buf[8-need]=' '; need--; }
	return MK_CLSID(buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
}
// Possible variations of objects

// STATIC			- True Static object
// STATIC-FREEFORM	- Can be self modified (range must be specified) - usually skeletons
// MOBILE-FLOATING	- An Logic is associated - engine treates it as "object" - range must be specified
// MOBILE			- True Mobile Object - like actor

// WARNING:
//	BuilderDLL		processes only STATIC !!!
//	VistesterDLL	performs visibility pre-culling (based on "Gravity" tester)

#endif
/*
#ifndef _XR_COMM_
#define _XR_COMM_

#pragma pack(push,8)

#define XR_MAX_TEXTURES	16

#define	B_LIGHT			512		// face want's lighmaps on it
#define B_CASTSHADOW	1024	// face casts shadows
#define B_NOTESSELATE	2048	// don't tesselate this face

typedef unsigned __int64	T_CLSID;

// All types to interact with BuilderDLL

typedef Fvector b_vertex;

struct b_uvmap
{
	float tu;
	float tv;
};
struct b_face
{
	DWORD	v[3];					// vertices
	b_uvmap t[XR_MAX_TEXTURES][3];	// TC
	DWORD	dwRMode;				// OGF-render mode
	DWORD	dwMaterial;				// index of material
};
struct b_material
{
	DWORD	dwTexCount;
	DWORD	surfidx	[16];			// indices of texture surfaces
	DWORD	shader;					// index of shader that combine them
	T_CLSID	tclass	[16];			// class id of textures
	DWORD	script	[16];			// scripting(names) applyed to them
};
struct b_shader
{
	char	name[64];				// name of the shader
};
struct b_script
{
	char	script[64];				// name of the texture-transform script
};
struct b_texture
{
	char	name[64];
	DWORD	dwWidth;
	DWORD	dwHeight;
	BOOL	bHasAlpha;
	DWORD*	pSurface;
};
struct b_light
{
    DWORD           type;				// Type of light source		
    Fcolor          diffuse;			// Diffuse color of light	
    Fcolor          specular;			// Specular color of light	
    Fcolor          ambient;			// Ambient color of light	
    Fvector         position;			// Position in world space	
    Fvector         direction;			// Direction in world space	
    float		    range;				// Cutoff range				
    float	        falloff;			// Falloff					
    float	        attenuation0;		// Constant attenuation		
    float	        attenuation1;		// Linear attenuation		
    float	        attenuation2;		// Quadratic attenuation	
    float	        theta;				// Inner angle of spotlight cone 
    float	        phi;				// Outer angle of spotlight cone 
};

struct b_transfer
{
	b_vertex*	vertices;
	DWORD		vertex_count;
	b_face*		faces;
	DWORD		face_count;
	b_material*	material;
	DWORD		mtl_count;
	b_shader*	shaders;
	DWORD		shader_count;
	b_script*	scripts;
	DWORD		script_count;
	b_texture*	textures;
	DWORD		tex_count;
	b_light*	lights;
	DWORD		light_count;

	BOOL		bPreprocess;
	BOOL		bCalcLM_UV;
	BOOL		bCalcLM_Surfaces;
	BOOL		bHandleTransparent;
};
#pragma pack(pop)

// Possible variations of objects

// STATIC			- True Static object
// STATIC-FREEFORM	- Can be self modified (range must be specified) - usually skeletons
// MOBILE-FLOATING	- An Logic is associated - engine treates it as "object" - range must be specified
// MOBILE			- True Mobile Object - like actor

// WARNING:
//	BuilderDLL		processes only STATIC !!!
//	VistesterDLL	performs visibility pre-culling (based on "Gravity" tester)

#endif
*/
