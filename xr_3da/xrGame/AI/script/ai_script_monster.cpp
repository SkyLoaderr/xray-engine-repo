////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_monster.cpp
//	Created 	: 06.10.2003
//  Modified 	: 06.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Script behaviour interface for all the monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_monster.h"
#include "../../CustomMonster.h"
#include "../../../feel_vision.h"
#include "../../ai_script_actions.h"
#include "../../weapon.h"
#include "../../ParticlesObject.h"
#include "../../ai_script_classes.h"

void __stdcall ActionCallback(CKinematics *tpKinematics);

CScriptMonster::CScriptMonster()
{
	Init					();
}

CScriptMonster::~CScriptMonster()
{
	FreeAll								();
	ResetScriptData						();
}

void CScriptMonster::Init()
{
	// animation
	for (u32 i=(u32)eActionTypeMovement; i<(u32)eActionTypeCount; ++i) {
		m_tpCallbacks[i].m_lua_function = 0;
		m_tpCallbacks[i].m_lua_object	= 0;
		m_tpCallbacks[i].m_method_name	= "";
	}

	m_tSoundCallback.m_lua_function		= 0;
	m_tSoundCallback.m_lua_object		= 0;
	m_tSoundCallback.m_method_name		= "";

	m_tHitCallback.m_lua_function		= 0;
	m_tHitCallback.m_lua_object			= 0;
	m_tHitCallback.m_method_name		= "";
	m_current_sound						= 0;

	ResetScriptData						();
}

void CScriptMonster::ResetScriptData(void *pointer)
{
	if (!m_tpActionQueue.empty())
		vfFinishAction					(m_tpActionQueue.front());

	while (!m_tpActionQueue.empty()) {
		xr_delete						(m_tpActionQueue.front());
		m_tpActionQueue.erase			(m_tpActionQueue.begin());
	}

	m_caScriptName						= "";
	m_bScriptControl					= false;
	m_tpScriptAnimation					= 0;
	m_tpCurrentEntityAction				= 0;
	m_tpNextAnimation					= 0;
}

void CScriptMonster::FreeAll()
{
	for (u32 i=(u32)eActionTypeMovement; i<(u32)eActionTypeCount; ++i) {
		xr_delete						(m_tpCallbacks[i].m_lua_function);
		xr_delete						(m_tpCallbacks[i].m_lua_object);
		m_tpCallbacks[i].m_method_name	= "";
	}

	xr_delete							(m_tSoundCallback.m_lua_function);
	xr_delete							(m_tSoundCallback.m_lua_object);
	m_tSoundCallback.m_method_name		= "";

	xr_delete							(m_tHitCallback.m_lua_function);
	xr_delete							(m_tHitCallback.m_lua_object);
	m_tHitCallback.m_method_name		= "";
}

void CScriptMonster::reinit()
{
	inherited::reinit					();
	FreeAll								();
}

void CScriptMonster::SetScriptControl(const bool bScriptControl, ref_str caSciptName)
{
	VERIFY				(
		(
			(m_bScriptControl && !bScriptControl) || 
			(!m_bScriptControl && bScriptControl)
		) &&
			(
				bScriptControl || 
				(
					xr_strlen(*m_caScriptName) && 
					!xr_strcmp(caSciptName,m_caScriptName)
				)
			)
		);
	if (bScriptControl && !m_bScriptControl)
		add_visual_callback			(&ActionCallback);
	else
		if (!bScriptControl && m_bScriptControl)
			remove_visual_callback	(&ActionCallback);

	m_bScriptControl	= bScriptControl;
	m_caScriptName		= caSciptName;
#ifdef DEBUG
	if (bScriptControl)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeInfo,"Script %s set object %s under its control",*caSciptName,*cName());
	else
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeInfo,"Script %s freed object %s from its control",*caSciptName,*cName());
#endif
	if (!bScriptControl)
		ResetScriptData(this);
}

bool CScriptMonster::GetScriptControl() const
{
	return				(m_bScriptControl);
}

