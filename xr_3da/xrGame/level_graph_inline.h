////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC const CLevelGraph::CHeader &CLevelGraph::header	() const
{
	return				(m_header);
}

IC bool CLevelGraph::valid_vertex_id	(u32 id) const
{
	return				(id && (int(id) < (int)header().vertex_count()));
}

IC	CLevelGraph::CVertex	*CLevelGraph::vertex(u32 id) const
{
	VERIFY				(valid_vertex_id(id));
	return				(m_nodes_ptr[id]);
}

IC u8	CLevelGraph::ref_add		(u32 id)	
{
	VERIFY				(valid_vertex_id(id));
	m_ref_counts[id]	+= u8(1);	
	return				(m_ref_counts[id]);	
}

IC u8	CLevelGraph::ref_dec		(u32 id)
{
	VERIFY				(valid_vertex_id(id));
	m_ref_counts[id]	-= u8(1);	
	return				(m_ref_counts[id]);	
}

IC void CLevelGraph::unpack_xz(const CLevelGraph::CPosition &vertex_position, int &x, int &z) const
{
	x					= vertex_position.xz / m_row_length;
	z					= vertex_position.xz % m_row_length;
}

IC void CLevelGraph::unpack_xz(const CLevelGraph::CPosition &vertex_position, float &x, float &z) const
{
	int					_x,_z;
	unpack_xz			(vertex_position,_x,_z);
	x					= float(_x)*header().cell_size();
	z 					= float(_z)*header().cell_size();
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
	int					x,z;
	unpack_xz			(source_position,x,z);
	dest_position.x		= float(x)*header().cell_size();
	dest_position.y 	= (float(source_position.y)/65535)*header().factor_y() + header().box().min.y;
	dest_position.z 	= float(z)*header().cell_size();
	return				(dest_position);
}

IC const Fvector &CLevelGraph::vertex_position	(Fvector &dest_position, const CLevelGraph::CPosition &source_position) const
{
	return				(dest_position = vertex_position(source_position));
}

IC const CLevelGraph::CPosition &CLevelGraph::vertex_position	(CLevelGraph::CPosition &dest_position, const Fvector &source_position) const
{
	float				sp = 1/header().cell_size();
	int					pxz	= iFloor(((source_position.x - header().box().min.x)*sp + EPS_L + .5f)*m_row_length) + iFloor((source_position.z - header().box().min.z)*sp   + EPS_L + .5f);
	int					py	= iFloor(65535.f*(source_position.y - header().box().min.y)/header().factor_y() + EPS_L);
	clamp				(pxz,0,(1 << 24) - 1);
	dest_position.xz	= u32(pxz);
	clamp				(py,0,65535);
	dest_position.y	= u16(py);
	return				(dest_position);
}

IC  const Fvector CLevelGraph::vertex_position(u32 vertex_id) const
{
	return				(vertex_position(vertex(vertex_id)));
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
	return 				(vertex_position.xz == vertex.position().xz);
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
	return				(inside(vertex(vertex_id),position));
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

IC float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex &vertex, float X, float Z) const
{
	Fvector				DUP, normal, v, v1, P;
	Fplane				PL; 

	DUP.set				(0,1,0);
	pvDecompress		(normal,vertex.plane());
	vertex_position		(P,vertex.position());
	PL.build			(P,normal);
	v.set				(X,P.y,Z);	
	PL.intersectRayPoint(v,DUP,v1);	
	return				(v1.y);
}

IC float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex &vertex) const
{
	float				x,z;
	unpack_xz			(vertex,x,z);
	return				(vertex_plane_y(vertex,x,z));
}

IC float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex *vertex, float X, float Z) const
{
	return				(vertex_plane_y(*vertex,X,Z));
}

IC float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex *vertex) const
{
	return				(vertex_plane_y(*vertex));
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
	return				(NodeCompressed::light);
}

IC	u8	CLevelGraph::CVertex::cover() const
{
	return				(NodeCompressed::cover);
}

IC	u16	CLevelGraph::CVertex::plane() const
{
	return				(NodeCompressed::plane);
}

IC	u32	CLevelGraph::CVertex::link(int i) const
{
	switch (i) {
			case 0 :	return link0;
			case 1 :	return link1;
			case 2 :	return link2;
			case 3 :	return link3;
			default :	NODEFAULT;
	}
	return		(u32(-1));
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

IC	void CLevelGraph::begin(const CVertex &vertex, const_iterator &begin, const_iterator &end) const
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

#ifdef DEBUG
#ifndef AI_COMPILER
IC void CLevelGraph::set_start_point	(const Fvector &start_point)
{
	m_start_point		= start_point;
}

IC void CLevelGraph::set_finish_point	(const Fvector &finish_point)
{
	m_finish_point		= finish_point;
}
#endif
#endif

IC	bool CLevelGraph::is_accessible		(const u32 vertex_id) const
{
	VERIFY				(valid_vertex_id(vertex_id));
	return				(!m_ref_counts[vertex_id]);
}
