#pragma once
#include "../BaseMonster/base_monster.h"
#include "../../../script_export_space.h"

class CTushkano : public CBaseMonster {
	typedef		CBaseMonster		inherited;

public:
					CTushkano 			();
	virtual			~CTushkano 			();	

	virtual void	Load				(LPCSTR section);
	virtual void	CheckSpecParams		(u32 spec_params);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CTushkano)
#undef script_type_list
#define script_type_list save_type_list(CTushkano)
