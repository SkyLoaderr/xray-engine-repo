// AI_Space.h: interface for the CAI_Space class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_)
#define AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_
#pragma once

#include "group.h"
#include "..\xrLevel.h"
#include "ai_funcs.h"
#include "ai_alife_graph.h"
#include "ai_alife_cross_table.h"
#include "ai\ai_selector_template.h"

namespace AI {
	DEFINE_VECTOR	(u32,						DWORD_VECTOR,			DWORD_IT);
	DEFINE_VECTOR	(float,						FLOAT_VECTOR,			FLOAT_IT);
	DEFINE_VECTOR	(bool,						BOOL_VECTOR,			BOOL_IT);

	const int	hashSize	= 32;

	class	NodePath
	{
	public:
		DWORD_VECTOR	Nodes;
	};
};

class CAStar;

class CAI_Space	: public CALifeGraph, public CAI_DDD, public CALifeCrossTable, public pureDeviceCreate, pureDeviceDestroy
{
private:
	// Initial data
	IReader*			vfs;			// virtual file
	hdrNODES			m_header;		// m_header
	BYTE*				m_nodes;		// virtual nodes DATA array
	NodeCompressed**	m_nodes_ptr;	// pointers to node's data

	// Debug
	Shader*				sh_debug;

public:
	// Query
	vector<bool>		q_mark_bit;		// temporal usage mark for queries
	vector<bool>		q_mark_bit_x;		// temporal usage mark for queries
	vector<BYTE>		q_mark;			// temporal usage mark for queries
	vector<u32>			q_stack;
	
						CAI_Space		();
	virtual				~CAI_Space		();

	void				Load			();
	void				Unload			();
	void				Render			();

	u32					q_Node			(u32 PrevNode,  const Fvector& Pos,		bool bShortSearch = false);
	int					q_LoadSearch	(const Fvector& Pos);	// <0 - failure

	IC	bool			bfCheckIfMapLoaded()		{return(vfs != 0);}
	IC	bool			bfCheckIfGraphLoaded()		{return(m_tpGraphVFS != 0);}
	IC	bool			bfCheckIfCrossTableLoaded()	{return(m_tpCrossTableVFS != 0);}

	// a* algorithm		interfaces
	float				m_fSize2,m_fYSize2;
	CAStar				*m_tpAStar;

	// Helper functions
	IC	const hdrNODES&		Header()		{ return m_header; }
	IC	NodeCompressed*		Node(u32 ID)	{ return vfs?m_nodes_ptr[ID]:NULL; }

	IC	void		UnpackPosition	(Fvector& Pdest, const NodePosition& Psrc)
	{
		Pdest.x = float(Psrc.x)*m_header.size;
		Pdest.y = (float(Psrc.y)/65535)*m_header.size_y + m_header.aabb.min.y;
		Pdest.z = float(Psrc.z)*m_header.size;
	}
	IC	u32		UnpackLink		(NodeLink& L)
	{	return (*LPDWORD(&L))&0x00ffffff;	}

	IC	void		PackPosition	(NodePosition& Pdest, const Fvector& Psrc)
	{
		float sp = 1/m_header.size;
		int px	= iFloor(Psrc.x*sp+EPS_L + .5f);
		int py	= iFloor(65535.f*(Psrc.y-m_header.aabb.min.y)/(m_header.size_y)+EPS_L);
		int pz	= iFloor(Psrc.z*sp+EPS_L + .5f);
		/**/
		clamp	(px,-32767,32767);	Pdest.x = s16	(px);
		clamp	(py,0,     65535);	Pdest.y = u16	(py);
		clamp	(pz,-32767,32767);	Pdest.z = s16	(pz);
	}

	// REF-counting
	IC  BYTE		ref_add			(u32 ID)	
	{
		if (vfs && ID!=u32(-1))	return q_mark[ID] += BYTE(1);	
		else						return BYTE(0);
	}
	IC	BYTE		ref_dec			(u32 ID)
	{
		if (vfs && ID!=u32(-1))	return q_mark[ID] -= BYTE(1);	
		else						return BYTE(0);
	}

