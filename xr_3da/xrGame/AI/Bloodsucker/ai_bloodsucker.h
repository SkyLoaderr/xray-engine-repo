#pragma once

#include "../biting/ai_biting.h"
#include "../ai_monster_bones.h"
#include "../ai_monster_invisibility.h"

class CAI_Bloodsucker : public CAI_Biting, 
						public CMonsterInvisibility {

	typedef		CAI_Biting	inherited;

	
	static	void __stdcall	BoneCallback			(CBoneInstance *B);
			void			vfAssignBones			();
			void			LookDirection			(Fvector to_dir, float bone_turn_speed);


	bonesManipulation		Bones;

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

	virtual void			Init					();
	virtual void			UpdateCL				();
	virtual void			StateSelector			();
	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual	void			Load					(LPCSTR section);
	virtual	void			LookPosition			(Fvector to_point);

			void			ActivateEffector		(float life_time);
	
	virtual	void			CheckSpecParams			(u32 spec_params);

	float					m_fInvisibilityDist;			// ��������� �� �����, �� ������� ������ ����� ����������� ���������
	float					m_ftrPowerDown;					// �����������, �� ������� ������ ���� ������� ��� ��������� ����������� 
	float					m_fPowerThreshold;				// ����� ����, ����� �������� ������ ����� ���������
	float					m_fEffectDist;

	// ---------------------------------------------------------------------------------

	
	
	GTask					*task;
	TTime					last_time_finished;
		
	bool					IsActiveTaskFinished		();
	void					StopTask					();
	bool					CheckValidity				();
	bool					CheckCanSetWithTime			();
	bool					CheckCanSetWithConditions	();	
	bool					CanExecuteSquadTask			();
	bool					SquadTaskIsHigherPriority	(); 
	void					UpdateTaskStatus			();
	bool					IsTaskActive				();
	
	// ---------------------------------------------------------------------------------

};