// Environment.cpp: implementation of the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Environment.h"
#include "fstaticrender.h"
#include "xr_effsun.h"
#include "xr_trims.h"
#include "xr_input.h"
#include "xr_ini.h"
#include "xr_creator.h"
#include "xr_streamsnd.h"
#include "xr_sndman.h"
#include "PSObject.h"

#include "fmesh.h"
#include "fhierrarhyvisual.h"

#include "customhud.h"

// font
#include "x_ray.h"
#include "xr_smallfont.h"

const float music_fade = 1.5f;

void CEnvironment::Zoom(BOOL E) 
{
	if (E) {
		FOV_Dest	=10;
		MOUSE_Sens	=.1f;
	} else {
		FOV_Dest	=90;
		MOUSE_Sens	=1.f;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ENGINE_API float	psGravity = 30.f;

CEnvironment::CEnvironment()
{
	// environment data
	fDayLight				= 1.0f;
	fWindDir				= PI_DIV_2;
	fWindStrength			= 2;
	fFlash					= -1;

	// environment objects
	Device.seqDevCreate.Add	(this);
	Device.seqDevDestroy.Add(this);
	bUpdateBrightness		= false;
	
	FOV_Dest				= FOV_Current	= 90.f;
	MOUSE_Sens				= 1.f;
	Zoom					(false);

	// Music
	Music_Active			= -1;
	Music_Fade				= -1;

	c_Invalidate			();
}
CEnvironment::~CEnvironment()
{
	for(DWORD i=0; i<Suns.size(); i++) delete Suns[i];
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);
	if (Device.bReady) OnDeviceDestroy();
}

void CEnvironment::Load(CInifile *pIni, char *section)
{
	pm_bug			= FALSE;

	for(int env=0; env<32; env++) {
		char	name[32];
		sprintf(name,"env%d",env);
		if (!pIni->LineExists(section,name)) break;
		LPCSTR	E	= pIni->ReadSTRING(section,name);
		SEnvDef	D;
		sscanf	(E,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
			&D.Ambient.r,&D.Ambient.g,&D.Ambient.b,
			&D.Fog.r,	&D.Fog.g,	&D.Fog.b,
			&D.Fogness,	&D.Far,
			&D.Sky.r,	&D.Sky.g,	&D.Sky.b,	&D.Sky.a
			);
		Palette.push_back	(D);
	}
	
	if (pIni->LineExists(section,"suns"))
	{
		LPCSTR		S;
		FILE_NAME	name;
		CSun*		pSun;
		S = pIni->ReadSTRING(section,"suns");
		DWORD scnt = _GetItemCount(S);
		for (DWORD i=0; i<scnt; i++){
			_GetItem(S,i,name,"");
			pSun	= new CSun(pIni, name);
			Suns.push_back(pSun);
		}
	}
	if (pIni->LineExists(section,"pm_workaround"))
	{	pm_bug = TRUE; }

	c_Invalidate		();
	Current.Sky.set		(0,0,0,0);
	Current.Ambient.set	(0,0,0,0);
	Current.Fog.set		(0,0,0,0);
	Current.Fogness		= 1;
	Current.Far			= 99;
	CurrentID			= 0;
	CurrentSpeed		= 20;

	// update suns
	for(DWORD i=0; i<Suns.size(); i++) Suns[i]->Update();

	if (Device.bReady) OnDeviceCreate();
}

void CEnvironment::Load_Music(CInifile* INI)
{
	if (INI->SectionExists("music")) 
	{
		for (int i=0; ; i++)
		{
			char buf[128];
			sprintf(buf,"track%d",i);

			if (!INI->LineExists("music",buf)) break;

			LPCSTR N		= INI->ReadSTRING("music",buf);
			CSoundStream* S = pSounds->CreateStream(N);
			R_ASSERT		(S);
			Music.push_back	(S);
		}
		if (INI->LineExists("music","track")) {
			LPCSTR N		= INI->ReadSTRING("music","track");
			CSoundStream* S = pSounds->CreateStream((char*)N);
			R_ASSERT		(S);
			S->Play			(true);
			vector<CSoundStream*>::iterator F = find(Music.begin(),Music.end(),S);
			if (F!=Music.end()) Music_Active = F-Music.begin();
			else	{
				Music_Active = Music.size();
				Music.push_back(S);
			}
		}
	}
}

void CEnvironment::Music_Play(int id)
{
	if (Music.empty()) return;

	id %= Music.size();
	if (id != Music_Active) {
		Music[Music_Active]->Stop();
		Music_Fade			= Music_Active;
		Music_Active		= id;
		Music[id]->Play		(true);
		Music[id]->SetVolume(1);
	}
}

void CEnvironment::OnMove()
{
	// Suns
	for(DWORD i=0; i<Suns.size(); i++) Suns[i]->OnMove();

	// Flash and brightness
	if (fFlash>0) fFlash -= Device.fTimeDelta;
	if (fFlash>0) SetGradient(fFlash*3);

	if (bUpdateBrightness){
		Device.Gamma.Brightness(1.f+fBrightness);
		Device.Gamma.Update();
		bUpdateBrightness = false;
		fBrightness = 0;
	}

	// ******************** Viewport params
	float src = 10*Device.fTimeDelta;	clamp(src,0.f,1.f);
	float dst = 1-src;
	FOV_Current= FOV_Current*dst + FOV_Dest*src;
	psMouseSensScale = psMouseSensScale*dst + MOUSE_Sens*src;

	// ******************** Environment params (interpolation)
	float _s	= CurrentSpeed*Device.fTimeDelta;	clamp(_s,0.f,1.f);
	float _d	= 1-_s;
	SEnvDef&	Dest	= Palette[CurrentID%Palette.size()];
	Current.Sky.r		= Current.Sky.r*_d		+ Dest.Sky.r*_s;
	Current.Sky.g		= Current.Sky.g*_d		+ Dest.Sky.g*_s;
	Current.Sky.b		= Current.Sky.b*_d		+ Dest.Sky.b*_s;
	Current.Sky.a		= Current.Sky.a*_d		+ Dest.Sky.a*_s;
	Current.Ambient.r	= Current.Ambient.r*_d	+ Dest.Ambient.r*_s;
	Current.Ambient.g	= Current.Ambient.g*_d	+ Dest.Ambient.g*_s;
	Current.Ambient.b	= Current.Ambient.b*_d	+ Dest.Ambient.b*_s;
	Current.Fog.r		= Current.Fog.r*_d		+ Dest.Fog.r*_s;
	Current.Fog.g		= Current.Fog.g*_d		+ Dest.Fog.g*_s;
	Current.Fog.b		= Current.Fog.b*_d		+ Dest.Fog.b*_s;
	Current.Fogness		= Current.Fogness*_d	+ Dest.Fogness*_s;
	Current.Far			= Current.Far*_d		+ Dest.Far*_s;

	// ******************** Environment params (setting)
	DWORD	_amb		= Current.Ambient.get	();
	if (_amb!=c_Ambient){ CHK_DX(HW.pDevice->SetRenderState	( D3DRS_AMBIENT, _amb )); c_Ambient = _amb; }
	DWORD	_fog		= Current.Fog.get		();
	if (_fog!=c_Fog)	{ CHK_DX(HW.pDevice->SetRenderState	( D3DRS_FOGCOLOR, _fog )); c_Fog = _fog; }
	if (!fsimilar(c_Fogness,Current.Fogness) || !fsimilar(c_Far,Current.Far)) {
		c_Fogness	= Current.Fogness;
		c_Far		= Current.Far;

		// update fog params
		float start	= (1.0f - c_Fogness)*0.85f * c_Far;
		float end	= 0.93f * c_Far;
		if (HW.Caps.bTableFog && !HW.Caps.bWFog) {
			start/=c_Far;
			end  /=c_Far;
		}
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGSTART,	*(DWORD *)(&start)	));
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGEND,	*(DWORD *)(&end)	));

		// update suns
		for(DWORD i=0; i<Suns.size(); i++) Suns[i]->Update();
	}

