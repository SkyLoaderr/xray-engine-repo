////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fsm.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : FSM for biting class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\rat\\ai_rat.h"
#include "..\\..\\actor.h"

using namespace AI_Biting;
#define SILENCE


#undef	WRITE_TO_LOG
#ifdef SILENCE
#define WRITE_TO_LOG(s) ;

#else
#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* State : %s",cName(),s);\
}
#endif

void CAI_Biting::Death()
{
	WRITE_TO_LOG("Death");
}

void CAI_Biting::Panic()
{
	WRITE_TO_LOG("Panic");

//	if (m_bStateChanged) {
//		m_tActionState = eActionStateWatchGo;
//		m_dwActionStartTime = Level().timeServer() + ::Random.randI(2000,5000);
//	}
//
//	m_dwInertion				= 60000;
//
//	if ((AI_Path.fSpeed < EPS_L) && !m_tEnemy.Enemy) { // ������������� 
//		switch (m_tActionState) {
//			case eActionStateWatch : 
//				vfSetMotionActionParams	(eBodyStateStand,eMovementTypeRun,eMovementDirectionForward,eStateTypeDanger,eActionTypeRun);
//
//				vfSetParameters			(ePathTypeStraight,0,0,false,0);
//
//				if (r_torso_current.yaw == r_torso_target.yaw) {
//					m_ls_yaw = r_torso_current.yaw;
//					m_tActionState = eActionStateDontWatch;
//				}
//				break;
//									 
//			case eActionStateDontWatch : 
//				break;
//			default : 
//				m_tActionState = eActionStateWatch;
//				break;
//		}
//		return;
//	}
/*	Temp comment: Saved enemy...
 
	m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tSavedEnemyPosition.distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
	m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + 3.f;

	vfSetMotionActionParams		(eBodyStateStand,eMovementTypeRun,eMovementDirectionForward,eStateTypeDanger,eActionTypeRun);
	vfSetParameters				(ePathTypeStraight,&m_tSelectorFreeHunting,0,true,0);
}

void CAI_Biting::Scared()
{
	WRITE_TO_LOG("Scared");

//	if (m_bStateChanged) {
//		m_tActionState = eActionStateWatch;
//		m_dwActionStartTime = m_dwCurrentUpdate + 1500;
//
//		Fvector ptr = vPosition;
//		/*	Temp comment: Saved enemy...
//			ptr.sub(m_tSavedEnemy->Position());
//		*/
//		ptr.normalize();
//		ptr.mul(20.f);
//		ptr.add(vPosition);
//		m_EnemyPos = ptr;
//	}
//
//	if (m_dwActionStartTime < m_dwCurrentUpdate) {
//		m_tActionState = eActionStateDontWatch;
//	}
//
//	m_dwInertion				= 60000;
//
//	switch (m_tActionState) {
//		case eActionStateWatch :		// ��������
//			// ��������� ���� � �������� �� ����� �������
//			vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, eMovementDirectionBack, 
//				eStateTypeDanger,eActionTypeWalk);
//			
//			vfSetParameters	 (ePathTypeStraight,0,&m_EnemyPos,true,0,true);
//			break;
//		case eActionStateDontWatch :	// �������
	/*	Temp comment: Saved enemy...		
			m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tSavedEnemyPosition.distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
			m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMaxEnemyDistance;
			m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + 3.f;

			vfSetMotionActionParams		(eBodyStateStand,eMovementTypeRun,eMovementDirectionForward,eStateTypeDanger,eActionTypeRun);
			vfSetParameters				(ePathTypeStraight,&m_tSelectorFreeHunting,0,true,0);
	*/
//			break;
//	}
}