LPCSTR CScriptMonster::GetScriptControlName() const
{
	return				(*m_caScriptName);
}

bool CScriptMonster::CheckObjectVisibility(const CObject *tpObject)
{
	CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(this);
	if (!l_tpCustomMonster)
		return			(false);
	l_tpCustomMonster->feel_vision_get(l_tpCustomMonster->m_visible_objects);
	xr_vector<CObject*>::const_iterator	I = l_tpCustomMonster->m_visible_objects.begin();
	xr_vector<CObject*>::const_iterator	E = l_tpCustomMonster->m_visible_objects.end();
	for ( ; I != E; ++I)
		if (tpObject == dynamic_cast<CObject*>(*I))
			return		(true);
	return				(false);
}

//определяет видимость определенного типа объектов, 
//заданного через section_name
bool CScriptMonster::CheckTypeVisibility(const char* section_name)
{
	CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(this);
	if (!l_tpCustomMonster)
		return			(false);
	l_tpCustomMonster->feel_vision_get(l_tpCustomMonster->m_visible_objects);
	xr_vector<CObject*>::const_iterator	I = l_tpCustomMonster->m_visible_objects.begin();
	xr_vector<CObject*>::const_iterator	E = l_tpCustomMonster->m_visible_objects.end();
	for ( ; I != E; ++I)
	{
		CObject* pObject = dynamic_cast<CObject*>(*I);
		if (!xr_strcmp(section_name, pObject->cNameSect()))
			return		(true);
	}
	return				(false);
}


void CScriptMonster::UseObject(const CObject * /**tpObject/**/)
{
#pragma todo("Dima to Dima : Use object specified by script")
}

void CScriptMonster::AddAction(const CEntityAction *tpEntityAction, bool bHighPriority)
{
#ifdef _DEBUG
//	if (!xr_strcmp("m_stalker_wounded",*cName())) {
//		Msg				("%6d Adding action : %s",Level().timeServer(),*tpEntityAction->m_tAnimationAction.m_caAnimationToPlay);
//	}
#endif
	bool				empty = m_tpActionQueue.empty();
	if (!bHighPriority || m_tpActionQueue.empty())
		m_tpActionQueue.push_back(xr_new<CEntityAction>(*tpEntityAction));
	else {
		VERIFY			(m_tpActionQueue.front());
		CEntityAction	*l_tpEntityAction = xr_new<CEntityAction>(*m_tpActionQueue.front());
		vfFinishAction	(m_tpActionQueue.front());
		xr_delete		(m_tpActionQueue.front());
		m_tpActionQueue.front() = l_tpEntityAction;
		m_tpActionQueue.insert(m_tpActionQueue.begin(),xr_new<CEntityAction>(*tpEntityAction));
	}

	if (empty)
		ProcessScripts	();

#ifdef _DEBUG
//	if (!xr_strcmp("m_stalker_wounded",*cName()))
//		Msg					("\n%6d Action queue",Level().timeServer());
//	xr_deque<CEntityAction*>::const_iterator	I = m_tpActionQueue.begin();
//	xr_deque<CEntityAction*>::const_iterator	E = m_tpActionQueue.end();
//	for ( ; I != E; ++I)
//		if (!xr_strcmp("m_stalker_wounded",*cName()))
//			Msg				("%6d Action : %s",Level().timeServer(),*(*I)->m_tAnimationAction.m_caAnimationToPlay);
//	if (!xr_strcmp("m_stalker_wounded",*cName()))
//		Msg					("\n");
#endif
}

CEntityAction *CScriptMonster::GetCurrentAction()
{
	if (m_tpActionQueue.empty())
		return(0);
	else
		return(m_tpActionQueue.front());
}

void __stdcall ActionCallback(CKinematics *tpKinematics)
{
	// sounds
	CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(static_cast<CObject*>(tpKinematics->Update_Callback_Param));
	VERIFY			(l_tpScriptMonster);
	if (!l_tpScriptMonster->GetCurrentAction())
		return;
	l_tpScriptMonster->vfUpdateSounds();
	l_tpScriptMonster->vfUpdateParticles();
}

