////////////////////////////////////////////////////////////////////////////
//	Module 		: danger_object.h
//	Created 	: 14.02.2005
//  Modified 	: 14.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Danger object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "entity_alive.h"

class CDangerObject {
public:
	enum EDangerType {
		eDangerTypeRicochet = u32(0),
		eDangerTypeShot,
		eDangerTypeAttack,
		eDangerTypeHit,
		eDangerTypeDeath,
		eDangerTypeCorpse,
		eDangerTypeDummy = u32(-1),
	};

	enum EDangerPerceiveType {
		eDangerPerceiveTypeVisual = u32(0),
		eDangerPerceiveTypeSound,
		eDangerPerceiveTypeHit,
		eDangerPerceiveTypeDummy = u32(-1),
	};

private:
	const CEntityAlive					*m_object;
	Fvector								m_position;
	u32									m_time;
	EDangerType							m_type;
	EDangerPerceiveType					m_perceive_type;

public:
	IC									CDangerObject	(const CEntityAlive	*object, const Fvector &position, u32 time, const EDangerType &type, const EDangerPerceiveType	&perceive_type);
	virtual								~CDangerObject	();
	IC		const CEntityAlive			*object			() const;
	IC		const Fvector				&position		() const;
	IC		u32							time			() const;
	IC		const EDangerType			&type			() const;
	IC		const EDangerPerceiveType	&perceive_type	() const;
	IC		bool						operator==		(const CDangerObject &object) const;
};

#include "danger_object_inline.h"