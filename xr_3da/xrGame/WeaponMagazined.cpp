#include "stdafx.h"
#include "effectorshot.h"
#include "..\render.h"
#include "..\bodyinstance.h"
#include "..\xr_tokens.h"
#include "..\3DSound.h"
#include "..\PSObject.h"
#include "..\xr_trims.h"
#include "hudmanager.h"

#include "WeaponHUD.h"
#include "WeaponMagazined.h"
#include "entity.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponMagazined::CWeaponMagazined(LPCSTR name) : CWeapon(name)
{
	fTime			= 0;
}

CWeaponMagazined::~CWeaponMagazined()
{
	// sounds
	SoundDestroy		(sndShow		);
	SoundDestroy		(sndHide		);
	SoundDestroy		(sndShot		);
	SoundDestroy		(sndEmptyClick	);
	SoundDestroy		(sndReload		);
	SoundDestroy		(sndRicochet[0]	);
	SoundDestroy		(sndRicochet[1]	);
	SoundDestroy		(sndRicochet[2]	);
	SoundDestroy		(sndRicochet[3]	);
	SoundDestroy		(sndRicochet[4]	);
	
	MediaUNLOAD		();
}

void CWeaponMagazined::Load(CInifile* ini, const char* section)
{
	inherited::Load	(ini, section);
	bFlame			= FALSE;
	// Sounds
	SoundCreate			(sndShow,		"draw"    ,m_eSoundShow);
	SoundCreate			(sndHide,		"holster" ,m_eSoundHide);
	SoundCreate			(sndShot,		"shoot"   ,m_eSoundShot);
	SoundCreate			(sndEmptyClick,	"empty"   ,m_eSoundEmptyClick);
	SoundCreate			(sndReload,		"reload"  ,m_eSoundReload);
	SoundCreate			(sndRicochet[0],"ric1"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[1],"ric2"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[2],"ric3"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[3],"ric4"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[4],"ric5"    ,m_eSoundRicochet);
	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	mhud_idle			= m_pHUD->animGet("idle"	);
	mhud_reload			= m_pHUD->animGet("reload"	);
	mhud_show			= m_pHUD->animGet("draw"	);
	mhud_hide			= m_pHUD->animGet("holster"	);
	for (int i=0; i<32; i++)
	{
		string128		sh_anim;
		sprintf			(sh_anim,"shoot%d",i);
		CMotionDef* M	= m_pHUD->animGet(sh_anim);
		if (M)			mhud_shots.push_back(M);
	}
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

void CWeaponMagazined::UpdateFP		(BOOL bHUDView)
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

void CWeaponMagazined::FireStart		()
{
	if (!IsWorking() && IsValid())
	{
		if (st_current==eReload)			return;
		if (st_current==eShowing)			return;
		if (st_current==eHiding)			return;
		if (!iAmmoElapsed && iAmmoCurrent)	
		{
			CWeapon::FireStart	();
			st_target			= eMagEmpty;
		}
		else							
		{
			CWeapon::FireStart	();
			st_target			= eFire;
		}
	}
}

void CWeaponMagazined::FireEnd			()
{
	if (IsWorking())
	{
		CWeapon::FireEnd	();
		if (eMagEmpty == st_current)	TryReload	();
		else							st_target	= eIdle;
	}
}

void CWeaponMagazined::Reload			()
{
	TryReload			();
}

void CWeaponMagazined::TryReload		()
{
	if (iAmmoCurrent && (iAmmoElapsed<iMagazineSize))	st_target = eReload;
	else												st_target = eIdle;
}

void CWeaponMagazined::OnMagazineEmpty	()
{
	st_target			=	eMagEmpty;
	fTime				=	fTimeToEmptyClick;
}

void CWeaponMagazined::ReloadMagazine	()
{
	st_target			=	eIdle;
	if (iAmmoCurrent>=iMagazineSize)	
	{
		iAmmoCurrent += iAmmoElapsed; 
		iAmmoElapsed =	iMagazineSize; 
		iAmmoCurrent -= iMagazineSize;	
	}
	else
	{ 
		iAmmoElapsed = iAmmoCurrent+iAmmoElapsed;
		if (iAmmoElapsed>iMagazineSize)	iAmmoCurrent =	iAmmoElapsed-iMagazineSize;
		else							iAmmoCurrent =  0;
	}
}

void CWeaponMagazined::Update			(float dt, BOOL bHUDView)
{
	inherited::Update	(dt,bHUDView);
	VERIFY				(H_Parent());
	
	// on state change
	if (st_target!=st_current)
	{
		switch (st_target)
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
		case eShowing:
			switch2_Showing	(bHUDView);
			break;
		case eHiding:
			switch2_Hiding	(bHUDView);
			break;
		}
		st_current = st_target;
	}
	
	// cycle update
	switch (st_current)
	{
	case eIdle:
	case eShowing:
	case eHiding:
	case eReload:
		fTime			-=	dt;
		if (fTime<0)	fTime = 0;
		break;
	case eFire:			state_Fire		(bHUDView,dt);	break;
	case eMagEmpty:		state_MagEmpty	(bHUDView,dt);	break;
	}
	bVisible			= TRUE;
	bPending			= FALSE;
	
	// sound fire loop
	UpdateFP					(bHUDView);
	if (sndShow.feedback)		sndShow.feedback->SetPosition		(vLastFP);
	if (sndHide.feedback)		sndHide.feedback->SetPosition		(vLastFP);
	if (sndShot.feedback)		sndShot.feedback->SetPosition		(vLastFP);
	if (sndReload.feedback)		sndReload.feedback->SetPosition		(vLastFP);
	if (sndEmptyClick.feedback)	sndEmptyClick.feedback->SetPosition	(vLastFP);
}

