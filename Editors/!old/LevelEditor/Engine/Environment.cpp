#include "stdafx.h"
#pragma hdrstop

#include "resourcemanager.h"
#include "blenders\blender.h"
#include "render.h"

#include "Environment.h"
#include "xr_efflensflare.h"
#include "rain.h"
#include "thunderbolt.h"

#include "xr_trims.h"

#ifndef _EDITOR
	#include "IGame_Level.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ENGINE_API	float			psVisDistance	= 1.f;

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

//////////////////////////////////////////////////////////////////////////
// environment desc
CEnvDescriptor::CEnvDescriptor()
{
}

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
	sky_color				= pSettings->r_fvector3	(S,"sky_color");		sky_color.mul(.5f);
	far_plane				= pSettings->r_float	(S,"far_plane");
	fog_color				= pSettings->r_fvector3	(S,"fog_color");
	fog_density				= pSettings->r_float	(S,"fog_density");
	rain_density			= pSettings->r_float	(S,"rain_density");
	rain_color				= pSettings->r_fvector3	(S,"rain_color");            
    wind_velocity			= pSettings->r_float	(S,"wind_velocity");
    wind_direction			= deg2rad(pSettings->r_float(S,"wind_direction"));
	ambient					= pSettings->r_fvector3	(S,"ambient");
	lmap_color				= pSettings->r_fvector3	(S,"lmap_color");
	hemi_color				= pSettings->r_fvector3	(S,"hemi_color");
	sun_color				= pSettings->r_fvector3	(S,"sun_color");
	Fvector2 sund			= pSettings->r_fvector2	(S,"sun_dir");	sun_dir.setHP	(deg2rad(sund.y),deg2rad(sund.x));
    lens_flare_id			= parent->eff_LensFlare->AppendDef(pSettings,pSettings->r_string(S,"flares"));
    thunderbolt				= pSettings->r_bool		(S,"thunderbolt");
	bolt_period				= thunderbolt?pSettings->r_float	(S,"bolt_period"):0.f;
	bolt_duration			= thunderbolt?pSettings->r_float	(S,"bolt_duration"):0.f;
}
void CEnvDescriptor::unload	()
{
	sky_r_textures.clear		();
	sky_r_textures.push_back	(0);
	sky_r_textures.push_back	(0);
	sky_r_textures.push_back	(0);

	sky_r_textures_env.clear	();
	sky_r_textures_env.push_back(0);
	sky_r_textures_env.push_back(0);
	sky_r_textures_env.push_back(0);
}
void CEnvDescriptor::lerp	(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power)
{
	float	_power			=	1.f/(m_power+1);	// the environment itself
	float	fi				=	1-f;

	sky_r_textures.clear	();
	sky_r_textures.push_back(A.sky_texture);
	sky_r_textures.push_back(B.sky_texture);

	sky_r_textures_env.clear	();
	sky_r_textures_env.push_back(A.sky_texture_env);
	sky_r_textures_env.push_back(B.sky_texture_env);

	sky_factor				= f;
	sky_color.lerp			(A.sky_color,B.sky_color,f);
	far_plane				= (fi*A.far_plane + f*B.far_plane + M.far_plane)*psVisDistance*_power;
	fog_color.lerp			(A.fog_color,B.fog_color,f).add(M.fog_color).mul(_power);
	fog_density				= (fi*A.fog_density + f*B.fog_density + M.fog_density)*_power;
	fog_near				= (1.0f - fog_density)*0.85f * far_plane;
	fog_far					= 0.95f * far_plane;
	rain_density			= fi*A.rain_density + f*B.rain_density;
	rain_color.lerp			(A.rain_color,B.rain_color,f);
	bolt_period				= fi*A.bolt_period + f*B.bolt_period;
	bolt_duration			= fi*A.bolt_duration + f*B.bolt_duration;
	// wind
    wind_velocity			= fi*A.wind_velocity + f*B.wind_velocity;
    wind_direction			= fi*A.wind_direction + f*B.wind_direction;
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
	CurrentA				= 0;
	CurrentB				= 0;
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

	wind_strength			= 0.f;
}
CEnvironment::~CEnvironment	()
{
	OnDeviceDestroy			();
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
	{
		CInifile::Sect&		S	= pSettings->r_section	("music");
		CInifile::SectIt	it	= S.begin(), end = S.end();
		for (;it!=end; it++)
		{
			string256	_n, _l, _r;
			strcpy		(_n,*it->first);
			strconcat	(_l,_n,"_l");
			strconcat	(_r,_n,"_r");
			music*		m = xr_new<music>	();
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
    for (; _I!=_E; _I++)
    	for (EnvIt it=_I->second.begin(); it!=_I->second.end(); it++)
        	xr_delete	(*it);
	Weathers.clear		();
    xr_delete			(eff_Rain);
    xr_delete			(eff_LensFlare);
    xr_delete			(eff_Thunderbolt);
    CurrentWeather		= 0;
    CurrentWeatherName	= 0;
    CurrentEnv.unload	();
    CurrentA			= 0;
    CurrentB			= 0;
	tonemap				= 0;

	// music
	for (int m=0; m<playlist.size(); m++)
	{
		music*	M		= playlist	[m];
		M->left.stop	();
		M->right.stop	();
		xr_delete		(M);
	}
	playlist.clear	();
}

void CEnvironment::SetWeather(shared_str name)
{
	if (*name && xr_strlen(name))	{
        WeatherPairIt it	= Weathers.find(name);
        R_ASSERT3			(it!=Weathers.end(),"Invalid weather name.",*name);
        CurrentWeather		= &it->second;
        CurrentWeatherName	= it->first;
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
    if ((CurrentA==CurrentB)&&(CurrentA==0)){
	    bTerminator		= false;
        EnvIt env		= std::lower_bound(CurrentWeather->begin(),CurrentWeather->end(),gt,lb_env_pred);
        if (env==CurrentWeather->end()){
            CurrentA	= *(CurrentWeather->end()-1);
            CurrentB	= CurrentWeather->front();
            bTerminator	= true;
        }else{
            CurrentB	= *env;
            if (env==CurrentWeather->begin()){
		        CurrentA= *(CurrentWeather->end()-1);
	            bTerminator	= true;
            }else{
		        CurrentA= *(env-1);
            }
        }
    }else{
    	if (bTerminator){
            if ((gt>CurrentB->exec_time)&&(gt<CurrentA->exec_time)){
                bTerminator		= false;
                CurrentA		= CurrentB;
                EnvIt env		= std::lower_bound(CurrentWeather->begin(),CurrentWeather->end(),gt,lb_env_pred);
                if (env==CurrentWeather->end()){
                    CurrentB	= CurrentWeather->front();
                    bTerminator	= true;
                }else{
                    CurrentB	= *env;
                }
            }
        }else{
            if (gt>CurrentB->exec_time){
			    bTerminator		= false;       
                CurrentA		= CurrentB;
                EnvIt env		= std::lower_bound(CurrentWeather->begin(),CurrentWeather->end(),gt,lb_env_pred);
                if (env==CurrentWeather->end()){
                    CurrentB	= CurrentWeather->front();
                    bTerminator	= true;
                }else{
                    CurrentB	= *env;
                }
            }
        }
    }
}

void CEnvironment::OnFrame()
{
#ifdef _EDITOR
	if (!psDeviceFlags.is(rsEnvironment))		return;
	fGameTime				+= Device.fTimeDelta*fTimeFactor;
    if (fsimilar(ed_to_time,day_tm)&&fsimilar(ed_from_time,0.f)){
	    if (fGameTime>day_tm)	fGameTime-=day_tm;
    }else{
	    if (fGameTime>ed_to_time){	
        	fGameTime=fGameTime-ed_to_time+ed_from_time;
            CurrentA=CurrentB=0;
        }
    	if (fGameTime<ed_from_time){	
        	fGameTime=ed_from_time;
            CurrentA=CurrentB=0;
        }
    }
#else
	if (!g_pGameLevel)		return;
#endif
	SelectEnvs				(fGameTime);
    VERIFY					(CurrentA&&CurrentB);

    float t_fact;
    if (bTerminator){
	    float x				= fGameTime>CurrentA->exec_time?fGameTime-CurrentA->exec_time:(day_tm-CurrentA->exec_time)+fGameTime;
	    t_fact				= x/((day_tm-CurrentA->exec_time)+CurrentB->exec_time); 
    }else{
	    t_fact				= (fGameTime-CurrentA->exec_time)/(CurrentB->exec_time-CurrentA->exec_time); 
    }
    clamp					(t_fact,0.f,1.f);

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
	CurrentEnv.lerp						(this,*CurrentA,*CurrentB,t_fact,EM,mpower);
	CurrentEnv.sky_r_textures.push_back		(tonemap);		//. hack
	CurrentEnv.sky_r_textures_env.push_back	(tonemap);		//. hack
    int id								= (t_fact<0.5f)?CurrentA->lens_flare_id:CurrentB->lens_flare_id;
	eff_LensFlare->OnFrame				(id);
    BOOL tb_enabled						= (t_fact<0.5f)?CurrentA->thunderbolt:CurrentB->thunderbolt;
    eff_Thunderbolt->OnFrame			(tb_enabled,CurrentEnv.bolt_period,CurrentEnv.bolt_duration);

	// ******************** Environment params (setting)
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGCOLOR,	color_rgba_f(CurrentEnv.fog_color.x,CurrentEnv.fog_color.y,CurrentEnv.fog_color.z,0) )); 
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGSTART,	*(u32 *)(&CurrentEnv.fog_near)	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGEND,	*(u32 *)(&CurrentEnv.fog_far)	));

	// ******************** Music
	if (!playlist.front()->playing())	{
		// shedule next item
		music*	_old		= playlist.front();
		playlist.pop_front	();
		playlist.push_back	(_old);
		playlist.front()->left.play_at_pos	(0,Device.vCameraPosition);	playlist.front()->left.set_priority(1000.f);
		playlist.front()->right.play_at_pos	(0,Device.vCameraPosition);	playlist.front()->right.set_priority(1000.f);
	}

	// update
	CSound_params		spL,spR;
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

