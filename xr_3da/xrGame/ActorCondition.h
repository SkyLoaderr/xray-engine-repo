// ActorCondition.h: ����� ��������� ������
//

#pragma once

#include "EntityCondition.h"
#include "actor_defs.h"

template <typename _return_type>
class CScriptCallbackEx;



class CActor;
class CUIActorSleepVideoPlayer;

class CActorCondition: public CEntityCondition {
private:
	typedef CEntityCondition inherited;

private:
	CActor*						m_object;
	CScriptCallbackEx<LPCSTR>*	m_can_sleep_callback;
	CScriptCallbackEx<LPCSTR>*	m_get_sleep_video_name_callback;
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
	bool				AllowSleep					();
	ACTOR_DEFS::EActorSleep		CanSleepHere		();
	ACTOR_DEFS::EActorSleep		GoSleep				(ALife::_TIME_ID sleep_time, bool without_check = false);
			void				Awoke				();

	// �������� ��� ������ ��� � ��������
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

	CUIActorSleepVideoPlayer*	m_actor_sleep_wnd;

protected:
	float m_fAlcohol;

	float m_fPowerLeakSpeed;
	//���� ����������� �� ������ � ���
	//(��� ������������ ����)
	float m_fJumpPower;
	float m_fStandPower;
	float m_fWalkPower;
	float m_fJumpWeightPower;
	float m_fWalkWeightPower;
	float m_fOverweightWalkK;
	float m_fOverweightJumpK;
	float m_fAccelK;
	float m_fSprintK;


	//��������� ���
	bool m_bIsSleeping;
	//������������ ��������� ��������� ���������� �� ����� ���
	float m_fK_SleepHealth;
	float m_fK_SleepPower;
	float m_fK_SleepSatiety;
	float m_fK_SleepRadiation;
	float m_fK_SleepPsyHealth;

	mutable bool m_bLimping;
	mutable bool m_bCantWalk;
	mutable bool m_bCantSprint;

	//����� ���� � �������� ������ �������� ����� �������� �������
	float m_fLimpingPowerBegin;
	float m_fLimpingPowerEnd;
	float m_fCantWalkPowerBegin;
	float m_fCantWalkPowerEnd;

	float m_fCantSprintPowerBegin;
	float m_fCantSprintPowerEnd;

	float m_fLimpingHealthBegin;
	float m_fLimpingHealthEnd;
};
