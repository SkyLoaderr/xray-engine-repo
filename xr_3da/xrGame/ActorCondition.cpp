#include "stdafx.h"
#include "actorcondition.h"
#include "actor.h"
#include "inventory.h"
#include "level.h"
#include "sleepeffector.h"
#include "game_base_space.h"
#include "autosave_manager.h"
#include "xrserver.h"
#include "ai_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "game_object_space.h"

#define ENEMIES_RADIUS				30.f

CActorCondition::CActorCondition(CActor *object) :
	inherited	(object)
{
	VERIFY		(object);
	m_object	= object;
	m_fJumpPower = 0.f;
	m_fStandPower = 0.f;
	m_fWalkPower = 0.f;
	m_fJumpWeightPower = 0.f;
	m_fWalkWeightPower = 0.f;
	m_fOverweightWalkK = 0.f;
	m_fOverweightJumpK = 0.f;
	m_fAccelK = 0.f;
	m_fSprintK = 0.f;
	m_bLimping  = false;
}

CActorCondition::~CActorCondition(void)
{
}

void CActorCondition::LoadCondition(LPCSTR entity_section)
{
	inherited::LoadCondition(entity_section);

	LPCSTR				section = READ_IF_EXISTS(pSettings,r_string,entity_section,"condition_sect",entity_section);
	m_fJumpPower		= pSettings->r_float(section,"jump_power");
	m_fStandPower		= pSettings->r_float(section,"stand_power");
	m_fWalkPower		= pSettings->r_float(section,"walk_power");
	m_fJumpWeightPower	= pSettings->r_float(section,"jump_weight_power");
	m_fWalkWeightPower	= pSettings->r_float(section,"walk_weight_power");
	m_fOverweightWalkK	= pSettings->r_float(section,"overweight_walk_k");
	m_fOverweightJumpK	= pSettings->r_float(section,"overweight_jump_k");
	m_fAccelK			= pSettings->r_float(section,"accel_k");
	m_fSprintK			= pSettings->r_float(section,"sprint_k");

	//порог силы и здоровья меньше которого актер начинает хромать
	m_fLimpingHealthBegin	= pSettings->r_float(section,	"limping_health_begin");
	m_fLimpingHealthEnd		= pSettings->r_float(section,	"limping_health_end");
	R_ASSERT(m_fLimpingHealthBegin<m_fLimpingHealthEnd);

	m_fLimpingPowerBegin	= pSettings->r_float(section,	"limping_power_begin");
	m_fLimpingPowerEnd		= pSettings->r_float(section,	"limping_power_end");
	R_ASSERT(m_fLimpingPowerBegin<m_fLimpingPowerEnd);

	m_fCantWalkPowerBegin	= pSettings->r_float(section,	"cant_walk_power_begin");
	m_fCantWalkPowerEnd		= pSettings->r_float(section,	"cant_walk_power_end");
	R_ASSERT(m_fCantWalkPowerBegin<m_fCantWalkPowerEnd);

	m_fCantSprintPowerBegin	= pSettings->r_float(section,	"cant_sprint_power_begin");
	m_fCantSprintPowerEnd		= pSettings->r_float(section,	"cant_sprint_power_end");
	R_ASSERT(m_fCantSprintPowerBegin<m_fCantSprintPowerEnd);

}


//вычисление параметров с ходом времени
void CActorCondition::UpdateCondition()
{
	if (GodMode())/*(psActorFlags.test(AF_GODMODE))*/ return;
	if (!object().g_Alive()) return;
	
//	if (object().Remote()) return;

	if ((object().mstate_real&mcAnyMove)) {
		ConditionWalk(object().inventory().TotalWeight()/object().inventory().GetMaxWeight(), object().isAccelerated(object().mstate_real), (object().mstate_real&mcSprint) != 0);
	}
	else {
		ConditionStand(object().inventory().TotalWeight()/object().inventory().GetMaxWeight());
	};
	
	inherited::UpdateCondition();
}

CWound* CActorCondition::ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element)
{
	if (GodMode())/*(psActorFlags.test(AF_GODMODE))*/ return NULL;
	return inherited::ConditionHit(who, hit_power, hit_type, element);
}

//weight - "удельный" вес от 0..1
void CActorCondition::ConditionJump(float weight)
{
	float power = m_fJumpPower;
	power += m_fJumpWeightPower*weight*(weight>1.f?m_fOverweightJumpK:1.f);
	m_fPower -= HitPowerEffect(power);
}
void CActorCondition::ConditionWalk(float weight, bool accel, bool sprint)
{	
	float delta_time = float(m_iDeltaTime)/1000.f;
	float power = m_fWalkPower;
	power += m_fWalkWeightPower*weight*(weight>1.f?m_fOverweightWalkK:1.f);
	power *= delta_time*(accel?(sprint?m_fSprintK:m_fAccelK):1.f);
	m_fPower -= HitPowerEffect(power);
}

