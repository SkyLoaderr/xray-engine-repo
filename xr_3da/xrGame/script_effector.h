////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../effectorpp.h"
#include "script_export_space.h"
#include "../cameramanager.h"

class CScriptEffector : public CEffectorPP {
public:
	typedef CEffectorPP inherited;
	EEffectorPPType		m_tEffectorType;
	SPPInfo				m_tInfo;

	IC					CScriptEffector		(int iType, float time);
	virtual				~CScriptEffector	();
	virtual	BOOL		Process				(SPPInfo &pp);
	virtual	void		Add					();
	virtual	void		Remove				();
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptEffector)
#undef script_type_list
#define script_type_list save_type_list(CScriptEffector)

#include "script_effector_inline.h"