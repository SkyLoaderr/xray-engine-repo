#pragma once

//////////////////////////////////////////////////////////////////////////
// StepSounds
struct SStepSound {
	float	vol;
	float	freq;
};
DEFINE_MAP(EMotionAnim, SStepSound, STEP_SOUND_MAP, STEP_SOUND_MAP_IT);


class _biting_shared : public CSharedResource {
public:
	// float speed factors
	float					m_fsTurnNormalAngular;
	float					m_fsWalkFwdNormal;
	float					m_fsWalkFwdDamaged;
	float					m_fsWalkBkwdNormal;
	float 					m_fsWalkAngular;
	float 					m_fsRunFwdNormal;
	float 					m_fsRunFwdDamaged;
	float 					m_fsRunAngular;
	float					m_fsDrag;
	float					m_fsSteal;

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

};
