#pragma once
#include "../../biting/ai_biting.h"
#include "../../ai_monster_bones.h"

class CStateManagerZombie;

class CZombie :	public CAI_Biting {
	
	typedef		CAI_Biting		inherited;

	CStateManagerZombie *StateMan;

	bonesManipulation	Bones;

public:
					CZombie		();
	virtual			~CZombie		();	

	virtual void	Load				(LPCSTR section);
	virtual BOOL	net_Spawn			(LPVOID DC);
	virtual void	reinit				();
	
	virtual bool	UpdateStateManager	();
	virtual	void	PitchCorrection		() {}

	static	void __stdcall	BoneCallback			(CBoneInstance *B);
			void			vfAssignBones			();

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

};
