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
	#endif
	#define TIMER_START(a)
	#define TIMER_STOP(a)
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
	VERIFY				(vertex_position.xz() < (1 << MAX_NODE_BIT_COUNT) - 1);
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
	VERIFY				(iFloor((source_position.z - header().box().min.z)/header().cell_size() + .5f) < (int)m_row_length);
	int					pxz	= iFloor(((source_position.x - header().box().min.x)/header().cell_size() + .5f))*m_row_length + iFloor((source_position.z - header().box().min.z)/header().cell_size() + .5f);
	int					py	= iFloor(65535.f*(source_position.y - header().box().min.y)/header().factor_y() + EPS_S);
	VERIFY				(pxz < (1 << MAX_NODE_BIT_COUNT) - 1);
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
	if (!valid_vertex_position(position))
		return			(false);
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
	if (!valid_vertex_position(position))
		return			(false);
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
	int					pxz	= iFloor(((position.x - header().box().min.x)/header().cell_size() + .5f))*m_row_length + iFloor((position.y - header().box().min.z)/header().cell_size() + .5f);
	VERIFY				(pxz < (1 << MAX_NODE_BIT_COUNT) - 1);
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
	return				(NodeCompressed::light());
}

IC	u32	CLevelGraph::CVertex::link(int index) const
{
	return		(NodeCompressed::link(u8(index)));
}

