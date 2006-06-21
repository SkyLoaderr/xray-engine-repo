////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_debug2.cpp
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph debug functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef DEBUG
#ifndef AI_COMPILER

#include "level_graph.h"
#include "../customhud.h"
#include "ai_space.h"
#include "hudmanager.h"
#include "game_graph.h"
#include "game_sv_single.h"
#include "custommonster.h"
#include "ai/stalker/ai_stalker.h"
#include "xrserver_objects_alife_monsters.h"
#include "cover_point.h"
#include "cover_manager.h"
#include "cover_evaluators.h"
#include "team_base_zone.h"
#include "alife_simulator.h"
#include "alife_graph_registry.h"
#include "alife_object_registry.h"
#include "game_cl_base.h"
#include "space_restriction_manager.h"
#include "space_restriction.h"
#include "space_restrictor.h"
#include "space_restriction_base.h"
#include "detail_path_manager.h"
#include "memory_manager.h"
#include "enemy_manager.h"
#include "memory_space.h"
#include "level.h"
#include "ai_object_location.h"
#include "movement_manager.h"

void CLevelGraph::on_render1()
{
	CGameObject*	O	= smart_cast<CGameObject*> (Level().CurrentEntity());
	Fvector	POSITION	= O->Position();
	POSITION.y += 0.5f;

	// display
	Fvector P			= POSITION;

//	CPosition			Local;
//	vertex_position		(Local,P);

	u32 ID				= O->ai_location().level_vertex_id();

	CGameFont* F		= HUD().Font().pFontDI;
	F->SetSizeI			(.02f);
	F->OutI				(0.f,0.5f,"%f,%f,%f",VPUSH(P));
//	float				x,z;
//	unpack_xz			(Local,x,z);
//	F->Out				(0.f,0.55f,"%3d,%4d,%3d -> %d",	iFloor(x),iFloor(Local.y()),iFloor(z),u32(ID));

	svector<u32,128>	linked;
	{
		const_iterator	i,e;
		begin			(ID,i,e);
		for(; i != e; ++i)
			linked.push_back(value(ID,i));
	}

	// render
	float	sc		= header().cell_size()/16;
	float	st		= 0.98f*header().cell_size()/2;
	float	tt		= 0.01f;

	Fvector	DUP;	DUP.set(0,1,0);

	RCache.set_Shader(sh_debug);
	F->SetColor		(color_rgba(255,255,255,255));

	// если включён ai_dbg_frustum раскрасить ноды по light
	// иначе раскрашивать по cover
	bool b_light = !!psAI_Flags.test(aiFrustum);
	
	//////////////////////////////////////////////////////////////////////////
	Fvector min_position,max_position;
	max_position = min_position = Device.vCameraPosition;
	min_position.sub(30.f);
	max_position.add(30.f);
	
	CLevelGraph::const_vertex_iterator	 I, E;
	if (valid_vertex_position(min_position))
		I = std::lower_bound(begin(),end(),vertex_position(min_position).xz());
	else
		I = begin();

	if (valid_vertex_position(max_position)) {
		E = std::upper_bound(begin(),end(),vertex_position(max_position).xz());
		if (E != end()) ++E;
	}
	else
		E = end();

	//////////////////////////////////////////////////////////////////////////

	for ( ; I != E; ++I)
	{
		const CLevelGraph::CVertex&	N	= *I;
		Fvector			PC;
		PC				= vertex_position(N);

		u32 Nid			= vertex_id(I);

		if (Device.vCameraPosition.distance_to(PC)>30) continue;

		float			sr	= header().cell_size();
		if (::Render->ViewBase.testSphere_dirty(PC,sr)) {
			
			u32	LL = ((b_light) ?	iFloor(float(N.light())/15.f*255.f) : 
									iFloor(vertex_cover(I)/4*255.f));
			
			u32	CC		= D3DCOLOR_XRGB(0,0,255);
			u32	CT		= D3DCOLOR_XRGB(LL,LL,LL);
			u32	CH		= D3DCOLOR_XRGB(0,128,0);

			BOOL	bHL		= FALSE;
			if (Nid==u32(ID))	{ bHL = TRUE; CT = D3DCOLOR_XRGB(0,255,0); }
			else {
				for (u32 t=0; t<linked.size(); ++t) {
					if (linked[t]==Nid) { bHL = TRUE; CT = CH; break; }
				}
			}

			// unpack plane
			Fplane PL; Fvector vNorm;
			pvDecompress(vNorm,N.plane());
			PL.build	(PC,vNorm);

			// create vertices
			Fvector		v,v1,v2,v3,v4;
			v.set(PC.x-st,PC.y,PC.z-st);	PL.intersectRayPoint(v,DUP,v1);	v1.mad(v1,PL.n,tt);	// minX,minZ
			v.set(PC.x+st,PC.y,PC.z-st);	PL.intersectRayPoint(v,DUP,v2);	v2.mad(v2,PL.n,tt);	// maxX,minZ
			v.set(PC.x+st,PC.y,PC.z+st);	PL.intersectRayPoint(v,DUP,v3);	v3.mad(v3,PL.n,tt);	// maxX,maxZ
			v.set(PC.x-st,PC.y,PC.z+st);	PL.intersectRayPoint(v,DUP,v4);	v4.mad(v4,PL.n,tt);	// minX,maxZ

			// render quad
			RCache.dbg_DrawTRI	(Fidentity,v3,v2,v1,CT);
			RCache.dbg_DrawTRI	(Fidentity,v1,v4,v3,CT);

			// render center
			RCache.dbg_DrawAABB	(PC,sc,sc,sc,CC);

			// render id
			if (bHL) {
				Fvector		T;
				Fvector4	S;
				T.set		(PC); T.y+=0.3f;
				Device.mFullTransform.transform	(S,T);
				if (S.z < 0 || S.z < 0)												continue;
				if (S.x < -1.f || S.x > 1.f || S.y<-1.f || S.x>1.f)					continue;
				F->SetSizeI	(0.05f/_sqrt(_abs(S.w)));
				F->SetColor	(0xffffffff);
				F->OutI		(S.x,-S.y,"~%d",Nid);
			}
		}
	}
}

void CLevelGraph::draw_oriented_bounding_box(Fmatrix &T,	Fvector &half_dim, u32 C,	u32 C1) const
{
	RCache.dbg_DrawOBB(T,half_dim,C1);

	Fmatrix mL2W_Transform,mScaleTransform;
	mScaleTransform.scale(half_dim);
	mL2W_Transform.mul_43(T,mScaleTransform);

	Fvector aabb[8];
	aabb[0].set( -1, -1, -1); // 0
	aabb[1].set( -1, +1, -1); // 1
	aabb[2].set( +1, +1, -1); // 2
	aabb[3].set( +1, -1, -1); // 3
	aabb[4].set( -1, -1, +1); // 4
	aabb[5].set( -1, +1, +1); // 5
	aabb[6].set( +1, +1, +1); // 6
	aabb[7].set( +1, -1, +1); // 7

	//	u16             aabb_id[12*2] = {
	//		0,1,  1,2,  2,3,  3,0,  4,5,  5,6,  6,7,  7,4,  1,5,  2,6,  3,7,  0,4
	//	};
	for (int i=0; i<8; ++i) {
		Fmatrix						V = Fidentity;
		mL2W_Transform.transform_tiny(V.c,aabb[i]);
		V.i.x						= .05f;
		V.j.y						= .05f;
		V.k.z						= .05f;
		RCache.dbg_DrawEllipse		(V,C);
	}
}

