#include "stdafx.h"
#include "weaponmagazinedwgrenade.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "PSObject.h"
#include "PGObject.h"
#include "extendedgeom.h"
#include "effectorshot.h"

static void GetBasis(const Fvector &n, Fvector &u, Fvector &v) {
	if(_abs(n.z)>.707107f) { FLOAT a=n.y*n.y+n.z*n.z, k=1.f/_sqrt(a); u.x=0; u.y=-n.z*k; u.z=n.y*k; v.x=a*k; v.y=-n.x*u.z; v.z=n.x*u.y; }
	else { FLOAT a=n.x*n.x+n.y*n.y, k=1.f/_sqrt(a); u.x=-n.y*k; u.y=n.x*k; u.z=0; v.x=-n.z*u.y; v.y=n.z*u.x; v.z=a*k; }
}

CWeaponFakeGrenade::CWeaponFakeGrenade() 
{
	m_state = stInactive;
	m_pos.set(0, 0, 0); m_vel.set(0, 0, 0);
	m_pOwner = NULL;
	m_eSoundExplode = ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);
	m_eSoundRicochet = ESoundTypes(0/*SOUND_TYPE_WEAPON_BULLET_RICOCHET*/);
	m_pLight = ::Render->light_create();
	m_pLight->set_shadow(true);
	m_explodeTime = m_engineTime = m_flashTime = 0;
	m_jump = 0;
}

CWeaponFakeGrenade::~CWeaponFakeGrenade	()
{
	hWallmark.destroy();
	::Render->light_destroy(m_pLight);
	xr_delete(m_pPhysicsShell);
	SoundDestroy(sndExplode);
	SoundDestroy(sndRicochet[0]);
	SoundDestroy(sndRicochet[1]);
	SoundDestroy(sndRicochet[2]);
	SoundDestroy(sndRicochet[3]);
	SoundDestroy(sndRicochet[4]);
}

void __stdcall CWeaponFakeGrenade::ObjectContactCallback(bool& do_colide,dContact& c) {
	do_colide = false;
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

	CWeaponFakeGrenade *l_this = l_pUD1 ? dynamic_cast<CWeaponFakeGrenade*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_this) l_this = l_pUD2 ? dynamic_cast<CWeaponFakeGrenade*>(l_pUD2->ph_ref_object) : NULL;
	if(!l_this) return;
	CGameObject *l_pOwner = l_pUD1 ? dynamic_cast<CGameObject*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner == (CGameObject*)l_this) l_pOwner = l_pUD2 ? dynamic_cast<CGameObject*>(l_pUD2->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner != l_this->m_pOwner) {
		if(l_this->m_pOwner) {
			Fvector l_pos; l_pos.set(l_this->Position());
			if(!l_pUD1||!l_pUD2) {
				dxGeomUserData *&l_pUD = l_pUD1?l_pUD1:l_pUD2;
				if(l_pUD->pushing_neg) 
				{
					Fvector velocity;
					l_this->PHGetLinearVell(velocity);
					velocity.normalize();
					float cosinus=velocity.dotproduct(*((Fvector*)l_pUD->neg_tri.norm));
					float dist=l_pUD->neg_tri.dist/cosinus;
					velocity.mul(dist);
					l_pos.sub(velocity);

				}
			}
			l_pos.mad(*(Fvector*)&c.geom.normal, l_this->m_jump);
			l_this->Explode(l_pos, *(Fvector*)&c.geom.normal);
		}
	} else {
	}
}

