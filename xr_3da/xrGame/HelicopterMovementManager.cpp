#include "stdafx.h"
#include "Helicopter.h"
#include "level.h"
#include "patrol_path.h"
#include "patrol_path_storage.h"
#include "script_game_object.h"
#include "game_object_space.h"
#include "script_callback_ex.h"


SHeliMovementState::~SHeliMovementState()
{
	if(need_to_del_path&&currPatrolPath){
		CPatrolPath* tmp = const_cast<CPatrolPath*>(currPatrolPath);
		xr_delete( tmp );
	}

}

void SHeliMovementState::Update()
{
	switch(type){
		case eMovNone:
			break;
		case eMovToPoint:
			UpdateMovToPoint();
			break;
		case eMovPatrolPath:
			UpdatePatrolPath();
			break;
		case eMovLanding:
		case eMovTakeOff:
			break;
		default:
			NODEFAULT;
	};
}

void SHeliMovementState::reinit()
{
	type						= eMovNone;
	currPatrolPath				= NULL;
	currPatrolVertex			= NULL;
	patrol_begin_idx			= 0;
	patrol_path_name			= "";
	need_to_del_path			= false;
	currP.set					(0.0f,0.0f,0.0f);
	currPathH					= 0.0f;
	currPathP					= 0.0f;
}
float SHeliMovementState::GetDistanceToDestPosition()
{
	return desiredPoint.distance_to(currP);
}

void SHeliMovementState::UpdatePatrolPath()
{
	if( AlreadyOnPoint() ){

		float dist = GetDistanceToDestPosition();
		parent->callback(GameObject::eHelicopterOnPoint)(dist,currP, currPatrolVertex ? currPatrolVertex->vertex_id() : -1);
		CPatrolPath::const_iterator b,e;
		currPatrolPath->begin(currPatrolVertex,b,e);
		if(b!=e){
			currPatrolVertex =  currPatrolPath->vertex((*b).vertex_id());
			
			Fvector p = currPatrolVertex->data().position();
			SetDestPosition(&p);
			type = eMovPatrolPath;

		}else{
			type = eMovNone;
			curLinearSpeed	= 0.0f;
			curLinearAcc	= 0.0f;
		}
	}
}

void SHeliMovementState::UpdateMovToPoint()
{
	if(	AlreadyOnPoint() ){
		float dist = GetDistanceToDestPosition();
		parent->callback(GameObject::eHelicopterOnPoint)(dist,currP, -1);
		type = eMovNone;
		curLinearSpeed	= 0.0f;
		curLinearAcc	= 0.0f;
	}
}
extern float STEP;
bool SHeliMovementState::AlreadyOnPoint()
{
	float dist = GetDistanceToDestPosition();
	if(dist<=0.1f)return true;

	if(	dist < onPointRangeDist ){
		Fvector P1 = currP;
		Fvector dir;
		dir.setHP(currPathH,0.0f);
		P1.mad(dir, curLinearSpeed*STEP);
		float new_dist = desiredPoint.distance_to(P1);
		return (new_dist>dist);
	}
	return false;
}

void SHeliMovementState::getPathAltitude (Fvector& point, float base_altitude)
{
	Fbox	boundingVolume = Level().ObjectSpace.GetBoundingVolume();
	Fvector boundSize;
	boundingVolume.getsize(boundSize);

	collide::rq_result		cR;
	Fvector down_dir;
	down_dir.set(0.0f, -1.0f, 0.0f);

	point.y = boundingVolume.max.y+EPS_L;
	VERIFY( _valid(point) );

	Level().ObjectSpace.RayPick(point, down_dir, boundSize.y+1.0f, collide::rqtStatic, cR);
	
	point.y = point.y-cR.range;

	if( point.y+base_altitude < boundingVolume.max.y )
		point.y += base_altitude;
	else
		point.y = boundingVolume.max.y-EPS_L;

	VERIFY( _valid(point) );

	float minY = boundingVolume.min.y;//+(m_boundingVolume.max.y-m_boundingVolume.min.y)*m_heli->m_data.m_alt_korridor;
	float maxY = boundingVolume.max.y+base_altitude;
	clamp (point.y,minY,maxY);
	VERIFY( _valid(point) );

}
void SHeliMovementState::SetDestPosition(Fvector* pos)
{
	desiredPoint = *pos;
//	getPathAltitude(desiredPoint, wrk_altitude);
	type = eMovToPoint;
}

