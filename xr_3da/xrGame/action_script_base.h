////////////////////////////////////////////////////////////////////////////
//	Module 		: action_script_base.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Base action with script support
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"
//#include "script_game_object.h"

class CScriptGameObject;

template <typename _object_type>
class CActionScriptBase : public CScriptActionBase {
protected:
	typedef CScriptActionBase inherited;

public:
	_object_type			*m_object;

public:
	IC						CActionScriptBase	(const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects, _object_type *object = 0, LPCSTR action_name = "");
	IC						CActionScriptBase	(_object_type *object = 0, LPCSTR action_name = "");
	virtual					~CActionScriptBase	();
	virtual	void			reinit				(_object_type *object, CPropertyStorage *storage, bool clear_all);
	virtual	void			reinit				(CScriptGameObject *object, CPropertyStorage *storage, bool clear_all);
};

#include "action_script_base_inline.h"