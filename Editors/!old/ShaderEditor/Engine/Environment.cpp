#include "stdafx.h"
#pragma hdrstop

#include "resourcemanager.h"
#include "blenders\blender.h"
#include "render.h"

#include "Environment.h"
#include "xr_efflensflare.h"
#include "rain.h"
#include "thunderbolt.h"
#include "xrHemisphere.h"
#include "perlin.h"

#include "xr_trims.h"

#ifndef _EDITOR
	#include "IGame_Level.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ENGINE_API	float			psVisDistance	= 1.f;
static const float			MAX_NOISE_FREQ	= 0.3f;

//////////////////////////////////////////////////////////////////////////
// shader/blender
class CBlender_skybox		: public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: combiner";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C)
	{
		C.r_Pass			("sky2",		"sky2",		FALSE,	TRUE, FALSE);
		C.r_Sampler_clf		("s_sky0",		"$null"			);
		C.r_Sampler_clf		("s_sky1",		"$null"			);
		C.r_Sampler_rtf		("s_tonemap",	"$user$tonemap"	);	//. hack
		C.r_End				();
	}
};
static CBlender_skybox		b_skybox;

//////////////////////////////////////////////////////////////////////////
// vertex
#pragma pack(push,1)
struct v_skybox				{
	Fvector3	p;
	u32			color;
	Fvector3	uv	[2];

	void		set			(Fvector3& _p, u32 _c, Fvector3& _tc)
	{
		p					= _p;
		color				= _c;
		uv[0]				= _tc;
		uv[1]				= _tc;
	}
};
const	u32 v_skybox_fvf	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1);
struct v_clouds				{
	Fvector3	p;
	u32			color;
	u32			intensity;
	void		set			(Fvector3& _p, u32 _c, u32 _i)
	{
		p					= _p;
		color				= _c;
		intensity			= _i;
	}
};
const	u32 v_clouds_fvf	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR;
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////
// half box def
static	Fvector3	hbox_verts[24]	=
{
	{-1.f,	-1.f,	-1.f}, {-1.f,	-1.01f,	-1.f},	// down
	{ 1.f,	-1.f,	-1.f}, { 1.f,	-1.01f,	-1.f},	// down
	{-1.f,	-1.f,	 1.f}, {-1.f,	-1.01f,	 1.f},	// down
	{ 1.f,	-1.f,	 1.f}, { 1.f,	-1.01f,	 1.f},	// down
	{-1.f,	 1.f,	-1.f}, {-1.f,	 1.f,	-1.f},
	{ 1.f,	 1.f,	-1.f}, { 1.f,	 1.f,	-1.f},
	{-1.f,	 1.f,	 1.f}, {-1.f,	 1.f,	 1.f},
	{ 1.f,	 1.f,	 1.f}, { 1.f,	 1.f,	 1.f},
	{-1.f,	 0.f,	-1.f}, {-1.f,	-1.f,	-1.f},	// half
	{ 1.f,	 0.f,	-1.f}, { 1.f,	-1.f,	-1.f},	// half
	{ 1.f,	 0.f,	 1.f}, { 1.f,	-1.f,	 1.f},	// half
	{-1.f,	 0.f,	 1.f}, {-1.f,	-1.f,	 1.f}	// half
};
static	u16			hbox_faces[20*3]	=
{
	0,	 2,	 3,
	3,	 1,	 0,
	4,	 5,	 7,
	7,	 6,	 4,
	0,	 1,	 9,
	9,	 8,	 0,
	8,	 9,	 5,
	5,	 4,	 8,
	1,	 3,	10,
	10,	 9,	 1,
	9,	10,	 7,
	7,	 5,	 9,
	3,	 2,	11,
	11,	10,	 3,
	10,	11,	 6,
	6,	 7,	10,
	2,	 0,	 8,
	8,	11,	 2,
	11,	 8,	 4,
	4,	 6,	11
};

