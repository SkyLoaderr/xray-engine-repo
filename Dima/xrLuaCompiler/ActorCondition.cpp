#include "stdafx.h"
#include ".\actorcondition.h"


CActorCondition::CActorCondition(void)
{
	m_fAccelPower = 0.0006f;
	m_fJumpPower = 0.05f;


	m_fMedkit = 0.3f;
	m_fMedkitWound = 0.1f;
	m_fAntirad = 0.3f;

}

CActorCondition::~CActorCondition(void)
{
}

void CActorCondition::Load(LPCSTR section)
{
	inherited::Load(section);

	m_fAccelPower = pSettings->r_float(section,"accel_power_v");
	m_fJumpPower = pSettings->r_float(section,"jump_power_v");

	//temporary
	m_fMedkit = pSettings->r_float(section,"medkit");
	m_fMedkitWound = pSettings->r_float(section,"medkit_wound");
	m_fAntirad = pSettings->r_float(section,"antirad");

}




//вычисление параметров с ходом времени
void CActorCondition::UpdateCondition()
{
	inherited::UpdateCondition();
}

void CActorCondition::ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element)
{
	inherited::ConditionHit(who, hit_power, hit_type, element);
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