void CWeaponFakeGrenade::Load(LPCSTR section) {
	inherited::Load(section);
	m_blast = pSettings->r_float(section,"blast");
	m_blastR = pSettings->r_float(section,"blast_r");
	m_frags = pSettings->r_s32(section,"frags");
	m_fragsR = pSettings->r_float(section,"frags_r");
	m_fragHit = pSettings->r_float(section,"frag_hit");

	LPCSTR	name = pSettings->r_string(section,"wm_name");
	pstrWallmark = xr_strdup(name);
	fWallmarkSize = pSettings->r_float(section,"wm_size");

	m_mass = pSettings->r_float(section,"ph_mass");
	//m_engine_f = pSettings->r_float(section,"engine_f");
	//m_engine_u = pSettings->r_float(section,"engine_u");

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

	//strcpy(m_trailEffectsSTR, pSettings->r_string(section,"trail"));
	//char* l_trailEffectsSTR = m_trailEffectsSTR; R_ASSERT(l_trailEffectsSTR);
	//m_trailEffects.clear(); m_trailEffects.push_back(l_trailEffectsSTR);
	//while(*l_trailEffectsSTR) {
	//	if(*l_trailEffectsSTR == ',') {
	//		*l_trailEffectsSTR = 0; l_trailEffectsSTR++;
	//		while(*l_trailEffectsSTR == ' ' || *l_trailEffectsSTR == '\t') l_trailEffectsSTR++;
	//		m_trailEffects.push_back(l_trailEffectsSTR);
	//	}
	//	l_trailEffectsSTR++;
	//}

	sscanf(pSettings->r_string(section,"light_color"), "%f,%f,%f", &m_lightColor.r, &m_lightColor.g, &m_lightColor.b); m_lightColor.a=1.f;
	m_lightColor.normalize_rgb(); m_lightColor.mul_rgb(3.f);
	m_lightRange = pSettings->r_float(section,"light_range");
	m_lightTime = pSettings->r_u32(section,"light_time");

	SoundCreate(sndExplode, "explode", m_eSoundExplode);
	SoundCreate(sndRicochet[0], "ric1", m_eSoundRicochet);
	SoundCreate(sndRicochet[1], "ric2", m_eSoundRicochet);
	SoundCreate(sndRicochet[2], "ric3", m_eSoundRicochet);
	SoundCreate(sndRicochet[3], "ric4", m_eSoundRicochet);
	SoundCreate(sndRicochet[4], "ric5", m_eSoundRicochet);
	m_state = stInactive;
}

