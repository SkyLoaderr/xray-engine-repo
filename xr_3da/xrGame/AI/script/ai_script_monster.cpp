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

CScriptMonster::CScriptMonster()
{
	m_tpActionQueue.clear	();
	m_caScriptName			= "";
	m_bScriptControl		= false;
	for (u32 i=(u32)eActionTypeMovement; i<(u32)eActionTypeCount; ++i) {
		m_tpCallbacks[i].m_lua_object = 0;
		m_tpCallbacks[i].m_method_name = "";
	}
	InitScript				();
}

CScriptMonster::~CScriptMonster()
{
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
					!strcmp(*caSciptName,*m_caScriptName)
				)
			)
		);
	m_bScriptControl	= bScriptControl;
	m_caScriptName		= caSciptName;
#ifdef DEBUG
	if (bScriptControl)
		LuaOut			(Lua::eLuaMessageTypeInfo,"Script %s set object %s under its control",*caSciptName,cName());
	else
		LuaOut			(Lua::eLuaMessageTypeInfo,"Script %s freed object %s from its control",*caSciptName,cName());
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
	l_tpCustomMonster->feel_vision_get(l_tpCustomMonster->m_tpaVisibleObjects);
	xr_vector<CObject*>::const_iterator	I = l_tpCustomMonster->m_tpaVisibleObjects.begin();
	xr_vector<CObject*>::const_iterator	E = l_tpCustomMonster->m_tpaVisibleObjects.end();
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
	l_tpCustomMonster->feel_vision_get(l_tpCustomMonster->m_tpaVisibleObjects);
	xr_vector<CObject*>::const_iterator	I = l_tpCustomMonster->m_tpaVisibleObjects.begin();
	xr_vector<CObject*>::const_iterator	E = l_tpCustomMonster->m_tpaVisibleObjects.end();
	for ( ; I != E; ++I)
	{
		CObject* pObject = dynamic_cast<CObject*>(*I);
		if (strcmp(section_name, pObject->cNameSect()))
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
	if (!l_tpScriptMonster->GetCurrentAction()) {
		tpKinematics->Callback(0,0);
		return;
	}
	l_tpScriptMonster->vfUpdateSounds();
	l_tpScriptMonster->vfUpdateParticles();
}

void CScriptMonster::vfUpdateParticles()
{
	CParticleAction	&l_tParticleAction = GetCurrentAction()->m_tParticleAction;
	if (xr_strlen(l_tParticleAction.m_caBoneName)) {
		CParticlesObject	*l_tpParticlesObject = l_tParticleAction.m_tpParticleSystem;
		l_tpParticlesObject->UpdateParent(GetUpdatedMatrix(l_tParticleAction.m_caBoneName,l_tParticleAction.m_tParticlePosition,l_tParticleAction.m_tParticleAngles),l_tParticleAction.m_tParticleVelocity);
	}
}

void CScriptMonster::vfUpdateSounds()
{
	CSoundAction	&l_tSoundAction = GetCurrentAction()->m_tSoundAction;
	if (xr_strlen(l_tSoundAction.m_caBoneName) && l_tSoundAction.m_tpSound && l_tSoundAction.m_tpSound->feedback)
		l_tSoundAction.m_tpSound->feedback->set_position(GetUpdatedMatrix(l_tSoundAction.m_caBoneName,l_tSoundAction.m_tSoundPosition,Fvector().set(0,0,0)).c);
}

void CScriptMonster::vfFinishAction(CEntityAction *tpEntityAction)
{
	if (tpEntityAction->m_tSoundAction.m_bLooped)
		tpEntityAction->m_tSoundAction.m_tpSound->destroy();
	if (!tpEntityAction->m_tParticleAction.m_bAutoRemove)
		xr_delete(tpEntityAction->m_tParticleAction.m_tpParticleSystem);
}

