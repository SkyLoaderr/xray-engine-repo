////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_wrapper.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script motivation wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation.h"
#include "script_space.h"

class CScriptMotivationWrapper : public CScriptMotivation, public luabind::wrap_base {
public:
	typedef	CScriptMotivation	inherited;

protected:
	typedef CScriptGameObject		_object_type;

public:
	virtual					~CScriptMotivationWrapper	();
	virtual void			reinit						(_object_type *object);
	static	void			reinit_static				(inherited *motivation, _object_type *object);
	virtual void			Load						(LPCSTR section);
	static	void			Load_static					(inherited *motivation, LPCSTR section);
	virtual void			reload						(LPCSTR section);
	static	void			reload_static				(inherited *motivation, LPCSTR section);
	virtual float			evaluate					(u32 sub_motivation_id);
	static	float			evaluate_static				(inherited *motivation, u32 sub_motivation_id);
};

#include "script_motivation_wrapper_inline.h"