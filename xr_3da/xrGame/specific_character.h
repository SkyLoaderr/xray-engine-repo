//////////////////////////////////////////////////////////////////////////
// specific_character.h:	игровая информация для о конкретном 
//							персонажей в игре
//////////////////////////////////////////////////////////////////////////

#pragma		once


#include "character_info_defs.h"
#include "PhraseDialogDefs.h"

#include "shared_data.h"
#include "xml_str_id_loader.h"


//////////////////////////////////////////////////////////////////////////
// SSpecificCharacterData: данные о конкретном персонаже
//////////////////////////////////////////////////////////////////////////
struct SSpecificCharacterData : CSharedResource
{
	SSpecificCharacterData ();
	virtual ~SSpecificCharacterData ();

	//игровое имя персонажа
	ref_str m_sGameName;

	//начальный диалог
	PHRASE_DIALOG_INDEX m_iStartDialog;
	//диалог актера, который будет доступен только при встрече с данным персонажем
	PHRASE_DIALOG_INDEX m_iActorDialog;

	//положение большой икноки (для торговли и общения) в файле с иконками 
	int	m_iIconX, m_iIconY;
	//положение мальнькой иконки (для карты)
	int	m_iMapIconX, m_iMapIconY;

	//команда 
	CHARACTER_COMMUNITY		m_Community;
	//ранг
	CHARACTER_RANK			m_Rank;
	//репутация
	CHARACTER_REPUTATION	m_Reputation;
};


class CCharacterInfo;


class CSpecificCharacter: public CSharedClass<SSpecificCharacterData, SPECIFIC_CHARACTER_INDEX>,
						  public CXML_IdToIndex<SPECIFIC_CHARACTER_ID, SPECIFIC_CHARACTER_INDEX, CSpecificCharacter>
{
private:
	typedef CSharedClass	<SSpecificCharacterData, SPECIFIC_CHARACTER_INDEX>							inherited_shared;
	typedef CXML_IdToIndex	<SPECIFIC_CHARACTER_ID, SPECIFIC_CHARACTER_INDEX, CSpecificCharacter>	id_to_index;

	friend id_to_index;
	friend CCharacterInfo;
public:

	CSpecificCharacter();
	~CSpecificCharacter();

	//инициализация данными
	//если таким id раньше не использовалось
	//будет загружено из файла
	virtual void Load	(SPECIFIC_CHARACTER_ID		id);
	virtual void Load	(SPECIFIC_CHARACTER_INDEX	index);

protected:
	const SSpecificCharacterData* data() const { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	SSpecificCharacterData*		  data()	   { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	//загрузка из XML файла
	virtual void load_shared		(LPCSTR);
	static void	 InitXmlIdToIndex	();

	SPECIFIC_CHARACTER_INDEX m_iOwnIndex;
public:

	LPCSTR					Name		() const ;
	LPCSTR					Community	() const ;
	CHARACTER_RANK			Rank		() const ;
	CHARACTER_REPUTATION	Reputation	() const ;
	

	int		TradeIconX	() const	 {return data()->m_iIconX;}
	int		TradeIconY	() const	 {return data()->m_iIconY;}
	int		MapIconX	() const 	 {return data()->m_iMapIconX;}
	int		MapIconY	() const	 {return data()->m_iMapIconY;}
};


//////////////////////////////////////////////////////////////////////////
