#include "stdafx.h"
#include "weaponrpg7.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "extendedgeom.h"
#include "..\PSObject.h"
#include "..\PGObject.h"

CWeaponRPG7Grenade::CWeaponRPG7Grenade() {
	m_pos.set(0, 0, 0); m_vel.set(0, 0, 0);
	m_pOwner = NULL;
	m_eSoundExplode = ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);
	m_eSoundRicochet = ESoundTypes(SOUND_TYPE_WEAPON_BULLET_RICOCHET);
	m_pLight = ::Render->light_create();
	m_pLight->set_shadow(true);
	m_expoldeTime = m_engineTime = 0xffffffff;
}

CWeaponRPG7Grenade::~CWeaponRPG7Grenade() {
	if(hWallmark) Device.Shader.Delete(hWallmark);
	::Render->light_destroy(m_pLight);
}

void __stdcall CWeaponRPG7Grenade::ObjectContactCallback(bool& do_colide,dContact& c) {
	dxGeomUserData *l_pUD1 = NULL;
	dxGeomUserData *l_pUD2 = NULL;
	if(dGeomGetClass(c.geom.g1)==dGeomTransformClass) {
		const dGeomID geom=dGeomTransformGetGeom(c.geom.g1);
		l_pUD1 = dGeomGetUserData(geom);
	} else l_pUD1 = dGeomGetUserData(c.geom.g1);
	if(dGeomGetClass(c.geom.g2)==dGeomTransformClass) {
		const dGeomID geom=dGeomTransformGetGeom(c.geom.g2);
		l_pUD2 = dGeomGetUserData(geom);
	} else l_pUD2 = dGeomGetUserData(c.geom.g2);

	CWeaponRPG7Grenade *l_this = l_pUD1 ? dynamic_cast<CWeaponRPG7Grenade*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_this) l_this = l_pUD2 ? dynamic_cast<CWeaponRPG7Grenade*>(l_pUD2->ph_ref_object) : NULL;
	if(!l_this) return;
	CGameObject *l_pOwner = l_pUD1 ? dynamic_cast<CGameObject*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner == (CGameObject*)l_this) l_pOwner = l_pUD2 ? dynamic_cast<CGameObject*>(l_pUD2->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner != l_this->m_pOwner) {
		if(l_this->m_pOwner) l_this->Explode();
	} else {
		do_colide = false;
	}
}

void CWeaponRPG7Grenade::Load(LPCSTR section) {
	inherited::Load(section);
	m_blast = pSettings->r_float(section,"blast");
	m_blastR = pSettings->r_float(section,"blast_r");
	m_frags = pSettings->r_s32(section,"frags");
	m_fragsR = pSettings->r_float(section,"frags_r");
	m_fragHit = pSettings->r_float(section,"frag_hit");

	LPCSTR	name = pSettings->r_string(section,"wm_name");
	pstrWallmark = xr_strdup(name);
	fWallmarkSize = pSettings->r_float(section,"wm_size");

	strcpy(m_effectsSTR, pSettings->r_string(section,"effects"));
	char* l_effectsSTR = m_effectsSTR; R_ASSERT(l_effectsSTR);
	m_effects.clear(); m_effects.push_back(l_effectsSTR);
	while(*l_effectsSTR) {
		if(*l_effectsSTR == ',') {
			*l_effectsSTR = 0; l_effectsSTR++;
			while(*l_effectsSTR == ' ' || *l_effectsSTR == '\t') l_effectsSTR++;
			m_effects.push_back(l_effectsSTR);
		}
		l_effectsSTR++;
	}

	sscanf(pSettings->r_string(section,"light_color"), "%f,%f,%f", &m_lightColor.r, &m_lightColor.g, &m_lightColor.b);
	m_lightRange = pSettings->r_float(section,"light_range");
	m_lightTime = pSettings->r_u32(section,"light_time");

	SoundCreate(sndExplode, "explode", m_eSoundExplode);
	SoundCreate(sndRicochet[0], "ric1", m_eSoundRicochet);
	SoundCreate(sndRicochet[1], "ric2", m_eSoundRicochet);
	SoundCreate(sndRicochet[2], "ric3", m_eSoundRicochet);
	SoundCreate(sndRicochet[3], "ric4", m_eSoundRicochet);
	SoundCreate(sndRicochet[4], "ric5", m_eSoundRicochet);
}

void CWeaponRPG7Grenade::Explode() {
	m_expoldeTime = 500;
	setVisible(false);
	Sound->play_at_pos(sndExplode, 0, vPosition, false);
	Fvector l_dir; f32 l_dst;
	m_blasted.clear();
	feel_touch.clear();
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
	CPGObject* pStaticPG; s32 l_c = m_effects.size();
	for(s32 i = 0; i < l_c; i++) {
		pStaticPG = xr_new<CPGObject>(m_effects[i],Sector()); pStaticPG->play_at_pos(vPosition);
	}
	m_pLight->set_position(vPosition); m_pLight->set_active(true);
	setEnabled(true);
	m_pOwner = NULL;
}

#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}

