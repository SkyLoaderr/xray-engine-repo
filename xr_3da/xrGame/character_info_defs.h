//////////////////////////////////////////////////////////////////////////
// character_info_defs.h		������� ���������� ��� ���������� � ����
// 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

typedef int					PROFILE_INDEX;
typedef shared_str			PROFILE_ID;
#define NO_PROFILE			PROFILE_INDEX(-1)
#define DEFAULT_PROFILE		PROFILE_INDEX(0)


typedef int						SPECIFIC_CHARACTER_INDEX;
typedef shared_str				SPECIFIC_CHARACTER_ID;
#define NO_SPECIFIC_CHARACTER	SPECIFIC_CHARACTER_INDEX(-1)

//������ ��������� (���������������) ������ ��������� � ������� - 
//�������� �� -100< (������ ����������) �� >100 (����� �����������)
typedef int					CHARACTER_GOODWILL;
#define NO_GOODWILL			-type_max(CHARACTER_GOODWILL)
#define NEUTRAL_GOODWILL	CHARACTER_GOODWILL(0)

typedef shared_str				CHARACTER_CLASS;
#define NO_CHARACTER_CLASS		NULL

//��������� ��������� - �������� �� -100 (����� ������, �������������) 
//�� 100 (����� �������, �����������)
typedef int					CHARACTER_REPUTATION_VALUE;
#define NO_REPUTATION		-type_max(CHARACTER_REPUTATION_VALUE)
#define NEUTAL_REPUTATION	0

//���� ��������� - �������� �� 0 (������ ���������) 
//�� >100 (����� �������)
typedef int			CHARACTER_RANK_VALUE;
#define NO_RANK		-type_max(CHARACTER_RANK_VALUE)


typedef shared_str				CHARACTER_COMMUNITY_ID;
#define NO_COMMUNITY_ID			CHARACTER_COMMUNITY_ID(NULL)

typedef int						CHARACTER_COMMUNITY_INDEX;
#define NO_COMMUNITY_INDEX		CHARACTER_COMMUNITY_INDEX(-1)