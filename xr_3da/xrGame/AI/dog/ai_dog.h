#pragma once

#include "..\\biting\\ai_biting.h"
#include "..\\ai_monster_bones.h"
#include "..\\ai_monster_jump.h"
#include "..\\ai_monster_share.h"

class _dog_shared : public CSharedResource {
public:
	float real_speed;
};

class CAI_Dog : public CAI_Biting, public CJumping, public CSharedClass<_dog_shared> {
	typedef		CAI_Biting	inherited;
	typedef		CSharedClass<_dog_shared> _sd_dog;

	bool					strike_in_jump;
	bonesManipulation		Bones;

public:
					CAI_Dog				();
	virtual			~CAI_Dog			();	

	virtual	BOOL	net_Spawn			(LPVOID DC);
	virtual void	Load				(LPCSTR section);
	virtual void	UpdateCL			();

	virtual void	Init				();
	virtual void	StateSelector		();
	virtual void	CheckSpecParams		(u32 spec_params);
	virtual	void	OnSoundPlay			();

	virtual void	LookPosition		(Fvector to_point);		

	virtual void	OnJumpStop			();
	virtual bool	CanJump				() {return true;}

			void	LoadShared			(LPCSTR section);

private:
	static	void __stdcall	BoneCallback			(CBoneInstance *B);
	
};
