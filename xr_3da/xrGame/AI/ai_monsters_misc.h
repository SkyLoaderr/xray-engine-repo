////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious routines for monsters
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_MONSTERS_MISC__
#define __XRAY_AI_MONSTERS_MISC__

#include "..\\ai_PathNodes.h"

	// Fuzzy State Machine
	#define GO_TO_NEW_STATE(a) {\
		eCurrentState = a;\
		m_dwLastRangeSearch = 0;\
		return;\
	}

	#define SWITCH_TO_NEW_STATE(a) {\
		tStateStack.push(eCurrentState);\
		eCurrentState = a;\
		m_dwLastRangeSearch = 0;\
		return;\
	}

	#define SWITCH_TO_PREV_STATE {\
		eCurrentState = tStateStack.top();\
		tStateStack.pop();\
		m_dwLastRangeSearch = 0;\
		return;\
	}

	#define CHECK_IF_SWITCH_TO_NEW_STATE(a,b)\
		if (a)\
			SWITCH_TO_NEW_STATE(b)

	#define CHECK_IF_SWITCH_TO_PREV_STATE(a)\
		if (a)\
			SWITCH_TO_PREV_STATE

	#define CHECK_IF_GO_TO_NEW_STATE(a,b)\
		if (a)\
			GO_TO_NEW_STATE(b)

	#define CHECK_FOR_DEATH \
		CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
		/**
		if (g_Health() <= 0) {\
			eCurrentState = aiSoldierDie;\
			return;\
		}
		/**/

	// Bones
	#define ASSIGN_SPINE_BONE {\
		if (fabsf(r_torso_target.yaw - r_target.yaw) < MIN_SPINE_TURN_ANGLE) {\
			r_spine_target.yaw = r_target.yaw;\
			r_target.yaw -= PI_DIV_6;\
		}\
		else\
			r_target.yaw = r_spine_target.yaw =  (r_torso_target.yaw + r_target.yaw)/2;\
	}
			
	#define LOOK_AT_DIRECTION(A) {\
		mk_rotation(A,r_torso_target);\
		r_target.yaw = r_torso_target.yaw;\
		ASSIGN_SPINE_BONE}

	#define TEST_MICRO_ACTION(A) \
		if (Level().iGetKeyState(DIK_##A)) {\
			if (!m_bMicroAction##A) {\
				m_bMicroAction##A = true;\
				tStateStack.push(eCurrentState);\
				eCurrentState = aiSoldierTestMicroAction##A;\
				return;\
			}\
		}\
		else\
			m_bMicroAction##A = false;
			
	#define CASE_MICRO_ACTION(A) \
		case aiSoldierTestMicroAction##A : {\
			TestMicroAction##A();\
			break;\
		}
			
	#define DECLARE_MICRO_ACTION(A)\
		void TestMicroAction##A();\
		bool m_bMicroAction##A;
	
	#define TEST_MACRO_ACTION(A) \
		if (Level().iGetKeyState(DIK_##A)) {\
			if (!m_bMacroAction##A) {\
				m_bMacroAction##A = true;\
				tStateStack.push(eCurrentState);\
				eCurrentState = aiSoldierTestMacroAction##A;\
				return;\
			}\
		}\
		else\
			m_bMacroAction##A = false;
			
	#define CASE_MACRO_ACTION(A) \
		case aiSoldierTestMacroAction##A : {\
			TestMacroAction##A();\
			break;\
		}
			
	#define DECLARE_MACRO_ACTION(A)\
		void TestMacroAction##A();\
		bool m_bMacroAction##A;
	
	#define INIT_SQUAD_AND_LEADER \
		CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];\
		CEntity* Leader = Squad.Leader;\
		if (Leader->g_Health() <= 0)\
			Leader = this;\
		R_ASSERT (Leader);
	
	//#define WRITE_LOG

	#ifdef WRITE_LOG
		#define WRITE_TO_LOG(S) {\
			Msg("%s,%s,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",cName(),S,Level().timeServer(),vPosition.x,vPosition.y,vPosition.z,r_current.yaw,r_current.pitch,r_torso_current.yaw,r_torso_current.pitch);\
			bStopThinking = true;\
		}
	#else
		#define WRITE_TO_LOG(S) \
			bStopThinking = true;
	#endif

	#define ADJUST_ANGLE(A) \
		if (A >= PI_MUL_2 - EPS_L)\
			A -= PI_MUL_2;\
		else\
			if (A <= -EPS_L)\
				A += PI_MUL_2;
			
	#define ADJUST_BONE(A) \
		ADJUST_ANGLE(A.yaw);\
		ADJUST_ANGLE(A.pitch);
			
	#define ADJUST_BONE_ANGLES \
		ADJUST_BONE(r_target);\
		ADJUST_BONE(r_current);\
		ADJUST_BONE(r_torso_target);\
		ADJUST_BONE(r_torso_current);\
		ADJUST_BONE(r_spine_target);\
		ADJUST_BONE(r_spine_current);
			
	#define SUB_ANGLE(A,B)\
		A -= B;\
		if (A <= -EPS_L )\
			A += PI_MUL_2;
			
	#define ADD_ANGLE(A,B)\
		A += B;\
		if (A >= PI_MUL_2 - EPS_L)\
			A -= PI_MUL_2;
	
	#define CHECK_FOR_STATE_TRANSITIONS(S) \
		WRITE_TO_LOG(S);\
		\
		if (g_Health() <= 0) {\
			eCurrentState = aiSoldierDie;\
			return;\
		}\
		\
		SelectEnemy(Enemy);\
		\
		if (Enemy.Enemy) {\
			tStateStack.push(eCurrentState);\
			eCurrentState = aiSoldierAttackFire;\
			m_dwLastRangeSearch = 0;\
			return;\
		}\
		\
		DWORD dwCurTime = Level().timeServer();\
		\
		if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {\
			tStateStack.push(eCurrentState);\
			eCurrentState = aiSoldierPatrolHurt;\
			m_dwLastRangeSearch = 0;\
			return;\
		}\
		\
		if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {\
			tStateStack.push(eCurrentState);\
			eCurrentState = aiSoldierSenseSomething;\
			m_dwLastRangeSearch = 0;\
			return;\
		}\
		\
		INIT_SQUAD_AND_LEADER;\
		\
		CGroup &Group = Squad.Groups[g_Group()];\
		\
		if ((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime)) {\
			tStateStack.push(eCurrentState);\
			eCurrentState = aiSoldierPatrolUnderFire;\
			m_dwLastRangeSearch = 0;\
			return;\
		}

	/************************************************************************/
	/* 		
	tHitDir = Group.m_tLastHitDirection;\
	dwHitTime = Group.m_dwLastHitTime;\
	tHitPosition = Group.m_tHitPosition;\
	*/
	/************************************************************************/

	#define SET_LOOK_FIRE_MOVEMENT(a,b,c)\
		if (!(Group.m_bLessCoverLook)) {\
			Group.m_bLessCoverLook = m_bLessCoverLook = true;\
			Group.m_dwLastViewChange = dwCurTime;\
		}\
		else\
			if ((m_bLessCoverLook) && (dwCurTime - Group.m_dwLastViewChange > 5000))\
				Group.m_bLessCoverLook = m_bLessCoverLook = false;\
		if (m_bLessCoverLook)\
			SetLessCoverLook(AI_Node);\
		else\
			SetDirectionLook();\
		\
		vfSetFire(a,Group);\
		\
		vfSetMovementType(b,c);

void vfCreateFastRealisticPath(vector<Fvector> &tpaPoints, DWORD dwStartNode, vector<Fvector> &tpaDeviations, vector<CTravelNode> &tpaPath, bool bUseDeviations = true, float fRoundedDistanceMin = 1.5f, float fRoundedDistanceMax = 3.0f, float fRadiusMin = 0.5f, float fRadiusMax = 1.5f, float fSuitableAngle = PI_DIV_8*.25f, float fSegmentSizeMin = Level().AI.GetHeader().size*.5f, float fSegmentSizeMax = Level().AI.GetHeader().size*2.f);

#endif
