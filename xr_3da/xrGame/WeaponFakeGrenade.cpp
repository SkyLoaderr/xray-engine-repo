// WeaponFakeGrenade.cpp: объект для эмулирования подствольной гранаты
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WeaponFakeGrenade.h"

#include "WeaponMagazinedWGrenade.h"

#include "Entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"
#include "PhysicsShell.h"

#include "extendedgeom.h"


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

void __stdcall CWeaponFakeGrenade::ObjectContactCallback(bool& do_colide,dContact& c) 
{
	do_colide = false;
	dxGeomUserData *l_pUD1 = NULL;
	dxGeomUserData *l_pUD2 = NULL;
	l_pUD1 = retrieveGeomUserData(c.geom.g1);
	l_pUD2 = retrieveGeomUserData(c.geom.g2);

	CWeaponFakeGrenade *l_this = l_pUD1 ? dynamic_cast<CWeaponFakeGrenade*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_this) l_this = l_pUD2 ? dynamic_cast<CWeaponFakeGrenade*>(l_pUD2->ph_ref_object) : NULL;
	if(!l_this) return;

	CGameObject *l_pOwner = l_pUD1 ? dynamic_cast<CGameObject*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner == (CGameObject*)l_this) l_pOwner = l_pUD2 ? dynamic_cast<CGameObject*>(l_pUD2->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner != l_this->m_pOwner) 
	{
		if(l_this->m_pOwner) 
		{
			Fvector l_pos; l_pos.set(l_this->Position());
			if(!l_pUD1||!l_pUD2) 
			{
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
	} else {}
}

void CWeaponFakeGrenade::Load(LPCSTR section) 
{
	inherited::Load(section);
	m_fBlastHit = pSettings->r_float(section,"blast");
	m_fBlastRadius = pSettings->r_float(section,"blast_r");
	m_iFragsNum = pSettings->r_s32(section,"frags");
	m_fFragsRadius = pSettings->r_float(section,"frags_r");
	m_fFragHit = pSettings->r_float(section,"frag_hit");

	LPCSTR	name = pSettings->r_string(section,"wm_name");
	pstrWallmark = name;
	fWallmarkSize = pSettings->r_float(section,"wm_size");

	m_mass = pSettings->r_float(section,"ph_mass");

	m_jump = pSettings->r_float(section, "jump_height");

	string512 m_effectsSTR;
	strcpy(m_effectsSTR, pSettings->r_string(section,"effects"));
	char* l_effectsSTR = m_effectsSTR; R_ASSERT(l_effectsSTR);
	m_effects.clear(); 
	m_effects.push_back(l_effectsSTR);
	
	while(*l_effectsSTR) 
	{
		if(*l_effectsSTR == ',') 
		{
			*l_effectsSTR = 0; ++l_effectsSTR;
			while(*l_effectsSTR == ' ' || *l_effectsSTR == '\t') ++l_effectsSTR;
			m_effects.push_back(l_effectsSTR);
		}
		++l_effectsSTR;
	}

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

void CWeaponFakeGrenade::Explode(const Fvector &pos, const Fvector &/**normal/**/) 
{
	m_state			= stExplode;
	m_explodeTime	= EXPLODE_TIME;
	m_flashTime		= FLASH_TIME;
	Position().set(pos);
	setVisible(false);
	//list<CParticlesObject*>::iterator l_it;
	//for(l_it = m_trailEffectsPSs.begin(); m_trailEffectsPSs.end() != l_it; ++l_it) (*l_it)->Stop();
	Sound->play_at_pos(sndExplode, 0, Position(), false);
	Fvector l_dir; f32 l_dst;
	m_blasted.clear();
	feel_touch.clear();
	feel_touch_update(Position(), m_fBlastRadius);
	xr_list<s16> l_elsemnts;
	xr_list<Fvector> l_bs_positions;
	while(m_blasted.size()) 
	{
		CGameObject *l_pGO = *m_blasted.begin();
		Fvector l_goPos; 
		if(l_pGO->Visual()) 
			l_pGO->Center(l_goPos); 
		else 
			l_goPos.set(l_pGO->Position());
		l_dir.sub(l_goPos, Position()); 
		l_dst = l_dir.magnitude(); l_dir.div(l_dst); 
		
		f32 l_S = (l_pGO->Visual()?l_pGO->Radius()*l_pGO->Radius():0);
		if(l_pGO->Visual()) 
		{
			const Fbox &l_b1 = l_pGO->BoundingBox(); 
			Fbox l_b2; 
			l_b2.invalidate();
			Fmatrix l_m; 
			l_m.identity(); 
			l_m.k.set(l_dir); 
			Fvector::generate_orthonormal_basis(l_m.k, l_m.i, l_m.j);
			for(int i = 0; i < 8; ++i) 
			{ 
				Fvector l_v; 
				l_b1.getpoint(i, l_v); 
				l_m.transform_tiny(l_v); 
				l_b2.modify(l_v); 
			}
			
			Fvector l_c, l_d; 
			l_b2.get_CD(l_c, l_d);
			l_S = l_d.x*l_d.y;
		}
		f32 l_impuls = m_fBlastHit * (1.f - (l_dst/m_fBlastRadius)*(l_dst/m_fBlastRadius)) * l_S;
		if(l_impuls > .001f) 
		{
			setEnabled(false);
			l_impuls *= l_pGO->ExplosionEffect(Position(), m_fBlastRadius, l_elsemnts, l_bs_positions);
			setEnabled(true);
		}
		if(l_impuls > .001f) while(l_elsemnts.size()) 
		{
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
			P.w_u16			(eHitTypeWound);
			u_EventSend		(P);
			l_elsemnts.pop_front();
			l_bs_positions.pop_front();
		}
		m_blasted.pop_front();
	}
	
	Collide::rq_result RQ;
	setEnabled(false);
	for(s32 i = 0; i < m_iFragsNum; ++i) 
	{
		l_dir.set(::Random.randF(-.5f,.5f), ::Random.randF(-.5f,.5f), ::Random.randF(-.5f,.5f)); 
		l_dir.normalize();
		
		if(Level().ObjectSpace.RayPick(Position(), l_dir, m_fFragsRadius, Collide::rqtBoth, RQ)) 
		{
			Fvector l_end, l_bs_pos; 
			l_end.mad(Position(),l_dir,RQ.range); 
			l_bs_pos.set(0, 0, 0);
			if(RQ.O) 
			{
				f32 l_hit = m_fFragHit * (1.f - (RQ.range/m_fFragsRadius)*(RQ.range/m_fFragsRadius));
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
				P.w_u16			(eHitTypeWound);
				u_EventSend		(P);
			}
			FragWallmark(l_dir, l_end, RQ);
		}
	}


	//запустить партикловую анимацию взрыва
	CParticlesObject* pStaticPG; 
	s32 l_c = (s32)m_effects.size();
	for(s32 i = 0; i < l_c; ++i) 
	{
		pStaticPG = xr_new<CParticlesObject>(*m_effects[i],Sector()); 
		pStaticPG->play_at_pos(pos);
	}
	m_pLight->set_position(pos); 
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
	else hWallmark.create			("effects\\wallmark",*pstrWallmark);

//	CSkeletonAnimated* V = PSkeletonAnimated(Visual());
//	if(V) V->PlayCycle("idle");

	CSkeletonRigid* V = PSkeletonRigid(Visual());
	R_ASSERT(V);

	
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

void CWeaponFakeGrenade::FragWallmark	(const Fvector& vDir, const Fvector &vEnd, Collide::rq_result& R) 
{
	if (!hWallmark)	return;
	
	if (R.O) 
	{
		if (R.O->CLS_ID==CLSID_ENTITY)
		{
#pragma todo("Oles to Yura: replace 'CPSObject' with 'CParticlesObject'")
			/*
			IRender_Sector* S	= R.O->Sector();
			Fvector D;	D.invert(vDir);

			LPCSTR ps_gibs		= "blood_1";//(Random.randI(5)==0)?"sparks_1":"stones";
			CPSObject* PS		= xr_new<CPSObject> (ps_gibs,S,true);
			PS->m_Emitter.m_ConeDirection.set(D);
			PS->play_at_pos		(vEnd);
			*/
		}
	} 
	else 
	{
		R_ASSERT(R.element >= 0);
		::Render->add_Wallmark (
			hWallmark,
			vEnd,
			fWallmarkSize,
			g_pGameLevel->ObjectSpace.GetStaticTris()+R.element,
			g_pGameLevel->ObjectSpace.GetStaticVerts()
			);
	}

	Sound->play_at_pos(sndRicochet[Random.randI(SND_RIC_COUNT)], 0, vEnd,false);
	
	if (!R.O) 
	{
		// particles
		Fvector		N,D;
		Fvector*	pVerts	= g_pGameLevel->ObjectSpace.GetStaticVerts();
		CDB::TRI*	pTri	= g_pGameLevel->ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
		D.reflect			(vDir,N);
		
#pragma todo("Oles to Yura: replace 'CPSObject' with 'CParticlesObject'")
		/*
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
		*/
	}
}

void CWeaponFakeGrenade::feel_touch_new(CObject* O) 
{
	CGameObject *pGameObject = dynamic_cast<CGameObject*>(O);
	if(pGameObject && this != pGameObject) m_blasted.push_back(pGameObject);
}

void CWeaponFakeGrenade::net_Destroy() 
{
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	//while(m_trailEffectsPSs.size()) { xr_delete(*(m_trailEffectsPSs.begin())); m_trailEffectsPSs.pop_front(); }
	inherited::net_Destroy();
}

void CWeaponFakeGrenade::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
	CObject*	 E = dynamic_cast<CObject*>(H_Parent());
	R_ASSERT	(E);
	XFORM().set(E->XFORM());
	
	if(m_pPhysicsShell) 
	{
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		CWeaponMagazinedWGrenade *l_pW = dynamic_cast<CWeaponMagazinedWGrenade*>(E);
		Fmatrix l_p1, l_r; 
		l_r.rotateY(M_PI*2.f); 
		l_p1.mul(l_pW->GetHUDmode()?trans:XFORM(), l_r); 
		l_p1.c.set(*l_pW->m_pGrenadePoint);
		
		Fvector a_vel; 
		a_vel.set(0, 0, 0);
		m_pPhysicsShell->Activate(l_p1, m_vel, a_vel);
		XFORM().set(m_pPhysicsShell->mXFORM);
		Position().set(m_pPhysicsShell->mXFORM.c);
		m_pPhysicsShell->set_PhysicsRefObject(this);
		m_pPhysicsShell->set_ObjectContactCallback(ObjectContactCallback);
		m_pPhysicsShell->set_ContactCallback(NULL);

		m_state			= stEngine;
		m_engineTime	= ENGINE_TIME;

		//CParticlesObject* pStaticPG; s32 l_c = m_trailEffects.size();
		//Fmatrix l_m; l_m.set(XFORM());// GetBasis(normal, l_m.k, l_m.i);
		//for(s32 i = 0; i < l_c; ++i) {
		//	pStaticPG = xr_new<CParticlesObject>(m_trailEffects[i],Sector(),false);
		//	pStaticPG->SetTransform(l_m);
		//	pStaticPG->Play();
		//	m_trailEffectsPSs.push_back(pStaticPG);
		//}
	}
}

void CWeaponFakeGrenade::UpdateCL() 
{
	inherited::UpdateCL();
	
	switch (m_state)
	{
	case stInactive:
		break;
	case stDestroying: 
		return;
	case stExplode: 
		if(m_explodeTime <= 0) 
		{
			//while(m_trailEffectsPSs.size()) { xr_delete(*(m_trailEffectsPSs.begin())); m_trailEffectsPSs.pop_front(); }
			m_state			= stDestroying;
			NET_Packet		P;
			u_EventGen		(P, GE_DESTROY, ID());
			u_EventSend		(P);
			return;
		}
		else
			m_explodeTime	-= Device.dwTimeDelta;
		if(m_flashTime <= 0)
		{
			m_pLight->set_active(false);
		}
		else
		{
			if(m_pLight->get_active())
			{
				float scale	= float(m_flashTime)/float(FLASH_TIME);
				m_curColor.mul_rgb	(m_lightColor,scale);
				m_pLight->set_color	(m_curColor);
			}
			m_flashTime		-= Device.dwTimeDelta;
		}
		break;
	case stEngine:
		if(getVisible() && m_pPhysicsShell) 
		{
			m_pPhysicsShell->Update	();
			XFORM().set	(m_pPhysicsShell->mXFORM);
			Position().set	(m_pPhysicsShell->mXFORM.c);
			if(m_engineTime <= 0) 
			{
				m_state		= stFlying;
				// остановить двигатель
				xr_list<CParticlesObject*>::iterator l_it;
				//for(l_it = m_trailEffectsPSs.begin(); m_trailEffectsPSs.end() != l_it; ++l_it) (*l_it)->Stop();
			}
			else
			{
				// двигатель все еще работает
				m_engineTime -= Device.dwTimeDelta;
				Fvector l_pos, l_dir;; l_pos.set(0, 0, 3.f); l_dir.set(XFORM().k); l_dir.normalize();
				float l_force = m_engine_f * Device.dwTimeDelta / 1000.f;
				//m_pPhysicsShell->applyImpulseTrace(l_pos, l_dir, l_force);
				l_dir.set(0, 1.f, 0);
				l_force = m_engine_u * Device.dwTimeDelta / 1000.f;
				//m_pPhysicsShell->applyImpulse(l_dir, l_force);
				xr_list<CParticlesObject*>::iterator l_it;
				Fvector vel;
				PHGetLinearVell(vel);
				//m_pPhysicsShell->get_LinearVel(vel);
				// обновить эффекты
				//for(l_it = m_trailEffectsPSs.begin(); m_trailEffectsPSs.end() != l_it; ++l_it) (*l_it)->UpdateParent(XFORM(),vel);
			}
		}
		break;
	case stFlying:
		if(getVisible() && m_pPhysicsShell) 
		{
			m_pPhysicsShell->Update	();
			XFORM().set	(m_pPhysicsShell->mXFORM);
			Position().set	(m_pPhysicsShell->mXFORM.c);
		}
		break;
	}
}

void CWeaponFakeGrenade::SoundCreate(ref_sound& dest, LPCSTR s_name, int iType, BOOL /**bCtrlFreq/**/) 
{
	string256	name,temp;
	strconcat	(name,"weapons\\","rpg7","_",s_name,".ogg");
	if (FS.exist(temp,"$game_sounds$",name)) 
	{
		dest.create		(TRUE,name,iType);
		return;
	}
	strconcat	(name,"weapons\\","generic_",s_name,".ogg");
	if (FS.exist(temp,"$game_sounds$",name))	
	{
		dest.create		(TRUE,name,iType);
		return;
	}
	Debug.fatal	("Can't find ref_sound '%s' for weapon '%s'", name, "rpg_grenade");
}

void CWeaponFakeGrenade::SoundDestroy(ref_sound& dest) 
{
	dest.destroy();
}
