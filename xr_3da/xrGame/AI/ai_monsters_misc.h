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
		   
	// Fuzzy State Machine
	//#define WRITE_LOG

	#ifdef WRITE_LOG
		#define WRITE_TO_LOG(S) {\
			Msg("%s,%s,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",cName(),S,Level().timeServer(),vPosition.x,vPosition.y,vPosition.z,r_current.yaw,r_target.yaw,r_spine_current.yaw,r_spine_target.yaw,r_torso_current.yaw,r_torso_target.yaw);\
			vfUpdateDynamicObjects();\
			bStopThinking = true;\
		}
	#else
		#define WRITE_TO_LOG(S) {\
			vfUpdateDynamicObjects();\
			bStopThinking = true;\
		}
	#endif

	//m_dwLastRangeSearch = 0;
	#define GO_TO_NEW_STATE(a) {\
		eCurrentState = a;\
		return;\
	}

	//m_dwLastRangeSearch = 0;
	#define GO_TO_PREV_STATE {\
		eCurrentState = tStateStack.top();\
		tStateStack.pop();\
		return;\
	}

	#define SWITCH_TO_NEW_STATE(a) {\
		tStateStack.push(eCurrentState);\
		GO_TO_NEW_STATE(a);\
	}

	#define CHECK_IF_SWITCH_TO_NEW_STATE(a,b)\
		if (a)\
			SWITCH_TO_NEW_STATE(b)

	#define CHECK_IF_GO_TO_PREV_STATE(a)\
		if (a)\
			GO_TO_PREV_STATE

	#define CHECK_IF_GO_TO_NEW_STATE(a,b)\
		if (a)\
			GO_TO_NEW_STATE(b)
	
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
			r_target.yaw -= PI_DIV_6;\
		}\
		else\
			r_target.yaw = r_spine_target.yaw =  (r_torso_target.yaw + r_target.yaw)/2;\
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
	
	#define CUBE(x)	((x)*(x)*(x))
	#define LEFT_NODE(Index)					((Index + 3) & 3)
	#define RIGHT_NODE(Index)					((Index + 5) & 3)
	#define NEXT_POINT(m_iCurrentPoint)			(m_iCurrentPoint) == tpaPatrolPoints.size() - 1 ? 0 : (m_iCurrentPoint) + 1
	#define PREV_POINT(m_iCurrentPoint)			(m_iCurrentPoint) == 0 ? tpaPatrolPoints.size() - 1 : (m_iCurrentPoint) - 1
	#define COMPUTE_DISTANCE_2D(t,p)			(sqrtf(_sqr((t).x - (p).x) + _sqr((t).z - (p).z)))
	#define FN(i)								(float(tNode->cover[(i)])/255.f)

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

	IC bool bfInsideContour(Fvector &tPoint, PContour &tContour)
	{
		return((tContour.v1.x - EPS_L <= tPoint.x) && (tContour.v1.z - EPS_L <= tPoint.z) && (tContour.v3.x + EPS_L >= tPoint.x) && (tContour.v3.z + EPS_L >= tPoint.z));
	}

	IC bool bfSimilar(Fvector &tPoint0, Fvector &tPoint1)
	{
		return((fabsf(tPoint0.x - tPoint1.x) < EPS_L) && (fabsf(tPoint0.z - tPoint1.z) < EPS_L));
	}

	IC bool bfInsideNode(CAI_Space &AI, NodeCompressed *tpNode, Fvector &tCurrentPosition, float fHalfSubNodeSize)
	{
		Fvector tP0, tP1;
		AI.UnpackPosition(tP0,tpNode->p0);
		AI.UnpackPosition(tP1,tpNode->p1);
		return(
			(tCurrentPosition.x >= tP0.x - fHalfSubNodeSize - EPS) &&
			(tCurrentPosition.z >= tP0.z - fHalfSubNodeSize - EPS) &&
			(tCurrentPosition.x <= tP1.x + fHalfSubNodeSize + EPS) &&
			(tCurrentPosition.z <= tP1.z + fHalfSubNodeSize + EPS)
		);
	}

	typedef struct tagSSubNode {
		Fvector tLeftDown;
		Fvector tRightUp;
		bool	bEmpty;
	} SSubNode;

	extern void vfCreateFastRealisticPath(vector<Fvector> &tpaPoints, DWORD dwStartNode, vector<Fvector> &tpaDeviations, vector<CTravelNode> &tpaPath, vector<DWORD> &dwaNodes, bool bLooped, bool bUseDeviations = true, float fRoundedDistanceMin = 1.5f, float fRoundedDistanceMax = 3.0f, float fRadiusMin = .5f, float fRadiusMax = 3.0f, float fSuitableAngle = PI_DIV_8*.375f, float fSegmentSizeMin = Level().AI.GetHeader().size*.5f, float fSegmentSizeMax = Level().AI.GetHeader().size*2.f);
	extern void vfCreatePointSequence(CLevel::SPatrolPath &tpPatrolPath,vector<Fvector> &tpaPoints, bool &bLooped);
	extern float ffCalcSquare(float fAngle, float fAngleOfView, float _b0, float _b1, float _b2, float _b3);
#endif