void CAI_Biting::BackCover(bool bFire)
{
	WRITE_TO_LOG("Back Cover");
	
//	if (m_bStateChanged) {
//		m_tActionState			= eActionStateWatchGo;
//		m_dwActionStartTime		= m_dwCurrentUpdate;
//	}
//
//	m_dwInertion				= 60000;
//	
//	Fvector						EnemyPosition = m_tSavedEnemyPosition;
//	m_tSelectorCover.m_fMinEnemyDistance = EnemyPosition.distance_to(vPosition) + 3.f;
//
//	switch (m_tActionState) {
//		case eActionStateWatchGo : 
//			WRITE_TO_LOG			("WatchGo : BackCover");
//			if (m_dwCurrentUpdate - m_dwActionStartTime < 8500) {
//				vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, eMovementDirectionForward, 
//										eStateTypeDanger,eActionTypeRun);
//				vfSetParameters			(ePathTypeStraight,&m_tSelectorCover,0,true,0);
//			}
//			else {
//				vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, eMovementDirectionForward, 
//										eStateTypeDanger,eActionTypeWalk);
//				vfSetParameters			(ePathTypeStraight,0,0,true,0);
//			}
//
//
//			if ((m_dwCurrentUpdate - m_dwActionStartTime > 10000) && 
//				((getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,EnemyPosition) != u32(-1)) || 
//				 (m_dwCurrentUpdate - m_dwActionStartTime > 8000))) {
//				
//				m_tActionState		= eActionStateWatchLook;
//				m_dwActionStartTime = m_dwCurrentUpdate;
//			}
//			break;
//		case eActionStateWatchLook : 
//			break;
//		default : 
//			m_tActionState			= eActionStateWatchGo;
//			break;
//				  
//	}
}

void CAI_Biting::ForwardStraight()
{
	WRITE_TO_LOG("Forward Straight");
	
	VisionElem ve;

	if (!SelectEnemy(ve)) return;



// -----------------------------------------------------------------------------
// Choose branch
	m_dwInertion				= 6000;
	
	EActionState				OldState;
	bool						bAttackRat;

	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(ve.obj);
	if (tpRat) 	bAttackRat = true;
	else bAttackRat = false;

	OldState = m_tActionState;
	
	float tDist1 = 2.4f;
	float tDist2 = 3.8f;

	if (bAttackRat) {
		tDist1 = 0.7f;
		tDist2 = 2.8f;
	}

	m_tActionState = ((ve.obj->Position().distance_to(Position()) > tDist1) ? eActionStateRun : eActionStateStand);	
	
	if ((OldState == eActionStateStand && m_tActionState == eActionStateRun)&& 
		(ve.obj->Position().distance_to(Position()) < tDist2)) m_tActionState = OldState;


// -----------------------------------------------------------------------------
// Process branch
//	Msg("EnemyNode : %d [%d]",ve.node_id,AI_Path.DestNode);

	switch (m_tActionState) {

		case eActionStateRun:			// ������ � �����
			AI_Path.DestNode		= ve.node_id;
			vfSetMotionActionParams	(eBodyStateStand, eMovementTypeRun, 
									eMovementDirectionForward, eStateTypeDanger, eActionTypeRun);
			vfSetParameters			(ePathTypeStraight,0,&ve.obj->Position(),false,0);
			break;
		case eActionStateStand:			// ����������
			
			if (bAttackRat) {
					
					vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
											eMovementDirectionNone, eStateTypeNormal, eActionTypeAttack);
					vfSetParameters			(ePathTypeStraight,0,&ve.obj->Position(),false,0);

					if (AttackMelee(ve.obj,true)) {
						DoDamage(ve.obj);
					}

//					m_AttackInterval = 500;
//					if ((m_AttackLastTime + m_AttackInterval) < m_dwCurrentUpdate) {
//						if (ve.obj->Position().distance_to(m_AttackLastPosition) < 0.7f) {
//							DoDamage(ve.obj);
//						}
//						m_AttackLastTime = m_dwCurrentUpdate;
//						m_AttackLastPosition = ve.obj->Position();
//					}
			}
			else {
					vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
											eMovementDirectionNone, eStateTypeDanger, eActionTypeAttack);
					vfSetParameters			(ePathTypeStraight,0,&ve.obj->Position(),false,&ve.obj->Position());


					if (AttackMelee(ve.obj,false)) {
						DoDamage(ve.obj);
					}		

//					m_AttackInterval = 1000;
//					if ((m_AttackLastTime + m_AttackInterval) < m_dwCurrentUpdate) {
//						if (ve.obj->Position().distance_to(m_AttackLastPosition) < 0.7f) {
//							DoDamage(ve.obj);
//						}
//						m_AttackLastTime = m_dwCurrentUpdate;
//						m_AttackLastPosition = ve.obj->Position();
//					}
			}	

			if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
				if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
					m_tpSoundBeingPlayed = 0;
					m_dwLastVoiceTalk = m_dwCurrentUpdate;
				}
				if (m_dwCurrentUpdate - m_dwLastVoiceTalk > (u32)::Random.randI(500,2000)) {
					m_tpSoundBeingPlayed = &(m_tpaSoundAttack[::Random.randI(SND_ATTACK_COUNT)]);
					::Sound->play_at_pos(*m_tpSoundBeingPlayed,this,eye_matrix.c);
				}
			}
			
			
			break;
	}	
	
}


