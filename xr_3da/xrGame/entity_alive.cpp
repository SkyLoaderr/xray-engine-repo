#include "stdafx.h"
#include "entity_alive.h"
#include "inventoryowner.h"
#include "inventory.h"
#include "physicsshell.h"
#include "gamemtllib.h"
#include "phmovementcontrol.h"
#include "wound.h"
#include "xrmessages.h"
#include "level.h"


#define SMALL_ENTITY_RADIUS		0.6f


//отметки крови на стенах 
SHADER_VECTOR* CEntityAlive::m_pBloodMarksVector = NULL;
float CEntityAlive::m_fBloodMarkSizeMin = 0.f;
float CEntityAlive::m_fBloodMarkSizeMax = 0.f;
float CEntityAlive::m_fBloodMarkDistance = 0.f;
float CEntityAlive::m_fNominalHit = 0.f;

//капание крови
SHADER_VECTOR* CEntityAlive::m_pBloodDropsVector = NULL;
float CEntityAlive::m_fStartBloodWoundSize = 0.3f;
float CEntityAlive::m_fStopBloodWoundSize = 0.1f;
float CEntityAlive::m_fBloodDropSize = 0.03f;


//минимальный размер ожега, после которого горят партиклы
//минимальное время горения
u32	  CEntityAlive::m_dwMinBurnTime = 10000;
//размер раны, чтоб запустить партиклы
float CEntityAlive::m_fStartBurnWoundSize = 0.3f;
//размер раны, чтоб остановить партиклы
float CEntityAlive::m_fStopBurnWoundSize = 0.1f;
//время через которое с раны размером 1.0 будет падать капля крови
float CEntityAlive::m_fBloodDropTime = 0.9f;

STR_VECTOR* CEntityAlive::m_pFireParticlesVector = NULL;

/////////////////////////////////////////////
// CEntityAlive
/////////////////////////////////////////////
CEntityAlive::CEntityAlive()
{
	init				();
}

CEntityAlive::~CEntityAlive()
{
	xr_delete				(m_PhysicMovementControl);
}

void CEntityAlive::init			()
{
	m_PhysicMovementControl = xr_new<CPHMovementControl>();
}

void CEntityAlive::Load		(LPCSTR section)
{
	CEntity::Load			(section);
	CEntityCondition::Load	(section);
	m_fFood					= 100*pSettings->r_float	(section,"ph_mass");

	//bloody wallmarks
	if(0==m_pBloodMarksVector)
		LoadBloodyWallmarks ("bloody_marks");

	if(0==m_pFireParticlesVector)
		LoadFireParticles	("entity_fire_particles");
}

void CEntityAlive::LoadBloodyWallmarks (LPCSTR section)
{
	VERIFY					(0==m_pBloodMarksVector);
	VERIFY					(0==m_pBloodDropsVector);
	m_pBloodMarksVector		= xr_new<SHADER_VECTOR>();
	m_pBloodDropsVector		= xr_new<SHADER_VECTOR>();
	
	//кровавые отметки на стенах
	string256	tmp;
	LPCSTR wallmarks_name = pSettings->r_string(section, "wallmarks"); 
	
	int cnt		=_GetItemCount(wallmarks_name);
	
	ref_shader	s;
	for (int k=0; k<cnt; ++k)
	{
		s.create ("effects\\wallmark",_GetItem(wallmarks_name,k,tmp));
		m_pBloodMarksVector->push_back	(s);
	}

	
	m_fBloodMarkSizeMin = pSettings->r_float(section, "min_size"); 
	m_fBloodMarkSizeMax = pSettings->r_float(section, "max_size"); 
	m_fBloodMarkDistance = pSettings->r_float(section, "dist"); 
	m_fNominalHit = pSettings->r_float(section, "nominal_hit"); 



	//капли крови с открытых ран
	wallmarks_name = pSettings->r_string(section, "blood_drops");
	cnt		=_GetItemCount(wallmarks_name);

	for (int k=0; k<cnt; ++k)
	{
		s.create ("effects\\wallmark",_GetItem(wallmarks_name,k,tmp));
		m_pBloodDropsVector->push_back	(s);
	}

	m_fBloodDropTime		= pSettings->r_float(section, "blood_drop_time");	
	m_fStartBloodWoundSize  = pSettings->r_float(section, "start_blood_size");
	m_fStopBloodWoundSize   = pSettings->r_float(section, "stop_blood_size");
	m_fBloodDropSize		= pSettings->r_float(section, "blood_drop_size");
}

