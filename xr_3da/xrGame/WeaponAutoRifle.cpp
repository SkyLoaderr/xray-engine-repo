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
CWeaponAutoRifle::CWeaponAutoRifle(LPCSTR name) : CWeaponMagazined(name)
{
	iFlameDiv		= 0;
	fFlameLength	= 0;
	fFlameSize		= 0;
}

CWeaponAutoRifle::~CWeaponAutoRifle()
{
	// sounds
	SoundDestroy		(sndFireLoop	);
	SoundDestroy		(sndEmptyClick	);
	SoundDestroy		(sndReload		);
	SoundDestroy		(sndRicochet[0]	);
	SoundDestroy		(sndRicochet[1]	);
	SoundDestroy		(sndRicochet[2]	);
	SoundDestroy		(sndRicochet[3]	);
	SoundDestroy		(sndRicochet[4]	);
}

void CWeaponAutoRifle::Load	(CInifile* ini, const char* section)
{
	inherited::Load		(ini, section);
	
	iFlameDiv			= ini->ReadINT	(section,"flame_div");
	fFlameLength		= ini->ReadFLOAT(section,"flame_length");
	fFlameSize			= ini->ReadFLOAT(section,"flame_size");

	// Sounds
	SoundCreate			(sndFireLoop,	"fire");
	SoundCreate			(sndEmptyClick,	"empty");
	SoundCreate			(sndReload,		"reload");
	SoundCreate			(sndRicochet[0],"ric1");
	SoundCreate			(sndRicochet[1],"ric2");
	SoundCreate			(sndRicochet[2],"ric3");
	SoundCreate			(sndRicochet[3],"ric4");
	SoundCreate			(sndRicochet[4],"ric5");
	
	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	mhud_idle			= m_pHUD->animGet("idle");
	mhud_reload			= m_pHUD->animGet("reload");
	mhud_show			= m_pHUD->animGet("draw");
	mhud_hide			= m_pHUD->animGet("holster");
	mhud_shots.push_back( m_pHUD->animGet("shoot0"));
	mhud_shots.push_back( m_pHUD->animGet("shoot1"));
	mhud_shots.push_back( m_pHUD->animGet("shoot2"));
}

void CWeaponAutoRifle::MediaLOAD		()
{
	// flame textures
	LPCSTR S		= pSettings->ReadSTRING	(cName(),"flame");
	DWORD scnt		= _GetItemCount(S);
	string256 name;
	for (DWORD i=0; i<scnt; i++)
		hFlames.push_back(Device.Shader.Create("particles\\add",_GetItem(S,i,name),false));
}

void CWeaponAutoRifle::MediaUNLOAD	()
{
	for (DWORD i=0; i<hFlames.size(); i++)
		Device.Shader.Delete(hFlames[i]);
	hFlames.clear();
}

void CWeaponAutoRifle::switch2_Idle	(BOOL bHUDView)
{
	if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
	if (bHUDView)	Level().Cameras.RemoveEffector	(cefShot);
	m_pHUD->animPlay(mhud_idle);
}
void CWeaponAutoRifle::switch2_Fire	(BOOL bHUDView)
{
	if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
	pSounds->Play3DAtPos(sndFireLoop,vLastFP,true);
}
void CWeaponAutoRifle::switch2_Empty	(BOOL bHUDView)
{
	if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
	if (bHUDView)	Level().Cameras.RemoveEffector	(cefShot);
}
void CWeaponAutoRifle::switch2_Reload(BOOL bHUDView)
{
	pSounds->Play3DAtPos		(sndReload,vLastFP);
	m_pHUD->animPlay			(mhud_reload,TRUE,this);
}
void CWeaponAutoRifle::OnShot		(BOOL bHUDView)
{
	if (bHUDView)	{
		CEffectorShot*	S = dynamic_cast<CEffectorShot*>(Level().Cameras.GetEffector(cefShot));
		if (S)			S->Shot();
	}
	m_pHUD->animPlay	(mhud_shots[Random.randI(mhud_shots.size())],FALSE);
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
	case eReload:	ReloadMagazine();	break;	// End of reload animation
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
	if (sndFireLoop.feedback)	sndFireLoop.feedback->SetPosition(vLastFP);
	if (sndReload.feedback)		sndReload.feedback->SetPosition(vLastFP);
}
