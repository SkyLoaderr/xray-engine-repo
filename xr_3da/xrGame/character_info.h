//////////////////////////////////////////////////////////////////////////
// character_info.h			������� ���������� ��� ���������� � ����
// 
//////////////////////////////////////////////////////////////////////////

#pragma		once
#include "alife_space.h"

#define DEFAULT_CHARACTER_FILE "npc_profile.xml"
#define NO_GOODWILL -1


class CInventoryOwner;

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


class CCharacterInfo
{
private:
	friend CInventoryOwner;
public:
	CCharacterInfo();
	~CCharacterInfo();

	virtual LPCSTR Name() const ;
	virtual LPCSTR Rank() const ;
	virtual LPCSTR Community() const ;

	int		TradeIconX()	const	 {return m_iIconX;}
	int		TradeIconY()	const	 {return m_iIconY;}
	int		MapIconX()		const 	{return m_iMapIconX;}
	int		MapIconY()		const	{return m_iMapIconY;}


	//�������� ������� ��������� �� xml �����,
	//���� ������� �� ������ - ���������� false, ����� true
	bool Load(LPCSTR name_id, LPCSTR xml_file = DEFAULT_CHARACTER_FILE);


	int					 GetGoodwill			(u16 person_id) const ;
	void				 SetGoodwill			(u16 person_id, int goodwill);
	ALife::ERelationType GetRelationType		(u16 person_id) const ;
	void				 SetRelationType		(u16 person_id, ALife::ERelationType new_relation);
	void				 ClearRelations	();

protected:
	//////////////////////////////////////////////////////////////////////////
	// ������� ������������� 

	//������� ��� ���������
	ref_str m_sGameName;

	//��������...(���� ��� RGP ���������)
	ref_str m_sTeamName;
	ref_str m_sRank;

	//��������� ������
	ref_str m_sStartDialog;


	//��������� ������� �� ����� � ������ ����������
	DEFINE_MAP(u16, SRelation, RELATION_MAP, RELATION_MAP_IT);
	RELATION_MAP m_RelationMap;

	//////////////////////////////////////////////////////////////////////////
	// ���������� �������������
	
	//��� ������������ ������
	ref_str m_sVisualName;
	//��������� ������� ������ (��� �������� � �������) � ����� � �������� 
	int	m_iIconX, m_iIconY;	
	//��������� ��������� ������ (��� �����)
	int	m_iMapIconX, m_iMapIconY;	
};