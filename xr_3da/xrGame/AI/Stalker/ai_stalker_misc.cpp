////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_misc.cpp
//	Created 	: 27.02.2003
//  Modified 	: 27.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../ai_monsters_misc.h"
#include "../../actor.h"
#include "../../bolt.h"
#include "../../ai_script_actions.h"
#include "../../ef_storage.h"

void CAI_Stalker::vfSetParameters(
	PathManagers::CAbstractVertexEvaluator	*tpPathEvaluator, 
	Fvector									*tpDesiredPosition, 
	PathManagers::CAbstractVertexEvaluator	*tpNodeEvaluator, 
	EObjectAction							tWeaponState, 
	EPathType								tGlobalPathType, 
	EDetailPathType							tPathType, 
	EBodyState								tBodyState, 
	EMovementType							tMovementType, 
	EMentalState							tMentalState, 
	ELookType								tLookType
)
{
	R_ASSERT		(eLookTypePoint != tLookType);
	vfSetParameters	(tpPathEvaluator,tpDesiredPosition,tpNodeEvaluator,tWeaponState,tGlobalPathType,tPathType,tBodyState,tMovementType,tMentalState, tLookType, Fvector().set(0,0,0));
}

void CAI_Stalker::vfSetParameters(
	PathManagers::CAbstractVertexEvaluator	*tpPathEvaluator, 
	Fvector									*tpDesiredPosition, 
	PathManagers::CAbstractVertexEvaluator	*tpNodeEvaluator, 
	EObjectAction							tWeaponState, 
	EPathType								tGlobalPathType,
	EDetailPathType							tPathType, 
	EBodyState								tBodyState, 
	EMovementType							tMovementType, 
	EMentalState							tMentalState, 
	ELookType								tLookType, 
	const Fvector							&tPointToLook, 
	u32										dwLookOverDelay
)
{
	CStalkerMovementManager::update(
		tpNodeEvaluator,
		tpPathEvaluator,
		tpDesiredPosition,
		tGlobalPathType,
		tPathType,
		tBodyState,
		tMovementType,
		tMentalState
	);
	CSightManager::update			(
		tLookType,
		&tPointToLook,
		dwLookOverDelay
	);
	
//	if (!GetScriptControl())
	CObjectHandler::set_dest_state	(tWeaponState);
}

bool CAI_Stalker::useful		(const CGameObject *object) const
{
	if (!CItemManager::useful(object))
		return			(false);

	const CInventoryItem *inventory_item = dynamic_cast<const CInventoryItem*>(object);
	if (!inventory_item)
		return			(false);

	const CBolt			*bolt = dynamic_cast<const CBolt*>(object);
	if (bolt)
		return			(false);

	return				(true);
}

float CAI_Stalker::evaluate		(const CGameObject *object) const
{
	return				(Position().distance_to_sqr(object->Position()));
}

