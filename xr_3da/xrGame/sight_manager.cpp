////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sight manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sight_manager.h"
#include "custommonster.h"
#include "ai/stalker/ai_stalker.h"

CSightManager::CSightManager		()
{
	Init						();
}

CSightManager::~CSightManager		()
{
}

void CSightManager::Init			()
{
}

void CSightManager::Load			(LPCSTR section)
{
}

void CSightManager::reinit			(CAI_Stalker *object)
{
	inherited::reinit			(object);
}

void CSightManager::SetPointLookAngles(const Fvector &tPosition, float &yaw, float &pitch)
{
	Fvector			tTemp;
	tTemp.sub		(tPosition,m_object->eye_matrix.c);
	tTemp.getHP		(yaw,pitch);
	VERIFY			(_valid(yaw));
	VERIFY			(_valid(pitch));
	yaw				*= -1;
	pitch			*= -1;
}

void CSightManager::SetFirePointLookAngles(const Fvector &tPosition, float &yaw, float &pitch)
{
	Fvector			tTemp;
	m_object->Center(tTemp);
	tTemp.sub		(tPosition,tTemp);
	tTemp.getHP		(yaw,pitch);
	VERIFY			(_valid(yaw));
	VERIFY			(_valid(pitch));
	yaw				*= -1;
	pitch			*= -1;
}

void CSightManager::SetDirectionLook()
{
	MonsterSpace::SBoneRotation		orientation = m_object->m_head, body_orientation = m_object->body_orientation();
	GetDirectionAngles				(m_object->m_head.target.yaw,m_object->m_head.target.pitch);
	m_object->m_head.target.yaw		*= -1;
	m_object->m_head.target.pitch	*= 0;//-1;
	m_object->m_body.target			= m_object->m_head.target;
}

void CSightManager::SetLessCoverLook(const CLevelGraph::CVertex *tpNode, bool bDifferenceLook)
{
	SetDirectionLook();
	SetLessCoverLook(tpNode,MAX_HEAD_TURN_ANGLE,bDifferenceLook);
}

void CSightManager::SetLessCoverLook(const CLevelGraph::CVertex *tpNode, float fMaxHeadTurnAngle, bool bDifferenceLook)
{
	float fAngleOfView		= m_object->eye_fov/180.f*PI, fMaxSquare = -1.f, fBestAngle = m_object->m_head.target.yaw;

	CLevelGraph::CVertex	*tpNextNode = 0;
	u32						node_id;
	bool bOk = false;
	if (bDifferenceLook && !m_object->CDetailPathManager::path().empty() && (m_object->CDetailPathManager::path().size() - 1 > m_object->CDetailPathManager::curr_travel_point_index())) {
		CLevelGraph::const_iterator	i, e;
		ai().level_graph().begin(tpNode,i,e);
		for ( ; i != e; ++i) {
			node_id			= ai().level_graph().value(tpNode,i);
			if (!ai().level_graph().valid_vertex_id(node_id))
				continue;
			tpNextNode = ai().level_graph().vertex(node_id);
			if (ai().level_graph().inside(tpNextNode,m_object->CDetailPathManager::path()[m_object->CDetailPathManager::curr_travel_point_index() + 1].position)) {
				bOk = true;
				break;
			}
		}
	}

	if (!bDifferenceLook || !bOk) 
		for (float fIncrement = m_object->m_body.target.yaw - fMaxHeadTurnAngle; fIncrement <= m_object->m_body.target.yaw + fMaxHeadTurnAngle; fIncrement += 2*fMaxHeadTurnAngle/60.f) {
			float fSquare = ai().level_graph().compute_square(-fIncrement + PI,fAngleOfView,tpNode);
			if (fSquare > fMaxSquare) {
				fMaxSquare = fSquare;
				fBestAngle = fIncrement;
			}
		}
	else {
		float fMaxSquareSingle = -1.f, fSingleIncrement = m_object->m_head.target.yaw;
		for (float fIncrement = m_object->m_body.target.yaw - fMaxHeadTurnAngle; fIncrement <= m_object->m_body.target.yaw + fMaxHeadTurnAngle; fIncrement += 2*fMaxHeadTurnAngle/60.f) {
			float fSquare0 = ai().level_graph().compute_square(-fIncrement + PI,fAngleOfView,tpNode);
			float fSquare1 = ai().level_graph().compute_square(-fIncrement + PI,fAngleOfView,tpNextNode);
			if	(
					(fSquare1 - fSquare0 > fMaxSquare) || 
					(
						fsimilar(fSquare1 - fSquare0,fMaxSquare,EPS_L) && 
						(_abs(fIncrement - m_object->m_body.target.yaw) < _abs(fBestAngle - m_object->m_body.target.yaw))
					)
				)
			{
				fMaxSquare = fSquare1 - fSquare0;
				fBestAngle = fIncrement;
			}

			if (fSquare0 > fMaxSquareSingle) {
				fMaxSquareSingle = fSquare0;
				fSingleIncrement = fIncrement;
			}
		}
		if (_sqrt(fMaxSquare) < 0*PI_DIV_6)
			fBestAngle = fSingleIncrement;
	}

	m_object->m_head.target.yaw = angle_normalize_signed(fBestAngle);
	m_object->m_head.target.pitch = 0;
	VERIFY					(_valid(m_object->m_head.target.yaw));
}