static const u32 EXPLODE_TIME	= 5000;
static const u32 FLASH_TIME		= 300;
static const u32 ENGINE_TIME	= 3000;
void CWeaponFakeGrenade::Explode(const Fvector &pos, const Fvector &normal) 
{
	m_state			= stExplode;
	m_explodeTime	= EXPLODE_TIME;
	m_flashTime		= FLASH_TIME;
	Position().set(pos);
	setVisible(false);
	//list<CPGObject*>::iterator l_it;
	//for(l_it = m_trailEffectsPSs.begin(); l_it != m_trailEffectsPSs.end(); l_it++) (*l_it)->Stop();
	Sound->play_at_pos(sndExplode, 0, Position(), false);
	Fvector l_dir; f32 l_dst;
	m_blasted.clear();
	feel_touch.clear();
	feel_touch_update(Position(), m_blastR);
	xr_list<s16> l_elsemnts;
	xr_list<Fvector> l_bs_positions;
	while(m_blasted.size()) {
		CGameObject *l_pGO = *m_blasted.begin();
		Fvector l_goPos; if(l_pGO->Visual()) l_pGO->Center(l_goPos); else l_goPos.set(l_pGO->Position());
		l_dir.sub(l_goPos, Position()); l_dst = l_dir.magnitude(); l_dir.div(l_dst); 
		f32 l_S = (l_pGO->Visual()?l_pGO->Radius()*l_pGO->Radius():0);
		if(l_pGO->Visual()) {
			const Fbox &l_b1 = l_pGO->BoundingBox(); Fbox l_b2; l_b2.invalidate();
			Fmatrix l_m; l_m.identity(); l_m.k.set(l_dir); GetBasis(l_m.k, l_m.i, l_m.j);
			for(int i = 0; i < 8; i++) { Fvector l_v; l_b1.getpoint(i, l_v); l_m.transform_tiny(l_v); l_b2.modify(l_v); }
			Fvector l_c, l_d; l_b2.get_CD(l_c, l_d);
			l_S = l_d.x*l_d.y;
		}
		f32 l_impuls = m_blast * (1.f - (l_dst/m_blastR)*(l_dst/m_blastR)) * l_S;
		if(l_impuls > .001f) {
			setEnabled(false);
			l_impuls *= l_pGO->ExplosionEffect(Position(), m_blastR, l_elsemnts, l_bs_positions);
			setEnabled(true);
		}
		if(l_impuls > .001f) while(l_elsemnts.size()) {
			l_dir.y += .2f;
			s16 l_element = *l_elsemnts.begin();
			Fvector l_bs_pos = *l_bs_positions.begin();
			NET_Packet		P;
			u_EventGen		(P,GE_HIT,l_pGO->ID());
			P.w_u16			(u16(ID()));
			P.w_dir			(l_dir);
			P.w_float		(l_impuls);
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
		if(Level().ObjectSpace.RayPick(Position(), l_dir, m_fragsR, RQ)) {
			Fvector l_end, l_bs_pos; l_end.mad(Position(),l_dir,RQ.range); l_bs_pos.set(0, 0, 0);
			if(RQ.O) {
				f32 l_hit = m_fragHit * (1.f - (RQ.range/m_fragsR)*(RQ.range/m_fragsR));
				CEntity* E = dynamic_cast<CEntity*>(RQ.O);
				if(E) l_hit *= E->HitScale(RQ.element);
				NET_Packet		P;
				u_EventGen		(P,GE_HIT,RQ.O->ID());
				P.w_u16			(u16(ID()));
				P.w_dir			(l_dir);
				P.w_float		(l_hit);
				P.w_s16			((s16)RQ.element);
				P.w_vec3		(l_bs_pos);
				P.w_float		(l_hit/(E?E->HitScale(RQ.element):1.f));
				u_EventSend		(P);
			}
			FragWallmark(l_dir, l_end, RQ);
		}
	}
	CPGObject* pStaticPG; s32 l_c = (s32)m_effects.size();
	Fmatrix l_m; l_m.identity(); l_m.c.set(pos);l_m.j.set(normal); GetBasis(normal, l_m.k, l_m.i);
	for(s32 i = 0; i < l_c; i++) {
		pStaticPG = xr_new<CPGObject>(m_effects[i],Sector());
		pStaticPG->SetTransform(l_m);
		pStaticPG->Play();
	}
	m_curColor.set(m_lightColor);
	m_pLight->set_color(m_curColor);
	m_pLight->set_position(Position()); 
	m_pLight->set_active(true);
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

BOOL CWeaponFakeGrenade::net_Spawn(LPVOID DC) 
{
	m_state = stInactive;
	BOOL l_res = inherited::net_Spawn(DC);
	m_pos.set(0, 0, 0); m_vel.set(0, 0, 0);
	m_pOwner = NULL;
	m_explodeTime = m_engineTime = m_flashTime = 0;

	if(0==pstrWallmark) hWallmark	= 0; 
	else hWallmark.create			("effects\\wallmark",pstrWallmark);

	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");
	//setVisible					(true);
	//setEnabled					(true);

	if (0==m_pPhysicsShell)
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->vis.box.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_translate.set(0, 0, 0); obb.m_halfsize.set(.07f, .07f, .35f);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		
		Fvector ax;
		float	radius;
		CHOOSE_MAX(	obb.m_halfsize.x,ax.set(obb.m_rotate.i) ; ax.mul(obb.m_halfsize.x); radius=_min(obb.m_halfsize.y,obb.m_halfsize.z) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.y,ax.set(obb.m_rotate.j) ; ax.mul(obb.m_halfsize.y); radius=_min(obb.m_halfsize.x,obb.m_halfsize.z) ;obb.m_halfsize.x/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.z,ax.set(obb.m_rotate.k) ; ax.mul(obb.m_halfsize.z); radius=_min(obb.m_halfsize.y,obb.m_halfsize.x) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.x/=2.f
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
		//m_pPhysicsShell->setDensity			(8000.f);
		m_pPhysicsShell->setMass			(m_mass);
		//m_pPhysicsShell->Activate			(XFORM(),0,XFORM());
		//m_pPhysicsShell->Activate			();
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
		m_pPhysicsShell->SetAirResistance(.000f, 0.f);
		//m_pPhysicsShell->Deactivate();
	}
	return l_res;
}

void CWeaponFakeGrenade::FragWallmark	(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R) 
{
	if (!hWallmark)	return;
	
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
			g_pGameLevel->ObjectSpace.GetStaticTris()+R.element/**/);
	}

	Sound->play_at_pos(sndRicochet[Random.randI(SND_RIC_COUNT)], 0, vEnd,false);
	
	if (!R.O) 
	{
		// particles
		Fvector N,D;
		CDB::TRI* pTri		= g_pGameLevel->ObjectSpace.GetStaticTris()+R.element;
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

void CWeaponFakeGrenade::feel_touch_new(CObject* O) {
	CGameObject *l_pGO = dynamic_cast<CGameObject*>(O);
	if(l_pGO && l_pGO != this) m_blasted.push_back(l_pGO);
}

void CWeaponFakeGrenade::net_Destroy() {
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	//while(m_trailEffectsPSs.size()) { xr_delete(*(m_trailEffectsPSs.begin())); m_trailEffectsPSs.pop_front(); }
	inherited::net_Destroy();
}

void CWeaponFakeGrenade::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	R_ASSERT		(E);
	XFORM().set(E->XFORM());
	Position().set(XFORM().c);
	if(m_pPhysicsShell) {
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		CWeaponMagazinedWGrenade *l_pW = dynamic_cast<CWeaponMagazinedWGrenade*>(E);
		Fmatrix l_p1, l_r; l_r.rotateY(M_PI*2.f); l_p1.mul(l_pW->GetHUDmode()?trans:XFORM(), l_r); l_p1.c.set(*l_pW->m_pGrenadePoint);
		Fvector a_vel; a_vel.set(0, 0, 0);
		m_pPhysicsShell->Activate(l_p1, m_vel, a_vel);
		XFORM().set(m_pPhysicsShell->mXFORM);
		Position().set(m_pPhysicsShell->mXFORM.c);
		m_pPhysicsShell->set_PhysicsRefObject(this);
		m_pPhysicsShell->set_ObjectContactCallback(ObjectContactCallback);
		m_pPhysicsShell->set_ContactCallback(NULL);

		m_state			= stEngine;
		m_engineTime	= ENGINE_TIME;

		//CPGObject* pStaticPG; s32 l_c = m_trailEffects.size();
		//Fmatrix l_m; l_m.set(XFORM());// GetBasis(normal, l_m.k, l_m.i);
		//for(s32 i = 0; i < l_c; i++) {
		//	pStaticPG = xr_new<CPGObject>(m_trailEffects[i],Sector(),false);
		//	pStaticPG->SetTransform(l_m);
		//	pStaticPG->Play();
		//	m_trailEffectsPSs.push_back(pStaticPG);
		//}
	}
}

void CWeaponFakeGrenade::UpdateCL() {
	inherited::UpdateCL();
	switch (m_state){
	case stInactive:
		break;
	case stDestroying: 
		return;
	case stExplode: 
		if(m_explodeTime <= 0) {
			//while(m_trailEffectsPSs.size()) { xr_delete(*(m_trailEffectsPSs.begin())); m_trailEffectsPSs.pop_front(); }
			m_state			= stDestroying;
			NET_Packet		P;
			u_EventGen		(P, GE_DESTROY, ID());
			u_EventSend		(P);
			return;
		}else
			m_explodeTime	-= Device.dwTimeDelta;
		if(m_flashTime <= 0){
			m_pLight->set_active(false);
		}else{
			if(m_pLight->get_active()){
				float scale	= float(m_flashTime)/float(FLASH_TIME);
				m_curColor.mul_rgb	(m_lightColor,scale);
				m_pLight->set_color	(m_curColor);
			}
			m_flashTime		-= Device.dwTimeDelta;
		}
		break;
	case stEngine:
		if(getVisible() && m_pPhysicsShell) {
			m_pPhysicsShell->Update	();
			XFORM().set	(m_pPhysicsShell->mXFORM);
			Position().set	(m_pPhysicsShell->mXFORM.c);
			if(m_engineTime <= 0) {
				m_state		= stFlying;
				// остановить двигатель
				xr_list<CPGObject*>::iterator l_it;
				//for(l_it = m_trailEffectsPSs.begin(); l_it != m_trailEffectsPSs.end(); l_it++) (*l_it)->Stop();
			}else{
				// двигатель все еще работает
				m_engineTime -= Device.dwTimeDelta;
				Fvector l_pos, l_dir;; l_pos.set(0, 0, 3.f); l_dir.set(XFORM().k); l_dir.normalize();
				float l_force = m_engine_f * Device.dwTimeDelta / 1000.f;
				//m_pPhysicsShell->applyImpulseTrace(l_pos, l_dir, l_force);
				l_dir.set(0, 1.f, 0);
				l_force = m_engine_u * Device.dwTimeDelta / 1000.f;
				//m_pPhysicsShell->applyImpulse(l_dir, l_force);
				xr_list<CPGObject*>::iterator l_it;
				Fvector vel;
				m_pPhysicsShell->get_LinearVel(vel);
				// обновить эффекты
				//for(l_it = m_trailEffectsPSs.begin(); l_it != m_trailEffectsPSs.end(); l_it++) (*l_it)->UpdateParent(XFORM(),vel);
			}
		}
		break;
	case stFlying:
		if(getVisible() && m_pPhysicsShell) {
			m_pPhysicsShell->Update	();
			XFORM().set	(m_pPhysicsShell->mXFORM);
			Position().set	(m_pPhysicsShell->mXFORM.c);
		}
		break;
	}
}

void CWeaponFakeGrenade::SoundCreate(ref_sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq) {
	string256	name,temp;
	strconcat	(name,"weapons\\","rpg7","_",s_name,".wav");
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
	Debug.fatal	("Can't find ref_sound '%s' for weapon '%s'", name, "rpg_grenade");
}

void CWeaponFakeGrenade::SoundDestroy(ref_sound& dest) {
	dest.destroy();
}
//////////////////////////////////////////////////////////////////////////////////////////////////

CWeaponMagazinedWGrenade::CWeaponMagazinedWGrenade(LPCSTR name,ESoundTypes eSoundType) : CWeaponMagazined(name, eSoundType)
{
	m_grenadeMode = false;
	m_ammoType2 = 0;
	m_pGrenade = 0;
}

CWeaponMagazinedWGrenade::~CWeaponMagazinedWGrenade(void)
{
}

void CWeaponMagazinedWGrenade::Load	(LPCSTR section)
{
	inherited::Load		(section);
	//// Sounds
	//SoundCreate			(sndShow,		"draw"    ,m_eSoundShow);
	//SoundCreate			(sndHide,		"holster" ,m_eSoundHide);
	SoundCreate			(sndShotG,		"shoot"   ,m_eSoundShot);
	//SoundCreate			(sndEmptyClick,	"empty"   ,m_eSoundEmptyClick);
	SoundCreate			(sndReloadG,		"reload"  ,m_eSoundReload);
	//SoundCreate			(sndRicochet[0],"ric1"    ,m_eSoundRicochet);
	//SoundCreate			(sndRicochet[1],"ric2"    ,m_eSoundRicochet);
	//SoundCreate			(sndRicochet[2],"ric3"    ,m_eSoundRicochet);
	//SoundCreate			(sndRicochet[3],"ric4"    ,m_eSoundRicochet);
	//SoundCreate			(sndRicochet[4],"ric5"    ,m_eSoundRicochet);
	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle_g,		"idle_g");
	animGet				(mhud_reload_g,	"reload_g");
	animGet				(mhud_shots_g,	"shoot_g");
	animGet				(mhud_switch_g,	"switch_g");
	animGet				(mhud_switch,	"switch");

	strcpy(m_ammoSect2, pSettings->r_string(section,"grenade_class"));
	char* l_ammoSect = m_ammoSect2; R_ASSERT(l_ammoSect);
	m_ammoTypes2.clear(); m_ammoTypes2.push_back(l_ammoSect);
	while(*l_ammoSect) {
		if(*l_ammoSect == ',') {
			*l_ammoSect = 0; l_ammoSect++;
			while(*l_ammoSect == ' ' || *l_ammoSect == '\t') l_ammoSect++;
			m_ammoTypes2.push_back(l_ammoSect);
		}
		l_ammoSect++;
	}
	m_ammoName2 = pSettings->r_string(m_ammoTypes2[0],"inv_name_short");
	iMagazineSize2 = iMagazineSize;
}

