////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_manager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sight manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CSightManager::GetDirectionAnglesByPrevPositions(float &yaw, float &pitch)
{
	CObject					*object = dynamic_cast<CObject*>(this);
	VERIFY					(object);
	Fvector					tDirection;
	int						i = object->ps_Size	();

	if (i < 2) 
		return;

	CObject::SavedPosition	tPreviousPosition = object->ps_Element(i - 2), tCurrentPosition = object->ps_Element(i - 1);
	VERIFY					(_valid(tPreviousPosition.vPosition));
	VERIFY					(_valid(tCurrentPosition.vPosition));
	tDirection.sub			(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
	if (tDirection.magnitude() < EPS_L)	return;
	tDirection.getHP		(yaw,pitch);
	VERIFY					(_valid(yaw));
	VERIFY					(_valid(pitch));
}

IC	void CSightManager::GetDirectionAngles				(float &yaw, float &pitch)
{
	GetDirectionAnglesByPrevPositions(yaw,pitch);
};

IC	bool CSightManager::use_torso_look	() const
{
	if (!m_actions.empty())
		return				(current_action().use_torso_look());
	else
		return				(true);
}