void CEnvAmbient::load(const shared_str& sect)
{
	section				= sect;
	string_path			tmp;
	// sounds
	if (pSettings->line_exist(sect,"sounds")){
		Fvector2 t		= pSettings->r_fvector2	(sect,"sound_period");
		sound_period.set(iFloor(t.x*1000.f),iFloor(t.y*1000.f));
		LPCSTR snds		= pSettings->r_string	(sect,"sounds");
		u32 cnt			= _GetItemCount(snds);
		if (cnt){
			sounds.resize(cnt);
			for (u32 k=0; k<cnt; ++k)
				sounds[k].create(TRUE,_GetItem(snds,k,tmp));
		}
	}
	// effects
	if (pSettings->line_exist(sect,"effects")){
		Fvector2 t		= pSettings->r_fvector2	(sect,"effect_period");
		effect_period.set(iFloor(t.x*1000.f),iFloor(t.y*1000.f));
		LPCSTR effs		= pSettings->r_string	(sect,"effects");
		u32 cnt			= _GetItemCount(effs);
		if (cnt){
			effects.resize(cnt);
			for (u32 k=0; k<cnt; ++k){
				_GetItem(effs,k,tmp);
				effects[k].life_time	= iFloor(pSettings->r_float(tmp,"life_time")*1000.f);
				effects[k].particles	= pSettings->r_string(tmp,"particles");		VERIFY(effects[k].particles.size());
				if (pSettings->line_exist(tmp,"sound"))
					effects[k].sound.create	(TRUE,pSettings->r_string(tmp,"sound"));
			}
		}
	}
	VERIFY(!sounds.empty() || !effects.empty());
}

//////////////////////////////////////////////////////////////////////////
// environment desc
CEnvDescriptor::CEnvDescriptor()
{
}

#define	C_CHECK(C)	if (C.x<0 || C.x>2 || C.y<0 || C.y>2 || C.z<0 || C.z>2)	{ Msg("! Invalid '%s' in env-section '%s'",#C,S);}


