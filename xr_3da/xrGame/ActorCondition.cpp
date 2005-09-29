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

#define ENEMIES_RADIUS				20.f

CActorCondition::CActorCondition(CActor *object) :
	inherited	(object)
{
	m_fJumpPower				= 0.f;
	m_fStandPower				= 0.f;
	m_fWalkPower				= 0.f;
	m_fJumpWeightPower			= 0.f;
	m_fWalkWeightPower			= 0.f;
	m_fOverweightWalkK			= 0.f;
	m_fOverweightJumpK			= 0.f;
	m_fAccelK					= 0.f;
	m_fSprintK					= 0.f;

	VERIFY						(object);
	m_object					= object;
}

CActorCondition::~CActorCondition(void)
{
}

void CActorCondition::LoadCondition(LPCSTR entity_section)
{
	inherited::LoadCondition(entity_section);

	LPCSTR						section = READ_IF_EXISTS(pSettings,r_string,entity_section,"condition_sect",entity_section);
	m_fJumpPower				= pSettings->r_float(section,"jump_power");
	m_fStandPower				= pSettings->r_float(section,"stand_power");
	m_fWalkPower				= pSettings->r_float(section,"walk_power");
	m_fJumpWeightPower			= pSettings->r_float(section,"jump_weight_power");
	m_fWalkWeightPower			= pSettings->r_float(section,"walk_weight_power");
	m_fOverweightWalkK			= pSettings->r_float(section,"overweight_walk_k");
	m_fOverweightJumpK			= pSettings->r_float(section,"overweight_jump_k");
	m_fAccelK					= pSettings->r_float(section,"accel_k");
	m_fSprintK					= pSettings->r_float(section,"sprint_k");

	//порог силы и здоровья меньше которого актер начинает хромать
	m_fLimpingHealthBegin		= pSettings->r_float(section,	"limping_health_begin");
	m_fLimpingHealthEnd			= pSettings->r_float(section,	"limping_health_end");
	R_ASSERT					(m_fLimpingHealthBegin<m_fLimpingHealthEnd);

	m_fLimpingPowerBegin		= pSettings->r_float(section,	"limping_power_begin");
	m_fLimpingPowerEnd			= pSettings->r_float(section,	"limping_power_end");
	R_ASSERT					(m_fLimpingPowerBegin<m_fLimpingPowerEnd);

	m_fCantWalkPowerBegin		= pSettings->r_float(section,	"cant_walk_power_begin");
	m_fCantWalkPowerEnd			= pSettings->r_float(section,	"cant_walk_power_end");
	R_ASSERT					(m_fCantWalkPowerBegin<m_fCantWalkPowerEnd);

	m_fCantSprintPowerBegin		= pSettings->r_float(section,	"cant_sprint_power_begin");
	m_fCantSprintPowerEnd		= pSettings->r_float(section,	"cant_sprint_power_end");
	R_ASSERT					(m_fCantSprintPowerBegin<m_fCantSprintPowerEnd);

	m_fPowerLeakSpeed			= READ_IF_EXISTS(pSettings,r_float,section,"max_power_leak_speed",0.0f);


	m_fK_SleepHealth		= pSettings->r_float(section,"sleep_health");
	m_fK_SleepPower			= pSettings->r_float(section,"sleep_power");
	m_fK_SleepSatiety		= pSettings->r_float(section,"sleep_satiety");	
	m_fK_SleepRadiation		= pSettings->r_float(section,"sleep_radiation");
	m_fK_SleepPsyHealth		= pSettings->r_float(section,"sleep_psy_health");

}


//вычисление параметров с ходом времени
void CActorCondition::UpdateCondition()
{
	if (GodMode())				return;
	if (!object().g_Alive())	return;
	

	if ((object().mstate_real&mcAnyMove)) {
		ConditionWalk(object().inventory().TotalWeight()/object().inventory().GetMaxWeight(), isActorAccelerated(object().mstate_real,object().IsZoomAimingMode()), (object().mstate_real&mcSprint) != 0);
	}
	else {
		ConditionStand(object().inventory().TotalWeight()/object().inventory().GetMaxWeight());
	};
	
	if( !IsSleeping() && GameID()==GAME_SINGLE ){
		// update power_leak
		float delta_time = float(m_iDeltaTime)/1000.f;
		SetMaxPower		(GetMaxPower()-m_fPowerLeakSpeed*delta_time);
	}

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
	float power			=	m_fJumpPower;
	power				+=	m_fJumpWeightPower*weight*(weight>1.f?m_fOverweightJumpK:1.f);
	m_fPower			-=	HitPowerEffect(power);
}
void CActorCondition::ConditionWalk(float weight, bool accel, bool sprint)
{	
	float delta_time	=	float(m_iDeltaTime)/1000.f;
	float power			=	m_fWalkPower;
	power				+=	m_fWalkWeightPower*weight*(weight>1.f?m_fOverweightWalkK:1.f);
	power				*=	delta_time*(accel?(sprint?m_fSprintK:m_fAccelK):1.f);
	m_fPower			-=	HitPowerEffect(power);
}

void CActorCondition::ConditionStand(float weight)
{	
	float delta_time	= float(m_iDeltaTime)/1000.f;
	float power			= m_fStandPower;
	power				*= delta_time;
	m_fPower			-= power;
}


