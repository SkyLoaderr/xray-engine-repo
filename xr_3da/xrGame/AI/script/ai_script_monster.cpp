////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_monster.cpp
//	Created 	: 06.10.2003
//  Modified 	: 06.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Script behaviour interface for all the monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_monster.h"
#include "..\\..\\CustomMonster.h"
#include "..\\..\\..\\feel_vision.h"
#include "..\\..\\ai_script_actions.h"
#include "..\\..\\weapon.h"
#include "..\\..\\ParticlesObject.h"

CScriptMonster::CScriptMonster()
{
	m_tpActionQueue.clear	();
	strcpy					(m_caScriptName,"");
	m_bScriptControl		= false;
	Init					();
}

CScriptMonster::~CScriptMonster()
{
}

void CScriptMonster::SetScriptControl(const bool bScriptControl, LPCSTR caSciptName)
{
	m_bScriptControl	= bScriptControl;
	strcpy				(m_caScriptName,caSciptName);
#ifdef DEBUG
	if (bScriptControl)
		Msg				("* Script %s set object %s under its control",caSciptName,cName());
	else
		Msg				("* Script %s freed object %s from its control",caSciptName,cName());
#endif
}

bool CScriptMonster::GetScriptControl() const
{
	return				(m_bScriptControl);
}

LPCSTR CScriptMonster::GetScriptControlName() const
{
	return				(m_caScriptName);
}

bool CScriptMonster::CheckObjectVisibility(const CObject *tpObject)
{
	CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(this);
	if (!l_tpCustomMonster)
		return			(false);
	l_tpCustomMonster->feel_vision_get(l_tpCustomMonster->m_tpaVisibleObjects);
	xr_vector<CObject*>::const_iterator	I = l_tpCustomMonster->m_tpaVisibleObjects.begin();
	xr_vector<CObject*>::const_iterator	E = l_tpCustomMonster->m_tpaVisibleObjects.end();
	for ( ; I != E; I++)
		if (tpObject == dynamic_cast<CObject*>(*I))
			return		(true);
	return				(false);
}

void CScriptMonster::UseObject(const CObject *tpObject)
{
#pragma todo("Dima to Dima : Use object specified by script")
}

void CScriptMonster::AddAction(const CEntityAction *tpEntityAction)
{
	m_tpActionQueue.push_back(xr_new<CEntityAction>(*tpEntityAction));
}

CEntityAction *CScriptMonster::GetCurrentAction()
{
	if (m_tpActionQueue.empty())
		return(0);
	else
		return(m_tpActionQueue.front());
}

void CScriptMonster::ProcessScripts()
{
	CEntityAction	*l_tpEntityAction = 0;
	while (!m_tpActionQueue.empty()) {
		l_tpEntityAction= *m_tpActionQueue.begin();
		R_ASSERT	(l_tpEntityAction);
		if (!l_tpEntityAction->CheckIfActionCompleted())
			break;

		xr_delete	(l_tpEntityAction);
		m_tpActionQueue.erase(m_tpActionQueue.begin());
	}

	if (m_tpActionQueue.empty()) {
		Msg			("* Object %s has an empty script queue!",cName());
		ResetScriptData();
		return;
	}

	bfAssignWatch	(l_tpEntityAction);
	bfAssignAnimation(l_tpEntityAction);
	bfAssignSound	(l_tpEntityAction);
	bfAssignParticles(l_tpEntityAction);
	bfAssignObject	(l_tpEntityAction);
	bfAssignMovement(l_tpEntityAction);
}

bool CScriptMonster::bfAssignWatch(CEntityAction *tpEntityAction)
{
	return			(true);//GetCurrentAction() && !GetCurrentAction()->m_tWatchAction.m_bCompleted);
}

bool CScriptMonster::bfAssignAnimation(CEntityAction *tpEntityAction)
{
	return			(GetCurrentAction() && !GetCurrentAction()->m_tAnimationAction.m_bCompleted);
}

