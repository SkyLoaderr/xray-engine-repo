////////////////////////////////////////////////////////////////////////////
//	Module 		: level_navigation_graph_space.h
//	Created 	: 02.10.2001
//  Modified 	: 27.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Level navigation graph space
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"

namespace LevelNavigationGraph {
	struct CCellVertex {
		u32				m_vertex_id;
#if 0
		u32				m_down;
		u32				m_right;
#endif
		union {
			typedef u8	_use_type;

			struct {
				u32		m_mark : 24;
				u32		m_use  : 8;
			};
			u32			m_data;
		};

		CCellVertex		()
		{
		}

		CCellVertex		(u32 vertex_id, u32 mark, u32 use) :
			m_vertex_id	(vertex_id),
			m_mark		(mark),
			m_use		(use)
		{
		}
	};

	class CSector : public IPureSerializeObject<IReader,IWriter> {
	private:
		typedef u32		vertex_id_type;
	private:
		vertex_id_type	m_min_vertex_id;
		vertex_id_type	m_max_vertex_id;

	public:
		IC				CSector		() {}
		
		IC				CSector		(const vertex_id_type &min_vertex_id, const vertex_id_type &max_vertex_id)
		{
			m_min_vertex_id	= min_vertex_id;
			m_max_vertex_id	= max_vertex_id;
		}

		virtual void	save		(IWriter &stream)
		{
			stream.w	(&m_min_vertex_id,sizeof(m_min_vertex_id));
			stream.w	(&m_max_vertex_id,sizeof(m_max_vertex_id));
		}
		
		virtual void	load		(IReader &stream)
		{
			stream.r	(&m_min_vertex_id,sizeof(m_min_vertex_id));
			stream.r	(&m_max_vertex_id,sizeof(m_max_vertex_id));
		}

		IC		bool	operator==	(const CSector &obj) const
		{
			return		(
				(min_vertex_id() == obj.min_vertex_id()) &&
				(max_vertex_id() == obj.max_vertex_id())
			);
		}

		IC		const vertex_id_type &min_vertex_id	() const
		{
			return		(m_min_vertex_id);
		}

		IC		const vertex_id_type &max_vertex_id	() const
		{
			return		(m_max_vertex_id);
		}
	};
};