void CLevelGraph::draw_travel_line() const
{
	draw_dynamic_obstacles				();
	for (u32 I=1, N=m_tpTravelLine.size(); I<N; ++I) {
		Fvector	P1, P2; 
		P1.set							(m_tpTravelLine[I - 1]);
		P2.set							(m_tpTravelLine[I]);

		P1.y							+= 0.1f;
		P2.y							+= 0.1f;

		RCache.dbg_DrawLINE				(Fidentity,P1,P2,D3DCOLOR_XRGB(0,255,0));

		if (I == (N - 1)) {
			P2.y						+= 0.005f;
			RCache.dbg_DrawAABB			(P2,.105f,.105f,.105f,D3DCOLOR_XRGB(255,0,0));
		}
		else 
			RCache.dbg_DrawAABB			(P2,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));

		Fvector         T;
		Fvector4        S;

		T.set							(m_tpTravelLine[I]);
		T.y								+= (1.5f);

		Device.mFullTransform.transform (S,T);
	}
	if (!m_tpTravelLine.empty()) {
		Fvector							P = m_tpTravelLine.front();
		P.y								+= .1f;
		RCache.dbg_DrawAABB				(P,.1f,.1f,.1f,D3DCOLOR_XRGB(255,255,255));
	}
}

void CLevelGraph::compute_travel_line(xr_vector<u32> &vertex_path, u32 vertex_id, u32 finish_vertex_id) const
{
//	Fvector						*tpDestinationPosition = &m_tFinishPoint;
//
//	if (tpDestinationPosition && inside(vertex(dwFinishNodeID),*tpDestinationPosition))
//		tpDestinationPosition->y = level_graph().vertex_plane_y(*vertex(dwFinishNodeID),tpDestinationPosition->x,tpDestinationPosition->z);
//
//	m_tpaPoints.clear			();
//	m_tpaDeviations.clear		();
//	m_tpaTravelPath.clear		();
//	m_tpaPointNodes.clear		();
//
//	u32							N = (int)m_level_path.size();
//	if (!N) {
//		Msg						("! vertex list is empty!");
//		m_level_path.clear		();
//		m_tpTravelLine.clear	();
//		return;
//	}
//
//	Fvector						tStartPosition = m_start_point;
//	u32							dwCurNode = dwStartNodeID;
//	m_tpaPoints.push_back		(tStartPosition);
//	m_tpaPointNodes.push_back	(dwCurNode);
//
//	for (u32 i=1; i<=N; ++i)
//		if (i<N) {
//			if (!ai().level_graph().check_vertex_in_direction(dwCurNode,tStartPosition,m_level_path[i])) {
//				if (dwCurNode != m_level_path[i - 1])
//					m_tpaPoints.push_back(tStartPosition = level_graph().vertex_position(dwCurNode = m_level_path[--i]));
//				else
//					m_tpaPoints.push_back(tStartPosition = level_graph().vertex_position(dwCurNode = m_level_path[i]));
//				m_tpaPointNodes.push_back(dwCurNode);
//			}
//		}
//		else
//			if (tpDestinationPosition)
//				if (level_graph().check_position_in_direction(dwCurNode,tStartPosition,*tpDestinationPosition) == u32(-1)) {
//					if (dwCurNode != dwFinishNodeID)
//						m_tpaPointNodes.push_back(dwFinishNodeID);
//					m_tpaPoints.push_back(*tpDestinationPosition);
//				}
//				else {
//					dwCurNode = dwFinishNodeID;
//					if (inside(vertex(dwCurNode),*tpDestinationPosition)) {
//						tpDestinationPosition->y = level_graph().vertex_plane_y(*vertex(dwFinishNodeID),tpDestinationPosition->x,tpDestinationPosition->z);
//						m_tpaPointNodes.push_back(dwFinishNodeID);
//						m_tpaPoints.push_back(*tpDestinationPosition);
//					}
//				}
//
//				m_tpaDeviations.resize	(N = (int)m_tpaPoints.size());
//
//				m_tpTravelLine.clear();
//
//				m_level_path.clear		();
//
//				Fvector			T;
//				T.set			(0,0,0);
//				for (i=1; i<N; ++i) {
//					m_tpaLine.clear();
//					m_tpaLine.push_back(m_tpaPoints[i-1]);
//					m_tpaLine.push_back(m_tpaPoints[i]);
//					bfCreateStraightPTN_Path(m_tpaPointNodes[i-1],m_tpaPoints[i-1],m_tpaPoints[i],m_tpaTravelPath,m_tpaNodes, i == 1);
//					u32 n = (u32)m_tpaTravelPath.size();
//					for (u32 j= 0; j<n; ++j) {
//						T = m_tpaTravelPath[j];
//						m_tpTravelLine.push_back(T);
//						m_level_path.push_back(m_tpaNodes[j]);
//					}
//				}
//				if (N > 1) {
//					m_level_path[m_level_path.size() - 1] = dwFinishNodeID;
//					if (tpDestinationPosition && m_tpTravelLine.size() && m_tpTravelLine[m_tpTravelLine.size() - 1].distance_to(*tpDestinationPosition) > EPS_L) {
//						if (inside(vertex(dwFinishNodeID),*tpDestinationPosition) && valid_vertex_id(check_position_in_direction(dwFinishNodeID,T,*tpDestinationPosition))) {
//							T = *tpDestinationPosition;
//							m_tpTravelLine.push_back(T);
//						}
//					}
//				}
//				else
//					if (tpDestinationPosition && m_tpTravelLine.size() && m_tpTravelLine[m_tpTravelLine.size() - 1].distance_to(*tpDestinationPosition) > EPS_L)
//						if (inside(vertex(dwFinishNodeID),*tpDestinationPosition) && valid_vertex_id(check_position_in_direction(dwFinishNodeID,T,*tpDestinationPosition)))
//							m_tpTravelLine.push_back(*tpDestinationPosition);
}

#include "graph_engine.h"

