#include "stdafx.h"

#include "WeaponBinoculars.h"
#include "..\render.h"
#include "..\bodyinstance.h"
#include "..\3DSound.h"
#include "..\portal.h"
#include "WeaponHUD.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponBinoculars::CWeaponBinoculars() : CWeapon("BINOCULARS")
{
	fMaxZoomFactor		= 35.f;
}

CWeaponBinoculars::~CWeaponBinoculars()
{
	SoundDestroy		(sndShow		);
	SoundDestroy		(sndHide		);
	SoundDestroy		(sndGyro		);
	SoundDestroy		(sndZoomIn		);
	SoundDestroy		(sndZoomOut		);
}

void CWeaponBinoculars::Load(CInifile* ini, const char* section)
{
	// verify class
	LPCSTR Class		= pSettings->ReadSTRING(section,"class");
	CLASS_ID load_cls	= TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	CObject::Load		(ini,section);

	SectorMode			= EPM_AT_LOAD;
	if (pSector)		pSector->objectRemove	(this);

	Fvector				pos,ypr;
	pos					= ini->ReadVECTOR(section,"position");
	ypr					= ini->ReadVECTOR(section,"orientation");
	ypr.mul				(PI/180.f);

	fMaxZoomFactor		= ini->ReadFLOAT	(section,"max_zoom_factor");

	m_Offset.setHPB		(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over(pos);

	ShaderCreate		(hUIIcon,"hud\\default","");

	LPCSTR hud_sect		= ini->ReadSTRING	(section,"hud");
	m_pHUD->Load		(ini,hud_sect);

	camMaxAngle			= ini->ReadFLOAT	(section,"cam_max_angle"	); camMaxAngle = deg2rad(camMaxAngle);
	camRelaxSpeed		= ini->ReadFLOAT	(section,"cam_relax_speed"	); camRelaxSpeed = deg2rad(camRelaxSpeed);
	camDispersion		= ini->ReadFLOAT	(section,"cam_dispersion"	); camDispersion = deg2rad(camDispersion);

	dispVelFactor		= ini->ReadFLOAT	(section,"disp_vel_factor"	);
	dispJumpFactor		= ini->ReadFLOAT	(section,"disp_jump_factor"	);
	dispCrouchFactor	= ini->ReadFLOAT	(section,"disp_crouch_factor");

	bVisible			= FALSE;

	// Sounds
	SoundCreate			(sndShow,		"draw");
	SoundCreate			(sndHide,		"holster");
	SoundCreate			(sndGyro,		"gyro");
	SoundCreate			(sndZoomIn,		"zoomin");
	SoundCreate			(sndZoomOut,	"zoomout");
	
	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle,		"idle");
	animGet				(mhud_show,		"draw");
	animGet				(mhud_hide,		"holster");
}

void CWeaponBinoculars::UpdateXForm(BOOL bHUDView)
{
	if (Device.dwFrame!=dwXF_Frame){
		dwXF_Frame = Device.dwFrame;

		if (bHUDView){
			if (m_pHUD){
				Fmatrix			trans;
				Level().Cameras.affected_Matrix(trans);
				m_pHUD->UpdatePosition(trans);
			}
		} else {
			Fmatrix			mRes;
			CKinematics* V	= PKinematics	(H_Parent()->Visual());
			V->Calculate	();
			Fmatrix& mL		= V->LL_GetTransform(m_pContainer->m_iACTboneL);
			Fmatrix& mR		= V->LL_GetTransform(m_pContainer->m_iACTboneR);
			
			Fvector			R,D,N; 
			D.sub			(mL.c,mR.c);	D.normalize_safe();
			R.crossproduct	(mR.j,D);		R.normalize_safe();
			N.crossproduct	(D,R);			N.normalize_safe();
			mRes.set		(R,N,D,mR.c);
			mRes.mulA_43	(H_Parent()->clXFORM());
			UpdatePosition	(mRes);
		}
	}
}