void CAI_Stalker::vfUpdateParameters(bool &A, bool &B, bool &C, bool &D, bool &E, bool &F, bool &G, bool &H, bool &I, bool &J, bool &K, bool &L, bool &M)
{
//	// ref_sound
//	int					iIndex;
//	
//	A					= (iIndex > -1) && ((m_sounds[iIndex].m_sound_type & SOUND_TYPE_WEAPON) == SOUND_TYPE_WEAPON);
//	B					= (iIndex > -1) && !A;
//	bool _A				= (m_sound_index > -1) && ((m_sounds[m_sound_index].m_sound_type & SOUND_TYPE_WEAPON) == SOUND_TYPE_WEAPON);
//	bool _B				= (m_sound_index > -1) && !A;
//	
//	if (A || B)
//		m_sound_index = iIndex;
//	else
//		if (((!A && _A) || (!B && _B)) && (!m_sounds[m_sound_index].tpEntity || m_sounds[m_sound_index].tpEntity->g_Alive()) && (Level().timeServer() - m_sounds[m_sound_index].dwTime < m_dwInertion) && (m_sounds[m_sound_index].dwNodeID < ai().level_graph().header().vertex_count())) {
////		if ((!m_sounds[m_sound_index].tpEntity || m_sounds[m_sound_index].tpEntity->g_Alive()) && (Level().timeServer() - m_sounds[m_sound_index].dwTime < m_dwInertion)) {
//#ifndef SILENCE
//			Msg("Inertion sound from %s",m_sounds[m_sound_index].tpEntity ? m_sounds[m_sound_index].tpEntity->cName() : "world");
//#endif
//			A = _A;
//			B = _B;
//		}
//		else
//			m_sound_index = -1;
//	J					= A || B;
//	
//	// victory probability
//	C = D = E = F = G	= false;
//	objVisible			&VisibleEnemies = Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
//	if (bfIsAnyAlive(VisibleEnemies)) {
// 		switch (dwfChooseAction(0,m_fAttackSuccessProbability0,m_fAttackSuccessProbability1,m_fAttackSuccessProbability2,m_fAttackSuccessProbability3,g_Team(),g_Squad(),g_Group(),0,1,2,3,4,this,30.f)) {
//			case 4 : 
//				C = true;
//				break;
//			case 3 : 
//				D = true;
//				break;
//			case 2 : 
//				E = true;
//				break;
//			case 1 : 
//				F = true;
//				break;
//			case 0 : 
//				G = true;
//				break;
//		}
//	}
//	K					= C | D | E | F | G;
//	
//	// does enemy see me?
//	SelectEnemy			(m_tEnemy);
//	I					= false;
//	for (int i=0, n=VisibleEnemies.size(); i<n; ++i) {
//		float			yaw1, pitch1, yaw2, pitch2, fYawFov, fPitchFov, fRange;
//		Fvector			tPosition = VisibleEnemies[i].key->Position();
//
//		CCustomMonster	*tpCustomMonster = dynamic_cast<CCustomMonster *>(VisibleEnemies[i].key);
//		if (tpCustomMonster) {
//			yaw1		= -tpCustomMonster->m_head.current.yaw;
//			pitch1		= -tpCustomMonster->m_head.current.pitch;
//			fYawFov		= angle_normalize_signed(tpCustomMonster->ffGetFov()*PI/180.f);
//			fRange		= tpCustomMonster->ffGetRange();
//		}
//		else {
//			CActor		*tpActor = dynamic_cast<CActor *>(VisibleEnemies[i].key);
//			if (tpActor) {
//				yaw1	= tpActor->Orientation().yaw;
//				pitch1	= tpActor->Orientation().pitch;
//				fYawFov	= angle_normalize_signed(tpActor->ffGetFov()*PI/180.f);
//				fRange	= tpActor->ffGetRange();
//			}
//			else
//				continue;
//		}
//		
//		if (tPosition.distance_to(Position()) > fRange)
//			continue;
//
//		fYawFov			= angle_normalize_signed((_abs(fYawFov) + _abs(atanf(1.f/tPosition.distance_to(Position()))))/2.f);
//		fPitchFov		= angle_normalize_signed(fYawFov*1.f);
//		tPosition.sub	(Position());
//		tPosition.mul	(-1);
//		tPosition.getHP	(yaw2,pitch2);
//		yaw1			= angle_normalize_signed(yaw1);
//		pitch1			= angle_normalize_signed(pitch1);
//		yaw2			= angle_normalize_signed(yaw2);
//		pitch2			= angle_normalize_signed(pitch2);
//		if (0 != (I = ((angle_difference(yaw1,yaw2) <= fYawFov) && (angle_difference(pitch1,pitch2) <= fPitchFov))))
//			break;
//	}
//	
//	// is enemy expedient?
//	H = false;
//	ai().ef_storage().m_tpCurrentMember = this;
//	for ( i=0, n=VisibleEnemies.size(); i<n; ++i) {
//		if (0 == (ai().ef_storage().m_tpCurrentEnemy  = dynamic_cast<CEntityAlive*>(VisibleEnemies[i].key)))
//			continue;
//		if ((E || F || G) && (true == (H = !!ai().ef_storage().m_pfExpediency->dwfGetDiscreteValue())))
//			break;
//		else
//			if (-1 != ifFindHurtIndex(ai().ef_storage().m_tpCurrentEnemy))
//				H = true;
//	}
//	H = true;
//	
//	// is there any items to pick up?
//	L = false;
//	if (7 != m_dwParticularState)
//		vfCheckForItems();
//	
//	M = !!m_tpItemsToTake.size();
}

bool CAI_Stalker::bfIfHuman(const CEntity *tpEntity)
{
	if (!tpEntity)
		tpEntity = enemy();
	if (!tpEntity)
		return(false);
	switch (tpEntity->SUB_CLS_ID) {
		case CLSID_OBJECT_ACTOR :
		case CLSID_AI_SCIENTIST :
		case CLSID_AI_SOLDIER :
		case CLSID_AI_STALKER_MILITARY :
		case CLSID_AI_STALKER : 
			return(true);
		default : return(false);
	}
}
