//////////////////////////////////////////////////////////////////////////
// relation_registry.h: ������ ��� �������� ������ �� ��������� ��������� � 
//						������ ����������
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "character_info_defs.h"

class CRelationRegistryWrapper;

//////////////////////////////////////////////////////////////////////////

#define GAME_RELATIONS_SECT "game_relations"

//////////////////////////////////////////////////////////////////////////

struct RELATION_REGISTRY 
{
public:
	RELATION_REGISTRY  ();
	virtual ~RELATION_REGISTRY ();

public:	

	template<typename T>
	ALife::ERelationType GetRelationBetween			(T char1,T char2) const;

	template<typename T>
	ALife::ERelationType GetRelationType			(T from, T to) const ;
	template<typename T>
	void				 SetRelationType			(T from, T to, ALife::ERelationType new_relation);

	//����� ���������  ������ ��������� � �������, ����������� �� �������
	//� ������ ���� �������� - �������� �� 
	//-100< (������ ����������) �� >100 (����� �����������)
	
	template<typename T>
	CHARACTER_GOODWILL	 GetAttitude				(T from, T to)	const ;

	//������ ��������� (���������������) ������ ��������� � ������� - 
	//�������� �� -100< (������ ����������) �� >100 (����� �����������)
	CHARACTER_GOODWILL	 GetGoodwill				(u16 from, u16 to) const ;
	void				 SetGoodwill				(u16 from, u16 to, CHARACTER_GOODWILL goodwill);
	void				 ChangeGoodwill 			(u16 from, u16 to, CHARACTER_GOODWILL delta_goodwill);

	//��������� ����������� � ��������� (������ ���, � �� ��������)
	//�.�. �������� ��� ������, ��� � ���� ����� ����������� ���������
	CHARACTER_GOODWILL	 GetCommunityGoodwill		(CHARACTER_COMMUNITY_INDEX from_community, u16 to_character) const ;
	void				 SetCommunityGoodwill		(CHARACTER_COMMUNITY_INDEX from_community, u16 to_character, CHARACTER_GOODWILL goodwill);
	void				 ChangeCommunityGoodwill	(CHARACTER_COMMUNITY_INDEX from_community, u16 to_character, CHARACTER_GOODWILL delta_goodwill);

	void				 ClearRelations				(u16 person_id);

private:
	CHARACTER_GOODWILL	 GetCommunityRelation		(CHARACTER_COMMUNITY_INDEX, CHARACTER_COMMUNITY_INDEX) const;	
	CHARACTER_GOODWILL	 GetRankRelation			(CHARACTER_RANK_VALUE, CHARACTER_RANK_VALUE) const;
	CHARACTER_GOODWILL	 GetReputationRelation		(CHARACTER_REPUTATION_VALUE, CHARACTER_REPUTATION_VALUE) const;
	
public:
	static CRelationRegistryWrapper&			relation_registry();
	static void									clear_relation_registry();
private:
	static CRelationRegistryWrapper				*m_relation_registry;
};

#include "relation_registry_inline.h"