void CEnvDescriptor::load	(LPCSTR exec_tm, LPCSTR S, CEnvironment* parent)
{
	Ivector3 tm				={0,0,0};
	sscanf					(exec_tm,"%d:%d:%d",&tm.x,&tm.y,&tm.z);
    R_ASSERT3				((tm.x>=0)&&(tm.x<24)&&(tm.y>=0)&&(tm.y<60)&&(tm.z>=0)&&(tm.z<60),"Incorrect weather time",S);
	exec_time				= tm.x*3600.f+tm.y*60.f+tm.z;
	string_path	st,st_env;
	strcpy					(st,pSettings->r_string	(S,"sky_texture"));
	strconcat				(st_env,st,"#small"		);
	sky_texture.create		(st);
	sky_texture_env.create	(st_env);
	clouds_texture.create	(pSettings->r_string	(S,"clouds_texture"));
	LPCSTR	cldclr			= pSettings->r_string	(S,"clouds_color");
	float	multiplier		= 0, save=0;
	sscanf					(cldclr,"%f,%f,%f,%f,%f",&clouds_color.x,&clouds_color.y,&clouds_color.z,&clouds_color.w,&multiplier);
	save=clouds_color.w;	clouds_color.mul		(.5f*multiplier);		clouds_color.w	= save; 
	sky_color				= pSettings->r_fvector3	(S,"sky_color");		sky_color.mul(.5f);
	if (pSettings->line_exist(S,"sky_rotation"))	sky_rotation	= deg2rad(pSettings->r_float(S,"sky_rotation"));
	else											sky_rotation	= 0;
	far_plane				= pSettings->r_float	(S,"far_plane");
	fog_color				= pSettings->r_fvector3	(S,"fog_color");
	fog_density				= pSettings->r_float	(S,"fog_density");
	rain_density			= pSettings->r_float	(S,"rain_density");		clamp(rain_density,0.f,1.f);
	rain_color				= pSettings->r_fvector3	(S,"rain_color");            
    wind_velocity			= pSettings->r_float	(S,"wind_velocity");
    wind_direction			= deg2rad(pSettings->r_float(S,"wind_direction"));
	gust_factor				= pSettings->r_float	(S,"gust_factor");		clamp(gust_factor,0.f,1.f);
	ambient					= pSettings->r_fvector3	(S,"ambient");
	lmap_color				= pSettings->r_fvector3	(S,"lmap_color");
	hemi_color				= pSettings->r_fvector4	(S,"hemi_color");
	sun_color				= pSettings->r_fvector3	(S,"sun_color");
	Fvector2 sund			= pSettings->r_fvector2	(S,"sun_dir");	sun_dir.setHP	(deg2rad(sund.y),deg2rad(sund.x));
    lens_flare_id			= parent->eff_LensFlare->AppendDef(pSettings,pSettings->r_string(S,"flares"));
    thunderbolt				= pSettings->r_bool		(S,"thunderbolt");
	bolt_period				= thunderbolt?pSettings->r_float	(S,"bolt_period"):0.f;
	bolt_duration			= thunderbolt?pSettings->r_float	(S,"bolt_duration"):0.f;
	env_ambient				= pSettings->line_exist(S,"env_ambient")?parent->AppendEnvAmb	(pSettings->r_string(S,"env_ambient")):0;

	C_CHECK					(clouds_color);
	C_CHECK					(sky_color	);
	C_CHECK					(fog_color	);
	C_CHECK					(rain_color	);
	C_CHECK					(ambient	);
	C_CHECK					(lmap_color	);
	C_CHECK					(hemi_color	);
	C_CHECK					(sun_color	);
}
void CEnvDescriptor::unload	()
{
	std::pair<u32,ref_texture>	zero = mk_pair(u32(0),ref_texture(0));
	sky_r_textures.clear		();
	sky_r_textures.push_back	(zero);
	sky_r_textures.push_back	(zero);
	sky_r_textures.push_back	(zero);

	sky_r_textures_env.clear	();
	sky_r_textures_env.push_back(zero);
	sky_r_textures_env.push_back(zero);
	sky_r_textures_env.push_back(zero);

	clouds_r_textures.clear		();
	clouds_r_textures.push_back	(zero);
	clouds_r_textures.push_back	(zero);
	clouds_r_textures.push_back	(zero);
}
void CEnvDescriptor::lerp	(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power)
{
	float	_power			=	1.f/(m_power+1);	// the environment itself
	float	fi				=	1-f;

	sky_r_textures.clear		();
	sky_r_textures.push_back	(mk_pair(0,A.sky_texture));
	sky_r_textures.push_back	(mk_pair(1,B.sky_texture));

	sky_r_textures_env.clear	();
	sky_r_textures_env.push_back(mk_pair(0,A.sky_texture_env));
	sky_r_textures_env.push_back(mk_pair(1,B.sky_texture_env));

	clouds_r_textures.clear		();
	clouds_r_textures.push_back	(mk_pair(0,A.clouds_texture));
	clouds_r_textures.push_back	(mk_pair(1,B.clouds_texture));

	weight					=	f;

	clouds_color.lerp		(A.clouds_color,B.clouds_color,f);
	sky_color.lerp			(A.sky_color,B.sky_color,f);
	sky_rotation			=	(fi*A.sky_rotation + f*B.sky_rotation);
	far_plane				=	(fi*A.far_plane + f*B.far_plane + M.far_plane)*psVisDistance*_power;
	fog_color.lerp			(A.fog_color,B.fog_color,f).add(M.fog_color).mul(_power);
	fog_density				=	(fi*A.fog_density + f*B.fog_density + M.fog_density)*_power;
	fog_near				=	(1.0f - fog_density)*0.85f * far_plane;
	fog_far					=	0.99f * far_plane;
	rain_density			=	fi*A.rain_density + f*B.rain_density;
	rain_color.lerp			(A.rain_color,B.rain_color,f);
	bolt_period				=	fi*A.bolt_period + f*B.bolt_period;
	bolt_duration			=	fi*A.bolt_duration + f*B.bolt_duration;
	// wind
	gust_factor				=	fi*A.gust_factor + f*B.gust_factor;
    wind_velocity			=	fi*A.wind_velocity + f*B.wind_velocity;
    wind_direction			=	fi*A.wind_direction + f*B.wind_direction;
	// colors
	ambient.lerp			(A.ambient,B.ambient,f).add(M.ambient).mul(_power);
	lmap_color.lerp			(A.lmap_color,B.lmap_color,f).add(M.lmap_color).mul(_power);
	hemi_color.lerp			(A.hemi_color,B.hemi_color,f);
	sun_color.lerp			(A.sun_color,B.sun_color,f);
	sun_dir.lerp			(A.sun_dir,B.sun_dir,f).normalize();
}
void	CEnvModifier::load	(IReader* fs)
{
					fs->r_fvector3	(position);
	radius			= fs->r_float	();
	power			= fs->r_float	();
	far_plane		= fs->r_float	();
					fs->r_fvector3	(fog_color);
	fog_density		= fs->r_float	();
					fs->r_fvector3	(ambient);
					fs->r_fvector3	(lmap_color);
}
float	CEnvModifier::sum	(CEnvModifier& M, Fvector3& view)
{
	float	_dist_sq	=	view.distance_to_sqr(M.position);
	if (_dist_sq>=(M.radius*M.radius))	return 0;
	float	_att		=	1-_sqrt(_dist_sq)/M.radius;	//[0..1];
	float	_power		=	M.power*_att;
	far_plane			+=	M.far_plane*_power;
	fog_color.mad		(M.fog_color,_power);
	fog_density			+=	M.fog_density*_power;
	ambient.mad			(M.ambient,_power);
	lmap_color.mad		(M.lmap_color,_power);
	return				_power;
}

