////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\..\\actor.h"
#include "..\\rat\\ai_rat.h"

using namespace AI_Biting;

/**
	Msg("Path state : %s",(m_tPathState == ePathStateSearchNode) ? "Searching for the node" : (m_tPathState == ePathStateBuildNodePath) ? "Building path" : (m_tPathState == ePathStateBuildTravelLine) ? "Building travel line" : "Dodging travel line");\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
/**/

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

	if (m_bStateChanged) {
		m_tActionState = eActionStateWatchGo;
		m_dwActionStartTime = Level().timeServer() + ::Random.randI(2000,5000);
	}

	m_dwInertion				= 60000;

	if ((AI_Path.fSpeed < EPS_L) && !m_tEnemy.Enemy) { // ������������� 
		switch (m_tActionState) {
			case eActionStateWatch : {
				vfSetMotionActionParams	(eBodyStateStand,eMovementTypeRun,eMovementDirectionForward,eStateTypeDanger,eActionTypeRun);

				vfSetParameters			(0,0,false,0);

				if (r_torso_current.yaw == r_torso_target.yaw) {
					m_ls_yaw = r_torso_current.yaw;
					m_tActionState = eActionStateDontWatch;
				}
				break;
									 }
			case eActionStateDontWatch : {
//				Fvector					tPoint;
//				tPoint.setHP			(m_ls_yaw,0);
//				tPoint.mul				(100.f);
//				tPoint.add				(vPosition);
//				vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeLookOver,tPoint,2500);
				break;
										 }
			default : {
				m_tActionState = eActionStateWatch;
				break;
					  }
		}
		return;
	}

	m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tSavedEnemyPosition.distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
	m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(vPosition) + 3.f;

	vfSetMotionActionParams		(eBodyStateStand,eMovementTypeRun,eMovementDirectionForward,eStateTypeDanger,eActionTypeRun);
	vfSetParameters				(&m_tSelectorFreeHunting,0,true,0);
}

void CAI_Biting::BackCover(bool bFire)
{
	WRITE_TO_LOG("Back Cover");
	
	if (m_bStateChanged) {
		m_tActionState			= eActionStateWatchGo;
		m_dwActionStartTime		= m_dwCurrentUpdate;
	}

	m_dwInertion				= 60000;
	Fvector						EnemyPosition = m_tSavedEnemyPosition;
	m_tSelectorCover.m_fMinEnemyDistance = EnemyPosition.distance_to(vPosition) + 3.f;


	switch (m_tActionState) {
		case eActionStateWatchGo : 
			WRITE_TO_LOG			("WatchGo : BackCover");
			if (m_dwCurrentUpdate - m_dwActionStartTime < 8500) {
				vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, eMovementDirectionForward, 
										eStateTypeDanger,eActionTypeRun);
				vfSetParameters			(&m_tSelectorCover,0,true,0);
			}
			else {
				vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, eMovementDirectionForward, 
										eStateTypeDanger,eActionTypeWalk);
				vfSetParameters			(0,0,true,0);
			}


			if ((m_dwCurrentUpdate - m_dwActionStartTime > 10000) && 
				((getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,EnemyPosition) != u32(-1)) || 
				 (m_dwCurrentUpdate - m_dwActionStartTime > 8000))) {
				
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = m_dwCurrentUpdate;
			}
			break;
		case eActionStateWatchLook : 
			break;
		default : 
			m_tActionState			= eActionStateWatchGo;
			break;
				  
	}
}

