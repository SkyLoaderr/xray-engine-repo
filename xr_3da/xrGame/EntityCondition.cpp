// EntityCondition.cpp: класс состояния живой сущности
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\entitycondition.h"


#define MAX_HEALTH 1.0f
#define MAX_SATIETY 1.0f
#define MAX_POWER 1.0f
#define MAX_RADIATION 1.0f


CEntityCondition::CEntityCondition(void)
{
	m_iLastTimeCalled = 0;
	m_bTimeValid = false;

	m_fHealthMax = MAX_HEALTH;				
	m_fPowerMax = MAX_POWER;
	m_fSatietyMax = MAX_SATIETY;
	m_fRadiationMax = MAX_RADIATION;

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

	m_fK_Burn = 1.0f;
	m_fK_Strike = 1.0f;
	m_fK_Wound = 1.0f;
	m_fK_Radiation = 1.0f;
	m_fK_Telepatic = 1.0f;
	m_fK_Shock = 1.0f;

	m_fHealthHitPart = 1.0f;
	m_fPowerHitPart = 0.5f;

	m_fDeltaHealth = 0;
	m_fDeltaPower = 0;
	m_fDeltaSatiety = 0;
	m_fDeltaRadiation = 0;


	m_fHealthLost = 0.f;
	m_pWho = NULL;

	m_mWound.clear();

	Awoke();
/*
	m_fK_SleepHealth = 1.0f;
	m_fK_SleepPower = 1.0f;
	m_fK_SleepSatiety = 1.0f;	
	m_fK_SleepRadiation = 1.0f;*/
}

CEntityCondition::~CEntityCondition(void)
{
}

void CEntityCondition::Load(LPCSTR section)
{
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

	m_fHealthHitPart = pSettings->r_float(section,"health_hit_part");
	m_fPowerHitPart = pSettings->r_float(section,"power_hit_part");

	m_fK_Burn = pSettings->r_float(section,"burn_immunity");
	m_fK_Strike = pSettings->r_float(section,"strike_immunity");
	m_fK_Shock = pSettings->r_float(section,"shock_immunity");
	m_fK_Wound = pSettings->r_float(section,"wound_immunity");
	m_fK_Radiation = pSettings->r_float(section,"radiation_immunity");
	m_fK_Telepatic = pSettings->r_float(section,"telepatic_immunity");

	m_fK_SleepHealth = pSettings->r_float(section,"sleep_health");
	m_fK_SleepPower = pSettings->r_float(section,"sleep_power");
	m_fK_SleepSatiety = pSettings->r_float(section,"sleep_satiety");	
	m_fK_SleepRadiation = pSettings->r_float(section,"sleep_radiation");


}

void CEntityCondition::ChangeHealth(float value)
{
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
//вычисление параметров с ходом времени
void CEntityCondition::UpdateCondition()
{
	u64 cur_time = Level().GetGameTime();

	if(m_bTimeValid)
		m_iDeltaTime = cur_time - m_iLastTimeCalled;
	else
	{
		m_iDeltaTime = 0;
		m_bTimeValid = true;

		m_fDeltaHealth = 0;
		m_fDeltaPower = 0;
		m_fDeltaSatiety = 0;
		m_fDeltaRadiation = 0;
	}

	m_iLastTimeCalled = cur_time;

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

	if(m_fHealth<0) m_fHealth = 0;
	if(m_fHealth>m_fHealthMax) m_fHealth = m_fHealthMax;
	if(m_fPower<0) m_fPower = 0;
	if(m_fPower>m_fPowerMax) m_fPower = m_fPowerMax;
	if(m_fRadiation<0) m_fRadiation = 0;
	if(m_fRadiation>m_fRadiationMax) m_fRadiation = m_fRadiationMax;
	if(m_fSatiety<0) m_fSatiety = 0;
	if(m_fSatiety>m_fSatietyMax) m_fSatiety = m_fSatietyMax;
}


void CEntityCondition::Sleep(float hours)
{
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



	if(m_fHealth<0) m_fHealth = 0;
	if(m_fHealth>m_fHealthMax) m_fHealth = m_fHealthMax;
	if(m_fPower<0) m_fPower = 0;
	if(m_fPower>m_fPowerMax) m_fPower = m_fPowerMax;
	if(m_fRadiation<0) m_fRadiation = 0;
	if(m_fRadiation>m_fRadiationMax) m_fRadiation = m_fRadiationMax;
	if(m_fSatiety<0) m_fSatiety = 0;
	if(m_fSatiety>m_fSatietyMax) m_fSatiety = m_fSatietyMax;
	}

	Awoke();
}