void CAI_Biting::Hide()
{
	WRITE_TO_LOG("Hide");

	m_dwInertion				= 60000;

/* Temp comment: save enemy
	Fvector		EnemyPosition;
	EnemyPosition = ((m_tEnemy.Enemy) ? m_tEnemy.Enemy->Position() : m_tSavedEnemyPosition);
	
	m_tSelectorCover.m_fMaxEnemyDistance = EnemyPosition.distance_to(Position()) + m_tSelectorCover.m_fSearchRange;
	m_tSelectorCover.m_fOptEnemyDistance = m_tSelectorCover.m_fMaxEnemyDistance;
	m_tSelectorCover.m_fMinEnemyDistance = EnemyPosition.distance_to(vPosition) + 3.f;

	vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
							eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);

	vfSetParameters	(ePathTypeStraight,&m_tSelectorCover,0,true,0);
	*/
}

void CAI_Biting::Detour()
{
	WRITE_TO_LOG("Detour");

//	m_dwInertion				= 60000;
//	if (m_bStateChanged) {
////		m_tActionState = eActionStateWatchLook;
//		m_dwActionStartTime = m_dwCurrentUpdate;
//		//AI_Path.TravelPath.clear();
//	}
//
///* Temp comment: save enemy
//	Fvector		EnemyPosition;
//	EnemyPosition = ((m_tEnemy.Enemy) ? m_tEnemy.Enemy->Position() : m_tSavedEnemyPosition);
//
//	m_tSelectorFreeHunting.m_fMaxEnemyDistance = EnemyPosition.distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
//	m_tSelectorFreeHunting.m_fOptEnemyDistance = 15;
//	m_tSelectorFreeHunting.m_fMinEnemyDistance = EnemyPosition.distance_to(vPosition) + 3.f;
//*/
//	vfUpdateDetourPoint();
//	AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
//	if (!AI_Path.DestNode) {
//		Msg("! Invalid graph point node (graph index %d)",m_tNextGP);
//		for (int i=0; i<(int)getAI().GraphHeader().dwVertexCount; i++)
//			Msg("%3d : %6d",i,getAI().m_tpaGraph[i].tNodeID);
//	}
//
//	vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);
//	vfSetParameters	 (ePathTypeCriteria,&m_tSelectorFreeHunting,0,false,0,false,true);
}


void CAI_Biting::ExploreDE()
{
	WRITE_TO_LOG("Explore danger-expedient enemy");

//	float		turn_side;
//
//	m_dwInertion				= 30000;
//
//// -----------------------------------------------------------------------------
//// Choose branch
//	if (m_bStateChanged) {
//		m_dwActionIndex	= 0;
//		m_dwActionStartTime = m_dwCurrentUpdate;
//	}
//
//	if (m_dwActionStartTime <= m_dwCurrentUpdate) {		
//		switch (m_dwActionIndex) {
//			case 0:					// turn once
//			case 1:					// turn twice
//				turn_side = ((::Random.randI(2)) ? 1.f : (-1.f));
//
//				r_torso_target.yaw += (PI_DIV_2) * turn_side;
//				r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
//				
//				m_dwActionStartTime = m_dwCurrentUpdate + 2000; 
//				break;
//			
//			case 2:
//				m_dwActionStartTime = m_dwCurrentUpdate + 20000; 
//				break;
//		}
//		m_dwActionIndex ++;
//	}
//
//
//// -----------------------------------------------------------------------------
//// Process branch
//	
//
//	switch (m_dwActionIndex) {
//		case 1:
//		case 2:
//			WRITE_TO_LOG("Explore DE :: Turning");
//			vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
//				eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);
//
//			vfSetParameters			(ePathTypeStraight,0,0,false,0);
//			break;
//		case 3:
//			WRITE_TO_LOG("Explore DE :: Hide");
//			Hide();
//			break;
//	}
//
}

