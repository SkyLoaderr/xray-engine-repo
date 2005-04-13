#pragma once 
#include "../../shared_data.h"
#include "ai_monster_defs.h"

struct SMonsterSettings {
	
	// float speed factors

	float					m_fDistToCorpse;
	float					m_fDamagedThreshold;		// ����� ��������, ���� �������� ��������������� ���� m_bDamaged

	// -------------------------------------------------------

	u32						m_dwIdleSndDelay;
	u32						m_dwEatSndDelay;
	u32						m_dwAttackSndDelay;

	// -------------------------------------------------------

	u32						m_dwDayTimeBegin;
	u32						m_dwDayTimeEnd;
	float					m_fMinSatiety;
	float					m_fMaxSatiety;
	
	// ----------------------------------------------------------- 

	float					m_fSoundThreshold;

	float					m_fEatFreq;
	float					m_fEatSlice;
	float					m_fEatSliceWeight;

	u8						m_legs_number;
	SAttackEffector			m_attack_effector;

	float					m_max_hear_dist;

	float					m_run_attack_path_dist;
	float					m_run_attack_start_dist;
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
};