BOOL CWeaponRPG7Grenade::net_Spawn(LPVOID DC) {
	BOOL l_res = inherited::net_Spawn(DC);

	if(0==pstrWallmark) hWallmark	= 0; 
	else hWallmark	= Device.Shader.Create("effects\\wallmark",pstrWallmark);

	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle1");
	//setVisible					(true);
	//setEnabled					(true);

	if (0==m_pPhysicsShell)
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->vis.box.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		
		Fvector ax;
		float	radius;
		CHOOSE_MAX(	obb.m_halfsize.x,ax.set(obb.m_rotate.i) ; ax.mul(obb.m_halfsize.x); radius=min(obb.m_halfsize.y,obb.m_halfsize.z) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.y,ax.set(obb.m_rotate.j) ; ax.mul(obb.m_halfsize.y); radius=min(obb.m_halfsize.x,obb.m_halfsize.z) ;obb.m_halfsize.x/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.z,ax.set(obb.m_rotate.k) ; ax.mul(obb.m_halfsize.z); radius=min(obb.m_halfsize.y,obb.m_halfsize.x) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.x/=2.f
					)
		//radius*=1.4142f;
		Fsphere sphere1,sphere2;
		sphere1.P.add						(obb.m_translate,ax);
		sphere1.R							=radius*1.4142f;

		sphere2.P.sub						(obb.m_translate,ax);
		sphere2.R							=radius/2.f;

		E->add_Box							(obb);
		E->add_Sphere						(sphere1);
		E->add_Sphere						(sphere2);
	
		// Physics (Shell)
		m_pPhysicsShell						= P_create_Shell	();
		R_ASSERT							(m_pPhysicsShell);
		m_pPhysicsShell->add_Element		(E);
		m_pPhysicsShell->setMass			(8000.f);
		//m_pPhysicsShell->Activate			(svXFORM(),0,svXFORM());
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
		m_pPhysicsShell->SetAirResistance(.000f, 0.f);
		//m_pPhysicsShell->Deactivate();
	}
	return l_res;
}

void CWeaponRPG7Grenade::FragWallmark	(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R) {
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

void CWeaponRPG7Grenade::feel_touch_new(CObject* O) {
	CGameObject *l_pGO = dynamic_cast<CGameObject*>(O);
	if(l_pGO && l_pGO != this) m_blasted.push_back(l_pGO);
}

void CWeaponRPG7Grenade::net_Destroy() {
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	inherited::net_Destroy();
}

void CWeaponRPG7Grenade::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	R_ASSERT		(E);
	svTransform.set(E->clXFORM());
	vPosition.set(svTransform.c);
	if(m_pPhysicsShell) {
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		Fmatrix l_p1, l_r; l_r.rotateY(M_PI*2.f); l_p1.mul(trans, l_r); l_p1.c.set(m_pos);
		Fvector a_vel; a_vel.set(0, 0, 0);
		m_pPhysicsShell->Activate(l_p1, m_vel, a_vel);
		svTransform.set(m_pPhysicsShell->mXFORM);
		vPosition.set(m_pPhysicsShell->mXFORM.c);
		m_pPhysicsShell->set_PhysicsRefObject(this);
		m_pPhysicsShell->set_ObjectContactCallback(ObjectContactCallback);
		m_engineTime = 500;
	}
}

void CWeaponRPG7Grenade::UpdateCL() {
	inherited::UpdateCL();
	if(m_expoldeTime <= Device.dwTimeDelta) {
		m_expoldeTime = 0xffffffff;
		NET_Packet P;
		u_EventGen(P, GE_DESTROY, ID());
		u_EventSend(P);
		return;
	}
	if(m_expoldeTime < 0xffffffff) m_expoldeTime -= Device.dwTimeDelta;
	if(getVisible() && m_pPhysicsShell) {
		m_pPhysicsShell->Update	();
		svTransform.set(m_pPhysicsShell->mXFORM);
		vPosition.set(m_pPhysicsShell->mXFORM.c);
		if(m_engineTime <= Device.dwTimeDelta) m_engineTime = 0xffffffff;
		if(m_engineTime < 0xffffffff) {
			m_engineTime -= Device.dwTimeDelta;
			Fvector l_pos; l_pos.set(0, 0, 3.f);
			m_pPhysicsShell->applyImpulseTrace(l_pos, svTransform.k, 45.f);
		}
	} //else if(H_Parent()) svTransform.set(H_Parent()->clXFORM());
}

void CWeaponRPG7Grenade::SoundCreate(sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq) {
	string256	name,temp;
	strconcat	(name,"weapons\\","rpg_grenade","_",s_name,".wav");
	if (FS.exist(temp,"$game_sounds$",name)) 
	{
		dest.create		(TRUE,name,iType);
		return;
	}
	strconcat	(name,"weapons\\","generic_",s_name,".wav");
	if (FS.exist(temp,"$game_sounds$",name))	
	{
		dest.create		(TRUE,name,iType);
		return;
	}
	Debug.fatal	("Can't find sound '%s' for weapon '%s'", name, "rpg_grenade");
}

