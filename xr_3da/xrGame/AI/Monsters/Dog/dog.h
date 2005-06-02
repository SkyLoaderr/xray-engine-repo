#pragma once

#include "../BaseMonster/base_monster.h"
#include "../ai_monster_bones.h"
#include "../ai_monster_jump.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"

class CAI_Dog : public CBaseMonster, 
				public CJumping,
				public CControlledEntity<CAI_Dog> {
	
	typedef		CBaseMonster				inherited;
	typedef		CControlledEntity<CAI_Dog>	CControlled;

	bool					strike_in_jump;
	bonesManipulation		Bones;

public:
					CAI_Dog				();
	virtual			~CAI_Dog			();	

	virtual void	Load				(LPCSTR section);
	virtual void	UpdateCL			();

	virtual void	reinit				();
	virtual void	CheckSpecParams		(u32 spec_params);
	virtual	void	OnSoundPlay			();

	virtual void	OnJumpStop			();
	virtual bool	CanJump				() {return true;}

	virtual bool	ability_can_drag		() {return true;}
	virtual bool	ability_can_jump		() {return true;}
	virtual bool	ability_rotation_jump	() {return true;}

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CAI_Dog)
#undef script_type_list
#define script_type_list save_type_list(CAI_Dog)