void CScriptMonster::vfUpdateParticles()
{
	CScriptParticleAction	&l_tParticleAction = GetCurrentAction()->m_tParticleAction;
	if (xr_strlen(l_tParticleAction.m_caBoneName)) {
		CParticlesObject	*l_tpParticlesObject = l_tParticleAction.m_tpParticleSystem;
		l_tpParticlesObject->UpdateParent(GetUpdatedMatrix(l_tParticleAction.m_caBoneName,l_tParticleAction.m_tParticlePosition,l_tParticleAction.m_tParticleAngles),l_tParticleAction.m_tParticleVelocity);
	}
}

void CScriptMonster::vfUpdateSounds()
{
	CScriptSoundAction	&l_tSoundAction = GetCurrentAction()->m_tSoundAction;
	if (xr_strlen(l_tSoundAction.m_caBoneName) && m_current_sound && m_current_sound->feedback)
		m_current_sound->feedback->set_position(GetUpdatedMatrix(l_tSoundAction.m_caBoneName,l_tSoundAction.m_tSoundPosition,Fvector().set(0,0,0)).c);
}

void CScriptMonster::vfFinishAction(CEntityAction *tpEntityAction)
{
	if (m_current_sound) {
		m_current_sound->destroy	();
		xr_delete					(m_current_sound);
	}
	if (!tpEntityAction->m_tParticleAction.m_bAutoRemove)
		xr_delete(tpEntityAction->m_tParticleAction.m_tpParticleSystem);
}

void CScriptMonster::ProcessScripts()
{
	CEntityAction	*l_tpEntityAction = 0;
#ifdef DEBUG
	bool			empty_queue = m_tpActionQueue.empty();
#endif
	while (!m_tpActionQueue.empty()) {
		l_tpEntityAction= m_tpActionQueue.front();
		VERIFY		(l_tpEntityAction);
#ifdef _DEBUG
//		if (!xr_strcmp("m_stalker_wounded",*cName()))
//			Msg			("%6d Processing action : %s",Level().timeServer(),*l_tpEntityAction->m_tAnimationAction.m_caAnimationToPlay);
#endif
		
		if (m_tpCurrentEntityAction != l_tpEntityAction)
			l_tpEntityAction->initialize	();

		m_tpCurrentEntityAction	= l_tpEntityAction;

		if (!l_tpEntityAction->CheckIfActionCompleted())
			break;

#ifdef _DEBUG
//		if (!xr_strcmp("m_stalker_wounded",*cName()))
//			Msg			("%6d Action completed : %s",Level().timeServer(),*l_tpEntityAction->m_tAnimationAction.m_caAnimationToPlay);
#endif

		vfFinishAction(l_tpEntityAction);
		
		Msg("Entity Action removed!!!");

		xr_delete	(l_tpEntityAction);
		m_tpActionQueue.erase(m_tpActionQueue.begin());
	}

	if (m_tpActionQueue.empty()) {
#ifdef DEBUG
		if (empty_queue)
			ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeInfo,"Object %s has an empty script queue!",*cName());
#endif
		return;
	}

	bool			l_bCompleted;
	
	l_bCompleted	= l_tpEntityAction->m_tWatchAction.m_bCompleted;
	bfAssignWatch	(l_tpEntityAction);
	if (l_tpEntityAction->m_tWatchAction.m_bCompleted && !l_bCompleted && m_tpCallbacks[eActionTypeWatch].m_lua_object)
		callback(eActionTypeWatch);

	bfAssignAnimation(l_tpEntityAction);

	l_bCompleted	= l_tpEntityAction->m_tSoundAction.m_bCompleted;
	bfAssignSound	(l_tpEntityAction);
	if (l_tpEntityAction->m_tSoundAction.m_bCompleted && !l_bCompleted && m_tpCallbacks[eActionTypeSound].m_lua_object)
		callback(eActionTypeSound);
	
	l_bCompleted	= l_tpEntityAction->m_tParticleAction.m_bCompleted;
	bfAssignParticles(l_tpEntityAction);
	if (l_tpEntityAction->m_tParticleAction.m_bCompleted && !l_bCompleted && m_tpCallbacks[eActionTypeParticle].m_lua_object)
		callback(eActionTypeParticle);
	
	l_bCompleted	= l_tpEntityAction->m_tObjectAction.m_bCompleted;
	bfAssignObject	(l_tpEntityAction);
	if (l_tpEntityAction->m_tObjectAction.m_bCompleted && !l_bCompleted && m_tpCallbacks[eActionTypeObject].m_lua_object)
		callback(eActionTypeObject);
	
	l_bCompleted	= l_tpEntityAction->m_tMovementAction.m_bCompleted;
	bfAssignMovement(l_tpEntityAction);
	if (l_tpEntityAction->m_tMovementAction.m_bCompleted && !l_bCompleted)
		if (m_tpCallbacks[eActionTypeMovement].m_lua_object)
			luabind::call_member<void>(*(m_tpCallbacks[eActionTypeMovement].m_lua_object),*(m_tpCallbacks[eActionTypeMovement].m_method_name),lua_game_object(),u32(eActionTypeMovement),-1);
		else
			if (m_tpCallbacks[eActionTypeMovement].m_lua_function)
				(*m_tpCallbacks[eActionTypeMovement].m_lua_function)(lua_game_object(),u32(eActionTypeMovement),-1);

	if (!l_tpEntityAction->m_tAnimationAction.m_bCompleted)
		bfScriptAnimation	();

	bfAssignMonsterAction(l_tpEntityAction);
}

