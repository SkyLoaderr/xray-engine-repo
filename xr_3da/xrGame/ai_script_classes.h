////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_classes.h
//	Created 	: 25.09.2003
//  Modified 	: 25.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"

class CLuaGameObject {
	CGameObject			*m_tpGameObject;
public:

						CLuaGameObject(CGameObject *tpGameObject)
	{
		m_tpGameObject	= tpGameObject;
		R_ASSERT2		(m_tpGameObject,"Null actual object passed!");
	}

						CLuaGameObject(const CLuaGameObject *tpLuaGameObject)
	{
		m_tpGameObject	= tpLuaGameObject->m_tpGameObject;
		R_ASSERT2		(m_tpGameObject,"Null actual object passed!");
	}
						
						CLuaGameObject(LPCSTR caObjectName)
	{
		m_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
		R_ASSERT2		(m_tpGameObject,"Null actual object passed!");
	}

	Fvector				&Position() const
	{
		return			(m_tpGameObject->Position());
	}
	
	LPCSTR				cName() const
	{
		return			(m_tpGameObject->cName());
	}
};