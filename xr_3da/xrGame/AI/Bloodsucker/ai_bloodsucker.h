#pragma once

#include "..\\biting\\ai_biting.h"
#include "..\\ai_monster_bones.h"
#include "..\\ai_monster_invisibility.h"
#include "ai_bloodsucker_state.h"


class CAI_Bloodsucker : public CAI_Biting, 
						public CMonsterInvisibility {

	typedef		CAI_Biting	inherited;
public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

	virtual void			Init					();

	virtual void			UpdateCL				();
	virtual void			StateSelector			();


	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual	void			Load					(LPCSTR section);
			void			vfAssignBones			();

	static	void __stdcall	BoneCallback			(CBoneInstance *B);

			CBoneInstance*	GetBone					(LPCTSTR bone_name);
			CBoneInstance*	GetBone					(int bone_id);

			void			LookDirection			(Fvector to_dir, float bone_turn_speed);
	virtual	void			LookPosition			(Fvector to_point);

			void			ActivateEffector		(float life_time);
	// Flesh-specific FSM
	CBloodsuckerHearDNE		*stateHearDNE;
	CBloodsuckerHearNDE		*stateHearNDE;

	friend	class			CBloodsuckerHearDNE;
	friend	class			CBloodsuckerHearNDE;

	
	bonesManipulation		Bones;

	float					m_fInvisibilityDist;			// ��������� �� �����, �� ������� ������ ����� ����������� ���������
	float					m_ftrPowerDown;					// �����������, �� ������� ������ ���� ������� ��� ��������� ����������� 
	float					m_fPowerThreshold;				// ����� ����, ����� �������� ������ ����� ���������

};