void	CEnvironment::mods_load			()
{
	Modifiers.clear_and_free			();
	string_path							path;
	if (FS.exist(path,"$level$","level.env_mod"))	
	{
		IReader*	fs	= FS.r_open		(path);
		u32			id	= 0;
		while		(fs->find_chunk(id))	
		{
			CEnvModifier		E;
			E.load				(fs);
			Modifiers.push_back	(E);
			id					++;
		}
		FS.r_close	(fs);
	}
}
void	CEnvironment::mods_unload		()
{
	Modifiers.clear_and_free			();
}
//////////////////////////////////////////////////////////////////////////
// environment
static const float day_tm	= 86400.f;
CEnvironment::CEnvironment	()
{
	Current[0]				= 0;
	Current[1]				= 0;
    CurrentWeather			= 0;
    CurrentWeatherName		= 0;
	eff_Rain				= 0;
    eff_LensFlare 			= 0;
    eff_Thunderbolt			= 0;
	OnDeviceCreate			();
#ifdef _EDITOR
	ed_from_time			= 0.f;
	ed_to_time				= day_tm;
#endif
	fGameTime				= 0.f;
    fTimeFactor				= 12.f;

	wind_strength_factor	= 0.f;

	// fill clouds hemi verts & faces 
	const Fvector* verts;
	CloudsVerts.resize		(xrHemisphereVertices(2,verts));
	Memory.mem_copy			(&CloudsVerts.front(),verts,CloudsVerts.size()*sizeof(Fvector));
	const u16* indices;
	CloudsIndices.resize	(xrHemisphereIndices(2,indices));
	Memory.mem_copy			(&CloudsIndices.front(),indices,CloudsIndices.size()*sizeof(u16));

	// perlin noise
	PerlinNoise1D			= xr_new<CPerlinNoise1D>(Random.randI(0,0xFFFF));
	PerlinNoise1D->SetOctaves(2);
	PerlinNoise1D->SetAmplitude(2.5f);
}
CEnvironment::~CEnvironment	()
{
	xr_delete				(PerlinNoise1D);
	OnDeviceDestroy			();
}

void CEnvironment::Invalidate()
{
	Current[0]				= 0;
	Current[1]				= 0;
	if (eff_LensFlare)		eff_LensFlare->Invalidate();
}

CEnvAmbient* CEnvironment::AppendEnvAmb		(const shared_str& sect)
{
	for (EnvAmbVecIt it=Ambients.begin(); it!=Ambients.end(); it++)
		if ((*it)->name().equal(sect)) return *it;
	Ambients.push_back		(xr_new<CEnvAmbient>());
	Ambients.back()->load	(sect);
	return Ambients.back();
}

