// AI_Space.h: interface for the CAI_Space class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_)
#define AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_
#pragma once

//#include "virtualvector.h"
#include "..\xrLevel.h"

namespace AI {
	class	NodeEstimator
	{
		friend class CAI_Space;
	public:
		DWORD	BestNode;
		float	BestCost;
	public:
		MemberPlacement taMemberPositions;
		MemberNodes		taMemberNodes;
		MemberPlacement taDestMemberPositions;
		MemberNodes		taDestMemberNodes;
		EntityVec		*taMembers;
		#define MAX_NODE_ESTIMATION_COST 10000000.f
		// Estimator itself: 
		// Node, SqrDistance2node, stop/continue
		// Return: min - best, max - worse
		virtual	float	Estimate	(NodeCompressed* Node, float SqrDist, BOOL& bStop)=0;
	};

	const int	hashSize	= 32;

	class	Path
	{
	public:
		vector<DWORD>	Nodes;
	};
};

class CAI_Space	: public pureDeviceCreate, pureDeviceDestroy
{
private:
	// Initial data
	CStream*						vfs;			// virtual file
	hdrNODES						m_header;		// m_header
	BYTE*							m_nodes;		// virtual nodes DATA array
	NodeCompressed**				m_nodes_ptr;	// pointers to node's data

	// Debug
	Shader*							sh_debug;
public:
	// Query
	vector<bool>					q_mark_bit;		// temporal usage mark for queries
	vector<bool>					q_mark_bit_x;		// temporal usage mark for queries
	vector<BYTE>					q_mark;			// temporal usage mark for queries
	vector<DWORD>					q_stack;
	
	CAI_Space		();
	~CAI_Space		();

	void			Load			(LPCSTR name);
	void			Render			();

	const hdrNODES	GetHeader		(){return m_header;};
	void			q_Range(DWORD StartNode, const Fvector& BasePos, float Range, AI::NodeEstimator& Estimator, float &fOldCost, DWORD dwTimeDifference);
	void			q_Range			(DWORD StartNode, const Fvector& Pos,	float Range,	AI::NodeEstimator& Estimator, float &fOldCost);
	void			q_Range_Bit		(DWORD StartNode, const Fvector& BasePos, float Range, NodePosition* QueryPosition, DWORD &BestNode, float &BestCost);
	void			q_Range_Bit_X	(DWORD StartNode, const Fvector& BasePos, float Range, NodePosition* QueryPosition, DWORD &BestNode, float &BestCost);
	//void			q_Range_Bit		(DWORD StartNode, const Fvector& BasePos, float Range, DWORD &BestNode, float &BestCost);
	DWORD			q_Node			(DWORD PrevNode,  const Fvector& Pos);

	// yet another A* search
	#define DEFAULT_LIGHT_WEIGHT		  5.f 
	#define DEFAULT_COVER_WEIGHT		 10.f 
	#define DEFAULT_DISTANCE_WEIGHT		 40.f
	#define DEFAULT_ENEMY_VIEW_WEIGHT	100.f
	float			vfFindTheXestPath(DWORD dwStartNode, DWORD dwGoalNode, AI::Path& Result, float fLightWeight = DEFAULT_LIGHT_WEIGHT, float fCoverWeight = DEFAULT_COVER_WEIGHT, float fDistanceWeight = DEFAULT_DISTANCE_WEIGHT);
	float			vfFindTheXestPath(DWORD dwStartNode, DWORD dwGoalNode, AI::Path& Result, NodeCompressed& tEnemyNode, float fOptimalEnemyDistance, float fLightWeight = DEFAULT_LIGHT_WEIGHT, float fCoverWeight = DEFAULT_COVER_WEIGHT, float fDistanceWeight = DEFAULT_DISTANCE_WEIGHT, float fEnemyViewWeight = DEFAULT_ENEMY_VIEW_WEIGHT);
	float			vfFindTheXestPath(DWORD dwStartNode, DWORD dwGoalNode, AI::Path& Result, Fvector tEnemyPosition, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight);
	void			vfLoadSearch();
	void			vfUnloadSearch();
	IC const vector<BYTE>	tpfGetNodeMarks() {return(q_mark);};
	// 

	// yet another best node search
	//void			vfFindTheBestNode(DWORD StartNode, const Fvector& Pos,	float Range,	AI::NodeEstimator& Estimator);
	// 

	int				q_LoadSearch	(const Fvector& Pos);	// <0 - failure

	// Helper functions
	IC	const hdrNODES&		Header()		{ return m_header; }
	IC	NodeCompressed*		Node(DWORD ID)	{ return vfs?m_nodes_ptr[ID]:NULL; }

	IC	void		UnpackPosition	(Fvector& Pdest, const NodePosition& Psrc)
	{
		Pdest.x = float(Psrc.x)*m_header.size;
		Pdest.y = (float(Psrc.y)/65535)*m_header.size_y + m_header.aabb.min.y;
		Pdest.z = float(Psrc.z)*m_header.size;
	}
	IC	DWORD		UnpackLink		(NodeLink& L)
	{	return (*LPDWORD(&L))&0x00ffffff;	}

	IC	void		PackPosition	(NodePosition& Pdest, const Fvector& Psrc)
	{
		float sp = 1/m_header.size;
		/**
		int px	= iFloor(Psrc.x*sp+EPS_L);
		int py	= iFloor(65535.f*(Psrc.y-m_header.aabb.min.y)/(m_header.size_y)+EPS_L);
		int pz	= iFloor(Psrc.z*sp+EPS_L);
		/**/
		int px	= iFloor(Psrc.x*sp+EPS_L + .5f);
		int py	= iFloor(65535.f*(Psrc.y-m_header.aabb.min.y)/(m_header.size_y)+EPS_L);
		int pz	= iFloor(Psrc.z*sp+EPS_L + .5f);
		/**/
		clamp	(px,-32767,32767);	Pdest.x = s16	(px);
		clamp	(py,0,     65535);	Pdest.y = u16	(py);
		clamp	(pz,-32767,32767);	Pdest.z = s16	(pz);
	}

	// REF-counting
	IC  BYTE		ref_add			(DWORD ID)	{	return vfs?(q_mark[ID] += 1):0;	}
	IC	BYTE		ref_dec			(DWORD ID)  {	return vfs?(q_mark[ID] -= 1):0;	}

	// Utilities
	float			u_SqrDistance2Node	(const Fvector& P, const NodeCompressed* Node);
	IC BOOL			u_InsideNode		(const NodeCompressed& N, const NodePosition& P)
	{
		return 	((P.x>=N.p0.x)&&(P.x<=N.p1.x))&&	// X inside
				((P.z>=N.p0.z)&&(P.z<=N.p1.z));		// Z inside
	}
	
	Fvector	tfGetNodeCenter(DWORD dwNodeID);
	Fvector	tfGetNodeCenter(NodeCompressed *tpNode);
	
	float ffGetDistanceBetweenNodeCenters(DWORD dwNodeID0, DWORD dwNodeID1);
	float ffGetDistanceBetweenNodeCenters(NodeCompressed *tpNode0, DWORD dwNodeID1);
	float ffGetDistanceBetweenNodeCenters(DWORD dwNodeID0, NodeCompressed *tpNode1);
	float ffGetDistanceBetweenNodeCenters(NodeCompressed *tpNode0, NodeCompressed *tpNode1);

	// Device dependance
	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
};

#endif // !defined(AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_)
