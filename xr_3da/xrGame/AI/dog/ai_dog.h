#pragma once

#include "..\\biting\\ai_biting.h"
#include "..\\ai_monster_bones.h"

class CAI_Dog : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
					CAI_Dog				();
	virtual			~CAI_Dog			();	

	virtual	BOOL	net_Spawn			(LPVOID DC);
	
	virtual void	Init				();
	virtual void	StateSelector		();
	virtual void	CheckSpecParams		(u32 spec_params);
	virtual	void	OnSoundPlay			();

	static	void __stdcall	BoneCallback			(CBoneInstance *B);

	bonesManipulation		Bones;

	virtual	bool			CanJump				() {return true;}
	
};
