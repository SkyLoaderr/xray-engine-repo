#include "stdafx.h"
#include "effectorshot.h"
#include "..\fstaticrender.h"
#include "..\bodyinstance.h"
#include "..\xr_tokens.h"
#include "..\3DSound.h"
#include "..\PSObject.h"
#include "..\xr_trims.h"
#include "hudmanager.h"

#include "WeaponHUD.h"
#include "WeaponAK74.h"
#include "entity.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponMagazined::CWeaponMagazined(LPCSTR name) : CWeapon(name)
{
	vLastFP.set		(0,0,0);
	vLastFD.set		(0,0,0);
	
	fTime			= 0;
}

CWeaponMagazined::~CWeaponMagazined()
{
	MediaUNLOAD		();
}

void CWeaponMagazined::Load(CInifile* ini, const char* section)
{
	inherited::Load	(ini, section);
	bFlame			= FALSE;
}

void CWeaponMagazined::OnDeviceCreate()
{
	REQ_CREATE	();
	inherited::OnDeviceCreate	();
	MediaLOAD	();
}

void CWeaponMagazined::OnDeviceDestroy()
{
	inherited::OnDeviceDestroy	();
	MediaUNLOAD	();
}

void CWeaponMagazined::UpdateXForm(BOOL bHUDView)
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame = Device.dwFrame;

		if (bHUDView) 
		{
			if (m_pHUD)	
			{
				Fmatrix			trans;
				Level().Cameras.unaffected_Matrix(trans);
				m_pHUD->UpdatePosition(trans);
			}
		} else {
			Fmatrix			mRes;
			CKinematics* V	= PKinematics(m_pParent->Visual());
			V->Calculate	();
			Fmatrix& mL		= V->LL_GetTransform(m_pContainer->m_iACTboneL);
			Fmatrix& mR		= V->LL_GetTransform(m_pContainer->m_iACTboneR);
			
			Fvector			R,D,N;
			D.sub			(mL.c,mR.c);	D.normalize_safe();
			R.crossproduct	(mR.j,D);		R.normalize_safe();
			N.crossproduct	(D,R);			N.normalize_safe();
			mRes.set		(R,N,D,mR.c);
			mRes.mul2_43	(m_pParent->clTransform);
			UpdatePosition	(mRes);
		}
	}
}

void CWeaponMagazined::UpdateFP(BOOL bHUDView)
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
			fire_mat.transform_tiny	(vLastFP,fp);
			parent.transform_tiny	(vLastFP);
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);
		} else {
			// 3rd person
			Fmatrix& parent			= svTransform;
			Fvector& fp				= vFirePoint;
			parent.transform_tiny	(vLastFP,fp);
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);
		}
	}
}

void CWeaponMagazined::FireStart		()
{
	if (!IsWorking() && IsValid()){ 
		CWeapon::FireStart	();
		st_target			= eFire;
	}
}

void CWeaponMagazined::FireEnd			()
{
	if (IsWorking())
	{
		CWeapon::FireEnd	();
		m_pHUD->FireEnd		();
		st_target			= eIdle;
	}
}

void CWeaponMagazined::OnMagazineEmpty	()
{
	st_target	=	eMagazineEmpty;
	fTime		=	fTimeToEmptyClick;
}

void CWeaponMagazined::Update(float dt, BOOL bHUDView)
{
	BOOL bShot			= false;

	inherited::Update	(dt,bHUDView);
	VERIFY				(m_pParent);
	
	// on state change
	if (st_target!=st_current)
	{
		switch(st_target)
		{
		case eIdle:
			bFlame			= FALSE;
			switch2_Idle	(bHUDView);
			break;
		case eFire:
			switch2_Fire	(bHUDView);
			break;
		case eMagEmpty:
			bFlame			= FALSE;
			switch2_Empty	(bHUDView);
			break;
		case eReload:
			switch2_Reload	(bHUDView);
			break;
		}
		st_current = st_target;
	}

	// cycle update
	switch (st_current)
	{
	case eIdle:
		fTime			-=	dt;
		if (fTime<0)	fTime = 0;
		break;
	case eFire:
		{
			UpdateFP				(bHUDView);
			fTime					-=dt;
			Fvector					p1, d;
			m_pParent->g_fireParams	(p1,d);
			
			while (fTime<0)
			{
				bFlame			=	TRUE;
				fTime			+=	fTimeToFire;

				OnShot			(bHUDView);
				FireTrace		(p1,vLastFP,d);
				m_pHUD->Shoot	();
			}
		}
		break;
	case eMagEmpty:
		{
			UpdateFP	(bHUDView);
			fTime		-=dt;

			while		(fTime<0)
			{
				fTime			+=  fTimeToEmptyClick;
				OnEmptyClick	();
			}
		}
		break;
	}

	m_pHUD->UpdateAnimation	();
}

void CWeaponMagazined::Render(BOOL bHUDView)
{
	inherited::Render		(bHUDView);
	UpdateXForm				(bHUDView);
	if (bHUDView)
	{ 
		// Interpolate lighting
		float l_f	= Device.fTimeDelta*fLightSmoothFactor;
		float l_i	= 1.f-l_f;
		float&	LL	= m_pParent->AI_Lighting;
		NodeCompressed*	NODE = m_pParent->AI_Node;
		float	CL	= NODE?float(NODE->light):255;
		LL			= l_i*LL + l_f*CL; 

		// HUD render
		if (m_pHUD) Level().HUD()->RenderModel(m_pHUD->Visual(),m_pHUD->Transform(),iFloor(LL));
	}
	else
	{
		// Actor render
		::Render.set_Transform		(&svTransform);
		::Render.set_LightLevel		(iFloor(m_pParent->AI_Lighting));
		::Render.add_leafs_Dynamic	(Visual());
	}
	if ((st_current==eFire) && bFlame) 
	{
		if (bHUDView &&	(0==Level().Cameras.GetEffector(cefShot)))	Level().Cameras.AddEffector(new CEffectorShot(camRelax,camDispersion));

		UpdateFP	(bHUDView);
		OnDrawFlame	(bHUDView);
	}
}

void CWeaponMagazined::SetDefaults	()
{
	CWeapon::SetDefaults		();
	iAmmoElapsed				= 0;
}

void CWeaponMagazined::Hide		()
{
	inherited::Hide				();
	signal_HideComplete			();
}
void CWeaponMagazined::Show		()
{
	inherited::Show				();
}

void CWeaponMagazined::FireShotmark(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R) 
{
	inherited::FireShotmark		(vDir, vEnd, R);
	OnShotmark					(vDir, vEnd, R);
}
