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


typedef int					SPECIFIC_CHARACTER_INDEX;
typedef shared_str			SPECIFIC_CHARACTER_ID;
#define NO_SPECIFIC_CHARACTER	SPECIFIC_CHARACTER_INDEX(-1)

//��������� ��������� - �������� �� -100 (����� ������, �������������) 
//�� 100 (����� �������, �����������)
typedef int				CHARACTER_REPUTATION;
#define NO_REPUTATION	CHARACTER_REPUTATION(-1)

//���� ��������� - �������� �� 0 (������ ���������) 
//�� 100 (����� �������)
typedef int			CHARACTER_RANK;
#define NO_RANK		CHARACTER_RANK(-1)

//������ ��������� (���������������) ������ ��������� � ������� - 
//�������� �� 0 (������ ����������) �� 100 (����� �����������)
typedef int			CHARACTER_GOODWILL;
#define NO_GOODWILL	CHARACTER_GOODWILL(-1)

//����� ���������  ������ ��������� � �������, ����������� �� �������
//� ������ ���� �������� - �������� �� 
//0 (������ ����������) �� 100 (����� �����������)
typedef int			CHARACTER_ATTITUDE;
#define NO_ATTITUDE	CHARACTER_ATTITUDE(-1)


//���������, ����������� ��������� ������ ��������� � �������
struct SRelation
{
	SRelation();
	~SRelation();
	CHARACTER_GOODWILL		Goodwill		() const;
	void					SetGoodwill		(CHARACTER_GOODWILL new_goodwill);
private:
	//���������������
	CHARACTER_GOODWILL m_iGoodwill;
};

DEFINE_MAP(u16, SRelation, RELATION_MAP, RELATION_MAP_IT);