void CLevelGraph::draw_dynamic_obstacles() const
{
//	Level().ObjectSpace.GetNearest		(m_start_point,100.f,Level().CurrentEntity()); 
//	xr_vector<CObject*>					&tpNearestList = Level().ObjectSpace.q_nearest; 
//	{
//		xr_vector<CObject*>::iterator		I = tpNearestList.begin();
//		xr_vector<CObject*>::iterator		E = tpNearestList.end();
//		for ( ; I != E; ++I) {
//			Fvector							c, d, C2;
//			(*I)->Visual()->vis.box.get_CD	(c,d);
//			Fmatrix							M = (*I)->XFORM();
//			M.transform_tiny				(C2,c);
//			M.c								= C2;
//			draw_oriented_bounding_box		(M,d,color_rgba(0,0,255,255),color_rgba(0,255,0,255));
//		}
//	}
//	for (u32 i=0, n = ai().m_visited_nodes.size(); i<n; ++i) {
//		Fvector				position = vertex_position(ai().m_visited_nodes[i]);
//		position.y			+= 0.005f;
//		RCache.dbg_DrawAABB	(position,.105f,.105f,.105f,D3DCOLOR_XRGB(255,0,0));
//	}
}

void CLevelGraph::set_start_point	()
{
	start.position			= v2d(Level().CurrentEntity()->Position());
	start.vertex_id			= vertex(v3d(start.position));
//	CObject					*obj = Level().Objects.FindObjectByName("m_stalker_e0000");
//	CAI_Stalker				*stalker = smart_cast<CAI_Stalker*>(obj);
//	obj						= Level().Objects.FindObjectByName("localhost/dima/name=DIMA-AI");
//	CActor					*actor = smart_cast<CActor*>(obj);
//	if (!stalker || !actor)
//		return;
//
//	start.position			= v2d(stalker->Position());
//	start.direction.x		= -_sin(-stalker->m_body.current.yaw);
//	start.direction.y		= _cos(-stalker->m_body.current.yaw);
//	start.vertex_id			= vertex(v3d(start.position));
//
//	dest.position			= v2d(actor->Position());
//	dest.direction.x		= -_sin(actor->r_model_yaw);
//	dest.direction.y		= _cos(actor->r_model_yaw);
//	dest.vertex_id			= vertex(v3d(dest.position));
//	
//	start.angular_velocity	= 1.f;
//	start.linear_velocity	= 2.f;

//	start.position			= Fvector2().set(-50.f,-40.f);
//	start.direction.set		(0.f,1.f);
//	start.vertex_id			= vertex(v3d(start.position));
//	
//	dest.position			= Fvector2().set(-40.f,-40.f);
//	dest.direction.set		(0.f,1.f);
//	dest.vertex_id			= vertex(v3d(dest.position));
//
//	static int start_static = 0;
//	switch ((start_static >> 2) & 3) {
//		case 0 :
//			start.direction.set(0.f,1.f);
//			break;
//		case 1 :
//			start.direction.set(1.f,0.f);
//			break;
//		case 2 :
//			start.direction.set(0.f,-1.f);
//			break;
//		case 3 :
//			start.direction.set(-1.f,0.f);
//			break;
//	}
//	switch (start_static & 3) {
//		case 0 :
//			dest.direction.set(0.f,1.f);
//			break;
//		case 1 :
//			dest.direction.set(1.f,0.f);
//			break;
//		case 2 :
//			dest.direction.set(0.f,-1.f);
//			break;
//		case 3 :
//			dest.direction.set(-1.f,0.f);
//			break;
//	}
//	++start_static;
//	dest.angular_velocity	= 1.f;
//	dest.linear_velocity	= 2.f;

//	start.position.set		(-42.369289f,-27.931879f);
//	start.direction.set		(-0.007825f,0.999969f);
//	start.vertex_id			= vertex(v3d(start.position));
//
//	dest.position.set		(-42.342930f,-28.019650f);
//	dest.direction.set		(0.699997f,0.000000f);
//	dest.vertex_id			= vertex(v3d(dest.position));
//
//	STrajectoryPoint		temp = start;
//	start					= dest;
//	dest					= temp;

//	write_trajectory_point	(start,"start");
//	write_trajectory_point	(dest,"dest");
}

extern bool is_negative(float a);
//{
//	return				(!fis_zero(a) && (a < 0.f));
//}

IC	void adjust_point(
	const Fvector2		&source, 
	float				yaw, 
	float				magnitude, 
	Fvector2			&dest
)
{
	dest.x				= -_sin(yaw);
	dest.y				= _cos(yaw);
	dest.mad			(source,dest,magnitude);
}

enum EVelocityDirectionType {
	eDirectionTypeFP = u32(0),
	eDirectionTypeFN = u32(1),
	eDirectionTypeSP = u32(0),
	eDirectionTypeSN = u32(2),
	eDirectionTypePP = eDirectionTypeFP | eDirectionTypeSP, // both linear velocities are positive
	eDirectionTypeNN = eDirectionTypeFN | eDirectionTypeSN, // both linear velocities are negative
	eDirectionTypePN = eDirectionTypeFP | eDirectionTypeSN, // the first linear velocity is positive, the second one - negative
	eDirectionTypeNP = eDirectionTypeFN | eDirectionTypeSP, // the first linear velocity is negative, the second one - positive
};

IC	void assign_angle(
	float					&angle, 
	const float				start_yaw, 
	const float				dest_yaw, 
	const bool				positive,
	const EVelocityDirectionType	direction_type,
	const bool				start = true
)
{
	if (positive)
		if (dest_yaw >= start_yaw)
			angle		= dest_yaw - start_yaw;
		else
			angle		= PI_MUL_2 - start_yaw + dest_yaw;
	else
		if (dest_yaw <= start_yaw)
			angle		= dest_yaw - start_yaw;
		else
			angle		= dest_yaw - start_yaw - PI_MUL_2;

	if (!start && ((direction_type == eDirectionTypePP) || (direction_type == eDirectionTypeNN)))
		if (angle < 0.f)
			angle = angle + PI_MUL_2;
		else
			angle = angle - PI_MUL_2;

	if (fsimilar(_abs(angle),PI_MUL_2,EPS_L))
		angle	  = 0;

	VERIFY				(_valid(angle));
}

IC	void compute_circles(
	CLevelGraph::STrajectoryPoint	&point, 
	CLevelGraph::SCirclePoint		*circles
)
{
	VERIFY				(!fis_zero(point.angular_velocity));
	point.radius		= _abs(point.linear_velocity)/point.angular_velocity;
	circles[0].radius	= circles[1].radius = point.radius;
	VERIFY				(fsimilar(point.direction.square_magnitude(),1.f));
	circles[0].center.x =  point.direction.y*point.radius + point.position.x;
	circles[0].center.y = -point.direction.x*point.radius + point.position.y;
	circles[1].center.x = -point.direction.y*point.radius + point.position.x;
	circles[1].center.y =  point.direction.x*point.radius + point.position.y;
}

