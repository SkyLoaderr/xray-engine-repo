////////////////////////////////////////////////////////////////////////////
//	Module 		: a_star_interfaces.h
//	Created 	: 21.03.2002
//  Modified 	: 26.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Interfaces for A* algortihm
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAI_Space;

typedef struct tagSAIMapData {
	CAI_Space	*tpAI_Space;
	u32			dwFinishNode;
} SAIMapData;

typedef struct tagSAIMapDataL {
	CAI_Space	*tpAI_Space;
	u32			dwFinishNode;
	float		fLight;
	float		fCover;
	float		fDistance;
} SAIMapDataL;

typedef struct tagSAIMapDataE {
	CAI_Space	*tpAI_Space;
	u32			dwFinishNode;
	u32			dwEnemyNode;
	float		fLight;
	float		fCover;
	float		fDistance;
	float		fEnemyDistance;
	float		fEnemyView;
} SAIMapDataE;

typedef struct tagSAIMapDataF {
	CAI_Space	*tpAI_Space;
	u32			dwFinishNode;
	Fvector		tEnemyPosition;
	float		fLight;
	float		fCover;
	float		fDistance;
	float		fEnemyDistance;
	float		fEnemyView;
} SAIMapDataF;

class CAIMapTemplateNode {
public:
	float		x1;
	float		y1;
	float		z1;
	float		x2;
	float		y2;
	float		z2;
	float		x3;
	float		y3;
	float		z3;
	u32			m_dwLastBestNode;
	typedef		NodeLink* iterator;
};

class CAIGraphTemplateNode {
public:
	u32			m_dwLastBestNode;
	typedef		AI::SGraphEdge* iterator;
	u32			get_value				(iterator &tIterator)
	{
		return(tIterator->dwVertexNumber);
	}
};

class CAIMapShortestPathNode : public CAIMapTemplateNode {
public:
	SAIMapData	tData;
				CAIMapShortestPathNode	(SAIMapData &tAIMapData);
	void		begin					(u32 dwNode, iterator &tStart, iterator &tEnd);
	u32			get_value				(iterator &tIterator);
	bool		bfCheckIfAccessible		(u32 dwNode);
	float		ffEvaluate				(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator);
	float		ffAnticipate			(u32 dwStartNode);
	float		ffAnticipate			();
};

class CAIMapLCDPathNode : public CAIMapTemplateNode {
public:
	SAIMapDataL	tData;
	float		m_fSum;
				CAIMapLCDPathNode		(SAIMapDataL &tAIMapData);
	void		begin					(u32 dwNode, iterator &tStart, iterator &tEnd);
	u32			get_value				(iterator &tIterator);
	bool		bfCheckIfAccessible		(u32 dwNode);
	float		ffEvaluate				(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator);
	float		ffAnticipate			(u32 dwStartNode);
	float		ffAnticipate			();
};

class CAIMapEnemyPathNode : public CAIMapTemplateNode {
public:
	float		x4;
	float		y4;
	float		z4;
	SAIMapDataE	tData;
	float		m_fSum;
				CAIMapEnemyPathNode		(SAIMapDataE &tAIMapData);
	void		begin					(u32 dwNode, iterator &tStart, iterator &tEnd);
	u32			get_value				(iterator &tIterator);
	bool		bfCheckIfAccessible		(u32 dwNode);
	float		ffEvaluate				(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator);
	float		ffAnticipate			(u32 dwStartNode);
	float		ffAnticipate			();
};

class CAIMapEnemyPositionPathNode : public CAIMapTemplateNode {
public:
	float		x4;
	float		y4;
	float		z4;
	SAIMapDataF	tData;
	float		m_fSum;
				CAIMapEnemyPositionPathNode	(SAIMapDataF &tAIMapData);
	void		begin					(u32 dwNode, iterator &tStart, iterator &tEnd);
	u32			get_value				(iterator &tIterator);
	bool		bfCheckIfAccessible		(u32 dwNode);
	float		ffEvaluate				(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator);
	float		ffAnticipate			(u32 dwStartNode);
	float		ffAnticipate			();
};

class CAIGraphShortestPathNode : public CAIGraphTemplateNode {
public:
	SAIMapData	tData;
				CAIGraphShortestPathNode(SAIMapData &tAIMapData);
	void		begin					(u32 dwNode, iterator &tStart, iterator &tEnd);
	bool		bfCheckIfAccessible		(u32 dwNode);
	float		ffEvaluate				(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator);
	float		ffAnticipate			(u32 dwStartNode);
	float		ffAnticipate			();
};