IC	u16	CLevelGraph::CVertex::cover(u8 index) const
{
	return		(NodeCompressed::cover(index));
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

IC  Fvector CLevelGraph::v3d(const Fvector2 &vector2d) const
{
	return			(Fvector().set(vector2d.x,0.f,vector2d.y));
}

IC  Fvector2 CLevelGraph::v2d(const Fvector &vector3d) const
{
	return			(Fvector2().set(vector3d.x,vector3d.z));
}

template <bool bAssignY, typename T>
IC	bool	CLevelGraph::create_straight_PTN_path	(u32 start_vertex_id, const Fvector2 &start_point, const Fvector2 &finish_point, xr_vector<T> &tpaOutputPoints, const T &example, bool bAddFirstPoint, bool bClearPath) const
{
	u32						cur_vertex_id = start_vertex_id, prev_vertex_id = start_vertex_id;
	Fbox2					box;
	Fvector2				identity, start, dest, dir;
	T						path_node = example;

	identity.x = identity.y	= header().cell_size()/2.f;
	start					= start_point;
	dest					= finish_point;
	dir.sub					(dest,start);
	Fvector2				temp;
	Fvector					pos3d;
	unpack_xz				(vertex(start_vertex_id),temp.x,temp.y);

	if (bClearPath)
		tpaOutputPoints.clear	();
	if (bAddFirstPoint) {
		pos3d				= v3d(start_point);
		if (bAssignY)
			pos3d.y			= vertex_plane_y(start_vertex_id,start_point.x,start_point.y);
		path_node.set_position(pos3d);
		path_node.set_vertex_id(start_vertex_id);
		tpaOutputPoints.push_back(path_node);
	}

	float					cur_sqr = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
	for (;;) {
		const_iterator		I,E;
		begin				(cur_vertex_id,I,E);
		bool				found = false;
		for ( ; I != E; ++I) {
			u32				next_vertex_id = value(cur_vertex_id,I);
			if ((next_vertex_id == prev_vertex_id) || !valid_vertex_id(next_vertex_id))
				continue;
			unpack_xz		(vertex(next_vertex_id),temp.x,temp.y);
			box.min			= box.max = temp;
			box.grow		(identity);
			if (box.pick_exact(start,dir)) {
				Fvector2		temp;
				temp.add		(box.min,box.max);
				temp.mul		(.5f);
				float			dist = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
				if (dist > cur_sqr)
					continue;
				cur_sqr			= dist;

				Fvector2		next1, next2;
#ifdef DEBUG
				next1			= next2 = Fvector2().set(0.f,0.f);
#endif
				Fvector			tIntersectPoint;

				switch (I) {
					case 0 : {
						next1		= box.max;
						next2.set	(box.max.x,box.min.y);
						break;
					}
					case 1 : {
						next1		= box.min;
						next2.set	(box.max.x,box.min.y);
						break;
					}
					case 2 : {
						next1		= box.min;
						next2.set	(box.min.x,box.max.y);
						break;
					}
					case 3 : {
						next1		= box.max;
						next2.set	(box.min.x,box.max.y);
						break;
					}
					default : NODEFAULT;
				}
#ifdef DEBUG
				VERIFY			(_valid(next1));
				VERIFY			(_valid(next2));
				u32				dwIntersect = 
#endif
					intersect_no_check(start_point.x,start_point.y,finish_point.x,finish_point.y,next1.x,next1.y,next2.x,next2.y,&tIntersectPoint.x,&tIntersectPoint.z);
#ifdef DEBUG
				VERIFY			(dwIntersect);
				VERIFY			(_valid(tIntersectPoint.x));
				VERIFY			(_valid(tIntersectPoint.z));
#endif

				clamp			(tIntersectPoint.x,_min(next1.x,next2.x),_max(next1.x,next2.x));
				clamp			(tIntersectPoint.z,_min(next1.y,next2.y),_max(next1.y,next2.y));
				if (bAssignY)
					tIntersectPoint.y = vertex_plane_y(vertex(cur_vertex_id),tIntersectPoint.x,tIntersectPoint.z);
				path_node.set_position(tIntersectPoint);
				path_node.set_vertex_id(next_vertex_id);
				tpaOutputPoints.push_back(path_node);

				if (box.contains(dest)) {
					tIntersectPoint = v3d(dest);
					if (bAssignY)
						tIntersectPoint.y = vertex_plane_y(vertex(cur_vertex_id),tIntersectPoint.x,tIntersectPoint.z);
					path_node.set_position(tIntersectPoint);
					path_node.set_vertex_id(next_vertex_id);
					tpaOutputPoints.push_back(path_node);
					return		(true);
				}
				found			= true;
				prev_vertex_id	= cur_vertex_id;
				cur_vertex_id	= next_vertex_id;
				break;
			}
		}
		if (!found)
			return			(false);
	}
}

template<typename T>
IC	void CLevelGraph::assign_y_values		(xr_vector<T> &path)
{
	Fvector						DUP = {0,1,0}, normal, v1, P = {0,0,0};
	Fplane						PL; 
	const CVertex				*_vertex;
	u32							prev_id = u32(-1);

	xr_vector<T>::iterator		I = path.begin();
	xr_vector<T>::iterator		E = path.end();
	for ( ; I != E; ++I) {
		if (prev_id != (*I).get_vertex_id()) {
			_vertex				= vertex((*I).get_vertex_id());
			pvDecompress		(normal,_vertex->plane());
			vertex_position		(P,_vertex->position());
			PL.build			(P,normal);
			prev_id				= (*I).get_vertex_id();
		}
		(*I).get_position().y	= P.y;
		PL.intersectRayPoint	((*I).get_position(),DUP,v1);	
		(*I).get_position().y	= v1.y;
	}
}

IC	u32	CLevelGraph::row_length				() const
{
	return						(m_row_length);
}

IC	bool CLevelGraph::valid_vertex_position	(const Fvector &position) const
{
	if ((position.x < header().box().min.x - header().cell_size()*.5f) || (position.x > header().box().max.x + header().cell_size()*.5f) || (position.z < header().box().min.z - header().cell_size()*.5f) || (position.z > header().box().max.z + header().cell_size()*.5f))
		return			(false);

	if (!(iFloor((position.z - header().box().min.z)/header().cell_size() + .5f) < (int)m_row_length))
		return			(false);
	
	if (!(iFloor((position.x - header().box().min.x)/header().cell_size() + .5f) < (int)m_column_length))
		return			(false);

	return				((vertex_position(position).xz() < (1 << MAX_NODE_BIT_COUNT) - 1));
}