void CWeaponRPG7Grenade::SoundDestroy(sound& dest) {
	dest.destroy();
}
//////////////////////////////////////////////////////////////////////////////////////////////////
CWeaponRPG7::CWeaponRPG7(void) : CWeaponCustomPistol("RPG7") {
	m_weight = 5.f;
	m_slot = 2;
	m_hideGrenade = false;
	m_pGrenade = NULL;
}

CWeaponRPG7::~CWeaponRPG7(void) {
}

void CWeaponRPG7::Load	(LPCSTR section)
{
	inherited::Load			(section);
	fMaxZoomFactor			= pSettings->r_float	(section,"max_zoom_factor");
}

BOOL CWeaponRPG7::net_Spawn(LPVOID DC) {
	BOOL l_res = inherited::net_Spawn(DC);
	CKinematics* V = PKinematics(m_pHUD->Visual()); R_ASSERT(V);
	V->LL_GetInstance(V->LL_BoneID("grenade_0")).set_callback(GrenadeCallback, this);
	V = PKinematics(Visual()); R_ASSERT(V);
	V->LL_GetInstance(V->LL_BoneID("grenade")).set_callback(GrenadeCallback, this);
	m_hideGrenade = !iAmmoElapsed;
	return l_res;
}

void __stdcall CWeaponRPG7::GrenadeCallback(CBoneInstance* B) {
	CWeaponRPG7* l_pW = dynamic_cast<CWeaponRPG7*>(static_cast<CObject*>(B->Callback_Param)); R_ASSERT(l_pW);
	if(l_pW->m_hideGrenade) B->mTransform.scale(0, 0, 0);
}

void CWeaponRPG7::OnStateSwitch(u32 S) {
	inherited::OnStateSwitch(S);
	m_hideGrenade = (!iAmmoElapsed && !(S == eReload));
}

void CWeaponRPG7::ReloadMagazine() {
	inherited::ReloadMagazine();
	if(iAmmoElapsed && !m_pGrenade) {
		xrServerEntity*		D	= F_entity_Create("wpn_rpg7_messile");
		R_ASSERT			(D);
		// Fill
		strcpy				(D->s_name,"wpn_rpg7_messile");
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

void CWeaponRPG7::switch2_Fire	()
{
	if (fTime<=0)
	{
		UpdateFP					();

		// Fire
		Fvector						p1, d; p1.set(vLastFP); d.set(vLastFD);
		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(p1,d);

		m_pGrenade->m_pos.set(p1);
		m_pGrenade->m_vel.set(d); m_pGrenade->m_vel.y += .15f; m_pGrenade->m_vel.mul(50.f);
		m_pGrenade->m_pOwner = dynamic_cast<CGameObject*>(H_Parent());
		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16(u16(m_pGrenade->ID()));
		u_EventSend(P);

		bFlame						=	TRUE;
		OnShot						();
		FireTrace					(p1,vLastFP,d);
		fTime						+= fTimeToFire;

		CPGObject* pStaticPG;/* s32 l_c = m_effects.size();*/
		pStaticPG = xr_new<CPGObject>("weapons\\rpg_shoot_01",Sector());
		Fmatrix l_pos; l_pos.set(svTransform); //l_pos.c.set(p1);
		pStaticPG->UpdateParent(l_pos); pStaticPG->Play();
	//for(s32 i = 0; i < l_c; i++) {
	//}

		// Patch for "previous frame position" :)))
		dwFP_Frame					= 0xffffffff;
		dwXF_Frame					= 0xffffffff;
	}
}

//void CWeaponRPG7::OnShot() {
//	inherited::OnShot();
//	R_ASSERT(m_pGrenade);
//	m_pGrenade->m_vel.set(0, 0, 0);
//	NET_Packet P;
//	u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
//	P.w_u16(u16(m_pGrenade->ID()));
//	u_EventSend(P);
//}

void CWeaponRPG7::OnEvent(NET_Packet& P, u16 type) {
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CWeaponRPG7Grenade *l_pG = dynamic_cast<CWeaponRPG7Grenade*>(Level().Objects.net_Find(id));
			m_pGrenade = l_pG;
			l_pG->H_SetParent(this);
		} break;
		case GE_OWNERSHIP_REJECT : {
			P.r_u16(id);
			CWeaponRPG7Grenade *l_pG = dynamic_cast<CWeaponRPG7Grenade*>(Level().Objects.net_Find(id));
			m_pGrenade = NULL;
			l_pG->H_SetParent(0);
		} break;
	}
}

void CWeaponRPG7::Fire2Start () {
	inherited::Fire2Start();
	OnZoomIn();
	fZoomFactor = fMaxZoomFactor;
}
void CWeaponRPG7::Fire2End () {
	inherited::Fire2End();
	OnZoomOut();
	fZoomFactor = DEFAULT_FOV;
}

bool CWeaponRPG7::Action(s32 cmd, u32 flags) {
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) {
		case kWPN_ZOOM : {
			if(flags&CMD_START) Fire2Start();
			else Fire2End();
		} return true;
	}
	return false;
}