IC bool sort_env_pred		(const CEnvDescriptor* x, const CEnvDescriptor* y)
{	return x->exec_time < y->exec_time;	}
void CEnvironment::load		()
{
	tonemap					= Device.Resources->_CreateTexture("$user$tonemap");	//. hack
    if (!eff_Rain)    		eff_Rain 		= xr_new<CEffect_Rain>();
    if (!eff_LensFlare)		eff_LensFlare 	= xr_new<CLensFlare>();
    if (!eff_Thunderbolt)	eff_Thunderbolt	= xr_new<CEffect_Thunderbolt>();
	if (Weathers.empty()){
    	LPCSTR first_weather=0;
    	int weather_count	= pSettings->line_count("weathers");
        for (int w_idx=0; w_idx<weather_count; w_idx++){
        	LPCSTR weather, sect_w;
			if (pSettings->r_line("weathers",w_idx,&weather,&sect_w)){
            	if (0==first_weather) first_weather=weather;
                int env_count	= pSettings->line_count(sect_w);
	        	LPCSTR exec_tm, sect_e;
                for (int env_idx=0; env_idx<env_count; env_idx++){
					if (pSettings->r_line(sect_w,env_idx,&exec_tm,&sect_e)){
                        CEnvDescriptor*		D=xr_new<CEnvDescriptor>();
                        D->load				(exec_tm,sect_e,this);
                        Weathers[weather].push_back	(D);
                	}
                }
            }
        }
        // sorting weather envs
        WeatherPairIt _I=Weathers.begin();
        WeatherPairIt _E=Weathers.end();
		for (; _I!=_E; _I++){
        	R_ASSERT3	(_I->second.size()>1,"Environment in weather must >=2",*_I->first);
        	std::sort(_I->second.begin(),_I->second.end(),sort_env_pred);
        }
        R_ASSERT2	(!Weathers.empty(),"Empty weathers.");
		SetWeather	(first_weather);
    }

	// music
	if (pSettings->section_exist("music")){
		CInifile::Sect&		S	= pSettings->r_section	("music");
		CInifile::SectIt	it	= S.begin(), end = S.end();
		for (;it!=end; it++)
		{
			string256	_n, _l, _r;
			strcpy		(_n,*it->first);
			strconcat	(_l,_n,"_l");
			strconcat	(_r,_n,"_r");
			music*		m = xr_new<music>	();
#ifdef DEBUG
			m->name				= _n;
#endif
			m->left.create		(true,_l,0);
			m->right.create		(true,_r,0);
			playlist.push_back	(m);
		}
	}
}

void CEnvironment::unload	()
{
    WeatherPairIt _I=Weathers.begin();
    WeatherPairIt _E=Weathers.end();
    for (; _I!=_E; _I++){
    	for (EnvIt it=_I->second.begin(); it!=_I->second.end(); it++)
        	xr_delete	(*it);
    }
	Weathers.clear		();
	for (EnvAmbVecIt it=Ambients.begin(); it!=Ambients.end(); it++)
		xr_delete		(*it);
	Ambients.clear		();
    xr_delete			(eff_Rain);
    xr_delete			(eff_LensFlare);
    xr_delete			(eff_Thunderbolt);
    CurrentWeather		= 0;
    CurrentWeatherName	= 0;
    CurrentEnv.unload	();
	Invalidate			();
	tonemap				= 0;

	// music
	for (u32 m=0; m<playlist.size(); m++)
	{
		music*	M		= playlist	[m];
		M->left.stop	();
		M->right.stop	();
		xr_delete		(M);
	}
	playlist.clear	();
}

void CEnvironment::SetWeather(shared_str name, bool forced)
{
	if (name.size())	{
        WeatherPairIt it	= Weathers.find(name);
        R_ASSERT3			(it!=Weathers.end(),"Invalid weather name.",*name);
        CurrentWeather		= &it->second;
        CurrentWeatherName	= it->first;
		if (forced)			{Invalidate();}
    }else{
#ifndef _EDITOR
		Debug.fatal			("Empty weather name");
#endif
        unload				();
        load				();
    }
}

IC bool lb_env_pred(const CEnvDescriptor* x, float val)
{	return x->exec_time < val;	}

