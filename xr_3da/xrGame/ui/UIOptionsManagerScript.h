
#pragma once
#include "../script_export_space.h"

class CUIOptionsManagerScript {
public:
	void SetDefaultValues(const char* group);
	void SaveValues(const char* group);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIOptionsManagerScript)
#undef script_type_list
#define script_type_list save_type_list(CUIOptionsManagerScript)