void SHeliMovementState::goPatrolByPatrolPath (LPCSTR path_name, int start_idx)
{
	if(need_to_del_path&&currPatrolPath){
		CPatrolPath* tmp = const_cast<CPatrolPath*>(currPatrolPath);
		xr_delete( tmp );
	}

	patrol_begin_idx = start_idx;
	patrol_path_name = path_name;

	currPatrolPath = Level().patrol_paths().path(patrol_path_name);
	need_to_del_path = false;
	currPatrolVertex =  currPatrolPath->vertex(patrol_begin_idx);

	desiredPoint = currPatrolVertex->data().position();
//	getPathAltitude(desiredPoint, wrk_altitude);
	
	type = eMovPatrolPath;
}

void SHeliMovementState::save(NET_Packet &output_packet)
{
	output_packet.w_s16		((s16)type);
	output_packet.w_u32		(patrol_begin_idx);
	output_packet.w_stringZ	(patrol_path_name);

	output_packet.w_float	(maxLinearSpeed);
	output_packet.w_float	(LinearAcc_fw);
	output_packet.w_float	(LinearAcc_bk);

	output_packet.w_float	(angularSpeedPitch);
	output_packet.w_float	(angularSpeedHeading);
	output_packet.w_float	(speedInDestPoint);
	
	output_packet.w_vec3	(desiredPoint);

	output_packet.w_float	(curLinearSpeed);
	output_packet.w_float	(curLinearAcc);

	output_packet.w_vec3	(currP);


	output_packet.w_float	(currPathH);
	output_packet.w_float	(currPathP);
	
	output_packet.w_vec3	(round_center);

	output_packet.w_float	(round_radius);
	
	output_packet.w_u8		(round_reverse ? 1 : 0);

	output_packet.w_float	(onPointRangeDist);

	if(type==eMovPatrolPath){
		output_packet.w_s32( currPatrolVertex->vertex_id() );
	}
}

void SHeliMovementState::load(IReader &input_packet)
{
	type			=		(EHeilMovementState)input_packet.r_s16();
	patrol_begin_idx=		input_packet.r_u32();
	input_packet.r_stringZ	(patrol_path_name);

	maxLinearSpeed	=		input_packet.r_float();
	LinearAcc_fw	=		input_packet.r_float();
	LinearAcc_bk	=		input_packet.r_float();

	angularSpeedPitch	=	input_packet.r_float();
	angularSpeedHeading =	input_packet.r_float();
	speedInDestPoint	=	input_packet.r_float();

	input_packet.r_fvector3(desiredPoint);

	curLinearSpeed		=	input_packet.r_float();
	curLinearAcc		=	input_packet.r_float();

	input_packet.r_fvector3		(currP);


	currPathH			=	input_packet.r_float();
	currPathP			=	input_packet.r_float();

	input_packet.r_fvector3		(round_center);

	round_radius		=	input_packet.r_float();

	round_reverse		=	!!input_packet.r_u8();

	onPointRangeDist	=	input_packet.r_float();

	if(type==eMovPatrolPath){
		currPatrolPath = Level().patrol_paths().path(patrol_path_name);
		int idx = input_packet.r_s32();
		currPatrolVertex =  currPatrolPath->vertex(idx);
	}

}


