////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_feel.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Feelings for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../ai_monsters_misc.h"
#include "../../hudmanager.h"
#include "../../actor.h"

//#undef SILENCE

#define MIN_SOUND_VOLUME				.05f

void CAI_Stalker::SetPointLookAngles(const Fvector &tPosition, float &yaw, float &pitch)
{
	Fvector			tTemp;
	tTemp.sub		(tPosition,eye_matrix.c);
	tTemp.getHP		(yaw,pitch);
	VERIFY			(_valid(yaw));
	VERIFY			(_valid(pitch));
	yaw				*= -1;
	pitch			*= -1;
}

void CAI_Stalker::SetFirePointLookAngles(const Fvector &tPosition, float &yaw, float &pitch)
{
	Fvector			tTemp;
	Center			(tTemp);
	tTemp.sub		(tPosition,tTemp);
	tTemp.getHP		(yaw,pitch);
	VERIFY			(_valid(yaw));
	VERIFY			(_valid(pitch));
	yaw				*= -1;
	pitch			*= -1;
}

BOOL CAI_Stalker::feel_vision_isRelevant(CObject* O)
{
	CEntityAlive*	E = dynamic_cast<CEntityAlive*>		(O);
	CInventoryItem*	I = dynamic_cast<CInventoryItem*>	(O);
	if (!E && !I)	return	(FALSE);
	if (E && (E->g_Team() == g_Team()))			return FALSE;
	return(TRUE);
}

void CAI_Stalker::renderable_Render	()
{
	inherited::renderable_Render	();
	if(m_inventory.ActiveItem())
		m_inventory.ActiveItem()->renderable_Render();
}

bool CAI_Stalker::bfIf_I_SeePosition(Fvector tPosition)
{
	float	yaw, pitch;
	Fvector	tVector;
	tVector.sub(tPosition,Position());
	tVector.getHP(yaw,pitch);
	yaw		= angle_normalize_signed(-yaw);
	pitch	= angle_normalize_signed(-pitch);
	return	(angle_difference(yaw,m_head.current.yaw) <= PI_DIV_6);// && angle_difference(pitch,m_head.current.pitch,PI_DIV_6));
}