void CAI_Biting::ForwardStraight()
{
	WRITE_TO_LOG("Forward Straight");
	
// -----------------------------------------------------------------------------
// Choose branch
	m_dwInertion				= 6000;
	
	Fvector						EnemyPosition;
	EActionState				OldState;

	bool						bAttackRat;

	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(m_tSavedEnemy);
	if (tpRat) 	bAttackRat = true;
	else bAttackRat = false;

	EnemyPosition = ((m_tEnemy.Enemy) ? m_tEnemy.Enemy->Position() : m_tSavedEnemy->Position());
	OldState = m_tActionState;
	
	float tDist1 = 2.4f;
	float tDist2 = 3.8f;

	if (bAttackRat) {
		tDist1 = 0.7f;
		tDist2 = 2.7f;
	}

	m_tActionState = ((EnemyPosition.distance_to(vPosition) > tDist1) ? eActionStateRun : eActionStateStand);	
	
	if ((OldState == eActionStateStand && m_tActionState == eActionStateRun)&& 
		(EnemyPosition.distance_to(vPosition) < tDist2)) m_tActionState = OldState;


// -----------------------------------------------------------------------------
// Process branch
	switch (m_tActionState) {

		case eActionStateRun:			// ������ � �����
			vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
									eMovementDirectionForward, eStateTypeDanger, eActionTypeRun);
			vfSetParameters			(0,&EnemyPosition,false,0);
			break;
		case eActionStateStand:			// ����������
			
			if (bAttackRat) {
					vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
											eMovementDirectionNone, eStateTypeNormal, eActionTypeAttack);
					vfSetParameters			(0,&EnemyPosition,false,0);
			}
			else {
					vfSetMotionActionParams(eBodyStateStand, eMovementTypeStand, 
											eMovementDirectionNone, eStateTypeDanger, eActionTypeAttack);
					vfSetParameters			(0,&EnemyPosition,false,&EnemyPosition);
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
	
	Fvector		EnemyPosition;
	EnemyPosition = ((m_tEnemy.Enemy) ? m_tEnemy.Enemy->Position() : m_tSavedEnemyPosition);
	
	m_tSelectorCover.m_fMaxEnemyDistance = EnemyPosition.distance_to(vPosition) + m_tSelectorCover.m_fSearchRange;
	m_tSelectorCover.m_fOptEnemyDistance = m_tSelectorCover.m_fMaxEnemyDistance;
	m_tSelectorCover.m_fMinEnemyDistance = EnemyPosition.distance_to(vPosition) + 3.f;

	vfSetMotionActionParams(eBodyStateStand, eMovementTypeWalk, 
							eMovementDirectionForward, eStateTypeNormal, eActionTypeWalk);

	vfSetParameters	(&m_tSelectorCover,0,true,0);
}

void CAI_Biting::Detour()
{
	WRITE_TO_LOG("Detour");

	m_dwInertion				= 60000;
	if (m_bStateChanged) {
		m_tActionState = eActionStateWatchLook;
		m_dwActionStartTime = m_dwCurrentUpdate;
		AI_Path.TravelPath.clear();
	}

	Fvector		EnemyPosition;
	EnemyPosition = ((m_tEnemy.Enemy) ? m_tEnemy.Enemy->Position() : m_tSavedEnemyPosition);

	m_tSelectorFreeHunting.m_fMaxEnemyDistance = EnemyPosition.distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
	m_tSelectorFreeHunting.m_fOptEnemyDistance = 15;
	m_tSelectorFreeHunting.m_fMinEnemyDistance = EnemyPosition.distance_to(vPosition) + 3.f;

	u32 dwDelay1, dwDelay2;
	float f = 0;
	if (C)	f =100.f;
	if (D)	f =80.f;
	if (E)	f =60.f;
	if (F)	f =40.f;
	if (G)	f =20.f;

	float fDistance = _min(vPosition.distance_to(EnemyPosition),f);
	dwDelay1 = iFloor((fDistance / f) * 20000);
	dwDelay2 = _max(u32(20000 - dwDelay1),u32(2000));
	
	switch (m_tActionState) {
		case eActionStateWatchGo : 
			WRITE_TO_LOG			("WatchGo : Detour");
			AccomplishTask			((F || G) ? 0 : &m_tSelectorFreeHunting);
			if ((m_dwCurrentUpdate - m_dwActionStartTime > dwDelay1) && (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,EnemyPosition) != u32(-1))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
								  
		case eActionStateWatchLook : 
			WRITE_TO_LOG			("WatchLook : Detour");
			
			vfUpdateDetourPoint();
			AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
			if (!AI_Path.DestNode) {
				Msg("! Invalid graph point node (graph index %d)",m_tNextGP);
				for (int i=0; i<(int)getAI().GraphHeader().dwVertexCount; i++)
					Msg("%3d : %6d",i,getAI().m_tpaGraph[i].tNodeID);
			}
			
			//vfSetParameters			((F || G) ? 0 : &m_tSelectorFreeHunting,0,false,eWeaponStateIdle,(F || G) ? ePathTypeStraight : ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,(F || G) ? eLookTypePoint : eLookTypeFirePoint,tPoint,false,true);
			if ((m_dwCurrentUpdate - m_dwActionStartTime > dwDelay2) || (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,EnemyPosition) == u32(-1))) {
				m_tActionState		= eActionStateWatchGo;
				m_dwActionStartTime = m_dwCurrentUpdate;
			}
			break;
	}
}