void __stdcall ParticleCallback(CBoneInstance *tpBoneInstance)
{
	CScriptMonster	*l_tpScriptMonster = static_cast<CScriptMonster*>(tpBoneInstance->Callback_Param);
	CCustomMonster	*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(l_tpScriptMonster);
	R_ASSERT		(l_tpScriptMonster);
	if (!l_tpCustomMonster || l_tpCustomMonster->g_Alive()) {
		CParticleAction	&l_tParticleAction = l_tpScriptMonster->GetCurrentAction()->m_tParticleAction;
		LPCSTR			l_caBoneName = l_tParticleAction.m_caBoneName;
		if (l_tpScriptMonster->GetCurrentAction() && strlen(l_caBoneName)) {
			CParticlesObject	*l_tpParticlesObject = l_tParticleAction.m_tpParticleSystem;
			l_tpParticlesObject->UpdateParent(l_tpScriptMonster->GetUpdatedMatrix(l_caBoneName,l_tParticleAction.m_tParticlePosition,l_tParticleAction.m_tParticleAngles),l_tParticleAction.m_tParticleVelocity);
		}
		else {
			CBoneInstance	&l_tBoneInstance = PKinematics(l_tpScriptMonster->Visual())->LL_GetInstance(PKinematics(l_tpScriptMonster->Visual())->LL_BoneID(l_caBoneName));
			l_tBoneInstance.set_callback(0,0);
		}
	}
}

void __stdcall SoundCallback(CBoneInstance *tpBoneInstance)
{
	CScriptMonster	*l_tpScriptMonster = static_cast<CScriptMonster*>(tpBoneInstance->Callback_Param);
	CCustomMonster	*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(l_tpScriptMonster);
	R_ASSERT		(l_tpScriptMonster);
	if (!l_tpCustomMonster || l_tpCustomMonster->g_Alive()) {
		CSoundAction	&l_tSoundAction = l_tpScriptMonster->GetCurrentAction()->m_tSoundAction;
		LPCSTR			l_caBoneName = l_tSoundAction.m_caBoneName;
		if (l_tpScriptMonster->GetCurrentAction() && strlen(l_caBoneName) && l_tSoundAction.m_tpSound->feedback)
			l_tSoundAction.m_tpSound->feedback->set_position(l_tpScriptMonster->GetUpdatedMatrix(l_caBoneName,l_tSoundAction.m_tSoundPosition,Fvector().set(0,0,0)).c);
		else {
			R_ASSERT		(l_caBoneName && strlen(l_caBoneName));
			CBoneInstance	&l_tBoneInstance = PKinematics(l_tpScriptMonster->Visual())->LL_GetInstance(PKinematics(l_tpScriptMonster->Visual())->LL_BoneID(l_caBoneName));
			l_tBoneInstance.set_callback(0,0);
		}
	}
}

const Fmatrix CScriptMonster::GetUpdatedMatrix(LPCSTR caBoneName, const Fvector &tPositionOffset, const Fvector &tAngleOffset, BoneCallback fpBoneCallback)
{
	Fmatrix			l_tMatrix;

	l_tMatrix.setHPB(VPUSH(tAngleOffset));
	l_tMatrix.c		= tPositionOffset;

	if (caBoneName && strlen(caBoneName)) {
		CBoneInstance	&l_tBoneInstance = PKinematics(Visual())->LL_GetInstance(PKinematics(Visual())->LL_BoneID(caBoneName));
#pragma todo("Dima to Dima : null callbacks after completion")
		if (fpBoneCallback)
			l_tBoneInstance.set_callback(fpBoneCallback,this);
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
				CEntityAlive	*l_tpEntityAlive = dynamic_cast<CEntityAlive*>(this);
				const Fmatrix	&l_tMatrix = GetUpdatedMatrix(l_tSoundAction.m_caBoneName,l_tSoundAction.m_tSoundPosition,l_tSoundAction.m_tSoundAngles,(!l_tpEntityAlive || l_tpEntityAlive->g_Alive()) ? SoundCallback : 0);
				l_tSoundAction.m_tpSound->play_at_pos(this,l_tMatrix.c,l_tSoundAction.m_bLooped);
				l_tSoundAction.m_bStartedToPlay = true;
			}
			else
				l_tSoundAction.m_bCompleted = true;
	}
	else
		l_tSoundAction.m_bCompleted = true;
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
				CEntityAlive	*l_tpEntityAlive = dynamic_cast<CEntityAlive*>(this);
				const Fmatrix	&l_tMatrix = GetUpdatedMatrix(l_tParticleAction.m_caBoneName,l_tParticleAction.m_tParticlePosition,l_tParticleAction.m_tParticleAngles,(!l_tpEntityAlive || l_tpEntityAlive->g_Alive()) ? ParticleCallback : 0);
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

