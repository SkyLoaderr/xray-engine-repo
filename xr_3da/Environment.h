// Environment.h: interface for the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVIRONMENT_H__155A6D9E_2ECC_4DCC_80B2_27CD3F070B41__INCLUDED_)
#define AFX_ENVIRONMENT_H__155A6D9E_2ECC_4DCC_80B2_27CD3F070B41__INCLUDED_

class ENGINE_API CSun;
class ENGINE_API IRender_Visual;
class ENGINE_API CInifile;
class ENGINE_API CSoundStream;

enum {
	effSunGlare		= (1ul<<0ul),
	effGlows		= (1ul<<1ul)
};

class ENGINE_API	CEnvDescriptor
{
public:
	ref_texture_list	sky_r_textures;			// C
	float				sky_factor;		// C

	ref_texture			sky_texture;
	Fvector3			sky_color;

	float				far_plane;

	Fvector3			fog_color;
	float				fog_density;
	float				fog_near;		// C
	float				fog_far;		// C

	Fvector3			ambient;
	Fvector3			lmap_color;
	Fvector3			hemi_color;
	Fvector3			sun_color;
	Fvector3			sun_dir;

	void				load		(LPCSTR sect);
	void				lerp		(CEnvDescriptor& A, CEnvDescriptor& B, float f);
};

class ENGINE_API	CEnvironment
{
public:
	xr_vector<CSun*>			Suns;
	xr_vector<CSoundStream*>	Music;
	int							Music_Active;
	int							Music_Fade;
public:
	// Environments
	CEnvDescriptor				Current;
	xr_vector<CEnvDescriptor>	Palette;
	ref_shader					sh_2sky;
	ref_geom					sh_2geom;
public:
					CEnvironment		();
					~CEnvironment		();

	void			Load				(CInifile *pIni, char* section);

	void			OnFrame				();

	void			RenderFirst			();
	void			RenderLast			();
};

ENGINE_API extern Flags32	psEnvFlags;
ENGINE_API extern float		psGravity;

#endif // !defined(AFX_ENVIRONMENT_H__155A6D9E_2ECC_4DCC_80B2_27CD3F070B41__INCLUDED_)
