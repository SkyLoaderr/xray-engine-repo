//////////////////////////////////////////////////////////////////////
// Actor_Sleep.cpp:	 сон у актера
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "actor.h"
#include "SleepEffector.h"

#include "game_sv_single.h"
#include "ai_alife.h"


#define ONLINE_RADIUS 2.f

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

	if (Level().game.type == GAME_SINGLE) 
	{
		game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
		if (tpGame && tpGame->m_tpALife && tpGame->m_tpALife->m_bLoaded) 
			tpGame->m_tpALife->vfSetSwitchDistance(m_fOldOnlineRadius);
	}


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

		if (Level().game.type == GAME_SINGLE) 
		{
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife && tpGame->m_tpALife->m_bLoaded) 
			{
				m_fOldOnlineRadius = tpGame->m_tpALife->ffGetSwitchDistance();
				tpGame->m_tpALife->vfSetSwitchDistance(ONLINE_RADIUS);
			}
		}

		m_pSleepEffectorPP->m_eSleepState = CSleepEffectorPP::SLEEPING;
	}


	//разбудить актера, если он проспал столько сколько задумал
	if(Level().GetGameTime()>m_dwWakeUpTime)
		Awoke();
}