bool CScriptMonster::bfAssignObject(CEntityAction *tpEntityAction)
{
	return			(GetCurrentAction() && !GetCurrentAction()->m_tObjectAction.m_bCompleted);
}

void CScriptMonster::vfChoosePatrolPathPoint(CEntityAction *tpEntityAction)
{
	CMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	if (l_tMovementAction.m_bCompleted)
		return;

	CLevel::SPathPairIt	I = Level().m_PatrolPaths.find(l_tMovementAction.m_caPatrolPathToGo);
	if (I == Level().m_PatrolPaths.end()) {
		Msg			("* [LUA] Patrol path %s not found!",l_tMovementAction.m_caPatrolPathToGo);
		l_tMovementAction.m_bCompleted = true;
		return;
	}

	const CLevel::SPath	&l_tPatrolPath = (*I).second;

	int			l_iFirst = -1;
	if (m_iPreviousPatrolPoint == -1) {
		switch (l_tMovementAction.m_tPatrolPathStart) {
			case CPatrolPathParams::ePatrolPathFirst : {
				l_iFirst = 0;
				break;
			}
			case CPatrolPathParams::ePatrolPathLast : {
				l_iFirst = l_tPatrolPath.tpaWayPoints.size() - 1;
				break;
			}
			case CPatrolPathParams::ePatrolPathNearest : {
				float		l_fMinLength = 10000.f;
				for (int i=0, n=l_tPatrolPath.tpaWayPoints.size(); i<n; i++) {
					float	l_fTempDistance = l_tPatrolPath.tpaWayPoints[i].tWayPoint.distance_to(Position());
					if (l_fTempDistance < l_fMinLength) {
						l_iFirst		= i;
						l_fMinLength	= l_fTempDistance;
					}
				}
				break;
			}
			default : NODEFAULT;
		}
		R_ASSERT	(l_iFirst>=0);
	}
	else {
		if (l_tMovementAction.m_tDestinationPosition.distance_to(Position()) > .1f)
			return;

		int			l_iCount = 0;
		for (int i=0, n=(int)l_tPatrolPath.tpaWayLinks.size(); i<n; i++)
			if ((l_tPatrolPath.tpaWayLinks[i].wFrom == m_iCurrentPatrolPoint) && (l_tPatrolPath.tpaWayLinks[i].wTo != m_iPreviousPatrolPoint)) {
				if (!l_iCount)
					l_iFirst = i;
				l_iCount++;
			}

		if (!l_iCount) {
			switch (l_tMovementAction.m_tPatrolPathStop) {
				case CPatrolPathParams::ePatrolPathStop : {
					l_tMovementAction.m_bCompleted = true;
					return;
				}
				case CPatrolPathParams::ePatrolPathContinue : {
					for (int i=0, n=(int)l_tPatrolPath.tpaWayLinks.size(); i<n; i++)
						if (l_tPatrolPath.tpaWayLinks[i].wFrom == m_iCurrentPatrolPoint) {
							l_iFirst = i;
							break;
						}
					if (l_iFirst == -1) {
						l_tMovementAction.m_bCompleted = true;
						return;
					}
					break;
				}
				default : NODEFAULT;
			}
		}
		else {
			if (l_tMovementAction.m_bRandom && (l_iCount > 1)) {
				int			l_iChoosed = ::Random.randI(l_iCount);
				l_iCount	= 0;
				for (int i=0, n=(int)l_tPatrolPath.tpaWayLinks.size(); i<n; i++)
					if ((l_tPatrolPath.tpaWayLinks[i].wFrom == m_iCurrentPatrolPoint) && (l_tPatrolPath.tpaWayLinks[i].wTo != m_iPreviousPatrolPoint))
						if (l_iCount == l_iChoosed) {
							l_iFirst = i;
							break;
						}
						else
							l_iCount++;
			}
		}
	}
	
	m_iPreviousPatrolPoint	= m_iCurrentPatrolPoint;
	m_iCurrentPatrolPoint	= l_tPatrolPath.tpaWayLinks[l_iFirst].wTo;
	l_tMovementAction.m_tNodeID = l_tPatrolPath.tpaWayPoints[m_iCurrentPatrolPoint].dwNodeID;
	l_tMovementAction.m_tDestinationPosition = l_tPatrolPath.tpaWayPoints[m_iCurrentPatrolPoint].tWayPoint;
}

