#include "stdafx.h"
#include "grenade.h"
#include "PhysicsShell.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "..\PSObject.h"

CGrenade::CGrenade(void) {
	m_pFake = NULL;
	m_blast = 50.f;
	m_blastR = 10.f;
	m_frags = 20;
	m_fragsR = 30.f;
	m_fragHit = 50;
	m_eSoundExplode = ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);
	m_eSoundRicochet = ESoundTypes(SOUND_TYPE_WEAPON_BULLET_RICOCHET);
	m_expoldeTime = 0xffffffff;
}

CGrenade::~CGrenade(void) {
	if(hWallmark) Device.Shader.Delete(hWallmark);
}

void CGrenade::Load(LPCSTR section) {
	inherited::Load(section);
	m_blast = pSettings->r_float(section,"blast");
	m_blastR = pSettings->r_float(section,"blast_r");
	m_frags = pSettings->r_s32(section,"frags");
	m_fragsR = pSettings->r_float(section,"frags_r");
	m_fragHit = pSettings->r_float(section,"frag_hit");

	LPCSTR	name = pSettings->r_string(section,"wm_name");
	pstrWallmark = xr_strdup(name);
	fWallmarkSize = pSettings->r_float(section,"wm_size");

	SoundCreate(sndExplode, "explode", m_eSoundExplode);
	SoundCreate(sndRicochet[0], "ric1", m_eSoundRicochet);
	SoundCreate(sndRicochet[1], "ric2", m_eSoundRicochet);
	SoundCreate(sndRicochet[2], "ric3", m_eSoundRicochet);
	SoundCreate(sndRicochet[3], "ric4", m_eSoundRicochet);
	SoundCreate(sndRicochet[4], "ric5", m_eSoundRicochet);
}

BOOL CGrenade::net_Spawn(LPVOID DC) {
	if(0==pstrWallmark) hWallmark	= 0; 
	else hWallmark	= Device.Shader.Create("effects\\wallmark",pstrWallmark);
	return inherited::net_Spawn(DC);
}

void CGrenade::net_Destroy() {
	if(hWallmark) Device.Shader.Delete(hWallmark);
	SoundDestroy(sndExplode);
	SoundDestroy(sndRicochet[0]);
	SoundDestroy(sndRicochet[1]);
	SoundDestroy(sndRicochet[2]);
	SoundDestroy(sndRicochet[3]);
	SoundDestroy(sndRicochet[4]);
	inherited::net_Destroy();
}

void CGrenade::OnH_A_Chield() {
	inherited::OnH_A_Chield();
	if(!m_pFake && !dynamic_cast<CGrenade*>(H_Parent())) {
		xrServerEntity*		D	= F_entity_Create(cNameSect());
		R_ASSERT			(D);
		// Fill
		strcpy				(D->s_name,cNameSect());
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	(u16)ID();
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_ACTIVE | M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
}

void CGrenade::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	//if(dynamic_cast<CGrenade*>(H_Parent())) inherited::OnH_B_Independent();
	//else CInventoryItem::OnH_B_Independent();
}

bool CGrenade::Activate() {
	Show();
	return true;
}

void CGrenade::Deactivate() {
	Hide();
}

void CGrenade::Throw() {
	if(!m_pFake) return;
	inherited::Throw();
	CGrenade *l_pG = m_pFake;
	if(l_pG) {
		l_pG->m_destroyTime = 3500;
		l_pG->m_force = m_force; m_force = 0;
		{
			NET_Packet P;
			u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
			P.w_u16(u16(l_pG->ID()));
			u_EventSend(P);
		}
	}
}

void CGrenade::Destroy() {
	Explode();
	m_expoldeTime = 500;
	//inherited::Destroy();
}