void SHeliMovementState::goByRoundPath(Fvector center_, float radius_, bool clockwise_)
{
/*	round_center	= center_;
	round_radius	= radius_;
	round_reverse	= !clockwise_;

	if(need_to_del_path&&currPatrolPath){
		CPatrolPath* tmp = const_cast<CPatrolPath*>(currPatrolPath);
		xr_delete( tmp );
	}

	CPatrolPath* pp = xr_new<CPatrolPath>();
	need_to_del_path = true;
	xr_vector<Fvector> pts;
	//fill new path points
	Fvector p,center,dir,dir_norm;
	Fvector cur_pos = currP;
	center = round_center;
	float radius = round_radius;

	dir.sub(cur_pos,center).normalize_safe();
	dir_norm.set(-dir.z, 0.0f, dir.x);
	p.mad(center,dir,radius);
	getPathAltitude(p, wrk_altitude);
	pts.push_back(p);


	p.mad(center,dir_norm,radius);
	getPathAltitude(p, wrk_altitude);
	pts.push_back(p);

	p.mad(center,dir,-radius);
	getPathAltitude(p, wrk_altitude);
	pts.push_back(p);

	p.mad(center,dir_norm,-radius);
	getPathAltitude(p, wrk_altitude);
	pts.push_back(p);

	if(!round_reverse){//clockwise
		xr_vector<Fvector>::iterator i = pts.begin();
		++i;
		std::reverse(i ,pts.end() );
	}


	xr_vector<Fvector>::iterator it = pts.begin();
	for(u32 idx=0;it!=pts.end();++it,++idx){//create patrol path
		CPatrolPoint pt = CPatrolPoint(*it,0,0,"");
		pp->add_vertex(pt,idx);
		if (idx)
			pp->add_edge(idx-1,idx,1.f);
	}
	pp->add_edge(idx-1,0,1.f);

	currPatrolPath = pp;
	currPatrolVertex =  currPatrolPath->vertex(0);

	desiredPoint = currPatrolVertex->data().position();
	getPathAltitude(desiredPoint, wrk_altitude);

	type = eMovPatrolPath;*/
}

#ifdef DEBUG
void CHelicopter::OnRender()
{
/*	RCache.dbg_DrawLINE(Fidentity,m_heli->m_right_rocket_bone_xform.c, m_heli->m_data.m_destEnemyPos,D3DCOLOR_XRGB(0,255,0));

	RCache.dbg_DrawLINE(Fidentity,m_heli->XFORM().c,m_heli->m_data.m_destEnemyPos,D3DCOLOR_XRGB(255,0,0));
	return;
*/
//	float t = Level().timeServer()/1000.0f;
//	Fvector P,R;
//	_Evaluate(t,P,R);
//	RCache.dbg_DrawAABB  (P,0.1f,0.1f,0.1f,D3DCOLOR_XRGB(255,0,0));
///*	Fvector P_,R_;
//	_Evaluate(t+1.0f,P_,R_);
//	float s = P_.distance_to(P);
//
//	HUD().pFontSmall->SetColor(color_rgba(0xff,0xff,0xff,0xff));
//	HUD().pFontSmall->OutSet	(320,630);
//	HUD().pFontSmall->OutNext("Motion Speed:			[%3.2f]",s);
//*/
//	if(bDrawInterpolated){
//	
//	FvectorVec path_points;
//	FvectorVec path_rot;
//
//	float min_t				= m_startTime;
//	float max_t				= m_endTime;
//
//	Fvector 				T,r;
//	path_points.clear		();
//	for (float t=min_t; (t<max_t)||fsimilar(t,max_t,EPS_L); t+=dTime){
//		_Evaluate(t,T,r);
//		path_points.push_back(T);
//		path_rot.push_back(r);
//	}
//
//	float path_box_size = .05f;
//	for(u32 i = 0; i<path_points.size (); ++i) {
//		RCache.dbg_DrawAABB  (path_points[i],path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));
//	}	
///*		r.setHP(path_rot[i].y, path_rot[i].x );
//		r.mul (3.0f);
//		T.add (path_points[i],r);
//		RCache.dbg_DrawLINE (Fidentity, path_points[i], T, D3DCOLOR_XRGB(255,0,0));
//*/
//	}
//
//	if(bDrawKeys){
//		float key_box_size = .25f;
//		u32 cnt = KeyCount();
//		for(u32 ii=0;ii<cnt;++ii) {
//			Fvector _t;
//			Fvector _r;
//			GetKey (ii,_t,_r);
//			RCache.dbg_DrawAABB  (_t,key_box_size,key_box_size,key_box_size,D3DCOLOR_XRGB(0,255,255));
//
////			_r.setHP(_r.y, _r.x );
////			_r.mul (6.0f);
////			TT.add (_t,_r);
////			RCache.dbg_DrawLINE (Fidentity, _t, TT, D3DCOLOR_XRGB(255,0,0));
//		}
//	}
//

}
#endif

