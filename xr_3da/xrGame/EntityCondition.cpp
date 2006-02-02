// EntityCondition.cpp: ����� ��������� ����� ��������
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "entitycondition.h"
#include "inventoryowner.h"
#include "customoutfit.h"
#include "inventory.h"
#include "wound.h"
#include "level.h"
#include "game_cl_base.h"
#include "entity_alive.h"
#include "..\SkeletonCustom.h"
#include "object_broker.h"

#define MAX_HEALTH 1.0f
#define MIN_HEALTH -0.01f

#define MAX_SATIETY 1.0f

#define MAX_POWER 1.0f
#define MAX_RADIATION 1.0f
#define MAX_PSY_HEALTH 1.0f

CEntityConditionSimple::CEntityConditionSimple()
{
	max_health()		= MAX_HEALTH;
	health()			= MAX_HEALTH;
}

CEntityConditionSimple::~CEntityConditionSimple()
{}

CEntityCondition::CEntityCondition(CEntityAlive *object)
:CEntityConditionSimple()
{
	VERIFY				(object);

	m_object			= object;

	m_use_limping_state = false;
	m_iLastTimeCalled	= 0;
	m_bTimeValid		= false;

	m_fPowerMax			= MAX_POWER;
	m_fSatietyMax		= MAX_SATIETY;
	m_fRadiationMax		= MAX_RADIATION;
	m_fPsyHealthMax		= MAX_PSY_HEALTH;

	m_fCircumspection	= m_fCircumspectionMax = 1.f;
	m_fEntityMorale		=  m_fEntityMoraleMax = 1.f;
	m_fV_Circumspection =  0.01f;
	m_fV_EntityMorale	= 0.01f;
	m_fV_PsyHealth		= 0.01f;


	m_fPower			= MAX_POWER;
	m_fSatiety			= MAX_SATIETY;
	m_fRadiation		= 0;
	m_fPsyHealth		= MAX_PSY_HEALTH;

	m_fV_SatietyHealth	= 0.001f;
	m_fV_SatietyPower	= 0.01f;
	m_fV_Satiety		= 0.004f;

	m_fV_RadiationHealth = 0.006f;
	m_fV_Radiation		= 0.004f;

	m_fV_Bleeding			= 0.09f;
	m_fV_WoundIncarnation	= 0.001f;
	m_fMinWoundSize			= 0.00001f;

	
	m_fHealthHitPart		= 1.0f;
	m_fPowerHitPart			= 0.5f;

	m_fDeltaHealth			= 0;
	m_fDeltaPower			= 0;
	m_fDeltaSatiety			= 0;
	m_fDeltaRadiation		= 0;
	m_fDeltaPsyHealth		= 0;


	m_fHealthLost			= 0.f;
	m_pWho					= NULL;
	m_iWhoID				= 0;

	m_WoundVector.clear		();


	m_fHitBoneScale			= 1.f;
	m_fWoundBoneScale		= 1.f;

	m_bIsBleeding			= false;
}

CEntityCondition::~CEntityCondition(void)
{
	ClearWounds				();
}

void CEntityCondition::ClearWounds()
{
	for(WOUND_VECTOR_IT it = m_WoundVector.begin(); m_WoundVector.end() != it; ++it)
		xr_delete(*it);
	m_WoundVector.clear();

	m_bIsBleeding = false;
}

void CEntityCondition::LoadCondition(LPCSTR entity_section)
{
	LPCSTR				section = READ_IF_EXISTS(pSettings,r_string,entity_section,"condition_sect",entity_section);


	//�������� ��������� ������������� ���������
	//� �������
	m_fV_Satiety			= pSettings->r_float(section,"satiety_v");		
	m_fV_Radiation			= pSettings->r_float(section,"radiation_v");	
	m_fV_SatietyPower		= pSettings->r_float(section,"satiety_power_v");
	m_fV_SatietyHealth		= pSettings->r_float(section,"satiety_health_v");
	m_fSatietyCritical		= pSettings->r_float(section,"satiety_critical");

	m_fV_PsyHealth			= pSettings->r_float(section,"psy_health_v");	

	m_fV_RadiationHealth	= pSettings->r_float(section,"radiation_health_v");
	m_fV_Bleeding			= pSettings->r_float(section,"bleeding_v");
	m_fV_WoundIncarnation	= pSettings->r_float(section,"wound_incarnation_v");
	m_fMinWoundSize			= pSettings->r_float(section,"min_wound_size");
	m_fV_EntityMorale		= pSettings->r_float(section,"morale_v");
	
	m_fHealthHitPart		= pSettings->r_float(section,"health_hit_part");
	m_fPowerHitPart			= pSettings->r_float(section,"power_hit_part");
	


	m_use_limping_state		= !!(READ_IF_EXISTS(pSettings,r_bool,section,"use_limping_state",FALSE));
	m_limping_threshold		= READ_IF_EXISTS(pSettings,r_float,section,"limping_threshold",.5f);
}