bool CAI_Stalker::bfCheckForVisibility(CEntity* /**tpEntity/**/)
{
	return(true);
////	if (Level().iGetKeyState(DIK_RCONTROL))
////		return(false);
//#ifdef LOG_PARAMETERS
////	bool		bMessage = g_Alive() && !!dynamic_cast<CActor*>(tpEntity);//!!Levsssssssssssssssssssssel().iGetKeyState(DIK_LALT);
//	int			iLogParameters = (g_Alive() && !!dynamic_cast<CActor*>(tpEntity)) ? (Level().iGetKeyState(DIK_1) ? 2 : Level().iGetKeyState(DIK_0) ? 1 : 0) : 0;
//	string4096	S = "";
//#endif
//	float fResult = 0.f;
//	
//	// computing maximum viewable distance in the specified direction
//	float fDistance = Position().distance_to(tpEntity->Position()), yaw, pitch;
//	Fvector tDirection;
//	tDirection.sub(tpEntity->Position(),Position());
//	tDirection.getHP(yaw,pitch);
//
//	float fEyeFov = eye_fov*PI/180.f, fAlpha = _abs(_min(angle_normalize_signed(yaw - m_head.current.yaw),angle_normalize_signed(pitch - m_head.current.pitch)));
//	float fMaxViewableDistanceInDirection = eye_range*(1 - fAlpha/(fEyeFov/m_fLateralMultiplier));
//	
//	// computing distance weight
//	fResult += fDistance >= fMaxViewableDistanceInDirection ? 0.f : m_fDistanceWeight*(1.f - fDistance/fMaxViewableDistanceInDirection);
//	
//	// computing movement speed weight
//	float fSpeed = 0;
//	if (tpEntity->ps_Size() > 1) {
//		u32 dwTime = tpEntity->ps_Element(tpEntity->ps_Size() - 1).dwTime;
//		if (dwTime < m_dwMovementIdleTime) {
//			fSpeed = tpEntity->ps_Element(tpEntity->ps_Size() - 2).Position().distance_to(tpEntity->ps_Element(tpEntity->ps_Size() - 1).Position())/dwTime;
//			fResult += fSpeed < m_fMaxInvisibleSpeed ? m_fMovementSpeedWeight*fSpeed/m_fMaxInvisibleSpeed : m_fMovementSpeedWeight;
//		}
//	}
//	
//	// computing my ability to view the enemy
//	fResult += m_fCurSpeed < m_fMaxViewableSpeed ? m_fSpeedWeight*(1.f - m_fCurSpeed/m_fMaxViewableSpeed) : m_fSpeedWeight;
//	
//	// computing lightness weight
//	fResult += (1 - float(tpEntity->level_vertex()->light)/255.f)*m_fShadowWeight;
//	
//#ifdef LOG_PARAMETERS
//	if ((g_Alive() && !!dynamic_cast<CActor*>(tpEntity)) && (fResult >= m_fVisibilityThreshold))
//		HUD().outMessage(0xffffffff,cName(),"%s : %d",fResult >= m_fVisibilityThreshold ? "I see actor" : "I don't see actor",Level().timeServer());
//	Msg("**********");
//	Msg("Distance : %f [%f]",fDistance, fDistance >= fMaxViewableDistanceInDirection ? 0.f : m_fDistanceWeight*(1.f - fDistance/fMaxViewableDistanceInDirection));
//	Msg("MySpeed  : %f [%f]",m_fCurSpeed, m_fCurSpeed < m_fMaxViewableSpeed ? m_fSpeedWeight*(1.f - m_fCurSpeed/m_fMaxViewableSpeed) : m_fSpeedWeight);
//	Msg("Speed    : %f [%f]",fSpeed, fSpeed < m_fMaxInvisibleSpeed ? m_fMovementSpeedWeight*fSpeed/m_fMaxInvisibleSpeed : m_fMovementSpeedWeight);
//	Msg("Shadow   : %f [%f]",float(tpEntity->level_vertex()->light)/255.f,(1 - float(tpEntity->level_vertex()->light)/255.f)*m_fShadowWeight);
//	Msg("Result   : %f [%f]",fResult,m_fVisibilityThreshold);
////	if (iLogParameters) {
////		fprintf(ST_VF,"%6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f\n",fDistance,fAlpha,fSpeed,speed(),float(tpEntity->level_vertex()->light)/255.f,float(level_vertex()->light)/255.f,tpEntity->Radius(),float(iLogParameters - 1));
////	}
//#endif
//	return(fResult >= m_fVisibilityThreshold);
}

void CAI_Stalker::SetDirectionLook()
{
	GetDirectionAngles(m_head.target.yaw,m_head.target.pitch);
	m_head.target.yaw *= -1;
	m_head.target.pitch = 0;
	m_body.target = m_head.target;
}

void CAI_Stalker::SetLessCoverLook(const CLevelGraph::CVertex *tpNode, bool bDifferenceLook)
{
	SetDirectionLook();
	SetLessCoverLook(tpNode,MAX_HEAD_TURN_ANGLE,bDifferenceLook);
}

