#pragma once 
#include "ai_monster_share.h"
#include "ai_monster_defs.h"

class _biting_shared : public CSharedResource {
public:
	// float speed factors
	SVelocityParam			m_fsVelocityNone;
	SVelocityParam			m_fsVelocityStandTurn;
	SVelocityParam			m_fsVelocityWalkFwdNormal;
	SVelocityParam			m_fsVelocityWalkFwdDamaged;
	SVelocityParam			m_fsVelocityRunFwdNormal;
	SVelocityParam			m_fsVelocityRunFwdDamaged;
	SVelocityParam 			m_fsVelocityDrag;
	SVelocityParam 			m_fsVelocitySteal;

	u32						m_dwProbRestWalkFree;
	u32						m_dwProbRestStandIdle;
	u32						m_dwProbRestLieIdle;
	u32						m_dwProbRestTurnLeft;

	float					m_fImpulseMin;
	float					m_fImpulseMax;

	float					m_fDistToCorpse;
	float					m_fMinAttackDist;
	float					m_fMaxAttackDist;

	float					m_fDamagedThreshold;		// ����� ��������, ���� �������� ��������������� ���� m_bDamaged

	// -------------------------------------------------------

	TTime					m_dwIdleSndDelay;
	TTime					m_dwEatSndDelay;
	TTime					m_dwAttackSndDelay;

	// -------------------------------------------------------

	// ������ 
	float					m_fMoraleSuccessAttackQuant;		// ���������� ������ ��� �������� �����
	float					m_fMoraleDeathQuant;				// ���������� ������ ��� ������ ������� �� ����� �������
	float					m_fMoraleFearQuant;					// ���������� ������ � ������
	float					m_fMoraleRestoreQuant;				// ����� �������������� ������ ? 
	float					m_fMoraleBroadcastDistance;			// ������ �����������, ���� � ������ ������� ���� ������ �� �������

	// ----------------------------------------------------------- 

	u32						m_dwDayTimeBegin;
	u32						m_dwDayTimeEnd;
	float					m_fMinSatiety;
	float					m_fMaxSatiety;
	// ----------------------------------------------------------- 

	float					m_fSoundThreshold;
	float					m_fHitPower;

	float					m_fEatFreq;
	float					m_fEatSlice;
	float					m_fEatSliceWeight;

	STEP_SOUND_MAP			step_sounds;

	u8						m_bUsedSquadAttackAlg;

	SAttackEffector			m_attack_effector;


};

class _motion_shared : public CSharedResource {
public:
	ANIM_ITEM_MAP			m_tAnims;			// ����� ��������
	MOTION_ITEM_MAP			m_tMotions;			// ����� ����������� EAction � SMotionItem
	TRANSITION_ANIM_VECTOR	m_tTransitions;		// ������ ��������� �� ����� �������� � ������
	ATTACK_ANIM				aa_all;				// ������ ����

	t_fx_index				default_fx_indexes;
	FX_MAP_STRING			fx_map_string;
	FX_MAP_U16				fx_map_u16;
	bool					map_converted;
	
	AA_MAP					aa_map;
	STEPS_MAP				steps_map;
};


