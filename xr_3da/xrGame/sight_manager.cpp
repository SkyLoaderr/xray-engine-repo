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

void CSightManager::reinit			()
{
	m_dwLookChangedTime			= 0;
	m_tLookType					= MonsterSpace::eLookTypePathDirection;
}

void CSightManager::SetPointLookAngles(const Fvector &tPosition, float &yaw, float &pitch)
{
	CCustomMonster	*custom_mosnter = dynamic_cast<CCustomMonster*>(this);
	VERIFY			(custom_mosnter);
	Fvector			tTemp;
	tTemp.sub		(tPosition,custom_mosnter->eye_matrix.c);
	tTemp.getHP		(yaw,pitch);
	VERIFY			(_valid(yaw));
	VERIFY			(_valid(pitch));
	yaw				*= -1;
	pitch			*= -1;
}

void CSightManager::SetFirePointLookAngles(const Fvector &tPosition, float &yaw, float &pitch)
{
	CObject			*object = dynamic_cast<CObject*>(this);
	VERIFY			(object);
	Fvector			tTemp;
	object->Center	(tTemp);
	tTemp.sub		(tPosition,tTemp);
	tTemp.getHP		(yaw,pitch);
	VERIFY			(_valid(yaw));
	VERIFY			(_valid(pitch));
	yaw				*= -1;
	pitch			*= -1;
}

void CSightManager::SetDirectionLook()
{
	CAI_Stalker						*stalker = dynamic_cast<CAI_Stalker*>(this);
	VERIFY							(stalker);
	CMovementManager::SBoneRotation	orientation = stalker->m_head, body_orientation = stalker->body_orientation();
	GetDirectionAngles				(stalker->m_head.target.yaw,stalker->m_head.target.pitch);
	stalker->m_head.target.yaw		*= -1;
	stalker->m_head.target.pitch	= 0;
	stalker->m_body.target			= stalker->m_head.target;
}

void CSightManager::SetLessCoverLook(const CLevelGraph::CVertex *tpNode, bool bDifferenceLook)
{
	SetDirectionLook();
	SetLessCoverLook(tpNode,MAX_HEAD_TURN_ANGLE,bDifferenceLook);
}

void CSightManager::SetLessCoverLook(const CLevelGraph::CVertex *tpNode, float fMaxHeadTurnAngle, bool bDifferenceLook)
{
//	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(this);
//	VERIFY					(stalker);
//	float fAngleOfView		= stalker->eye_fov/180.f*PI, fMaxSquare = -1.f, fBestAngle = stalker->m_head.target.yaw;
//
//	CLevelGraph::CVertex	*tpNextNode = 0;
//	u32						node_id;
//	bool bOk = false;
//	if (bDifferenceLook && !CDetailPathManager::path().empty() && (CDetailPathManager::path().size() - 1 > CDetailPathManager::curr_travel_point_index())) {
//		CLevelGraph::const_iterator	i, e;
//		ai().level_graph().begin(tpNode,i,e);
//		for ( ; i != e; ++i) {
//			node_id			= ai().level_graph().value(tpNode,i);
//			if (!ai().level_graph().valid_vertex_id(node_id))
//				continue;
//			tpNextNode = ai().level_graph().vertex(node_id);
//			if (ai().level_graph().inside(tpNextNode,CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].position)) {
//				bOk = true;
//				break;
//			}
//		}
//	}
//
//	if (!bDifferenceLook || !bOk) 
//		for (float fIncrement = stalker->m_body.current.yaw - fMaxHeadTurnAngle; fIncrement <= stalker->m_body.current.yaw + fMaxHeadTurnAngle; fIncrement += 2*fMaxHeadTurnAngle/60.f) {
//			float fSquare = ai().level_graph().compute_square(-fIncrement,fAngleOfView,tpNode);
//			if (fSquare > fMaxSquare) {
//				fMaxSquare = fSquare;
//				fBestAngle = fIncrement;
//			}
//		}
//	else {
//		float fMaxSquareSingle = -1.f, fSingleIncrement = stalker->m_head.target.yaw;
//		for (float fIncrement = stalker->m_body.current.yaw - fMaxHeadTurnAngle; fIncrement <= stalker->m_body.current.yaw + fMaxHeadTurnAngle; fIncrement += 2*fMaxHeadTurnAngle/60.f) {
//			float fSquare0 = ai().level_graph().compute_square(-fIncrement,fAngleOfView,tpNode);
//			float fSquare1 = ai().level_graph().compute_square(-fIncrement,fAngleOfView,tpNextNode);
//			if (fSquare1 - fSquare0 > fMaxSquare) {
//				fMaxSquare = fSquare1 - fSquare0;
//				fBestAngle = fIncrement;
//			}
//			if (fSquare0 > fMaxSquareSingle) {
//				fMaxSquareSingle = fSquare0;
//				fSingleIncrement = fIncrement;
//			}
//		}
//		if (_sqrt(fMaxSquare) < 0*PI_DIV_6)
//			fBestAngle = fSingleIncrement;
//	}
//
//	stalker->m_head.target.yaw = fBestAngle;
//	VERIFY					(_valid(stalker->m_head.target.yaw));
}