extern float psHUD_FOV;
void CEnvironment::RenderFirst	()
{
	// Render skybox/plane
	{
		::Render->rmFar				();
		Fmatrix						mSky;
		mSky.translate				(Device.vCameraPosition);
		RCache.set_xform_world		(mSky);

		u32		i_offset,v_offset;
		Fcolor	clr					= { CurrentEnv.sky_color.x, CurrentEnv.sky_color.y, CurrentEnv.sky_color.z, CurrentEnv.sky_factor };
		u32		C					= clr.get	();

		// Fill index buffer
		u16*	pib					= RCache.Index.Lock	(20*3,i_offset);
		CopyMemory					(pib,hbox_faces,20*3*2);
		RCache.Index.Unlock			(20*3);

		// Fill vertex buffer
		v_skybox* pv				= (v_skybox*)	RCache.Vertex.Lock	(12,sh_2geom.stride(),v_offset);
		for (u32 v=0; v<12; v++)
			pv[v].set				(hbox_verts[v*2],C,hbox_verts[v*2+1]);
		RCache.Vertex.Unlock		(12,sh_2geom.stride());

		// Render
		RCache.set_Geometry			(sh_2geom);
		RCache.set_Shader			(sh_2sky);
		RCache.set_Textures			(&CurrentEnv.sky_r_textures);
		RCache.Render				(D3DPT_TRIANGLELIST,v_offset,0,12,i_offset,20);

		::Render->rmNormal			();
	}

    eff_LensFlare->Render			(TRUE,FALSE,FALSE);
}

void CEnvironment::RenderLast		()
{
    eff_LensFlare->Render			(FALSE,TRUE,TRUE);
	eff_Rain->Render				();
    eff_Thunderbolt->Render			();
}

void CEnvironment::OnDeviceCreate()
{
	sh_2sky.create			(&b_skybox,"skybox_2t");
	sh_2geom.create			(v_skybox_fvf,RCache.Vertex.Buffer(), RCache.Index.Buffer());
    load					();
}
void CEnvironment::OnDeviceDestroy()
{
	unload					();
	sh_2sky.destroy			();
	sh_2geom.destroy		();
}

#ifdef _EDITOR
void CEnvironment::ED_Reload()
{
	OnDeviceDestroy			();
	OnDeviceCreate			();
}
#endif