void CAI_Stalker::SetLessCoverLook(const CLevelGraph::CVertex *tpNode, float fMaxHeadTurnAngle, bool bDifferenceLook)
{
	float fAngleOfView = eye_fov/180.f*PI, fMaxSquare = -1.f, fBestAngle = m_head.target.yaw;
	
	CLevelGraph::CVertex	*tpNextNode = 0;
	u32						node_id;
	bool bOk = false;
	if (bDifferenceLook && !CDetailPathManager::path().empty() && (CDetailPathManager::path().size() - 1 > CDetailPathManager::curr_travel_point_index())) {
		CLevelGraph::const_iterator	i, e;
		ai().level_graph().begin(tpNode,i,e);
		for ( ; i != e; ++i) {
			node_id			= ai().level_graph().value(tpNode,i);
			if (!ai().level_graph().valid_vertex_id(node_id))
				continue;
			tpNextNode = ai().level_graph().vertex(node_id);
 			if (ai().level_graph().inside(tpNextNode,CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].position)) {
				bOk = true;
				break;
			}
		}
	}

	if (!bDifferenceLook || !bOk) 
		for (float fIncrement = m_body.current.yaw - fMaxHeadTurnAngle; fIncrement <= m_body.current.yaw + fMaxHeadTurnAngle; fIncrement += 2*fMaxHeadTurnAngle/60.f) {
			float fSquare = ai().level_graph().compute_square(-fIncrement,fAngleOfView,tpNode);
			if (fSquare > fMaxSquare) {
				fMaxSquare = fSquare;
				fBestAngle = fIncrement;
			}
		}
	else {
		float fMaxSquareSingle = -1.f, fSingleIncrement = m_head.target.yaw;
		for (float fIncrement = m_body.current.yaw - fMaxHeadTurnAngle; fIncrement <= m_body.current.yaw + fMaxHeadTurnAngle; fIncrement += 2*fMaxHeadTurnAngle/60.f) {
			float fSquare0 = ai().level_graph().compute_square(-fIncrement,fAngleOfView,tpNode);
			float fSquare1 = ai().level_graph().compute_square(-fIncrement,fAngleOfView,tpNextNode);
			if (fSquare1 - fSquare0 > fMaxSquare) {
				fMaxSquare = fSquare1 - fSquare0;
				fBestAngle = fIncrement;
			}
			if (fSquare0 > fMaxSquareSingle) {
				fMaxSquareSingle = fSquare0;
				fSingleIncrement = fIncrement;
			}
		}
		if (_sqrt(fMaxSquare) < 0*PI_DIV_6)
			fBestAngle = fSingleIncrement;
	}
	
	m_head.target.yaw = fBestAngle;
	VERIFY					(_valid(m_head.target.yaw));
}

void CAI_Stalker::vfValidateAngleDependency(float x1, float &x2, float x3)
{
	float	_x2	= angle_normalize_signed(x2 - x1);
	float	_x3	= angle_normalize_signed(x3 - x1);
	if ((_x2*_x3 <= 0) && (_abs(_x2) + _abs(_x3) > PI - EPS_L))
		x2  = x3;
}