void CAI_Biting::ExploreDNE()
{
	WRITE_TO_LOG("Explore danger-non-expedient enemy");
//
//	m_dwInertion				= 30000;
//	
//
//	if (m_bStateChanged) {
//		u32		time;
//		time =  m_dwCurrentUpdate;
//		_CAction.Add(ePostureStand, eActionAttackTurnLeft, (time += 900), angle_normalize(r_torso_target.yaw + 3*PI_DIV_4), PI_MUL_2);
//		_CAction.Add(ePostureStand, eActionScared, (time += 1400),r_torso_target.yaw);
//		_CAction.Switch();
//		if (m_tLastSound.tpEntity)
//			m_AttackLastPosition = m_tLastSound.tpEntity->Position();
//		else m_AttackLastPosition = vPosition;
//	}
//
//	vfSetMotionActionParams		(eBodyStateStand,eMovementTypeRun,eMovementDirectionForward,eStateTypeDanger,eActionTypeRun);
//
//	m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_AttackLastPosition.distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
//	m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMaxEnemyDistance;
//	m_tSelectorFreeHunting.m_fMinEnemyDistance = m_AttackLastPosition.distance_to(vPosition) + 10.f;
//	vfSetParameters				(ePathTypeStraight,&m_tSelectorFreeHunting,0,true,0);
}


void CAI_Biting::ExploreNDE()
{
	WRITE_TO_LOG("Explore non-danger-expedient enemy");

//	if (m_bStateChanged) m_dwInertion = 6000;
//
//	vfSetMotionActionParams	(eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward,eStateTypeDanger,eActionTypeWalk);
//	
//
//	AI_Path.DestNode		= m_dwLastSoundNodeID;
//	vfSetParameters			(ePathTypeStraight,0,0,false,0);

}

