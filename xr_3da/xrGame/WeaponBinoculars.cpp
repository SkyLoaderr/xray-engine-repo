#include "stdafx.h"

#include "WeaponBinoculars.h"
#include "..\portal.h"
#include "WeaponHUD.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponBinoculars::CWeaponBinoculars() : CWeapon("BINOCULARS")
{
	fMaxZoomFactor		= 35.f;
	fGyroSpeed			= 0.f;
}

CWeaponBinoculars::~CWeaponBinoculars()
{
	SoundDestroy		(sndShow		);
	SoundDestroy		(sndHide		);
	SoundDestroy		(sndGyro		);
	SoundDestroy		(sndZoomIn		);
	SoundDestroy		(sndZoomOut		); 
}

void CWeaponBinoculars::Load	(LPCSTR section)
{
	// verify class
	LPCSTR Class		= pSettings->ReadSTRING(section,"class");
	CLASS_ID load_cls	= TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	CGameObject::Load	(section);

	Fvector				pos,ypr;
	pos					= pSettings->ReadVECTOR(section,"position");
	ypr					= pSettings->ReadVECTOR(section,"orientation");
	ypr.mul				(PI/180.f);

	fMaxZoomFactor		= pSettings->ReadFLOAT	(section,"max_zoom_factor");

	m_Offset.setHPB		(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over(pos);

	ShaderCreate		(hUIIcon,"hud\\default","");

	LPCSTR hud_sect		= pSettings->ReadSTRING	(section,"hud");
	m_pHUD->Load		(hud_sect);

	camMaxAngle			= pSettings->ReadFLOAT	(section,"cam_max_angle"	); camMaxAngle = deg2rad(camMaxAngle);
	camRelaxSpeed		= pSettings->ReadFLOAT	(section,"cam_relax_speed"	); camRelaxSpeed = deg2rad(camRelaxSpeed);
	camDispersion		= pSettings->ReadFLOAT	(section,"cam_dispersion"	); camDispersion = deg2rad(camDispersion);

	dispVelFactor		= pSettings->ReadFLOAT	(section,"disp_vel_factor"	);
	dispJumpFactor		= pSettings->ReadFLOAT	(section,"disp_jump_factor"	);
	dispCrouchFactor	= pSettings->ReadFLOAT	(section,"disp_crouch_factor");
	eHandDependence		= CWeapon::EHandDependence(pSettings->ReadINT(section,"hand_dependence"));

	setVisible			(FALSE);

	// Sounds
	SoundCreate			(sndShow,		"draw");
	SoundCreate			(sndHide,		"holster");
	SoundCreate			(sndGyro,		"gyro",0,TRUE);
	SoundCreate			(sndZoomIn,		"zoomin");
	SoundCreate			(sndZoomOut,	"zoomout");
	
	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle,		"idle");
	animGet				(mhud_show,		"draw");
	animGet				(mhud_hide,		"holster");
}


void CWeaponBinoculars::OnVisible	()
{
	inherited::OnVisible			();
	UpdateXForm						();
	if (hud_mode && m_pHUD)
	{ 
		// HUD render
		::Render->set_Transform		(&m_pHUD->Transform());
		::Render->add_Visual		(m_pHUD->Visual());
	}
	else
	{
		// Actor render
		::Render->set_Transform		(&svTransform);
		::Render->add_Visual		(Visual());
	}
}

void CWeaponBinoculars::OnStateSwitch	(u32 S)
{
	switch (S)
	{
	case eIdle:
		switch2_Idle	();
		break;
	case eShowing:
		switch2_Showing	();
		break;
	case eHiding:
		switch2_Hiding	();
		break;
	case eZooming:
		switch2_Zooming	();
		break;
	}
	STATE = S;
}
	
void CWeaponBinoculars::UpdateCL	()
{
	inherited::UpdateCL	();

	float dt			= Device.fTimeDelta;
	
	// cycle update
	switch (STATE)
	{
	case eZooming:
		state_Zooming	(dt);
		break;
	}

	// setVisible			(TRUE);
	bPending			= FALSE;
	
	// sound fire loop
	UpdateFP					();
	if (sndShow.feedback)		sndShow.feedback->SetPosition		(vLastFP);
	if (sndHide.feedback)		sndHide.feedback->SetPosition		(vLastFP);
	if (sndGyro.feedback)		sndGyro.feedback->SetPosition		(vLastFP);
	if (sndZoomIn.feedback)		sndZoomIn.feedback->SetPosition		(vLastFP);
	if (sndZoomOut.feedback)	sndZoomOut.feedback->SetPosition	(vLastFP);
}

void CWeaponBinoculars::Hide	()
{
	FireEnd							();
	bPending						= TRUE;

	// add shot effector
	if (Local())					
	{
		Level().Cameras.RemoveEffector	(cefShot);
		SwitchState						(eHiding);
	}
}

void CWeaponBinoculars::Show	()
{
	if (Local())				
	{
		SwitchState						(eShowing);
	}
}

float CWeaponBinoculars::GetZoomFactor()
{
	if (eZooming==STATE)	return	fMaxZoomFactor;
	else					return	inherited::GetZoomFactor();
}

void CWeaponBinoculars::OnZoomIn()
{
	inherited::OnZoomIn			();
	SwitchState	(eZooming);
	fGyroSpeed  = 0;
}

void CWeaponBinoculars::OnZoomOut()
{
	inherited::OnZoomOut();
	SwitchState	(eIdle);
	fGyroSpeed  = 0;
}

void CWeaponBinoculars::OnAnimationEnd()
{
	switch (STATE)
	{
	case eHiding:	signal_HideComplete();	break;	// End of Hide
	case eShowing:	SwitchState(eIdle);		break;	// End of Show
	}
}

void CWeaponBinoculars::state_Zooming	(float dt)
{
	fGyroSpeed += dt;
	if (sndGyro.feedback){
		sndGyro.feedback->SetPosition(vLastFP);
		float k = sqrtf(fGyroSpeed);
		clamp(k,0.f,1.f); k = 0.6f*k+0.4f;
		sndGyro.feedback->SetFrequencyScale(k);
	}
}

void CWeaponBinoculars::switch2_Zooming	()
{
	pSounds->PlayAtPos			(sndZoomIn,H_Root(),vLastFP);
	pSounds->PlayAtPos			(sndGyro,H_Root(),vLastFP,true);
}

void CWeaponBinoculars::switch2_Idle	()
{
	switch (STATE)
	{
	case eZooming: 
		pSounds->PlayAtPos			(sndZoomOut,H_Root(),vLastFP);
		if (sndGyro.feedback)		sndGyro.feedback->Stop();
		break;
	}
	m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
}

void CWeaponBinoculars::switch2_Hiding	()
{
	switch2_Idle			();
	pSounds->PlayAtPos		(sndHide,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
}

void CWeaponBinoculars::switch2_Showing	()
{
	setVisible				(TRUE);
	pSounds->PlayAtPos		(sndShow,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}

