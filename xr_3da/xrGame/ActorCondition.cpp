#include "stdafx.h"
#include ".\actorcondition.h"


CActorCondition::CActorCondition(void)
{
	m_fAccelPower = 0.0006f;
	m_fJumpPower = 0.05f;

	m_bLimping  = false;
}

CActorCondition::~CActorCondition(void)
{
}

void CActorCondition::LoadCondition(LPCSTR section)
{
	inherited::LoadCondition(section);

	m_fAccelPower = pSettings->r_float(section,"accel_power_v");
	m_fJumpPower = pSettings->r_float(section,"jump_power_v");

	//порог силы и здоровья меньше которого актер начинает хромать
	m_fLimpingHealthBegin	= pSettings->r_float(section,	"limping_health_begin");
	m_fLimpingHealthEnd		= pSettings->r_float(section,	"limping_health_end");
	R_ASSERT(m_fLimpingHealthBegin<m_fLimpingHealthEnd);

	m_fLimpingPowerBegin	= pSettings->r_float(section,	"limping_power_begin");
	m_fLimpingPowerEnd		= pSettings->r_float(section,	"limping_power_end");
	R_ASSERT(m_fLimpingPowerBegin<m_fLimpingPowerEnd);
}




//вычисление параметров с ходом времени
void CActorCondition::UpdateCondition()
{
	inherited::UpdateCondition();
}

CWound* CActorCondition::ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element)
{
	return inherited::ConditionHit(who, hit_power, hit_type, element);
}

//weight - "удельный" вес от 0..1
//сам актер + ноша
void CActorCondition::ConditionJump(float weight)
{
	m_fPower -= m_fJumpPower*weight;
}
void CActorCondition::ConditionAccel(float weight)
{
	m_fPower -= m_fAccelPower*weight;
}


bool CActorCondition::IsLimping() const
{
	if(m_fPower< m_fLimpingPowerBegin || m_fHealth < m_fLimpingHealthBegin)
	{
		m_bLimping = true;
	}
	else if(m_fPower > m_fLimpingPowerEnd && m_fHealth > m_fLimpingHealthEnd)
	{
		m_bLimping = false;
	}

	return m_bLimping;
}