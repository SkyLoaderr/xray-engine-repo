// Explosive.cpp: ��������� ��� ������������� ��������
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "explosive.h"

#include "PhysicsShell.h"
#include "entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"

//��� ������ ����������� ������� ��������� ���������
#include "Weapon.h"

#include "explode_effector.h" 
#include "actor.h"
#include "actoreffector.h"
#include "level_bullet_manager.h"

#define EFFECTOR_RADIUS 30.f

CExplosive::CExplosive(void) 
{
	m_fBlastHit = 50.f;
	m_fBlastRadius = 10.f;
	m_iFragsNum = 20;
	m_fFragsRadius = 30.f;
	m_fFragHit = 50;

	m_fUpThrowFactor = 0.f;


	m_eSoundExplode = ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);

	m_pLight = ::Render->light_create();
	m_pLight->set_shadow(true);


	m_eHitTypeBlast = ALife::eHitTypeExplosion;
	m_eHitTypeFrag = ALife::eHitTypeFireWound;


	m_iCurrentParentID = 0xffff;
	m_bReadyToExplode = false;
}

CExplosive::~CExplosive(void) 
{
	::Render->light_destroy	(m_pLight);

	sndExplode.destroy();
}

void CExplosive::Load(LPCSTR section) 
{
	inherited::Load		(section);
	m_fBlastHit			= pSettings->r_float(section,"blast");
	m_fBlastRadius		= pSettings->r_float(section,"blast_r");
	m_fBlastHitImpulse	= pSettings->r_float(section,"blast_impulse");
	
	m_iFragsNum			= pSettings->r_s32(section,"frags");
	m_fFragsRadius		= pSettings->r_float(section,"frags_r");
	m_fFragHit			= pSettings->r_float(section,"frag_hit");
	m_fFragHitImpulse	= pSettings->r_float(section,"frag_hit_impulse");

	m_eHitTypeBlast		= ALife::g_tfString2HitType(pSettings->r_string(section, "hit_type_blast"));
	m_eHitTypeFrag		= ALife::g_tfString2HitType(pSettings->r_string(section, "hit_type_frag"));

	m_fUpThrowFactor	= pSettings->r_float(section,"up_throw_factor");


	fWallmarkSize		= pSettings->r_float(section,"wm_size");
	R_ASSERT			(fWallmarkSize>0);

	m_sExplodeParticles = pSettings->r_string(section,"explode_particles");

	sscanf				(pSettings->r_string(section,"light_color"), "%f,%f,%f", &m_LightColor.r, &m_LightColor.g, &m_LightColor.b);
	m_fLightRange		= pSettings->r_float(section,"light_range");
	m_dwLightTime		= iFloor(pSettings->r_float(section,"light_time")*1000.f);

	//������ ��� ������� ��������
	tracerHeadSpeed		= pSettings->r_float		(section,"tracer_head_speed"	);
	tracerMaxLength		= pSettings->r_float		(section,"tracer_max_length"	);

	ref_str				snd_name = pSettings->r_string(section,"snd_explode");
	sndExplode.create	(TRUE,*snd_name, m_eSoundExplode);

	m_dwExplodeDurationMax= EXPLODE_TIME_MAX;

	effector.time			= pSettings->r_float("explode_effector","time");
	effector.amplitude		= pSettings->r_float("explode_effector","amplitude");
	effector.period_number	= pSettings->r_float("explode_effector","period_number");
}

BOOL CExplosive::net_Spawn		(LPVOID DC)
{
	return				(inherited::net_Spawn(DC));
}

void CExplosive::net_Destroy	()
{
	inherited::net_Destroy();
	m_pLight->set_active(false);
}


