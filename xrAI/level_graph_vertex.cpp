////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_vertex.cpp
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph vertex functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_graph.h"
#include "game_level_cross_table.h"
#ifndef AI_COMPILER
#include "ai_space.h"
#endif

#pragma todo("Jim to Dima: CAbstractVertexEvaluator uses large amount of memory. Currently 372 bytes!!!")

float CLevelGraph::distance(const Fvector &position, const CLevelGraph::CVertex *vertex) const
{
	SContour				_contour;
	contour					(_contour,vertex);

	// calculate minimal distance
	float					best,dist;
	best					= distance(position,_contour.v1,_contour.v2);
	dist					= distance(position,_contour.v2,_contour.v3);
	if (dist < best)
		best				= dist;

	dist					= distance(position,_contour.v3,_contour.v4);
	if (dist < best)
		best				= dist;

	dist					= distance(position,_contour.v4,_contour.v1);
	if (dist < best)
		best				= dist;

	return					(best);
}

void CLevelGraph::compute_circle(const Fvector &position, const Fvector &point0, const Fvector &point1, float &radius, Fvector &center, Fvector &final_position, float &beta) const
{
	Fvector			tP0, tP1;
	float			alpha, fSinus, fCosinus, fRx;

	tP0.sub			(position,point0);
	tP1.sub			(point1,point0);
	fRx				= tP0.magnitude();
	tP0.normalize	();
	tP1.normalize	();

	clamp			(alpha = tP0.dotproduct(tP1),-0.9999999f,0.9999999f);
	alpha			= .5f*(beta = acosf(alpha));
	beta			= PI - beta;

	tP0.mul			(fRx);
	tP1.mul			(fRx);

	final_position	= tP1;
	final_position.add(point0);

	center.add		(tP0,tP1);
	fSinus			= _sin(alpha);
	fCosinus		= _cos(alpha);
	radius			= fRx*fSinus/fCosinus;
	fRx				= radius*(1.f/fSinus - 1.f);
	center.mul		((radius + fRx)/center.magnitude());
	center.add		(point0);
}

void CLevelGraph::choose_point(const Fvector &start_point, const Fvector &finish_point, const SContour &_contour, int node_id, Fvector &temp_point, int &saved_index) const
{
	SContour					tNextContour;
	SSegment					tNextSegment;
	Fvector						tCheckPoint1 = start_point, tCheckPoint2 = start_point, tIntersectPoint;
	contour						(tNextContour,node_id);
	intersect					(tNextSegment,tNextContour,_contour);
	u32							dwIntersect = intersect(start_point.x,start_point.z,finish_point.x,finish_point.z,tNextSegment.v1.x,tNextSegment.v1.z,tNextSegment.v2.x,tNextSegment.v2.z,&tIntersectPoint.x,&tIntersectPoint.z);
	if (!dwIntersect)
		return;
	for (int i=0; i<4; ++i) {
		switch (i) {
			case 0 : {
				tCheckPoint1	= tNextContour.v1;
				tCheckPoint2	= tNextContour.v2;
				break;
					 }
			case 1 : {
				tCheckPoint1	= tNextContour.v2;
				tCheckPoint2	= tNextContour.v3;
				break;
					 }
			case 2 : {
				tCheckPoint1	= tNextContour.v3;
				tCheckPoint2	= tNextContour.v4;
				break;
					 }
			case 3 : {
				tCheckPoint1	= tNextContour.v4;
				tCheckPoint2	= tNextContour.v1;
				break;
					 }
			default : NODEFAULT;
		}
		dwIntersect				= intersect(start_point.x,start_point.z,finish_point.x,finish_point.z,tCheckPoint1.x,tCheckPoint1.z,tCheckPoint2.x,tCheckPoint2.z,&tIntersectPoint.x,&tIntersectPoint.z);
		if (dwIntersect == eLineIntersectionIntersect) {
			if (finish_point.distance_to_xz(tIntersectPoint) < finish_point.distance_to_xz(temp_point) + EPS_L) {
				temp_point = tIntersectPoint;
				saved_index = node_id;
			}
		}
		else
			if (dwIntersect == eLineIntersectionEqual) {
				if (start_point.distance_to_xz(tCheckPoint1) > start_point.distance_to_xz(temp_point))
					if (start_point.distance_to_xz(tCheckPoint1) > start_point.distance_to_xz(tCheckPoint2)) {
						temp_point = tCheckPoint1;
						saved_index = node_id;
					}
					else {
						temp_point = tCheckPoint2;
						saved_index = node_id;
					}
				else
					if (start_point.distance_to_xz(tCheckPoint2) > start_point.distance_to_xz(temp_point)) {
						temp_point = tCheckPoint2;
						saved_index = node_id;
					}

			}
	}
}

