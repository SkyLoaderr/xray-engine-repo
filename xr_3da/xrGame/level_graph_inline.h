////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef AI_COMPILER
	#include "profile.h"
#else
	#ifdef TIMER_START
		#undef TIMER_START
		#undef TIMER_STOP
		#define TIMER_START(a)
		#define TIMER_STOP(a)
	#endif
#endif

IC const CLevelGraph::CHeader &CLevelGraph::header	() const
{
	return				(*m_header);
}

IC bool CLevelGraph::valid_vertex_id	(u32 id) const
{
	TIMER_START			(ValidVertexID)
	bool				b = id < header().vertex_count();
	TIMER_STOP			(ValidVertexID)
	return				(b);
}

IC	CLevelGraph::CVertex	*CLevelGraph::vertex(const u32 vertex_id) const
{
	VERIFY				(valid_vertex_id(vertex_id));
	return				(m_nodes + vertex_id);
}

IC	u32	CLevelGraph::vertex	(const CVertex *vertex_p) const
{
	VERIFY				((vertex_p > m_nodes) && valid_vertex_id(u32(vertex_p - m_nodes)));
	return				(u32(vertex_p - m_nodes));
}

IC	u32	CLevelGraph::vertex	(const CVertex &vertex_r) const
{
	return				(vertex(&vertex_r));
}

IC u8	CLevelGraph::ref_add		(u32 id)	
{
	VERIFY				(valid_vertex_id(id));
	VERIFY				(m_ref_counts[id] < u8(-1));
	m_ref_counts[id]	+= u8(1);	
	return				(m_ref_counts[id]);	
}

IC u8	CLevelGraph::ref_dec		(u32 id)
{
	VERIFY				(valid_vertex_id(id));
	VERIFY				(m_ref_counts[id]);
	m_ref_counts[id]	-= u8(1);	
	return				(m_ref_counts[id]);	
}

IC void CLevelGraph::unpack_xz(const CLevelGraph::CPosition &vertex_position, int &x, int &z) const
{
	x					= vertex_position.xz() / m_row_length;
	z					= vertex_position.xz() % m_row_length;
}

IC void CLevelGraph::unpack_xz(const CLevelGraph::CPosition &vertex_position, float &x, float &z) const
{
	int					_x,_z;
	unpack_xz			(vertex_position,_x,_z);
	x					= float(_x)*header().cell_size() + header().box().min.x;
	z 					= float(_z)*header().cell_size() + header().box().min.z;
}

template <typename T>
IC void CLevelGraph::unpack_xz(const CLevelGraph::CVertex &vertex, T &x, T &z) const
{
	unpack_xz			(vertex.position(),x,z);
}

template <typename T>
IC void CLevelGraph::unpack_xz(const CLevelGraph::CVertex *vertex, T &x, T &z) const
{
	unpack_xz			(*vertex,x,z);
}

IC const Fvector CLevelGraph::vertex_position	(const CLevelGraph::CPosition &source_position) const
{
	Fvector				dest_position;
	unpack_xz			(source_position,dest_position.x,dest_position.z);
	dest_position.y 	= (float(source_position.y())/65535)*header().factor_y() + header().box().min.y;
	return				(dest_position);
}

IC const Fvector &CLevelGraph::vertex_position	(Fvector &dest_position, const CLevelGraph::CPosition &source_position) const
{
	return				(dest_position = vertex_position(source_position));
}

IC const CLevelGraph::CPosition &CLevelGraph::vertex_position	(CLevelGraph::CPosition &dest_position, const Fvector &source_position) const
{
	float				sp = 1/header().cell_size();
	int					pxz	= iFloor(((source_position.x - header().box().min.x)*sp + EPS_S + .5f))*m_row_length + iFloor((source_position.z - header().box().min.z)*sp   + EPS_S + .5f);
	int					py	= iFloor(65535.f*(source_position.y - header().box().min.y)/header().factor_y() + EPS_S);
	clamp				(pxz,0,(1 << 21) - 1);
	dest_position.xz	(u32(pxz));
	clamp				(py,0,65535);
	dest_position.y		(u16(py));
	return				(dest_position);
}

