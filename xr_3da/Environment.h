#ifndef EnvironmentH
#define EnvironmentH

class ENGINE_API CLensFlare;
class ENGINE_API IRender_Visual;
class ENGINE_API CInifile;

class ENGINE_API	CEnvDescriptor
{
public:
	STextureList		sky_r_textures;	// C
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

	CLensFlare*			lens_flare;

	void				load		(LPCSTR sect);
	void				lerp		(CEnvDescriptor& A, CEnvDescriptor& B, float f);
};

class ENGINE_API	CEnvironment
{
public:
	// Environments
	CEnvDescriptor				Current;
	xr_vector<CEnvDescriptor>	Palette;
	ref_shader					sh_2sky;
	ref_geom					sh_2geom;
public:
					CEnvironment		();
					~CEnvironment		();

	void			load				();

	void			OnFrame				();

	void			RenderFirst			();
	void			RenderLast			();
};

ENGINE_API extern Flags32	psEnvFlags;
ENGINE_API extern float		psGravity;

#endif //EnvironmentH
y;

#endif //EnvironmentH
