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
#include "..\\Level.h"
#include "..\\Entity.h"
#include "..\\CustomMonster.h"
#include "..\\Group.h"
		   
	// Fuzzy State Machine
	#define ASSIGN_PROPORTIONAL_POWER(a,b)	if ((eType & a) == a) power*=b;

	#define WRITE_LOG

	#ifndef DEBUG
	#undef WRITE_LOG
	#endif

	#ifdef WRITE_LOG
		#define WRITE_TO_LOG(S) {\
			Msg("%s,%s,%d,p[%.2f,%.2f,%.2f],h[%.2f,%.2f],t[%.2f,%.2f]",cName(),S,Level().timeServer(),vPosition.x,vPosition.y,vPosition.z,r_current.yaw,r_target.yaw,r_torso_current.yaw,r_torso_target.yaw);\
			Msg("%d",Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].m_tpaSuspiciousNodes.size());\
			vfUpdateDynamicObjects();\
			bStopThinking = true;\
		}
	#else
		#define WRITE_TO_LOG(S) {\
			vfUpdateDynamicObjects();\
			bStopThinking = true;\
		}
	#endif

	#define GO_TO_NEW_STATE(a) {\
		tStateStack.top() = eCurrentState = a;\
		return;\
	}

	#define GO_TO_PREV_STATE {\
		tStateStack.pop();\
		eCurrentState = tStateStack.top();\
		return;\
	}

	#define SWITCH_TO_NEW_STATE(a) {\
		tStateStack.push(a);\
		GO_TO_NEW_STATE(a);\
	}

	#define CHECK_IF_SWITCH_TO_NEW_STATE(a,b) {\
		if (a)\
			SWITCH_TO_NEW_STATE(b);\
	}

	#define CHECK_IF_GO_TO_PREV_STATE(a) {\
		if (a)\
			GO_TO_PREV_STATE;\
	}

	#define CHECK_IF_GO_TO_NEW_STATE(a,b) {\
		if (a)\
			GO_TO_NEW_STATE(b);\
	}
	
	//////////////////////////////////////////////////////////////////////////
	#define SWITCH_TO_NEW_STATE_AND_UPDATE(a) {\
		tStateStack.push(a);\
		vfAddStateToList(a);\
		GO_TO_NEW_STATE(a);\
	}

	#define CHECK_IF_SWITCH_TO_NEW_STATE_AND_UPDATE(a,b) {\
		if (a)\
			SWITCH_TO_NEW_STATE_AND_UPDATE(b);\
	}
	//////////////////////////////////////////////////////////////////////////
	#define GO_TO_NEW_STATE_THIS_UPDATE(a) {\
		bStopThinking = false;\
		GO_TO_NEW_STATE(a);\
	}

	#define GO_TO_PREV_STATE_THIS_UPDATE {\
		bStopThinking = false;\
		GO_TO_PREV_STATE;\
	}

	#define SWITCH_TO_NEW_STATE_THIS_UPDATE(a) {\
		tStateStack.push(eCurrentState = a);\
		GO_TO_NEW_STATE_THIS_UPDATE(a);\
	}

	#define CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(a,b) {\
		if (a)\
			SWITCH_TO_NEW_STATE_THIS_UPDATE(b);\
	}

	#define CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(a) {\
		if (a)\
			GO_TO_PREV_STATE_THIS_UPDATE;\
	}

	#define CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(a,b) \
		if (a)\
			GO_TO_NEW_STATE_THIS_UPDATE(b);
	
	//////////////////////////////////////////////////////////////////////////
	#define SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(a) {\
		tStateStack.push(a);\
		vfAddStateToList(a);\
		GO_TO_NEW_STATE_THIS_UPDATE(a);\
	}

	#define CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(a,b) {\
		if (a)\
			SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(b);\
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

	// Bones
	#define ASSIGN_SPINE_BONE {\
		if (fabsf(r_torso_target.yaw - r_target.yaw) < MIN_SPINE_TURN_ANGLE) {\
			r_spine_target.yaw = r_target.yaw;\
		}\
		else\
			r_target.yaw = r_spine_target.yaw =  r_torso_target.yaw;\
	}
			
	#define LOOK_AT_DIRECTION(A) {\
		mk_rotation(A,r_torso_target);\
		r_target.yaw = r_torso_target.yaw;\
		ASSIGN_SPINE_BONE\
	}

	#define INIT_SQUAD_AND_LEADER \
		CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];\
		CEntity* Leader = Squad.Leader;\
		if (Leader->g_Health() <= 0)\
			Leader = this;\
		R_ASSERT (Leader);
	
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
		ADJUST_BONE(r_torso_current);
			
	#define SUB_ANGLE(A,B)\
		A -= B;\
		if (A <= -EPS_L )\
			A += PI_MUL_2;
			
	#define ADD_ANGLE(A,B)\
		A += B;\
		if (A >= PI_MUL_2 - EPS_L)\
			A -= PI_MUL_2;
	
	#define CUBE(x)	((x)*(x)*(x))
	#define LEFT_NODE(Index)					((Index + 3) & 3)
	#define RIGHT_NODE(Index)					((Index + 5) & 3)
	#define NEXT_POINT(m_iCurrentPoint)			(m_iCurrentPoint) == tpaPatrolPoints.size() - 1 ? 0 : (m_iCurrentPoint) + 1
	#define PREV_POINT(m_iCurrentPoint)			(m_iCurrentPoint) == 0 ? tpaPatrolPoints.size() - 1 : (m_iCurrentPoint) - 1
	#define COMPUTE_DISTANCE_2D(t,p)			(sqrtf(_sqr((t).x - (p).x) + _sqr((t).z - (p).z)))

	IC float ffGetY(NodeCompressed &tNode, float X, float Z)
	{
		Fvector	DUP, vNorm, v, v1, P0;
		DUP.set(0,1,0);
		pvDecompress(vNorm,tNode.plane);
		Fplane PL; 
		Level().AI.UnpackPosition(P0,tNode.p0);
		PL.build(P0,vNorm);
		v.set(X,P0.y,Z);	
		PL.intersectRayPoint(v,DUP,v1);	
		return(v1.y);
	}

	IC bool bfCheckIfReadyToPatrol(CCustomMonster *tpCustomMonster, CGroup &Group, bool bLeader = false)
	{
		int i = 0;
		if (!bLeader) {
			for ( i=0; i<(int)Group.Members.size(); i++)
				if (Group.Members[i] == tpCustomMonster)
					break;
			i = (i + 1) % 3;
		}
		return((tpCustomMonster->m_iCurrentPatrolIndex >= 0) && (tpCustomMonster->Position().distance_to(tpCustomMonster->m_tpPath->tpaVectors[i][tpCustomMonster->m_iCurrentPatrolIndex]) < EPS_L));
	}

	typedef struct tagSSubNode {
		Fvector tLeftDown;
		Fvector tRightUp;
		bool	bEmpty;
	} SSubNode;

	extern void vfGoToPointViaNodes(vector<CTravelNode> &tpaPath, DWORD dwCurNode, Fvector tStartPoint, Fvector tFinishPoint);
	extern float ffCalcSquare(float fAngle, float fAngleOfView, float _b0, float _b1, float _b2, float _b3);
	extern float ffCalcSquare(float fAngle, float fAngleOfView, NodeCompressed *tpNode);
	extern float ffCalcSquare(float fAngle, float fAngleOfView, DWORD dwNodeID);
	extern float ffGetCoverInDirection(float fAngle, float b0, float b1, float b2, float b3);
	extern float ffGetCoverInDirection(float fAngle, NodeCompressed *tpNode);
	extern float ffGetCoverInDirection(float fAngle, DWORD dwNodeID);
	extern SRotation tfGetOrientation(CEntity *tpEntity);
	extern int   ifFindNearestPatrolPoint(vector<Fvector> &tpaVector, const Fvector &tPosition);
#endif
