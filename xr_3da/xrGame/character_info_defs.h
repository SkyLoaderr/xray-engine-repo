//////////////////////////////////////////////////////////////////////////
// character_info_defs.h		������� ���������� ��� ���������� � ����
// 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

#define NO_GOODWILL -1

//���������, ����������� ��������� ������ ��������� � �������
struct SRelation
{
	SRelation();
	~SRelation();

	ALife::ERelationType	RelationType	() const;
	void					SetRelationType	(ALife::ERelationType relation);

	int						Goodwill		() const;
	void					SetGoodwill		(int new_goodwill);
private:
	//��������� (����, �������, ����)
	ALife::ERelationType m_eRelationType;
	//���������������
	int m_iGoodwill;
};

DEFINE_MAP(u16, SRelation, RELATION_MAP, RELATION_MAP_IT);