bool CActorCondition::IsCantWalk() const
{
	if(m_fPower< m_fCantWalkPowerBegin)
		m_bCantWalk		= true;
	else if(m_fPower > m_fCantWalkPowerEnd)
		m_bCantWalk		= false;
	return				m_bCantWalk;
}

bool CActorCondition::IsCantSprint() const
{
	if(m_fPower< m_fCantSprintPowerBegin)
		m_bCantSprint	= true;
	else if(m_fPower > m_fCantSprintPowerEnd)
		m_bCantSprint	= false;
	return				m_bCantSprint;
}

bool CActorCondition::IsLimping() const
{
	if(m_fPower< m_fLimpingPowerBegin || m_fHealth < m_fLimpingHealthBegin)
		m_bLimping = true;
	else if(m_fPower > m_fLimpingPowerEnd && m_fHealth > m_fLimpingHealthEnd)
		m_bLimping = false;
	return m_bLimping;
}
extern bool g_bShowHudInfo;

EActorSleep CActorCondition::GoSleep(ALife::_TIME_ID sleep_time, bool without_check)
{
	if (IsSleeping()) return easCanSleep;

	EActorSleep result = without_check?easCanSleep:CanSleepHere();
	if(easCanSleep != result) 
		return result;

	g_bShowHudInfo				= false;
	m_bIsSleeping				= true;

	ProcessSleep				(sleep_time);// change conditions

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

	m_bIsSleeping				= false;

	Level().Server->game->SetGameTimeFactor(object().m_fOldTimeFactor);

	if ((GameID() == GAME_SINGLE)  &&ai().get_alife()) {
		NET_Packet		P;
		P.w_begin		(M_SWITCH_DISTANCE);
		P.w_float		(object().m_fOldOnlineRadius);
		Level().Send	(P,net_flags(TRUE,TRUE));
	}


	VERIFY(m_object == smart_cast<CActor*>(Level().CurrentEntity()));
	VERIFY(object().m_pSleepEffectorPP);

	if(object().m_pSleepEffectorPP)
	{
		object().m_pSleepEffectorPP->m_eSleepState = CSleepEffectorPP::AWAKING;
		object().m_pSleepEffectorPP = NULL;
	}

	g_bShowHudInfo			= true;
	
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
	BOOL				enabled = object().getEnabled();
	object().setEnabled	(FALSE);
	BOOL				result = 
		Level().ObjectSpace.RayPick(
			pos,
			dir,
			0.3f, 
			collide::rqtBoth,
			RQ
		);
	object().setEnabled(enabled);
	
	//актер стоит на динамическом объекте или вообще падает - 
	//спать нельзя
	if(!result || RQ.O)	
		return easNotSolidGround;

	BOOL				_enabled = object().getEnabled();
	object().setEnabled	(FALSE);
	xr_vector<CObject*> NearestList;	// = Level().ObjectSpace.q_nearest; 
	Level().ObjectSpace.GetNearest	(NearestList, pos, ENEMIES_RADIUS); 
	//xr_vector<CObject*> &NearestList = Level().ObjectSpace.q_nearest; 
	object().setEnabled	(_enabled);

	for(xr_vector<CObject*>::iterator it = NearestList.begin();
									NearestList.end() != it;
									it++)
	{
		CEntityAlive* entity = smart_cast<CEntityAlive*>(*it);
		if(entity && entity->g_Alive() && entity->is_relation_enemy(m_object))
			return easEnemies;
	}

	return easCanSleep;
}

void CActorCondition::save(NET_Packet &output_packet)
{
	inherited::save		(output_packet);
}

void CActorCondition::load(IReader &input_packet)
{
	inherited::load		(input_packet);
}

void CActorCondition::ProcessSleep(ALife::_TIME_ID sleep_time)
{
	if(GetHealth()<=0) return;

	//десять секунд
	m_iDeltaTime = 1000*10;

	for(float time=0; time<sleep_time; time += m_iDeltaTime)
	{
		UpdateHealth			();
		UpdatePower				();
		UpdateSatiety			(m_fK_SleepSatiety);
		UpdateRadiation			(m_fK_SleepRadiation);
		UpdatePsyHealth			(m_fK_SleepPsyHealth);

		m_fHealth				+= m_fDeltaHealth;
		m_fPower				+= m_fDeltaPower;
		m_fSatiety				+= m_fDeltaSatiety;
		m_fRadiation			+=  m_fDeltaRadiation;
		m_fPsyHealth			+= m_fDeltaPsyHealth;

		SetMaxPower				(m_fPowerMax+m_fDeltaPower);

		m_fDeltaHealth			= 0;
		m_fDeltaPower			= 0;
		m_fDeltaSatiety			= 0;
		m_fDeltaRadiation		= 0;
		m_fDeltaPsyHealth		= 0;
		

		clamp					(m_fHealth,		0.0f,		m_fHealthMax);
		clamp					(m_fPower,		0.0f,		m_fPowerMax);
		clamp					(m_fRadiation,	0.0f,		m_fRadiationMax);
		clamp					(m_fSatiety,	0.0f,		m_fSatietyMax);
		clamp					(m_fPsyHealth,	0.0f,		m_fPsyHealthMax);
	}

}

void CActorCondition::reinit	()
{
	inherited::reinit	();
	m_bLimping					= false;
	m_bIsSleeping				= false;
}