BOOL CWeaponMagazinedWGrenade::net_Spawn(LPVOID DC) {
	m_pGrenadePoint = &vLastFP;
	BOOL l_res = inherited::net_Spawn(DC);
	CKinematics* V = PKinematics(m_pHUD->Visual()); R_ASSERT(V);
	V->LL_GetInstance(V->LL_BoneID("grenade_0")).set_callback(GrenadeCallback, this);
	//V = PKinematics(Visual()); R_ASSERT(V);
	//V->LL_GetInstance(V->LL_BoneID("grenade")).set_callback(GrenadeCallback, this);
	CSE_ALifeObject *l_tpALifeObject = (CSE_ALifeObject*)(DC);
	m_hideGrenade = !iAmmoElapsed;
	if(iAmmoElapsed && !m_pGrenade) {
		CSE_Abstract*		D	= F_entity_Create("wpn_fake_missile");
		R_ASSERT			(D);
		CSE_ALifeDynamicObject				*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
		R_ASSERT							(l_tpALifeDynamicObject);
		l_tpALifeDynamicObject->m_tNodeID	= l_tpALifeObject->m_tNodeID;
		// Fill
		strcpy				(D->s_name,"wpn_fake_missile");
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	(u16)ID();
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
	return l_res;
}
void CWeaponMagazinedWGrenade::switch2_Idle() {
	if(m_grenadeMode) m_pHUD->animPlay(mhud_idle_g[Random.randI(mhud_idle_g.size())],FALSE);
	else inherited::switch2_Idle();
}
void CWeaponMagazinedWGrenade::switch2_Reload()
{
	Sound->play_at_pos		(sndReloadG,H_Root(),vLastFP);
	if (sndReload.feedback)
		sndReload.feedback->set_volume(.2f);
	if(m_grenadeMode) m_pHUD->animPlay(mhud_reload_g[Random.randI(mhud_reload_g.size())],FALSE,this);
	else inherited::switch2_Reload();
}

void CWeaponMagazinedWGrenade::OnShot		()
{
	if(m_grenadeMode) {
		Sound->play_at_pos			(sndShot,H_Root(),vLastFP);
		if(hud_mode) {
			CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
			if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot> (camMaxAngle,camRelaxSpeed));
			R_ASSERT				(S);
			S->Shot					(camDispersion);
			m_pHUD->animPlay(mhud_shots_g[Random.randI(mhud_shots_g.size())],TRUE,this);
		}
		CPGObject* pStaticPG;/* s32 l_c = m_effects.size();*/
		pStaticPG = xr_new<CPGObject>("weapons\\generic_shoot",Sector());
		Fmatrix l_pos; l_pos.set(XFORM()); l_pos.c.set(vLastFP);
#pragma todo("Oles to Yura : 'ps_Element(0).dwTime' in game time, not in global time")
		Fvector l_vel; l_vel.sub(Position(),ps_Element(0).vPosition); l_vel.div((Level().timeServer()-ps_Element(0).dwTime)/1000.f);
		pStaticPG->UpdateParent(l_pos, l_vel); pStaticPG->Play();
	} else inherited::OnShot();
	//// Sound
	//Sound->play_at_pos			(sndShot,H_Root(),vLastFP);

	//// Camera
	//if (hud_mode)	
	//{
	//	CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
	//	if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot> (camMaxAngle,camRelaxSpeed));
	//	R_ASSERT				(S);
	//	S->Shot					(camDispersion);
	//}
	//
	//// Animation
	//m_pHUD->animPlay			(mhud_shots[Random.randI(mhud_shots.size())],TRUE,this);
	//
	//// Flames
	//fFlameTime					= .1f;
	//
	//// Shell Drop
	//OnShellDrop					();

	//CPGObject* pStaticPG;/* s32 l_c = m_effects.size();*/
	//pStaticPG = xr_new<CPGObject>("weapons\\generic_shoot",Sector());
	//Fmatrix l_pos; l_pos.set(XFORM()); l_pos.c.set(vLastFP);
	//Fvector l_vel; l_vel.sub(Position(),ps_Element(0).Position()); l_vel.div((Device.dwTimeGlobal-ps_Element(0).dwTime)/1000.f);
	//pStaticPG->UpdateParent(l_pos, l_vel); pStaticPG->Play();
	////pStaticPG->SetTransform(l_pos); pStaticPG->Play();
}

