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
	// yaw					= pitch = 0;
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

IC	MonsterSpace::ELookType	CSightManager::look_type	() const
{
	return					(m_tLookType);
}
