// EntityCondition.cpp: класс состояния живой сущности
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


#define MAX_HEALTH 1.0f
#define MIN_HEALTH -0.01f

#define MAX_SATIETY 1.0f
#define MAX_POWER 1.0f
#define MAX_RADIATION 1.0f



CEntityCondition::CEntityCondition(void)
{
	m_use_limping_state = false;
	m_iLastTimeCalled = 0;
	m_bTimeValid = false;

	m_fHealthMax = MAX_HEALTH;
	m_fPowerMax = MAX_POWER;
	m_fSatietyMax = MAX_SATIETY;
	m_fRadiationMax = MAX_RADIATION;

	m_fCircumspection = m_fCircumspectionMax = 1.f;
	m_fEntityMorale =  m_fEntityMoraleMax = 1.f;
	m_fV_Circumspection =  0.01f;
	m_fV_EntityMorale = 0.01f;


	m_fHealth  = MAX_HEALTH;
	m_fPower  = MAX_POWER;
	m_fSatiety = MAX_SATIETY;
	m_fRadiation  = 0;

	m_fV_SatietyHealth = 0.001f;
	m_fV_SatietyPower = 0.01f;
	m_fV_Satiety = 0.004f;

	m_fV_RadiationHealth = 0.006f;
	m_fV_Radiation = 0.004f;

	m_fV_Bleeding = 0.09f;
	m_fV_WoundIncarnation = 0.001f;
	m_fMinWoundSize = 0.00001f;

	
	m_fHealthHitPart = 1.0f;
	m_fPowerHitPart = 0.5f;

	m_fDeltaHealth = 0;
	m_fDeltaPower = 0;
	m_fDeltaSatiety = 0;
	m_fDeltaRadiation = 0;


	m_fHealthLost = 0.f;
	m_pWho = NULL;

	m_WoundVector.clear();

	Awoke();
	/*
	m_fK_SleepHealth = 1.0f;
	m_fK_SleepPower = 1.0f;
	m_fK_SleepSatiety = 1.0f;	
	m_fK_SleepRadiation = 1.0f;*/

	m_fHitBoneScale = 1.f;
	m_fWoundBoneScale = 1.f;
}

CEntityCondition::~CEntityCondition(void)
{
	ClearWounds();
}

void CEntityCondition::ClearWounds()
{
	for(WOUND_VECTOR_IT it = m_WoundVector.begin(); m_WoundVector.end() != it; ++it)
		xr_delete(*it);
	m_WoundVector.clear();
}

void CEntityCondition::LoadCondition(LPCSTR entity_section)
{
	LPCSTR section = entity_section;
	if(pSettings->line_exist(entity_section, "condition_sect"))
	{
		section = pSettings->r_string(section, "condition_sect");
	}


	//скорости изменения характеристик состояния
	//в секунду
	m_fV_Satiety = pSettings->r_float(section,"satiety_v");		
	m_fV_Radiation = pSettings->r_float(section,"radiation_v");	
	m_fV_SatietyPower = pSettings->r_float(section,"satiety_power_v");
	m_fV_SatietyHealth = pSettings->r_float(section,"satiety_health_v");
	m_fSatietyCritical = pSettings->r_float(section,"satiety_critical");

	m_fV_RadiationHealth = pSettings->r_float(section,"radiation_health_v");
	m_fV_Bleeding = pSettings->r_float(section,"bleeding_v");
	m_fV_WoundIncarnation = pSettings->r_float(section,"wound_incarnation_v");
	m_fMinWoundSize = pSettings->r_float(section,"min_wound_size");
	m_fV_EntityMorale = pSettings->r_float(section,"morale_v");
	
	m_fHealthHitPart = pSettings->r_float(section,"health_hit_part");
	m_fPowerHitPart = pSettings->r_float(section,"power_hit_part");
	

	m_fK_SleepHealth = pSettings->r_float(section,"sleep_health");
	m_fK_SleepPower = pSettings->r_float(section,"sleep_power");
	m_fK_SleepSatiety = pSettings->r_float(section,"sleep_satiety");	
	m_fK_SleepRadiation = pSettings->r_float(section,"sleep_radiation");

	m_use_limping_state = pSettings->line_exist(section,"use_limping_state") ? !!pSettings->r_bool(section,"use_limping_state") : false;
}

