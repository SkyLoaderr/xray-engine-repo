// ActorCondition.h: класс состояния игрока
//
//////////////////////////////////////////////////////////////////////
 
#ifndef _ACTOR_CONDITION_H_
#define _ACTOR_CONDITION_H_
#pragma once

#include "EntityCondition.h"

class CActor;


class CActorCondition: public CEntityCondition {
private:
	typedef CEntityCondition inherited;

private:
	CActor		*m_object;

public:
	CActorCondition(CActor *object);
	virtual ~CActorCondition(void);

	virtual void LoadCondition(LPCSTR section);

	virtual CWound* ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element = 0);
	virtual void UpdateCondition();

	void ConditionJump(float weight);
	void ConditionWalk(float weight, bool accel, bool sprint);
	void ConditionStand(float weight);

protected:
	float m_fAlcohol;

	//силы расходуемые на прыжки и бег
	//(при максимальном весе)
	float m_fJumpPower;
	float m_fStandPower;
	float m_fWalkPower;
	float m_fJumpWeightPower;
	float m_fWalkWeightPower;
	float m_fOverweightWalkK;
	float m_fOverweightJumpK;
	float m_fAccelK;
	float m_fSprintK;

public:
	//хромание при потере сил и здоровья
	virtual bool IsLimping() const;
	virtual bool IsCantWalk() const;

protected:
	mutable bool m_bLimping;
	mutable bool m_bCantWalk;

	//порог силы и здоровья меньше которого актер начинает хромать
	float m_fLimpingPowerBegin;
	float m_fLimpingPowerEnd;
	float m_fCantWalkPowerBegin;
	float m_fCantWalkPowerEnd;

	float m_fLimpingHealthBegin;
	float m_fLimpingHealthEnd;
};

#endif //_ACTOR_CONDITION_H_