#pragma once
#include "../../biting/ai_biting.h"
#include "../../ai_monster_bones.h"
#include "../../anim_triple.h"

class CStateManagerZombie;

class CZombie :	public CAI_Biting {
	
	typedef		CAI_Biting		inherited;

	CStateManagerZombie *StateMan;

	bonesManipulation	Bones;

public:
					CZombie		();
	virtual			~CZombie	();	

	virtual void	Load				(LPCSTR section);
	virtual BOOL	net_Spawn			(LPVOID DC);
	virtual void	reinit				();
	virtual	void	reload				(LPCSTR section);

	virtual void	Hit					(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type);

	virtual bool	UpdateStateManager	();
	virtual	void	PitchCorrection		() {}

	virtual void	shedule_Update		(u32 dt);
	
	static	void __stdcall	BoneCallback			(CBoneInstance *B);
			void			vfAssignBones			();

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

	CAnimTriple		anim_triple_death;
	u32				time_dead_start;
	u32				last_hit_frame;
	u32				time_resurrect;

	u8				fake_death_count;
	float			health_death_threshold;
	float			last_health_fake_death;
};
