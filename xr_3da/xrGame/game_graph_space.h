////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_space.h
//	Created 	: 18.02.2003
//  Modified 	: 11.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Game graph namespace
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef AI_COMPILER
#	include "xrLevel.h"
#else
#	include "../xrLevel.h"
#endif

namespace GameGraph {
	typedef u16	_GRAPH_ID;
	typedef u32 _LEVEL_ID;
	typedef u8	_LOCATION_ID;

	enum {
		LOCATION_TYPE_COUNT = 4,
		LOCATION_COUNT		= (u32(1) << (8*sizeof(_LOCATION_ID))),
	};

	class SLevel {
		shared_str				caLevelName;
		Fvector					tOffset;
		_LEVEL_ID				tLevelID;
		shared_str				m_section;

	public:
		IC shared_str name() const {
			return				caLevelName;
		}

		IC const Fvector &offset() const {
			return				(tOffset);
		}

		IC _LEVEL_ID id() const {
			return				(tLevelID);
		}

		IC shared_str section() const {
			return				(m_section);
		}

		IC void load(IReader *reader);

		friend class CGameGraph;
#ifdef AI_COMPILER
		friend class CGraphSaver;
		friend class CGraphMerger;
		friend class CSpawn;
		friend class CSpawnMerger;
		friend class CRenumbererConverter;
		friend class CLevelSpawnConstructor;
		friend class CGameSpawnConstructor;
#endif
	};

	typedef xr_map<_LEVEL_ID,SLevel>LEVEL_MAP;

#pragma pack(push,4)
	class CEdge {
		u32							dwVertexNumber;
		float						fPathDistance;
	public:
		IC	_GRAPH_ID				vertex_id			() const;
		IC	float					distance			() const;
#ifdef AI_COMPILER
		friend class CLevelGameGraph;
		friend class CGraphThread;
		friend class CGraphMerger;
		friend class CRenumbererConverter;
#endif
	};

	class CVertex {
		Fvector						tLocalPoint;
		Fvector						tGlobalPoint;
		u32							tLevelID:8;
		u32							tNodeID:24;
		u8							tVertexTypes[LOCATION_TYPE_COUNT];
		u32							tNeighbourCount:8;
		u32							dwEdgeOffset:24;
		u32							tDeathPointCount:8;
		u32							dwPointOffset:24;
	public:
		IC	const Fvector			&level_point		() const;
		IC	const Fvector			&game_point			() const;
		IC	_LEVEL_ID				level_id			() const;
		IC	u32						level_vertex_id		() const;
		IC	const u8				*vertex_type		() const;
		IC	_GRAPH_ID				edge_count			() const;
		IC	u32						edge_offset			() const;
		IC	u32						death_point_count	() const;
		IC	u32						death_point_offset	() const;
		friend class CGameGraph;
#ifdef AI_COMPILER
		friend class CLevelGameGraph;
		friend class CGraphSaver;
		friend class CRenumbererConverter;
#endif
	};

	class CHeader {
		u32							dwVersion;
		u32							dwLevelCount;
		u32							dwVertexCount;
		u32							dwEdgeCount;
		u32							dwDeathPointCount;
		LEVEL_MAP					tpLevels;

	public:
		IC	u32						version				() const;
		IC	_LEVEL_ID				level_count			() const;
		IC	_GRAPH_ID				vertex_count		() const;
		IC	_GRAPH_ID				edge_count			() const;
		IC	u32						death_point_count	() const;
		IC	const LEVEL_MAP			&levels				() const;
		IC	const SLevel			&level				(const _LEVEL_ID &id) const;
		IC	const SLevel			&level				(LPCSTR level_name) const;
		IC	void					load				(IReader *reader);
		friend class CGameGraph;
#ifdef AI_COMPILER
		friend class CGraphSaver;
		friend class CGraphMerger;
		friend class CRenumbererConverter;
#endif
	};
#pragma pack(pop)

	class CLevelPoint  {
		Fvector		tPoint;
		u32			tNodeID;
		float		fDistance;	
	public:
		IC const Fvector			&level_point		() const
		{
			return				(tPoint);
		}

		IC u32						level_vertex_id		() const
		{
			return				(tNodeID);
		}

		IC float					distance			() const
		{
			return				(fDistance);
		}

#ifdef AI_COMPILER
		friend class CLevelGameGraph;
		friend class CSpawn;
		friend class CLevelSpawnConstructor;
		friend class CRenumbererConverter;
#endif
	};

	struct STerrainPlace{
		svector<_LOCATION_ID,LOCATION_TYPE_COUNT>	tMask;
		u32											dwMinTime;
		u32											dwMaxTime;
	};

	DEFINE_VECTOR	(STerrainPlace,				TERRAIN_VECTOR,					TERRAIN_IT);
};