IC	bool compute_tangent(
	const CLevelGraph::STrajectoryPoint	&start, 
	const CLevelGraph::SCirclePoint		&start_circle, 
	const CLevelGraph::STrajectoryPoint	&dest, 
	const CLevelGraph::SCirclePoint		&dest_circle, 
	CLevelGraph::SCirclePoint			*tangents,
	const EVelocityDirectionType				direction_type
)
{
	float				start_cp, dest_cp, distance, alpha, start_yaw, dest_yaw, yaw1, yaw2;
	Fvector2			direction, temp;
	
	// computing 2D cross product for start point
	direction.sub		(start.position,start_circle.center);
	if (fis_zero(direction.square_magnitude()))
		direction		= start.direction;

	start_yaw			= direction.getH();
	start_yaw			= start_yaw >= 0.f ? start_yaw : start_yaw + PI_MUL_2;
	start_cp			= start.direction.crossproduct(direction);
	
	// computing 2D cross product for dest point
	direction.sub		(dest.position,dest_circle.center);
	if (fis_zero(direction.square_magnitude()))
		direction		= dest.direction;

	dest_yaw			= direction.getH();
	dest_yaw			= dest_yaw >= 0.f ? dest_yaw : dest_yaw + PI_MUL_2;
	dest_cp				= dest.direction.crossproduct(direction);

	// direction from the first circle to the second one
	direction.sub		(dest_circle.center,start_circle.center);
	yaw1				= direction.getH();
	yaw1 = yaw2			= yaw1 >= 0.f ? yaw1 : yaw1 + PI_MUL_2;

	if (start_cp*dest_cp >= 0.f) {
		// so, our tangents are outside
		if (start_circle.center.similar(dest_circle.center)) {
			if  (fsimilar(start_circle.radius,dest_circle.radius)) {
				// so, our circles are equal
				tangents[0]			= tangents[1] = start_circle;
				if (start_cp >= 0.f) {
					adjust_point	(start_circle.center,yaw1 + PI_DIV_2,	start_circle.radius,tangents[0].point);
					assign_angle	(tangents[0].angle,start_yaw,yaw1 + PI_DIV_2 < PI_MUL_2 ? yaw1 + PI_DIV_2 : yaw1 - PI - PI_DIV_2,true,direction_type);
				}
				else {
					adjust_point	(start_circle.center,yaw1 - PI_DIV_2,	start_circle.radius,tangents[0].point);
					assign_angle	(tangents[0].angle,start_yaw,yaw1 - PI_DIV_2 >= 0.f ? yaw1 - PI_DIV_2 : yaw1 + PI + PI_DIV_2,false,direction_type);
				}

				tangents[1].point	= tangents[0].point;
				tangents[1].angle	= 0.f;
				return				(true);
			}
			else {
				return				(false);
			}
		}
		else {
			// distance between circle centers
			distance		= start_circle.center.distance_to(dest_circle.center);
			// radius difference
			float			r_diff = start_circle.radius - dest_circle.radius;
			if ((r_diff > distance) && !fsimilar(r_diff,distance)) {
				return		(false);
			}
			// angle between external tangents and circle centers segment
			float			temp = r_diff/distance;
			clamp			(temp,-.99999f,.99999f);
			alpha			= acosf(temp);
			alpha			= alpha >= 0.f ? alpha : alpha + PI_MUL_2;
		}
	}
	else {
		distance		= start_circle.center.distance_to(dest_circle.center);
		// so, our tangents are inside (crossing)
		if ((start_circle.radius + dest_circle.radius > distance) && !fsimilar(start_circle.radius + dest_circle.radius,distance)) {
			return		(false);
		}
	
		// angle between internal tangents and circle centers segment
		float			temp = (start_circle.radius + dest_circle.radius)/distance;
		clamp			(temp,-.99999f,.99999f);
		alpha			= acosf(temp);
		alpha			= alpha >= 0.f ? alpha : alpha + PI_MUL_2;
		yaw2			= yaw1 < PI ? yaw1 + PI : yaw1 - PI;
	}

	tangents[0]			= start_circle;
	tangents[1]			= dest_circle;

	// compute external tangent points
	adjust_point		(start_circle.center,yaw1 + alpha,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center, yaw2 + alpha,	dest_circle.radius, tangents[1].point);

	direction.sub		(tangents[1].point,tangents[0].point);
	temp.sub			(tangents[0].point,start_circle.center);
	float				tangent_cp = direction.crossproduct(temp);
	if (start_cp*tangent_cp >= 0) {
		assign_angle	(tangents[0].angle,start_yaw,yaw1 + alpha < PI_MUL_2 ? yaw1 + alpha : yaw1 + alpha - PI_MUL_2,start_cp >= 0,direction_type);
		assign_angle	(tangents[1].angle,dest_yaw, yaw2 + alpha < PI_MUL_2 ? yaw2 + alpha : yaw2 + alpha - PI_MUL_2,dest_cp  >= 0,direction_type,false);
		return			(true);
	}

	// compute external tangent points
	adjust_point		(start_circle.center,yaw1 - alpha,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center, yaw2 - alpha,	dest_circle.radius, tangents[1].point);
	assign_angle		(tangents[0].angle,start_yaw,yaw1 - alpha >= 0.f ? yaw1 - alpha : yaw1 - alpha + PI_MUL_2,start_cp >= 0,direction_type);
	assign_angle		(tangents[1].angle,dest_yaw, yaw2 - alpha >= 0.f ? yaw2 - alpha : yaw2 - alpha + PI_MUL_2,dest_cp  >= 0,direction_type,false);

	return				(true);
}

template<
	typename T
>
IC	T sin_apb(T sina, T cosa, T sinb, T cosb)
{
	return				(sina*cosb + cosa*sinb);
}

template<
	typename T
>
IC	T cos_apb(T sina, T cosa, T sinb, T cosb)
{
	return				(cosa*cosb - sina*sinb);
}

