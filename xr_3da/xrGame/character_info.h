//////////////////////////////////////////////////////////////////////////
// character_info.h			������, ��� ������������� ������������ ��������
// 
//////////////////////////////////////////////////////////////////////////

#pragma		once

#include "character_info_defs.h"
#include "shared_data.h"
#include "xml_str_id_loader.h"


#ifdef XRGAME_EXPORTS
	#include "PhraseDialogDefs.h"
	#include "specific_character.h"
	#include "relation_registry.h"
	#include "character_community.h"
#endif


//////////////////////////////////////////////////////////////////////////
// SCharacterProfile: ������ ������� ���������
//////////////////////////////////////////////////////////////////////////
struct SCharacterProfile : CSharedResource
{
#ifdef XRGAME_EXPORTS
	SCharacterProfile ();
	virtual ~SCharacterProfile ();

    //���� ������, �� ���������� ������ ����� �������,
	//����� ������ ��������,��������������� �������
	SPECIFIC_CHARACTER_INDEX m_iCharacterIndex;	

	//��������� ��������� ���������
	CHARACTER_CLASS			m_Class;
	CHARACTER_RANK			m_Rank;
	CHARACTER_REPUTATION	m_Reputation;
#endif
};


class CInventoryOwner;
class CSE_ALifeTraderAbstract;

class CCharacterInfo: public CSharedClass<SCharacterProfile, PROFILE_INDEX>,
					  public CXML_IdToIndex<PROFILE_ID, PROFILE_INDEX, CCharacterInfo>
{
private:
	typedef CSharedClass	<SCharacterProfile, PROFILE_INDEX>			inherited_shared;
	typedef CXML_IdToIndex	<PROFILE_ID, PROFILE_INDEX, CCharacterInfo>	id_to_index;

	friend id_to_index;
	friend CInventoryOwner;
	friend CSE_ALifeTraderAbstract;
public:

#ifdef XRGAME_EXPORTS

	CCharacterInfo();
	~CCharacterInfo();

	virtual void Load	(PROFILE_ID id);
	virtual void Load	(PROFILE_INDEX index);


	void load	(IReader&);
	void save	(NET_Packet&);

	//������������� ������� �������������
	//�������� ���������������� CSpecificCharacter, �� 
	//���������� �������
	void InitSpecificCharacter (SPECIFIC_CHARACTER_INDEX new_index);
#endif

protected:
	const SCharacterProfile* data() const	{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	SCharacterProfile* data()				{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	static void	 InitXmlIdToIndex	();

#ifdef XRGAME_EXPORTS
	//�������� �� XML �����
	virtual void load_shared		(LPCSTR);

	//������ ������������ �������
	PROFILE_INDEX m_iProfileIndex;
	
	//������ ������ � ���������� ���������, �������
	//������������ � ������ ���������� ������
	SPECIFIC_CHARACTER_INDEX m_iSpecificCharacterIndex;
	//����������� ���������� � ���������� ���������
	CSpecificCharacter m_SpecificCharacter;

public:

	LPCSTR						Name()				const;
	LPCSTR						Bio()				const;
	const CHARACTER_COMMUNITY&	Community()			const;

	CHARACTER_RANK				Rank()				const;
	CHARACTER_REPUTATION		Reputation()		const;

	void	SetRank			(CHARACTER_RANK				rank);
	void	SetReputation	(CHARACTER_REPUTATION		reputation);
	void	SetCommunity	(const CHARACTER_COMMUNITY&	community);


	int		TradeIconX	()	const;
	int		TradeIconY	()	const;
	int		MapIconX	()	const;
	int		MapIconY	()	const;

	PHRASE_DIALOG_INDEX			StartDialog	()	const;
	const DIALOG_INDEX_VECTOR&	ActorDialogs()	const;

	RELATION_REGISTRY&			Relations ();
protected:
	//���� ��������� � ������� �����������
	RELATION_REGISTRY relation_registry;

	CHARACTER_RANK					m_CurrentRank;
	CHARACTER_REPUTATION			m_CurrentReputation;
	CHARACTER_COMMUNITY				m_CurrentCommunity;
#endif
};