bool CLevelGraph::check_vertex_in_direction(u32 start_vertex_id, const Fvector &start_position, u32 finish_vertex_id) const
{
	SContour				_contour;
	const_iterator			I,E;
	int						saved_index, iPrevIndex = -1, iNextNode;
	Fvector					start_point = start_position, temp_point = start_position, finish_point = vertex_position(finish_vertex_id);;
	float					fCurDistance = 0.f, fDistance = start_position.distance_to_xz(vertex_position(finish_vertex_id));
	u32						dwCurNode = start_vertex_id;

	while ((dwCurNode != finish_vertex_id) && (fCurDistance < (fDistance + EPS_L))) {
		if (inside(vertex(dwCurNode),finish_point))
			break;
		begin				(dwCurNode,I,E);
		saved_index			= -1;
		contour				(_contour,dwCurNode);
		for ( ; I != E; ++I) {
			iNextNode = value(dwCurNode,I);
			if (valid_vertex_id(iNextNode) && (iPrevIndex != iNextNode))
				choose_point(start_point,finish_point,_contour, iNextNode,temp_point,saved_index);
		}

		if (saved_index > -1) {
			fCurDistance	= start_point.distance_to_xz(temp_point);
			iPrevIndex		= dwCurNode;
			dwCurNode		= saved_index;
		}
		else
			return(false);
	}
	return(dwCurNode == finish_vertex_id);
}

u32 CLevelGraph::check_position_in_direction(u32 start_vertex_id, const Fvector &start_position, const Fvector &finish_position) const
{
	SContour				_contour;
	const_iterator			I,E;
	int						saved_index, iPrevIndex = -1, iNextNode;
	Fvector					start_point = start_position, temp_point = start_position, finish_point = finish_position;
	float					fCurDistance = 0.f, fDistance = start_position.distance_to_xz(finish_position);
	u32						dwCurNode = start_vertex_id;

	while (!inside(vertex(dwCurNode),finish_position) && (fCurDistance < (fDistance + EPS_L))) {
		begin				(dwCurNode,I,E);
		saved_index			= -1;
		contour				(_contour,dwCurNode);
		for ( ; I != E; ++I) {
			iNextNode = value(dwCurNode,I);
			if (valid_vertex_id(iNextNode) && (iPrevIndex != iNextNode))
				choose_point(start_point,finish_point,_contour, iNextNode,temp_point,saved_index);
		}

		if (saved_index > -1) {
			fCurDistance	= start_point.distance_to_xz(temp_point);
			iPrevIndex		= dwCurNode;
			dwCurNode		= saved_index;
		}
		else
			return(u32(-1));
	}

	if (inside(vertex(dwCurNode),finish_position) && (_abs(vertex_plane_y(*vertex(dwCurNode),finish_position.x,finish_position.z) - finish_position.y) < .5f))
		return(dwCurNode);
	else
		return((u32)(-1));
}

float CLevelGraph::check_position_in_direction(u32 start_vertex_id, const Fvector &start_position, const Fvector &finish_position, const float max_distance) const
{
	SContour				_contour;
	const_iterator			I,E;
	int						saved_index, iPrevIndex = -1, iNextNode;
	Fvector					start_point = start_position, temp_point = start_position, finish_point = finish_position;
	float					fCurDistance = 0.f, fDistance = start_position.distance_to_xz(finish_position);
	u32						dwCurNode = start_vertex_id;

	while (!inside(vertex(dwCurNode),finish_position) && (fCurDistance < (fDistance + EPS_L))) {
		begin				(dwCurNode,I,E);
		saved_index			= -1;
		contour				(_contour,dwCurNode);
		for ( ; I != E; ++I) {
			iNextNode = value(dwCurNode,I);
			if (valid_vertex_id(iNextNode) && (iPrevIndex != iNextNode))
				choose_point(start_point,finish_point,_contour, iNextNode,temp_point,saved_index);
		}

		if (saved_index > -1) {
			fCurDistance	= start_point.distance_to_xz(temp_point);
			iPrevIndex		= dwCurNode;
			dwCurNode		= saved_index;
		}
		else
			return			(max_distance);
	}

	if (inside(vertex(dwCurNode),finish_position) && (_abs(vertex_plane_y(*vertex(dwCurNode),finish_position.x,finish_position.z) - finish_position.y) < .5f))
		return				(start_point.distance_to_xz(finish_position));
	else
		return				(max_distance);
}

