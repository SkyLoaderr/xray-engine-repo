#pragma once
#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "../ai_monster_bones.h"
#include "../anim_triple.h"
#include "../../../script_export_space.h"

#define FAKE_DEATH_TYPES_COUNT	3

class CZombie :	public CBaseMonster,
				public CControlledEntity<CZombie> {
	
	typedef		CBaseMonster				inherited;
	typedef		CControlledEntity<CZombie>	CControlled;

	bonesManipulation	Bones;

public:
					CZombie		();
	virtual			~CZombie	();	

	virtual void	Load				(LPCSTR section);
	virtual BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	reinit				();
	virtual	void	reload				(LPCSTR section);
	
	virtual void	Hit					(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type);

	virtual	void	PitchCorrection		() {}

	virtual void	shedule_Update		(u32 dt);
	
	static	void 	BoneCallback		(CBoneInstance *B);
			void	vfAssignBones		();

	virtual bool	use_center_to_aim				() const {return true;}

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

	SAnimationTripleData	anim_triple_death[FAKE_DEATH_TYPES_COUNT];
	u8				active_triple_idx;
	
	u32				time_dead_start;
	u32				last_hit_frame;
	u32				time_resurrect;

	u8				fake_death_count;
	float			health_death_threshold;
	float			last_health_fake_death;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CZombie)
#undef script_type_list
#define script_type_list save_type_list(CZombie)
