#include "stdafx.h"
#include "xr_ini.h"
#include "xr_creator.h"
#include "xr_effects.h"
#include "x_ray.h"
#include "xr_zexplosion.h"
#include "xr_sndman.h"

D3DTLPOLYGON				IdentityTLPolygon;

//-----------------------------------------------------------------------------------------------------------
//EffectManager
//-----------------------------------------------------------------------------------------------------------
CEffectManager::CEffectManager	( )
{
	FPcolor					d, s;
    d.set					(255, 255, 255, 255);
    s.set					(0, 0, 0, 0);

	IdentityLPolygon[0].set (0.0f, 0.0f, 0.0f, d, s, 1.0f, 1.0f);
	IdentityLPolygon[1].set (0.0f, 0.0f, 0.0f, d, s, 0.0f, 1.0f);
	IdentityLPolygon[2].set (0.0f, 0.0f, 0.0f, d, s, 0.0f, 0.0f);
	IdentityLPolygon[3].set (0.0f, 0.0f, 0.0f, d, s, 1.0f, 0.0f);

	IdentityTLPolygon[0].set(0.0f, 0.0f, 0.0f, 1.0f, d, s, 1.0f, 1.0f);
	IdentityTLPolygon[1].set(0.0f, 0.0f, 0.0f, 1.0f, d, s, 0.0f, 1.0f);
	IdentityTLPolygon[2].set(0.0f, 0.0f, 0.0f, 1.0f, d, s, 0.0f, 0.0f);
	IdentityTLPolygon[3].set(0.0f, 0.0f, 0.0f, 1.0f, d, s, 1.0f, 0.0f);

	m_phase					= 0;

	OnDeviceCreate		( );

	Device.seqDevDestroy.Add(this);
	Device.seqDevCreate.Add(this);
}

CEffectManager::~CEffectManager	( )
{
	Device.seqDevDestroy.Remove(this);
	Device.seqDevCreate.Remove(this);

//	_DELETE					(sky);
//	_DELETE					(stars);
//	_DELETE					(clouds);
//	_DELETE					(xrGamma);

// explode data
	/*
	CXR_PSB_Explosion::UnloadTemplate( &ze_simple_bomb		);
	CXR_PSB_Explosion::UnloadTemplate( &ze_swiched_bomb		);
	CXR_PSB_Explosion::UnloadTemplate( &ze_static_bomb		);
	CXR_PSB_Explosion::UnloadTemplate( &ze_energon			);
	CXR_PSB_Explosion::UnloadTemplate( &ze_key				);
	CXR_PSB_Explosion::UnloadTemplate( &ze_teleport			);

	CSpriteParticle::UnloadTemplate( &sp_take_red_key	);
	CSpriteParticle::UnloadTemplate( &sp_take_green_key	);
	CSpriteParticle::UnloadTemplate( &sp_take_blue_key	);
	CSpriteParticle::UnloadTemplate( &sp_take_energon	);
	CSpriteParticle::UnloadTemplate( &sp_take_bomb		);
	*/
	while (effects_list.count){
		delete				effects_list[0];
		effects_list.Del	(effects_list[0]);
	}

// teleport data
}

