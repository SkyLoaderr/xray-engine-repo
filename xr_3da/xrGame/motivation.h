////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation.h
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

class CScriptGameObject;

template <typename _object_type>
class CMotivation {
public:
	_object_type	*m_object;

public:
	IC				CMotivation		();
	virtual			~CMotivation	();
	IC		void	init			();
	virtual void	reinit			(_object_type *object);
	virtual void	Load			(LPCSTR section);
	virtual void	reload			(LPCSTR section);
	virtual float	evaluate		(u32 sub_motivation_id);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
typedef CMotivation<CScriptGameObject> CScriptMotivation;
add_to_type_list(CScriptMotivation)
#undef script_type_list
#define script_type_list save_type_list(CScriptMotivation)

#include "motivation_inline.h"