//////////////////////////////////////////////////////////////////////////
// character_info.h			игровая информация для персонажей в игре
// 
//////////////////////////////////////////////////////////////////////////

#pragma		once

#include "character_info_defs.h"
#include "PhraseDialogDefs.h"

#include "alife_registry_container.h"
#include "alife_registry_wrapper.h"

#include "shared_data.h"
#include "xml_str_id_loader.h"


//////////////////////////////////////////////////////////////////////////
// SCharacterProfile: данные профиля персонажа
//////////////////////////////////////////////////////////////////////////
struct SCharacterProfile : CSharedResource
{
	SCharacterProfile ();
	virtual ~SCharacterProfile ();

	//игровое имя персонажа
	ref_str m_sGameName;

	//начальный диалог
	PHRASE_DIALOG_INDEX m_iStartDialog;

	//положение большой икноки (для торговли и общения) в файле с иконками 
	int	m_iIconX, m_iIconY;
	//положение мальнькой иконки (для карты)
	int	m_iMapIconX, m_iMapIconY;
};


class CInventoryOwner;

class CCharacterInfo: public CSharedClass<SCharacterProfile, PROFILE_INDEX>,
					  public CXML_IdToIndex<PROFILE_ID, PROFILE_INDEX, CCharacterInfo>
{
private:
	typedef CSharedClass	<SCharacterProfile, PROFILE_INDEX>			inherited_shared;
	typedef CXML_IdToIndex	<PROFILE_ID, PROFILE_INDEX, CCharacterInfo>	id_to_index;

	friend id_to_index;
	friend CInventoryOwner;
public:
	
	CCharacterInfo();
	~CCharacterInfo();


	//инициализация данными
	//если таким id раньше не использовалось
	//будет загружено из файла
	virtual void Load	(PROFILE_ID id);
	virtual void Load	(PROFILE_INDEX index);


protected:
	const SCharacterProfile* data() const { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	SCharacterProfile* data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	//загрузка из XML файла
	virtual void load_shared		(LPCSTR);
	static void	 InitXmlIdToIndex	();

	//индекс загруженного профиля
	PROFILE_INDEX m_iProfileIndex;

public:

	LPCSTR	Name()		const ;
	LPCSTR	Rank()		const ;
	LPCSTR	Community() const ;

	int		TradeIconX() const	 {return data()->m_iIconX;}
	int		TradeIconY() const	 {return data()->m_iIconY;}
	int		MapIconX()	 const 	 {return data()->m_iMapIconX;}
	int		MapIconY()	 const	 {return data()->m_iMapIconY;}


	int					 GetGoodwill			(u16 person_id) const ;
	void				 SetGoodwill			(u16 person_id, int goodwill);
	ALife::ERelationType GetRelationType		(u16 person_id) const ;
	void				 SetRelationType		(u16 person_id, ALife::ERelationType new_relation);
	void				 ClearRelations			();

protected:
	//наши отношения с другими персонажами
	typedef CALifeRegistryWrapper<CRelationRegistry> RELATION_REGISTRY;
	RELATION_REGISTRY relation_registry;

	//временно...(пока нет RGP компонент)
	ref_str m_sTeamName;
	ref_str m_sRank;
};