////////////////////////////////////////////////////////////////////////////
//	Module 		: visual_memory_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Visual memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "visual_memory_manager.h"

CVisualMemoryManager::CVisualMemoryManager		()
{
	Init				();
}

CVisualMemoryManager::~CVisualMemoryManager		()
{
}

void CVisualMemoryManager::Init					()
{
	m_max_object_count			= 0;
}

void CVisualMemoryManager::Load					(LPCSTR section)
{
	CGameObject::Load			(section);
	// visibility
//	m_dwMovementIdleTime		= pSettings->r_s32(section,"MovementIdleTime");
//	m_fMaxInvisibleSpeed		= pSettings->r_float(section,"MaxInvisibleSpeed");
//	m_fMaxViewableSpeed			= pSettings->r_float(section,"MaxViewableSpeed");
//	m_fMovementSpeedWeight		= pSettings->r_float(section,"MovementSpeedWeight");
//	m_fDistanceWeight			= pSettings->r_float(section,"DistanceWeight");
//	m_fSpeedWeight				= pSettings->r_float(section,"SpeedWeight");
//	m_fVisibilityThreshold		= pSettings->r_float(section,"VisibilityThreshold");
//	m_fLateralMultiplier		= pSettings->r_float(section,"LateralMultiplier");
//	m_fShadowWeight				= pSettings->r_float(section,"ShadowWeight");

	
	m_max_object_count		= pSettings->r_s32(section,"DynamicObjectsCount");
}

void CVisualMemoryManager::reinit					()
{
	CGameObject::reinit			();
	m_objects					= 0;
	m_visible_objects.clear		();
	m_visible_objects.reserve	(100);
}

void CVisualMemoryManager::reload				(LPCSTR section)
{
}