bool CSightManager::bfIf_I_SeePosition(Fvector tPosition) const
{
	const CAI_Stalker	*stalker = dynamic_cast<const CAI_Stalker*>(this);
	VERIFY				(stalker);
	float				yaw, pitch;
	Fvector				tVector;
	tVector.sub			(tPosition,stalker->Position());
	tVector.getHP		(yaw,pitch);
	yaw					= angle_normalize_signed(-yaw);
	pitch				= angle_normalize_signed(-pitch);
	return				(angle_difference(yaw,stalker->m_head.current.yaw) <= PI_DIV_6);// && angle_difference(pitch,stalker->m_head.current.pitch,PI_DIV_6));
}

void CSightManager::vfValidateAngleDependency(float x1, float &x2, float x3)
{
	float	_x2	= angle_normalize_signed(x2 - x1);
	float	_x3	= angle_normalize_signed(x3 - x1);
	if ((_x2*_x3 <= 0) && (_abs(_x2) + _abs(_x3) > PI - EPS_L))
		x2  = x3;
}

void CSightManager::Exec_Look(float dt)
{
	CAI_Stalker			*stalker = dynamic_cast<CAI_Stalker*>(this);
	VERIFY				(stalker);
	// normalizing torso angles
	stalker->m_body.current.yaw		= angle_normalize_signed	(stalker->m_body.current.yaw);
	stalker->m_body.current.pitch	= angle_normalize_signed	(stalker->m_body.current.pitch);
	stalker->m_body.target.yaw		= angle_normalize_signed	(stalker->m_body.target.yaw);
	stalker->m_body.target.pitch	= angle_normalize_signed	(stalker->m_body.target.pitch);

	// normalizing head angles
	stalker->m_head.current.yaw		= angle_normalize_signed	(stalker->m_head.current.yaw);
	stalker->m_head.current.pitch	= angle_normalize_signed	(stalker->m_head.current.pitch);
	stalker->m_head.target.yaw		= angle_normalize_signed	(stalker->m_head.target.yaw);
	stalker->m_head.target.pitch	= angle_normalize_signed	(stalker->m_head.target.pitch);

	// validating angles
	//#ifdef DEBUG
	//	Msg						("StalkerA (%d, %s) t=%f, c=%f, tt=%f, tc=%f",Level().timeServer(),cName(),stalker->m_head.target.yaw,stalker->m_head.current.yaw,stalker->m_body.target.yaw,stalker->m_body.current.yaw);
	VERIFY					(_valid(stalker->m_head.current.yaw));
	VERIFY					(_valid(stalker->m_head.current.pitch));
	VERIFY					(_valid(stalker->m_head.target.yaw));
	VERIFY					(_valid(stalker->m_head.target.pitch));
	VERIFY					(_valid(stalker->m_body.current.yaw));
	VERIFY					(_valid(stalker->m_body.current.pitch));
	VERIFY					(_valid(stalker->m_body.target.yaw));
	VERIFY					(_valid(stalker->m_body.target.pitch));
	//#endif
	vfValidateAngleDependency(stalker->m_head.current.yaw,stalker->m_head.target.yaw,stalker->m_body.target.yaw);
	vfValidateAngleDependency(stalker->m_body.current.yaw,stalker->m_body.target.yaw,stalker->m_head.current.yaw);

	// updating torso angles
	float							fSpeedFactor = 1.f;
	stalker->angle_lerp_bounds		(stalker->m_body.current.yaw,stalker->m_body.target.yaw,fSpeedFactor*stalker->m_body.speed,dt);
	stalker->angle_lerp_bounds		(stalker->m_body.current.pitch,stalker->m_body.target.pitch,stalker->m_body.speed,dt);

	// updating head angles
	stalker->angle_lerp_bounds		(stalker->m_head.current.yaw,stalker->m_head.target.yaw,stalker->m_head.speed,dt);
	stalker->angle_lerp_bounds		(stalker->m_head.current.pitch,stalker->m_head.target.pitch,stalker->m_head.speed,dt);

	// normalizing torso angles
	stalker->m_body.current.yaw		= angle_normalize_signed	(stalker->m_body.current.yaw);
	stalker->m_body.current.pitch	= angle_normalize_signed	(stalker->m_body.current.pitch);

	// normalizing head angles
	stalker->m_head.current.yaw		= angle_normalize_signed	(stalker->m_head.current.yaw);
	stalker->m_head.current.pitch	= angle_normalize_signed	(stalker->m_head.current.pitch);

	VERIFY					(_valid(stalker->m_head.current.yaw));
	VERIFY					(_valid(stalker->m_head.current.pitch));
	VERIFY					(_valid(stalker->m_head.target.yaw));
	VERIFY					(_valid(stalker->m_head.target.pitch));
	VERIFY					(_valid(stalker->m_body.current.yaw));
	VERIFY					(_valid(stalker->m_body.current.pitch));
	VERIFY					(_valid(stalker->m_body.target.yaw));
	VERIFY					(_valid(stalker->m_body.target.pitch));
}