float CLevelGraph::mark_nodes_in_direction(u32 start_vertex_id, const Fvector &start_position, const Fvector &tDirection, float fDistance, xr_vector<u32> &tpaStack, xr_vector<bool> *tpaMarks) const
{
	Fvector					finish_point, direction = tDirection;
	direction.normalize		();
	finish_point.mul		(direction,fDistance);
	finish_point.add		(start_position);
	return					(mark_nodes_in_direction(start_vertex_id,start_position,finish_point,tpaStack,tpaMarks));
}

float CLevelGraph::mark_nodes_in_direction(u32 start_vertex_id, const Fvector &start_position, u32 finish_vertex_id, xr_vector<u32> &tpaStack, xr_vector<bool> *tpaMarks) const
{
	return					(mark_nodes_in_direction(start_vertex_id,start_position,vertex_position(finish_vertex_id),tpaStack,tpaMarks));
}

float CLevelGraph::mark_nodes_in_direction(u32 start_vertex_id, const Fvector &start_point, const Fvector &finish_point, xr_vector<u32> &tpaStack, xr_vector<bool> *tpaMarks) const
{
	SContour				_contour;
	const_iterator			I,E;
	int						saved_index, iPrevIndex = -1, iNextNode;
	Fvector					temp_point = start_point;
	float					fDistance = start_point.distance_to(finish_point), fCurDistance = 0.f;
	u32						dwCurNode = start_vertex_id;

	while (!inside(vertex(dwCurNode),finish_point) && (fCurDistance < (fDistance + EPS_L))) {
		begin				(dwCurNode,I,E);
		saved_index			= -1;
		contour				(_contour,dwCurNode);
		for ( ; I != E; ++I) {
			iNextNode = value(dwCurNode,I);
			if (valid_vertex_id(iNextNode) && (iPrevIndex != iNextNode))
				choose_point(start_point,finish_point,_contour, iNextNode,temp_point,saved_index);
		}

		if (saved_index > -1) {
			fCurDistance	= start_point.distance_to_xz(temp_point);
			iPrevIndex		= dwCurNode;
			dwCurNode		= saved_index;
		}
		else
			return(fCurDistance);

		if (tpaMarks)
			(*tpaMarks)[dwCurNode]	= true;
		tpaStack.push_back	(dwCurNode);
	}

	return					(fCurDistance);
}

float CLevelGraph::find_farthest_node_in_direction(u32 start_vertex_id, const Fvector &start_point, const Fvector &finish_point, u32 &finish_vertex_id, xr_vector<bool> *tpaMarks) const
{
	SContour				_contour;
	const_iterator			I,E;
	int						saved_index, iPrevIndex = -1, iNextNode;
	Fvector					temp_point = start_point;
	float					fDistance = start_point.distance_to(finish_point), fCurDistance = 0.f;
	u32						dwCurNode = start_vertex_id;

	while (!inside(vertex(dwCurNode),finish_point) && (fCurDistance < (fDistance + EPS_L))) {
		begin				(dwCurNode,I,E);
		saved_index			= -1;
		contour				(_contour,dwCurNode);
		for ( ; I != E; ++I) {
			iNextNode = value(dwCurNode,I);
			if (valid_vertex_id(iNextNode) && (iPrevIndex != iNextNode))
				choose_point(start_point,finish_point,_contour, iNextNode,temp_point,saved_index);
		}

		if (saved_index > -1) {
			fCurDistance	= start_point.distance_to_xz(temp_point);
			iPrevIndex		= dwCurNode;
			dwCurNode		= saved_index;
		}
		else
			return(fCurDistance);

		if (tpaMarks)
			(*tpaMarks)[dwCurNode]	= true;
		finish_vertex_id		= dwCurNode;
	}
	return					(fCurDistance);
}

