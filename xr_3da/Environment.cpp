// Environment.cpp: implementation of the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"

#include "Environment.h"
#include "render.h"
#include "xr_effsun.h"
#include "xr_trims.h"
#include "xr_input.h"

#include "fmesh.h"
#include "fhierrarhyvisual.h"

#include "customhud.h"

// font
#include "x_ray.h"
#include "gamefont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ENGINE_API float	psGravity = 20.f;
ENGINE_API Flags32	psEnvFlags= {effSunGlare};

CEnvironment::CEnvironment	()
{
	pSkydome				= NULL;
}

CEnvironment::~CEnvironment	()
{
	for(u32 i=0; i<Suns.size(); i++) xr_delete(Suns[i]);

	Render->model_Delete	(pSkydome,TRUE);
}

void CEnvDescriptor::load	(LPCSTR S)
{
	sky_color				= pSettings->r_fvector3	(S,"sky_color");
	far_plane				= pSettings->r_float	(S,"far_plane");
	fog_color				= pSettings->r_fvector3	(S,"fog_color");
	fog_density				= pSettings->r_float	(S,"fog_density");
	ambient					= pSettings->r_fvector3	(S,"ambient");
	lmap_color				= pSettings->r_fvector3	(S,"lmap_color");
	hemi_color				= pSettings->r_fvector3	(S,"hemi_color");
	sun_color				= pSettings->r_fvector3	(S,"sun_color");
	Fvector2 sund			= pSettings->r_fvector2	(S,"sun_dir");	sun_dir.setHP	(deg2rad(sund.y),deg2rad(sund.x));
}
void CEnvDescriptor::lerp	(CEnvDescriptor& A, CEnvDescriptor& B, float f)
{
	float	fi	= 1-f;
	sky_color.lerp			(A.sky_color,B.sky_color,f);
	far_plane				= fi*A.far_plane + f*B.far_plane;
	fog_color.lerp			(A.fog_color,B.fog_color,f);
	fog_density				= fi*A.fog_density + f*B.fog_density;
	fog_near				= (1.0f - fog_density)*0.85f * far_plane;
	fog_far					= 0.95f * far_plane;
	ambient.lerp			(A.ambient,B.ambient,f);
	lmap_color.lerp			(A.lmap_color,B.lmap_color,f);
	hemi_color.lerp			(A.hemi_color,B.hemi_color,f);
	sun_color.lerp			(A.sun_color,B.sun_color,f);
	sun_dir.lerp			(A.sun_dir,B.sun_dir,f).normalize();
}

void CEnvironment::Load		(CInifile *pIni, char *section)
{
	for(int env=0; env<24; env++) 
	{
		LPCSTR	sect		= "environment";
		char	name		[32];
		sprintf				(name,"%d",env);
		if (!pSettings->line_exist	(sect,name))	continue;
		CEnvDescriptor		D;
		D.load				(pSettings->r_string(sect,name));
		Palette.push_back	(D);
	}
	
	if (pIni->line_exist(section,"suns"))
	{
		LPCSTR		S;
		string256	name;
		CSun*		pSun;
		S = pIni->r_string(section,"suns");
		u32 scnt = _GetItemCount(S);
		for (u32 i=0; i<scnt; i++){
			_GetItem(S,i,name);
			pSun	= xr_new<CSun> (pIni, name);
			Suns.push_back(pSun);
		}
		R_ASSERT2	(Suns.size(), "Should be at least one 'sun' source in level");
	}

	// environment objects
	if ((0==pSkydome) && (g_pGameLevel->pLevel->line_exist("environment","sky")))
	{
		LPCSTR S			= g_pGameLevel->pLevel->r_string("environment","sky");
		pSkydome			= Render->model_Create	(S);
	} else {
		pSkydome			= 0;
	}

	// update suns
	for(u32 i=0; i<Suns.size(); i++) Suns[i]->Update();
}

void CEnvironment::Load_Music(CInifile* INI)
{
	/*
	if (INI->section_exist("music")) 
	{
		for (int i=0; ; i++)
		{
			char buf[128];
			sprintf(buf,"track%d",i);

			if (!INI->line_exist("music",buf)) break;

			LPCSTR N		= INI->r_string("music",buf);
			CSoundStream* S = Sounds->CreateStream(N);
			R_ASSERT		(S);
			Music.push_back	(S);
		}
		if (INI->line_exist("music","track")) {
			LPCSTR N		= INI->r_string("music","track");
			CSoundStream* S = pSounds->CreateStream((char*)N);
			R_ASSERT		(S);
			S->Play			(true);
			xr_vector<CSoundStream*>::iterator F = find(Music.begin(),Music.end(),S);
			if (F!=Music.end()) Music_Active = F-Music.begin();
			else	{
				Music_Active = Music.size();
				Music.push_back(S);
			}
		}
	}
	*/
}

void CEnvironment::Music_Play(int id)
{
	/*
	if (Music.empty()) return;

	id %= Music.size();
	if (id != Music_Active) {
		Music[Music_Active]->Stop();
		Music_Fade			= Music_Active;
		Music_Active		= id;
		Music[id]->Play		(true);
		Music[id]->SetVolume(1);
	}
	*/
}

void CEnvironment::OnFrame()
{
	// ******************** Environment params (interpolation)
	float		t_pos	= Device.fTimeGlobal/10;
	float		t_ip;
	float		t_fact	= modff		(t_pos, &t_ip);
	int			f_1		= iFloor	(t_ip)	% Palette.size();
	int			f_2		= (f_1 + 1)			% Palette.size();
	CEnvDescriptor&	_A	= Palette	[f_1];
	CEnvDescriptor&	_B	= Palette	[f_2];
	Current.lerp		(_A,_B,t_fact);

	// ******************** Environment params (setting)
	Fcolor c_fog;	c_fog.set	(Current.fog_color.x,Current.fog_color.y,Current.fog_color.z,0);
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGCOLOR,	c_fog.get	( ) )); 
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGSTART,	*(u32 *)(&Current.fog_near)	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGEND,	*(u32 *)(&Current.fog_far)	));

	for(u32 i=0; i<Suns.size(); i++) Suns[i]->Update();
}

extern float psHUD_FOV;
void CEnvironment::RenderFirst()
{
	if (pSkydome) {
		::Render->rmFar				();

		Fmatrix						mSky;
		mSky.translate				(Device.vCameraPosition);
		RCache.set_xform_world		(mSky);

		switch (pSkydome->Type)
		{
		case MT_HIERRARHY:
			{
				FHierrarhyVisual* pV	= (FHierrarhyVisual*)pSkydome;
				xr_vector<IRender_Visual*>::iterator I,E;
				I = pV->children.begin	();
				E = pV->children.end	();
				for (; I!=E; I++)		
				{
					IRender_Visual* V				= *I;
					RCache.set_Shader		(V->hShader);
					V->Render				(1.f);
				}
			}
			break;
		default:
			RCache.set_Shader			(pSkydome->hShader);
			pSkydome->Render			(1.f);
			break;
		}

		::Render->rmNormal			();
	}

	// Sun sources
	/* *********************** interfere with R2
	if (psEnvFlags.test(effSunGlare))
		for(u32 i=0; i<Suns.size(); i++) Suns[i]->RenderSource();
	*/
}

void CEnvironment::RenderLast()
{
	if (psEnvFlags.test(effSunGlare))
		for(u32 i=0; i<Suns.size(); i++) Suns[i]->RenderFlares();
}