void CEntityCondition::ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element)
{
	//кто нанес хит
	m_pWho = who;

	//нормализуем силу удара
	hit_power = hit_power/100.f;

	//кол-во костей в существе
	//PKinematics(Visual())->LL_BoneCount();

	switch(hit_type)
	{
	case eHitTypeBurn:
		hit_power *= m_fK_Burn;
		break;
	case eHitTypeStrike:
		hit_power *= m_fK_Strike;
		break;
	case eHitTypeTelepatic:
		hit_power *= m_fK_Telepatic;
		break;
	case eHitTypeShock:
		hit_power *= m_fK_Shock;
		break;
	case eHitTypeWound:
		{
			hit_power *= m_fK_Wound;

			m_fHealthLost = hit_power*m_fHealthHitPart;

			m_fDeltaHealth -= m_fHealthLost;
			m_fDeltaPower -= hit_power*m_fPowerHitPart;

			//запомнить кость по которой ударили и силу удара
			WOUND_PAIR_IT it = m_mWound.find(element);
			//новая рана
			if (it == m_mWound.end())
			{
				m_mWound[element] = hit_power*m_fV_Bleeding;
			}
			//старая 
			else
			{
				m_mWound[element] += hit_power*m_fV_Bleeding;
			}
		}
		break;
	case eHitTypeRadiation:
		hit_power *= m_fK_Radiation;
		m_fDeltaRadiation += hit_power;
		break;
	}
	
}


float CEntityCondition::BleedingSpeed()
{
	float bleeding_speed =0;

	for(WOUND_PAIR_IT it = m_mWound.begin(); it!=m_mWound.end(); it++)
	{
		bleeding_speed += (*it).second;
	}
	return bleeding_speed;
}


void CEntityCondition::UpdateHealth()
{
	m_fDeltaHealth -= BleedingSpeed() * m_iDeltaTime/1000;

	//затянуть раны
	for(WOUND_PAIR_IT it = m_mWound.begin(); it!=m_mWound.end(); it++)
	{
		(*it).second -= m_fV_WoundIncarnation * m_iDeltaTime/1000;
		//рана полность зажила
		if((*it).second<0) (*it).second = 0;
	}
}
void CEntityCondition::UpdatePower()
{
}
void CEntityCondition::UpdateSatiety()
{
	if(m_fSatiety>0)
	{
		m_fDeltaSatiety -= m_fV_Satiety*
					  m_fCurrentSleepSatiety*
					  m_iDeltaTime/1000;
		
		m_fDeltaHealth += m_fV_SatietyHealth*
			         ((m_fSatiety - m_fSatietyCritical*m_fSatietyMax)/m_fSatietyMax)*
					 m_fCurrentSleepHealth*
			         m_iDeltaTime/1000;
		

		//коэффициенты уменьшения восстановления силы от сытоти и радиации
		float radiation_power_k = _abs(m_fRadiationMax - m_fRadiation)/m_fRadiationMax;
		float satiety_power_k = _abs(m_fSatiety/m_fSatietyMax);
			
		m_fDeltaPower += m_fV_SatietyPower*
					radiation_power_k*
					satiety_power_k*
					m_fCurrentSleepPower*
					m_iDeltaTime/1000;

	}
	else
	{
//		m_fHealth -= m_fV_Satiety*m_iDeltaTime/1000;
		m_fDeltaHealth += m_fV_SatietyHealth*
					     (m_fSatiety - m_fSatietyCritical*m_fSatietyMax)*
						 m_iDeltaTime/1000;
	}
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


void CEntityCondition::GoSleep()
{
	m_fCurrentSleepHealth =  m_fK_SleepHealth;
	m_fCurrentSleepPower =  m_fK_SleepPower;
	m_fCurrentSleepSatiety =  m_fK_SleepSatiety;
	m_fCurrentSleepRadiation =  m_fK_SleepRadiation;
}

void CEntityCondition::Awoke()
{
	m_fCurrentSleepHealth =  1.0f;
	m_fCurrentSleepPower =  1.0f;
	m_fCurrentSleepSatiety =  1.0f;
	m_fCurrentSleepRadiation =  1.0f;
}