void CWeaponMagazined::state_Fire	(BOOL bHUDView, float dt)
{
	UpdateFP				(bHUDView);
	fTime					-=dt;
	Fvector					p1, d;
	dynamic_cast<CEntity*>(H_Parent())->g_fireParams	(p1,d);
	
	while (fTime<0)
	{
		bFlame			=	TRUE;
		fTime			+=	fTimeToFire;
		
		OnShot			(bHUDView);
		FireTrace		(p1,vLastFP,d);
	}
}

void CWeaponMagazined::state_MagEmpty(BOOL bHUDView, float dt)
{
	UpdateFP	(bHUDView);
	fTime		-=dt;
	
	while		(fTime<0)
	{
		fTime			+=  fTimeToEmptyClick;
		OnEmptyClick	(bHUDView);
	}
}

void CWeaponMagazined::Render		(BOOL bHUDView)
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
	if (((eFire==st_current) || (eReload==st_current))&& bFlame) 
	{
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
}
void CWeaponMagazined::Show		()
{
	inherited::Show				();
}
void CWeaponMagazined::OnShow	()
{
	st_target	= eShowing;
}
void CWeaponMagazined::OnHide	()
{
	st_target	= eHiding;
}
void CWeaponMagazined::FireShotmark(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R) 
{
	inherited::FireShotmark		(vDir, vEnd, R);
	OnShotmark					(vDir, vEnd, R);
}

void CWeaponMagazined::MediaLOAD		()
{
	if (hFlames.size())		return;

	// flame textures
	LPCSTR S		= pSettings->ReadSTRING	(cName(),"flame");
	DWORD scnt		= _GetItemCount(S);
	string256 name;
	for (DWORD i=0; i<scnt; i++)
	{
		Shader* SH			= 0;
		ShaderCreate		(SH,"effects\\flame",_GetItem(S,i,name));
		hFlames.push_back	(SH);
	}
}