void CWeaponBinoculars::UpdateFP		(BOOL bHUDView)
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		UpdateXForm		(bHUDView);

		if (bHUDView)	
		{
			// 1st person view - skeletoned
			CKinematics* V			= PKinematics(m_pHUD->Visual());
			V->Calculate			();

			// fire point&direction
			Fmatrix& fire_mat		= V->LL_GetTransform(m_pHUD->iFireBone);
			Fmatrix& parent			= m_pHUD->Transform	();
			Fvector& fp				= m_pHUD->vFirePoint;
			Fvector& sp				= m_pHUD->vShellPoint;
			fire_mat.transform_tiny	(vLastFP,fp);
			parent.transform_tiny	(vLastFP);
			fire_mat.transform_tiny	(vLastSP,sp);
			parent.transform_tiny	(vLastSP);
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);
		} else {
			// 3rd person
			Fmatrix& parent			= svTransform;
			Fvector& fp				= vFirePoint;
			Fvector& sp				= vShellPoint;
			parent.transform_tiny	(vLastFP,fp);
			parent.transform_tiny	(vLastSP,sp);
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);
		}
	}
}

void CWeaponBinoculars::Render		(BOOL bHUDView)
{
	inherited::Render		(bHUDView);
	UpdateXForm				(bHUDView);
	if (bHUDView && m_pHUD)
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

void CWeaponBinoculars::Update			(float dt, BOOL bHUDView)
{
	inherited::Update	(dt,bHUDView);
	VERIFY				(H_Parent());
	
	// on state change
	if (st_target!=st_current)
	{
		switch (st_target)
		{
		case eIdle:
			switch2_Idle	(bHUDView);
			break;
		case eShowing:
			switch2_Showing	(bHUDView);
			break;
		case eHiding:
			switch2_Hiding	(bHUDView);
			break;
		}
		st_current = st_target;
	}

	bVisible			= TRUE;
	bPending			= FALSE;
	
	// sound fire loop
	UpdateFP					(bHUDView);
	if (sndShow.feedback)		sndShow.feedback->SetPosition		(vLastFP);
	if (sndHide.feedback)		sndHide.feedback->SetPosition		(vLastFP);
	if (sndGyro.feedback)		sndGyro.feedback->SetPosition		(vLastFP);
	if (sndZoomIn.feedback)		sndZoomIn.feedback->SetPosition		(vLastFP);
	if (sndZoomOut.feedback)	sndZoomOut.feedback->SetPosition	(vLastFP);
}
void CWeaponBinoculars::Hide	()
{
	inherited::Hide				();
}
void CWeaponBinoculars::Show	()
{
	inherited::Show				();
}
void CWeaponBinoculars::OnShow	()
{
	st_target	= eShowing;
}
void CWeaponBinoculars::OnHide	()
{
	st_target	= eHiding;
}
void CWeaponBinoculars::OnZoomIn()
{
	inherited::OnZoomIn			();
	pSounds->PlayAtPos			(sndZoomIn,H_Root(),vLastFP);
	pSounds->PlayAtPos			(sndGyro,H_Root(),vLastFP,true);
}
void CWeaponBinoculars::OnZoomOut()
{
	inherited::OnZoomOut();
	pSounds->PlayAtPos			(sndZoomOut,H_Root(),vLastFP);
	if (sndGyro.feedback)		sndGyro.feedback->Stop();
}
void CWeaponBinoculars::OnAnimationEnd()
{
	switch (st_current)
	{
	case eHiding:	signal_HideComplete();	break;	// End of Hide
	case eShowing:	st_target = eIdle;		break;	// End of Show
	}
}
void CWeaponBinoculars::switch2_Idle	(BOOL bHUDView)
{
	m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
}
void CWeaponBinoculars::switch2_Hiding(BOOL bHUDView)
{
	switch2_Idle			(bHUDView);
	pSounds->PlayAtPos		(sndHide,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
}
void CWeaponBinoculars::switch2_Showing(BOOL bHUDView)
{
	pSounds->PlayAtPos		(sndShow,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}

