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
	fMaxZoomFactor		= 40.f;
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
	LPCSTR Class		= pSettings->r_string(section,"class");
	CLASS_ID load_cls	= TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	CGameObject::Load	(section);

	Fvector				pos,ypr;
	pos					= pSettings->r_fvector3(section,"position");
	ypr					= pSettings->r_fvector3(section,"orientation");
	ypr.mul				(PI/180.f);

	fMaxZoomFactor		= pSettings->r_float	(section,"max_zoom_factor");

	m_Offset.setHPB		(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over(pos);

	ShaderCreate		(hUIIcon,"hud\\default","");

	LPCSTR hud_sect		= pSettings->r_string	(section,"hud");
	m_pHUD->Load		(hud_sect);

	camMaxAngle			= pSettings->r_float	(section,"cam_max_angle"	); camMaxAngle = deg2rad(camMaxAngle);
	camRelaxSpeed		= pSettings->r_float	(section,"cam_relax_speed"	); camRelaxSpeed = deg2rad(camRelaxSpeed);
	camDispersion		= pSettings->r_float	(section,"cam_dispersion"	); camDispersion = deg2rad(camDispersion);

	dispVelFactor		= pSettings->r_float	(section,"disp_vel_factor"	);
	dispJumpFactor		= pSettings->r_float	(section,"disp_jump_factor"	);
	dispCrouchFactor	= pSettings->r_float	(section,"disp_crouch_factor");
	eHandDependence		= CWeapon::EHandDependence(pSettings->r_s32(section,"hand_dependence"));

	// slot
	iSlotBinding		= pSettings->r_s32		(section,"slot");

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
	//if(STATE == S) return;
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
	case eHidden:
		switch2_Hidden	();
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
	case eShowing:
	case eHiding:
	case eReload:
		PKinematics		(m_pHUD->Visual())->Update();
	case eZooming:
		state_Zooming	(dt);
		break;
	}

	// setVisible			(TRUE);
	bPending			= FALSE;
	
	// sound fire loop
	UpdateFP					();
	sndShow.set_position		(vLastFP);
	sndHide.set_position		(vLastFP);
	sndGyro.set_position		(vLastFP);
	sndZoomIn.set_position		(vLastFP);
	sndZoomOut.set_position	(vLastFP);
}

void CWeaponBinoculars::Hide	()
{
	if (Local())	SwitchState						(eHiding);
	//FireEnd							();
	//bPending						= TRUE;

	//// add shot effector
	//if (Local())					
	//{
	//	Level().Cameras.RemoveEffector	(cefShot);
	//	SwitchState						(eHiding);
	//}
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
	case eHiding:	SwitchState(eHidden);	break;	// End of Hide
	case eShowing:	SwitchState(eIdle);		break;	// End of Show
	}
}

void CWeaponBinoculars::state_Zooming	(float dt)
{
	fGyroSpeed += dt;
	if (sndGyro.feedback){
		sndGyro.set_position(vLastFP);
		float k = _sqrt(fGyroSpeed);
		clamp(k,0.f,1.f); k = 0.6f*k+0.4f;
		sndGyro.set_frequency(k);
	}
}

void CWeaponBinoculars::switch2_Zooming	()
{
	sndZoomIn.play_at_pos		(H_Root(),vLastFP);
	sndGyro.play_at_pos			(H_Root(),vLastFP);
}

void CWeaponBinoculars::switch2_Idle	()
{
	switch (STATE)
	{
	case eZooming: 
		sndZoomOut.play_at_pos	(H_Root(),vLastFP);
		sndGyro.stop			();
		break;
	}
	m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
}

void CWeaponBinoculars::switch2_Hiding	()
{
	FireEnd					();
	bPending				= TRUE;
	Sound->play_at_pos		(sndHide,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
	if (Local())			Level().Cameras.RemoveEffector	(cefShot);

	//switch2_Idle			();
	//Sound->play_at_pos		(sndHide,H_Root(),vLastFP);
	//m_pHUD->animPlay		(mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
}
void CWeaponBinoculars::switch2_Hidden()
{
	signal_HideComplete		();
}

void CWeaponBinoculars::switch2_Showing	()
{
	setVisible				(TRUE);
	Sound->play_at_pos		(sndShow,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}


void CWeaponBinoculars::Fire2Start () {
	inherited::Fire2Start();
	OnZoomIn();
	fZoomFactor = fMaxZoomFactor;
}

void CWeaponBinoculars::Fire2End () {
	inherited::Fire2End();
	OnZoomOut();
	fZoomFactor = DEFAULT_FOV;
}