bool CSightManager::bfIf_I_SeePosition(Fvector tPosition) const
{
	float				yaw, pitch;
	Fvector				tVector;
	tVector.sub			(tPosition,m_object->Position());
	tVector.getHP		(yaw,pitch);
	yaw					= angle_normalize_signed(-yaw);
	pitch				= angle_normalize_signed(-pitch);
	return				(angle_difference(yaw,m_object->m_head.current.yaw) <= PI_DIV_6);// && angle_difference(pitch,m_object->m_head.current.pitch,PI_DIV_6));
}

void CSightManager::vfValidateAngleDependency(float x1, float &x2, float x3)
{
	float	_x2	= angle_normalize_signed(x2 - x1);
	float	_x3	= angle_normalize_signed(x3 - x1);
	if ((_x2*_x3 <= 0) && (_abs(_x2) + _abs(_x3) > PI - EPS_L))
		x2  = x3;
}

bool CSightManager::need_correction	(float x1, float x2, float x3)
{
	float	_x2	= angle_normalize_signed(x2 - x1);
	float	_x3	= angle_normalize_signed(x3 - x1);
	if ((_x2*_x3 <= 0) && (_abs(_x2) + _abs(_x3) > PI - EPS_L))
		return			(true);
	return				(false);
}

void CSightManager::Exec_Look		(float dt)
{
	// normalizing torso angles
	m_object->m_body.current.yaw	= angle_normalize_signed	(m_object->m_body.current.yaw);
	m_object->m_body.current.pitch	= angle_normalize_signed	(m_object->m_body.current.pitch);
	m_object->m_body.target.yaw		= angle_normalize_signed	(m_object->m_body.target.yaw);
	m_object->m_body.target.pitch	= angle_normalize_signed	(m_object->m_body.target.pitch);

	// normalizing head angles
	m_object->m_head.current.yaw	= angle_normalize_signed	(m_object->m_head.current.yaw);
	m_object->m_head.current.pitch	= angle_normalize_signed	(m_object->m_head.current.pitch);
	m_object->m_head.target.yaw		= angle_normalize_signed	(m_object->m_head.target.yaw);
	m_object->m_head.target.pitch	= angle_normalize_signed	(m_object->m_head.target.pitch);

	// validating angles
	VERIFY							(_valid(m_object->m_head.current.yaw));
	VERIFY							(_valid(m_object->m_head.current.pitch));
	VERIFY							(_valid(m_object->m_head.target.yaw));
	VERIFY							(_valid(m_object->m_head.target.pitch));
	VERIFY							(_valid(m_object->m_body.current.yaw));
	VERIFY							(_valid(m_object->m_body.current.pitch));
	VERIFY							(_valid(m_object->m_body.target.yaw));
	VERIFY							(_valid(m_object->m_body.target.pitch));

	// updating torso angles
	float							fSpeedFactor = 1.f;

//	bool							head_correction = need_correction(m_object->m_head.current.yaw,m_object->m_head.target.yaw,m_object->m_body.target.yaw);
//	bool							body_correction = need_correction(m_object->m_body.current.yaw,m_object->m_body.target.yaw,m_object->m_head.current.yaw);

//	Msg								("STALKER[%6d] : BODY [%f]->[%f], HEAD [%f]->[%f]",Level().timeServer(),m_object->m_body.current.yaw,m_object->m_body.target.yaw,m_object->m_head.current.yaw,m_object->m_head.target.yaw);
//	if (head_correction)
//		if (body_correction) 
//			vfValidateAngleDependency(m_object->m_body.current.yaw,m_object->m_body.target.yaw,m_object->m_head.current.yaw);
//		else
//			vfValidateAngleDependency(m_object->m_head.current.yaw,m_object->m_head.target.yaw,m_object->m_body.target.yaw);
//	else
//		if (body_correction)
			vfValidateAngleDependency(m_object->m_body.current.yaw,m_object->m_body.target.yaw,m_object->m_head.current.yaw);

	m_object->angle_lerp_bounds		(m_object->m_body.current.yaw,m_object->m_body.target.yaw,fSpeedFactor*m_object->m_body.speed,dt);
	m_object->angle_lerp_bounds		(m_object->m_body.current.pitch,m_object->m_body.target.pitch,m_object->m_body.speed,dt);

	m_object->angle_lerp_bounds		(m_object->m_head.current.yaw,m_object->m_head.target.yaw,m_object->m_head.speed,dt);
	m_object->angle_lerp_bounds		(m_object->m_head.current.pitch,m_object->m_head.target.pitch,m_object->m_head.speed,dt);

	// normalizing torso angles
	m_object->m_body.current.yaw	= angle_normalize_signed	(m_object->m_body.current.yaw);
	m_object->m_body.current.pitch	= angle_normalize_signed	(m_object->m_body.current.pitch);

	// normalizing head angles
	m_object->m_head.current.yaw	= angle_normalize_signed	(m_object->m_head.current.yaw);
	m_object->m_head.current.pitch	= angle_normalize_signed	(m_object->m_head.current.pitch);

	VERIFY							(_valid(m_object->m_head.current.yaw));
	VERIFY							(_valid(m_object->m_head.current.pitch));
	VERIFY							(_valid(m_object->m_head.target.yaw));
	VERIFY							(_valid(m_object->m_head.target.pitch));
	VERIFY							(_valid(m_object->m_body.current.yaw));
	VERIFY							(_valid(m_object->m_body.current.pitch));
	VERIFY							(_valid(m_object->m_body.target.yaw));
	VERIFY							(_valid(m_object->m_body.target.pitch));
}

void CSightManager::setup			(const SightManager::ESightType &sight_type, const Fvector *vector3d, u32 interval)
{
	setup							(CSightAction(sight_type,vector3d));
}

void CSightManager::setup			(const CSightAction &sight_action)
{
	if (m_actions.size() > 1)
		clear			();
	if (!m_actions.empty() && (*(*m_actions.begin()).second == sight_action))
		return;
	clear				();
	add_action			(0,xr_new<CSightControlAction>(1.f,u32(-1),sight_action));
}

void CSightManager::update			(u32 time_delta)
{
	inherited::update	(time_delta);
	
	if	(
			(m_object->speed() < EPS_L) && 
			(angle_difference(m_object->m_body.target.yaw,m_object->m_head.target.yaw) > PI_DIV_6)// &&
//			fsimilar(m_object->m_head.target.yaw,m_object->m_head.current.yaw)
		)
		m_object->m_body.target.yaw = m_object->m_head.target.yaw;
}