IC	bool build_circle_trajectory(
	CLevelGraph							&level_graph,
	const CLevelGraph::STrajectoryPoint	&position, 
	xr_vector<Fvector>					*path,
	u32									*vertex_id
)
{
	const float			min_dist = .1f;
	if (position.radius*_abs(position.angle) <= min_dist) {
		if (!path) {
			if (vertex_id)
				*vertex_id	= position.vertex_id;
			return			(true);
		}
		if (vertex_id) {
			*vertex_id		= position.vertex_id;
			if (path) {
				Fvector			t = level_graph.v3d(position.position);
				t.y				= level_graph.vertex_plane_y(level_graph.vertex(position.vertex_id),position.position.x,position.position.y);
				path->push_back	(t);
			}
		}
		return			(true);
	}
	Fvector2			direction;
	Fvector				curr_pos;
	u32					curr_vertex_id;
	direction.sub		(position.position,position.center);
	curr_pos.set		(position.position.x,0.f,position.position.y);
	curr_pos.y			= level_graph.vertex_plane_y(position.vertex_id,position.position.x,position.position.y);
	curr_vertex_id		= position.vertex_id;
	float				angle = position.angle;
	u32					size = path ? path->size() : u32(-1);

	if (!fis_zero(direction.square_magnitude()))
		direction.normalize	();
	else
		direction.set	(1.f,0.f);

	float				sina, cosa, sinb, cosb, sini, cosi, temp;
	u32					m = _min(iFloor(_abs(angle)/position.angular_velocity*10.f + 1.5f),iFloor(position.radius*_abs(angle)/min_dist + 1.5f));
	u32					n = fis_zero(position.angular_velocity) || !m ? 1 : m;
	int					k = vertex_id ? 0 : -1;

	if (path)
		path->reserve	(size + n + k);

	sina				= -direction.x;
	cosa				= direction.y;
	sinb				= _sin(angle/float(n));
	cosb				= _cos(angle/float(n));
	sini				= 0.f;
	cosi				= 1.f;

	for (u32 i=0; i<=n + k; ++i) {
		Fvector			t;
		t.x				= -sin_apb(sina,cosa,sini,cosi)*position.radius + position.center.x;
		t.z				= cos_apb(sina,cosa,sini,cosi)*position.radius + position.center.y;
		temp			= sin_apb(sinb,cosb,sini,cosi);
		cosi			= cos_apb(sinb,cosb,sini,cosi);
		sini			= temp;

		curr_vertex_id	= level_graph.check_position_in_direction(curr_vertex_id,curr_pos,t);
		if (!level_graph.valid_vertex_id(curr_vertex_id)) {
			return		(false);
		}
		if (path) {
			t.y			= level_graph.vertex_plane_y(curr_vertex_id,t.x,t.z);
			path->push_back(t);
		}
		curr_pos		= t;
	}
	
	if (vertex_id)
		*vertex_id		= curr_vertex_id;
	else
		if (path)
			reverse		(path->begin() + size,path->end());

	return				(true);
}

IC	bool build_line_trajectory(
	CLevelGraph							&level_graph,
	const CLevelGraph::STrajectoryPoint	&start, 
	const CLevelGraph::STrajectoryPoint	&dest, 
	u32									vertex_id,
	xr_vector<Fvector>					*path
)
{
	xr_vector<u32>			node_path;
	VERIFY					(level_graph.valid_vertex_id(vertex_id));
	if (level_graph.inside(vertex_id,dest.point)) {
		if (path) {
			Fvector			t = level_graph.v3d(dest.point);
			t.y				= level_graph.vertex_plane_y(vertex_id,dest.point.x,dest.point.y);
			path->push_back	(t);
		}
		return			(true);
	}
	bool				b = path ? level_graph.create_straight_path(vertex_id,start.point,dest.point,*path,node_path,false,false) : level_graph.valid_vertex_id(level_graph.check_position_in_direction(vertex_id,start.point,dest.point));
	return				(b);
}

IC	bool build_trajectory(
	CLevelGraph							&level_graph,
	const CLevelGraph::STrajectoryPoint	&start, 
	const CLevelGraph::STrajectoryPoint	&dest, 
	xr_vector<Fvector>					*path
)
{
	u32					vertex_id;
	if (!build_circle_trajectory(level_graph,start,path,&vertex_id)) {
//		Msg				("FALSE : Circle 0");
		return			(false);
	}
	if (!build_line_trajectory(level_graph,start,dest,vertex_id,path)) {
//		Msg				("FALSE : Line");
		return			(false);
	}
	if (!build_circle_trajectory(level_graph,dest,path,0)) {
//		Msg				("FALSE : Circle 1");
		return			(false);
	}
//	Msg					("TRUE");
	return				(true);
}

struct SDist {
	u32		index;
	float	time;

	bool operator<(const SDist &d1) const
	{
		return		(time < d1.time);
	}
};

IC	bool build_trajectory(
	CLevelGraph							&level_graph,
	CLevelGraph::STrajectoryPoint		&start, 
	CLevelGraph::STrajectoryPoint		&dest, 
	const CLevelGraph::SCirclePoint		tangents[4][2], 
	const u32							tangent_count,
	float								straight_velocity,
	xr_vector<Fvector>					*path,
	float								&time
)
{
	time			= flt_max;
	SDist			dist[4];
	{
		for (u32 i=0; i<tangent_count; ++i) {
			dist[i].index = i;
			dist[i].time = 
				_abs(tangents[i][0].angle)/start.angular_velocity +
				_abs(tangents[i][1].angle)/dest.angular_velocity +
				tangents[i][0].point.distance_to(tangents[i][1].point)*
				(fis_zero(straight_velocity) ? 0 : 1.f/straight_velocity); 
		}
	}
	
	std::sort	(dist,dist + tangent_count);

	{
		for (u32 i=0, j = path ? path->size() : 0; i<tangent_count; ++i) {
			(CLevelGraph::SCirclePoint&)(start) = tangents[dist[i].index][0];
			(CLevelGraph::SCirclePoint&)(dest)	= tangents[dist[i].index][1];
			if (build_trajectory(level_graph,start,dest,path)) {
				time	= dist[i].time;
				return	(true);
			}
			else
				if (path)
					path->resize(j);
		}
	}

	return		(false);
}

IC	bool compute_trajectory(
	CLevelGraph						&level_graph,
	CLevelGraph::STrajectoryPoint	&start,
	CLevelGraph::STrajectoryPoint	&dest,
	float							straight_velocity,
	xr_vector<Fvector>				*path,
	float							&time,
	const EVelocityDirectionType	direction_type
)
{
	CLevelGraph::SCirclePoint	start_circles[2], dest_circles[2];
	compute_circles				(start,start_circles);
	compute_circles				(dest,dest_circles);
	
	u32							tangent_count = 0;
	CLevelGraph::SCirclePoint	tangent_points[4][2];
	for (u32 i=0; i<2; ++i)
		for (u32 j=0; j<2; ++j)
			if (compute_tangent(
					start,
					start_circles[i],
					dest,
					dest_circles[j],
					tangent_points[tangent_count],
					direction_type
				)
			)
				++tangent_count;

	bool			b = build_trajectory(level_graph,start,dest,tangent_points,tangent_count,straight_velocity,path,time);
	return			(b);
}