void CAI_Stalker::Exec_Look(float dt)
{
	// normalizing torso angles
	m_body.current.yaw		= angle_normalize_signed	(m_body.current.yaw);
	m_body.current.pitch	= angle_normalize_signed	(m_body.current.pitch);
	m_body.target.yaw		= angle_normalize_signed	(m_body.target.yaw);
	m_body.target.pitch	= angle_normalize_signed	(m_body.target.pitch);
	
	// normalizing head angles
	m_head.current.yaw			= angle_normalize_signed	(m_head.current.yaw);
	m_head.current.pitch			= angle_normalize_signed	(m_head.current.pitch);
	m_head.target.yaw			= angle_normalize_signed	(m_head.target.yaw);
	m_head.target.pitch			= angle_normalize_signed	(m_head.target.pitch);

	// validating angles
//#ifdef DEBUG
//	Msg						("StalkerA (%d, %s) t=%f, c=%f, tt=%f, tc=%f",Level().timeServer(),cName(),m_head.target.yaw,m_head.current.yaw,m_body.target.yaw,m_body.current.yaw);
	VERIFY					(_valid(m_head.current.yaw));
	VERIFY					(_valid(m_head.current.pitch));
	VERIFY					(_valid(m_head.target.yaw));
	VERIFY					(_valid(m_head.target.pitch));
	VERIFY					(_valid(m_body.current.yaw));
	VERIFY					(_valid(m_body.current.pitch));
	VERIFY					(_valid(m_body.target.yaw));
	VERIFY					(_valid(m_body.target.pitch));
//#endif
	vfValidateAngleDependency(m_head.current.yaw,m_head.target.yaw,m_body.target.yaw);
	vfValidateAngleDependency(m_body.current.yaw,m_body.target.yaw,m_head.current.yaw);

	// updating torso angles
	//float					fAngleDifference = _abs(angle_normalize_signed(m_body.current.yaw - m_body.target.yaw));
	float					fSpeedFactor = 1.f;//fAngleDifference < PI_DIV_2 ? 1.f/6.f : 1.f;
	angle_lerp_bounds		(m_body.current.yaw,m_body.target.yaw,fSpeedFactor*m_body.speed,dt);
	angle_lerp_bounds		(m_body.current.pitch,m_body.target.pitch,m_body.speed,dt);
	
	// updating head angles
	angle_lerp_bounds		(m_head.current.yaw,m_head.target.yaw,m_head.speed,dt);
	angle_lerp_bounds		(m_head.current.pitch,m_head.target.pitch,m_head.speed,dt);

	// normalizing torso angles
	m_body.current.yaw		= angle_normalize_signed	(m_body.current.yaw);
	m_body.current.pitch	= angle_normalize_signed	(m_body.current.pitch);
	
	// normalizing head angles
	m_head.current.yaw			= angle_normalize_signed	(m_head.current.yaw);
	m_head.current.pitch			= angle_normalize_signed	(m_head.current.pitch);
//#ifdef DEBUG
//	Msg						("StalkerB (%d, %s) t=%f, c=%f, tt=%f, tc=%f",Level().timeServer(),cName(),m_head.target.yaw,m_head.current.yaw,m_body.target.yaw,m_body.current.yaw);
	VERIFY					(_valid(m_head.current.yaw));
	VERIFY					(_valid(m_head.current.pitch));
	VERIFY					(_valid(m_head.target.yaw));
	VERIFY					(_valid(m_head.target.pitch));
	VERIFY					(_valid(m_body.current.yaw));
	VERIFY					(_valid(m_body.current.pitch));
	VERIFY					(_valid(m_body.target.yaw));
	VERIFY					(_valid(m_body.target.pitch));
//#endif
	
	// updating rotation matrix
	Fmatrix mXFORM;
	mXFORM.setHPB			(-NET_Last.o_model,0,0);
	mXFORM.c.set			(Position());
	XFORM().set				(mXFORM);
	
	// checking if we have to switch onto another task
	Device.Statistic.AI_Think.End	();
	Engine.Sheduler.Slice	();
	Device.Statistic.AI_Think.Begin	();
}