void CEnvironment::SelectEnvs(float gt)
{
	VERIFY				(CurrentWeather);
    if ((Current[0]==Current[1])&&(Current[0]==0)){
	    bTerminator		= false;
        EnvIt env		= std::lower_bound(CurrentWeather->begin(),CurrentWeather->end(),gt,lb_env_pred);
        if (env==CurrentWeather->end()){
            Current[0]	= *(CurrentWeather->end()-1);
            Current[1]	= CurrentWeather->front();
            bTerminator	= true;
        }else{
            Current[1]	= *env;
            if (env==CurrentWeather->begin()){
		        Current[0]= *(CurrentWeather->end()-1);
	            bTerminator	= true;
            }else{
		        Current[0]= *(env-1);
            }
        }
    }else{
    	if (bTerminator){
            if ((gt>Current[1]->exec_time)&&(gt<Current[0]->exec_time)){
                bTerminator		= false;
                Current[0]		= Current[1];
                EnvIt env		= std::lower_bound(CurrentWeather->begin(),CurrentWeather->end(),gt,lb_env_pred);
                if (env==CurrentWeather->end()){
                    Current[1]	= CurrentWeather->front();
                    bTerminator	= true;
                }else{
                    Current[1]	= *env;
                }
            }
        }else{
            if (gt>Current[1]->exec_time){
			    bTerminator		= false;       
                Current[0]		= Current[1];
                EnvIt env		= std::lower_bound(CurrentWeather->begin(),CurrentWeather->end(),gt,lb_env_pred);
                if (env==CurrentWeather->end()){
                    Current[1]	= CurrentWeather->front();
                    bTerminator	= true;
                }else{
                    Current[1]	= *env;
                }
            }
        }
    }
}