bool compute_path(
	CLevelGraph								&level_graph,
	CLevelGraph::STrajectoryPoint			&_start,
	CLevelGraph::STrajectoryPoint			&_dest,
	xr_vector<CLevelGraph::STravelParams>	&start_set,
	xr_vector<CLevelGraph::STravelParams>	&dest_set,
	xr_vector<Fvector>						*m_tpTravelLine
)
{
	xr_vector<Fvector>		travel_line;
	CLevelGraph::STrajectoryPoint			start = _start;
	CLevelGraph::STrajectoryPoint			dest = _dest;
	float					min_time = flt_max, time;
	u32						size = m_tpTravelLine ? m_tpTravelLine->size() : 0;
	xr_vector<CLevelGraph::STravelParams>::const_iterator I = start_set.begin(), B = I;
	xr_vector<CLevelGraph::STravelParams>::const_iterator E = start_set.end();
	for ( ; I != E; ++I) {
		EVelocityDirectionType				direction_type = eDirectionTypePP;
		start								= _start;
		(CLevelGraph::STravelParams&)start	= *I;
		if (is_negative(start.linear_velocity)) {
			direction_type	= EVelocityDirectionType(direction_type | eDirectionTypeFN);
			start.direction.mul	(-1.f);
		}
		xr_vector<CLevelGraph::STravelParams>::const_iterator i = dest_set.begin(), b = i;
		xr_vector<CLevelGraph::STravelParams>::const_iterator e = dest_set.end();
		for ( ; i != e; ++i) {
			dest								= _dest;
			(CLevelGraph::STravelParams&)dest	= *i;
			if (is_negative(dest.linear_velocity))
				direction_type	= EVelocityDirectionType(direction_type | eDirectionTypeSN);
			if (direction_type & eDirectionTypeFN)
				dest.direction.mul	(-1.f);
			travel_line.clear				();
			if (compute_trajectory(level_graph,start,dest,6.f,m_tpTravelLine ? &travel_line : 0,time,direction_type)) {
				if (time < min_time) {
					min_time = time;
					if (m_tpTravelLine) {
						m_tpTravelLine->resize(size);
						m_tpTravelLine->insert(m_tpTravelLine->end(),travel_line.begin(),travel_line.end());
					}
					else {
						return(true);
					}
				}
			}
		}
	}
	
	if (fsimilar(min_time,flt_max)) {
		return				(false);
	}
	
	return					(true);
}

void fill_params(
	CLevelGraph::STrajectoryPoint			&start,
	CLevelGraph::STrajectoryPoint			&dest,
	xr_vector<CLevelGraph::STravelParams>	&start_set,
	xr_vector<CLevelGraph::STravelParams>	&dest_set
)
{
//	start.angular_velocity	= PI_MUL_2;
//	start.linear_velocity	= 0.f;
//	start_set.push_back		(start);
//
//	start.angular_velocity	= PI;
//	start.linear_velocity	= 2.15f;
//	start_set.push_back		(start);
//
//	start.angular_velocity	= PI_DIV_2;
//	start.linear_velocity	= 4.5f;
//	start_set.push_back		(start);
//
//	start.angular_velocity	= PI_DIV_4;
//	start.linear_velocity	= 6.f;
//	start_set.push_back		(start);
//
//	dest.angular_velocity	= PI_MUL_2;
//	dest.linear_velocity	= 0.f;
//	dest_set.push_back		(dest);
//
//	dest.angular_velocity	= PI;
//	dest.linear_velocity	= 2.15f;
//	dest_set.push_back		(dest);
//
//	dest.angular_velocity	= PI_DIV_2;
//	dest.linear_velocity	= 4.5f;
//	dest_set.push_back		(dest);
//
//	dest.angular_velocity	= PI_DIV_4;
//	dest.linear_velocity	= 6.f;
//	dest_set.push_back		(dest);

	start.angular_velocity	= 1.f;
	start.linear_velocity	= 2.f;
	start_set.push_back		(start);

	dest.angular_velocity	= 1.f;
	dest.linear_velocity	= 2.f;
	dest_set.push_back		(dest);
}

void CLevelGraph::build_detail_path()
{
	xr_vector<STravelParams>				start_set, dest_set;
//	xr_vector<Fvector>						m_tpTravelLine;
	xr_vector<Fvector>						travel_line;
	xr_vector<STravelPoint>					key_points;
	STrajectoryPoint						s,d,t,p;
	CLevelGraph								&level_graph = *this;

	fill_params								(start,dest,start_set,dest_set);

	if (start.direction.square_magnitude() < EPS_L)
		start.direction.set					(0.f,1.f);
	else
		start.direction.normalize			();

	if (dest.direction.square_magnitude() < EPS_L)
		dest.direction.set					(0.f,1.f);
	else
		dest.direction.normalize			();

	m_tpTravelLine.clear					();
//	ai().m_visited_nodes.clear				();
	if (!ai().graph_engine().search(ai().level_graph(),start.vertex_id,dest.vertex_id,&m_tpaNodes,GraphEngineSpace::CBaseParameters()))
		return;

#ifndef AI_COMPILER
	Device.Statistic->TEST0.Begin			();
#endif
	VERIFY									(!m_tpaNodes.empty());
	if (m_tpaNodes.size() == 1) {
		if (!compute_path(level_graph,start,dest,start_set,dest_set,&m_tpTravelLine)) {
			m_tpTravelLine.clear			();
#ifndef AI_COMPILER
			Device.Statistic->TEST0.End		();
#endif
			return;
		}
	}
	else {
		if (compute_path(level_graph,start,dest,start_set,dest_set,&m_tpTravelLine)) {
			Msg	("%d : ok, %d points",Device.dwTimeGlobal,m_tpTravelLine.size());
#ifndef AI_COMPILER
			Device.Statistic->TEST0.End		();
#endif
			return;
		}
		
		m_tpTravelLine.clear				();

		STravelPoint						start_point;
		start_point.vertex_id				= m_tpaNodes.front();
		start_point.position				= start.position;

		for (int _i=0, i=0, n=(int)m_tpaNodes.size() - 1, j = n, m=j; _i < n; ) {
			if (!level_graph.check_vertex_in_direction(start_point.vertex_id,start_point.position,m_tpaNodes[j])) {
				m							= j;
				j							= (i + j)/2;
			}
			else {
				i							= j;
				j							= (i + m)/2;
			}
			if (i >= m - 1) {
				if (i <= _i) {
#ifndef AI_COMPILER
					Device.Statistic->TEST0.End();
#endif
					return;
				}
				_i							= i;
				key_points.push_back		(start_point);
				if (i == n) {
					if (level_graph.valid_vertex_id(level_graph.check_position_in_direction(start_point.vertex_id,start_point.position,dest.position))) {
						key_points.push_back(dest);
						break;
					}
					else {
#ifndef AI_COMPILER
						Device.Statistic->TEST0.End();
#endif
						return;
					}
				}
				start_point.vertex_id		= m_tpaNodes[_i];
				start_point.position		= v2d(level_graph.vertex_position(start_point.vertex_id));
				j = m						= n;
			}
		}

		s = t								= start;
		xr_vector<STravelPoint>::const_iterator	I = key_points.begin(), P = I;
		xr_vector<STravelPoint>::const_iterator	E = key_points.end();
		for ( ; I != E; ++I) {
			// setting up destination
			if ((I + 1) != E) {
				(STravelPoint&)d = *I;
				d.direction.sub				((I + 1)->position,d.position);
				VERIFY						(!fis_zero(d.direction.magnitude()));
				d.direction.normalize		();
			}
			else
				d							= dest;

			if (!compute_path(level_graph,s,d,start_set,dest_set,0)) {
				if (I == P) {
					m_tpTravelLine.clear	();
#ifndef AI_COMPILER
					Device.Statistic->TEST0.End();
#endif
					return;
				}
				
				p							= d;
				(STravelPoint&)d = *(I - 1);
				d.direction.sub				((*I).position,d.position);
				VERIFY						(!fis_zero(d.direction.magnitude()));
				d.direction.normalize		();
				if (!compute_path(level_graph,s,d,start_set,dest_set,&m_tpTravelLine)) {
					VERIFY					(false);
				}
				P							= I - 1;
				d							= p;
				s							= t;
				if (!m_tpTravelLine.empty()) {
//					if (is_negative((*I).second.linear_velocity))
//						s.direction.mul		(-1.f);
				}
				VERIFY						(!fis_zero(s.direction.magnitude()));
				if (!compute_path(level_graph,s,d,start_set,dest_set,0)) {
					m_tpTravelLine.clear	();
#ifndef AI_COMPILER
					Device.Statistic->TEST0.End();
#endif
					return;
				}
			}
			t								= d;
		}
		if (!compute_path(level_graph,s,d,start_set,dest_set,&m_tpTravelLine)) {
			if (compute_path(level_graph,s,d,start_set,dest_set,0)) {
				compute_path(level_graph,s,d,start_set,dest_set,&m_tpTravelLine);
				t=t;
			}
			VERIFY							(false);
		}
	}
#ifndef AI_COMPILER
	Device.Statistic->TEST0.End				();
#endif
}

