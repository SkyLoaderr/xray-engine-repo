////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph.h
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xrLevel.h"
#include "ai_alife_space.h"
#include <math.h>


class CLevelGraph {
public:
	typedef u8				Cover[4];

	class CHeader : private hdrNODES {
	public:
		IC	u32				version					() const;
		IC	u32				vertex_count			() const;
		IC	float			cell_size				() const;
		IC	float			factor_y				() const;
		IC	const Fbox		&box					() const;
	};

	typedef NodePosition CPosition;

	class CVertex : private NodeCompressed {
	public:
		IC	u32				link					(int i) const;
		IC	u8				light					() const;
		IC	u8				cover					() const;
		IC	u16				plane					() const;
		IC	const CPosition &position				() const;
		friend class CLevelGraph;
	};

private:
	enum ELineIntersections {
		eLineIntersectionNone		= u32(0),
		eLineIntersectionCollinear	= u32(0),
		eLineIntersectionIntersect	= u32(1),
		eLineIntersectionEqual		= u32(2)
	};

	struct SSegment
	{
		Fvector v1;
		Fvector v2;
	};

	struct SContour : public SSegment
	{
		Fvector v3;
		Fvector v4;
	};
	
	IReader				*m_reader;		// level graph virtual storage
	CHeader				m_header;		// level graph header
	u32					m_palette_size;
	Cover				*m_cover_palette;
	u8					*m_nodes;		// nodes array
	CVertex				**m_nodes_ptr;	// pointers to vertex's data
	xr_vector<u8>		m_ref_counts;	// reference counters for handling dynamic objects
	u32					m_level_id;		// unique level identifier
	u32					m_row_length;
public:
#ifdef AI_COMPILER
	xr_vector<bool>		q_mark_bit;
#endif
protected:

public:
	typedef u32 const_iterator;
	typedef u32 const_spawn_iterator;
	typedef u32 const_death_iterator;

#ifndef AI_COMPILER
					CLevelGraph					();
#else
					CLevelGraph					(LPCSTR file_name);
#endif
	virtual			~CLevelGraph				();
	IC		u8		ref_add						(u32 vertex_id);
	IC		u8		ref_dec						(u32 vertex_id);
	IC		void	set_level_id				(u32 level_id);
	IC		void	begin						(const CVertex &vertex, const_iterator &begin, const_iterator &end) const;
	IC		void	begin						(const CVertex *vertex, const_iterator &begin, const_iterator &end) const;
	IC		void	begin						(u32 vertex_id,			const_iterator &begin, const_iterator &end) const;
	IC		u32		value						(const CVertex &vertex, const_iterator &i) const;
	IC		u32		value						(const CVertex *vertex, const_iterator &i) const;
	IC		u32		value						(const u32 vertex_id,	const_iterator &i) const;
	IC		bool	is_accessible				(const u32 vertex_id) const;
	IC		const CHeader &header				() const;
	IC		bool	valid_vertex_id				(u32 vertex_id) const;
	IC		u32		level_id					() const;
	IC		u8		*cover						(const CVertex &vertex) const;
	IC		u8		*cover						(const CVertex *vertex) const;
	IC		u8		*cover						(const u32 vertex) const;
	IC		void	unpack_xz					(const CLevelGraph::CPosition &vertex_position, int &x, int &z) const;
	IC		void	unpack_xz					(const CLevelGraph::CPosition &vertex_position, float &x, float &z) const;
	template <typename T>
	IC		void	unpack_xz					(const CLevelGraph::CVertex &vertex, T &x, T &z) const;
	template <typename T>
	IC		void	unpack_xz					(const CLevelGraph::CVertex *vertex, T &x, T &z) const;
	IC		CVertex	*vertex						(u32 vertex_id) const;
	IC		const	Fvector						vertex_position(const CLevelGraph::CPosition &source_position) const;
	IC		const	Fvector						&vertex_position(Fvector &dest_position, const CLevelGraph::CPosition &source_position) const;
	IC		const	CLevelGraph::CPosition		&vertex_position(CLevelGraph::CPosition &dest_position, const Fvector &source_position) const;
	IC		const	CLevelGraph::CPosition		vertex_position	(const Fvector &position) const;
	IC		const	Fvector						vertex_position	(u32 vertex_id) const;
	IC		const	Fvector						vertex_position	(const CVertex &vertex) const;
	IC		const	Fvector						vertex_position	(const CVertex *vertex) const;
	IC		float	vertex_plane_y				(const CVertex &vertex, float X, float Z) const;
	IC		float	vertex_plane_y				(const CVertex *vertex, float X, float Z) const;
	IC		float	vertex_plane_y				(const u32 vertex_id, float X, float Z) const;
	IC		float	vertex_plane_y				(const CVertex &vertex) const;
	IC		float	vertex_plane_y				(const CVertex *vertex) const;
	IC		float	vertex_plane_y				(const u32 vertex_id) const;
	IC		bool	inside						(const CVertex &vertex, const CLevelGraph::CPosition &vertex_position) const;
	IC		bool	inside						(const CVertex &vertex, const Fvector &vertex_position) const;
	IC		bool	inside						(const CVertex *vertex, const CLevelGraph::CPosition &vertex_position) const;
	IC		bool	inside						(const CVertex *vertex, const Fvector &vertex_position) const;
	IC		bool	inside						(const u32 vertex_id,	const CLevelGraph::CPosition &vertex_position) const;
	IC		bool	inside						(const u32 vertex_id,	const Fvector &position) const;
	IC		bool	inside						(const CVertex &vertex, const CLevelGraph::CPosition &vertex_position, const float epsilon) const;
	IC		bool	inside						(const CVertex &vertex, const Fvector &vertex_position, const float epsilon) const;
	IC		bool	inside						(const CVertex *vertex, const CLevelGraph::CPosition &vertex_position, const float epsilon) const;
	IC		bool	inside						(const CVertex *vertex, const Fvector &vertex_position, const float epsilon) const;
	IC		bool	inside						(const u32 vertex_id,	const CLevelGraph::CPosition &vertex_position, const float epsilon) const;
	IC		bool	inside						(const u32 vertex_id,	const Fvector &position, const float epsilon) const;
	IC		void	project_point				(const Fplane &plane,	Fvector &point) const;
			float	distance					(const Fvector &position, const CVertex *vertex) const;
			float	distance					(const Fvector &position, const u32 vertex_id) const;
			float	distance					(const u32 vertex_id, const Fvector &position) const;
	IC		float	distance					(const Fvector &position, const Fvector &point0, const Fvector &point1) const;
	IC		float	distance					(u32 vertex_id0, u32 vertex_id1) const;
	IC		float	distance					(const CVertex *tpNode0, u32 vertex_id1) const;
	IC		float	distance					(u32 vertex_id0, const CVertex *vertex) const;
	IC		float	distance					(const CVertex *node0, const CVertex *node1) const;
	IC		ELineIntersections	intersect		(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float *x, float *y) const;
	IC		bool	similar						(const Fvector &point0, const Fvector &point1) const;
	IC		bool	inside						(const Fvector &point, const SContour &contour) const;
	IC		void	intersect					(SSegment &segment, const SContour &contour0, const SContour &contour1) const;
	IC		float	nearest						(Fvector &destination, const Fvector &position, const Fvector &point0, const Fvector &point1) const;
	IC		void	contour						(SContour &contour, u32 vertex_id) const;
	IC		void	contour						(SContour &contour, const CVertex *vertex) const;
	IC		void	nearest						(Fvector &destination, const Fvector &position, const SContour &contour) const;
	IC		bool	intersect					(Fvector &destination, const Fvector &v1, const Fvector& v2, const Fvector& v3, const Fvector& v4) const;
	IC		float	square						(float a1, float b1, float alpha = PI_DIV_2) const;
	IC		float	compute_square				(float angle, float AOV, float b0, float b1, float b2, float b3) const;
	IC		float	compute_square				(float angle, float AOV, const CVertex *vertex) const;
	IC		float	compute_square				(float angle, float AOV, u32 dwNodeID) const;
	IC		float	cover_in_direction			(float angle, float b0, float b1, float b2, float b3) const;
	IC		float	cover_in_direction			(float angle, const CVertex *vertex) const;
	IC		float	cover_in_direction			(float angle, u32 vertex_id) const;
			u32		vertex						(const Fvector &position) const;
			u32		vertex						(u32 current_vertex_id, const Fvector &position, bool full_search = false) const;
			void	compute_circle				(const Fvector &position,	const Fvector &point0, const Fvector &point1, float &radius, Fvector &center, Fvector &final_position, float &beta) const;
			void	choose_point				(const Fvector &start_point, const Fvector &finish_point, const SContour &contour, int vertex_id, Fvector &temp_point, int &saved_index) const;
			bool	check_vertex_in_direction	(u32 start_vertex_id, const Fvector &start_position, u32 finish_vertex_id) const;
			u32		check_position_in_direction	(u32 start_vertex_id, const Fvector &start_position, const Fvector &finish_position) const;
			float	check_position_in_direction	(u32 start_vertex_id, const Fvector &start_position, const Fvector &finish_position, const float max_distance) const;
			float	mark_nodes_in_direction		(u32 start_vertex_id, const Fvector &start_position, const Fvector &direction, float distance, xr_vector<u32> &vertex_stack, xr_vector<bool> *vertex_marks) const;
			float	mark_nodes_in_direction		(u32 start_vertex_id, const Fvector &start_position, u32 finish_node, xr_vector<u32> &vertex_stack, xr_vector<bool> *vertex_marks) const;
			float	mark_nodes_in_direction		(u32 start_vertex_id, const Fvector &start_position, const Fvector &finish_point, xr_vector<u32> &vertex_stack, xr_vector<bool> *vertex_marks) const;
			float	find_farthest_node_in_direction(u32 start_vertex_id, const Fvector &start_point, const Fvector &finish_point, u32 &finish_vertex_id, xr_vector<bool> *tpaMarks) const;
			bool	create_straight_PTN_path	(u32 start_vertex_id, const Fvector &start_point, const Fvector &finish_point, xr_vector<Fvector> &tpaOutputPoints, xr_vector<u32> &tpaOutputNodes, bool bAddFirstPoint) const;
#ifndef AI_COMPILER
			void	find_game_point_in_direction(u32 start_vertex_id, const Fvector &start_point, const Fvector &tDirection, u32 &finish_vertex_id, ALife::_GRAPH_ID tGraphID) const;
#endif

#ifdef DEBUG
#ifndef AI_COMPILER
private:
	ref_shader			sh_debug;
	Fvector				m_start_point;
	Fvector				m_finish_point;
	xr_vector<Fvector>	m_tpTravelLine;
	xr_vector<Fvector>	m_tpaPoints;
	xr_vector<Fvector>	m_tpaDeviations;
	xr_vector<Fvector>	m_tpaTravelPath;
	xr_vector<u32>		m_tpaPointNodes;
	xr_vector<Fvector>	m_tpaLine;
	xr_vector<u32>		m_tpaNodes;
	xr_vector<Fvector>	m_tpaTempPath;
public:
	xr_vector<BYTE>		q_mark;			// temporal usage mark for queries
	xr_vector<u32>		q_stack;		// temporal usage stack for search enemy queries

			void	render						();
	IC		void	set_start_point				(const Fvector &start_point);
	IC		void	set_finish_point			(const Fvector &finish_point);
			void	draw_oriented_bounding_box	(Fmatrix &T, Fvector &half_dim, u32 C,	u32 C1) const;
			void	draw_travel_line			() const;
			void	compute_travel_line			(xr_vector<u32> &path, u32 start_vertex_id, u32 finish_vertex_id) const;
			void	compute_path				() const;
			void	draw_dynamic_obstacles		() const;
#endif
#endif
};

#include "level_graph_inline.h"
#include "level_graph_vertex_inline.h"