bool CScriptMonster::bfAssignWatch(CEntityAction *tpEntityAction)
{
	return			(true);
}

bool CScriptMonster::bfAssignMonsterAction(CEntityAction *tpEntityAction)
{
	if (GetCurrentAction() && GetCurrentAction()->m_tMonsterAction.m_bCompleted)
		return		(false);

	return			(true);
}

bool CScriptMonster::bfAssignAnimation(CEntityAction *tpEntityAction)
{
	m_tpNextAnimation = 0;
	if (GetCurrentAction() && GetCurrentAction()->m_tAnimationAction.m_bCompleted)
		return		(false);
	if (!xr_strlen(GetCurrentAction()->m_tAnimationAction.m_caAnimationToPlay))
		return		(true);
	CSkeletonAnimated	&tVisualObject = *(PSkeletonAnimated(Visual()));
	m_tpNextAnimation	= tVisualObject.ID_Cycle_Safe(*GetCurrentAction()->m_tAnimationAction.m_caAnimationToPlay);
	return			(true);
}

const Fmatrix CScriptMonster::GetUpdatedMatrix(ref_str caBoneName, const Fvector &tPositionOffset, const Fvector &tAngleOffset)
{
	Fmatrix			l_tMatrix;

	l_tMatrix.setHPB(VPUSH(tAngleOffset));
	l_tMatrix.c		= tPositionOffset;

	if (xr_strlen(caBoneName)) {
		CBoneInstance	&l_tBoneInstance = PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID(caBoneName));
		l_tMatrix.mulA	(l_tBoneInstance.mTransform);
		l_tMatrix.mulA	(XFORM());
	}

	return			(l_tMatrix);
}

