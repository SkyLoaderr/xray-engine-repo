//////////////////////////////////////////////////////////////////////
// Actor_Sleep.cpp:	 сон у актера
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "actor.h"
#include "SleepEffector.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "level.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "xrServer.h"
#include "autosave_manager.h"
#include "ai_space.h"
#include "actorcondition.h"

#define ONLINE_RADIUS				2.f
#define MIN_SPRING_TO_SLEEP			0.8f	

void CActor::UpdateSleep()
{
	if(!conditions().IsSleeping()) return;


	VERIFY(this == smart_cast<CActor*>(Level().CurrentEntity()));
	VERIFY(m_pSleepEffectorPP);

//	u32 y,m,d,h,mi,s,ms;
//	split_time(Level().GetGameTime(),y,m,d,h,mi,s,ms);
//	Msg	("Sleep time : %d.%d.%d %d:%d:%d.%d",y,m,d,h,mi,s,ms);

	if(CSleepEffectorPP::BEFORE_SLEEPING == m_pSleepEffectorPP->m_eSleepState)
	{
		m_fOldTimeFactor = Level().GetGameTimeFactor();
		Level().Server->game->SetGameTimeFactor(m_fSleepTimeFactor);
		
		if ((GameID() == GAME_SINGLE) && ai().get_alife()) {
			m_fOldOnlineRadius = ai().alife().switch_distance();
//			ai().alife().set_switch_distance(ONLINE_RADIUS);
			NET_Packet		P;
			P.w_begin		(M_SWITCH_DISTANCE);
			P.w_float		(ONLINE_RADIUS);
			Level().Send	(P,net_flags(TRUE,TRUE));
		}

		m_pSleepEffectorPP->m_eSleepState = CSleepEffectorPP::SLEEPING;
	}


	//разбудить актера, если он проспал столько сколько задумал
	if(Level().GetGameTime()>m_dwWakeUpTime)
		conditions().Awoke();
}