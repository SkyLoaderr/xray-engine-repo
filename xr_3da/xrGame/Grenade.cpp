#include "stdafx.h"
#include "grenade.h"
#include "PhysicsShell.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"

#define INVSQRT2 .70710678118654752440084436210485f
static void GetBasis(const Fvector &n, Fvector &u, Fvector &v) 
{
	if(_abs(n.z) > INVSQRT2) 
	{
		FLOAT a = n.y * n.y + n.z * n.z;
		FLOAT k = 1.f / _sqrt(a);
		u.x = 0;
		u.y = -n.z * k;
		u.z = n.y * k;
		v.x = a * k;
		v.y = -n.x * u.z;
		v.z = n.x * u.y;
	} else {
		FLOAT a = n.x * n.x + n.y * n.y;
		FLOAT k = 1.f / _sqrt(a);
		u.x = -n.y * k;
		u.y = n.x * k;
		u.z = 0;
		v.x = -n.z * u.y;
		v.y = n.z * u.x;
		v.z = a * k;
	}
}

CGrenade::CGrenade(void) 
{
	m_pFake = NULL;
	m_blast = 50.f;
	m_blastR = 10.f;
	m_frags = 20;
	m_fragsR = 30.f;
	m_fragHit = 50;
	m_eSoundExplode = ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);
	m_eSoundRicochet = ESoundTypes(0/*SOUND_TYPE_WEAPON_BULLET_RICOCHET*/);
	m_eSoundCheckout = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);
	m_expoldeTime = 0xffffffff;
	m_pLight = ::Render->light_create();
	m_pLight->set_shadow(true);
	hWallmark = 0;
}

CGrenade::~CGrenade(void) 
{
	hWallmark.destroy		();
	::Render->light_destroy	(m_pLight);
	SoundDestroy(sndExplode);
	SoundDestroy(sndCheckout);
	SoundDestroy(sndRicochet[0]);
	SoundDestroy(sndRicochet[1]);
	SoundDestroy(sndRicochet[2]);
	SoundDestroy(sndRicochet[3]);
	SoundDestroy(sndRicochet[4]);
}

void CGrenade::Load(LPCSTR section) 
{
	inherited::Load(section);
	m_blast = pSettings->r_float(section,"blast");
	m_blastR = pSettings->r_float(section,"blast_r");
	m_frags = pSettings->r_s32(section,"frags");
	m_fragsR = pSettings->r_float(section,"frags_r");
	m_fragHit = pSettings->r_float(section,"frag_hit");

	LPCSTR	name = pSettings->r_string(section,"wm_name");
	pstrWallmark = name;
	fWallmarkSize = pSettings->r_float(section,"wm_size");

	string256	m_effectsSTR;
	strcpy(m_effectsSTR, pSettings->r_string(section,"effects"));
	char* l_effectsSTR = m_effectsSTR; R_ASSERT(l_effectsSTR);
	m_effects.clear(); m_effects.push_back(l_effectsSTR);
	
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

	sscanf(pSettings->r_string(section,"light_color"), "%f,%f,%f", &m_lightColor.r, &m_lightColor.g, &m_lightColor.b);
	m_lightRange = pSettings->r_float(section,"light_range");
	m_lightTime = pSettings->r_u32(section,"light_time");

	SoundCreate(sndExplode, "explode", m_eSoundExplode);
	SoundCreate(sndCheckout, "checkout", m_eSoundCheckout);
	SoundCreate(sndRicochet[0], "ric1", m_eSoundRicochet);
	SoundCreate(sndRicochet[1], "ric2", m_eSoundRicochet);
	SoundCreate(sndRicochet[2], "ric3", m_eSoundRicochet);
	SoundCreate(sndRicochet[3], "ric4", m_eSoundRicochet);
	SoundCreate(sndRicochet[4], "ric5", m_eSoundRicochet);
}

BOOL CGrenade::net_Spawn(LPVOID DC) 
{
	if(0==pstrWallmark) hWallmark	= 0; 
	else hWallmark.create		("effects\\wallmark",*pstrWallmark);
	return inherited::net_Spawn	(DC);
}

void CGrenade::net_Destroy() 
{
	hWallmark.destroy		();
	inherited::net_Destroy	();
}