void CWeaponMagazinedWGrenade::SwitchMode() {
	m_grenadeMode = !m_grenadeMode;

	iMagazineSize = m_grenadeMode?1:iMagazineSize2;

	m_ammoTypes.swap(m_ammoTypes2);

	u32 iTmp = m_ammoType; m_ammoType = m_ammoType2; m_ammoType2 = iTmp;

	LPCSTR sTmp = m_ammoName; m_ammoName = m_ammoName2; m_ammoName2 = sTmp;

	xr_stack<CCartridge> l_magazine;
	while(m_magazine.size()) { l_magazine.push(m_magazine.top()); m_magazine.pop(); }
	while(m_magazine2.size()) { m_magazine.push(m_magazine2.top()); m_magazine2.pop(); }
	while(l_magazine.size()) { m_magazine2.push(l_magazine.top()); l_magazine.pop(); }
	iAmmoElapsed = (int)m_magazine.size();
	

	if(m_grenadeMode) m_pHUD->animPlay(mhud_switch_g[Random.randI(mhud_switch_g.size())],FALSE,this);
	else m_pHUD->animPlay(mhud_switch[Random.randI(mhud_switch.size())],FALSE,this);
}

bool CWeaponMagazinedWGrenade::Action(s32 cmd, u32 flags) {
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) {
		case kWPN_FUNC : {
			if(flags&CMD_START) SwitchMode();
		} return true;
	}
	return false;
}