void CLevelGraph::set_dest_point			()
{
	dest.position							= v2d(Level().CurrentEntity()->Position());
}

void CLevelGraph::select_cover_point		()
{
	Fvector									target = v3d(dest.position);
	Fvector									position = v3d(start.position);
	Fvector									direction;
	float									y, p, best_value = flt_max;
	xr_vector<CCoverPoint*>					nearest;

	ai().cover_manager().covers().nearest	(position,30.f,nearest);
	m_best_point							= 0;
	xr_vector<CCoverPoint*>::const_iterator	I = nearest.begin();
	xr_vector<CCoverPoint*>::const_iterator	E = nearest.end();
	for ( ; I != E; ++I) {
		direction.sub	(target,position);
		direction.getHP	(y,p);
		float			cover_value			= ai().level_graph().cover_in_direction(y,(*I)->level_vertex_id());
		float			enemy_distance		= target.distance_to((*I)->position());
		float			my_distance			= position.distance_to((*I)->position());
		if ((cover_value < best_value) && (enemy_distance > 5.f) && (my_distance < 30.f)) {
			best_value		= cover_value;
			m_best_point	= *I;
		}
	}
}

void CLevelGraph::on_render2	()
{
	CSpaceRestrictionManager::SPACE_RESTRICTIONS::const_iterator	I = Level().space_restriction_manager().restrictions().begin();
	CSpaceRestrictionManager::SPACE_RESTRICTIONS::const_iterator	E = Level().space_restriction_manager().restrictions().end();

	CRandom R;

	for ( ; I != E; ++I) {
		if (!(*I).second->m_ref_count)
			continue;
		if (!(*I).second->initialized()) continue;

		u8 b = u8(R.randI(255));
		u8 g = u8(R.randI(255));
		u8 r = u8(R.randI(255));

		xr_vector<u32>::const_iterator	i = (*I).second->border().begin();
		xr_vector<u32>::const_iterator	e = (*I).second->border().end();
		for ( ; i != e; ++i) {
			Fvector temp = ai().level_graph().vertex_position(*i);
			temp.y += .1f;
			RCache.dbg_DrawAABB(temp,.05f,.05f,.05f,D3DCOLOR_XRGB(r,g,b));
		}

#ifdef USE_FREE_IN_RESTRICTIONS
		CSpaceRestriction::FREE_IN_RESTRICTIONS::const_iterator II = (*I).second->m_free_in_restrictions.begin();
		CSpaceRestriction::FREE_IN_RESTRICTIONS::const_iterator EE = (*I).second->m_free_in_restrictions.end();
		for ( ; II != EE; ++II) {
			xr_vector<u32>::const_iterator	i = (*II).m_restriction->border().begin();
			xr_vector<u32>::const_iterator	e = (*II).m_restriction->border().end();
			for ( ; i != e; ++i) {
				Fvector temp = ai().level_graph().vertex_position(*i);
				temp.y += .1f;
				RCache.dbg_DrawAABB(temp,.05f,.05f,.05f,D3DCOLOR_XRGB(255,0,0));
			}
			{
				xr_vector<u32>::const_iterator	i = (*II).m_restriction->border().begin();
				xr_vector<u32>::const_iterator	e = (*II).m_restriction->border().end();
				for ( ; i != e; ++i) {
					Fvector temp = ai().level_graph().vertex_position(*i);
					temp.y += .1f;
					RCache.dbg_DrawAABB(temp,.05f,.05f,.05f,D3DCOLOR_XRGB(0,255,0));
				}
			}
		}
#endif
	}
}