void CEntityCondition::reinit	()
{
	m_iLastTimeCalled		= 0;
	m_bTimeValid			= false;

	max_health()			= MAX_HEALTH;
	m_fPowerMax				= MAX_POWER;
	m_fSatietyMax			= MAX_SATIETY;
	m_fRadiationMax			= MAX_RADIATION;
	m_fPsyHealthMax			= MAX_PSY_HEALTH;

	m_fCircumspection		= m_fCircumspectionMax = 1.f;
	m_fEntityMorale			=  m_fEntityMoraleMax = 1.f;

	health()				= MAX_HEALTH;
	m_fPower				= MAX_POWER;
	m_fSatiety				= MAX_SATIETY;
	m_fRadiation			= 0;
	m_fPsyHealth			= MAX_PSY_HEALTH;

	m_fDeltaHealth			= 0;
	m_fDeltaPower			= 0;
	m_fDeltaSatiety			= 0;
	m_fDeltaRadiation		= 0;
	m_fDeltaCircumspection	= 0;
	m_fDeltaEntityMorale	= 0;
	m_fDeltaPsyHealth		= 0;


	m_fHealthLost			= 0.f;
	m_pWho					= NULL;
	m_iWhoID				= NULL;

	ClearWounds				();

}

void CEntityCondition::ChangeHealth(float value)
{
	VERIFY(_valid(value));
	m_fDeltaHealth += value;
}
void CEntityCondition::ChangePower(float value)
{
	m_fDeltaPower += value;
}
void CEntityCondition::ChangeSatiety(float value)
{
	m_fDeltaSatiety += value;
}
void CEntityCondition::ChangeRadiation(float value)
{
	m_fDeltaRadiation += value;
}

void CEntityCondition::ChangePsyHealth(float value)
{
	m_fDeltaPsyHealth += value;
}


void CEntityCondition::ChangeCircumspection(float value)
{
	m_fDeltaCircumspection += value;
}
void CEntityCondition::ChangeEntityMorale(float value)
{
	m_fDeltaEntityMorale += value;
}


void CEntityCondition::ChangeBleeding(float percent)
{
	//�������� ����
	for(WOUND_VECTOR_IT it = m_WoundVector.begin(); m_WoundVector.end() != it; ++it)
	{
		(*it)->Incarnation			(percent, m_fMinWoundSize);
		if(0 == (*it)->TotalSize	())
			(*it)->SetDestroy		(true);
	}
}
bool RemoveWoundPred(CWound* pWound)
{
	if(pWound->GetDestroy())
	{
		xr_delete		(pWound);
		return			true;
	}
	return				false;
}

void  CEntityCondition::UpdateWounds		()
{
	//������ ��� �������� ���� �� ������
	WOUND_VECTOR_IT last_it = remove_if(m_WoundVector.begin(), m_WoundVector.end(),RemoveWoundPred);
	m_WoundVector.erase(last_it, m_WoundVector.end());
}

void CEntityCondition::UpdateConditionTime()
{
	u64 cur_time = (GameID() == GAME_SINGLE) ? Level().GetGameTime() : Level().timeServer();

	if(m_bTimeValid)
	{
		if (cur_time > m_iLastTimeCalled)
			m_iDeltaTime		= cur_time - m_iLastTimeCalled;
		else 
			m_iDeltaTime		= 0;
	}
	else
	{
		m_iDeltaTime			= 0;
		m_bTimeValid			= true;

		m_fDeltaHealth			= 0;
		m_fDeltaPower			= 0;
		m_fDeltaSatiety			= 0;
		m_fDeltaRadiation		= 0;
		m_fDeltaCircumspection	= 0;
		m_fDeltaEntityMorale	= 0;

	}

	m_iLastTimeCalled			= cur_time;
}

