//////////////////////////////////////////////////////////////////////
// Actor_Sleep.cpp:	 сон у актера
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "actor.h"
#include "SleepEffector.h"


void CActor::GoSleep(u32 sleep_time)
{
	CActorCondition::GoSleep();

	//остановить актера, если он двигался
	mstate_wishful	&=		~mcAnyMove;
	mstate_real		&=		~mcAnyMove;

	//поставить будильник
	m_dwWakeUpTime = Level().GetGameTime() + sleep_time;

	VERIFY(this == dynamic_cast<CActor*>(Level().CurrentEntity()));

	Level().Cameras.RemoveEffector(EEffectorPPType(SLEEP_EFFECTOR_TYPE_ID));
	m_pSleepEffectorPP = xr_new<CSleepEffectorPP>(m_pSleepEffector->ppi,
													m_pSleepEffector->time,
													m_pSleepEffector->time_attack,
													m_pSleepEffector->time_release);

	Level().Cameras.AddEffector(m_pSleepEffectorPP);
}
void CActor::Awoke()
{
	if(!IsSleeping()) return;
	CActorCondition::Awoke();

	Level().SetGameTimeFactor(m_fOldTimeFactor);


	VERIFY(this == dynamic_cast<CActor*>(Level().CurrentEntity()));
	VERIFY(m_pSleepEffectorPP);

	if(m_pSleepEffectorPP)
	{
		m_pSleepEffectorPP->m_eSleepState = CSleepEffectorPP::AWAKING;
		m_pSleepEffectorPP = NULL;
	}

	
}

void CActor::UpdateSleep()
{
	if(!IsSleeping()) return;


	VERIFY(this == dynamic_cast<CActor*>(Level().CurrentEntity()));
	VERIFY(m_pSleepEffectorPP);

	if(CSleepEffectorPP::BEFORE_SLEEPING == m_pSleepEffectorPP->m_eSleepState)
	{
		m_fOldTimeFactor = Level().GetGameTimeFactor();
		Level().SetGameTimeFactor(m_fSleepTimeFactor*m_fOldTimeFactor);
		m_pSleepEffectorPP->m_eSleepState = CSleepEffectorPP::SLEEPING;
	}


	//разбудить актера, если он проспал столько сколько задумал
	if(Level().GetGameTime()>m_dwWakeUpTime)
		Awoke();
}