bool CVisualMemoryManager::visible(const CGameObject *game_object) const
{
	VERIFY				(game_object);
	
	if (game_object->getDestroy())
		return			(false);

	
	const CCustomMonster	*m_object = dynamic_cast<const CCustomMonster*>(this);
	if (!m_object)
		return			(true);

	Fmatrix								&eye_matrix = PKinematics(m_object->Visual())->LL_GetTransform(u16(m_object->eye_bone));
	VERIFY								(_valid(eye_matrix));
	const CAI_Stalker					*stalker = dynamic_cast<const CAI_Stalker*>(this);
	const MonsterSpace::SBoneRotation	&rotation = stalker ? stalker->head_orientation() : m_body;
	Fvector								current_direction, object_direction;
	current_direction.setHP				(-rotation.current.yaw,-rotation.current.pitch);
	game_object->Center					(object_direction);
	float								object_distance = object_direction.distance_to(eye_matrix.c);
	object_direction.sub				(eye_matrix.c);
	object_direction.normalize_safe		();
	float								cos_alpha = current_direction.dot_product(object_direction);
	clamp								(cos_alpha,-.99999f,.99999f);
	float								alpha = acosf(cos_alpha);
	float								fov = deg2rad(m_object->eye_fov)*.5f;
	float								distance = (1.f - (fov - alpha)/fov)*(m_max_view_distance - m_min_view_distance) + m_min_view_distance;
	if (distance < object_distance)
		return							(false);
	
	return								(true);
////	if (Level().iGetKeyState(DIK_RCONTROL))
////		return(false);
//#ifdef LOG_PARAMETERS
////	bool		bMessage = g_Alive() && !!dynamic_cast<CActor*>(tpEntity);//!!Levsssssssssssssssssssssel().iGetKeyState(DIK_LALT);
//	int			iLogParameters = (g_Alive() && !!dynamic_cast<CActor*>(tpEntity)) ? (Level().iGetKeyState(DIK_1) ? 2 : Level().iGetKeyState(DIK_0) ? 1 : 0) : 0;
//	string4096	S = "";
//#endif
//	float fResult = 0.f;
//	
//	// computing maximum viewable distance in the specified direction
//	float fDistance = Position().distance_to(tpEntity->Position()), yaw, pitch;
//	Fvector tDirection;
//	tDirection.sub(tpEntity->Position(),Position());
//	tDirection.getHP(yaw,pitch);
//
//	float fEyeFov = eye_fov*PI/180.f, fAlpha = _abs(_min(angle_normalize_signed(yaw - m_head.current.yaw),angle_normalize_signed(pitch - m_head.current.pitch)));
//	float fMaxViewableDistanceInDirection = eye_range*(1 - fAlpha/(fEyeFov/m_fLateralMultiplier));
//	
//	// computing distance weight
//	fResult += fDistance >= fMaxViewableDistanceInDirection ? 0.f : m_fDistanceWeight*(1.f - fDistance/fMaxViewableDistanceInDirection);
//	
//	// computing movement speed weight
//	float fSpeed = 0;
//	if (tpEntity->ps_Size() > 1) {
//		u32 dwTime = tpEntity->ps_Element(tpEntity->ps_Size() - 1).dwTime;
//		if (dwTime < m_dwMovementIdleTime) {
//			fSpeed = tpEntity->ps_Element(tpEntity->ps_Size() - 2).Position().distance_to(tpEntity->ps_Element(tpEntity->ps_Size() - 1).Position())/dwTime;
//			fResult += fSpeed < m_fMaxInvisibleSpeed ? m_fMovementSpeedWeight*fSpeed/m_fMaxInvisibleSpeed : m_fMovementSpeedWeight;
//		}
//	}
//	
//	// computing my ability to view the enemy
//	fResult += m_fCurSpeed < m_fMaxViewableSpeed ? m_fSpeedWeight*(1.f - m_fCurSpeed/m_fMaxViewableSpeed) : m_fSpeedWeight;
//	
//	// computing lightness weight
//	fResult += (1 - float(tpEntity->level_vertex()->light)/255.f)*m_fShadowWeight;
//	
//#ifdef LOG_PARAMETERS
//	if ((g_Alive() && !!dynamic_cast<CActor*>(tpEntity)) && (fResult >= m_fVisibilityThreshold))
//		HUD().outMessage(0xffffffff,cName(),"%s : %d",fResult >= m_fVisibilityThreshold ? "I see actor" : "I don't see actor",Level().timeServer());
//	Msg("**********");
//	Msg("Distance : %f [%f]",fDistance, fDistance >= fMaxViewableDistanceInDirection ? 0.f : m_fDistanceWeight*(1.f - fDistance/fMaxViewableDistanceInDirection));
//	Msg("MySpeed  : %f [%f]",m_fCurSpeed, m_fCurSpeed < m_fMaxViewableSpeed ? m_fSpeedWeight*(1.f - m_fCurSpeed/m_fMaxViewableSpeed) : m_fSpeedWeight);
//	Msg("Speed    : %f [%f]",fSpeed, fSpeed < m_fMaxInvisibleSpeed ? m_fMovementSpeedWeight*fSpeed/m_fMaxInvisibleSpeed : m_fMovementSpeedWeight);
//	Msg("Shadow   : %f [%f]",float(tpEntity->level_vertex()->light)/255.f,(1 - float(tpEntity->level_vertex()->light)/255.f)*m_fShadowWeight);
//	Msg("Result   : %f [%f]",fResult,m_fVisibilityThreshold);
////	if (iLogParameters) {
////		fprintf(ST_VF,"%6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f\n",fDistance,fAlpha,fSpeed,speed(),float(tpEntity->level_vertex()->light)/255.f,float(level_vertex()->light)/255.f,tpEntity->Radius(),float(iLogParameters - 1));
////	}
//#endif
//	return(fResult >= m_fVisibilityThreshold);
}

