//////////////////////////////////////////////////////////////////////////
// specific_character.h:	игровая информация для о конкретном 
//							персонажей в игре
//////////////////////////////////////////////////////////////////////////

#pragma		once


#include "character_info_defs.h"
#include "shared_data.h"
#include "xml_str_id_loader.h"


#ifdef XRGAME_EXPORTS

#include "PhraseDialogDefs.h"
#include "character_community.h"

#endif


//////////////////////////////////////////////////////////////////////////
// SSpecificCharacterData: данные о конкретном персонаже
//////////////////////////////////////////////////////////////////////////
struct SSpecificCharacterData : CSharedResource
{

#ifdef  XRGAME_EXPORTS

	SSpecificCharacterData ();
	virtual ~SSpecificCharacterData ();

	//игровое имя персонажа
	std::string m_sGameName;
	//текст с биографией персонажа (линк на string table)
	std::string m_sBioText;
	//строка содержащая предметы, которые нужно проспавнить 
	std::string m_sSupplySpawn;

#endif

	//имя модели
	std::string m_sVisual;

#ifdef  XRGAME_EXPORTS
	
	//начальный диалог
	PHRASE_DIALOG_INDEX m_iStartDialog;
	//диалоги актера, которые будут доступны только при встрече с данным персонажем
	DIALOG_INDEX_VECTOR m_ActorDialogs;

	//положение большой икноки (для торговли и общения) в файле с иконками 
	int	m_iIconX, m_iIconY;
	//положение мальнькой иконки (для карты)
	int	m_iMapIconX, m_iMapIconY;

	//команда 
	CHARACTER_COMMUNITY			m_Community;

#endif
	
	//ранг
	CHARACTER_RANK				m_Rank;
	//репутация
	CHARACTER_REPUTATION		m_Reputation;

	//классы персонажа (военные-ветераны, ученые и т.д.)
	//к которым он принадлежит
	xr_vector<CHARACTER_CLASS>	m_Classes;


	//указание на то что персонаж не предназначен для случайного выбора
	//и задается только через явное указание ID
	bool m_bNoRandom;
	//если персонаж является заданым по умолчанию для своей команды
	bool m_bDefaultForCommunity;
};

class CInventoryOwner;
class CCharacterInfo;
class CSE_ALifeTraderAbstract;


class CSpecificCharacter: public CSharedClass<SSpecificCharacterData, SPECIFIC_CHARACTER_INDEX>,
						  public CXML_IdToIndex<SPECIFIC_CHARACTER_ID, SPECIFIC_CHARACTER_INDEX, CSpecificCharacter>
{
private:
	typedef CSharedClass	<SSpecificCharacterData, SPECIFIC_CHARACTER_INDEX>							inherited_shared;
	typedef CXML_IdToIndex	<SPECIFIC_CHARACTER_ID, SPECIFIC_CHARACTER_INDEX, CSpecificCharacter>	id_to_index;

	friend id_to_index;
	friend CInventoryOwner;
	friend CCharacterInfo;
	friend CSE_ALifeTraderAbstract;
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

#ifdef  XRGAME_EXPORTS
	LPCSTR						Name		() const ;
	LPCSTR						Bio			() const ;
	const CHARACTER_COMMUNITY&	Community	() const ;
#endif

	CHARACTER_RANK				Rank		() const ;
	CHARACTER_REPUTATION		Reputation	() const ;
	LPCSTR						Visual		() const ;

#ifdef  XRGAME_EXPORTS
	LPCSTR					SupplySpawn	() const ;
	
	int		TradeIconX	() const	 {return data()->m_iIconX;}
	int		TradeIconY	() const	 {return data()->m_iIconY;}
	int		MapIconX	() const 	 {return data()->m_iMapIconX;}
	int		MapIconY	() const	 {return data()->m_iMapIconY;}
#endif
};


//////////////////////////////////////////////////////////////////////////