void CGrenade::OnH_A_Chield() 
{
	inherited::OnH_A_Chield();

	if(!m_pFake && !dynamic_cast<CGrenade*>(H_Parent())) 
	{
		CSE_Abstract		*D	= F_entity_Create(cNameSect());
		R_ASSERT			(D);
		CSE_ALifeDynamicObject				*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
		R_ASSERT							(l_tpALifeDynamicObject);
		l_tpALifeDynamicObject->m_tNodeID	= level_vertex_id();
		// Fill
		strcpy				(D->s_name,cNameSect());
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

void CGrenade::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	//if(dynamic_cast<CGrenade*>(H_Parent())) inherited::OnH_B_Independent();
	//else CInventoryItem::OnH_B_Independent();
}

u32 CGrenade::State(u32 state) 
{
	if(state == MS_THREATEN) 
	{
		Sound->play_at_pos(sndCheckout, 0, Position(), false);
	}
	return inherited::State(state);
}

bool CGrenade::Activate() 
{
	Show();
	return true;
}

void CGrenade::Deactivate() 
{
	Hide();
}

void CGrenade::Throw() 
{
	if(!m_pFake) return;
	
	inherited::Throw();
	
	CGrenade *pGrenade = m_pFake;
	if(pGrenade) 
	{
		pGrenade->m_destroyTime = 3500;
		pGrenade->m_force = m_force; 
		m_force = 0;
		{
			NET_Packet P;
			u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
			P.w_u16(u16(pGrenade->ID()));
			u_EventSend(P);
		}
	}
}

void CGrenade::Destroy() 
{
	Explode();
	m_expoldeTime = 5000;
	//inherited::Destroy();
}

void CGrenade::Explode() 
{
#pragma todo("Yura to Yura: adjust explosion effect to objects")

	setVisible(false);
	Sound->play_at_pos(sndExplode, 0, Position(), false);
	Fvector l_dir; f32 l_dst;
	m_blasted.clear();
	feel_touch_update(Position(), m_blastR);
	xr_list<s16>		l_elsemnts;
	xr_list<Fvector>	l_bs_positions;
	while(m_blasted.size()) 
	{
		CGameObject *l_pGO = *m_blasted.begin();
		Fvector l_goPos; 
		if(l_pGO->Visual()) 
			l_pGO->Center(l_goPos); 
		else 
			l_goPos.set(l_pGO->Position());
		
		l_dir.sub(l_goPos, Position()); 
		l_dst = l_dir.magnitude(); 
		l_dir.div(l_dst); l_dir.y += .2f;
		f32 l_S = (l_pGO->Visual()?l_pGO->Radius()*l_pGO->Radius():0);
		
		//взрывна€ волна
		if(l_pGO->Visual()) 
		{
			const Fbox &l_b1 = l_pGO->BoundingBox(); 
			Fbox l_b2; 
			l_b2.invalidate();
			Fmatrix l_m; l_m.identity(); 
			l_m.k.set(l_dir); 
			GetBasis(l_m.k, l_m.i, l_m.j);
			
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
		f32 l_impuls = m_blast * (1.f - (l_dst/m_blastR)*(l_dst/m_blastR)) * l_S;

		if(l_impuls > .001f) 
		{
			setEnabled(false);
			l_impuls *= l_pGO->ExplosionEffect(Position(), m_blastR, l_elsemnts, l_bs_positions);
			setEnabled(true);
		}
		if(l_impuls > .001f) 
		{
			while(l_elsemnts.size()) 
			{
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
				P.w_u16			(u16(eHitTypeWound));
				u_EventSend		(P);
				l_elsemnts.pop_front();
				l_bs_positions.pop_front();
			}
		}
		m_blasted.pop_front();
	}
	Collide::rq_result RQ;
	setEnabled(false);
	
	
	//осколки
	for(s32 i = 0; i < m_frags; ++i) 
	{
		l_dir.set(::Random.randF(-.5f,.5f), 
				  ::Random.randF(-.5f,.5f), 
				  ::Random.randF(-.5f,.5f)); 
		l_dir.normalize();

		if(Level().ObjectSpace.RayPick(Position(), l_dir, m_fragsR, Collide::rqtBoth, RQ)) 
		{
			Fvector l_end, l_bs_pos; 
			l_end.mad(Position(),l_dir,RQ.range); 
			l_bs_pos.set(0, 0, 0);
			
			if(RQ.O) 
			{
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
				P.w_u16			(eHitTypeWound);
				u_EventSend		(P);
			}
			FragWallmark(l_dir, l_end, RQ);
		}
	}
	
	CParticlesObject* pStaticPG; s32 l_c = (s32)m_effects.size();

	for(s32 i = 0; i < l_c; ++i) 
	{
		pStaticPG = xr_new<CParticlesObject>(*m_effects[i],Sector()); pStaticPG->play_at_pos(Position());
	}
	m_pLight->set_position(Position()); 
	m_pLight->set_active(true);
	setEnabled(true);
}

void CGrenade::FragWallmark	(const Fvector& vDir, const Fvector &vEnd, Collide::rq_result& R) 
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
	} else {
		R_ASSERT(R.element >= 0);
		::Render->add_Wallmark	(
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
		Fvector N,D;
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

void CGrenade::feel_touch_new(CObject* O) 
{
	CGameObject *l_pGO = dynamic_cast<CGameObject*>(O);
	if(l_pGO && this != l_pGO) m_blasted.push_back(l_pGO);
}

bool CGrenade::Useful() 
{
	// ≈сли IItem еще не полностью использованый, вернуть true
	return m_destroyTime == 0xffffffff;
}

void CGrenade::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	u16 id;
	switch (type)
	{
	case GE_OWNERSHIP_TAKE: 
		{
			P.r_u16(id);
			CGrenade *pGrenade = dynamic_cast<CGrenade*>(Level().Objects.net_Find(id));
			m_pFake = pGrenade;
			pGrenade->H_SetParent(this);
		} 
		break;
	case GE_OWNERSHIP_REJECT: 
		{
			P.r_u16(id);
			CGrenade *pGrenade = dynamic_cast<CGrenade*>(Level().Objects.net_Find(id));
			m_pFake = NULL;
			pGrenade->H_SetParent(0);
		} 
break;
	}
}

void CGrenade::OnAnimationEnd() 
{
	switch(inherited::State()) 
	{
	case MS_END:
		{
			if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
			xr_delete(m_pPhysicsShell);
			
			//выкинуть гранату из инвентар€
			m_pInventory->Ruck(this); 
			m_destroyTime = 0;
			
			NET_Packet P;
			u_EventGen(P, GE_OWNERSHIP_REJECT, H_Parent()->ID());
			P.w_u16(u16(ID()));
			u_EventSend(P);

			//найти такую же гранату и положить в рюкзак
			CGrenade *pNext = dynamic_cast<CGrenade*>(m_pInventory->Same(this));
			//или найти любую другую гранату на по€се
			if(!pNext) pNext = dynamic_cast<CGrenade*>(m_pInventory->SameSlot(m_slot));
						
			if(pNext) 
			{ 
				m_pInventory->Slot(pNext); 
				m_pInventory->Activate(pNext->m_slot); 
			}
		} 
		break;
		default : inherited::OnAnimationEnd();
	}
}

void CGrenade::UpdateCL() 
{
	inherited::UpdateCL();
	if(m_expoldeTime > 0 && m_expoldeTime <= Device.dwTimeDelta) 
	{
		m_expoldeTime = 0;
		m_pLight->set_active(false);
		inherited::Destroy();
	} 
	else if(m_expoldeTime < 0xffffffff) 
	{
		m_expoldeTime -= Device.dwTimeDelta;
		
		if(m_expoldeTime > (5000 - m_lightTime)) 
		{
			f32 l_scale = f32(m_expoldeTime - (5000 - m_lightTime))/f32(m_lightTime);
			m_pLight->set_color(m_lightColor.r*l_scale, 
								m_lightColor.g*l_scale, 
								m_lightColor.b*l_scale);
			
			m_pLight->set_range(m_lightRange*l_scale);
		} 
		else m_pLight->set_range(0);
	} 
	else m_pLight->set_active(false);

	if (Remote() && NET.size())
	{
		net_update N = NET.back();
		NET.pop_back();
		NET_Last = N;

		Position().set(NET_Last.pos);
	};
}

bool CGrenade::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;

	switch(cmd) 
	{
	case kWPN_NEXT:
		{
            if(flags&CMD_START) 
			{
				if(m_pInventory)
				{

					//ищем на по€се такую же гранату
					PPIItem it = m_pInventory->m_belt.begin();
					while(m_pInventory->m_belt.end() != it && 
						  strcmp((*it)->cNameSect(), cNameSect())) ++it;
					
					if(m_pInventory->m_belt.end() != it) 
					{
						while(m_pInventory->m_belt.end() != it) 
						{
							CGrenade *pGrenade = dynamic_cast<CGrenade*>(*it);
							if(pGrenade && strcmp(pGrenade->cNameSect(), cNameSect())) 
							{
								m_pInventory->Ruck(this); 
								m_pInventory->Slot(pGrenade); 
								m_pInventory->Belt(this); 
								m_pInventory->Activate(pGrenade->m_slot);
								return true;
							}
							++it;
						}

						
						it = m_pInventory->m_belt.begin();
						CGrenade *pGrenade = dynamic_cast<CGrenade*>(*it);

						while(this != pGrenade) 
						{
							if(pGrenade && strcmp(pGrenade->cNameSect(), cNameSect())) 
							{
								m_pInventory->Ruck(this); 
								m_pInventory->Slot(pGrenade); 
								m_pInventory->Belt(this); 
								m_pInventory->Activate(pGrenade->m_slot);
								return true;
							}
							++it;
							if(it == m_pInventory->m_belt.end()) break;
							
							pGrenade = dynamic_cast<CGrenade*>(*it);
						}
						
					}
					return true;
				}
			}
		} return true;
	}
	return false;
}

void CGrenade::net_Import			(NET_Packet& P) 
{
	net_update			N;

	P.r_u32					( N.dwTimeStamp );
	P.r_vec3				( N.pos	);
	P.r_angle8				( N.angles.x);
	P.r_angle8				( N.angles.y);
	P.r_angle8				( N.angles.z);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	
	{
		NET.push_back			(N);
	}
};

void CGrenade::net_Export			(NET_Packet& P) 
{
	P.w_u32				(Level().timeServer());
	P.w_vec3			(Position()	);

	float					_x,_y,_z;
	XFORM().getHPB			(_y,_x,_z);
	P.w_angle8				(_x);
	P.w_angle8				(_y);
	P.w_angle8				(_z);
};