//���������� ���������� � ����� �������� �������
void CEntityCondition::UpdateCondition()
{
	if(GetHealth()<=0)			return;
	//-----------------------------------------
	bool CriticalHealth			= false;

	if (m_fDeltaHealth+GetHealth() <= 0)
	{
		CriticalHealth			= true;
		if (Game().EntityCanBeHarmed(m_object)) m_object->OnCriticalHitHealthLoss();
	}
	else
	{
		if (m_fDeltaHealth<0 && Game().EntityCanBeHarmed(m_object)) m_object->OnHitHealthLoss(GetHealth()+m_fDeltaHealth);
	}
	//-----------------------------------------
	UpdateHealth				();
	//-----------------------------------------
	if (!CriticalHealth && m_fDeltaHealth+GetHealth() <= 0)
	{
		CriticalHealth			= true;
		if(Game().EntityCanBeHarmed(m_object)) m_object->OnCriticalWoundHealthLoss();
	};
	//-----------------------------------------
	UpdatePower					();
	UpdateSatiety				();
	UpdateRadiation				();
	//-----------------------------------------
	if (!CriticalHealth && m_fDeltaHealth+GetHealth() <= 0)
	{
		CriticalHealth = true;
		if(Game().EntityCanBeHarmed(m_object)) m_object->OnCriticalRadiationHealthLoss();
	};
	//-----------------------------------------
	UpdatePsyHealth				();

	UpdateCircumspection		();
	UpdateEntityMorale			();

	health()					+= Game().EntityCanBeHarmed(m_object) ? m_fDeltaHealth : 0;
	m_fPower					+= m_fDeltaPower;
	m_fSatiety					+= m_fDeltaSatiety;
	m_fRadiation				+= m_fDeltaRadiation;
	m_fPsyHealth				+= m_fDeltaPsyHealth;
	m_fCircumspection			+= m_fDeltaCircumspection;
	m_fEntityMorale				+= m_fDeltaEntityMorale;

	m_fDeltaHealth				= 0;
	m_fDeltaPower				= 0;
	m_fDeltaSatiety				= 0;
	m_fDeltaRadiation			= 0;
	m_fDeltaPsyHealth			= 0;
	m_fDeltaCircumspection		= 0;
	m_fDeltaEntityMorale		= 0;

	clamp						(health(),			MIN_HEALTH, max_health());
	clamp						(m_fPower,			0.0f,		m_fPowerMax);
	clamp						(m_fRadiation,		0.0f,		m_fRadiationMax);
	clamp						(m_fSatiety,		0.0f,		m_fSatietyMax);
	clamp						(m_fCircumspection,	0.0f,		m_fCircumspectionMax);
	clamp						(m_fEntityMorale,	0.0f,		m_fEntityMoraleMax);
	clamp						(m_fPsyHealth,		0.0f,		m_fPsyHealthMax);
}



float CEntityCondition::HitOutfitEffect(float hit_power, ALife::EHitType hit_type, s16 element, float AP)
{
    CInventoryOwner* pInvOwner		= smart_cast<CInventoryOwner*>(m_object);
	if(!pInvOwner)					return hit_power;

	CCustomOutfit* pOutfit			= (CCustomOutfit*)pInvOwner->inventory().m_slots[OUTFIT_SLOT].m_pIItem;
	if(!pOutfit)					return hit_power;

	float new_hit_power				= hit_power;

	if (hit_type == ALife::eHitTypeFireWound)// && GameID() != GAME_SINGLE)
		new_hit_power				= pOutfit->HitThruArmour(hit_power, element, AP);
	else
		new_hit_power				*= pOutfit->GetHitTypeProtection(hit_type,element);
	
	//��������� ������������ �������
	pOutfit->Hit					(hit_power, hit_type);

	return							new_hit_power;
}

float CEntityCondition::HitPowerEffect(float power_loss)
{
	CInventoryOwner* pInvOwner		 = smart_cast<CInventoryOwner*>(m_object);
	if(!pInvOwner)					 return power_loss;

	CCustomOutfit* pOutfit			= (CCustomOutfit*)pInvOwner->inventory().m_slots[OUTFIT_SLOT].m_pIItem;
	if(!pOutfit)					return power_loss;

	float new_power_loss			= power_loss*pOutfit->GetPowerLoss();

	return							new_power_loss;
}

