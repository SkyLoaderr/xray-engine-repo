#include "stdafx.h"
#include "effectorshot.h"
#include "..\xr_tokens.h"
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
CWeaponMagazined::CWeaponMagazined(LPCSTR name, ESoundTypes eSoundType) : CWeapon(name)
{
	m_eSoundShow		= ESoundTypes(SOUND_TYPE_WEAPON_CHANGING | eSoundType);
	m_eSoundHide		= ESoundTypes(SOUND_TYPE_WEAPON_HIDING | eSoundType);
	m_eSoundShot		= ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING | eSoundType);
	m_eSoundEmptyClick	= ESoundTypes(SOUND_TYPE_WEAPON_EMPTY_CLICKING | eSoundType);
	m_eSoundReload		= ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING | eSoundType);
	m_eSoundRicochet	= ESoundTypes(SOUND_TYPE_WEAPON_BULLET_RICOCHET | eSoundType);
	fTime				= 0;
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

void CWeaponMagazined::Load	(LPCSTR section)
{
	inherited::Load		(section);
	bFlame				= FALSE;
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
	animGet				(mhud_idle,		"idle");
	animGet				(mhud_reload,	"reload");
	animGet				(mhud_show,		"draw");
	animGet				(mhud_hide,		"holster");
	animGet				(mhud_shots,	"shoot");
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

void CWeaponMagazined::FireStart		()
{
	if (IsValid())
	{
		if (!IsWorking())
		{
			if (STATE==eReload)			return;
			if (STATE==eShowing)		return;
			if (STATE==eHiding)			return;

			if (!iAmmoElapsed && iAmmoCurrent)	
			{
				CWeapon::FireStart	();
				SwitchState			(eMagEmpty);
			}
			else							
			{
				CWeapon::FireStart	();
				SwitchState			(eFire);
			}
		}
	}else{
		if (!iAmmoElapsed && !iAmmoCurrent)	
			SwitchState			(eMagEmpty);
	}
}

void CWeaponMagazined::FireEnd			()
{
	if (IsWorking())
	{
		CWeapon::FireEnd	();
		if (iAmmoCurrent && !iAmmoElapsed)	TryReload	();
		else								SwitchState (eIdle);
	}
}

void CWeaponMagazined::Reload			()
{
	TryReload			();
}

void CWeaponMagazined::TryReload		()
{
	if (iAmmoCurrent && (iAmmoElapsed<iMagazineSize))	SwitchState(eReload);
	else												SwitchState(eIdle);
}

void CWeaponMagazined::OnMagazineEmpty	()
{
	SwitchState			(eMagEmpty);
}

void CWeaponMagazined::ReloadMagazine	()
{
	SwitchState			(eIdle);
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

void CWeaponMagazined::OnStateSwitch	(u32 S)
{
	switch (S)
	{
	case eIdle:
//		bFlame			= FALSE;
		switch2_Idle	();
		break;
	case eFire:
		switch2_Fire	();
		break;
	case eMagEmpty:
//		bFlame			= FALSE;
		switch2_Empty	();
		break;
	case eReload:
		bFlame			= FALSE;
		switch2_Reload	();
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
	}
	STATE = S;
}

void CWeaponMagazined::UpdateCL			()
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
	case eIdle:
		fTime			-=	dt;
		if (fTime<0)	fTime = 0;
		break;
	case eFire:			state_Fire		(dt);	break;
	case eMagEmpty:		state_MagEmpty	(dt);	break;
	case eHidden:		break;
	}
	UpdateSounds		();
	bPending			= FALSE;
}

void CWeaponMagazined::UpdateSounds	()
{
	// sound positions
	if (sndShow.feedback || sndHide.feedback || sndShot.feedback || sndReload.feedback || sndEmptyClick.feedback)
	{
		UpdateFP					();
		if (sndShow.feedback)		sndShow.feedback->SetPosition		(vLastFP);
		if (sndHide.feedback)		sndHide.feedback->SetPosition		(vLastFP);
		if (sndShot.feedback)		sndShot.feedback->SetPosition		(vLastFP);
		if (sndReload.feedback)		sndReload.feedback->SetPosition		(vLastFP);
		if (sndEmptyClick.feedback)	sndEmptyClick.feedback->SetPosition	(vLastFP);
	}
}

void CWeaponMagazined::state_Fire	(float dt)
{
	UpdateFP				();
	fTime					-=dt;
	Fvector					p1, d;
	dynamic_cast<CEntity*>	(H_Parent())->g_fireParams	(p1,d);
	
	while (fTime<0)
	{
		bFlame			=	TRUE;
		fTime			+=	fTimeToFire;
		
		OnShot			();
		FireTrace		(p1,vLastFP,d);
	}
	UpdateSounds			();
}