void CWeaponMagazinedWGrenade::state_Fire(float dt) {
	if(m_grenadeMode) {
		UpdateFP				();
		fTime					-=dt;
		Fvector					p1, d; p1.set(vLastFP); d.set(vLastFD);
		if(H_Parent()) dynamic_cast<CEntity*>	(H_Parent())->g_fireParams	(p1,d);
		else return;
		
		while (fTime<0)
		{
			bFlame			=	TRUE;
			fTime			+=	fTimeToFire;

			m_shotNum++;
			OnShot			();
			//FireTrace		(p1,vLastFP,d);
			// Ammo
			if(Local()) {
				//m_abrasion		= _max(0.f, m_abrasion - l_cartridge.m_impair);
				m_magazine.pop	();
				if(!(--iAmmoElapsed)) OnMagazineEmpty();
			}
		}
		UpdateSounds			();
		if(m_shotNum == m_queueSize) FireEnd();
	} else inherited::state_Fire(dt);
}
void CWeaponMagazinedWGrenade::SwitchState(u32 S) {
	inherited::SwitchState(S);
	if(m_grenadeMode && STATE == eIdle && S==eFire && m_pGrenade) {
		Fvector						p1, d; p1.set(vLastFP); d.set(vLastFD);
		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(p1,d);
		m_pGrenade->m_pos.set(p1);
		m_pGrenade->m_vel.set(d); m_pGrenade->m_vel.y += .0f; m_pGrenade->m_vel.mul(50.f);
		m_pGrenade->m_pOwner = dynamic_cast<CGameObject*>(H_Parent());
		m_pGrenade->m_jump = 1.f;
		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16(u16(m_pGrenade->ID()));
		u_EventSend(P);
	}
}