void CLevelGraph::on_render3	()
{
	float					half_size = ai().level_graph().header().cell_size()*.5f;
	xr_vector<CCoverPoint*>	nearest;
	nearest.reserve			(1000);
	ai().cover_manager().covers().nearest(Device.vCameraPosition,5.f,nearest);
	xr_vector<CCoverPoint*>::const_iterator	I = nearest.begin();
	xr_vector<CCoverPoint*>::const_iterator	E = nearest.end();
	for ( ; I != E; ++I) {
		Fvector				position = (*I)->position();
		position.y			+= 1.f;
		RCache.dbg_DrawAABB	(position,half_size - .01f,1.f,ai().level_graph().header().cell_size()*.5f-.01f,D3DCOLOR_XRGB(0*255,255,0*255));

		CVertex				*v = vertex((*I)->level_vertex_id());
		Fvector				direction;
		float				best_value = -1.f;

		for (u32 i=0, j = 0; i<36; ++i) {
			float				value = cover_in_direction(float(10*i)/180.f*PI,v);
			direction.setHP		(float(10*i)/180.f*PI,0);
			direction.normalize	();
			direction.mul		(value*half_size);
			direction.add		(position);
			direction.y			= position.y;
			RCache.dbg_DrawLINE(Fidentity,position,direction,D3DCOLOR_XRGB(0,0,255));
			value				= compute_square(float(10*i)/180.f*PI,PI/2.f,v);
			if (value > best_value) {
				best_value		= value;
				j				= i;
			}
		}

		direction.set		(position.x - half_size*float(v->cover(0))/15.f,position.y,position.z);
		RCache.dbg_DrawLINE(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		direction.set		(position.x,position.y,position.z + half_size*float(v->cover(1))/15.f);
		RCache.dbg_DrawLINE(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		direction.set		(position.x + half_size*float(v->cover(2))/15.f,position.y,position.z);
		RCache.dbg_DrawLINE(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		direction.set		(position.x,position.y,position.z - half_size*float(v->cover(3))/15.f);
		RCache.dbg_DrawLINE(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		float				value = cover_in_direction(float(10*j)/180.f*PI,v);
		direction.setHP		(float(10*j)/180.f*PI,0);
		direction.normalize	();
		direction.mul		(value*half_size);
		direction.add		(position);
		direction.y			= position.y;
		RCache.dbg_DrawLINE	(Fidentity,position,direction,D3DCOLOR_XRGB(0,0,0));
	}

	{
		Fvector				position = v3d(dest.position);
		position.y			= 1.f;
		RCache.dbg_DrawAABB	(position,ai().level_graph().header().cell_size()*.5f-.01f,1.f,ai().level_graph().header().cell_size()*.5f-.01f,D3DCOLOR_XRGB(0*255,0*255,255));

	}
	
	{
		Fvector				position = v3d(start.position);
		position.y			= 1.f;
		RCache.dbg_DrawAABB	(position,ai().level_graph().header().cell_size()*.5f-.01f,1.f,ai().level_graph().header().cell_size()*.5f-.01f,D3DCOLOR_XRGB(255,255,0*255));
	}
	
	if (m_best_point) {
		Fvector				position = m_best_point->position();
		position.y			+= 1.f;
		RCache.dbg_DrawAABB(position,ai().level_graph().header().cell_size()*.5f-.01f,1.f,ai().level_graph().header().cell_size()*.5f-.01f,D3DCOLOR_XRGB(255,0*255,0*255));
	}
}

void CLevelGraph::on_render5	()
{
	u32	I	= 0;
	u32 E	= Level().Objects.o_count	();
	for ( ; I < E; ++I) {
		CObject*		_O				= Level().Objects.o_get_by_iterator(I);
		CTeamBaseZone	*team_base_zone = smart_cast<CTeamBaseZone*>(_O);
		if (team_base_zone) {
			team_base_zone->OnRender();
			continue;
		}
		CCustomMonster *tpCustomMonster = smart_cast<CCustomMonster*>(_O);
		if (tpCustomMonster) {
			tpCustomMonster->OnRender();
			if (!tpCustomMonster->movement().detail().path().empty()) {
				Fvector temp = tpCustomMonster->movement().detail().path()[tpCustomMonster->movement().detail().path().size() - 1].position;
				RCache.dbg_DrawAABB(temp,1.f,1.f,1.f,D3DCOLOR_XRGB(0,0,255));
			}
		}
		CAI_Stalker		*stalker = smart_cast<CAI_Stalker*>(_O);
		if (!stalker || !stalker->memory().enemy().selected())
			continue;

		CMemoryInfo					mem_object = stalker->memory().memory(stalker->memory().enemy().selected());
		if (!mem_object.m_object)
			continue;
		Fvector						position = mem_object.m_object_params.m_position;
		stalker->m_ce_angle->setup	(position,10.f,170.f,mem_object.m_object_params.m_level_vertex_id);
		const CCoverPoint			*point = ai().cover_manager().best_cover(stalker->Position(),10.f,*stalker->m_ce_angle);
		if (!point)
			continue;
		position					= point->position();
		position.y					+= 1.f;
		float						half_size = header().cell_size()*.5f;
		RCache.dbg_DrawAABB			(position,half_size - .01f,1.f,half_size-.01f,D3DCOLOR_XRGB(0*255,255,0*255));
		float						best_value = -1.f;
		float						m_best_angle = 0.f;
		for (float alpha = 0.f, step = PI_MUL_2/360.f; alpha < PI_MUL_2; alpha += step) {
			float					value = compute_square(alpha,PI,mem_object.m_object_params.m_level_vertex_id);
			if (value > best_value) {
				best_value			= value;
				m_best_angle		= alpha;
			}
		}
		CVertex						*v = vertex(mem_object.m_object_params.m_level_vertex_id);
		position					= vertex_position(mem_object.m_object_params.m_level_vertex_id);
		RCache.dbg_DrawAABB			(position,half_size - .01f,1.f,half_size-.01f,D3DCOLOR_XRGB(255,0*255,0*255));
		
		Fvector						direction;
		direction.set				(position.x - half_size*float(v->cover(0))/15.f,position.y,position.z);
		RCache.dbg_DrawLINE			(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		direction.set				(position.x,position.y,position.z + half_size*float(v->cover(1))/15.f);
		RCache.dbg_DrawLINE			(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		direction.set				(position.x + half_size*float(v->cover(2))/15.f,position.y,position.z);
		RCache.dbg_DrawLINE			(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		direction.set				(position.x,position.y,position.z - half_size*float(v->cover(3))/15.f);
		RCache.dbg_DrawLINE			(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		direction.setHP				(m_best_angle + PI,0.f);
		direction.mul				(half_size*cover_in_direction(m_best_angle,mem_object.m_object_params.m_level_vertex_id));
		direction.add				(position);
		direction.y					+= .5f;
		position.y					+= .5f;
		RCache.dbg_DrawLINE			(Fidentity,position,direction,D3DCOLOR_XRGB(255,255,255));
	}
}

void CLevelGraph::on_render6	()
{
	u32	I	= 0;
	u32 E	= Level().Objects.o_count	();
	for ( ; I < E; ++I) {
		CObject*		_O				= Level().Objects.o_get_by_iterator(I);
		CCustomMonster *tpCustomMonster = smart_cast<CCustomMonster*>(_O);
		if (tpCustomMonster) {
			tpCustomMonster->OnRender();
			if (!tpCustomMonster->movement().detail().path().empty()) {
				Fvector temp = tpCustomMonster->movement().detail().path()[tpCustomMonster->movement().detail().path().size() - 1].position;
				RCache.dbg_DrawAABB(temp,1.f,1.f,1.f,D3DCOLOR_XRGB(0,0,255));
			}
		}
	}
}

#ifdef DEBUG
#ifndef AI_COMPILER
void CLevelGraph::on_render7	()
{
	if (g_bDebugNode) {
		Fvector pos_src, pos_dest;

		if (ai().level_graph().valid_vertex_id(g_dwDebugNodeSource)) {
			pos_src		= ai().level_graph().vertex_position(g_dwDebugNodeSource);
			pos_dest	= pos_src;
			pos_dest.y	+= 10.0f;

			RCache.dbg_DrawAABB(pos_src,0.35f,0.35f,0.35f,D3DCOLOR_XRGB(0,0,255));
			RCache.dbg_DrawLINE(Fidentity,pos_src,pos_dest,D3DCOLOR_XRGB(0,0,255));
		}

		if (ai().level_graph().valid_vertex_id(g_dwDebugNodeDest)) {
			pos_src		= ai().level_graph().vertex_position(g_dwDebugNodeDest);
			pos_dest	= pos_src;
			pos_dest.y	+= 10.0f;

			RCache.dbg_DrawAABB(pos_src,0.35f,0.35f,0.35f,D3DCOLOR_XRGB(255,0,0));
			RCache.dbg_DrawLINE(Fidentity,pos_src,pos_dest,D3DCOLOR_XRGB(255,0,0));
		}
	}
}
#endif
#endif

#endif // AI_COMPILER
#endif // DEBUG