// ActorCondition.h: ����� ��������� ������
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

	virtual void LoadCondition(LPCSTR section);

	virtual CWound* ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element = 0);
	virtual void UpdateCondition();

protected:
	void ConditionJump(float weight);
	void ConditionAccel(float weight);


	float m_fAlcohol;

	//���� ����������� �� ������ � ���
	//(��� ������������ ����)
	float m_fJumpPower;
	float m_fAccelPower;

protected:
	//�������� ��� ������ ��� � ��������
	virtual bool IsLimping() const;

	mutable bool m_bLimping;
	//����� ���� � �������� ������ �������� ����� �������� �������
	float m_fLimpingPowerBegin;
	float m_fLimpingPowerEnd;
	float m_fLimpingHealthBegin;
	float m_fLimpingHealthEnd;
};

#endif //_ACTOR_CONDITION_H_