CWound* CEntityCondition::AddWound(float hit_power, ALife::EHitType hit_type, u16 element)
{
	//������������ ����� �������� 64
	VERIFY(element  < 64 || BI_NONE == element);

	//��������� ����� �� ������� ������� � ���� �����
	WOUND_VECTOR_IT it = m_WoundVector.begin();
	for(;it != m_WoundVector.end(); it++)
	{
		if((*it)->GetBoneNum() == element)
			break;
	}
	
	CWound* pWound = NULL;

	//����� ����
	if (it == m_WoundVector.end())
	{
		pWound = xr_new<CWound>(element);
		pWound->AddHit(hit_power*::Random.randF(0.5f,1.5f), hit_type);
		m_WoundVector.push_back(pWound);
	}
	//������ 
	else
	{
		pWound = *it;
		pWound->AddHit(hit_power*::Random.randF(0.5f,1.5f), hit_type);
	}

	VERIFY(pWound);
	return pWound;
}

//CWound* CEntityCondition::ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, u16 element)
CWound* CEntityCondition::ConditionHit(SHit* pHDS)
{
	//��� ����� ��������� ���
	m_pWho = pHDS->who;
	m_iWhoID = (NULL != pHDS->who) ? pHDS->who->ID() : 0;

	float hit_power_org = pHDS->damage();
	float hit_power = hit_power_org;
	//����������� ���� �����
//	hit_power = hit_power/100.f;
	hit_power = HitOutfitEffect(hit_power, pHDS->hit_type, pHDS->boneID, pHDS->ap);

	bool bAddWound = true;
	switch(pHDS->hit_type)
	{
	case ALife::eHitTypeTelepatic:
		// -------------------------------------------------
		// temp (till there is no death from psy hits)
		hit_power *= m_HitTypeK[pHDS->hit_type];
		m_fHealthLost = hit_power*m_fHealthHitPart*m_fHitBoneScale;
		m_fDeltaHealth -= m_fHealthLost;
		m_fDeltaPower -= hit_power*m_fPowerHitPart;
		// -------------------------------------------------

		hit_power *= m_HitTypeK[pHDS->hit_type];
		ChangePsyHealth(-hit_power);
		bAddWound =false;
		break;
	case ALife::eHitTypeBurn:
		hit_power *= m_HitTypeK[pHDS->hit_type];
		m_fHealthLost = hit_power*m_fHealthHitPart*m_fHitBoneScale;
		m_fDeltaHealth -= m_fHealthLost;
		m_fDeltaPower -= hit_power*m_fPowerHitPart;
		bAddWound		=  false;
		break;
	case ALife::eHitTypeChemicalBurn:
		hit_power *= m_HitTypeK[pHDS->hit_type];
		break;
	case ALife::eHitTypeShock:
		hit_power		*= m_HitTypeK[pHDS->hit_type];
		m_fHealthLost	=  hit_power*m_fHealthHitPart;
		m_fDeltaHealth	-= m_fHealthLost;
		m_fDeltaPower	-= hit_power*m_fPowerHitPart;
		bAddWound		=  false;
		break;
	case ALife::eHitTypeRadiation:
		/*hit_power *= m_HitTypeK[pHDS->hit_type];*/
		m_fDeltaRadiation += hit_power;
		return NULL;
		break;
	case ALife::eHitTypeExplosion:
	case ALife::eHitTypeStrike:
		hit_power *= m_HitTypeK[pHDS->hit_type];
		m_fHealthLost = hit_power*m_fHealthHitPart;
		m_fDeltaHealth -= m_fHealthLost;
		m_fDeltaPower -= hit_power*m_fPowerHitPart;
		break;
	case ALife::eHitTypeFireWound:
	case ALife::eHitTypeWound:
		hit_power *= m_HitTypeK[pHDS->hit_type];
		m_fHealthLost = hit_power*m_fHealthHitPart*m_fHitBoneScale;
		m_fDeltaHealth -= m_fHealthLost;
		m_fDeltaPower -= hit_power*m_fPowerHitPart;
		break;
	}

	if (bDebug) Msg("%s hitted in %s with %f[%f]", m_object->Name(), smart_cast<CKinematics*>(m_object->Visual())->LL_BoneName_dbg(pHDS->boneID), m_fHealthLost*100.0f, hit_power_org);
	//���� ����������� ������ ������
	if(bAddWound && GetHealth()>0)
		return AddWound(hit_power*m_fWoundBoneScale, pHDS->hit_type, pHDS->boneID);
	else
		return NULL;
}


float CEntityCondition::BleedingSpeed()
{
	float bleeding_speed		=0;

	for(WOUND_VECTOR_IT it = m_WoundVector.begin(); m_WoundVector.end() != it; ++it)
		bleeding_speed			+= (*it)->TotalSize();
	
	
	return (m_WoundVector.empty() ? 0.f : bleeding_speed / m_WoundVector.size());
}