/////////////////////////////////////////////////////////
// ����� 
/////////////////////////////////////////////////////////
void CExplosive::Explode()
{
	VERIFY(0xffff != m_iCurrentParentID);
	VERIFY(m_bReadyToExplode);

	Fvector& pos = m_vExplodePos;
	Fvector& dir = m_vExplodeDir;

	setVisible(false);
	setEnabled(false);

	//������ ���� ������
	Sound->play_at_pos(sndExplode, 0, pos, false);
	
	//���������� �������
	CParticlesObject* pStaticPG; 
	pStaticPG = xr_new<CParticlesObject>(*m_sExplodeParticles,Sector()); 
	
	Fvector vel;
	PHGetLinearVell(vel);

	Fmatrix explode_matrix;
	explode_matrix.identity();
	explode_matrix.j.set(dir);
	Fvector::generate_orthonormal_basis(explode_matrix.j, explode_matrix.k, explode_matrix.i);
	explode_matrix.c.set(pos);

	pStaticPG->UpdateParent(explode_matrix,vel);
	pStaticPG->Play();

	//�������� ��������� �� ������
	if(m_dwLightTime>0)
	{
		m_pLight->set_color(m_LightColor.r, 
			m_LightColor.g, 
			m_LightColor.b);
		m_pLight->set_range(m_fLightRange);
		m_pLight->set_position(pos); 
		m_pLight->set_active(true);
	}

	//trace frags
	Fvector frag_dir; 
	
	//////////////////////////////
	//�������
	//////////////////////////////
	for(int i = 0; i < m_iFragsNum; ++i) 
	{
		frag_dir.random_dir();
		frag_dir.normalize();
		
		float		m_fCurrentFireDist = m_fFragsRadius;
		float		m_fCurrentHitPower = m_fFragHit;
		float		m_fCurrentHitImpulse = m_fFragHitImpulse;
		ALife::EHitType m_eCurrentHitType = m_eHitTypeFrag;
		float		m_fCurrentWallmarkSize = fWallmarkSize;
		Fvector		m_vCurrentShootDir = frag_dir;
		Fvector		m_vCurrentShootPos = pos;
		
		CCartridge cartridge;
		cartridge.m_kDist = 1.f;
		cartridge.m_kHit = 1.f;
		cartridge.m_kImpulse = 1.f;
		cartridge.m_kPierce = 1.f;
		cartridge.fWallmarkSize = m_fCurrentWallmarkSize;

		Level().BulletManager().AddBullet(	m_vCurrentShootPos, m_vCurrentShootDir, tracerHeadSpeed,
											m_fCurrentHitPower, m_fCurrentHitImpulse, m_iCurrentParentID,
											ID(), m_eCurrentHitType, m_fCurrentFireDist, cartridge, tracerMaxLength);
	}	

	if (Remote()) return;
	
	/////////////////////////////////
	//�������� �����
	////////////////////////////////

	Fvector l_goPos;
	Fvector l_dir;
	float l_dst;

	m_blasted.clear();
	feel_touch_update(pos, m_fBlastRadius);
	
	xr_list<s16>		l_elements;
	xr_list<Fvector>	l_bs_positions;
	
	while(m_blasted.size()) 
	{
		CGameObject *l_pGO = *m_blasted.begin();
		
		if(l_pGO->Visual()) 
			l_pGO->Center(l_goPos); 
		else 
			l_goPos.set(l_pGO->Position());
		
		l_dir.sub(l_goPos, pos); 
		l_dst = l_dir.magnitude(); 
		l_dir.div(l_dst); 
		l_dir.y += m_fUpThrowFactor;

		f32 l_S = (l_pGO->Visual()?l_pGO->Radius()*l_pGO->Radius():0);
		
		if(l_pGO->Visual()) 
		{
			const Fbox &l_b1 = l_pGO->BoundingBox(); 
			Fbox l_b2; 
			l_b2.invalidate();
			Fmatrix l_m; l_m.identity(); 
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
		
		float l_impuls	= m_fBlastHitImpulse * (1.f - (l_dst/m_fBlastRadius)*(l_dst/m_fBlastRadius)) * l_S;
		float l_hit		= m_fBlastHit * (1.f - (l_dst/m_fBlastRadius)*(l_dst/m_fBlastRadius)) * l_S;

		if(l_impuls > .001f) 
		{
			l_impuls *= l_pGO->ExplosionEffect(pos, m_fBlastRadius, l_elements, l_bs_positions);
		}

		if(l_impuls > .001f) 
		{
			while(l_elements.size()) 
			{
				s16 l_element = *l_elements.begin();
				Fvector l_bs_pos = *l_bs_positions.begin();
				NET_Packet		P;
				u_EventGen		(P,GE_HIT,l_pGO->ID());
				P.w_u16			(m_iCurrentParentID);
				P.w_u16			(ID());
				P.w_dir			(l_dir);
				P.w_float		(l_hit);
				P.w_s16			(l_element);
				P.w_vec3		(l_bs_pos);
				P.w_float		(l_impuls);
				P.w_u16			(u16(m_eHitTypeBlast));
				u_EventSend		(P);
				l_elements.pop_front();
				l_bs_positions.pop_front();
			}
		}
		m_blasted.pop_front();
	}	

	//////////////////////////////////////////////////////////////////////////
	// Explode Effector	//////////////
	CActor* pActor =  dynamic_cast<CActor*>(Level().CurrentEntity());
	if(pActor)
	{
		float dist_to_actor = pActor->Position().distance_to(pos);
		float max_dist		= EFFECTOR_RADIUS;
		if (dist_to_actor < max_dist) 
			pActor->EffectorManager().AddEffector(xr_new<CExplodeEffector>(effector.time, effector.amplitude, effector.period_number, (max_dist - dist_to_actor) / max_dist));
	}
	//////////////////////////////////
	
}

void CExplosive::feel_touch_new(CObject* O) 
{
	CGameObject *pGameObject = dynamic_cast<CGameObject*>(O);
	if(pGameObject && this != pGameObject) m_blasted.push_back(pGameObject);
}

void CExplosive::UpdateCL() 
{
	//����� �����, �������� ��� ������
	if(m_dwExplodeDuration > 0 && m_dwExplodeDuration <= Device.dwTimeDelta) 
	{
		m_dwExplodeDuration = 0;
		m_pLight->set_active(false);
		
		//������������� ��� ������ 
		NET_Packet			P;
		u_EventGen			(P,GE_DESTROY,ID());
//		Msg					("ge_destroy: [%d] - %s",ID(),*cName());
		if (Local()) u_EventSend			(P);
	} 
	else if(m_dwExplodeDuration>0 && m_dwExplodeDuration < 0xffffffff) 
	{
		m_dwExplodeDuration -= Device.dwTimeDelta;
		
		if(m_dwExplodeDuration > (m_dwExplodeDurationMax - m_dwLightTime)) 
		{
			if(m_dwLightTime>0)
			{
				float scale = float(m_dwExplodeDuration - 
					(m_dwExplodeDurationMax - m_dwLightTime))/float(m_dwLightTime);
				m_pLight->set_color(m_LightColor.r*scale, 
					m_LightColor.g*scale, 
					m_LightColor.b*scale);

				m_pLight->set_range(m_fLightRange*scale);
			}
		} 
//		else m_pLight->set_range(0);
	} 
	else m_pLight->set_active(false);
}

void CExplosive::OnEvent(NET_Packet& P, u16 type) 
{
	switch (type) {
		case GE_GRENADE_EXPLODE : {
			Explode();
			m_dwExplodeDuration = m_dwExplodeDurationMax;
			break;
		}
	}
}

void  CExplosive::ExplodeParams(const Fvector& pos, 
								const Fvector& dir)
{
	m_bReadyToExplode = true;
	m_vExplodePos = pos;
	m_vExplodeDir = dir;
}


void CExplosive::renderable_Render		()
{
	inherited::renderable_Render	();
}