void CSightManager::update		(MonsterSpace::ELookType tLookType, const Fvector *tPointToLook, u32 dwLookOverDelay)
{
	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(this);
	VERIFY					(stalker);

	bool					bLookChanged = (m_tLookType != tLookType);
	m_tLookType				= tLookType;

	switch (m_tLookType) {
		case eLookTypePathDirection : {
			SetDirectionLook();
			break;
		}
		case eLookTypeDirection : {
			tPointToLook->getHP	(stalker->m_head.target.yaw,stalker->m_head.target.pitch);
			stalker->m_head.target.yaw		*= -1;
			stalker->m_head.target.pitch		*= -1;
			break;
		}
		case eLookTypeSearch : {
			SetLessCoverLook(stalker->level_vertex(),true);
			break;
		}
		case eLookTypeDanger : {
			SetLessCoverLook(stalker->level_vertex(),PI,true);
			break;
		}
		case eLookTypePoint : {
			SetPointLookAngles(*tPointToLook,stalker->m_head.target.yaw,stalker->m_head.target.pitch);
			break;
		}
		case eLookTypeFirePoint : {
			SetFirePointLookAngles(*tPointToLook,stalker->m_head.target.yaw,stalker->m_head.target.pitch);
			break;
		}
		case eLookTypeLookOver : {
			if (bLookChanged)
				m_dwLookChangedTime = Level().timeServer();
			Fvector tTemp;
			tTemp.sub				(*tPointToLook,stalker->eye_matrix.c);
			tTemp.getHP				(stalker->m_head.target.yaw,stalker->m_head.target.pitch);
			VERIFY					(_valid(stalker->m_head.target.yaw));
			VERIFY					(_valid(stalker->m_head.target.pitch));
			if (Level().timeServer() - m_dwLookChangedTime > dwLookOverDelay)
				if (Level().timeServer() - m_dwLookChangedTime < 2*dwLookOverDelay)
					stalker->m_head.target.yaw += PI_DIV_6*2;
				else {
					if (Level().timeServer() - m_dwLookChangedTime >= 3*dwLookOverDelay)
						m_dwLookChangedTime = Level().timeServer();
					stalker->m_head.target.yaw -= PI_DIV_6*2;
				}
			stalker->m_head.target.yaw *= -1;
			stalker->m_head.target.pitch *= -1;
			break;
		}
		case eLookTypeLookFireOver : {
			if (bLookChanged)
				m_dwLookChangedTime = Level().timeServer();
			Fvector tTemp;
			stalker->Center(tTemp);
			tTemp.sub	(*tPointToLook,tTemp);
			tTemp.getHP	(stalker->m_head.target.yaw,stalker->m_head.target.pitch);
			VERIFY					(_valid(stalker->m_head.target.yaw));
			VERIFY					(_valid(stalker->m_head.target.pitch));
			if (Level().timeServer() - m_dwLookChangedTime > dwLookOverDelay)
				if (Level().timeServer() - m_dwLookChangedTime < 2*dwLookOverDelay)
					stalker->m_head.target.yaw += PI_DIV_6*2;
				else {
					if (Level().timeServer() - m_dwLookChangedTime >= 3*dwLookOverDelay)
						m_dwLookChangedTime = Level().timeServer();
					stalker->m_head.target.yaw -= PI_DIV_6*2;
				}
			stalker->m_head.target.yaw *= -1;
			stalker->m_head.target.pitch *= -1;
			break;
		}
		default : NODEFAULT;
	}
	
	if (stalker->speed() < EPS_L)
		if (angle_difference(stalker->m_body.target.yaw,stalker->m_head.target.yaw) > 2*PI_DIV_6)
			stalker->m_body.target.yaw = stalker->m_head.target.yaw;
}