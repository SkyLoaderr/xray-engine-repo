////////////////////////////////////////////////////////////////////////////
//	Module 		: script_zone.cpp
//	Created 	: 10.10.2003
//  Modified 	: 10.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Script zone object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_zone.h"
#include "ai_script_classes.h"

CScriptZone::CScriptZone		()
{
	m_tpOnEnter = m_tpOnExit = 0;
}

CScriptZone::~CScriptZone		()
{
#pragma todo("Dima to Dima : Memory leak is here, change object destruction sequence")
//	xr_delete					(m_tpOnEnter);
//	xr_delete					(m_tpOnExit);
}

void CScriptZone::spatial_register()
{
	R_ASSERT2					(CFORM(),"Invalid or no CForm!");
	spatial.center.set			(CFORM()->getSphere().P);
	spatial.radius				= CFORM()->getRadius();
	ISpatial::spatial_register	();
}

void CScriptZone::spatial_move()
{
	R_ASSERT2					(CFORM(),"Invalid or no CForm!");
	spatial.center.set			(CFORM()->getSphere().P);
	spatial.radius				= CFORM()->getRadius();
	ISpatial::spatial_move		();
}

BOOL CScriptZone::net_Spawn	(LPVOID DC) 
{
	CCF_Shape					*l_pShape = xr_new<CCF_Shape>(this);
	collidable.model			= l_pShape;

	CSE_Abstract				*l_tpAbstract = (CSE_Abstract*)(DC);
	CSE_ALifeScriptZone			*l_tpALifeScriptZone = dynamic_cast<CSE_ALifeScriptZone*>(l_tpAbstract);
	R_ASSERT					(l_tpALifeScriptZone);

	feel_touch.clear			();

	for (u32 i=0; i < l_tpALifeScriptZone->shapes.size(); i++) {
		CSE_Shape::shape_def	&S = l_tpALifeScriptZone->shapes[i];
		switch (S.type) {
			case 0 : {
				l_pShape->add_sphere(S.data.sphere);
				break;
					 }
			case 1 : {
				l_pShape->add_box(S.data.box);
				break;
					 }
		}
	}

	BOOL						bOk = inherited::net_Spawn(DC);
	if (bOk) {
		l_pShape->ComputeBounds	();
		Fvector					P;
		XFORM().transform_tiny	(P,CFORM()->getSphere().P);
		setEnabled				(true);
	}

	return						(bOk);
}

void CScriptZone::UpdateCL	()
{
	inherited::UpdateCL			();
	const Fsphere				&s = CFORM()->getSphere();
	Fvector						P;
	XFORM().transform_tiny		(P,s.P);
	feel_touch_update			(P,s.R);
}

void CScriptZone::feel_touch_new	(CObject *tpObject)
{
	CGameObject					*l_tpGameObject = dynamic_cast<CGameObject*>(tpObject);
	if (!l_tpGameObject)
		return;
	if (m_tpOnEnter)
		(*m_tpOnEnter)(xr_new<CLuaGameObject>(this),xr_new<CLuaGameObject>(l_tpGameObject));
}

void CScriptZone::feel_touch_delete	(CObject *tpObject)
{
	CGameObject					*l_tpGameObject = dynamic_cast<CGameObject*>(tpObject);
	if (!l_tpGameObject)
		return;

	if (m_tpOnExit)
		(*m_tpOnExit)(xr_new<CLuaGameObject>(this),xr_new<CLuaGameObject>(l_tpGameObject));
}

void CScriptZone::set_callback(luabind::functor<void> tpZoneCallback, bool bOnEnter)
{
	if (bOnEnter)
		if (tpZoneCallback.is_valid())
			m_tpOnEnter				= xr_new<luabind::functor<void> >(tpZoneCallback);
		else
			xr_delete				(m_tpOnEnter);
	else
		if (tpZoneCallback.is_valid())
			m_tpOnExit				= xr_new<luabind::functor<void> >(tpZoneCallback);
		else
			xr_delete				(m_tpOnExit);
}

void CScriptZone::clear_callback(bool bOnEnter)
{
	if (bOnEnter)
		m_tpOnEnter				= 0;
	else
		m_tpOnExit				= 0;
}