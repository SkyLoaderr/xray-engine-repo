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
#include "WeaponFN2000.h"
#include "entity.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponFN2000::CWeaponFN2000() : CWeaponMagazined("FN2000")
{
	pSounds->Create3D(sndFireLoop,		"weapons\\FN2000_fire");
	pSounds->Create3D(sndEmptyClick,	"weapons\\generic_empty");
	pSounds->Create3D(sndReload,		"weapons\\generic_reload");
	pSounds->Create3D(sndRicochet[0],	"weapons\\ric1");
	pSounds->Create3D(sndRicochet[1],	"weapons\\ric2");
	pSounds->Create3D(sndRicochet[2],	"weapons\\ric3");
	pSounds->Create3D(sndRicochet[3],	"weapons\\ric4");
	pSounds->Create3D(sndRicochet[4],	"weapons\\ric5");
	
	iFlameDiv		= 0;
	fFlameLength	= 0;
	fFlameSize		= 0;
}

CWeaponFN2000::~CWeaponFN2000()
{
	// sounds
	pSounds->Delete3D	(sndFireLoop	);
	pSounds->Delete3D	(sndEmptyClick	);
	pSounds->Delete3D	(sndReload		);
	for (int i=0; i<SND_RIC_COUNT; i++) pSounds->Delete3D(sndRicochet[i]);
}

void CWeaponFN2000::Load	(CInifile* ini, const char* section){
	inherited::Load		(ini, section);
	
	iFlameDiv			= ini->ReadINT	(section,"flame_div");
	fFlameLength		= ini->ReadFLOAT(section,"flame_length");
	fFlameSize			= ini->ReadFLOAT(section,"flame_size");
	
	R_ASSERT			(m_pHUD);
	mhud_idle			= m_pHUD->animGet("idle");
	mhud_reload			= m_pHUD->animGet("reload");
	mhud_show			= m_pHUD->animGet("draw");
	mhud_hide			= m_pHUD->animGet("holster");
	mhud_shots.push_back( m_pHUD->animGet("shoot0"));
	mhud_shots.push_back( m_pHUD->animGet("shoot1"));
	mhud_shots.push_back( m_pHUD->animGet("shoot2"));
}

void CWeaponFN2000::MediaLOAD		()
{
	// flame textures
	LPCSTR S		= pSettings->ReadSTRING	(cName(),"flame");
	DWORD scnt		= _GetItemCount(S);
	string256 name;
	for (DWORD i=0; i<scnt; i++)
		hFlames.push_back(Device.Shader.Create("particles\\add",_GetItem(S,i,name),false));
}

void CWeaponFN2000::MediaUNLOAD	()
{
	for (DWORD i=0; i<hFlames.size(); i++)
		Device.Shader.Delete(hFlames[i]);
	hFlames.clear();
}

void CWeaponFN2000::switch2_Idle	(BOOL bHUDView)
{
	if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
	if (bHUDView)	Level().Cameras.RemoveEffector	(cefShot);
	m_pHUD->animPlay(mhud_idle);
}
void CWeaponFN2000::switch2_Fire	(BOOL bHUDView)
{
	if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
	pSounds->Play3DAtPos(sndFireLoop,vLastFP,true);
}
void CWeaponFN2000::switch2_Empty	(BOOL bHUDView)
{
	if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
	if (bHUDView)	Level().Cameras.RemoveEffector	(cefShot);
}
void CWeaponFN2000::switch2_Reload(BOOL bHUDView)
{
	pSounds->Play3DAtPos		(sndReload,vLastFP);
	m_pHUD->animPlay			(mhud_reload,TRUE,this);
}
void CWeaponFN2000::OnShot		(BOOL bHUDView)
{
	if (bHUDView)	{
		CEffectorShot*	S = dynamic_cast<CEffectorShot*>(Level().Cameras.GetEffector(cefShot));
		if (S)			S->Shot();
	}
	m_pHUD->animPlay	(mhud_shots[Random.randI(mhud_shots.size())],FALSE);
}
void CWeaponFN2000::OnEmptyClick	(BOOL bHUDView)
{
	pSounds->Play3DAtPos	(sndEmptyClick,vLastFP);
}
void CWeaponFN2000::OnDrawFlame	(BOOL bHUDView)
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
void CWeaponFN2000::OnAnimationEnd()
{
	switch (st_current)
	{
	case eReload:	ReloadMagazine();	break;	// End of reload animation
	}
}
void CWeaponFN2000::OnShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R)
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
void CWeaponFN2000::Update		(float dt, BOOL bHUDView)
{
	// sound fire loop
	inherited::Update			(dt,bHUDView);
	if (sndFireLoop.feedback)	sndFireLoop.feedback->SetPosition(vLastFP);
	if (sndReload.feedback)		sndReload.feedback->SetPosition(vLastFP);
}