void CScriptMonster::ProcessScripts()
{
	CEntityAction	*l_tpEntityAction = 0;
	while (!m_tpActionQueue.empty()) {
		l_tpEntityAction= m_tpActionQueue.front();
		R_ASSERT	(l_tpEntityAction);
		if (!l_tpEntityAction->CheckIfActionCompleted())
			break;

		vfFinishAction(l_tpEntityAction);

		xr_delete	(l_tpEntityAction);
		m_tpActionQueue.erase(m_tpActionQueue.begin());
	}

	if (m_tpActionQueue.empty()) {
#ifdef DEBUG
		LuaOut		(Lua::eLuaMessageTypeInfo,"Object %s has an empty script queue!",cName());
#endif
//		SetScriptControl(false,m_caScriptName);
		return;
	}

	bool			l_bCompleted;
	PKinematics(Visual())->Callback(ActionCallback,this);
	
	l_bCompleted	= l_tpEntityAction->m_tMovementAction.m_bCompleted;
	bfAssignWatch	(l_tpEntityAction);
	if (l_tpEntityAction->m_tMovementAction.m_bCompleted && !l_bCompleted && m_tpCallbacks[eActionTypeWatch].m_lua_object)
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
	if (l_tpEntityAction->m_tMovementAction.m_bCompleted && !l_bCompleted && m_tpCallbacks[eActionTypeMovement].m_lua_object)
		luabind::call_member<void>(*(m_tpCallbacks[eActionTypeMovement].m_lua_object),*(m_tpCallbacks[eActionTypeMovement].m_method_name),CLuaGameObject(this),u32(eActionTypeMovement),u32(-1));
}

bool CScriptMonster::bfAssignWatch(CEntityAction *tpEntityAction)
{
	return			(true);
}

bool CScriptMonster::bfAssignAnimation(CEntityAction *tpEntityAction)
{
	return			(GetCurrentAction() && !GetCurrentAction()->m_tAnimationAction.m_bCompleted);
}

const Fmatrix CScriptMonster::GetUpdatedMatrix(ref_str caBoneName, const Fvector &tPositionOffset, const Fvector &tAngleOffset)
{
	Fmatrix			l_tMatrix;

	l_tMatrix.setHPB(VPUSH(tAngleOffset));
	l_tMatrix.c		= tPositionOffset;

	if (xr_strlen(caBoneName)) {
		CBoneInstance	&l_tBoneInstance = PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID(*caBoneName));
		l_tMatrix.mulA	(l_tBoneInstance.mTransform);
		l_tMatrix.mulA	(XFORM());
	}

	return			(l_tMatrix);
}

bool CScriptMonster::bfAssignSound(CEntityAction *tpEntityAction)
{
	CSoundAction	&l_tSoundAction = tpEntityAction->m_tSoundAction;
	if (l_tSoundAction.m_bCompleted)
		return		(false);
	if (l_tSoundAction.m_tpSound) {
		if (!l_tSoundAction.m_tpSound->feedback)
			if (!l_tSoundAction.m_bStartedToPlay) {
				const Fmatrix	&l_tMatrix = GetUpdatedMatrix(l_tSoundAction.m_caBoneName,l_tSoundAction.m_tSoundPosition,l_tSoundAction.m_tSoundAngles);
				l_tSoundAction.m_tpSound->play_at_pos(this,l_tMatrix.c,l_tSoundAction.m_bLooped ? TRUE : FALSE);
				l_tSoundAction.m_bStartedToPlay = true;
			}
			else
				l_tSoundAction.m_bCompleted = true;
	}
	else {
		if (xr_strlen(l_tSoundAction.m_caSoundToPlay))
			::Sound->create	(*(l_tSoundAction.m_tpSound = xr_new<ref_sound>()),TRUE,*l_tSoundAction.m_caSoundToPlay);
		else
			l_tSoundAction.m_bCompleted = true;
	}
	return		(!l_tSoundAction.m_bCompleted);
}