void CEntityCondition::UpdateHealth()
{
	float delta_time			= float(m_iDeltaTime)/1000.f;
	
	float bleeding_speed		= BleedingSpeed() * delta_time * m_fV_Bleeding;
	m_bIsBleeding				= fis_zero(bleeding_speed)?false:true;
	m_fDeltaHealth				-= bleeding_speed;
	VERIFY						(_valid(m_fDeltaHealth));
	ChangeBleeding				(m_fV_WoundIncarnation * delta_time);
}

void CEntityCondition::UpdatePower()
{
}

void CEntityCondition::UpdatePsyHealth(float k)
{
	if(m_fPsyHealth>0)
	{
		m_fDeltaPsyHealth += m_fV_PsyHealth*k*m_iDeltaTime/1000;
	}
}


void CEntityCondition::UpdateSatiety(float k)
{
	if (GameID() != GAME_SINGLE) return;

//	float sleep_k = 1.f;
	if(m_fSatiety>0)
	{
		m_fDeltaSatiety -=	m_fV_Satiety*
							k*
							m_iDeltaTime/1000;

//		sleep_k = m_fCurrentSleepHealth;
	}
		
	//������� ����������� �������� ������ ���� ��� �������� ���
	if(!m_bIsBleeding)
	{
		m_fDeltaHealth += m_fV_SatietyHealth*
					(m_fSatiety>m_fSatietyCritical?1.f:-1.f)*
//					sleep_k*
					m_iDeltaTime/1000;
	}

	//������������ ���������� �������������� ���� �� ������ � ��������
	float radiation_power_k		= 1.f;
	float satiety_power_k		= 1.f;
			
	m_fDeltaPower += m_fV_SatietyPower*
				radiation_power_k*
				satiety_power_k*
//				m_fCurrentSleepPower*
				m_iDeltaTime/1000;
}

void CEntityCondition::UpdateRadiation(float k)
{
	if(m_fRadiation>0)
	{
		m_fDeltaRadiation -= m_fV_Radiation*
							k*
							m_iDeltaTime/1000;

		m_fDeltaHealth -= m_fV_RadiationHealth*m_fRadiation*m_iDeltaTime/1000;
	}
}

void CEntityCondition::UpdateCircumspection()
{
	if(m_fCircumspection<m_fCircumspectionMax)
	{
		m_fDeltaCircumspection += m_fV_Circumspection*
								 m_iDeltaTime/1000;
	}
}
void CEntityCondition::UpdateEntityMorale()
{
	if(m_fEntityMorale<m_fEntityMoraleMax)
	{
		m_fDeltaEntityMorale += m_fV_EntityMorale*
						  m_iDeltaTime/1000;
	}
}



bool CEntityCondition::IsLimping() const
{
	if (!m_use_limping_state)
		return	(false);
	return (m_fPower*GetHealth() <= m_limping_threshold);
}

void CEntityCondition::save	(NET_Packet &output_packet)
{
	u8 is_alive	= (GetHealth()>0.f)?1:0;
	
	output_packet.w_u8	(is_alive);
	if(is_alive)
	{
		save_data						(m_fPower,output_packet);
		save_data						(m_fSatiety,output_packet);
		save_data						(m_fRadiation,output_packet);
		save_data						(m_fEntityMorale,output_packet);
		save_data						(m_fCircumspection,output_packet);
		save_data						(m_fPsyHealth,output_packet);

		output_packet.w_u8				((u8)m_WoundVector.size());
		for(WOUND_VECTOR_IT it = m_WoundVector.begin(); m_WoundVector.end() != it; it++)
			(*it)->save(output_packet);
	}
}

void CEntityCondition::load	(IReader &input_packet)
{
	m_bTimeValid = false;

	u8 is_alive				= input_packet.r_u8	();
	if(is_alive)
	{
		load_data						(m_fPower,input_packet);
		load_data						(m_fSatiety,input_packet);
		load_data						(m_fRadiation,input_packet);
		load_data						(m_fEntityMorale,input_packet);
		load_data						(m_fCircumspection,input_packet);
		load_data						(m_fPsyHealth,input_packet);

		ClearWounds();
		m_WoundVector.resize(input_packet.r_u8());
		if(!m_WoundVector.empty())
			for(u32 i=0; i<m_WoundVector.size(); i++)
			{
				CWound* pWound = xr_new<CWound>(BI_NONE);
				pWound->load(input_packet);
				m_WoundVector[i] = pWound;
			}
	}
}