BOOL CEffectManager::Load( CInifile* ini, LPSTR section )
{
//Load explode data
	/*
	CXR_PSB_Explosion::LoadTemplate( pSettings,	"ze_simple_bomb",		&ze_simple_bomb		);
	CXR_PSB_Explosion::LoadTemplate( pSettings,	"ze_swiched_bomb",		&ze_swiched_bomb	);
	CXR_PSB_Explosion::LoadTemplate( pSettings,	"ze_static_bomb",		&ze_static_bomb		);
	CXR_PSB_Explosion::LoadTemplate( pSettings,	"ze_energon",			&ze_energon			);
	CXR_PSB_Explosion::LoadTemplate( pSettings,	"ze_key",				&ze_key				);
	CXR_PSB_Explosion::LoadTemplate( pSettings,	"ze_teleport",			&ze_teleport		);
	CXR_PSB_Lightning::LoadTemplate( pSettings,	"zl_staticlightning",	&zl_staticlightning	);
	CXR_PSB_Lightning::LoadTemplate( pSettings,	"zl_dynamiclightning",	&zl_dynamiclightning);

//Load take key data
	CSpriteParticle::LoadTemplate( pSettings,	"take_red_key",		&sp_take_red_key	);
	CSpriteParticle::LoadTemplate( pSettings,	"take_green_key",	&sp_take_green_key	);
	CSpriteParticle::LoadTemplate( pSettings,	"take_blue_key",	&sp_take_blue_key	);
	CSpriteParticle::LoadTemplate( pSettings,	"take_energon",		&sp_take_energon	);
	CSpriteParticle::LoadTemplate( pSettings,	"take_bomb",		&sp_take_bomb		);

	VERIFY(&sp_take_red_key	);
	VERIFY(&sp_take_green_key);
	VERIFY(&sp_take_blue_key	);
	VERIFY(&sp_take_energon	);
	VERIFY(&sp_take_bomb		);
	*/
	/*
//	if ( ini->ReadSection	( "sky" ) ){
//		sky					= new CEffectSky();
//		if (!sky->Load( "sky" ))		_DELETE(sky);
//	}

	if ( ini->ReadSection	( "clouds" ) ){
		clouds				= new CEffectClouds();
		if (!clouds->Load( "clouds" ))	_DELETE(clouds);
	}

	if ( ini->ReadSection	( "stars" ) ){
		stars				= new CEffectStars();
		if (!stars->Load( "stars" ))	_DELETE(stars);
	}

	if (!xrGamma) xrGamma	= new CGammaControl	( );
	*/
	Update					( );
	_OK;
}

void CEffectManager::OnMove	( )
{
//---------------------------------------------------
	m_phase					+= 0.5f * Device.fTimeDelta;
	if (m_phase>PI_MUL_2)		m_phase -= PI_MUL_2;
	deltaY					= sinf(m_phase)*2.5f;
//---------------------------------------------------

//	if ( sky	&& (xrENV.state&EFF_SKY) )		sky->OnMove		( );
//	if ( clouds	&& (xrENV.state&EFF_CLOUDS) )	clouds->OnMove	( );
//	if ( stars	&& (xrENV.state&EFF_STARS) )	stars->OnMove	( );

// internal
// WARNING
//	if ( xrGamma )								xrGamma->OnMove	( );

	for (int i=effects_list.count-1; i>=0; i--){
		effects_list[i]->OnMove( );
		if ( effects_list[i]->state == EFF_STATE_STOP ){
			_DELETE			(effects_list[i]);
			effects_list.Del(effects_list[i]);
		}
	}
}

void CEffectManager::SetEffProjMat( )
{
	float	d = 2000.f + 2000.f * 0.4f;
	HW.pDevice->GetTransform	( D3DTRANSFORMSTATE_PROJECTION, save_matProj.d3d() );
	CopyMemory					( &matProj, &save_matProj, sizeof(Fmatrix));
	matProj._33					= d / ( d + save_matProj._43/save_matProj._33 );
	HW.pDevice->SetTransform	( D3DTRANSFORMSTATE_PROJECTION, matProj.d3d() );
}

void CEffectManager::ResetEffProjMat( )
{
	HW.pDevice->SetTransform	( D3DTRANSFORMSTATE_PROJECTION, save_matProj.d3d() );
}

void CEffectManager::RenderNormal	( )
{
	SetEffProjMat();
	Device.SetMode(vZTest|vLight);

//	if ( sky	&& (xrENV.state&EFF_SKY) )		sky->RenderNormal( );

	ResetEffProjMat();
}

