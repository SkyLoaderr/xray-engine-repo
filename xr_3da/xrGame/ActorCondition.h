// ActorCondition.h: класс состояния игрока
//

#pragma once

#include "EntityCondition.h"

namespace ACTOR_DEFS {
	enum EActorSleep;
};

class CActor;

class CActorCondition: public CEntityCondition {
private:
	typedef CEntityCondition inherited;

public:
	typedef ACTOR_DEFS::EActorSleep EActorSleep;

private:
	CActor				*m_object;

public:
						CActorCondition		(CActor *object);
	virtual				~CActorCondition	(void);

	virtual void		LoadCondition		(LPCSTR section);
	virtual void		reinit				();

	virtual CWound*		ConditionHit		(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element = 0);
	virtual void		UpdateCondition		();


	void				ProcessSleep			(ALife::_TIME_ID sleep_time);
	bool				IsSleeping				() {return m_bIsSleeping;}

	// sleeping
	EActorSleep			CanSleepHere		();
	EActorSleep			GoSleep				(ALife::_TIME_ID sleep_time, bool without_check = false);
			void		Awoke				();

	// хромание при потере сил и здоровья
	virtual	bool		IsLimping			() const;
	virtual bool		IsCantWalk			() const;
	virtual bool		IsCantSprint		() const;

			void		ConditionJump		(float weight);
			void		ConditionWalk		(float weight, bool accel, bool sprint);
			void		ConditionStand		(float weight);

public:
	IC		CActor		&object				() const
	{
		VERIFY			(m_object);
		return			(*m_object);
	}
	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);

protected:
	float m_fAlcohol;

	float m_fPowerLeakSpeed;
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


	//состояние сна
	bool m_bIsSleeping;
	//коэффициенты скоростей изменения параметров во время сна
	float m_fK_SleepHealth;
	float m_fK_SleepPower;
	float m_fK_SleepSatiety;
	float m_fK_SleepRadiation;
	float m_fK_SleepPsyHealth;

protected:
	mutable bool m_bLimping;
	mutable bool m_bCantWalk;
	mutable bool m_bCantSprint;

	//порог силы и здоровья меньше которого актер начинает хромать
	float m_fLimpingPowerBegin;
	float m_fLimpingPowerEnd;
	float m_fCantWalkPowerBegin;
	float m_fCantWalkPowerEnd;

	float m_fCantSprintPowerBegin;
	float m_fCantSprintPowerEnd;

	float m_fLimpingHealthBegin;
	float m_fLimpingHealthEnd;
};