bool CScriptMonster::bfAssignSound(CEntityAction *tpEntityAction)
{
	CScriptSoundAction	&l_tSoundAction = tpEntityAction->m_tSoundAction;
	if (l_tSoundAction.m_bCompleted)
		return		(false);
	if (m_current_sound) {
		if (!m_current_sound->feedback)
			if (!l_tSoundAction.m_bStartedToPlay) {
#ifdef _DEBUG
//				Msg									("%6d Starting sound %s",Level().timeServer(),*l_tSoundAction.m_caSoundToPlay);
#endif
				const Fmatrix	&l_tMatrix = GetUpdatedMatrix(l_tSoundAction.m_caBoneName,l_tSoundAction.m_tSoundPosition,l_tSoundAction.m_tSoundAngles);
				m_current_sound->play_at_pos(this,l_tMatrix.c,l_tSoundAction.m_bLooped ? sm_Looped : 0);
				l_tSoundAction.m_bStartedToPlay = true;
			}
			else {
				l_tSoundAction.m_bCompleted		= true;
			}
	}
	else {
		if (xr_strlen(l_tSoundAction.m_caSoundToPlay)) {
			m_current_sound						= xr_new<ref_sound>();
			m_current_sound->create				(TRUE,*l_tSoundAction.m_caSoundToPlay,l_tSoundAction.m_sound_type);
		}
		else
			l_tSoundAction.m_bCompleted = true;
	}
	return		(!l_tSoundAction.m_bCompleted);
}

bool CScriptMonster::bfAssignParticles(CEntityAction *tpEntityAction)
{
	CScriptParticleAction	&l_tParticleAction = tpEntityAction->m_tParticleAction;
	if (l_tParticleAction.m_bCompleted)
		return		(false);
	if (l_tParticleAction.m_tpParticleSystem) {
		if (true/** !l_tParticleAction.m_tpParticleSystem/**/)
			if (!l_tParticleAction.m_bStartedToPlay) {
				const Fmatrix	&l_tMatrix = GetUpdatedMatrix(*l_tParticleAction.m_caBoneName,l_tParticleAction.m_tParticlePosition,l_tParticleAction.m_tParticleAngles);
				Fvector zero_vel={0.f,0.f,0.f};
				l_tParticleAction.m_tpParticleSystem->UpdateParent(l_tMatrix,zero_vel);
				l_tParticleAction.m_tpParticleSystem->play_at_pos(l_tMatrix.c);
				l_tParticleAction.m_bStartedToPlay = true;
			}
			else {
				l_tParticleAction.m_bCompleted = true;
			}
	}
	else
		l_tParticleAction.m_bCompleted = true;

	return			(!l_tParticleAction.m_bCompleted);
}

bool CScriptMonster::bfAssignObject(CEntityAction *tpEntityAction)
{
	return			(GetCurrentAction() && !GetCurrentAction()->m_tObjectAction.m_bCompleted);
}

bool CScriptMonster::bfAssignMovement(CEntityAction *tpEntityAction)
{
	CScriptMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	
	if (l_tMovementAction.m_bCompleted)
		return		(false);

	CEntityAlive			*entity_alive = dynamic_cast<CEntityAlive*>(this);
	if (entity_alive && !entity_alive->g_Alive()) {
		l_tMovementAction.m_bCompleted = true;
		return				(false);
	}
	
	CMovementManager		*l_tpMovementManager = dynamic_cast<CMovementManager*>(this);

	switch (l_tMovementAction.m_tGoalType) {
		case CScriptMovementAction::eGoalTypeObject : {
			CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tMovementAction.m_tpObjectToGo);
			R_ASSERT		(l_tpGameObject);
			l_tpMovementManager->set_path_type(CMovementManager::ePathTypeLevelPath);
//			Msg			("%6d Object %s, position [%f][%f][%f]",Level().timeServer(),*l_tpGameObject->cName(),VPUSH(l_tpGameObject->Position()));
			l_tpMovementManager->set_dest_position(l_tpGameObject->Position());
			l_tpMovementManager->set_level_dest_vertex(l_tpGameObject->level_vertex_id());
			break;
		}
		case CScriptMovementAction::eGoalTypePatrolPath : {
			l_tpMovementManager->set_path_type	(CMovementManager::ePathTypePatrolPath);
			l_tpMovementManager->set_path		(l_tMovementAction.m_path,l_tMovementAction.m_path_name);
			l_tpMovementManager->set_start_type	(l_tMovementAction.m_tPatrolPathStart);
			l_tpMovementManager->set_route_type	(l_tMovementAction.m_tPatrolPathStop);
			l_tpMovementManager->set_random		(l_tMovementAction.m_bRandom);
			break;
		}
		case CScriptMovementAction::eGoalTypePathPosition : {
			l_tpMovementManager->set_path_type(CMovementManager::ePathTypeLevelPath);
			l_tpMovementManager->set_dest_position(l_tMovementAction.m_tDestinationPosition);
			
//			u64					start = CPU::GetCycleCount();
			u32					vertex_id;
			vertex_id			= ai().level_graph().vertex(level_vertex_id(),l_tMovementAction.m_tDestinationPosition);
			if (!ai().level_graph().valid_vertex_id(vertex_id)) {
				vertex_id		= ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),l_tMovementAction.m_tDestinationPosition);
			}