void CEffectManager::RenderAlphaFirst	( )
{
	SetEffProjMat();

	Device.SetMode(vAlpha);
//	CHK_DX(HW.pDevice->SetRenderState	( D3DRENDERSTATE_SRCBLEND,		D3DBLEND_ONE ));
//	CHK_DX(HW.pDevice->SetRenderState	( D3DRENDERSTATE_DESTBLEND,		D3DBLEND_INVSRCCOLOR ));

//	if ( sky	&& (xrENV.state&EFF_SKY) )		sky->RenderNormal( );

//	if ( stars	&& (xrENV.state&EFF_STARS) )	stars->RenderAlpha	( );
//	if ( sun	&& (xrENV.state&EFF_SUN) )		sun->RenderSource	( );
//	if ( moon	&& (xrENV.state&EFF_SUN) )		moon->RenderSource	( );

	// shader = "std_aop"
//	if ( clouds	&& (xrENV.state&EFF_CLOUDS) ){
//		clouds->RenderAlpha	( );
//	}

	ResetEffProjMat();
}

void CEffectManager::RenderAlphaLast	( )
{
	SetEffProjMat();

	Device.SetMode(vAlpha);

	if (effects_list.count){
		int cnt_alpha = 0;
		// ??? WARNING
//		CHK_DX(HW.pDevice->SetRenderState	( D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA ) );
//		CHK_DX(HW.pDevice->SetRenderState	( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE ) );
		for (int i=0; i<effects_list.count; i++)
			if (!effects_list[i]->bHasAlpha){
				effects_list[i]->Render	( );
				cnt_alpha++;
			}
		if (cnt_alpha != effects_list.count){
			// shader = "std_aop"
			for (i=0; i<effects_list.count; i++)
				if (effects_list[i]->bHasAlpha) effects_list[i]->Render( );
		}
	}

//  WARNING
//	CHK_DX(HW.pDevice->SetRenderState	( D3DRENDERSTATE_SRCBLEND,		D3DBLEND_ONE ));
//	CHK_DX(HW.pDevice->SetRenderState	( D3DRENDERSTATE_DESTBLEND,		D3DBLEND_INVSRCCOLOR ));

//	if ( sun && (xrENV.state&EFF_SUN) && (xrENV.state&EFF_FLARES) )	sun->RenderFlares	( );
//	WARNING
//	if (xrGamma)				xrGamma->RenderFlash();
	ResetEffProjMat();
}

// device dependance
void CEffectManager::OnDeviceDestroy( )
{
//	_DELETE					( xrGamma );
}

void CEffectManager::OnDeviceCreate( )
{
//	if (!xrGamma){
//		xrGamma				= new CGammaControl	( );
//		WARNING
//		if (xrGamma && !xrGamma->xrGammaControl) _DELETE(xrGamma);
//	}
	Update					( );
}

void CEffectManager::Update	( )
{
//	D3DVALUE k				= xrENV.daylight;
//	xrENV.daycolor.set		( xrENV.daycolorS  );
//	xrENV.backcolor.set		( xrENV.backcolorS );
//	xrENV.fogcolor.set		( xrENV.fogcolorS  );
//	xrENV.daycolor.mul		( k );
//	xrENV.backcolor.mul		( k );
//	xrENV.fogcolor.mul		( k );
//    HW.pDevice->SetTextureStageState( 0, D3DTSS_BORDERCOLOR, xrENV.fogcolor.c );

//	if ( sun	)			sun->Update		( );
//	if ( moon	)			moon->Update	( );
//	if ( clouds	)			clouds->Update	( );
//	if ( stars	)			stars->Update	( );
//	if ( sky	)			sky->Update		( );

//	internal
//	WARNING
//	if ( xrGamma )			xrGamma->Update	( );

//	CopyMemory(&xrENV_OLD,&xrENV,sizeof(xrENV));
}

void CEffectManager::StopEffect (CEffectBase *Effect)
{
	effects_list.Del(Effect);
	_DELETE (Effect);
}

