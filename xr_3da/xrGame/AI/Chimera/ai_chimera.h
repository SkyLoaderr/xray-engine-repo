#pragma once

#include "..\\biting\\ai_biting.h"
#include "..\\ai_monster_bones.h"

class CAI_Chimera : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Chimera			();
	virtual					~CAI_Chimera		();	

	virtual void			Init				();
	virtual void			StateSelector		();

	virtual BOOL			net_Spawn			(LPVOID DC);

	// Bone manipulation
			void			vfAssignBones		(CInifile *ini, const char *section);
	static	void __stdcall	SpinCallback		(CBoneInstance *B);
			void			SpinBoneInMotion	(CBoneInstance *B);
			void			SpinBoneInAttack	(CBoneInstance *B);

	virtual	bool			CanJump				() {return true;}
			
	// реализация плавного поворота	
	float	fSpinYaw;				// угол поворота для боны
	TTime	timeLastSpin;			// последнее время изменения SpinYaw
	float	fStartYaw, fFinishYaw;	// начальный и конечный углы поворота монстра
	float	fPrevMty;				// предыдущее значение target.yaw

	bonesManipulation		Bones;

};