void CWeaponMagazined::state_MagEmpty	(float dt)
{
	UpdateFP				();
	OnEmptyClick			();
	SwitchState				(eIdle);
	UpdateSounds			();
}

void CWeaponMagazined::OnVisible	()
{
	inherited::OnVisible	();
	UpdateXForm				();
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
	if (((eFire==STATE) || (eReload==STATE))&& bFlame) 
	{
		UpdateFP	();
		OnDrawFlame	();
	}
}

void CWeaponMagazined::SetDefaults	()
{
	CWeapon::SetDefaults		();
	iAmmoElapsed				= 0;
}

void CWeaponMagazined::Hide		()
{
	// add shot effector
	SwitchState						(eHiding);
}
void CWeaponMagazined::Show		()
{
	SwitchState						(eShowing);
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
	LPCSTR S		= pSettings->ReadSTRING	(cNameSect(),"flame");
	u32 scnt		= _GetItemCount(S);
	string256		name;
	for (u32 i=0; i<scnt; i++)
	{
		Shader* SH			= 0;
		ShaderCreate		(SH,"effects\\flame",_GetItem(S,i,name));
		hFlames.push_back	(SH);
	}
}

void CWeaponMagazined::MediaUNLOAD	()
{
	for (u32 i=0; i<hFlames.size(); i++)
		ShaderDestroy(hFlames[i]);
	hFlames.clear();
}

void CWeaponMagazined::OnShellDrop	()
{
	// shells
	/*
	CPSObject* PS				= new CPSObject("weapons\\shells_generic",H_Parent()->Sector(),true);
	Fmatrix M;
	M.setHPB(PS->m_Emitter.m_ConeHPB.x, PS->m_Emitter.m_ConeHPB.y, PS->m_Emitter.m_ConeHPB.z);
	Fvector V; M.transform_dir(V,vLastFD);
	PS->m_Emitter.m_ConeDirection.set(V);
	PS->PlayAtPos				(vLastSP);
	*/
}

void CWeaponMagazined::OnShot		()
{
	// Sound
	Sound->PlayAtPos			(sndShot,H_Root(),vLastFP);

	// Camera
	if (hud_mode)	
	{
		CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
		if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(new CEffectorShot(camMaxAngle,camRelaxSpeed));
		R_ASSERT				(S);
		S->Shot					(camDispersion);
	}
	
	// Animation
	m_pHUD->animPlay			(mhud_shots[Random.randI(mhud_shots.size())],FALSE);
	
	// Flames
	fFlameTime					= .1f;
	
	// Shell Drop
	OnShellDrop					();
}

void CWeaponMagazined::OnShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R)
{
	Sound->PlayAtPos		(sndRicochet[Random.randI(SND_RIC_COUNT)], 0, vEnd,false);
	
	if (!R.O) 
	{
		// particles
		Fvector N,D;
		CDB::TRI* pTri		= pCreator->ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
		D.reflect			(vDir,N);
		
		CSector* S			= ::Render->getSector(pTri->sector);
		
		// smoke
		LPCSTR ps_gibs		= (Random.randI(5)==0)?"sparks_1":"stones";
		CPSObject* PS		= new CPSObject(ps_gibs,S,true);
		PS->m_Emitter.m_ConeDirection.set(D);
		PS->PlayAtPos		(vEnd);
		
		// stones
		PS					= new CPSObject("stones",S,true);
		PS->m_Emitter.m_ConeDirection.set(D);
		PS->PlayAtPos		(vEnd);
	}
}

void CWeaponMagazined::OnEmptyClick	()
{
	Sound->PlayAtPos	(sndEmptyClick,H_Root(),vLastFP);
}
void CWeaponMagazined::OnAnimationEnd()
{
	switch (STATE)
	{
	case eReload:	ReloadMagazine();		break;	// End of reload animation
	case eHiding:	SwitchState(eHidden);	break;	// End of Hide
	case eShowing:	SwitchState(eIdle);		break;	// End of Show
	}
}
void CWeaponMagazined::switch2_Idle	()
{
	m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
}
void CWeaponMagazined::switch2_Fire	()
{
}
void CWeaponMagazined::switch2_Empty()
{
}
void CWeaponMagazined::switch2_Reload()
{
	Sound->PlayAtPos		(sndReload,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_reload[Random.randI(mhud_reload.size())],TRUE,this);
}
void CWeaponMagazined::switch2_Hiding()
{
	FireEnd					();
	bPending				= TRUE;
	Sound->PlayAtPos		(sndHide,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
	if (Local())			Level().Cameras.RemoveEffector	(cefShot);
}
void CWeaponMagazined::switch2_Hidden()
{
	signal_HideComplete		();
}
void CWeaponMagazined::switch2_Showing()
{
	setVisible				(TRUE);
	Sound->PlayAtPos		(sndShow,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}