IC  const Fvector CLevelGraph::vertex_position(u32 vertex_id) const
{
	TIMER_START			(VertexPosition)
	Fvector				t = vertex_position(vertex(vertex_id));
	TIMER_STOP			(VertexPosition)
	return				(t);
}

IC  const Fvector CLevelGraph::vertex_position(const CLevelGraph::CVertex &vertex) const
{
	Fvector				position;
	vertex_position		(position, vertex.position());
	return				(position);
}

IC  const Fvector CLevelGraph::vertex_position(const CLevelGraph::CVertex *vertex) const
{
	return				(vertex_position(*vertex));
}

IC	const CLevelGraph::CPosition	CLevelGraph::vertex_position	(const Fvector &position) const
{
	CLevelGraph::CPosition	_vertex_position;
	return				(vertex_position(_vertex_position,position));
}

IC bool	CLevelGraph::inside				(const CLevelGraph::CVertex &vertex, const CLevelGraph::CPosition &vertex_position) const
{
	return 				(vertex_position.xz() == vertex.position().xz());
}

IC bool	CLevelGraph::inside				(const CLevelGraph::CVertex &vertex, const Fvector &position) const
{
	return				(inside(vertex,vertex_position(position)));
}

IC bool	CLevelGraph::inside				(const CVertex *vertex, const CLevelGraph::CPosition &vertex_position) const
{
	return				(inside(*vertex,vertex_position));
}

IC bool CLevelGraph::inside				(const CVertex *vertex, const Fvector &position) const
{
	return				(inside(*vertex,position));
}

IC bool	CLevelGraph::inside				(const u32 vertex_id, const CLevelGraph::CPosition &vertex_position) const
{
	return				(inside(vertex(vertex_id),vertex_position));
}

IC bool CLevelGraph::inside				(const u32 vertex_id, const Fvector &position) const
{
	TIMER_START			(Inside)
	bool				b = inside(vertex(vertex_id),position);
	TIMER_STOP			(Inside)
	return				(b);
}

IC bool	CLevelGraph::inside				(const CLevelGraph::CVertex &vertex, const CLevelGraph::CPosition &_vertex_position, const float epsilon) const
{
	return				(inside(vertex,_vertex_position) && (_abs(vertex_position(vertex).y - vertex_position(_vertex_position).y) <= epsilon));
}

IC bool	CLevelGraph::inside				(const CLevelGraph::CVertex &vertex, const Fvector &position, const float epsilon) const
{
	return				(inside(vertex,vertex_position(position),epsilon));
}

IC bool	CLevelGraph::inside				(const CVertex *vertex, const CLevelGraph::CPosition &vertex_position, const float epsilon) const
{
	return				(inside(*vertex,vertex_position,epsilon));
}

IC bool CLevelGraph::inside				(const CVertex *vertex, const Fvector &position, const float epsilon) const
{
	return				(inside(*vertex,position,epsilon));
}

IC bool	CLevelGraph::inside				(const u32 vertex_id, const CLevelGraph::CPosition &vertex_position, const float epsilon) const
{
	return				(inside(vertex(vertex_id),vertex_position,epsilon));
}

IC bool CLevelGraph::inside				(const u32 vertex_id, const Fvector &position, const float epsilon) const
{
	return				(inside(vertex(vertex_id),position,epsilon));
}

IC bool	CLevelGraph::inside				(const u32 vertex_id,	const Fvector2 &position) const
{
	TIMER_START			(Inside)
	float				sp = 1/header().cell_size();
	int					pxz	= iFloor(((position.x - header().box().min.x)*sp + EPS_S + .5f))*m_row_length + iFloor((position.y - header().box().min.z)*sp + EPS_S + .5f);
	clamp				(pxz,0,(1 << 21) - 1);
	bool				b = vertex(vertex_id)->position().xz() == u32(pxz);
	TIMER_STOP			(Inside)
	return				(b);
}

IC float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex &vertex, const float X, const float Z) const
{
	TIMER_START			(VertexPlaneY)
	Fvector				DUP, normal, v, v1, P;
	Fplane				PL; 

	DUP.set				(0,1,0);
	pvDecompress		(normal,vertex.plane());
	vertex_position		(P,vertex.position());
	PL.build			(P,normal);
	v.set				(X,P.y,Z);	
	PL.intersectRayPoint(v,DUP,v1);	
	TIMER_STOP			(VertexPlaneY)
	return				(v1.y);
}

