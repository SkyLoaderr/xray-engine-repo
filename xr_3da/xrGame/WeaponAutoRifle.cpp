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
#include "WeaponAutoRifle.h"
#include "entity.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponAutoRifle::CWeaponAutoRifle(LPCSTR name) : CWeaponMagazined(name)
{
	iFlameDiv		= 0;
	fFlameLength	= 0;
	fFlameSize		= 0;
}

CWeaponAutoRifle::~CWeaponAutoRifle()
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
	
	MediaUNLOAD			();
}

void CWeaponAutoRifle::Load	(CInifile* ini, const char* section)
{
	inherited::Load		(ini, section);
	
	iFlameDiv			= ini->ReadINT	(section,"flame_div");
	fFlameLength		= ini->ReadFLOAT(section,"flame_length");
	fFlameSize			= ini->ReadFLOAT(section,"flame_size");

	// Sounds
	SoundCreate			(sndShow,		"draw");
	SoundCreate			(sndHide,		"holster");
	SoundCreate			(sndShot,		"shoot");
	SoundCreate			(sndEmptyClick,	"empty");
	SoundCreate			(sndReload,		"reload");
	SoundCreate			(sndRicochet[0],"ric1");
	SoundCreate			(sndRicochet[1],"ric2");
	SoundCreate			(sndRicochet[2],"ric3");
	SoundCreate			(sndRicochet[3],"ric4");
	SoundCreate			(sndRicochet[4],"ric5");
	
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

void CWeaponAutoRifle::MediaLOAD		()
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

void CWeaponAutoRifle::MediaUNLOAD	()
{
	for (DWORD i=0; i<hFlames.size(); i++)
		ShaderDestroy(hFlames[i]);
	hFlames.clear();
}

void CWeaponAutoRifle::switch2_Idle	(BOOL bHUDView)
{
	if (bHUDView)	Level().Cameras.RemoveEffector	(cefShot);
	m_pHUD->animPlay(mhud_idle);
}
void CWeaponAutoRifle::switch2_Fire	(BOOL bHUDView)
{
}
void CWeaponAutoRifle::switch2_Empty	(BOOL bHUDView)
{
	if (bHUDView)	Level().Cameras.RemoveEffector	(cefShot);
}
void CWeaponAutoRifle::switch2_Reload(BOOL bHUDView)
{
	pSounds->Play3DAtPos		(sndReload,vLastFP);
	m_pHUD->animPlay			(mhud_reload,TRUE,this);
}
void CWeaponAutoRifle::switch2_Hiding(BOOL bHUDView)
{
	switch2_Idle				(bHUDView);
	pSounds->Play3DAtPos		(sndHide,vLastFP);
	m_pHUD->animPlay			(mhud_hide,TRUE,this);
}
void CWeaponAutoRifle::switch2_Showing(BOOL bHUDView)
{
	pSounds->Play3DAtPos		(sndShow,vLastFP);
	m_pHUD->animPlay			(mhud_show,FALSE,this);
}
void CWeaponAutoRifle::OnShot		(BOOL bHUDView)
{
	if (bHUDView)	{
		CEffectorShot*	S = dynamic_cast<CEffectorShot*>(Level().Cameras.GetEffector(cefShot));
		if (S)			S->Shot();
	}
	m_pHUD->animPlay	(mhud_shots[Random.randI(mhud_shots.size())],FALSE);
	pSounds->Play3DAtPos(sndShot,vLastFP);
}
void CWeaponAutoRifle::OnEmptyClick	(BOOL bHUDView)
{
	pSounds->Play3DAtPos	(sndEmptyClick,vLastFP);
}
void CWeaponAutoRifle::OnDrawFlame	(BOOL bHUDView)
{
	if (bHUDView &&	(0==Level().Cameras.GetEffector(cefShot)))	Level().Cameras.AddEffector(new CEffectorShot(camRelax,camDispersion));
	
	// fire flash
	Fvector P = vLastFP;
	Fvector D; D.mul(vLastFD,::Random.randF(fFlameLength)/float(iFlameDiv));
	float f = fFlameSize;
	for (int i=0; i<iFlameDiv; i++){
		f*=0.9f;
		float	S = f+f*::Random.randF	();
		float	A = ::Random.randF		(PI_MUL_2);
		::Render.add_Patch				(hFlames[Random.randI(hFlames.size())],P,S,A,bHUDView);
		P.add(D);
	}
}

void CWeaponAutoRifle::OnAnimationEnd()
{
	switch (st_current)
	{
	case eReload:	ReloadMagazine();		break;	// End of reload animation
	case eHiding:	signal_HideComplete();	break;	// End of Hide
	case eShowing:	st_target = eIdle;		break;	// End of Show
	}
}

void CWeaponAutoRifle::OnShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R)
{
	pSounds->Play3DAtPos	(sndRicochet[Random.randI(SND_RIC_COUNT)], vEnd,false);
	
	// particles
	Fvector N,D;
	RAPID::tri* pTri	= pCreator->ObjectSpace.GetStaticTris()+R.element;
	N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
	D.reflect			(vDir,N);
	
	CSector* S			= ::Render.getSector(pTri->sector);
	
	// stones
	CPSObject* PS		= new CPSObject("stones",S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->PlayAtPos		(vEnd);
	
	// smoke
	PS					= new CPSObject("smokepuffs_1",S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->PlayAtPos		(vEnd);
}
void CWeaponAutoRifle::Update		(float dt, BOOL bHUDView)
{
	// sound fire loop
	inherited::Update			(dt,bHUDView);
	UpdateFP					(bHUDView);
	if (sndShow.feedback)		sndShow.feedback->SetPosition		(vLastFP);
	if (sndHide.feedback)		sndHide.feedback->SetPosition		(vLastFP);
	if (sndShot.feedback)		sndShot.feedback->SetPosition		(vLastFP);
	if (sndReload.feedback)		sndReload.feedback->SetPosition		(vLastFP);
	if (sndEmptyClick.feedback)	sndEmptyClick.feedback->SetPosition	(vLastFP);
}
