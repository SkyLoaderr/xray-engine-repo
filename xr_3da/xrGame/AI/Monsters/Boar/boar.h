#pragma once

#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"

class CAI_Boar : public CBaseMonster,
				 public CControlledEntity<CAI_Boar> {

	typedef		CBaseMonster	inherited;
	typedef		CControlledEntity<CAI_Boar>	controlled;

public:
					CAI_Boar			();
	virtual			~CAI_Boar			();	

	virtual void	Load				(LPCSTR section);
	virtual BOOL	net_Spawn			(LPVOID DC);
	virtual void	UpdateCL			();

	virtual	void	LookPosition		(Fvector to_point, float angular_speed = PI_DIV_3);

	virtual bool	CanExecRotationJump	() {return true;}
	virtual void	CheckSpecParams		(u32 spec_params);

	// look at enemy
	static void __stdcall BoneCallback	(CBoneInstance *B);
	
			float	_velocity;
			float	_cur_delta, _target_delta;
			bool	look_at_enemy;
	
	virtual bool	ability_can_drag	() {return true;}

};
