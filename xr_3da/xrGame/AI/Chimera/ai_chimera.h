#pragma once

#include "../biting/ai_biting.h"
#include "../ai_monster_bones.h"

class CAI_Chimera : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Chimera			();
	virtual					~CAI_Chimera		();	

	virtual void			Init				();
	virtual	void			Load				(LPCSTR section);

	virtual void			StateSelector		();

	virtual BOOL			net_Spawn			(LPVOID DC);

	// Bone manipulation
			void			vfAssignBones		(CInifile *ini, const char *section);
	static	void __stdcall	SpinCallback		(CBoneInstance *B);
			void			SpinBoneInMotion	(CBoneInstance *B);
			void			SpinBoneInAttack	(CBoneInstance *B);

	virtual	bool			CanJump				();
	virtual void			Jump				(Fvector to_pos);
			
	// ���������� �������� ��������	
	float	fSpinYaw;				// ���� �������� ��� ����
	TTime	timeLastSpin;			// ��������� ����� ��������� SpinYaw
	float	fStartYaw, fFinishYaw;	// ��������� � �������� ���� �������� �������
	float	fPrevMty;				// ���������� �������� target.yaw

	bonesManipulation		Bones;

};
