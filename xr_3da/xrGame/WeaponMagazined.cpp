#include "stdafx.h"
#include "effectorshot.h"
#include "..\PSObject.h"
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
	MediaUNLOAD		();
}

void CWeaponMagazined::net_Destroy()
{
	inherited::net_Destroy();

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

	MediaLOAD	();
}

void CWeaponMagazined::FireStart		()
{
	if(IsValid()) {
		if(!IsWorking()) {
			if(STATE==eReload) return;
			if(STATE==eShowing) return;
			if(STATE==eHiding) return;

			if (!iAmmoElapsed) {
				CWeapon::FireStart();
				SwitchState(eMagEmpty);
			} else {
				CWeapon::FireStart();
				SwitchState(eFire);
			}
		}
	} else {
		//if (1/*!iAmmoElapsed && !iAmmoCurrent*/)	
		//	SwitchState			(eMagEmpty);
	}
}

void CWeaponMagazined::FireEnd() {
	if(IsWorking()) {
		CWeapon::FireEnd();
	}
	if(!iAmmoElapsed) TryReload();
	else SwitchState(eIdle);
}

void CWeaponMagazined::Reload() {
	TryReload();
}

void CWeaponMagazined::TryReload() {
	if(m_pInventory) {
		m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(m_ammoTypes[m_ammoType]));
		if(m_pAmmo) {
			SwitchState(eReload); return;
		} else for(u32 i = 0; i < m_ammoTypes.size(); i++) {
			m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(m_ammoTypes[i]));
			if(m_pAmmo) { m_ammoType = i; SwitchState(eReload); return; }
		}
	}
	SwitchState(eIdle);

	////if(m_pInventory) for(PPIItem l_ppIItem = m_pInventory->m_belt.begin(); l_ppIItem != m_pInventory->m_belt.end(); l_ppIItem++) {
	////	//for(u32 i = 0; i < m_ammoTypes.size(); i++) if(!strcmp((*l_ppIItem)->cName(), m_ammoTypes[i])) {
	////	if(!strcmp((*l_ppIItem)->cName(), m_ammoTypes[m_ammoType])) {
	////		SwitchState(eReload);
	////		return;
	////	}
	////}
	////SwitchState(eIdle);
	//if(m_pAmmo && m_pAmmo->CanReload()) SwitchState(eReload);
	//else SwitchState(eIdle);
}

void CWeaponMagazined::OnMagazineEmpty() {
	SwitchState(eMagEmpty);
}

void CWeaponMagazined::UnloadMagazine() {
	map<LPCSTR, u16> l_ammo;
	while(m_magazine.size()) {
		CCartridge &l_cartridge = m_magazine.top();
		map<LPCSTR, u16>::iterator l_it;
		for(l_it = l_ammo.begin(); l_it != l_ammo.end(); l_it++) if(!strcmp(l_cartridge.m_ammoSect, l_it->first)) { l_it->second++; break; }
		if(l_it == l_ammo.end()) l_ammo[l_cartridge.m_ammoSect] = 1;
		m_magazine.pop(); iAmmoElapsed--;
	}
	map<LPCSTR, u16>::iterator l_it;
	for(l_it = l_ammo.begin(); l_it != l_ammo.end(); l_it++) {
		CWeaponAmmo *l_pA = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(l_it->first));
		if(l_pA) {
			u16 l_free = l_pA->m_boxSize - l_pA->m_boxCurr;
			l_pA->m_boxCurr = l_pA->m_boxCurr + (l_free < l_it->second ? l_free : l_it->second);
			l_it->second = l_it->second - (l_free < l_it->second ? l_free : l_it->second);
		}
		if(l_it->second) SpawnAmmo(l_it->second, l_it->first);
	}
}

void CWeaponMagazined::ReloadMagazine() {
	SwitchState(eIdle);
	static l_lockType = false;
	if(!l_lockType) m_ammoName = NULL;
	if(m_pInventory) {
		m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(m_ammoTypes[m_ammoType]));
		if(!m_pAmmo && !l_lockType) for(u32 i = 0; i < m_ammoTypes.size(); i++) {
			m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(m_ammoTypes[i]));
			if(m_pAmmo) { m_ammoType = i; break; }
		}
	}
	if(!l_lockType && m_magazine.size() && (!m_pAmmo || strcmp(m_pAmmo->cName(), m_magazine.top().m_ammoSect))) UnloadMagazine();
	if(m_pAmmo) {
		CCartridge l_cartridge;
		while(iAmmoElapsed < iMagazineSize && m_pAmmo->Get(l_cartridge)) {
			iAmmoElapsed++;
			m_magazine.push(l_cartridge);
		}
		m_ammoName = m_pAmmo->m_nameShort;
		if(!m_pAmmo->m_boxCurr) m_pAmmo->Drop();
		//	NET_Packet				P;
		//	u_EventGen				(P,GE_OWNERSHIP_REJECT,H_Parent()->ID());
		//	P.w_u16					(u16(m_pAmmo->ID()));
		//	u_EventSend				(P);
		//	//m_pInventory->Drop(m_pAmmo);
		//	m_pAmmo = NULL;
		//}
		if(iMagazineSize > iAmmoElapsed) { l_lockType = true; ReloadMagazine(); l_lockType = false; }
	}
}

