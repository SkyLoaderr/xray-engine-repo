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
CWeaponAK74::CWeaponAK74() : CWeaponMagazined("AK74")
{
	pSounds->Create3D(sndFireLoop,		"weapons\\AK74_fire");
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

CWeaponAK74::~CWeaponAK74()
{
	// sounds
	pSounds->Delete3D	(sndFireLoop	);
	pSounds->Delete3D	(sndEmptyClick	);
	for (int i=0; i<SND_RIC_COUNT; i++) pSounds->Delete3D(sndRicochet[i]);
}

void CWeaponAK74::Load	(CInifile* ini, const char* section){
	inherited::Load		(ini, section);
	R_ASSERT			(m_pHUD);
	
	iFlameDiv			= ini->ReadINT	(section,"flame_div");
	fFlameLength		= ini->ReadFLOAT(section,"flame_length");
	fFlameSize			= ini->ReadFLOAT(section,"flame_size");
}

void CWeaponAK74::MediaLOAD		()
{
	// flame textures
	LPCSTR S		= pSettings->ReadSTRING	(cName(),"flame");
	DWORD scnt		= _GetItemCount(S);
	string256 name;
	for (DWORD i=0; i<scnt; i++)
		hFlames.push_back(Device.Shader.Create("particles\\add",_GetItem(S,i,name),false));
}

void CWeaponAK74::MediaUNLOAD	()
{
	for (DWORD i=0; i<hFlames.size(); i++)
		Device.Shader.Delete(hFlames[i]);
	hFlames.clear();
}

void CWeaponAK74::switch2_Idle	(BOOL bHUDView)
{
	if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
	if (bHUDView)	Level().Cameras.RemoveEffector	(cefShot);
}
void CWeaponAK74::switch2_Fire	(BOOL bHUDView)
{
	if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
	pSounds->Play3DAtPos(sndFireLoop,vLastFP,true);
}
void CWeaponAK74::switch2_Empty	(BOOL bHUDView)
{
	if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
	if (bHUDView)	Level().Cameras.RemoveEffector	(cefShot);
}
void CWeaponAK74::switch2_Reload(BOOL bHUDView)
{
}
void CWeaponAK74::OnShot		(BOOL bHUDView)
{
	if (bHUDView)	{
		CEffectorShot*	S = dynamic_cast<CEffectorShot*>(Level().Cameras.GetEffector(cefShot));
		if (S)			S->Shot();
	}
}
void CWeaponAK74::OnEmptyClick	(BOOL bHUDView)
{
	pSounds->Play3DAtPos	(sndEmptyClick,vLastFP);
}
void CWeaponAK74::OnDrawFlame	(BOOL bHUDView)
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
void CWeaponAK74::OnShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R)
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
void CWeaponAK74::Update		(float dt, BOOL bHUDView)
{
	// sound fire loop
	inherited::Update			(dt,bHUDView);
	if (sndFireLoop.feedback) sndFireLoop.feedback->SetPosition(vLastFP);
}
