// ActorCondition.h: класс состояния игрока
//

#pragma once

#include "EntityCondition.h"
#include "actor_defs.h"

template <typename _return_type>
class CScriptCallbackEx;



class CActor;
//class CUIActorSleepVideoPlayer;

class CActorCondition: public CEntityCondition {
private:
	typedef CEntityCondition inherited;
	enum {	eCriticalPowerReached			=(1<<0),
//			eCriticalMaxPowerReached		=(1<<1),
			eCriticalBleedingSpeed			=(1<<2),
//			eCriticalSatietyReached			=(1<<3),
			eCriticalRadiationReached		=(1<<4),
			eWeaponJammedReached			=(1<<5),
			};
	Flags16											m_condition_flags;
private:
	CActor*											m_object;
/*
	CScriptCallbackEx<LPCSTR>*						m_can_sleep_callback;
	CScriptCallbackEx<LPCSTR>*						m_get_sleep_video_name_callback;
*/
	void				UpdateTutorialThresholds	();
public:
						CActorCondition				(CActor *object);
	virtual				~CActorCondition			(void);

	virtual void		LoadCondition				(LPCSTR section);
	virtual void		reinit						();

	virtual CWound*		ConditionHit				(SHit* pHDS);
	virtual void		UpdateCondition				();

	virtual void 		ChangeAlcohol			(float value);

/*
	bool				IsSleeping					() {return m_bIsSleeping;}
	bool						AllowSleep			();
	ACTOR_DEFS::EActorSleep		CanSleepHere		();
	ACTOR_DEFS::EActorSleep		GoSleep				(ALife::_TIME_ID sleep_time, bool without_check = false);
			void				Awoke				();
*/
	// хромание при потере сил и здоровья
	virtual	bool		IsLimping					() const;
	virtual bool		IsCantWalk					() const;
	virtual bool		IsCantSprint				() const;

			void		ConditionJump				(float weight);
			void		ConditionWalk				(float weight, bool accel, bool sprint);
			void		ConditionStand				(float weight);
			
			float	xr_stdcall	GetAlcohol			()	{return m_fAlcohol;}
			float	xr_stdcall	GetPsy				()	{return 1.0f-GetPsyHealth();}
public:
	IC		CActor		&object						() const
	{
		VERIFY			(m_object);
		return			(*m_object);
	}
	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);

//	CUIActorSleepVideoPlayer*	m_actor_sleep_wnd;

protected:
	float m_fAlcohol;

	float m_fV_Alcohol;

//	float m_fPowerLeakSpeed;

	float m_fJumpPower;
	float m_fStandPower;
	float m_fWalkPower;
	float m_fJumpWeightPower;
	float m_fWalkWeightPower;
	float m_fOverweightWalkK;
	float m_fOverweightJumpK;
	float m_fAccelK;
	float m_fSprintK;


//	bool m_bIsSleeping;
//	SConditionChangeV m_change_v_sleep;
//	float m_fK_SleepMaxPower;

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