void CWeaponMagazined::OnStateSwitch	(u32 S)
{
	switch (S)
	{
	case eIdle:
		bFlame			= FALSE;
		switch2_Idle	();
		break;
	case eFire:
		switch2_Fire	();
		break;
	case eFire2:
		switch2_Fire2	();
		break;
	case eMagEmpty:
		//bFlame			= FALSE;
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
	if(Remote()) NEXT_STATE = S;
}

void CWeaponMagazined::UpdateCL			()
{
	inherited::UpdateCL	();
	float dt			= Device.fTimeDelta;
	
	// cycle update
	switch (NEXT_STATE)
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
		if (sndShow.feedback)		sndShow.set_position		(vLastFP);
		if (sndHide.feedback)		sndHide.set_position		(vLastFP);
		if (sndShot.feedback)		sndShot.set_position		(vLastFP);
		if (sndReload.feedback)		sndReload.set_position		(vLastFP);
		if (sndEmptyClick.feedback)	sndEmptyClick.set_position	(vLastFP);
	}
}

void CWeaponMagazined::state_Fire	(float dt)
{
	UpdateFP				();
	fTime					-=dt;
	Fvector					p1, d; p1.set(vLastFP); d.set(vLastFD);
	if(H_Parent()) dynamic_cast<CEntity*>	(H_Parent())->g_fireParams	(p1,d);
	else return;
	
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
	////iAmmoElapsed				= 0;
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
	LPCSTR S		= pSettings->r_string	(cNameSect(),"flame");
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
	CPSObject* PS				= xr_new<CPSObject>("weapons\\shells_generic",H_Parent()->Sector(),true);
	Fmatrix M;
	M.setHPB(PS->m_Emitter.m_ConeHPB.x, PS->m_Emitter.m_ConeHPB.y, PS->m_Emitter.m_ConeHPB.z);
	Fvector V; M.transform_dir(V,vLastFD);
	PS->m_Emitter.m_ConeDirection.set(V);
	PS->play_at_pos				(vLastSP);
	*/
}

void CWeaponMagazined::OnShot		()
{
	// Sound
	Sound->play_at_pos			(sndShot,H_Root(),vLastFP);

	// Camera
	if (hud_mode)	
	{
		CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
		if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot> (camMaxAngle,camRelaxSpeed));
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
	Sound->play_at_pos		(sndRicochet[Random.randI(SND_RIC_COUNT)], 0, vEnd,false);
	
	if (!R.O) 
	{
		// particles
		Fvector N,D;
		CDB::TRI* pTri		= pCreator->ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
		D.reflect			(vDir,N);
		
		IRender_Sector* S	= ::Render->getSector(pTri->sector);
		
		// smoke
		LPCSTR ps_gibs		= (Random.randI(5)==0)?"sparks_1":"stones";
		CPSObject* PS		= xr_new<CPSObject> (ps_gibs,S,true);
		PS->m_Emitter.m_ConeDirection.set(D);
		PS->play_at_pos		(vEnd);
		
		// stones
		PS					= xr_new<CPSObject> ("stones",S,true);
		PS->m_Emitter.m_ConeDirection.set(D);
		PS->play_at_pos		(vEnd);
	}
}

void CWeaponMagazined::OnEmptyClick	()
{
	Sound->play_at_pos	(sndEmptyClick,H_Root(),vLastFP);
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
	Sound->play_at_pos		(sndReload,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_reload[Random.randI(mhud_reload.size())],TRUE,this);
}
void CWeaponMagazined::switch2_Hiding()
{
	CWeapon::FireEnd					();
	bPending				= TRUE;
	Sound->play_at_pos		(sndHide,H_Root(),vLastFP);
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
	Sound->play_at_pos		(sndShow,H_Root(),vLastFP);
	m_pHUD->animPlay		(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}

bool CWeaponMagazined::Action(s32 cmd, u32 flags) {
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) {
		case kWPN_RELOAD : {
			if(flags&CMD_START) TryReload();//if(m_pAmmo && m_pAmmo->CanReload()) m_pAmmo->Reload();
		} return true;
	}
	return false;
}