void CAI_Stalker::vfUpdateDynamicObjects()
{
	feel_vision_get(m_tpaVisibleObjects);
	u32 dwTime = m_current_update;
	for (int i=0; i<(int)m_tpaVisibleObjects.size(); ++i) {
		
		CEntity *tpEntity = dynamic_cast<CEntity *>(m_tpaVisibleObjects[i]);
		
		if (!tpEntity || !bfCheckForVisibility(tpEntity))
			continue;
		
		for (int j=0; j<(int)m_tpaDynamicObjects.size(); ++j)
			if (tpEntity == m_tpaDynamicObjects[j].tpEntity) {
				m_tpaDynamicObjects[j].dwTime = dwTime;
				++(m_tpaDynamicObjects[j].dwUpdateCount);
				m_tpaDynamicObjects[j].dwNodeID = tpEntity->level_vertex_id();
				m_tpaDynamicObjects[j].tSavedPosition = tpEntity->Position();
				m_tpaDynamicObjects[j].tOrientation = tfGetOrientation(tpEntity);
				m_tpaDynamicObjects[j].dwMyNodeID = level_vertex_id();
				m_tpaDynamicObjects[j].tMySavedPosition = Position();
				m_tpaDynamicObjects[j].tMyOrientation = m_body.current;
				break;
			}
		
		if (j >= (int)m_tpaDynamicObjects.size()) {
			if ((int)m_tpaDynamicObjects.size() >= m_dwMaxDynamicObjectsCount)	{
				u32 dwBest = dwTime + 1, dwIndex = u32(-1);
				for (int j=0; j<(int)m_tpaDynamicObjects.size(); ++j)
					if (m_tpaDynamicObjects[j].dwTime < dwBest) {
						dwIndex = i;
						dwBest = m_tpaDynamicObjects[j].dwTime;
					}
				if (dwIndex < m_tpaDynamicObjects.size()) {
					m_tpaDynamicObjects[dwIndex].dwTime = dwTime;
					m_tpaDynamicObjects[dwIndex].dwUpdateCount = 1;
					m_tpaDynamicObjects[dwIndex].dwNodeID = tpEntity->level_vertex_id();
					m_tpaDynamicObjects[dwIndex].tSavedPosition = tpEntity->Position();
					m_tpaDynamicObjects[dwIndex].tOrientation = tfGetOrientation(tpEntity);
					m_tpaDynamicObjects[dwIndex].dwMyNodeID = level_vertex_id();
					m_tpaDynamicObjects[dwIndex].tMySavedPosition = Position();
					m_tpaDynamicObjects[dwIndex].tMyOrientation = m_body.current;
					m_tpaDynamicObjects[dwIndex].tpEntity = tpEntity;
				}
			}
			else {
				SDynamicObject tDynamicObject;
				tDynamicObject.dwTime = dwTime;
				tDynamicObject.dwUpdateCount = 1;
				tDynamicObject.dwNodeID = tpEntity->level_vertex_id();
				tDynamicObject.tSavedPosition = tpEntity->Position();
				tDynamicObject.tOrientation = tfGetOrientation(tpEntity);
				tDynamicObject.dwMyNodeID = level_vertex_id();
				tDynamicObject.tMySavedPosition = Position();
				tDynamicObject.tMyOrientation = m_body.current;
				tDynamicObject.tpEntity = tpEntity;
				m_tpaDynamicObjects.push_back(tDynamicObject);
			}
		}
	}
	
	// verifying if object is online
	for (int i=0, n=m_tpaDynamicObjects.size(); i<n; ++i)
		if (m_tpaDynamicObjects[i].tpEntity && m_tpaDynamicObjects[i].tpEntity->getDestroy()) {
			m_tpaDynamicObjects.erase(m_tpaDynamicObjects.begin() + i);
			--i;
			--n;
		}
}

int	 CAI_Stalker::ifFindDynamicObject(CEntity *tpEntity)
{
	for (int i=0, n=m_tpaDynamicObjects.size(); i<n; ++i)
		if (m_tpaDynamicObjects[i].tpEntity == tpEntity)
			return(i);
	return(-1);
}

int	 CAI_Stalker::ifFindHurtIndex(CEntity *tpEntity)
{
	for (int i=0; i<(int)m_tpaHurts.size(); ++i)
		if (m_tpaHurts[i].tpEntity == tpEntity)
			return(i);
	return(-1);
}

void CAI_Stalker::vfAddHurt(const SHurt &tHurt)
{
	if (m_tpaHurts.size() >= MAX_HURT_COUNT)
		m_tpaHurts.erase(m_tpaHurts.begin());
	m_tpaHurts.push_back(tHurt);
}

void CAI_Stalker::vfUpdateHurt(const SHurt &tHurt)
{
	int iIndex = ifFindHurtIndex(tHurt.tpEntity);
	if (-1 != iIndex)
		m_tpaHurts[iIndex].dwTime = tHurt.dwTime;
	else
		vfAddHurt(tHurt);
}

bool CAI_Stalker::bfCheckIfSound()
{
	int iIndex;
	SelectSound(iIndex);
	if ((iIndex > 0) && m_tpaDynamicSounds[iIndex].tpEntity && ((iIndex != m_iSoundIndex) || (m_tpaDynamicSounds[iIndex].dwTime > m_dwSoundTime))) {
		m_iSoundIndex = iIndex;
		m_dwSoundTime = m_tpaDynamicSounds[iIndex].dwTime;
		return(true);
	}
	else
		return(false);
}

