#ifndef EnvironmentH
#define EnvironmentH

// refs
class ENGINE_API	IRender_Visual;
class ENGINE_API	CInifile;
class ENGINE_API 	CEnvironment;

// refs - effects
class ENGINE_API	CEnvironment;
class ENGINE_API	CLensFlare;	
class ENGINE_API	CEffect_Rain;
class ENGINE_API	CEffect_Thunderbolt;

class ENGINE_API	CPerlinNoise1D;
// t-defs
class ENGINE_API	CEnvModifier
{
public:
	Fvector3			position;
	float				radius;
	float				power;

	float				far_plane;
	Fvector3			fog_color;
	float				fog_density;
	Fvector3			ambient;
	Fvector3			lmap_color;

	void				load		(IReader*		fs);
	float				sum			(CEnvModifier&	_another, Fvector3& view);
};

class ENGINE_API	CEnvDescriptor
{
public:
    float				exec_time;
	
	STextureList		sky_r_textures;		// C
	STextureList		sky_r_textures_env;	// C
	float				sky_factor;			// C

	ref_texture			sky_texture;
	ref_texture			sky_texture_env;
	Fvector3			sky_color;

	float				far_plane;

	Fvector3			fog_color;
	float				fog_density;
	float				fog_near;		// C
	float				fog_far;		// C

	float				rain_density;
	Fvector3			rain_color;

	float				bolt_period;
	float				bolt_duration;

    float				wind_velocity;
    float				wind_direction;      
    
	Fvector3			ambient;
	Fvector3			lmap_color;
	Fvector3			hemi_color;
	Fvector3			sun_color;
	Fvector3			sun_dir;

    int					lens_flare_id;
    BOOL				thunderbolt;
    
						CEnvDescriptor	();

	void				load			(LPCSTR exec_tm, LPCSTR sect, CEnvironment* parent);
    void				unload			();
    
	void				lerp			(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power);
};

class ENGINE_API	CEnvironment
{
	struct str_pred : public std::binary_function<shared_str, shared_str, bool>	{	
		IC bool operator()(const shared_str& x, const shared_str& y) const
		{	return xr_strcmp(x,y)<0;	}
	};
public:
	DEFINE_VECTOR			(CEnvDescriptor*,EnvVec,EnvIt);
	DEFINE_MAP_PRED			(shared_str,EnvVec,WeatherMap,WeatherPairIt,str_pred);

	// Environments
	CEnvDescriptor			CurrentEnv;
	CEnvDescriptor*			CurrentA;
	CEnvDescriptor*			CurrentB;
    bool					bTerminator;
    
    EnvVec*					CurrentWeather;
    shared_str					CurrentWeatherName;
	WeatherMap				Weathers;
	xr_vector<CEnvModifier>	Modifiers;

	float					wind_strength;	

	ref_shader				sh_2sky;
	ref_geom				sh_2geom;

	CEffect_Rain*			eff_Rain;
	CLensFlare*				eff_LensFlare;
	CEffect_Thunderbolt*	eff_Thunderbolt;

	float					fGameTime;
	float					fTimeFactor;
	ref_texture				tonemap;

	// music interface
	struct	music
	{
		ref_sound			left;
		ref_sound			right;
		bool				playing		()		{ return left.feedback && right.feedback; }
	};
	xr_deque<music*>		playlist;

    void					SelectEnvs			(float gt);
public:
							CEnvironment		();
							~CEnvironment		();

	void					load				();
    void					unload				();

	void					mods_load			();
	void					mods_unload			();

	void					OnFrame				();

	void					RenderFirst			();
	void					RenderLast			();

    void					SetWeather			(shared_str name);
    shared_str					GetWeather			()					{ return CurrentWeatherName;}
	void					SetGameTime			(float game_time, float time_factor)	{ fGameTime = game_time;  fTimeFactor=time_factor;	}

	void					OnDeviceCreate		();
	void					OnDeviceDestroy		();

	// editor-related
#ifdef _EDITOR
public:
	float					ed_from_time;
	float					ed_to_time;
public:
    void					ED_Reload			();
#endif
};

ENGINE_API extern Flags32	psEnvFlags;
ENGINE_API extern float		psVisDistance;

#endif //EnvironmentH
