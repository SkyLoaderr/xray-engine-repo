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
#define ENEMIES_RADIUS				30.f

//проверка можем ли мы спать на этом месте
EActorSleep CActor::CanSleepHere()
{
	if(0 != mstate_real) return easNotSolidGround;

	collide::rq_result RQ;

	Fvector pos, dir;
	pos.set(Position());
	pos.y += 0.1f;
	dir.set(0, -1.f, 0);
	setEnabled(FALSE);
	BOOL result = Level().ObjectSpace.RayPick(pos, dir, 0.3f, 
				  collide::rqtBoth, RQ);
	setEnabled(TRUE);
	
	//актер стоит на динамическом объекте или вообще падает - 
	//спать нельзя
	if(!result || RQ.O)	
		return easNotSolidGround;
/*	
	//проверка на твердость материала на котором мы стоим 
	else
	{
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris() + RQ.element;
		u16 hit_material_idx	= pTri->material;
		SGameMtl* mtl	= GMLib.GetMaterialByIdx(hit_material_idx);
		if(mtl->fPHSpring < MIN_SPRING_TO_SLEEP) 
			return easNotSolidGround;
	}*/

	//проверить нет ли в радиусе врагов
	if (!Level().autosave_manager().ready_for_autosave())
		return easEnemies;

	setEnabled(false);
	Level().ObjectSpace.GetNearest	(pos, ENEMIES_RADIUS); 
	xr_vector<CObject*> &NearestList = Level().ObjectSpace.q_nearest; 
	setEnabled(true);

	for(xr_vector<CObject*>::iterator it = NearestList.begin();
									NearestList.end() != it;
									it++)
	{
		CEntityAlive* entity = smart_cast<CEntityAlive*>(*it);
		if(entity && entity->g_Alive() &&
			((entity->tfGetRelationType(this) == ALife::eRelationTypeEnemy) ||  
			(entity->tfGetRelationType(this) == ALife::eRelationTypeWorstEnemy)) )
			return easEnemies;
	}

	return easCanSleep;
}

EActorSleep CActor::GoSleep(ALife::_TIME_ID sleep_time, bool without_check)
{
	if (conditions().IsSleeping()) return easCanSleep;

	EActorSleep result = without_check?easCanSleep:CanSleepHere();
	if(easCanSleep != result) 
		return result;

	conditions().GoSleep();

	//остановить актера, если он двигался
	mstate_wishful	&=		~mcAnyMove;
	mstate_real		&=		~mcAnyMove;

	//поставить будильник
	m_dwWakeUpTime = Level().GetGameTime() + sleep_time;

	VERIFY(this == smart_cast<CActor*>(Level().CurrentEntity()));

	Level().Cameras.RemoveEffector(EEffectorPPType(SLEEP_EFFECTOR_TYPE_ID));
	m_pSleepEffectorPP = xr_new<CSleepEffectorPP>(m_pSleepEffector->ppi,
													m_pSleepEffector->time,
													m_pSleepEffector->time_attack,
													m_pSleepEffector->time_release);

	Level().Cameras.AddEffector(m_pSleepEffectorPP);

	return easCanSleep;
}

void CActor::Awoke()
{
	if(!conditions().IsSleeping()) return;
	conditions().Awoke();

	Level().Server->game->SetGameTimeFactor(m_fOldTimeFactor);

	if ((GameID() == GAME_SINGLE)  &&ai().get_alife()) {
		NET_Packet		P;
		P.w_begin		(M_SWITCH_DISTANCE);
		P.w_float		(m_fOldOnlineRadius);
		Level().Send	(P,net_flags(TRUE,TRUE));

//		ai().alife().set_switch_distance	(m_fOldOnlineRadius);
	}


	VERIFY(this == smart_cast<CActor*>(Level().CurrentEntity()));
	VERIFY(m_pSleepEffectorPP);

	if(m_pSleepEffectorPP)
	{
		m_pSleepEffectorPP->m_eSleepState = CSleepEffectorPP::AWAKING;
		m_pSleepEffectorPP = NULL;
	}

	
}

//#include "date_time.h"

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
		Awoke();
}