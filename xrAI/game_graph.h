////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph.h
//	Created 	: 18.02.2003
//  Modified 	: 13.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game graph class
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef AI_COMPILER
	#include "xrLevel.h"
#else
	#include "../xrLevel.h"
#endif

#include "ai_alife_space.h"

#define GRAPH_NAME							"game_test.graph"

class CGameGraph {
public:
#pragma pack(push,4)
	class CEdge {
		u32							dwVertexNumber;
		float						fPathDistance;
	public:
		IC	ALife::_GRAPH_ID		vertex_id			() const;
		IC	float					distance			() const;
#ifdef AI_COMPILER
		friend class CLevelGameGraph;
		friend class CGraphThread;
		friend class CGraphMerger;
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
		IC	ALife::_LEVEL_ID		level_id			() const;
		IC	u32						level_vertex_id		() const;
		IC	const u8				*vertex_type		() const;
		IC	ALife::_GRAPH_ID		edge_count			() const;
		IC	u32						edge_offset			() const;
		IC	u32						death_point_count	() const;
		IC	u32						death_point_offset	() const;
		friend class CGameGraph;
#ifdef AI_COMPILER
		friend class CLevelGameGraph;
		friend class CGraphSaver;
#endif
	};

	class CHeader {
		u32							dwVersion;
		u32							dwLevelCount;
		u32							dwVertexCount;
		u32							dwEdgeCount;
		u32							dwDeathPointCount;
		ALife::LEVEL_MAP			tpLevels;

	public:
		IC	u32						version				() const;
		IC	ALife::_LEVEL_ID		level_count			() const;
		IC	ALife::_GRAPH_ID		vertex_count		() const;
		IC	ALife::_GRAPH_ID		edge_count			() const;
		IC	u32						death_point_count	() const;
		IC	const ALife::LEVEL_MAP	&levels				() const;
		friend class CGameGraph;
#ifdef AI_COMPILER
		friend class CGraphSaver;
		friend class CGraphMerger;
#endif
	};
#pragma pack(pop)

	typedef const CEdge				*const_iterator;
#ifndef AI_COMPILER
	typedef const ALife::CLevelPoint*const_spawn_iterator;
#else
	typedef const ALife::SLevelPoint*const_spawn_iterator;
#endif

protected:
	CHeader							m_tGraphHeader;	// graph header
	IReader							*m_tpGraphVFS;	// virtual file
	CVertex							*m_tpaGraph;

public:
#ifndef AI_COMPILER
	IC 								CGameGraph		();
#else
	IC 								CGameGraph		(LPCSTR file_name);
#endif
	IC virtual						~CGameGraph		();
	IC const CHeader				&header			() const;
	IC		bool					mask			(svector<ALife::_LOCATION_ID,LOCATION_TYPE_COUNT> &M, const ALife::_LOCATION_ID E[LOCATION_TYPE_COUNT]) const;
	IC		bool					mask			(const ALife::_LOCATION_ID M[LOCATION_TYPE_COUNT], const ALife::_LOCATION_ID E[LOCATION_TYPE_COUNT]) const;
	IC		float					distance		(const ALife::_GRAPH_ID tGraphID0, const ALife::_GRAPH_ID tGraphID1) const;
	IC		bool					accessible		(const u32 vertex_id) const;
	IC		bool					valid_vertex_id	(const u32 vertex_id) const;
	IC		void					begin			(const u32 vertex_id, const_iterator &start, const_iterator &end) const;
	IC		void					begin_spawn		(const u32 vertex_id, const_spawn_iterator &start, const_spawn_iterator &end) const;
	IC		u32						value			(const u32 vertex_id, const_iterator &i) const;
	IC		u32						node_count		() const;
	IC		float					edge_weight		(const_iterator i) const;
	IC		const CVertex			&vertex			(u32 vertex_id) const;
	IC		void					set_invalid_vertex(u32 &vertex_id) const;
};

#include "game_graph_inline.h"