void CAI_Stalker::SelectSound(int &iIndex)
{
	iIndex = -1;
	float fMaxPower = 0.f;
	for (int i=0; i<(int)m_tpaDynamicSounds.size(); ++i)
		if (m_tpaDynamicSounds[i].tpEntity && (m_tpaDynamicSounds[i].tpEntity->g_Team() != g_Team()))
			if ((m_tpaDynamicSounds[i].dwTime + SOUND_UPDATE_DELAY > m_current_update) && (m_tpaDynamicSounds[i].fPower > fMaxPower)) {
				fMaxPower = m_tpaDynamicSounds[i].fPower;
				iIndex = i;
			}
}

void CAI_Stalker::feel_sound_new(CObject* who, int eType, const Fvector &Position, float power)
{
#ifndef SILENCE
	Msg("%s (%d) - sound type %x from %s at %d in (%.2f,%.2f,%.2f) with power %.2f",cName(),Level().timeServer(),eType,who ? who->cName() : "world",Level().timeServer(),Position.x,Position.y,Position.z,power);
#endif

#ifdef IGNORE_ACTOR
	if ((who == Level().CurrentEntity()) || (who && (who->H_Parent() == Level().CurrentEntity())))
		return;
#endif
	if (m_dwParticularState == 7)
		return;

	if (!g_Alive()) {
		m_tpaDynamicSounds.clear();
		m_iSoundIndex = -1;
		return;
	}
	
	power *= 1;//ffGetStartVolume(ESoundTypes(eType));
	
	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING) {
		power = 1.f;
		// if enemy made a shot to us, add a fictitious hurt
		CEntity *tpEntity = dynamic_cast<CEntity *>(who);
		if (tpEntity) {
			if (tpEntity->getDestroy() || (int(tpEntity->level_vertex_id()) <= 0))
				return;
			bool bFound = false;
			objVisible	&VisibleEnemies = Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
			for (int i=0, n=VisibleEnemies.size(); i<n; ++i)
				if (VisibleEnemies[i].key == tpEntity) {
					bFound = true;
					break;
				}
			if (bFound) {
				bFound = false;
				float yaw1 = 0, pitch1 = 0;
				CCustomMonster	*tpCustomMonster = dynamic_cast<CCustomMonster *>(tpEntity);
				if (tpCustomMonster) {
					yaw1		= -tpCustomMonster->m_head.current.yaw;
					pitch1		= -tpCustomMonster->m_head.current.pitch;
					bFound		= true;
				}
				else {
					CActor		*tpActor = dynamic_cast<CActor *>(tpEntity);
					if (tpActor) {
						yaw1	= tpActor->Orientation().yaw;
						pitch1	= tpActor->Orientation().pitch;
						bFound	= true;
					}
				}
				if (bFound && bfCheckIfCanKillTarget(tpEntity, this->Position(), yaw1,pitch1)) {//,5.f/180.f*PI)) {
					SHurt tHurt;
					tHurt.tpEntity = tpEntity;
					tHurt.dwTime = Level().timeServer();
#ifndef SILENCE
					Msg("! Adding fictitious hurt");
#endif
					vfUpdateHurt(tHurt);
				}
			}
		}
	}
	
	u32 dwTime = m_current_update;

	if ((power >= 0*m_fSensetivity*m_fSoundPower) && (power >= MIN_SOUND_VOLUME)) {
		if (!who || ((this != who) && (!who->H_Parent() || (who->H_Parent() != this)))) {
			CEntity *tpEntity = dynamic_cast<CEntity *>(who);
			int iIndex = ifFindDynamicObject(tpEntity);
			if ((((!tpEntity || (tpEntity->g_Team() != g_Team()) || !m_tEnemy.m_enemy) && (!who || !who->H_Parent() || !dynamic_cast<CEntity*>(who->H_Parent()) || (dynamic_cast<CEntity*>(who->H_Parent())->g_Team() != g_Team())))) && ((iIndex == -1) || (m_tpaDynamicObjects[iIndex].dwTime < m_current_update))) {
				int j;
#ifndef SILENCE
				Msg("* %s - ref_sound type %x from %s at %d in (%.2f,%.2f,%.2f) with power %.2f",cName(),eType,who ? who->cName() : "world",Level().timeServer(),Position.x,Position.y,Position.z,power);
#endif
				for ( j=0; j<(int)m_tpaDynamicSounds.size(); ++j)
					if (who == m_tpaDynamicSounds[j].tpEntity) {
						m_tpaDynamicSounds[j].eSoundType		= ESoundTypes(eType);
						m_tpaDynamicSounds[j].dwTime			= dwTime;
						m_tpaDynamicSounds[j].fPower			= power;
						++(m_tpaDynamicSounds[j].dwUpdateCount);
						m_tpaDynamicSounds[j].tSavedPosition	= Position;
						m_tpaDynamicSounds[j].tOrientation		= tfGetOrientation(tpEntity);
						m_tpaDynamicSounds[j].tMySavedPosition	= this->Position();
						m_tpaDynamicSounds[j].tMyOrientation	= m_body.current;
						m_tpaDynamicSounds[j].tpEntity			= tpEntity;
						m_tpaDynamicSounds[j].dwNodeID			= tpEntity ? tpEntity->level_vertex_id() : level_vertex_id();
						m_tpaDynamicSounds[j].dwMyNodeID		= level_vertex_id();
						R_ASSERT2(ai().level_graph().valid_vertex_id(m_tpaDynamicSounds[j].dwNodeID), "Invalid sound object vertex!");
						if (tpEntity && !ai().level_graph().inside(ai().level_graph().vertex(m_tpaDynamicSounds[j].dwNodeID),Position))
							m_tpaDynamicSounds[j].tSavedPosition	= ai().level_graph().vertex_position(m_tpaDynamicSounds[j].dwNodeID);
					}
				if (j >= (int)m_tpaDynamicSounds.size()) {
					if ((int)m_tpaDynamicSounds.size() >= m_dwMaxDynamicSoundsCount)	{
						u32 dwBest = dwTime + 1, dwIndex = u32(-1);
						for (int j=0; j<(int)m_tpaDynamicSounds.size(); ++j)
							if (m_tpaDynamicSounds[j].dwTime < dwBest) {
								dwIndex = j;
								dwBest = m_tpaDynamicSounds[j].dwTime;
							}
						if (dwIndex < (int)m_tpaDynamicSounds.size()) {
							m_tpaDynamicSounds[dwIndex].eSoundType			= ESoundTypes(eType);
							m_tpaDynamicSounds[dwIndex].dwTime				= dwTime;
							m_tpaDynamicSounds[dwIndex].fPower				= power;
							m_tpaDynamicSounds[dwIndex].dwUpdateCount		= 1;
							m_tpaDynamicSounds[dwIndex].tSavedPosition		= Position;
							m_tpaDynamicSounds[dwIndex].tOrientation		= tfGetOrientation(tpEntity);
							m_tpaDynamicSounds[dwIndex].tMySavedPosition	= this->Position();
							m_tpaDynamicSounds[dwIndex].tMyOrientation		= m_body.current;
							m_tpaDynamicSounds[dwIndex].tpEntity			= tpEntity;
							m_tpaDynamicSounds[dwIndex].dwNodeID			= tpEntity ? tpEntity->level_vertex_id() : level_vertex_id();
							m_tpaDynamicSounds[dwIndex].dwMyNodeID			= level_vertex_id();
							R_ASSERT2(ai().level_graph().valid_vertex_id(m_tpaDynamicSounds[dwIndex].dwNodeID), "Invalid sound object vertex!");
							if (tpEntity && !ai().level_graph().inside(ai().level_graph().vertex(m_tpaDynamicSounds[dwIndex].dwNodeID),Fvector(Position)))
								m_tpaDynamicSounds[dwIndex].tSavedPosition	= ai().level_graph().vertex_position(m_tpaDynamicSounds[dwIndex].dwNodeID);
						}
					}
					else {
						SDynamicSound					tDynamicSound;
						tDynamicSound.eSoundType		= ESoundTypes(eType);
						tDynamicSound.dwTime			= dwTime;
						tDynamicSound.fPower			= power;
						tDynamicSound.dwUpdateCount		= 1;
						tDynamicSound.tSavedPosition	= Position;
						tDynamicSound.tOrientation		= tfGetOrientation(tpEntity);
						tDynamicSound.tMySavedPosition	= this->Position();
						tDynamicSound.tMyOrientation	= m_body.current;
						tDynamicSound.tpEntity			= tpEntity;
						tDynamicSound.dwNodeID			= tpEntity ? tpEntity->level_vertex_id() : level_vertex_id();
						tDynamicSound.dwMyNodeID		= level_vertex_id();
						R_ASSERT2(ai().level_graph().valid_vertex_id(tDynamicSound.dwNodeID), "Invalid sound object vertex!");
						if (tpEntity && !ai().level_graph().inside(ai().level_graph().vertex(tDynamicSound.dwNodeID),Fvector(Position)))
							tDynamicSound.tSavedPosition	= ai().level_graph().vertex_position(tDynamicSound.dwNodeID);
						m_tpaDynamicSounds.push_back	(tDynamicSound);
					}
				}
			}
		}
	}
	// computing total power of my own sounds for computing tha ability to hear the others
	if (m_fSoundPower < power) {
		m_fSoundPower = m_fStartPower = power;
		m_dwSoundUpdate = dwTime;
	}
}

