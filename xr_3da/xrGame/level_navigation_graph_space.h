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

	struct CSector : public IPureSerializeObject<IReader,IWriter> {
		u32				min_vertex_id;
		u32				max_vertex_id;

		IC				CSector		() {}
		
		virtual void	save		(IWriter &stream)
		{
			stream.w	(&min_vertex_id,sizeof(min_vertex_id));
			stream.w	(&max_vertex_id,sizeof(max_vertex_id));
		}
		
		virtual void	load		(IReader &stream)
		{
			stream.r	(&min_vertex_id,sizeof(min_vertex_id));
			stream.r	(&max_vertex_id,sizeof(max_vertex_id));
		}

		IC		bool	operator==	(const CSector &obj) const
		{
			return		(
				(min_vertex_id == obj.min_vertex_id) &&
				(max_vertex_id == obj.max_vertex_id)
			);
		}
	};
};