//////////////////////////////////////////////////////////////////////////
// character_info.h			шаблон, для представления абстрактного песонажа
// 
//////////////////////////////////////////////////////////////////////////

#pragma		once

#include "character_info_defs.h"
#include "PhraseDialogDefs.h"

#include "shared_data.h"
#include "xml_str_id_loader.h"

#include "specific_character.h"
#include "relation_registry.h"
#include "character_community.h"



//////////////////////////////////////////////////////////////////////////
// SCharacterProfile: данные профиля персонажа
//////////////////////////////////////////////////////////////////////////
struct SCharacterProfile : CSharedResource
{
	SCharacterProfile ();
	virtual ~SCharacterProfile ();

    //если задано, то выбирается именно такой профиль,
	//иначе ищется случайно,удовлетворяющее шаблону
	SPECIFIC_CHARACTER_INDEX m_iCharacterIndex;	

	//требуемые параметры характера
	CHARACTER_COMMUNITY		m_Community;
	CHARACTER_RANK			m_Rank;
	CHARACTER_REPUTATION	m_Reputation;
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
	
	CCharacterInfo();
	~CCharacterInfo();

	virtual void Load	(PROFILE_ID id);
	virtual void Load	(PROFILE_INDEX index);


	void load	(IReader&);
	void save	(NET_Packet&);

	//инициализация профиля подразумевает
	//загрузку соответствующего CSpecificCharacter, по 
	//указанному индексу
	void InitSpecificCharacter (SPECIFIC_CHARACTER_INDEX new_index);

protected:
	const SCharacterProfile* data() const	{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	SCharacterProfile* data()				{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	//загрузка из XML файла
	virtual void load_shared		(LPCSTR);
	static void	 InitXmlIdToIndex	();

	//индекс загруженного профиля
	PROFILE_INDEX m_iProfileIndex;
	
	//индекс данных о конкретном персонаже, который
	//используется в данном экземпляре класса
	SPECIFIC_CHARACTER_INDEX m_iSpecificCharacterIndex;
	//загруженная информация о конкретном персонаже
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
	//наши отношения с другими персонажами
	RELATION_REGISTRY relation_registry;

	CHARACTER_RANK					m_CurrentRank;
	CHARACTER_REPUTATION			m_CurrentReputation;
	CHARACTER_COMMUNITY				m_CurrentCommunity;
};