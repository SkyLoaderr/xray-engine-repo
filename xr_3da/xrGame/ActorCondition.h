// ActorCondition.h: класс состояния игрока
//
//////////////////////////////////////////////////////////////////////
 
#ifndef _ACTOR_CONDITION_H_
#define _ACTOR_CONDITION_H_
#pragma once

#include "EntityCondition.h"

class CActorCondition: virtual public CEntityCondition
{
private:
	typedef CEntityCondition inherited;
public:
	CActorCondition(void);
	virtual ~CActorCondition(void);

	virtual void Load(LPCSTR section);

	virtual void ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element = 0);
	virtual void UpdateCondition();
	
	
	float m_fMedkit;
	float m_fAntirad;

protected:
	void ConditionJump(float weight);
	void ConditionAccel(float weight);

	float m_fAlcohol;

	//силы расходуемые на прыжки и бег
	//(при максимальном весе)
	float m_fJumpPower;
	float m_fAccelPower;


};

#endif //_ACTOR_CONDITION_H_