void CEntityCondition::reinit	()
{
	m_iLastTimeCalled = 0;
	m_bTimeValid = false;

	m_fHealthMax = MAX_HEALTH;
	m_fPowerMax = MAX_POWER;
	m_fSatietyMax = MAX_SATIETY;
	m_fRadiationMax = MAX_RADIATION;

	m_fCircumspection = m_fCircumspectionMax = 1.f;
	m_fEntityMorale =  m_fEntityMoraleMax = 1.f;

	m_fHealth  = MAX_HEALTH;
	m_fPower  = MAX_POWER;
	m_fSatiety = MAX_SATIETY;
	m_fRadiation  = 0;

	m_fDeltaHealth = 0;
	m_fDeltaPower = 0;
	m_fDeltaSatiety = 0;
	m_fDeltaRadiation = 0;
	m_fDeltaCircumspection = 0;
	m_fDeltaEntityMorale = 0;


	m_fHealthLost = 0.f;
	m_pWho = NULL;



	for(WOUND_VECTOR_IT it = m_WoundVector.begin(); m_WoundVector.end() != it; ++it)
		xr_delete(*it);
	m_WoundVector.clear();

	Awoke();
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
	//затянуть раны
	for(WOUND_VECTOR_IT it = m_WoundVector.begin(); m_WoundVector.end() != it; ++it)
	{
		(*it)->Incarnation(percent, m_fMinWoundSize);
		if(0 == (*it)->TotalSize())
			(*it)->SetDestroy(true);
	}
}
bool RemoveWoundPred(CWound* pWound)
{
	if(pWound->GetDestroy())
	{
		xr_delete(pWound);
		return true;
	}
	return false;
}

void  CEntityCondition::UpdateWounds		()
{
	//убрать все зашившие раны из списка
	WOUND_VECTOR_IT last_it = remove_if(m_WoundVector.begin(), m_WoundVector.end(),RemoveWoundPred);
	m_WoundVector.erase(last_it, m_WoundVector.end());
}

void CEntityCondition::UpdateConditionTime()
{
	u64 cur_time = Level().GetGameTime();

	if(m_bTimeValid)
	{
		if (cur_time > m_iLastTimeCalled)
			m_iDeltaTime = cur_time - m_iLastTimeCalled;
		else 
			m_iDeltaTime = 0;
	}
	else
	{
		m_iDeltaTime = 0;
		m_bTimeValid = true;

		m_fDeltaHealth = 0;
		m_fDeltaPower = 0;
		m_fDeltaSatiety = 0;
		m_fDeltaRadiation = 0;
		m_fDeltaCircumspection = 0;
		m_fDeltaEntityMorale = 0;

	}

	m_iLastTimeCalled = cur_time;
}

//вычисление параметров с ходом игрового времени
void CEntityCondition::UpdateCondition()
{
	if(GetHealth()<=0) return;

	UpdateHealth();
	UpdatePower();
	UpdateSatiety();
	UpdateRadiation();

	UpdateCircumspection();
	UpdateEntityMorale();

	m_fHealth		+= m_fDeltaHealth;
	m_fPower		+= m_fDeltaPower;
	m_fSatiety		+= m_fDeltaSatiety;
	m_fRadiation	+= m_fDeltaRadiation;

	m_fCircumspection	+= m_fDeltaCircumspection;
	m_fEntityMorale		+= m_fDeltaEntityMorale;

	m_fDeltaHealth = 0;
	m_fDeltaPower = 0;
	m_fDeltaSatiety = 0;
	m_fDeltaRadiation = 0;

	m_fDeltaCircumspection = 0;
	m_fDeltaEntityMorale = 0;

	clamp(m_fHealth, MIN_HEALTH, m_fHealthMax);
	
	clamp(m_fPower,0.0f,m_fPowerMax);
	clamp(m_fRadiation,0.0f,m_fRadiationMax);
	clamp(m_fSatiety,0.0f,m_fSatietyMax);
	clamp(m_fCircumspection,0.0f,m_fCircumspectionMax);
	clamp(m_fEntityMorale,0.0f,m_fEntityMoraleMax);
}