void CEnvironment::OnFrame()
{
#ifdef _EDITOR
	fGameTime				+= Device.fTimeDelta*fTimeFactor;
    if (fsimilar(ed_to_time,day_tm)&&fsimilar(ed_from_time,0.f)){
	    if (fGameTime>day_tm)	fGameTime-=day_tm;
    }else{
	    if (fGameTime>ed_to_time){	
        	fGameTime=fGameTime-ed_to_time+ed_from_time;
            Current[0]=Current[1]=0;
        }
    	if (fGameTime<ed_from_time){	
        	fGameTime=ed_from_time;
            Current[0]=Current[1]=0;
        }
    }
	if (!psDeviceFlags.is(rsEnvironment))		return;
#else
	if (!g_pGameLevel)		return;
#endif
	SelectEnvs				(fGameTime);
    VERIFY					(Current[0]&&Current[1]);

	float current_weight;
    if (bTerminator){
	    float x				= fGameTime>Current[0]->exec_time?fGameTime-Current[0]->exec_time:(day_tm-Current[0]->exec_time)+fGameTime;
	    current_weight		= x/((day_tm-Current[0]->exec_time)+Current[1]->exec_time); 
    }else{
	    current_weight		= (fGameTime-Current[0]->exec_time)/(Current[1]->exec_time-Current[0]->exec_time); 
    }
    clamp					(current_weight,0.f,1.f);

	// modifiers
	CEnvModifier			EM;
	EM.far_plane			= 0;
	EM.fog_color.set		( 0,0,0 );
	EM.fog_density			= 0;
	EM.ambient.set			( 0,0,0 );
	EM.lmap_color.set		( 0,0,0 );
	Fvector	view			= Device.vCameraPosition;
	float	mpower			= 0;
	for (xr_vector<CEnvModifier>::iterator mit=Modifiers.begin(); mit!=Modifiers.end(); mit++)
		mpower += EM.sum(*mit,view);

	// final lerp
	CurrentEnv.lerp							(this,*Current[0],*Current[1],current_weight,EM,mpower);
	if (::Render->get_generation()==IRender_interface::GENERATION_R2)
	{
		//. very very ugly hack
		if (HW.Caps.raster_major >= 3)		{
			// tonemapping in VS
			CurrentEnv.sky_r_textures.push_back		(mk_pair(u32(D3DVERTEXTEXTURESAMPLER0),tonemap));	//. hack
			CurrentEnv.sky_r_textures_env.push_back	(mk_pair(u32(D3DVERTEXTEXTURESAMPLER0),tonemap));	//. hack
			CurrentEnv.clouds_r_textures.push_back	(mk_pair(u32(D3DVERTEXTEXTURESAMPLER0),tonemap));	//. hack
		} else {
			// tonemapping in PS
			CurrentEnv.sky_r_textures.push_back		(mk_pair(2,tonemap));								//. hack
			CurrentEnv.sky_r_textures_env.push_back	(mk_pair(2,tonemap));								//. hack
			CurrentEnv.clouds_r_textures.push_back	(mk_pair(2,tonemap));								//. hack
		}
	}
	wind_strength_factor				= 0.f;
	if (!fis_zero(CurrentEnv.gust_factor,EPS_L)){
		PerlinNoise1D->SetFrequency		(CurrentEnv.gust_factor*MAX_NOISE_FREQ);
		float gust						= clampr(PerlinNoise1D->Get(Device.fTimeGlobal)+0.5f,0.f,1.f); 
		float gust_weight				= 0.25f;
		CurrentEnv.wind_velocity		= CurrentEnv.wind_velocity*gust_weight+CurrentEnv.wind_velocity*(1.f-gust_weight)*gust;
		wind_strength_factor			= gust/10.f;
	}

    int id								=	(current_weight<0.5f)?Current[0]->lens_flare_id:Current[1]->lens_flare_id;
	eff_LensFlare->OnFrame				(id);
    BOOL tb_enabled						=	(current_weight<0.5f)?Current[0]->thunderbolt:Current[1]->thunderbolt;
    eff_Thunderbolt->OnFrame			(tb_enabled,CurrentEnv.bolt_period,CurrentEnv.bolt_duration);

	// ******************** Environment params (setting)
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGCOLOR,	color_rgba_f(CurrentEnv.fog_color.x,CurrentEnv.fog_color.y,CurrentEnv.fog_color.z,0) )); 
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGSTART,	*(u32 *)(&CurrentEnv.fog_near)	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGEND,	*(u32 *)(&CurrentEnv.fog_far)	));

	// ******************** Music
	if (!playlist.empty()){
		if (!playlist.front()->playing())	{
			// shedule next item
			music*	_old		= playlist.front();
			playlist.pop_front	();
			playlist.push_back	(_old);
			playlist.front()->left.play_at_pos	(0,Device.vCameraPosition);	playlist.front()->left.set_priority(1000.f);
			playlist.front()->right.play_at_pos	(0,Device.vCameraPosition);	playlist.front()->right.set_priority(1000.f);
#ifdef	DEBUG
			Msg					("* playing music track: %s",playlist.front()->name.c_str());
#endif
		}

		// update
		CSound_params			spL,spR;
		spL.freq				= 1.f;
		spL.min_distance		= 10.f;
		spL.max_distance		= 100.f;
		spL.volume				= psSoundVMusic;
		spR						= spL;
		spL.position.mad(Device.vCameraPosition,Device.vCameraDirection,.3f).mad(Device.vCameraRight,-.5f).mad(Device.vCameraTop,.1f);
		spR.position.mad(Device.vCameraPosition,Device.vCameraDirection,.3f).mad(Device.vCameraRight,+.5f).mad(Device.vCameraTop,.1f);

		music*	_m				= playlist.front	();
		_m->left.set_params		(&spL);
		_m->right.set_params	(&spR);
	}
}

extern float psHUD_FOV;
void CEnvironment::RenderSky		()
{
	::Render->rmFar				();

	// draw sky box
	Fmatrix						mSky;
	mSky.rotateY				(CurrentEnv.sky_rotation);
	mSky.translate_over			(Device.vCameraPosition);

	u32		i_offset,v_offset;
	u32		C					= color_rgba(iFloor(CurrentEnv.sky_color.x*255.f), iFloor(CurrentEnv.sky_color.y*255.f), iFloor(CurrentEnv.sky_color.z*255.f), iFloor(CurrentEnv.weight*255.f));

	// Fill index buffer
	u16*	pib					= RCache.Index.Lock	(20*3,i_offset);
	CopyMemory					(pib,hbox_faces,20*3*2);
	RCache.Index.Unlock			(20*3);

	// Fill vertex buffer
	v_skybox* pv				= (v_skybox*)	RCache.Vertex.Lock	(12,sh_2geom.stride(),v_offset);
	for (u32 v=0; v<12; v++)	pv[v].set		(hbox_verts[v*2],C,hbox_verts[v*2+1]);
	RCache.Vertex.Unlock		(12,sh_2geom.stride());

	// Render
	RCache.set_xform_world		(mSky);
	RCache.set_Geometry			(sh_2geom);
	RCache.set_Shader			(sh_2sky);
	RCache.set_Textures			(&CurrentEnv.sky_r_textures);
	RCache.Render				(D3DPT_TRIANGLELIST,v_offset,0,12,i_offset,20);

	// Sun
	::Render->rmNormal			();
	eff_LensFlare->Render		(TRUE,FALSE,FALSE);
}