//	if (pWeather) ((CPSObject*)pWeather)->Update();
}

void CEnvironment::SetGradient(float b){
	if (bUpdateBrightness){
		if (b>fBrightness) fBrightness = b;
	}else{
		bUpdateBrightness = true;
		fBrightness = b;
	}
}

void CEnvironment::OnDeviceCreate()
{
	if (pCreator->pLevel->LineExists("environment","sky"))
	{
		LPCSTR S = pCreator->pLevel->ReadSTRING("environment","sky");
		pSkydome = Render.Models.Create(S);
	} else pSkydome = NULL;
	
//	pWeather = Render.Models.CreatePS("snow",&m_WEmitter);
	
	c_Invalidate	();
}

void CEnvironment::OnDeviceDestroy()
{
	Render.Models.Delete	(pSkydome);
//	Render.Models.Delete	(pWeather);
}

extern float psHUD_FOV;
void CEnvironment::RenderFirst()
{

	if (pSkydome) {
		Fmatrix						mSky;
		mSky.translate				(Device.vCameraPosition);
		HW.pDevice->SetTransform	(D3DTS_WORLD,mSky.d3d());

		switch (pSkydome->Type)
		{
		case MT_HIERRARHY:
			{
				FHierrarhyVisual* pV	= (FHierrarhyVisual*)pSkydome;
				vector<FBasicVisual*>::iterator I,E;
				I = pV->chields.begin	();
				E = pV->chields.end		();
				for (; I!=E; I++)		
				{
					FBasicVisual* V		= *I;
					Device.Shader.Set	(V->hShader);
					CHK_DX				(HW.pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,Current.Sky.get()));
					V->Render			(1.f);
				}
					
			}
			break;
		default:
			Device.Shader.Set	(pSkydome->hShader);
			CHK_DX				(HW.pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,Current.Sky.get()));
			pSkydome->Render	(1.f);
			break;
		}
	}

	// Sun sources
	for(DWORD i=0; i<Suns.size(); i++) Suns[i]->RenderSource();
}

void CEnvironment::RenderLast()
{
	for(DWORD i=0; i<Suns.size(); i++) Suns[i]->RenderFlares();
//	if (pWeather) {
//		m_WEmitter.m_Position.set(Device.vCameraPosition);
//		m_WEmitter.m_Position.y += 10.f;
//
//		pWeather->Render		(1.f);
//	}
}