//			u64					stop = CPU::GetCycleCount();
#ifdef _DEBUG
//			Msg					("%6d Searching for node for script object %s (%.5f seconds)",Level().timeServer(),*cName(),float(s64(stop - start))*CPU::cycles2seconds);
#endif
			VERIFY				(ai().level_graph().valid_vertex_id(vertex_id));
			l_tpMovementManager->CLevelPathManager::set_dest_vertex(vertex_id);
			l_tpMovementManager->CLevelLocationSelector::set_evaluator(0);
			break;
		}
		case CScriptMovementAction::eGoalTypeNoPathPosition : {
			if (l_tpMovementManager) {
				l_tpMovementManager->set_path_type(CMovementManager::ePathTypeLevelPath);
				if (l_tpMovementManager->CDetailPathManager::path().empty() || (l_tpMovementManager->CDetailPathManager::path()[l_tpMovementManager->CDetailPathManager::path().size() - 1].position.distance_to(l_tMovementAction.m_tDestinationPosition) > .1f)) {
					l_tpMovementManager->CDetailPathManager::m_path.resize(2);
					l_tpMovementManager->CDetailPathManager::m_path[0].position = Position();
					l_tpMovementManager->CDetailPathManager::m_path[1].position = l_tMovementAction.m_tDestinationPosition;
					l_tpMovementManager->CDetailPathManager::m_current_travel_point	= 0;
				}
				if (l_tpMovementManager->CDetailPathManager::m_path[1].position.similar(Position(),.2f))
					l_tMovementAction.m_bCompleted = true;
			}
			break;
		}
		default : {
			if (l_tpMovementManager) {
				//l_tpMovementManager->set_path_type	(CMovementManager::ePathTypeNoPath);
				l_tpMovementManager->set_desirable_speed(0.f);
			}
			return									(l_tMovementAction.m_bCompleted = true);
		}
	}

	if (l_tpMovementManager->actual_all() && l_tpMovementManager->path_completed())
		l_tMovementAction.m_bCompleted = true;

	return		(!l_tMovementAction.m_bCompleted);
}

void CScriptMonster::net_Destroy()
{
	inherited::net_Destroy	();
}

void CScriptMonster::set_callback	(const luabind::object &lua_object, LPCSTR method, const CScriptMonster::EActionType tActionType)
{
	VERIFY					(tActionType < eActionTypeCount);
	xr_delete				(m_tpCallbacks[tActionType].m_lua_object);
	m_tpCallbacks[tActionType].m_lua_object = xr_new<luabind::object>(lua_object);
	m_tpCallbacks[tActionType].m_method_name = method;
	if (eActionTypeMovement == tActionType) {
		CPatrolPathManager	*l_tpPatrolPathManager = dynamic_cast<CPatrolPathManager*>(this);
		if (l_tpPatrolPathManager)
			l_tpPatrolPathManager->set_callback(m_tpCallbacks[tActionType]);
	}
}

void CScriptMonster::set_callback	(const luabind::functor<void> &lua_function, const CScriptMonster::EActionType tActionType)
{
	VERIFY					(tActionType < eActionTypeCount);
	xr_delete				(m_tpCallbacks[tActionType].m_lua_object);
	m_tpCallbacks[tActionType].m_lua_function = xr_new<luabind::functor<void> >(lua_function);
	if (eActionTypeMovement == tActionType) {
		CPatrolPathManager	*l_tpPatrolPathManager = dynamic_cast<CPatrolPathManager*>(this);
		if (l_tpPatrolPathManager)
			l_tpPatrolPathManager->set_callback(m_tpCallbacks[tActionType]);
	}
}