void CAI_Biting::ExploreNDNE()
{
	WRITE_TO_LOG("Explore non-danger-non-expedient enemy");
	ExploreNDE();
}
// Walking Around
void CAI_Biting::AccomplishTask(IBaseAI_NodeEvaluator *tpNodeEvaluator)
{
	WRITE_TO_LOG("Accomplishing task");

// -----------------------------------------------------------------------------
// Choose branch
	bool	bCorpseFound = false;
	
	// �������� �� ��������� ������
	VisionElem ve;
	SelectCorpse(ve);
	
	Fvector *tpDesiredPosition = 0;
	
//	if (SelectCorpse(ve)) 
//		if (ve.obj->m_fFood >= 0) bCorpseFound = true;

	if (m_bStateChanged) {
		_CAction.Init();
	}

	if (bCorpseFound) {
		AI_Path.DestNode		=ve.node_id;
		Fvector l_tCorpsePosition;
		ve.obj->Center(l_tCorpsePosition);
		tpDesiredPosition = &l_tCorpsePosition;
		
		m_tActionState = eActionStateRun;			
		m_dwActionStartTime = 0;

	} else {

		if (m_bStateChanged || (m_dwActionStartTime < m_dwCurrentUpdate)) {
		
			m_bStateChanged = false;
			// �������� �������
			m_tActionState	= eActionStateStand;
			
			//AI_Path.TravelPath.clear();
			AI_Path.DestNode = AI_NodeID;

			vfSetParameters(ePathTypeStraight,0, 0, false, 0);


			u32		dwMinRand, dwMaxRand;
			dwMinRand = dwMaxRand = 1;

			switch (::Random.randI(11)) {
				
				case 0: m_tActionState = eActionStateStand;
						dwMinRand = 2000;
						dwMaxRand = 3000;
						
						break;
				case 1: case 2: case 3: case 4:
				
				 m_tActionState = eActionStateWatchGo; // ������� �� ������ �����?
						vfUpdateDetourPoint();	

						AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
						vfSetParameters(ePathTypeStraight,0, 0, false, 0);
						dwMinRand = 7000;
						dwMaxRand = 15000;
						break;
				case 5: case 6: case 7: case 8: case 9:
						m_tActionState = eActionStateStand;
						
						if (!::Random.randI(2))	r_torso_target.yaw += PI_DIV_2;
						else r_torso_target.yaw -= PI_DIV_2;
							
						r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
						dwMinRand = 3000;
						dwMaxRand = 4000;
						break;
				case 10: m_tActionState = eActionStateLie;
						if (!_CA.Active()) _CA.Set(ePostureStand,eActionLieDown);
						dwMinRand = 15000;
						dwMaxRand = 30000;
						break;
			}

			m_dwActionStartTime = m_dwCurrentUpdate + ::Random.randI(dwMinRand,dwMaxRand); // ����� �������� ��� ������� �� �����
		}
	}

// -----------------------------------------------------------------------------
// Process branch

	
	switch (m_tActionState) {
		case eActionStateStand :	
					
						vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
												eMovementDirectionNone, eStateTypeNormal, eActionTypeStand);
					

					break;
		case eActionStateLie:	
					vfSetMotionActionParams(eBodyStateLie, eMovementTypeStand, 
											eMovementDirectionNone, eStateTypeNormal, eActionTypeLie);
				break;

		case eActionStateWatchGo:   // �������
					vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
											eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);

					if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
						if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
							m_tpSoundBeingPlayed = 0;
							m_dwLastVoiceTalk = m_dwCurrentUpdate;
						}

						if (m_dwCurrentUpdate - m_dwLastVoiceTalk > (u32)::Random.randI(5000,15000)) {
							m_tpSoundBeingPlayed = &(m_tpaSoundVoice[::Random.randI(SND_VOICE_COUNT)]);
							::Sound->play_at_pos(*m_tpSoundBeingPlayed,this,eye_matrix.c);
						}
					}

					break;
		case eActionStateRun:   // ������ � �����
			if (ve.obj->Position().distance_to(Position()) > 2.0f) {
				vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
										eMovementDirectionForward, eStateTypeNormal, eActionTypeRun);
			} else  { // ��� ����
				if (!_CAction.Active() && !_CAction.Finished) {
					u32 time = m_dwCurrentUpdate;
					_CAction.Add(ePostureStand,eActionCheckCorpse,( time += 2000));
					_CAction.Add(ePostureStand, eActionLieDown,(time += 2000));
					_CAction.Switch();
				} 
					
				vfSetMotionActionParams(eBodyStateLie, eMovementTypeStand, 
										eMovementDirectionNone, eStateTypeNormal, eActionTypeEat);
				
				if (m_dwLastTimeEat + m_dwEatInterval < m_dwCurrentUpdate) {
					ve.obj->m_fFood -= m_fHitPower/5.f;
					m_dwLastTimeEat = m_dwCurrentUpdate;
				}

				if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
					if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
						m_tpSoundBeingPlayed = 0;
						m_dwLastVoiceTalk = m_dwCurrentUpdate;
					}

					if (m_dwCurrentUpdate - m_dwLastVoiceTalk > (u32)::Random.randI(1000,5000)) {
						m_tpSoundBeingPlayed = &(m_tpaSoundHit[::Random.randI(SND_HIT_COUNT)]);
						::Sound->play_at_pos(*m_tpSoundBeingPlayed,this,eye_matrix.c);
					}
				}

			}
			break;
	}
	vfSetParameters(ePathTypeStraight,0, tpDesiredPosition, false, 0);	

	// Play ref_sound
}