void CGrenade::Explode() {
	setVisible(false);
	Sound->play_at_pos(sndExplode, 0, vPosition, false);
	Fvector l_dir; f32 l_dst;
	m_blasted.clear();
	feel_touch_update(vPosition, m_blastR);
	list<s16> l_elsemnts;
	list<Fvector> l_bs_positions;
	while(m_blasted.size()) {
		CGameObject *l_pGO = *m_blasted.begin();
		l_dir.sub(l_pGO->Position(), vPosition); l_dst = l_dir.magnitude(); l_dir.div(l_dst);
		f32 l_impuls = m_blast * (1.f - l_dst/m_blastR);
		if(l_impuls > .001f) {
			setEnabled(false);
			l_impuls *= l_pGO->ExplosionEffect(vPosition, m_blastR, l_elsemnts, l_bs_positions);
			setEnabled(true);
		}
		if(l_impuls > .001f) while(l_elsemnts.size()) {
			s16 l_element = *l_elsemnts.begin();
			Fvector l_bs_pos = *l_bs_positions.begin();
			NET_Packet		P;
			u_EventGen		(P,GE_HIT,l_pGO->ID());
			P.w_u16			(u16(ID()));
			P.w_dir			(l_dir);
			P.w_float		(0);
			P.w_s16			(l_element);
			P.w_vec3		(l_bs_pos);
			P.w_float		(l_impuls);
			u_EventSend		(P);
			l_elsemnts.pop_front();
			l_bs_positions.pop_front();
		}
		m_blasted.pop_front();
	}
	Collide::ray_query RQ;
	setEnabled(false);
	for(s32 i = 0; i < m_frags; i++) {
		l_dir.set(::Random.randF(-.5f,.5f), ::Random.randF(-.5f,.5f), ::Random.randF(-.5f,.5f)); l_dir.normalize();
		if(Level().ObjectSpace.RayPick(vPosition, l_dir, m_fragsR, RQ)) {
			Fvector l_end, l_bs_pos; l_end.mad(vPosition,l_dir,RQ.range); l_bs_pos.set(0, 0, 0);
			if(RQ.O) {
				f32 l_hit = m_fragHit * (1.f - RQ.range/m_fragsR);
				CEntity* E = dynamic_cast<CEntity*>(RQ.O);
				if(E) l_hit *= E->HitScale(RQ.element);
				NET_Packet		P;
				u_EventGen		(P,GE_HIT,RQ.O->ID());
				P.w_u16			(u16(ID()));
				P.w_dir			(l_dir);
				P.w_float		(l_hit);
				P.w_s16			(RQ.element);
				P.w_vec3		(l_bs_pos);
				P.w_float		(l_hit/(E?E->HitScale(RQ.element):1.f));
				u_EventSend		(P);
			}
			FragWallmark(l_dir, l_end, RQ);
		}
	}
	setEnabled(true);
}

void CGrenade::FragWallmark	(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R) {
	if (0==hWallmark)	return;
	
	if (R.O) {
		if (R.O->CLS_ID==CLSID_ENTITY)
		{
			IRender_Sector* S	= R.O->Sector();
			Fvector D;	D.invert(vDir);

			LPCSTR ps_gibs		= "blood_1";//(Random.randI(5)==0)?"sparks_1":"stones";
			CPSObject* PS		= xr_new<CPSObject> (ps_gibs,S,true);
			PS->m_Emitter.m_ConeDirection.set(D);
			PS->play_at_pos		(vEnd);
		}
	} else {
		R_ASSERT(R.element >= 0);
		::Render->add_Wallmark	(
			hWallmark,
			vEnd,
			fWallmarkSize,
			pCreator->ObjectSpace.GetStaticTris()+R.element/**/);
	}

	Sound->play_at_pos(sndRicochet[Random.randI(SND_RIC_COUNT)], 0, vEnd,false);
	
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

void CGrenade::feel_touch_new(CObject* O) {
	CGameObject *l_pGO = dynamic_cast<CGameObject*>(O);
	if(l_pGO && l_pGO != this) m_blasted.push_back(l_pGO);
}

bool CGrenade::Useful() {
	// ≈сли IItem еще не полностью использованый, вернуть true
	return m_destroyTime == 0xffffffff;
}

void CGrenade::OnEvent(NET_Packet& P, u16 type) {
	inherited::OnEvent(P,type);
	u16 id;
	P.r_u16(id);
	CGrenade *l_pG = dynamic_cast<CGrenade*>(Level().Objects.net_Find(id));
	if(l_pG) switch (type) {
		case GE_OWNERSHIP_TAKE : {
			m_pFake = l_pG;
			l_pG->H_SetParent(this);
		} break;
		case GE_OWNERSHIP_REJECT : {
			m_pFake = NULL;
			l_pG->H_SetParent(0);
		} break;
	}
}

void CGrenade::OnAnimationEnd() {
	switch(State()) {
		case MS_END : {
			m_pInventory->Ruck(this); 
			m_destroyTime = 0;
			NET_Packet P;
			u_EventGen(P, GE_OWNERSHIP_REJECT, H_Parent()->ID());
			P.w_u16(u16(ID()));
			u_EventSend(P);

			CGrenade *l_pNext = dynamic_cast<CGrenade*>(m_pInventory->Same(this));
			if(!l_pNext) l_pNext = dynamic_cast<CGrenade*>(m_pInventory->SameSlot(m_slot));
			if(l_pNext) { m_pInventory->Slot(l_pNext); m_pInventory->Activate(l_pNext->m_slot); }

		} break;
		default : inherited::OnAnimationEnd();
	}
}

void CGrenade::UpdateCL() {
	inherited::UpdateCL();
	if(m_expoldeTime > 0 && m_expoldeTime <= Device.dwTimeDelta) { m_expoldeTime = 0; inherited::Destroy(); }
	else if(m_expoldeTime < 0xffffffff) m_expoldeTime -= Device.dwTimeDelta;
}