void CActorCondition::ConditionStand(float weight)
{	
	float delta_time = float(m_iDeltaTime)/1000.f;
	float power = m_fStandPower;
//	power += m_fWalkWeightPower*weight*(weight>1.f?m_fOverweightWalkK:1.f);
	power *= delta_time;//*(accel?m_fAccelK:1.f);
	m_fPower -= power;
}


bool CActorCondition::IsCantWalk() const
{
	if(m_fPower< m_fCantWalkPowerBegin)
		m_bCantWalk = true;
	else if(m_fPower > m_fCantWalkPowerEnd)
		m_bCantWalk = false;
	return m_bCantWalk;
}

bool CActorCondition::IsCantSprint() const
{
	if(m_fPower< m_fCantSprintPowerBegin)
		m_bCantSprint = true;
	else if(m_fPower > m_fCantSprintPowerEnd)
		m_bCantSprint = false;
	return m_bCantSprint;
}

bool CActorCondition::IsLimping() const
{
	if(m_fPower< m_fLimpingPowerBegin || m_fHealth < m_fLimpingHealthBegin)
		m_bLimping = true;
	else if(m_fPower > m_fLimpingPowerEnd && m_fHealth > m_fLimpingHealthEnd)
		m_bLimping = false;
	return m_bLimping;
}

EActorSleep CActorCondition::GoSleep(ALife::_TIME_ID sleep_time, bool without_check)
{
	if (IsSleeping()) return easCanSleep;

	EActorSleep result = without_check?easCanSleep:CanSleepHere();
	if(easCanSleep != result) 
		return result;

	inherited::GoSleep		();

	//остановить актера, если он двигался
	object().mstate_wishful	&=		~mcAnyMove;
	object().mstate_real	&=		~mcAnyMove;

	//поставить будильник
	object().m_dwWakeUpTime = Level().GetGameTime() + sleep_time;

	VERIFY	(m_object == smart_cast<CActor*>(Level().CurrentEntity()));

	Level().Cameras.RemoveEffector(EEffectorPPType(SLEEP_EFFECTOR_TYPE_ID));
	object().m_pSleepEffectorPP = xr_new<CSleepEffectorPP>(object().m_pSleepEffector->ppi,
													object().m_pSleepEffector->time,
													object().m_pSleepEffector->time_attack,
													object().m_pSleepEffector->time_release);

	Level().Cameras.AddEffector(object().m_pSleepEffectorPP);

	m_object->callback(GameObject::eActorSleep)( m_object->lua_game_object() );

	return easCanSleep;
}

void CActorCondition::Awoke()
{
	if(!IsSleeping()) return;

	CEntityCondition::Awoke();

	Level().Server->game->SetGameTimeFactor(object().m_fOldTimeFactor);

	if ((GameID() == GAME_SINGLE)  &&ai().get_alife()) {
		NET_Packet		P;
		P.w_begin		(M_SWITCH_DISTANCE);
		P.w_float		(object().m_fOldOnlineRadius);
		Level().Send	(P,net_flags(TRUE,TRUE));

//		ai().alife().set_switch_distance	(m_fOldOnlineRadius);
	}


	VERIFY(m_object == smart_cast<CActor*>(Level().CurrentEntity()));
	VERIFY(object().m_pSleepEffectorPP);

	if(object().m_pSleepEffectorPP)
	{
		object().m_pSleepEffectorPP->m_eSleepState = CSleepEffectorPP::AWAKING;
		object().m_pSleepEffectorPP = NULL;
	}

	
}

//проверка можем ли мы спать на этом месте
EActorSleep CActorCondition::CanSleepHere()
{
	if(0 != object().mstate_real) return easNotSolidGround;

	collide::rq_result RQ;

	Fvector pos, dir;
	pos.set(object().Position());
	pos.y += 0.1f;
	dir.set(0, -1.f, 0);
	object().setEnabled(FALSE);
	BOOL result = Level().ObjectSpace.RayPick(pos, dir, 0.3f, 
				  collide::rqtBoth, RQ);
	object().setEnabled(TRUE);
	
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

	object().setEnabled(false);
	Level().ObjectSpace.GetNearest	(pos, ENEMIES_RADIUS); 
	xr_vector<CObject*> &NearestList = Level().ObjectSpace.q_nearest; 
	object().setEnabled(true);

	for(xr_vector<CObject*>::iterator it = NearestList.begin();
									NearestList.end() != it;
									it++)
	{
		CEntityAlive* entity = smart_cast<CEntityAlive*>(*it);
		if(entity && entity->g_Alive() &&
			((entity->tfGetRelationType(m_object) == ALife::eRelationTypeEnemy) ||  
			(entity->tfGetRelationType(m_object) == ALife::eRelationTypeWorstEnemy)) )
			return easEnemies;
	}

	return easCanSleep;
}