bool CLevelGraph::create_straight_PTN_path(u32 start_vertex_id, const Fvector &start_point, const Fvector &finish_point, xr_vector<Fvector> &tpaOutputPoints, xr_vector<u32> &tpaOutputNodes, bool bAddFirstPoint, bool bClearPath) const
{
	SContour				_contour;
	const_iterator			I,E;
	int						saved_index, iPrevIndex = -1, iNextNode;
	Fvector					temp_point = start_point;
	float					fDistance = start_point.distance_to(finish_point), fCurDistance = 0.f;
	u32						dwCurNode = start_vertex_id;

	if (bClearPath) {
		tpaOutputPoints.clear	();
		tpaOutputNodes.clear	();
	}
	if (bAddFirstPoint) {
		tpaOutputPoints.push_back(start_point);
		tpaOutputNodes.push_back(start_vertex_id);
	}

	while (!inside(vertex(dwCurNode),finish_point) && (fCurDistance < (fDistance + EPS_L))) {
		begin				(dwCurNode,I,E);
		saved_index			= -1;
		contour				(_contour,dwCurNode);
		for ( ; I != E; ++I) {
			iNextNode = value(dwCurNode,I);
			if (valid_vertex_id(iNextNode) && (iPrevIndex != iNextNode))
				choose_point(start_point,finish_point,_contour, iNextNode,temp_point,saved_index);
		}

		if (saved_index > -1) {
			fCurDistance		= start_point.distance_to_xz(temp_point);
			SContour			tNextContour;
			SSegment			tNextSegment;
			Fvector				tIntersectPoint;
			contour				(tNextContour,saved_index);
			intersect			(tNextSegment,tNextContour,_contour);
			u32					dwIntersect = intersect(start_point.x,start_point.z,finish_point.x,finish_point.z,tNextSegment.v1.x,tNextSegment.v1.z,tNextSegment.v2.x,tNextSegment.v2.z,&tIntersectPoint.x,&tIntersectPoint.z);
			VERIFY				(dwIntersect);
			tIntersectPoint.y	= vertex_plane_y(vertex(dwCurNode),tIntersectPoint.x,tIntersectPoint.z);

			tpaOutputPoints.push_back(tIntersectPoint);
			tpaOutputNodes.push_back(saved_index);

			iPrevIndex			= dwCurNode;
			dwCurNode			= saved_index;
		}
		else {
//			int					node_id;
//			begin				(dwCurNode,I,E);
//			bool				bOk = false;
//			for ( ; I != E; ++I) {
//				node_id			= value(dwCurNode,I);
//				if (!valid_vertex_id(node_id))
//					continue;
//				CLevelGraph::CVertex *tpLastNode = vertex(node_id);
//				if (inside(tpLastNode,finish_point)) {
//					SContour			tNextContour;
//					SSegment			tNextSegment;
//					contour				(tNextContour,node_id);
//					intersect			(tNextSegment,tNextContour,_contour);
//					Fvector				tAdditionalPoint = finish_point.distance_to_xz(tNextSegment.v1) < finish_point.distance_to_xz(tNextSegment.v2) ? tNextSegment.v1 : tNextSegment.v2;
//					tAdditionalPoint.y	= vertex_plane_y(vertex(dwCurNode),tAdditionalPoint.x,tAdditionalPoint.z);
//
//					tpaOutputPoints.push_back(tAdditionalPoint);
//					tpaOutputNodes.push_back(node_id);
//
//					fCurDistance		= fDistance;
//					dwCurNode			= node_id;
//					bOk					= true;
//					break;
//				}
//			}
//			if (!bOk)
				return(false);
		}
	}

	if (inside(vertex(dwCurNode),finish_point)) {
		tpaOutputPoints.push_back(finish_point);
		tpaOutputPoints.back().y = vertex_plane_y(dwCurNode,finish_point.x,finish_point.z);
		tpaOutputNodes.push_back(dwCurNode);
		return(true);
	}
	else
		return(false);
}

#ifndef AI_COMPILER
void CLevelGraph::find_game_point_in_direction(u32 start_vertex_id, const Fvector &start_point, const Fvector &tDirection, u32 &finish_vertex_id, _GRAPH_ID tGraphID) const
{
	SContour				_contour;
	const_iterator			I, E;
	int						saved_index, iPrevIndex = -1, iNextNode;
	Fvector					finish_point = start_point, temp_point = start_point, direction = tDirection;
	u32						dwCurNode = start_vertex_id;
	direction.mul			(2000.f);
	finish_point.add		(direction);
	float					fCurDistance = 0.f;

	for (;;) {
		begin				(dwCurNode,I,E);
		saved_index			= -1;
		contour				(_contour,dwCurNode);
		for ( ; I != E; ++I) {
			iNextNode = value(dwCurNode,I);
			if (valid_vertex_id(iNextNode) && (iPrevIndex != iNextNode) && (ai().cross_table().vertex(iNextNode).game_vertex_id() == tGraphID))
				choose_point(start_point,finish_point,_contour, iNextNode,temp_point,saved_index);
		}

		if (saved_index > -1) {
			fCurDistance	= start_point.distance_to_xz(temp_point);
			iPrevIndex		= dwCurNode;
			dwCurNode		= saved_index;
		}
		else
			return;

		finish_vertex_id		= dwCurNode;
	}
}
#endif