bool CScriptMonster::bfAssignParticles(CEntityAction *tpEntityAction)
{
	CParticleAction	&l_tParticleAction = tpEntityAction->m_tParticleAction;
	if (l_tParticleAction.m_bCompleted)
		return		(false);
	if (l_tParticleAction.m_tpParticleSystem) {
		if (true/** !l_tParticleAction.m_tpParticleSystem/**/)
			if (!l_tParticleAction.m_bStartedToPlay) {
				const Fmatrix	&l_tMatrix = GetUpdatedMatrix(*l_tParticleAction.m_caBoneName,l_tParticleAction.m_tParticlePosition,l_tParticleAction.m_tParticleAngles);
				l_tParticleAction.m_tpParticleSystem->SetTransform(l_tMatrix);
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

bool CScriptMonster::bfAssignObject(CEntityAction * /**tpEntityAction/**/)
{
	return			(GetCurrentAction() && !GetCurrentAction()->m_tObjectAction.m_bCompleted);
}

bool CScriptMonster::bfAssignMovement(CEntityAction *tpEntityAction)
{
	CMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	
	if (l_tMovementAction.m_bCompleted)
		return		(false);

	CMovementManager		*l_tpMovementManager = dynamic_cast<CMovementManager*>(this);

	switch (l_tMovementAction.m_tGoalType) {
		case CMovementAction::eGoalTypeObject : {
			CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tMovementAction.m_tpObjectToGo);
			R_ASSERT		(l_tpGameObject);
			l_tpMovementManager->set_path_type(CMovementManager::ePathTypeLevelPath);
			l_tpMovementManager->set_dest_position(l_tpGameObject->Position());
			l_tpMovementManager->set_level_dest_vertex(l_tpGameObject->level_vertex_id());
			break;
		}
		case CMovementAction::eGoalTypePatrolPath : {
			l_tpMovementManager->set_path_type	(CMovementManager::ePathTypePatrolPath);
			l_tpMovementManager->set_path		(l_tMovementAction.m_path,l_tMovementAction.m_path_name);
			l_tpMovementManager->set_start_type	(l_tMovementAction.m_tPatrolPathStart);
			l_tpMovementManager->set_route_type	(l_tMovementAction.m_tPatrolPathStop);
			l_tpMovementManager->set_random		(l_tMovementAction.m_bRandom);
			break;
		}
		case CMovementAction::eGoalTypePathPosition : {
			l_tpMovementManager->set_path_type(CMovementManager::ePathTypeLevelPath);
			l_tpMovementManager->set_dest_position(l_tMovementAction.m_tDestinationPosition);
			l_tpMovementManager->set_level_dest_vertex(ai().level_graph().vertex(l_tpMovementManager->level_vertex_id(),l_tMovementAction.m_tDestinationPosition,true));
			l_tpMovementManager->CLevelLocationSelector::set_evaluator(0);
			break;
		}
		case CMovementAction::eGoalTypeNoPathPosition : {
			if (l_tpMovementManager) {
				l_tpMovementManager->set_path_type(CMovementManager::ePathTypeLevelPath);
				if (l_tpMovementManager->CDetailPathManager::path().empty() || (l_tpMovementManager->CDetailPathManager::path()[l_tpMovementManager->CDetailPathManager::path().size() - 1].m_position.distance_to(l_tMovementAction.m_tDestinationPosition) > .1f)) {
					l_tpMovementManager->CDetailPathManager::m_path.resize(2);
					l_tpMovementManager->CDetailPathManager::m_path[0].m_position = Position();
					l_tpMovementManager->CDetailPathManager::m_path[1].m_position = l_tMovementAction.m_tDestinationPosition;
					l_tpMovementManager->CDetailPathManager::m_current_travel_point	= 0;
				}
			}
			break;
		}
		default : {
			l_tpMovementManager->set_path_type	(CMovementManager::ePathTypeNoPath);
			return								(l_tMovementAction.m_bCompleted = true);
		}
	}

	if (l_tpMovementManager->actual() && l_tpMovementManager->path_completed())
		l_tMovementAction.m_bCompleted = true;

	return		(!l_tMovementAction.m_bCompleted);
}

void CScriptMonster::ResetScriptData(void * /**P/**/)
{
	InitScript				();
}

void CScriptMonster::InitScript()
{
	while (!m_tpActionQueue.empty()) {
		xr_delete	(m_tpActionQueue.front());
		m_tpActionQueue.erase(m_tpActionQueue.begin());
	}
	// animation
	m_tpScriptAnimation		= 0;

	// callbacks
	if (Visual())
		PKinematics(Visual())->Callback(0,0);
}

void CScriptMonster::net_Destroy()
{
	inherited::net_Destroy	();
	InitScript				();
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

void CScriptMonster::clear_callback	(const CScriptMonster::EActionType tActionType)
{
	VERIFY					(tActionType < eActionTypeCount);
	xr_delete				(m_tpCallbacks[tActionType].m_lua_object);
	m_tpCallbacks[tActionType].m_method_name = "";
	if (tActionType) {
		CPatrolPathManager	*l_tpPatrolPathManager = dynamic_cast<CPatrolPathManager*>(this);
		if (l_tpPatrolPathManager)
			l_tpPatrolPathManager->set_callback(m_tpCallbacks[tActionType]);
	}
}

void CScriptMonster::callback		(const CScriptMonster::EActionType tActionType)
{
	if (m_tpCallbacks[tActionType].m_lua_object)
		luabind::call_member<void>(*(m_tpCallbacks[tActionType].m_lua_object),*(m_tpCallbacks[tActionType].m_method_name),CLuaGameObject(this),u32(tActionType));
}

LPCSTR CScriptMonster::GetPatrolPathName()
{
	VERIFY					(GetScriptControl());
	if (m_tpActionQueue.empty())
		return				("");
	return					(*m_tpActionQueue.back()->m_tMovementAction.m_path_name);
}