void CWeaponMagazinedWGrenade::OnEvent(NET_Packet& P, u16 type) {
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CWeaponFakeGrenade *l_pG = dynamic_cast<CWeaponFakeGrenade*>(Level().Objects.net_Find(id));
			m_pGrenade = l_pG;
			l_pG->H_SetParent(this);
		} break;
		case GE_OWNERSHIP_REJECT : {
			P.r_u16(id);
			CWeaponFakeGrenade *l_pG = dynamic_cast<CWeaponFakeGrenade*>(Level().Objects.net_Find(id));
			m_pGrenade = NULL;
			l_pG->H_SetParent(0);
		} break;
	}
}

void CWeaponMagazinedWGrenade::ReloadMagazine() {
	inherited::ReloadMagazine();
	if(iAmmoElapsed && !m_pGrenade) {
		CSE_Abstract*		D	= F_entity_Create("wpn_fake_missile");
		R_ASSERT			(D);
		CSE_ALifeDynamicObject				*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
		R_ASSERT							(l_tpALifeDynamicObject);
		l_tpALifeDynamicObject->m_tNodeID	= AI_NodeID;
		// Fill
		strcpy				(D->s_name,"wpn_fake_missile");
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	(u16)ID();
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
}

void __stdcall CWeaponMagazinedWGrenade::GrenadeCallback(CBoneInstance* B) {
	CWeaponMagazinedWGrenade* l_pW = dynamic_cast<CWeaponMagazinedWGrenade*>(static_cast<CObject*>(B->Callback_Param)); R_ASSERT(l_pW);
	if(l_pW->m_hideGrenade) B->mTransform.scale(EPS, EPS, EPS);
}

void CWeaponMagazinedWGrenade::OnStateSwitch(u32 S) {
	inherited::OnStateSwitch(S);
	m_hideGrenade = (!iAmmoElapsed && !(S == eReload));
}
