#include "stdafx.h"
#include ".\actorcondition.h"


CActorCondition::CActorCondition(void)
{
	m_fAccelPower = 0.0006f;
	m_fJumpPower = 0.05f;
}

CActorCondition::~CActorCondition(void)
{
}

void CActorCondition::Load(LPCSTR section)
{
	inherited::Load(section);

	m_fAccelPower = pSettings->r_float(section,"accel_power_v");
	m_fJumpPower = pSettings->r_float(section,"jump_power_v");

	//порог силы и здоровья меньше которого актер начинает хромать
	m_fLimpingHealth	= pSettings->r_float(section,	"limping_health");
	m_fLimpingPower		= pSettings->r_float(section,	"limping_power");
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
	if(m_fLimpingPower > m_fPower || m_fLimpingHealth > m_fHealth)
		return true;
	else
		return false;
}