bool CScriptMonster::bfAssignMovement(CEntityAction *tpEntityAction)
{
	CMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	
	if (l_tMovementAction.m_bCompleted)
		return		(false);

	CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(this);

	switch (l_tMovementAction.m_tGoalType) {
		case CMovementAction::eGoalTypeObject : {
			CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tMovementAction.m_tpObjectToGo);
			R_ASSERT		(l_tpGameObject);
			l_tMovementAction.m_tDestinationPosition = l_tMovementAction.m_tpObjectToGo->Position();
			if (Position().distance_to(l_tMovementAction.m_tDestinationPosition) < .1f)
				l_tMovementAction.m_bCompleted = true;
			else
				if (!l_tpGameObject)
					if (getAI().bfInsideNode(AI_Node,l_tMovementAction.m_tDestinationPosition))
						l_tMovementAction.m_tNodeID	= AI_NodeID;
					else
						if (l_tpCustomMonster && (int(l_tpCustomMonster->AI_Path.DestNode) > 0) && getAI().bfInsideNode(getAI().Node(l_tpCustomMonster->AI_Path.DestNode),l_tMovementAction.m_tDestinationPosition))
							l_tMovementAction.m_tNodeID	= l_tpCustomMonster->AI_Path.DestNode;
						else
							l_tMovementAction.m_tNodeID	= getAI().q_Node(AI_NodeID,l_tMovementAction.m_tDestinationPosition,false);
				else
					l_tMovementAction.m_tNodeID	= l_tpGameObject->AI_NodeID;
			break;
		}
		case CMovementAction::eGoalTypePatrolPath : {
			vfChoosePatrolPathPoint(tpEntityAction);
			break;
		}
		case CMovementAction::eGoalTypePathPosition : {
			if (getAI().bfInsideNode(AI_Node,l_tMovementAction.m_tDestinationPosition))
				l_tMovementAction.m_tNodeID	= AI_NodeID;
			else
				if (l_tpCustomMonster && (int(l_tpCustomMonster->AI_Path.DestNode) > 0) && getAI().bfInsideNode(getAI().Node(l_tpCustomMonster->AI_Path.DestNode),l_tMovementAction.m_tDestinationPosition))
					l_tMovementAction.m_tNodeID	= l_tpCustomMonster->AI_Path.DestNode;
				else
					l_tMovementAction.m_tNodeID	= getAI().q_Node(AI_NodeID,l_tMovementAction.m_tDestinationPosition,false);
			break;
		}
		case CMovementAction::eGoalTypeNoPathPosition : {
			l_tMovementAction.m_tNodeID	= 1;
			if (l_tpCustomMonster) {
				if (l_tpCustomMonster->AI_Path.TravelPath.empty() || (l_tpCustomMonster->AI_Path.TravelPath[l_tpCustomMonster->AI_Path.TravelPath.size() - 1].P.distance_to(l_tMovementAction.m_tDestinationPosition) > .1f)) {
					l_tpCustomMonster->AI_Path.TravelPath.resize(2);
					l_tpCustomMonster->AI_Path.TravelPath[0].P = Position();
					l_tpCustomMonster->AI_Path.TravelPath[1].P = l_tMovementAction.m_tDestinationPosition;
					l_tpCustomMonster->AI_Path.TravelStart	= 0;
				}
			}
			break;
		}
		default :
			return(l_tMovementAction.m_bCompleted = true);
	}

	if (int(l_tMovementAction.m_tNodeID) > 0)
		l_tpCustomMonster->AI_Path.DestNode =  l_tMovementAction.m_tNodeID;
	else {
		Msg		("! Cannot find a node for the specified position [%f][%f][%f]",VPUSH(l_tMovementAction.m_tDestinationPosition));
		l_tMovementAction.m_tDestinationPosition = Position();
		l_tMovementAction.m_tNodeID = AI_NodeID;
	}

	return		(!l_tMovementAction.m_bCompleted);
}

void CScriptMonster::ResetScriptData(void *P)
{
	Init					();
}

void CScriptMonster::Init()
{
	// movement
	m_iCurrentPatrolPoint	= m_iPreviousPatrolPoint = -1;
	// animation
	m_tpScriptAnimation		= 0;
}

void CScriptMonster::net_Destroy()
{
	inherited::net_Destroy	();
	Init					();
}