IC float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex &vertex) const
{
	float				x,z;
	unpack_xz			(vertex,x,z);
	return				(vertex_plane_y(vertex,x,z));
}

IC float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex *vertex, const float X, const float Z) const
{
	return				(vertex_plane_y(*vertex,X,Z));
}

IC float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex *vertex) const
{
	return				(vertex_plane_y(*vertex));
}

IC float CLevelGraph::vertex_plane_y	(const u32 vertex_id, const float X, const float Z) const
{
	return				(vertex_plane_y(vertex(vertex_id),X,Z));
}

IC float CLevelGraph::vertex_plane_y	(const u32 vertex_id) const
{
	return				(vertex_plane_y(vertex(vertex_id)));
}

IC	u32	CLevelGraph::CHeader::version() const
{
	return				(hdrNODES::version);
}

IC	u32	CLevelGraph::CHeader::vertex_count() const
{
	return				(count);
}

IC	float	CLevelGraph::CHeader::cell_size() const
{
	return				(size);
}

IC	float	CLevelGraph::CHeader::factor_y() const
{
	return				(size_y);
}

IC	const Fbox &CLevelGraph::CHeader::box() const
{
	return				(aabb);
}

IC	u8	CLevelGraph::CVertex::light() const
{
//	return				(NodeCompressed::light());
	return				(data[10] >> 4);
}

IC	u32	CLevelGraph::CVertex::link(int index) const
{
	//	return		(NodeCompressed::link(u8(i)));
	switch (index) {
			case 0 :	return	((*(u32*)data) & 0x001fffff);
			case 1 :	return	(((*(u32*)(data + 2)) >> 5) & 0x001fffff);
			case 2 :	return	(((*(u32*)(data + 5)) >> 2) & 0x001fffff);
			case 3 :	return	(((*(u32*)(data + 7)) >> 7) & 0x001fffff);
			default :	NODEFAULT;
	}
#ifdef DEBUG
	return			(0);
#endif
}

IC	u8	CLevelGraph::CVertex::cover() const
{
	return				(NodeCompressed::cover);
}

IC	u16	CLevelGraph::CVertex::plane() const
{
	return				(NodeCompressed::plane);
}

IC	const CLevelGraph::CPosition &CLevelGraph::CVertex::position() const
{
	return				(p);
}

IC	u32	CLevelGraph::level_id() const
{
	return				(m_level_id);
}

IC	void CLevelGraph::set_level_id(u32 level_id)
{
	m_level_id			= level_id;
}

IC	void CLevelGraph::begin(const CVertex &/**vertex/**/, const_iterator &begin, const_iterator &end) const
{
	begin				= 0;
	end					= 4;
}

IC	void CLevelGraph::begin(const CVertex *_vertex, const_iterator &_begin, const_iterator &end) const
{
	begin				(*_vertex,_begin,end);
}

IC	void CLevelGraph::begin(const u32 vertex_id, const_iterator &_begin, const_iterator &end) const
{
	begin				(vertex(vertex_id),_begin,end);
}

IC	u32	CLevelGraph::value(const CVertex &vertex, const_iterator &i) const
{
	return				(vertex.link(i));
}

IC	u32	CLevelGraph::value(const CVertex *vertex, const_iterator &i) const
{
	return				(value(*vertex,i));
}

IC	u32	CLevelGraph::value(const u32 vertex_id, const_iterator &i) const
{
	return				(value(vertex(vertex_id),i));
}

IC	bool CLevelGraph::is_accessible		(const u32 vertex_id) const
{
	VERIFY				(valid_vertex_id(vertex_id));
	return				(!m_ref_counts[vertex_id]);
}

IC	void CLevelGraph::set_invalid_vertex(u32 &vertex_id, CVertex **vertex) const
{
	vertex_id			= u32(-1);
	VERIFY				(!valid_vertex_id(vertex_id));
	if (vertex)
		*vertex			= NULL;
}

IC	const u32 CLevelGraph::vertex_id(const CLevelGraph::CVertex *vertex) const
{
	VERIFY		(valid_vertex_id(u32(vertex - m_nodes)));
	return		(u32(vertex - m_nodes));
}
