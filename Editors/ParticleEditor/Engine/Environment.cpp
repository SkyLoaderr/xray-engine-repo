#include "stdafx.h"
#pragma hdrstop

#include "resourcemanager.h"
#include "blenders\blender.h"
#include "render.h"

#include "Environment.h"
#include "xr_efflensflare.h"
#include "xr_trims.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ENGINE_API float	psGravity	= 20.f;

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
		C.r_Pass			("null",			"r1_sky2",		FALSE,	TRUE, FALSE);
		C.r_Sampler			("s_sky0",			"$null",		D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r_Sampler			("s_sky1",			"$null",		D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
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
static	u16			hbox_faces[18*3]	=
{
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
void CEnvDescriptor::load	(LPCSTR exec_tm, LPCSTR S, CEnvironment* parent)
{
	Ivector3 tm				={0,0,0};
	sscanf					(exec_tm,"%d:%d:%d",&tm.x,&tm.y,&tm.z);
    R_ASSERT3				((tm.x>=0)&&(tm.x<24)&&(tm.y>=0)&&(tm.y<60)&&(tm.z>=0)&&(tm.z<60),"Incorrect weather time",S);
	exec_time				= tm.x*3600+tm.y*60+tm.z;
	sky_texture				= Device.Resources->_CreateTexture(pSettings->r_string(S,"sky_texture"));
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
	sky_r_textures.clear	();
	sky_r_textures.push_back(0);
	sky_r_textures.push_back(0);
}
void CEnvDescriptor::lerp	(CEnvDescriptor& A, CEnvDescriptor& B, float f)
{
	float	fi				= 1-f;
	sky_r_textures.clear	();
	sky_r_textures.push_back(A.sky_texture);
	sky_r_textures.push_back(B.sky_texture);
	sky_factor				= f;
	sky_color.lerp			(A.sky_color,B.sky_color,f);
	far_plane				= fi*A.far_plane + f*B.far_plane;
	fog_color.lerp			(A.fog_color,B.fog_color,f);
	fog_density				= fi*A.fog_density + f*B.fog_density;
	fog_near				= (1.0f - fog_density)*0.85f * far_plane;
	fog_far					= 0.95f * far_plane;
	rain_density			= fi*A.rain_density + f*B.rain_density;
	rain_color.lerp			(A.rain_color,B.rain_color,f);
	bolt_period				= fi*A.bolt_period + f*B.bolt_period;
	bolt_duration			= fi*A.bolt_duration + f*B.bolt_duration;
    wind_velocity			= fi*A.wind_velocity + f*B.wind_velocity;
    wind_direction			= fi*A.wind_direction + f*B.wind_direction;
	ambient.lerp			(A.ambient,B.ambient,f);
	lmap_color.lerp			(A.lmap_color,B.lmap_color,f);
	hemi_color.lerp			(A.hemi_color,B.hemi_color,f);
	sun_color.lerp			(A.sun_color,B.sun_color,f);
	sun_dir.lerp			(A.sun_dir,B.sun_dir,f).normalize();
}

//////////////////////////////////////////////////////////////////////////
// environment
CEnvironment::CEnvironment	()
{
	CurrentA				= 0;
	CurrentB				= 0;
	ABcurrent				= 0.f;
    ABlength				= 0.f;
    ABspeed					= 1.f;
    CurrentWeather			= 0;
    CurrentWeatherName		= 0;
	eff_Rain				= 0;
    eff_LensFlare 			= 0;
    eff_Thunderbolt			= 0;
	OnDeviceCreate			();
}
CEnvironment::~CEnvironment	()
{
	OnDeviceDestroy			();
}

IC bool sort_env_pred(const CEnvDescriptor*& x, const CEnvDescriptor*& y)
{	return x->exec_time < y->exec_time;	}

void CEnvironment::load		()
{
    if (!eff_Rain)			eff_Rain 		= xr_new<CEffect_Rain>();
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
    ABcurrent			= 0;
    ABlength			= 0;
}

void CEnvironment::SetWeather(LPCSTR name)
{
	R_ASSERT2			(name&&name[0],"Empty weather name");
	WeatherPairIt it	= Weathers.find(name);
    R_ASSERT3			(it!=Weathers.end(),"Invalid weather name.",name);
	CurrentWeather		= &it->second;
    CurrentWeatherName	= *it->first;
}

IC bool lb_env_pred(const CEnvDescriptor*& x, float val)
{	return x->exec_time < val;	}

void CEnvironment::SelectEnv()
{
	VERIFY				(CurrentWeather);
    if ((CurrentA==CurrentB)&&(CurrentA==0)){
        CurrentA		= *CurrentWeather->begin();
        CurrentB		= *(CurrentWeather->begin()+1);
        ABlength		= CurrentB->exec_time-CurrentA->exec_time;
        ABcurrent		= 0.f;
    }else{
        CurrentA		= CurrentB;
        EnvIt env		= std::lower_bound(CurrentWeather->begin(),CurrentWeather->end(),CurrentB->exec_time+1.f,lb_env_pred);
        if (env==CurrentWeather->end()){
            CurrentB	= CurrentWeather->front();
            ABlength	= (24*60*60-CurrentA->exec_time)+CurrentB->exec_time;
        }else{
            CurrentB	= *env;
            ABlength	= CurrentB->exec_time-CurrentA->exec_time;
        }
        ABcurrent		= 0.f;
    }
}

void CEnvironment::OnFrame()
{
#ifdef _EDITOR
	ABcurrent			+= Device.fTimeDelta*ABspeed;
#else
	ABcurrent			+= Device.fTimeDelta*Level().GetGameTimeFactor();
#endif
    if (ABcurrent>ABlength)	SelectEnv();

    VERIFY(CurrentA&&CurrentB);
    float t_fact			= ABcurrent/ABlength; VERIFY(t_fact<1.f);
	CurrentEnv.lerp			(*CurrentA,*CurrentB,t_fact);
    int id					= (t_fact<0.5f)?CurrentA->lens_flare_id:CurrentB->lens_flare_id;
	eff_LensFlare->OnFrame	(id);
    BOOL tb_enabled			= (t_fact<0.5f)?CurrentA->thunderbolt:CurrentB->thunderbolt;
    eff_Thunderbolt->OnFrame(tb_enabled,CurrentEnv.bolt_period,CurrentEnv.bolt_duration);

	// ******************** Environment params (setting)
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGCOLOR,	color_rgba_f(CurrentEnv.fog_color.x,CurrentEnv.fog_color.y,CurrentEnv.fog_color.z,0) )); 
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGSTART,	*(u32 *)(&CurrentEnv.fog_near)	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGEND,	*(u32 *)(&CurrentEnv.fog_far)	));
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
		u16*	pib					= RCache.Index.Lock	(18*3,i_offset);
		CopyMemory					(pib,hbox_faces,18*3*2);
		RCache.Index.Unlock			(18*3);

		// Fill vertex buffer
		v_skybox* pv				= (v_skybox*)	RCache.Vertex.Lock	(12,sh_2geom.stride(),v_offset);
		for (u32 v=0; v<12; v++)
			pv[v].set				(hbox_verts[v*2],C,hbox_verts[v*2+1]);
		RCache.Vertex.Unlock		(12,sh_2geom.stride());

		// Render
		RCache.set_Geometry			(sh_2geom);
		RCache.set_Shader			(sh_2sky);
		RCache.set_Textures			(&CurrentEnv.sky_r_textures);
		RCache.Render				(D3DPT_TRIANGLELIST,v_offset,0,12,i_offset,18);

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
//    eff_LensFlare->OnDeviceCreate();
}

void CEnvironment::OnDeviceDestroy()
{
	unload					();
	sh_2sky.destroy			();
	sh_2geom.destroy		();
}

void CEnvironment::ED_Reload()
{
	OnDeviceDestroy			();
	OnDeviceCreate			();
}


