//////////////////////////////////////////////////////////////////////////
// character_info.h			������, ��� ������������� ������������ ��������
// 
//////////////////////////////////////////////////////////////////////////

#pragma		once

#include "character_info_defs.h"
#include "shared_data.h"
#include "xml_str_id_loader.h"

class NET_Packet;

#ifndef AI_COMPILER
	#include "specific_character.h"
#endif

#ifdef XRGAME_EXPORTS
	#include "PhraseDialogDefs.h"
	#include "character_community.h"
	#include "character_rank.h"
	#include "character_reputation.h"
#endif


//////////////////////////////////////////////////////////////////////////
// SCharacterProfile: ������ ������� ���������
//////////////////////////////////////////////////////////////////////////
struct SCharacterProfile : CSharedResource
{
	SCharacterProfile ();
	virtual ~SCharacterProfile ();

    //���� ������, �� ���������� ������ ����� �������,
	//����� ������ ��������,��������������� �������
	SPECIFIC_CHARACTER_INDEX m_iCharacterIndex;	

	//��������� ��������� ���������
	CHARACTER_CLASS					m_Class;
	CHARACTER_RANK_VALUE			m_Rank;
	CHARACTER_REPUTATION_VALUE		m_Reputation;
};


class CInventoryOwner;
class CSE_ALifeTraderAbstract;

class CCharacterInfo: public CSharedClass<SCharacterProfile, PROFILE_INDEX, false>,
					  public CXML_IdToIndex<PROFILE_ID, PROFILE_INDEX, CCharacterInfo>
{
private:
	typedef CSharedClass	<SCharacterProfile, PROFILE_INDEX, false>	inherited_shared;
	typedef CXML_IdToIndex	<PROFILE_ID, PROFILE_INDEX, CCharacterInfo>	id_to_index;

	friend id_to_index;
	friend CInventoryOwner;
	friend CSE_ALifeTraderAbstract;
public:



	CCharacterInfo();
	~CCharacterInfo();

	virtual void Load	(PROFILE_ID id);
	virtual void Load	(PROFILE_INDEX index);

#ifdef XRGAME_EXPORTS
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


	//�������� �� XML �����
	virtual void load_shared		(LPCSTR);

	//������ ������������ �������
	PROFILE_INDEX m_iProfileIndex;
	
	//������ ������ � ���������� ���������, �������
	//������������ � ������ ���������� ������
	SPECIFIC_CHARACTER_INDEX m_iSpecificCharacterIndex;

#ifdef XRGAME_EXPORTS
	//����������� ���������� � ���������� ���������
	CSpecificCharacter m_SpecificCharacter;
#endif

public:


#ifdef XRGAME_EXPORTS
	PROFILE_INDEX				Profile()			const;
	LPCSTR						Name()				const;
	LPCSTR						Bio()				const;


	const CHARACTER_COMMUNITY&	Community()			const;
	const CHARACTER_RANK&		Rank()				const;
	const CHARACTER_REPUTATION&	Reputation()		const;

	//�������� ������ � InventoryOwner
protected:
	void	SetRank			(CHARACTER_RANK_VALUE			rank);
	void	SetReputation	(CHARACTER_REPUTATION_VALUE		reputation);
	void	SetCommunity	(const CHARACTER_COMMUNITY&		community);

public:
	int		TradeIconX	()	const;
	int		TradeIconY	()	const;
	int		MapIconX	()	const;
	int		MapIconY	()	const;

	PHRASE_DIALOG_INDEX			StartDialog	()	const;
	const DIALOG_INDEX_VECTOR&	ActorDialogs()	const;
#endif

protected:


#ifdef XRGAME_EXPORTS
	CHARACTER_RANK					m_CurrentRank;
	CHARACTER_REPUTATION			m_CurrentReputation;
	CHARACTER_COMMUNITY				m_CurrentCommunity;
#endif
};