void CScriptMonster::clear_callback	(const CScriptMonster::EActionType tActionType)
{
	VERIFY					(tActionType < eActionTypeCount);
	xr_delete				(m_tpCallbacks[tActionType].m_lua_object);
	xr_delete				(m_tpCallbacks[tActionType].m_lua_function);
	m_tpCallbacks[tActionType].m_method_name = "";
	if (tActionType) {
		CPatrolPathManager	*l_tpPatrolPathManager = dynamic_cast<CPatrolPathManager*>(this);
		if (l_tpPatrolPathManager)
			l_tpPatrolPathManager->set_callback(m_tpCallbacks[tActionType]);
	}
}

void CScriptMonster::set_sound_callback(const luabind::object &lua_object, LPCSTR method)
{
	xr_delete						(m_tSoundCallback.m_lua_object);
	m_tSoundCallback.m_lua_object	= xr_new<luabind::object>(lua_object);
	m_tSoundCallback.m_method_name	= method;
}

void CScriptMonster::set_sound_callback	(const luabind::functor<void> &lua_function)
{
	xr_delete						(m_tSoundCallback.m_lua_function);
	m_tSoundCallback.m_lua_function	= xr_new<luabind::functor<void> >(lua_function);
}

void CScriptMonster::clear_sound_callback	(bool member_callback)
{
	if (member_callback)
		xr_delete					(m_tSoundCallback.m_lua_object);
	else
		xr_delete					(m_tSoundCallback.m_lua_function);
}

void CScriptMonster::set_hit_callback(const luabind::object &lua_object, LPCSTR method)
{
	xr_delete						(m_tHitCallback.m_lua_object);
	m_tHitCallback.m_lua_object		= xr_new<luabind::object>(lua_object);
	m_tHitCallback.m_method_name	= method;
}

void CScriptMonster::set_hit_callback	(const luabind::functor<void> &lua_function)
{
	xr_delete						(m_tHitCallback.m_lua_function);
	m_tHitCallback.m_lua_function	= xr_new<luabind::functor<void> >(lua_function);
}

void CScriptMonster::clear_hit_callback	(bool member_callback)
{
	if (member_callback)
		xr_delete					(m_tHitCallback.m_lua_object);
	else
		xr_delete					(m_tHitCallback.m_lua_function);
}

void CScriptMonster::callback		(const CScriptMonster::EActionType tActionType)
{
	if (m_tpCallbacks[tActionType].m_lua_object)
		luabind::call_member<void>(*(m_tpCallbacks[tActionType].m_lua_object),*(m_tpCallbacks[tActionType].m_method_name),lua_game_object(),u32(tActionType));
	else
		if (m_tpCallbacks[tActionType].m_lua_function)
			(*m_tpCallbacks[tActionType].m_lua_function)(lua_game_object(),u32(tActionType));

}

LPCSTR CScriptMonster::GetPatrolPathName()
{
	VERIFY					(GetScriptControl());
	if (m_tpActionQueue.empty())
		return				("");
	return					(*m_tpActionQueue.back()->m_tMovementAction.m_path_name);
}

BOOL CScriptMonster::net_Spawn		(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return						(FALSE);

	setVisible						(TRUE);
	setEnabled						(TRUE);

	ResetScriptData					();

	return							(TRUE);
}

BOOL CScriptMonster::UsedAI_Locations()
{
	return							(FALSE);
}

void CScriptMonster::shedule_Update	(u32 DT)
{
	ProcessScripts					();
}