void CAI_Biting::ExploreDE()
{
	WRITE_TO_LOG("Explore danger-expedient enemy");
}

void CAI_Biting::ExploreDNE()
{
	WRITE_TO_LOG("Explore danger-non-expedient enemy");
}

void CAI_Biting::ExploreNDE()
{
	WRITE_TO_LOG("Explore non-danger-expedient enemy");

	if (m_bStateChanged) m_dwInertion = 6000;

	vfSetMotionActionParams	(eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward,eStateTypeDanger,eActionTypeWalk);
	

	AI_Path.DestNode		= m_dwLastSoundNodeID;
	vfSetParameters			(0,0,false,0);

}

void CAI_Biting::ExploreNDNE()
{
	WRITE_TO_LOG("Explore non-danger-non-expedient enemy");
}
// Walking Around
void CAI_Biting::AccomplishTask(IBaseAI_NodeEvaluator *tpNodeEvaluator)
{
	WRITE_TO_LOG("Accomplishing task");

// -----------------------------------------------------------------------------
// Choose branch

	bool	bCorpseFound = false;
	float   turn_side;

	// �������� �� ��������� ������
	SelectCorp(m_tEnemy);
	
	Fvector *tpDesiredPosition = 0;

	if (m_tEnemy.Enemy) m_tCorpse = m_tEnemy;
	
	if (m_tCorpse.Enemy) 
		if (m_tCorpse.Enemy->m_fFood >= 0) bCorpseFound = true;

	
	if (bCorpseFound) {
		AI_Path.DestNode		= m_tCorpse.Enemy->AI_NodeID;
		Fvector l_tCorpsePosition;
		m_tCorpse.Enemy->clCenter(l_tCorpsePosition);
		tpDesiredPosition = &l_tCorpsePosition;
		
		m_tActionState = eActionStateRun;			
		m_dwActionStartTime = 0;

	} else {
		
		if (m_bStateChanged || (m_dwActionStartTime < m_dwCurrentUpdate)) {
		
			m_bStateChanged = false;
			// �������� �������
			m_tActionState	= eActionStateStand;
			
			AI_Path.TravelPath.clear();
			AI_Path.DestNode = AI_NodeID;

			vfSetParameters(0, 0, false, 0);


			u32		dwMinRand, dwMaxRand;
			dwMinRand = dwMaxRand = 1;

			switch (::Random.randI(8)) {
				
				case 0: m_tActionState = eActionStateStand;
						dwMinRand = 2000;
						dwMaxRand = 3000;
						
						break;
				case 1:
				case 2:
				case 3: m_tActionState = eActionStateWatchGo; // ������� �� ������ �����?
						vfUpdateDetourPoint();	

						AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
						vfSetParameters(0, 0, false, 0);
						dwMinRand = 7000;
						dwMaxRand = 15000;
						break;
				case 4: 
				case 5:
				case 6:
						m_tActionState = eActionStateStand;
						
						turn_side = ((::Random.randI(2)) ? 1.f : (-1.f));
						
						r_torso_target.yaw += (PI_DIV_2) * turn_side;
						r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
						dwMinRand = 2000;
						dwMaxRand = 3000;
						break;
				case 7: m_tActionState = eActionStateLie;
						if (!_CA.Active()) _CA.Set(ePostureStand,eActionLieDown);
						dwMinRand = 7000;
						dwMaxRand = 15000;
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
			if (m_tCorpse.Enemy->Position().distance_to(vPosition) > 1.5f) {
				vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
										eMovementDirectionForward, eStateTypeNormal, eActionTypeRun);
			} else  { // ����
				vfSetMotionActionParams(eBodyStateLie, eMovementTypeStand, 
										eMovementDirectionNone, eStateTypeNormal, eActionTypeEat);
				
				if (m_dwLastTimeEat + m_dwEatInterval < m_dwCurrentUpdate) {
					m_tCorpse.Enemy->m_fFood -= m_fHitPower/5.f;
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
	vfSetParameters(0, tpDesiredPosition, false, 0);	

	// Play sound
/*	
	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
		if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
			m_tpSoundBeingPlayed = 0;
			m_dwLastVoiceTalk = m_dwCurrentUpdate;
		}
		if ((m_dwCurrentUpdate - m_dwLastSoundRefresh > m_fVoiceRefreshRate) && 
			((m_dwCurrentUpdate - m_dwLastVoiceTalk > m_fMaxVoiceIinterval) || 
			((m_dwCurrentUpdate - m_dwLastVoiceTalk > m_fMinVoiceIinterval) && 
			(::Random.randF(0,1) > (m_dwCurrentUpdate - m_dwLastVoiceTalk - m_fMinVoiceIinterval)/(m_fMaxVoiceIinterval - m_fMinVoiceIinterval))))) {
			
			m_dwLastSoundRefresh = m_dwCurrentUpdate;

			// Play voice-sound
			m_tpSoundBeingPlayed = &(m_tpaSoundVoice[Random.randI(SND_VOICE_COUNT)]);

			if (!m_tpSoundBeingPlayed->feedback)
				::Sound->play_at_pos(*m_tpSoundBeingPlayed,this,eye_matrix.c);
			else
				m_tpSoundBeingPlayed->feedback->set_position(eye_matrix.c);
		}
	}
*/
}


void CAI_Biting::Think()
{

	if (!g_Alive()) return;

	m_dwLastUpdateTime		= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();

	m_tEnemy.Enemy			= 0;
	vfUpdateParameters		();

	if (!K && _K && m_tSavedEnemy && m_tSavedEnemy->g_Alive() && ((m_dwCurrentUpdate - m_dwLostEnemyTime) < m_dwInertion)) {
		K = true;
		C = _C;
		E = _E;
		D = _D;
		F = _F;
		H = _H;
		I = _I;
		m_bStateChanged = false;
		//vfUpdateVisibilityBySensitivity();
	}

	m_bStateChanged			= ((_A	!= A) || (_B	!= B) || (_C	!= C) || (_D	!= D) || (_E	!= E) || (_F	!= F) || (_H	!= H) || (_I	!= I) || (_J	!= J) || (_K	!= K));

#ifndef SILENCE
	if (g_Alive())
		Msg("%s : [S=%d][A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][H=%d][I=%d][J=%d][K=%d]",cName(),m_bStateChanged ? 1 : 0,A,B,C,D,E,F,H,I,J,K);
#endif
	

// A - � ����� ������� ����
// B - � ����� ��������� ����
// � - � ���� ����� �������� �����
// D - � ���� �������� �����
// E - � ���� ������� �����
// F - � ���� ������� �����
// H - ���� ��������
// I - ���� ����� ����
// J - A | B
// K - C | D | E | F 

	if (!g_Alive()) {
		Death				();
	}				   
	else
	if (C && H && I) {
		m_dwRandomFactor	= 50;
		Panic			();
	} else
	if (C && H && !I) {
		Hide();
	} else
	if (C && !H && I) {
		m_dwRandomFactor	= 50;
		Panic			();
	} else
	if (C && !H && !I) {
		Hide();
	} else
	
	if (D && H && I) {
		m_dwRandomFactor	= 50;
		Panic	();
	} else
	if (D && H && !I) {
		Hide		();
	} else
	if (D && !H && I) {
		m_dwRandomFactor	= 50;
		Panic	();
	} else
	if (D && !H && !I) {
		Hide	();
	} else
	
	if (E && H && I) {
		m_dwRandomFactor	= 0;
		ForwardStraight	();
	} else
	if (E && H && !I) {
		m_dwRandomFactor	= 0;
		ForwardStraight	();
	} else
	if (E && !H && I) {
		Detour			();
	} else
	if (E && !H && !I) {
		Detour			();
	} else
	
	if (F && H && I) {
		m_dwRandomFactor	= 75;
		ForwardStraight	();
	} else
	if (F && H && !I) {
		m_dwRandomFactor	= 100;
		ForwardStraight	();
	} else
	if (F && !H && I) {
		Detour			();
	} else
	if (F && !H && !I) {
		Detour			();
	} else
	
	if (A && !K && !H) {	// ����� ������� ����, �� �� ����, ���� �� ��������
		ExploreDNE();
	} else
	if (A && !K && H) {		// ����� ������� ����, �� �� ����, ���� ��������
		ExploreDE();
	} else
	
	if (B && !K && !H) {
		ExploreNDNE();
	} else
	if (B && !K && H) {
		ExploreNDE();
	} else {
		AccomplishTask();
	}
	
	_A	= A;
	_B	= B;
	_C	= C;
	_D	= D;
	_E	= E;
	_F	= F;
	_H	= H;
	_I	= I;
	_J	= J;
	_K	= K;

	
	vfSetAnimation(true);
}