void CEntityAlive::UnloadBloodyWallmarks	()
{
	if (m_pBloodMarksVector){ 
		m_pBloodMarksVector->clear	();
		xr_delete					(m_pBloodMarksVector);
	}
	if (m_pBloodDropsVector){
		m_pBloodDropsVector->clear	();
		xr_delete					(m_pBloodDropsVector);
	}
}

void CEntityAlive::LoadFireParticles(LPCSTR section)
{
	m_pFireParticlesVector = xr_new<STR_VECTOR>();

	string256	tmp;
	LPCSTR particles_name = pSettings->r_string(section, "fire_particles"); 

	int cnt		=_GetItemCount(particles_name);

	ref_str	s;
	for (int k=0; k<cnt; ++k)
	{
		s  = _GetItem(particles_name,k,tmp);
		m_pFireParticlesVector->push_back	(s);
	}

	m_fStartBurnWoundSize  = pSettings->r_float(section, "start_burn_size");
	m_fStopBurnWoundSize   = pSettings->r_float(section, "stop_burn_size");
	
	m_dwMinBurnTime		   = pSettings->r_u32(section, "min_burn_time");
}

void CEntityAlive::UnloadFireParticles()
{
	m_pFireParticlesVector->clear();
	xr_delete(m_pFireParticlesVector);
}

void CEntityAlive::reinit			()
{
	CEntity::reinit			();
	CEntityCondition::reinit();

	m_fAccuracy				= 25.f;
	m_fIntelligence			= 25.f;
}

void CEntityAlive::reload		(LPCSTR section)
{
	CEntity::reload			(section);
//	CEntityCondition::reload(section);

	m_fFood					= 100*pSettings->r_float	(section,"ph_mass");
}

void CEntityAlive::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);

	//condition update with the game time pass
	UpdateCondition		();
	//Обновление партиклов огня
	UpdateFireParticles	();
	//обновить раны
	CEntityCondition::UpdateWounds		();
	//капли крови
	UpdateBloodDrops	();

	//убить сущность
	if(Local() && !g_Alive() && !AlreadyDie())
	{
		if(GetWhoHitLastTime()) {
//			Msg			("%6d : KillEntity from CEntityAlive (using who hit last time) for object %s",Level().timeServer(),*cName());
			KillEntity(GetWhoHitLastTime());
		}
		else {
//			Msg			("%6d : KillEntity from CEntityAlive for object %s",Level().timeServer(),*cName());
			KillEntity(this);
		}
	}
}

BOOL CEntityAlive::net_Spawn	(LPVOID DC)
{
	inherited::net_Spawn	(DC);

	//добавить кровь и огонь на партиклы, если нужно
	for(WOUND_VECTOR_IT it = m_WoundVector.begin(); m_WoundVector.end() != it; ++it)
	{
		CWound* pWound = *it;
		StartFireParticles(pWound);
		StartBloodDrops(pWound);
	}
	return					TRUE;
}

void CEntityAlive::net_Destroy	()
{
	inherited::net_Destroy		();
}

void CEntityAlive::HitImpulse	(float /**amount/**/, Fvector& /**vWorldDir/**/, Fvector& /**vLocalDir/**/)
{
	//	float Q					= 2*float(amount)/m_PhysicMovementControl->GetMass();
	//	m_PhysicMovementControl->vExternalImpulse.mad	(vWorldDir,Q);
}

void CEntityAlive::Hit(float P, Fvector &dir,CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type)
{
	HitScale(element, m_fHitBoneScale, m_fWoundBoneScale);

	//изменить состояние, перед тем как родительский класс обработает хит
	CWound* pWound = ConditionHit(who, P, hit_type, element);

	if(pWound)
	{
		if(ALife::eHitTypeBurn == hit_type)
			StartFireParticles(pWound);
		else if(ALife::eHitTypeWound == hit_type || ALife::eHitTypeFireWound == hit_type)
			StartBloodDrops(pWound);
	}

	//добавить кровь на стены
	BloodyWallmarks (P, dir, element, position_in_object_space);

	inherited::Hit(P,dir,who,element,position_in_object_space,impulse, hit_type);
}
/* not used
void CEntityAlive::BuyItem(LPCSTR buf)
{
	NET_Packet P;
	u_EventGen	(P,GE_BUY,ID());
	P.w_stringZ	(buf);
	u_EventSend	(P);
}*/