void CEnvironment::RenderClouds			()
{
	// draw clouds
	if (fis_zero(CurrentEnv.clouds_color.w,EPS_L))	return;

	::Render->rmFar				();

	Fmatrix						mXFORM, mScale;
	mScale.scale				(10,0.4f,10);
	mXFORM.rotateY				(CurrentEnv.sky_rotation);
	mXFORM.mulB_43				(mScale);
	mXFORM.translate_over		(Device.vCameraPosition);

	Fvector wd0,wd1;
	Fvector4 wind_dir;
	wd0.setHP					(CurrentEnv.wind_direction,0);
	wd1.setHP					(CurrentEnv.wind_direction+PI_DIV_8,0);
	wind_dir.set				(wd0.x,wd0.z,wd1.x,wd1.z).mul(0.5f).add(0.5f).mul(255.f);
	u32		i_offset,v_offset;
	u32		C0					= color_rgba(iFloor(wind_dir.x),iFloor(wind_dir.y),iFloor(wind_dir.w),iFloor(wind_dir.z));
	u32		C1					= color_rgba(iFloor(CurrentEnv.clouds_color.x*255.f),iFloor(CurrentEnv.clouds_color.y*255.f),iFloor(CurrentEnv.clouds_color.z*255.f),iFloor(CurrentEnv.clouds_color.w*255.f));

	// Fill index buffer
	u16*	pib					= RCache.Index.Lock	(CloudsIndices.size(),i_offset);
	Memory.mem_copy				(pib,&CloudsIndices.front(),CloudsIndices.size()*sizeof(u16));
	RCache.Index.Unlock			(CloudsIndices.size());

	// Fill vertex buffer
	v_clouds* pv				= (v_clouds*)	RCache.Vertex.Lock	(CloudsVerts.size(),clouds_geom.stride(),v_offset);
	for (FvectorIt it=CloudsVerts.begin(); it!=CloudsVerts.end(); it++,pv++)
		pv->set					(*it,C0,C1);
	RCache.Vertex.Unlock		(CloudsVerts.size(),clouds_geom.stride());

	// Render
	RCache.set_xform_world		(mXFORM);
	RCache.set_Geometry			(clouds_geom);
	RCache.set_Shader			(clouds_sh);
	RCache.set_Textures			(&CurrentEnv.clouds_r_textures);
	RCache.Render				(D3DPT_TRIANGLELIST,v_offset,0,CloudsVerts.size(),i_offset,CloudsIndices.size()/3);
	
	::Render->rmNormal			();
}

void CEnvironment::RenderFlares		()
{
	// 1
	eff_LensFlare->Render			(FALSE,TRUE,TRUE);
}

void CEnvironment::RenderLast		()
{
	// 2
	eff_Rain->Render				();
    eff_Thunderbolt->Render			();
}

void CEnvironment::OnDeviceCreate()
{
	sh_2sky.create			(&b_skybox,"skybox_2t");
	sh_2geom.create			(v_skybox_fvf,RCache.Vertex.Buffer(), RCache.Index.Buffer());
	clouds_sh.create		("clouds","null");
	clouds_geom.create		(v_clouds_fvf,RCache.Vertex.Buffer(), RCache.Index.Buffer());
    load					();
}
void CEnvironment::OnDeviceDestroy()
{
	unload					();
	sh_2sky.destroy			();
	sh_2geom.destroy		();
	clouds_sh.destroy		();
	clouds_geom.destroy		();
}

#ifdef _EDITOR
void CEnvironment::ED_Reload()
{
	OnDeviceDestroy			();
	OnDeviceCreate			();
}
#endif
