#include "stdafx.h"
#include ".\actorcondition.h"


CActorCondition::CActorCondition(void)
{
	m_fJumpPower = 0.f;
	m_fWalkPower = 0.f;
	m_fJumpWeightPower = 0.f;
	m_fWalkWeightPower = 0.f;
	m_fOverweightWalkK = 0.f;
	m_fOverweightJumpK = 0.f;
	m_fAccelK = 0.f;
	m_bLimping  = false;
}

CActorCondition::~CActorCondition(void)
{
}

void CActorCondition::LoadCondition(LPCSTR entity_section)
{
	inherited::LoadCondition(entity_section);

	LPCSTR section = entity_section;
	if(pSettings->line_exist(entity_section, "condition_sect"))
	{
		section = pSettings->r_string(section, "condition_sect");
	}

	m_fJumpPower		= pSettings->r_float(section,"jump_power");
	m_fWalkPower		= pSettings->r_float(section,"walk_power");
	m_fJumpWeightPower	= pSettings->r_float(section,"jump_weight_power");
	m_fWalkWeightPower	= pSettings->r_float(section,"walk_weight_power");
	m_fOverweightWalkK	= pSettings->r_float(section,"overweight_walk_k");
	m_fOverweightJumpK	= pSettings->r_float(section,"overweight_jump_k");
	m_fAccelK			= pSettings->r_float(section,"accel_k");

	//порог силы и здоровья меньше которого актер начинает хромать
	m_fLimpingHealthBegin	= pSettings->r_float(section,	"limping_health_begin");
	m_fLimpingHealthEnd		= pSettings->r_float(section,	"limping_health_end");
	R_ASSERT(m_fLimpingHealthBegin<m_fLimpingHealthEnd);

	m_fLimpingPowerBegin	= pSettings->r_float(section,	"limping_power_begin");
	m_fLimpingPowerEnd		= pSettings->r_float(section,	"limping_power_end");
	R_ASSERT(m_fLimpingPowerBegin<m_fLimpingPowerEnd);

	m_fCantWalkPowerBegin	= pSettings->r_float(section,	"cant_walk_power_begin");
	m_fCantWalkPowerEnd		= pSettings->r_float(section,	"cant_walk_power_end");
	R_ASSERT(m_fCantWalkPowerBegin<m_fCantWalkPowerEnd);
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
void CActorCondition::ConditionJump(float weight)
{
	float power = m_fJumpPower;
	power += m_fJumpWeightPower*weight*(weight>1.f?m_fOverweightJumpK:1.f);
	m_fPower -= power;
}
void CActorCondition::ConditionWalk(float weight, bool accel)
{	
	float delta_time = float(m_iDeltaTime)/1000.f;
	float power = m_fWalkPower;
	power += m_fWalkWeightPower*weight*(weight>1.f?m_fOverweightWalkK:1.f);
	power *= delta_time*(accel?m_fAccelK:1.f);
	m_fPower -= power;
}


bool CActorCondition::IsCantWalk() const
{
	if(m_fPower< m_fCantWalkPowerBegin)
		m_bCantWalk = true;
	else if(m_fPower > m_fCantWalkPowerEnd)
		m_bCantWalk = false;
	return m_bCantWalk;
}

bool CActorCondition::IsLimping() const
{
	if(m_fPower< m_fLimpingPowerBegin || m_fHealth < m_fLimpingHealthBegin)
		m_bLimping = true;
	else if(m_fPower > m_fLimpingPowerEnd && m_fHealth > m_fLimpingHealthEnd)
		m_bLimping = false;
	return m_bLimping;
}