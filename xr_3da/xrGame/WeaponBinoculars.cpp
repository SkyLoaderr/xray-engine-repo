#include "stdafx.h"
#include "WeaponBinoculars.h"
#include "WeaponHUD.h"
#include "xr_level_controller.h"
#include "level.h"
#include "../skeletonanimated.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponBinoculars::CWeaponBinoculars() : CWeapon("BINOCULARS")
{
	m_fScopeZoomFactor	= 40.f;
	fGyroSpeed			= 0.f;

	m_eSoundShow		= ESoundTypes(SOUND_TYPE_ITEM_TAKING | SOUND_TYPE_ITEM_USING);
	m_eSoundHide		= ESoundTypes(SOUND_TYPE_ITEM_HIDING | SOUND_TYPE_ITEM_USING);
}

CWeaponBinoculars::~CWeaponBinoculars()
{
	sndShow.destroy();
	sndHide.destroy();	
	sndGyro.destroy();
	sndZoomIn.destroy();
	sndZoomOut.destroy();
}

void CWeaponBinoculars::Load	(LPCSTR section)
{
	// verify class
	LPCSTR Class		= pSettings->r_string(section,"class");
	CLASS_ID load_cls	= TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	CHudItem::Load	(section);

	m_slot = pSettings->r_s32(section,"slot");

	Fvector				pos,ypr;
	pos					= pSettings->r_fvector3(section,"position");
	ypr					= pSettings->r_fvector3(section,"orientation");
	ypr.mul				(PI/180.f);

	m_fScopeZoomFactor	= pSettings->r_float	(section,"max_zoom_factor");

	m_Offset.setHPB		(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over(pos);

	LPCSTR hud_sect		= pSettings->r_string	(section,"hud");
	m_pHUD->Load		(hud_sect);

	camMaxAngle			= pSettings->r_float	(section,"cam_max_angle"	); camMaxAngle = deg2rad(camMaxAngle);
	camRelaxSpeed		= pSettings->r_float	(section,"cam_relax_speed"	); camRelaxSpeed = deg2rad(camRelaxSpeed);
	camDispersion		= pSettings->r_float	(section,"cam_dispersion"	); camDispersion = deg2rad(camDispersion);

	eHandDependence		= EHandDependence(pSettings->r_s32(section,"hand_dependence"));

	setVisible			(FALSE);

	// Sounds
	sndShow.create(TRUE, pSettings->r_string(section,	"snd_draw"), m_eSoundShow);
	sndHide.create(TRUE, pSettings->r_string(section,	"snd_holster"), m_eSoundHide);
	sndGyro.create(TRUE, pSettings->r_string(section,	"snd_gyro"), st_SourceType);
	sndZoomIn.create(TRUE, pSettings->r_string(section,	"snd_zoomin"), st_SourceType);
	sndZoomOut.create(TRUE, pSettings->r_string(section,"snd_zoomout"), st_SourceType);
	
	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle,		"idle");
	animGet				(mhud_show,		"draw");
	animGet				(mhud_hide,		"holster");
}

void CWeaponBinoculars::renderable_Render	()
{
	UpdateXForm						();
	if (hud_mode && m_pHUD)
	{ 
		// HUD render
		::Render->set_Transform		(&m_pHUD->Transform());
		::Render->add_Visual		(m_pHUD->Visual());
	}
	else
		inherited::renderable_Render();
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
		smart_cast<CSkeletonAnimated*>	(m_pHUD->Visual())->Update();
	case eZooming:
		state_Zooming		(dt);
		break;
	}

	// setVisible			(TRUE);
	m_bPending			= FALSE;
	
	// ref_sound fire loop
	///UpdateFP					();
	sndShow.set_position		(vLastFP);
	sndHide.set_position		(vLastFP);
	sndGyro.set_position		(vLastFP);
	sndZoomIn.set_position		(vLastFP);
	sndZoomOut.set_position		(vLastFP);
}

void CWeaponBinoculars::Hide	()
{
	if (Local())	SwitchState						(eHiding);
	//FireEnd							();
	//m_bPending						= TRUE;

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
	if (eZooming==STATE)	
		return	m_fScopeZoomFactor;
	else					
		return	inherited::GetZoomFactor();
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
	m_bPending				= TRUE;
	
	UpdateFP				();
	sndHide.play_at_pos		(H_Root(),vLastFP,hud_mode?sm_2D:0);

	m_pHUD->animPlay		(mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);

	RemoveShotEffector();

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
	
	UpdateFP				();
	sndShow.play_at_pos		(H_Root(),vLastFP,hud_mode?sm_2D:0);
	m_pHUD->animPlay		(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}

void CWeaponBinoculars::Fire2Start () {
	inherited::Fire2Start();
	OnZoomIn();
	m_fZoomFactor = m_fScopeZoomFactor;
}

void CWeaponBinoculars::Fire2End () {
	inherited::Fire2End();
	OnZoomOut();
	m_fZoomFactor = DEFAULT_FOV;
}

const char* CWeaponBinoculars::Name() {
	return CInventoryItem::Name();
}

bool CWeaponBinoculars::Action(s32 cmd, u32 flags) {
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) {
		case kWPN_ZOOM : {
			if(flags&CMD_START) Fire2Start();
			else Fire2End();
		} return true;
	}
	return false;
}