void CAI_Biting::Think()
{
	if (!g_Alive()) return;

//---------------------------------------------------------------------------------
	
	// ���������� �������
	m_dwLastUpdateTime		= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();

//	// ���������� ������ �������
//	UpdateMemory();
//
//
//	//		if (Motion.m_tSeq.Started) 
//	//			CurrentState->FreezeState(); // ���������� �������
//
//	if (Motion.m_tSeq.Active())	{
//		Motion.m_tSeq.Cycle(m_dwCurrentUpdate);
//	}else {
//		//- FSM 1-level 
//		VisionElem VE;
//		if (SelectEnemy(VE)) {
//			SetState(stateAttack);
//		} else {	
//			SetState(stateRest);
//		}
//		//---
//	}
//	
//	CurrentState->Execute(true);
//	Motion.SetFrameParams(this);
//
//	ControlAnimation();
//---------------------------------------------------------------------------------	
	
	return;


	
//
//	
//
//	
//	
////	m_dwLastUpdateTime		= m_dwCurrentUpdate;
////	m_dwCurrentUpdate		= Level().timeServer();
////
////	EPostureAnim old_p = m_tPostureAnim;
////	EActionAnim old_a = m_tActionAnim;
////
////
//////	if (CurrentState->CheckCompletion()) {
//////		// ����� ��������� �� ���������
//////		CurrentState = stateRest;
//////	}
//////
//////	CurrentState->Execute(true);
////
////	
////	if (microAction->Active()) {
////		microAction->Execute(true);
////	} else {
////		CurrentState->Execute();
////		CheckForMicroAction();
////	}
////	
////
////	if (old_p != m_tPostureAnim || old_a != m_tActionAnim) {
////		FORCE_ANIMATION_SELECT();
////	}
////	return;
//
//
//	m_dwLastUpdateTime		= m_dwCurrentUpdate;
//	m_dwCurrentUpdate		= Level().timeServer();
//
//	vfUpdateParameters		();
//
//	VisionElem ve;
//#pragma todo("MONSTER MEMORY REFACTORING...")
//	//Mem.SelectEnemy(ve);
//
//	if (!K && _K && ve.obj) {
//		K = true;
//		C = _C;
//		E = _E;
//		D = _D;
//		F = _F;
//		H = _H;
//		I = _I;
//		m_bStateChanged = false;
//	}
//
//	m_bStateChanged			= ((_A	!= A) || (_B	!= B) || (_C	!= C) || (_D	!= D) || (_E	!= E) || (_F	!= F) || (_H	!= H) || (_I	!= I) || (_J	!= J) || (_K	!= K));
//
//#ifndef SILENCE
//	if (g_Alive())
//		Msg("%s : [S=%d][A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][H=%d][I=%d][J=%d][K=%d]",cName(),m_bStateChanged ? 1 : 0,A,B,C,D,E,F,H,I,J,K);
//#endif
//	
//
//// A - � ����� ������� ����
//// B - � ����� ��������� ����
//// � - � ���� ����� �������� �����
//// D - � ���� �������� �����
//// E - � ���� ������� �����
//// F - � ���� ������� �����
//// H - ���� ��������
//// I - ���� ����� ����
//// J - A | B
//// K - C | D | E | F 
//
//	if (!g_Alive()) {
//		Death				();
//	} else 				   
//
//	if (C && H && I) {
//		Scared	();
//	} else
//	if (C && H && !I) {
//		Scared	();
//	} else
//	if (C && !H && I) {
//		Panic			();
//	} else
//	if (C && !H && !I) {
//		Panic();
//	} else
//	
//	if (D && H && I) {
//		Panic	();
//	} else
//	if (D && H && !I) {
//		ForwardStraight	(); //���� ����������� � ������ ������
//	} else
//	if (D && !H && I) {
//		Panic	();
//	} else
//	if (D && !H && !I) {
//		Hide	();			// ����� ����������� ����� �������
//	} else
//	if (E && H && I) {
//		ForwardStraight	();
//	} else
//	if (E && H && !I) {
//		ForwardStraight	(); //���� ����������� � ������ ������
//	} else
//	if (E && !H && I) {
//		Detour			();
//	} else
//	if (E && !H && !I) {
//		Detour			();
//	} else
//	
//	if (F && H && I) {
//		ForwardStraight	();
//	} else
//	if (F && H && !I) {
//		ForwardStraight	();
//	} else
//	if (F && !H && I) {
//		Detour			();
//	} else
//	if (F && !H && !I) {
//		Detour			();
//	} else
//	
//
//	if (A && !K && !H) {	// ����� ������� ����, �� �� ����, ���� �� ��������
//		ExploreDNE();
//	} else
//	if (A && !K && H) {		// ����� ������� ����, �� �� ����, ���� ��������
//		ExploreDE();
//	} else
//	
//	if (B && !K && !H) {	// ����� �� ������� ����, �� �� ����, ���� �� ��������
//		ExploreNDNE();
//	} else
//	if (B && !K && H) {		// ����� �� ������� ����, �� �� ����, ���� ��������
//		ExploreNDE();
//	} else {
//		AccomplishTask();   // ������
//	}
//	
//	_A	= A;
//	_B	= B;
//	_C	= C;
//	_D	= D;
//	_E	= E;
//	_F	= F;
//	_H	= H;
//	_I	= I;
//	_J	= J;
//	_K	= K;
//
//	
//	vfSetAnimation(true);
}