void CEntityCondition::Sleep(float hours)
{
	if(GetHealth()<=0) return;

	//u64 cur_time = Level().GetGameTime();

	//час здорового сна
	float time_to_sleep = hours*60*60*1000;
//	m_iDeltaTime = hours*60*60*1000;
	//десять секунд
	m_iDeltaTime = 1000*10;

	GoSleep();

	for(float time=0; time<time_to_sleep; time += m_iDeltaTime)
	{
		UpdateHealth();
		UpdatePower();
		UpdateSatiety();
		UpdateRadiation();

		m_fHealth += m_fDeltaHealth;
		m_fPower += m_fDeltaPower;
		m_fSatiety += m_fDeltaSatiety;
		m_fRadiation +=  m_fDeltaRadiation;

		m_fDeltaHealth = 0;
		m_fDeltaPower = 0;
		m_fDeltaSatiety = 0;
		m_fDeltaRadiation = 0;

		//
		clamp(m_fHealth, MIN_HEALTH, m_fHealthMax);
		
		clamp(m_fPower,0.0f,m_fPowerMax);
		clamp(m_fRadiation,0.0f,m_fRadiationMax);
		clamp(m_fSatiety,0.0f,m_fSatietyMax);
	}

	Awoke();
}

float CEntityCondition::HitOutfitEffect(float hit_power, ALife::EHitType hit_type)
{
    CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(this);
	if(!pInvOwner) return hit_power;

	CCustomOutfit* pOutfit = (CCustomOutfit*)pInvOwner->inventory().m_slots[OUTFIT_SLOT].m_pIItem;
	if(!pOutfit) return hit_power;
	float new_hit_power = hit_power*pOutfit->GetHitTypeProtection(hit_type);
	//увеличить изношенность костюма
	pOutfit->Hit(hit_power, hit_type);

	return new_hit_power;
}


CWound* CEntityCondition::AddWound(float hit_power, ALife::EHitType hit_type, u16 element)
{
	//максимальное число косточек 64
	VERIFY(element<64);

	//запомнить кость по которой ударили и силу удара
	WOUND_VECTOR_IT it = m_WoundVector.begin();
	for(;it != m_WoundVector.end(); it++)
	{
		if((*it)->GetBoneNum() == element)
			break;
	}
	
	CWound* pWound = NULL;

	//новая рана
	if (it == m_WoundVector.end())
	{
		pWound = xr_new<CWound>(element);
		pWound->AddHit(hit_power*::Random.randF(0.5f,1.5f), hit_type);
		m_WoundVector.push_back(pWound);
	}
	//старая 
	else
	{
		pWound = *it;
		pWound->AddHit(hit_power*::Random.randF(0.5f,1.5f), hit_type);
	}

	VERIFY(pWound);
	return pWound;
}

CWound* CEntityCondition::ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, u16 element)
{
	//кто нанес последний хит
	m_pWho = who;

	//нормализуем силу удара
	hit_power = hit_power/100.f;
	hit_power = HitOutfitEffect(hit_power, hit_type);


	switch(hit_type)
	{
	case ALife::eHitTypeTelepatic:
		hit_power *= m_HitTypeK[hit_type];
		m_fDeltaPower -= hit_power;
		return NULL;
		break;
	case ALife::eHitTypeBurn:
	case ALife::eHitTypeChemicalBurn:
	case ALife::eHitTypeShock:
		hit_power *= m_HitTypeK[hit_type];
		break;
	case ALife::eHitTypeRadiation:
		/*hit_power *= m_HitTypeK[hit_type];*/
		m_fDeltaRadiation += hit_power;
		return NULL;
		break;
	case ALife::eHitTypeExplosion:
	case ALife::eHitTypeStrike:
		hit_power *= m_HitTypeK[hit_type];
		m_fHealthLost = hit_power*m_fHealthHitPart;
		m_fDeltaHealth -= m_fHealthLost;
		m_fDeltaPower -= hit_power*m_fPowerHitPart;
		break;
	case ALife::eHitTypeFireWound:
	case ALife::eHitTypeWound:
		hit_power *= m_HitTypeK[hit_type];
		m_fHealthLost = hit_power*m_fHealthHitPart*m_fHitBoneScale;
		m_fDeltaHealth -= m_fHealthLost;
		m_fDeltaPower -= hit_power*m_fPowerHitPart;
		break;
	}

	//раны добавляются только живому
	if(GetHealth()>0)
		return AddWound(hit_power*m_fWoundBoneScale, hit_type, element);
	else
		return NULL;
}


float CEntityCondition::BleedingSpeed()
{
	float bleeding_speed =0;

	for(WOUND_VECTOR_IT it = m_WoundVector.begin(); m_WoundVector.end() != it; ++it)
	{
		bleeding_speed += (*it)->TotalSize();
	}
	return bleeding_speed;
}


