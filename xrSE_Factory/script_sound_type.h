////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_type.h
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound type
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

struct CScriptSoundType {
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptSoundType)
#undef script_type_list
#define script_type_list save_type_list(CScriptSoundType)
