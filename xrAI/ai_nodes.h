////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_nodes.h
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : AI nodes structures and functions
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAI_AI_NODES__
#define __XRAI_AI_NODES__

extern CStream*					vfs;			// virtual file
extern hdrNODES					m_header;		// m_header
extern BYTE*						m_nodes;		// virtual nodes DATA array
extern NodeCompressed**			m_nodes_ptr;	// pointers to node's data
extern vector<bool>				q_mark_bit;		// temporal usage mark for queries

IC	NodeCompressed*		Node(u32 ID)	{ return vfs?m_nodes_ptr[ID]:NULL; }

IC	u32		UnpackLink		(NodeLink& L) {	return (*LPDWORD(&L))&0x00ffffff;	}

IC	void PackPosition(NodePosition& Pdest, const Fvector& Psrc)
{
	float sp = 1/m_header.size;
	int px	= iFloor(Psrc.x*sp+EPS_L + .5f);
	int py	= iFloor(65535.f*(Psrc.y-m_header.aabb.min.y)/(m_header.size_y)+EPS_L);
	int pz	= iFloor(Psrc.z*sp+EPS_L + .5f);
	clamp	(px,-32767,32767);	Pdest.x = s16	(px);
	clamp	(py,0,     65535);	Pdest.y = u16	(py);
	clamp	(pz,-32767,32767);	Pdest.z = s16	(pz);
}

IC BOOL	u_InsideNode(const NodeCompressed& N, const NodePosition& P)
{
	return 	((P.x>=N.p0.x)&&(P.x<=N.p1.x))&&	// X inside
			((P.z>=N.p0.z)&&(P.z<=N.p1.z));		// Z inside
}

IC	void UnpackPosition(Fvector& Pdest, const NodePosition& Psrc)
{
	Pdest.x = float(Psrc.x)*m_header.size;
	Pdest.y = (float(Psrc.y)/65535)*m_header.size_y + m_header.aabb.min.y;
	Pdest.z = float(Psrc.z)*m_header.size;
}

#endif