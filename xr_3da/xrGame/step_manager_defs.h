#pragma once
#define MIN_LEGS_COUNT	1
#define MAX_LEGS_COUNT	4 

struct SStepParam {
	struct{
		float	time;
		float	power;
	} step[MAX_LEGS_COUNT];

	u8			cycles;
};

DEFINE_MAP(shared_str,	SStepParam, STEPS_MAP, STEPS_MAP_IT);

struct SStepInfo {
	struct {
		ref_sound		sound;

		bool			handled;		// ���������
		u8				cycle;			// ���� � ������� ���������
	} activity[MAX_LEGS_COUNT];

	SStepParam		params;
	bool			disable;

	u8				cur_cycle;

	SStepInfo()		{disable = true;}
};

enum ELegType {
	eFrontLeft,
	eFrontRight,
	eBackRight,
	eBackLeft
};

