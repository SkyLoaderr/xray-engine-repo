////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_classes.h
//	Created 	: 25.09.2003
//  Modified 	: 25.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "Entity.h"
#include "Inventory.h"

#define DECLARE_FUNCTION10(A,D)\
	IC		D				A					() const\
	{

#define DECLARE_FUNCTION11(A,D,F)\
	IC		D				A					(F f) const\
	{

#define CAST_OBJECT(A,B)\
		B				*l_tpEntity = dynamic_cast<B*>(m_tpGameObject);\
		if (!l_tpEntity) {\
			Log				("* [LUA] %s cannot access class member #A!",m_tpGameObject->cName());

#define CAST_OBJECT0(A,B)\
		CAST_OBJECT(A,B)\
			return;\
		}

#define CAST_OBJECT1(A,B,E)\
		CAST_OBJECT(A,B)\
			return		(E);\
		}

#define GET_MEMBER(C)\
		return			(l_tpEntity->C);\
	}

#define CALL_FUNCTION10(C)\
		return			(l_tpEntity->C());\
	}

#define CALL_FUNCTION11(C,F)\
		return			(l_tpEntity->C((F)(f)));\
	}

#define CALL_FUNCTION01(C,F)\
		l_tpEntity->C((F)(f));\
	}

#define BIND_MEMBER(A,B,C,D,E) \
	DECLARE_FUNCTION10	(A,D)\
	CAST_OBJECT1		(A,B,E)\
	GET_MEMBER			(C)

#define BIND_FUNCTION10(A,B,C,D,E) \
	DECLARE_FUNCTION10	(A,D)\
	CAST_OBJECT1		(A,B,E)\
	CALL_FUNCTION10		(C)

#define BIND_FUNCTION11(A,B,C,D,E,F) \
	DECLARE_FUNCTION11	(A,D,F)\
	CAST_OBJECT1		(A,B,E)\
	CALL_FUNCTION11		(C,F)

#define BIND_FUNCTION01(A,B,C,F) \
	DECLARE_FUNCTION11	(A,void,F)\
	CAST_OBJECT0		(A,B)\
	CALL_FUNCTION01		(C,F)

class CLuaGameObject {
	CGameObject			*m_tpGameObject;
public:

							CLuaGameObject		(CGameObject *tpGameObject)
	{
		m_tpGameObject	= tpGameObject;
		R_ASSERT2		(m_tpGameObject,"Null actual object passed!");
	}

							CLuaGameObject		(const CLuaGameObject *tpLuaGameObject)
	{
		m_tpGameObject	= tpLuaGameObject->m_tpGameObject;
		R_ASSERT2		(m_tpGameObject,"Null actual object passed!");
	}
						
							CLuaGameObject		(LPCSTR caObjectName)
	{
		m_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
		R_ASSERT2		(m_tpGameObject,"Null actual object passed!");
	}

	virtual					~CLuaGameObject()
	{
	}

	IC		CLuaGameObject	*Parent				() const
	{
		CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(m_tpGameObject->H_Parent());
		if (l_tpGameObject)
			return		(xr_new<CLuaGameObject>(l_tpGameObject));
		else
			return		(0);
	}

			void			Hit					(float power, Fvector &dir, CLuaGameObject *who, s16 element, Fvector p_in_object_space, float impulse, ALife::EHitType hit_type);


							operator CObject*() const
	{
		return			((CObject*)m_tpGameObject);
	}

	BIND_MEMBER		(ClassID,	CGameObject,	CLS_ID,			CLASS_ID,			CLASS_ID(-1));
	BIND_FUNCTION10	(Position,	CGameObject,	Position,		Fvector,			Fvector());
	BIND_FUNCTION10	(Name,		CGameObject,	cName,			LPCSTR,				"");
	BIND_FUNCTION10	(Section,	CGameObject,	cNameSect,		LPCSTR,				"");
	BIND_FUNCTION10	(Mass,		CGameObject,	GetMass,		float,				float(-1));
	BIND_FUNCTION10	(ID,		CGameObject,	ID,				u32,				u32(-1));
	BIND_FUNCTION10	(getVisible,CGameObject,	getVisible,		BOOL,				FALSE);
	BIND_FUNCTION01	(setVisible,CGameObject,	setVisible,		BOOL);
	BIND_FUNCTION10	(getEnabled,CGameObject,	getEnabled,		BOOL,				FALSE);
	BIND_FUNCTION01	(setEnabled,CGameObject,	setEnabled,		BOOL);

	BIND_MEMBER		(DeathTime,	CEntity,		m_dwDeathTime,	_TIME_ID,			_TIME_ID(-1));
	BIND_FUNCTION10	(Armor,		CEntity,		g_Armor,		float,				-1);
	BIND_FUNCTION10	(Health,	CEntity,		g_Health,		float,				-1);
	BIND_FUNCTION10	(MaxHealth,	CEntity,		g_MaxHealth,	float,				-1);
	BIND_FUNCTION10	(Accuracy,	CEntity,		g_Accuracy,		float,				-1);
	BIND_FUNCTION10	(Alive,		CEntity,		g_Alive,		BOOL,				FALSE);
	BIND_FUNCTION10	(Team,		CEntity,		g_Team,			int,				-1);
	BIND_FUNCTION10	(Squad,		CEntity,		g_Squad,		int,				-1);
	BIND_FUNCTION10	(Group,		CEntity,		g_Group,		int,				-1);
//	BIND_FUNCTION01	(Kill,		CEntity,		KillEntity,		CLuaGameObject*);
	
	IC		void			Kill				(CLuaGameObject* who)
	{
		CEntity				*l_tpEntity = dynamic_cast<CEntity*>(m_tpGameObject);
		if (!l_tpEntity) {
			Log				("* [LUA] %s cannot access class member!",m_tpGameObject->cName());
			return;
		}
		l_tpEntity->KillEntity					((CObject*)who);
	}
	
	BIND_FUNCTION10	(Cost,		CInventoryItem,	Cost,			u32,				u32(-1));
	
	
//			.def("Enabled",						&CLuaGameObject::Enabled)
//			.def("Visible",						&CLuaGameObject::Visible),
//
//		class_<CLuaGameItem,"CGameObject">("CGameItem")
//			.def("HealthValue",					&CLuaGameItem::HealthValue)
//			.def("FoodValue",					&CLuaGameItem::FoodValue)
//class CItemObject {
//public:
//	Active();
//	Visible(); // only for NPC
//	Condition();
//	get_cost();
//};
//
//class CAliveObject {
//public:
//	rank();
//	health();
//	activeweapon();
//	equipment();
//	asleep();
//	zombied();
//	checkifobjectvisible();
//
//};
//
};