//вывзывает при подсчете хита
float CEntityAlive::CalcCondition(float /**hit/**/)
{	
	UpdateCondition();

	//dont call inherited::CalcCondition it will be meaningless
	return GetHealthLost()*100.f;
}

///////////////////////////////////////////////////////////////////////
u16	CEntityAlive::PHGetSyncItemsNumber()
{
	if(m_PhysicMovementControl->CharacterExist()) return 1;
	else										  return inherited::PHGetSyncItemsNumber();
}
CPHSynchronize* CEntityAlive::PHGetSyncItem	(u16 item)
{
	if(m_PhysicMovementControl->CharacterExist()) return m_PhysicMovementControl->GetSyncItem();
	else										 return inherited::PHGetSyncItem(item);
}
void CEntityAlive::PHUnFreeze()
{
	if(m_PhysicMovementControl->CharacterExist()) m_PhysicMovementControl->UnFreeze();
	else if(m_pPhysicsShell) m_pPhysicsShell->UnFreeze();
}
void CEntityAlive::PHFreeze()
{
	if(m_PhysicMovementControl->CharacterExist()) m_PhysicMovementControl->Freeze();
	else if(m_pPhysicsShell) m_pPhysicsShell->Freeze();
}
//////////////////////////////////////////////////////////////////////

//добавление кровавых отметок на стенах, после получения хита
void CEntityAlive::BloodyWallmarks (float P, const Fvector &dir, s16 element, 
									const Fvector& position_in_object_space)
{
	if(BI_NONE == (u16)element)
		return;

	//вычислить координаты попадания
	CKinematics* V = smart_cast<CKinematics*>(Visual());
		
	Fvector start_pos = position_in_object_space;
	if(V)
	{
		Fmatrix& m_bone = (V->LL_GetBoneInstance(u16(element))).mTransform;
		m_bone.transform_tiny(start_pos);
	}
	XFORM().transform_tiny(start_pos);

	float small_entity = 1.f;
	if(Radius()<SMALL_ENTITY_RADIUS) small_entity = 0.5;


	float wallmark_size = m_fBloodMarkSizeMax;
	wallmark_size *= (P/m_fNominalHit);
	wallmark_size *= small_entity;
	clamp(wallmark_size, m_fBloodMarkSizeMin, m_fBloodMarkSizeMax);

	VERIFY(m_pBloodMarksVector);
	PlaceBloodWallmark(dir, start_pos, m_fBloodMarkDistance, 
						wallmark_size, *m_pBloodMarksVector);

}

void CEntityAlive::PlaceBloodWallmark(const Fvector& dir, const Fvector& start_pos, 
									  float trace_dist, float wallmark_size,
									  SHADER_VECTOR& wallmarks_vector)
{
	setEnabled(false);
	Collide::rq_result result;
	BOOL reach_wall = Level().ObjectSpace.RayPick(start_pos, dir, trace_dist, 
		Collide::rqtBoth, result) && !result.O;
	setEnabled(true);

	//если кровь долетела до статического объекта
	if(reach_wall)
	{
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+result.element;
		SGameMtl*	pMaterial = GMLib.GetMaterialByIdx(pTri->material);

		if(pMaterial->Flags.is(SGameMtl::flBloodmark))
		{
			//вычислить нормаль к пораженной поверхности
			Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();

			//вычислить точку попадания
			Fvector end_point;
			end_point.set(0,0,0);
			end_point.mad(start_pos, dir, result.range);

			ref_shader* pWallmarkShader = wallmarks_vector.empty()?NULL:
						&wallmarks_vector[::Random.randI(0,wallmarks_vector.size())];

			if (pWallmarkShader)
			{
				//добавить отметку на материале
				::Render->add_Wallmark(*pWallmarkShader, end_point,
					wallmark_size, pTri, pVerts);
			}
		}
	}
}



