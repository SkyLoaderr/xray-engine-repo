// AI_Space.h: interface for the CAI_Space class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_)
#define AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_
#pragma once

#include "virtualvector.h"
#include "xrLevel.h"
#include "xr_list.h"
#include "xr_qlist.h"

namespace AI {
	class ENGINE_API NodeEstimator
	{
		friend class CAI_Space;
	public:
		DWORD	BestNode;
		float	BestCost;
	public:
		// Estimator itself: 
		// Node, SqrDistance2node, stop/continue
		// Return: min - best, max - worse
		virtual	float	Estimate	(NodeCompressed* Node, float SqrDist, BOOL& bStop)=0;
	};
	class ENGINE_API NodeHeuristic
	{
	};

	const int	hashSize	= 32;

	class	ENGINE_API	Path
	{
	public:
		CList<DWORD>	Nodes;
	};
};

class ENGINE_API CAI_Space  
{
private:
	// Initial data
	CVirtualFileStream*				vfs;			// virtual file
	hdrNODES						m_header;		// m_header
	BYTE*							m_nodes;		// virtual nodes DATA array
	NodeCompressed**				m_nodes_ptr;	// pointers to node's data

	// Query
	vector<bool>					q_mark;			// temporal usage mark for queries
	CList<DWORD>					q_stack;
public:
	CAI_Space		();
	~CAI_Space		();

	void			Load			(LPCSTR name);
	void			Render			();

	void			q_Range			(DWORD StartNode, const Fvector& Pos,	float Range,	AI::NodeEstimator& Estimator);
	void			q_Path			(DWORD StartNode, DWORD GoalNode,		AI::Path&	Result);
	DWORD			q_Node			(DWORD PrevNode,  const Fvector& Pos);

	int				q_LoadSearch	(const Fvector& Pos);	// <0 - failure

	// Helper functions
	IC	hdrNODES&			Header()		{ return m_header; }
	IC	NodeCompressed*		Node(DWORD ID)	{ return vfs?m_nodes_ptr[ID]:NULL; }

	IC	void		UnpackPosition	(Fvector& Pdest, const NodePosition& Psrc)
	{
		Pdest.x = float(Psrc[0])*m_header.size;
		Pdest.y = (float(Psrc[1])/32767)*m_header.size_y;
		Pdest.z = float(Psrc[2])*m_header.size;
	}
	IC	DWORD		UnpackLink		(NodeLink& L)
	{	return (*LPDWORD(&L))&0x00ffffff;	}

	IC	void		PackPosition	(NodePosition& Pdest, const Fvector& Psrc)
	{
		float sy = 32767/m_header.size_y;
		float sp = 1/m_header.size;
		float sh = 0.5f*m_header.size;
		
		int px,py,pz;
		px = iFloor((Psrc.x+sh)*sp+EPS_L);							Pdest[0]=short(px);
		py = iFloor(Psrc.y*sy+EPS_L);		clamp(py,-32767,32767);	Pdest[1]=short(py);
		pz = iFloor((Psrc.z+sh)*sp+EPS_L);							Pdest[2]=short(pz);
	}

	// Utilities
	float			u_SqrDistance2Node	(const Fvector& P, const NodeCompressed* Node);
	IC BOOL			u_InsideNode		(const NodeCompressed& N, const NodePosition& P)
	{
		return 	((P[0]>=N.p0[0])&&(P[0]<=N.p1[0]))&&	// X inside
				((P[2]>=N.p0[2])&&(P[2]<=N.p1[2]));		// Z inside
	}
};

#endif // !defined(AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_)