void ScriptCallBack(CBlend* B)
{
	CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*> (static_cast<CObject*>(B->CallbackParam));
	R_ASSERT		(l_tpScriptMonster);
	if (l_tpScriptMonster->GetCurrentAction() && !B->bone_or_part) {
		if (!l_tpScriptMonster->GetCurrentAction()->m_tAnimationAction.m_bCompleted)
			l_tpScriptMonster->callback(CScriptMonster::eActionTypeAnimation);
#ifdef _DEBUG
//		if (!xr_strcmp("m_stalker_wounded",*l_tpScriptMonster->cName()))
//			Msg			("Completed %s",*l_tpScriptMonster->GetCurrentAction()->m_tAnimationAction.m_caAnimationToPlay);
#endif
		l_tpScriptMonster->m_tpScriptAnimation = 0;
		l_tpScriptMonster->GetCurrentAction()->m_tAnimationAction.m_bCompleted = true;
		if (l_tpScriptMonster->GetActionCount())
			l_tpScriptMonster->ProcessScripts();
	}
}

bool CScriptMonster::bfScriptAnimation()
{
	if (GetScriptControl() && !GetCurrentAction() && GetActionCount())
		ProcessScripts();

	if (
		GetScriptControl() && 
		GetCurrentAction() && 
		!GetCurrentAction()->m_tAnimationAction.m_bCompleted && 
		xr_strlen(GetCurrentAction()->m_tAnimationAction.m_caAnimationToPlay)) {

#ifdef _DEBUG
//			if (!xr_strcmp("m_stalker_wounded",*cName()))
//				Msg				("%6d Playing animation : %s",Level().timeServer(),*GetCurrentAction()->m_tAnimationAction.m_caAnimationToPlay);
#endif
			if (m_tpScriptAnimation != m_tpNextAnimation)
				PSkeletonAnimated(Visual())->PlayCycle(m_tpScriptAnimation = m_tpNextAnimation,TRUE,ScriptCallBack,this);
			return		(true);
		}
	else {
		m_tpScriptAnimation	= 0;
		return		(false);
	}
}

void CScriptMonster::UpdateCL		()
{
	bfScriptAnimation				();
}

u32	 CScriptMonster::GetActionCount	() const
{
	return							(m_tpActionQueue.size());
}

const CEntityAction *CScriptMonster::GetActionByIndex	(u32 action_index) const
{
	return							(m_tpActionQueue[action_index]);
}

void CScriptMonster::sound_callback	(const CObject *object, int sound_type, const Fvector &position, float sound_power)
{
	if (!dynamic_cast<const CGameObject*>(object))
		return;
	if (m_tSoundCallback.m_lua_object)
		luabind::call_member<void>(
			*m_tSoundCallback.m_lua_object,
			*m_tSoundCallback.m_method_name,
			lua_game_object(),
			const_cast<CGameObject*>(dynamic_cast<const CGameObject*>(object))->lua_game_object(),
			sound_type,
			position,
			sound_power
		);

	if (m_tSoundCallback.m_lua_function)
		(*m_tSoundCallback.m_lua_function)(
			lua_game_object(),
			const_cast<CGameObject*>(dynamic_cast<const CGameObject*>(object))->lua_game_object(),
			sound_type,
			position,
			sound_power
		);
}

void CScriptMonster::hit_callback	(float amount, const Fvector &vLocalDir, const CObject *who, s16 element)
{
	if (!dynamic_cast<const CGameObject*>(who))
		return;
	if (m_tHitCallback.m_lua_object)
		luabind::call_member<void>(
			*m_tHitCallback.m_lua_object,
			*m_tHitCallback.m_method_name,
			lua_game_object(),
			amount,
			vLocalDir,
			const_cast<CGameObject*>(dynamic_cast<const CGameObject*>(who))->lua_game_object(),
			element
		);

	if (m_tHitCallback.m_lua_function)
		(*m_tHitCallback.m_lua_function)(
			lua_game_object(),
			amount,
			vLocalDir,
			const_cast<CGameObject*>(dynamic_cast<const CGameObject*>(who))->lua_game_object(),
			element
		);
}

CEntity	*CScriptMonster::GetCurrentEnemy()
{
	return (0);
}
CEntity	*CScriptMonster::GetCurrentCorpse()
{
	return (0);
}

int CScriptMonster::get_enemy_strength()
{
	return (0);
}