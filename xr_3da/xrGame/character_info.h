//////////////////////////////////////////////////////////////////////////
// character_info.h			������� ���������� ��� ���������� � ����
// 
//////////////////////////////////////////////////////////////////////////

#pragma		once

#define DEFAULT_CHARACTER_FILE "npc_profile.xml"


class CInventoryOwner;

class CCharacterInfo
{
private:
	friend CInventoryOwner;
public:
	CCharacterInfo();
	~CCharacterInfo();

	virtual LPCSTR Name();
	virtual LPCSTR Rank();
	virtual LPCSTR Community();

	int		TradeIconX() {return m_iIconX;}
	int		TradeIconY() {return m_iIconY;}

	//�������� ������� ��������� �� xml �����,
	//���� ������� �� ������ - ���������� false, ����� true
	bool Load(LPCSTR name_id, LPCSTR xml_file = DEFAULT_CHARACTER_FILE);
protected:
	//////////////////////////////////////////////////////////////////////////
	// ������� ������������� 

	//������� ��� ���������
	ref_str m_sGameName;

	//��������...(���� ��� RGP ���������)
	ref_str m_sTeamName;
	ref_str m_sRank;

	//////////////////////////////////////////////////////////////////////////
	// ���������� �������������
	
	//��� ������������ ������
	ref_str m_sVisualName;
	//��������� ������� ������ (��� �������� � �������) � ����� � �������� 
	int	m_iIconX, m_iIconY;	
	//��������� ��������� ������ (��� �����)
	int	m_iMapIconX, m_iMapIconY;	
};