void CEntityAlive::StartFireParticles(CWound* pWound)
{
	if(pWound->TypeSize(ALife::eHitTypeBurn)>m_fStartBurnWoundSize)
	{
		if(std::find(m_ParticleWounds.begin(),
			m_ParticleWounds.end(),
			pWound) == m_ParticleWounds.end())
		{
			m_ParticleWounds.push_back(pWound);
		}

		CKinematics* V = smart_cast<CKinematics*>(Visual());

		u16 particle_bone = CParticlesPlayer::GetNearestBone(V, pWound->GetBoneNum());
		//VERIFY(BI_NONE != particle_bone);

		pWound->SetParticleBoneNum(particle_bone);
		pWound->SetParticleName((*m_pFireParticlesVector)[::Random.randI(0,m_pFireParticlesVector->size())]);

		if(BI_NONE != particle_bone)
		{
			CParticlesPlayer::StartParticles(pWound->GetParticleName(), 
				pWound->GetParticleBoneNum(),
				Fvector().set(0,1,0),
				ID(), 
				u32(float(m_dwMinBurnTime)*::Random.randF(0.5f,1.5f)), false);
		}
		else
		{
			CParticlesPlayer::StartParticles(pWound->GetParticleName(), 
				Fvector().set(0,1,0),
				ID(), 
				u32(float(m_dwMinBurnTime)*::Random.randF(0.5f,1.5f)), false);
		}
	}
}

void CEntityAlive::UpdateFireParticles()
{
	if(m_ParticleWounds.empty()) return;
	
	WOUND_VECTOR_IT last_it;

	for(WOUND_VECTOR_IT it = m_ParticleWounds.begin(); 
					  it != m_ParticleWounds.end();)
	{
		CWound* pWound = *it;
		float burn_size = pWound->TypeSize(ALife::eHitTypeBurn);

		if(pWound->GetDestroy() || 
			(burn_size>0 && (burn_size<m_fStopBurnWoundSize || !g_Alive())))
		{
			CParticlesPlayer::AutoStopParticles(pWound->GetParticleName(),
												pWound->GetParticleBoneNum());
			it = m_ParticleWounds.erase(it);
			continue;
		}
		it++;
	}
}

ALife::ERelationType CEntityAlive::tfGetRelationType	(const CEntityAlive *tpEntityAlive) const
{
	if (tpEntityAlive->g_Team() != g_Team())
		return(ALife::eRelationTypeEnemy);
	else
		return(ALife::eRelationTypeNeutral);
};


void CEntityAlive::StartBloodDrops			(CWound* pWound)
{
	if(pWound->BloodSize()>m_fStartBloodWoundSize)
	{
		if(std::find(m_BloodWounds.begin(), m_BloodWounds.end(),
			  pWound) == m_BloodWounds.end())
		{
			m_BloodWounds.push_back(pWound);
		}
	}
}

void CEntityAlive::UpdateBloodDrops()
{
	if(m_BloodWounds.empty()) return;

	if(!g_Alive())
	{
		m_BloodWounds.clear();
		return;
	}

	WOUND_VECTOR_IT last_it;

	for(WOUND_VECTOR_IT it = m_BloodWounds.begin(); 
		it != m_BloodWounds.end();)
	{
		CWound* pWound = *it;
		float blood_size = pWound->BloodSize();

		if(pWound->GetDestroy() || blood_size < m_fStopBloodWoundSize)
		{
			it =  m_BloodWounds.erase(it);
			continue;
		}

		if(!fis_zero(blood_size))
		{
			pWound->m_fUpdateTime += Device.fTimeDelta;
			float drop_time = m_fBloodDropTime*(1.f/blood_size)*Random.randF(0.8f, 1.2f);
			if(pWound->m_fUpdateTime>drop_time)
			{
				VERIFY(m_pBloodDropsVector);
				if(pWound->GetBoneNum() != BI_NONE)
				{
					Fvector pos;
					CParticlesPlayer::GetBonePos(this, pWound->GetBoneNum(), Fvector().set(0,0,0), pos);
					PlaceBloodWallmark(Fvector().set(0.f, -1.f, 0.f),
									pos, m_fBloodMarkDistance, 
									m_fBloodDropSize, *m_pBloodDropsVector);

				}
			}
		}
		it++;
	}
}



void CEntityAlive::save	(NET_Packet &output_packet)
{
	inherited::save(output_packet);
	CEntityCondition::save(output_packet);
}

void CEntityAlive::load	(IReader &input_packet)
{
	inherited::load(input_packet);
	CEntityCondition::load(input_packet);
}

BOOL	CEntityAlive::net_SaveRelevant		()
{
	return TRUE;
}