void CAI_Stalker::vfUpdateVisibilityBySensitivity()
{
	m_iSoundIndex = -1;
	for (int i=0; i<(int)m_tpaDynamicSounds.size(); ++i)
		if (m_tpaDynamicSounds[i].tpEntity == m_tSavedEnemy) {
			if (m_tpaDynamicSounds[i].dwTime > m_dwLostEnemyTime) {
				m_iSoundIndex			= i;
				m_dwLostEnemyTime		= m_tpaDynamicSounds[m_iSoundIndex].dwTime;

				m_tSavedEnemyPosition	= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
				m_dwSavedEnemyNodeID	= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
				R_ASSERT2				(ai().level_graph().valid_vertex_id(m_dwSavedEnemyNodeID), "Invalid enemy vertex");
				m_tMySavedPosition		= m_tpaDynamicSounds[m_iSoundIndex].tMySavedPosition;
				m_dwMyNodeID			= m_tpaDynamicSounds[m_iSoundIndex].dwMyNodeID;
				m_dwLastEnemySearch		= 0;
				int						iIndex = ifFindDynamicObject(m_tSavedEnemy);
				if (-1 != iIndex) {
					m_tpaDynamicObjects[iIndex].tSavedPosition		= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
					m_tpaDynamicObjects[iIndex].dwNodeID			= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
					m_tpaDynamicObjects[iIndex].tMySavedPosition	= m_tpaDynamicSounds[m_iSoundIndex].tMySavedPosition;
					m_tpaDynamicObjects[iIndex].dwMyNodeID			= m_tpaDynamicSounds[m_iSoundIndex].dwMyNodeID;
					m_tpaDynamicObjects[iIndex].tMyOrientation		= m_tpaDynamicSounds[m_iSoundIndex].tMyOrientation;
					m_tpaDynamicObjects[iIndex].dwTime				= m_tpaDynamicSounds[m_iSoundIndex].dwTime;
				}
				vfValidatePosition		(m_tSavedEnemyPosition,m_dwSavedEnemyNodeID);
			}
			break;
		}
}