	// Utilities
	float			u_SqrDistance2Node	(const Fvector& P, const NodeCompressed* Node);
	IC BOOL			u_InsideNode		(const NodeCompressed& N, const NodePosition& P)
	{
		return 	((P.x>=N.p0.x)&&(P.x<=N.p1.x))&&	// X inside
				((P.z>=N.p0.z)&&(P.z<=N.p1.z));		// Z inside
	}
	
	IC bool bfInsideNode(NodeCompressed *tpNode, Fvector &tCurrentPosition)
	{
		Fvector tP0, tP1;
		float fHalfSubNodeSize = m_header.size*.5f;
		UnpackPosition(tP0,tpNode->p0);
		UnpackPosition(tP1,tpNode->p1);
		return(
			(tCurrentPosition.x >= tP0.x - fHalfSubNodeSize - EPS) &&
			(tCurrentPosition.z >= tP0.z - fHalfSubNodeSize - EPS) &&
			(tCurrentPosition.x <= tP1.x + fHalfSubNodeSize + EPS) &&
			(tCurrentPosition.z <= tP1.z + fHalfSubNodeSize + EPS)
		);
	}

	IC float ffGetY(NodeCompressed &tNode, float X, float Z)
	{
		Fvector	DUP, vNorm, v, v1, P0;
		DUP.set(0,1,0);
		pvDecompress(vNorm,tNode.plane);
		Fplane PL; 
		UnpackPosition(P0,tNode.p0);
		PL.build(P0,vNorm);
		v.set(X,P0.y,Z);	
		PL.intersectRayPoint(v,DUP,v1);	
		return(v1.y);
	}

	IC	bool bfTooSmallAngle(float fAngle0, float fAngle1, float fDelta)
	{
		return(_abs(fAngle0 - fAngle1) < fDelta) || ((_abs(_abs(fAngle0 - fAngle1) - PI_MUL_2) < fDelta));
	}

	Fvector	tfGetNodeCenter(u32 dwNodeID);
	Fvector	tfGetNodeCenter(NodeCompressed *tpNode);
	
	float	ffGetDistanceBetweenNodeCenters(u32 dwNodeID0, u32 dwNodeID1);
	float	ffGetDistanceBetweenNodeCenters(NodeCompressed *tpNode0, u32 dwNodeID1);
	float	ffGetDistanceBetweenNodeCenters(u32 dwNodeID0, NodeCompressed *tpNode1);
	float	ffGetDistanceBetweenNodeCenters(NodeCompressed *tpNode0, NodeCompressed *tpNode1);
	void	vfCreateFastRealisticPath(vector<Fvector> &tpaPoints, u32 dwStartNode, vector<Fvector> &tpaDeviations, vector<Fvector> &tpaPath, vector<u32> &dwaNodes, bool bLooped, bool bUseDeviations = false, float fRoundedDistanceMin = 2.0f, float fRoundedDistanceMax = 2.0f, float fRadiusMin = 3.0f, float fRadiusMax = 3.0f, float fSuitableAngle = PI_DIV_8*.375f, float fSegmentSizeMin = .35f, float fSegmentSizeMax = 1.4f);
	float	ffMarkNodesInDirection(u32 dwStartNode, Fvector tStartPosition, Fvector tDirection, vector<bool> &tpaMarks, float fDistance, vector<u32> &tpaStack);
	bool	bfCheckNodeInDirection(u32 dwStartNode, Fvector tStartPosition, u32 dwFinishNode);
	u32		dwfCheckPositionInDirection(u32 dwStartNode, Fvector tStartPosition, Fvector tFinishPosition);

	// Device dependance
	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
};

extern CAI_Space *tpAI_Space;

IC CAI_Space& getAI()
{
	if (!tpAI_Space)
		 tpAI_Space = xr_new<CAI_Space>();
	return(*((CAI_Space*)tpAI_Space));
};

#endif // !defined(AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_)