void CEntityCondition::UpdateHealth()
{
	float delta_time = float(m_iDeltaTime)/1000.f;
	m_fDeltaHealth -= BleedingSpeed() * delta_time * m_fV_Bleeding;
	VERIFY(_valid(m_fDeltaHealth));
	ChangeBleeding(m_fV_WoundIncarnation * delta_time);
}
void CEntityCondition::UpdatePower()
{
}
void CEntityCondition::UpdateSatiety()
{
	if (GameID() != GAME_SINGLE) return;

	float sleep_k = 1.f;
	if(m_fSatiety>0)
	{
		m_fDeltaSatiety -= m_fV_Satiety*
					  m_fCurrentSleepSatiety*
					  m_iDeltaTime/1000;
		sleep_k = m_fCurrentSleepHealth;
	}
		
	m_fDeltaHealth += m_fV_SatietyHealth*
					(m_fSatiety>m_fSatietyCritical?1.f:-1.f)*
					sleep_k*m_iDeltaTime/1000;

	//коэффициенты уменьшения восстановления силы от сытоти и радиации
	float radiation_power_k = 1.f;/*_abs(m_fRadiationMax - m_fRadiation)/m_fRadiationMax*/
	float satiety_power_k = 1.f;/*_abs(m_fSatiety/m_fSatietyMax)*/
			
	m_fDeltaPower += m_fV_SatietyPower*
				radiation_power_k*
				satiety_power_k*
				m_fCurrentSleepPower*
				m_iDeltaTime/1000;
}
void CEntityCondition::UpdateRadiation()
{
	if(m_fRadiation>0)
	{
		m_fDeltaRadiation -= m_fV_Radiation*
							m_fCurrentSleepRadiation*
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


void CEntityCondition::GoSleep()
{
	m_bIsSleeping = true;

	m_fCurrentSleepHealth =  m_fK_SleepHealth;
	m_fCurrentSleepPower =  m_fK_SleepPower;
	m_fCurrentSleepSatiety =  m_fK_SleepSatiety;
	m_fCurrentSleepRadiation =  m_fK_SleepRadiation;
}

void CEntityCondition::Awoke()
{
	m_bIsSleeping = false;

	m_fCurrentSleepHealth =  1.0f;
	m_fCurrentSleepPower =  1.0f;
	m_fCurrentSleepSatiety =  1.0f;
	m_fCurrentSleepRadiation =  1.0f;
}

bool CEntityCondition::IsLimping() const
{
	return (false);//m_fPower*m_fHealth <= .5f);
}

void CEntityCondition::save	(NET_Packet &output_packet)
{
	u8 is_alive	= (m_fHealth>0.f)?1:0;
	
	output_packet.w_u8	(is_alive);
	if(is_alive)
	{
		output_packet.w_float_q8		(m_fPower,			0.f,1.f);
		output_packet.w_float_q8		(m_fSatiety,		0.f,1.f);
		output_packet.w_float_q8		(m_fRadiation,		0.f,1.f);
		output_packet.w_float_q8		(m_fEntityMorale,	0.f,1.f);
		output_packet.w_float_q8		(m_fCircumspection,	0.f,1.f);

		output_packet.w_u8				((u8)m_WoundVector.size());
		for(WOUND_VECTOR_IT it = m_WoundVector.begin(); m_WoundVector.end() != it; it++)
		{
			(*it)->save(output_packet);
		}
	}
}
void CEntityCondition::load	(IReader &input_packet)
{
	m_bTimeValid = false;

	u8 is_alive				= input_packet.r_u8	();
	if(is_alive)
	{
		m_fPower			= input_packet.r_float_q8	(0.f,1.f);
		m_fSatiety			= input_packet.r_float_q8	(0.f,1.f);
		m_fRadiation		= input_packet.r_float_q8	(0.f,1.f);

		m_fEntityMorale		= input_packet.r_float_q8	(0.f,1.f);
		m_fCircumspection	= input_packet.r_float_q8	(0.f,1.f);

		ClearWounds();
		m_WoundVector.resize(input_packet.r_u8());
		if(!m_WoundVector.empty())
		{
			for(u32 i=0; i<m_WoundVector.size(); i++)
			{
				CWound* pWound = xr_new<CWound>(BI_NONE);
				pWound->load(input_packet);
				m_WoundVector[i] = pWound;
			}
		}
	}
}