void CVisualMemoryManager::add_visible_object	(const CObject *object)
{
	const CGameObject *game_object	= dynamic_cast<const CGameObject*>(object);
	const CGameObject *self			= dynamic_cast<const CGameObject*>(this);
	if (!game_object || !visible(game_object))
		return;

	xr_vector<CVisibleObject>::iterator	J = std::find(m_objects->begin(),m_objects->end(),object_id(game_object));
	if (m_objects->end() == J) {
		CVisibleObject			visible_object;

		visible_object.fill		(game_object,self);

		if (m_max_object_count <= m_objects->size()) {
			xr_vector<CVisibleObject>::iterator	I = std::min_element(m_objects->begin(),m_objects->end(),SLevelTimePredicate<CGameObject>());
			VERIFY				(m_objects->end() != I);
			*I					= visible_object;
		}
		else
			m_objects->push_back	(visible_object);
	}
	else
		(*J).fill				(game_object,self);
}

void CVisualMemoryManager::add_visible_object	(const CVisibleObject visible_object)
{
	xr_vector<CVisibleObject>::iterator			J = std::find(m_objects->begin(),m_objects->end(),object_id(visible_object.m_object));
	if (m_objects->end() != J)
		*J				= visible_object;
	else
		if (m_max_object_count <= m_objects->size()) {
			xr_vector<CVisibleObject>::iterator	I = std::min_element(m_objects->begin(),m_objects->end(),SLevelTimePredicate<CGameObject>());
			VERIFY								(m_objects->end() != I);
			*I									= visible_object;
		}
		else
			m_objects->push_back(visible_object);
}
	
void CVisualMemoryManager::update				()
{
	m_visible_objects.clear				();
	feel_vision_get						(m_visible_objects);

	{
		xr_vector<CVisibleObject>::iterator	I = m_objects->begin();
		xr_vector<CVisibleObject>::iterator	E = m_objects->end();
		for ( ; I != E; ++I)
			(*I).m_visible				= false;
	}

	{
		xr_vector<CObject*>::const_iterator	I = m_visible_objects.begin();
		xr_vector<CObject*>::const_iterator	E = m_visible_objects.end();
		for ( ; I != E; ++I)
			add_visible_object			(*I);
	}
	
	// verifying if object is online
	xr_vector<CVisibleObject>::iterator		J = remove_if(m_objects->begin(),m_objects->end(),SRemoveOfflivePredicate());
	m_objects->erase						(J,m_objects->end());
}

bool CVisualMemoryManager::visible(u32 _level_vertex_id, float yaw, float eye_fov) const
{
	Fvector					direction;
	direction.sub			(ai().level_graph().vertex_position(_level_vertex_id),Position());
	direction.normalize_safe();
	float					y, p;
	direction.getHP			(y,p);
	if (angle_difference(yaw,y) <= eye_fov*PI/180.f/2.f)
		return(ai().level_graph().check_vertex_in_direction(level_vertex_id(),Position(),_level_vertex_id));
	else
		return(false);
}

bool CVisualMemoryManager::see		(const CEntityAlive *object0, const CEntityAlive *object1) const
{
	if (object0->Position().distance_to(object1->Position()) > object0->ffGetRange())
		return		(false);

	float			yaw1, pitch1, yaw2, pitch2, fYawFov, fPitchFov, fRange;
	Fvector			tPosition = object0->Position();

	yaw1			= object0->Orientation().yaw;
	pitch1			= object0->Orientation().pitch;
	fYawFov			= angle_normalize_signed(object0->ffGetFov()*PI/180.f);
	fRange			= object0->ffGetRange();

	fYawFov			= angle_normalize_signed((_abs(fYawFov) + _abs(atanf(1.f/tPosition.distance_to(object1->Position()))))/2.f);
	fPitchFov		= angle_normalize_signed(fYawFov*1.f);
	tPosition.sub	(object1->Position());
	tPosition.mul	(-1);
	tPosition.getHP	(yaw2,pitch2);
	yaw1			= angle_normalize_signed(yaw1);
	pitch1			= angle_normalize_signed(pitch1);
	yaw2			= angle_normalize_signed(yaw2);
	pitch2			= angle_normalize_signed(pitch2);
	if ((angle_difference(yaw1,yaw2) <= fYawFov) && (angle_difference(pitch1,pitch2) <= fPitchFov))
		return		(true);
	return			(false);
}