void CWeaponMagazined::MediaUNLOAD	()
{
	for (DWORD i=0; i<hFlames.size(); i++)
		ShaderDestroy(hFlames[i]);
	hFlames.clear();
}

void CWeaponMagazined::OnShellDrop(BOOL bHUD)
{
	// shells
	CPSObject* PS				= new CPSObject("weapons\\shells_generic",H_Parent()->Sector(),true);
	Fmatrix M;
	M.setHPB(PS->m_Emitter.m_ConeHPB.x, PS->m_Emitter.m_ConeHPB.y, PS->m_Emitter.m_ConeHPB.z);
	Fvector V; M.transform_dir(V,vLastFD);
	PS->m_Emitter.m_ConeDirection.set(V);
	PS->PlayAtPos				(vLastSP);
}

void CWeaponMagazined::OnShot(BOOL bHUD)
{
	// Sound
	pSounds->Play3DAtPos		(sndShot,H_Root(),vLastFP);
	// Camera
	if (bHUD)	{
		CEffectorShot* S		= dynamic_cast<CEffectorShot*>(Level().Cameras.GetEffector(cefShot)); 
		if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(new CEffectorShot(camMaxAngle,camRelaxSpeed));
		R_ASSERT(S);
		S->Shot					(camDispersion);
	}
	// Animation
	m_pHUD->animPlay			(mhud_shots[Random.randI(mhud_shots.size())],FALSE);
	// Flames
	fFlameTime					= .1f;
	// Shell Drop
	OnShellDrop					(bHUD);
}

void CWeaponMagazined::OnShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R)
{
	pSounds->Play3DAtPos	(sndRicochet[Random.randI(SND_RIC_COUNT)], 0, vEnd,false);
	
	if (!R.O) 
	{
		// particles
		Fvector N,D;
		CDB::TRI* pTri		= pCreator->ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
		D.reflect			(vDir,N);
		
		CSector* S			= ::Render->getSector(pTri->sector);
		
		// smoke
		CPSObject* PS		= new CPSObject("smokepuffs_1",S,true);
		PS->m_Emitter.m_ConeDirection.set(D);
		PS->PlayAtPos		(vEnd);
		
		// stones
		PS					= new CPSObject("stones",S,true);
		PS->m_Emitter.m_ConeDirection.set(D);
		PS->PlayAtPos		(vEnd);
	}
}

void CWeaponMagazined::OnEmptyClick	(BOOL bHUDView)
{
	pSounds->Play3DAtPos	(sndEmptyClick,H_Root(),vLastFP);
}

void CWeaponMagazined::OnAnimationEnd()
{
	switch (st_current)
	{
	case eReload:	ReloadMagazine();		break;	// End of reload animation
	case eHiding:	signal_HideComplete();	break;	// End of Hide
	case eShowing:	st_target = eIdle;		break;	// End of Show
	}
}


void CWeaponMagazined::switch2_Idle	(BOOL bHUDView)
{
	m_pHUD->animPlay(mhud_idle);
}
void CWeaponMagazined::switch2_Fire	(BOOL bHUDView)
{
}
void CWeaponMagazined::switch2_Empty(BOOL bHUDView)
{
}
void CWeaponMagazined::switch2_Reload(BOOL bHUDView)
{
	pSounds->Play3DAtPos		(sndReload,H_Root(),vLastFP);
	m_pHUD->animPlay			(mhud_reload,TRUE,this);
}

void CWeaponMagazined::switch2_Hiding(BOOL bHUDView)
{
	switch2_Idle				(bHUDView);
	pSounds->Play3DAtPos		(sndHide,H_Root(),vLastFP);
	m_pHUD->animPlay			(mhud_hide,TRUE,this);
}

void CWeaponMagazined::switch2_Showing(BOOL bHUDView)
{
	pSounds->Play3DAtPos		(sndShow,H_Root(),vLastFP);
	m_pHUD->animPlay			(mhud_show,FALSE,this);
}

