//////////////////////////////////////////////////////////////////////////
// character_info.h			игровая информация для персонажей в игре
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

	//загрузка профиля персонажа из xml файла,
	//если профиль не найден - возвращаем false, иначе true
	bool Load(LPCSTR name_id, LPCSTR xml_file = DEFAULT_CHARACTER_FILE);


	ALife::ERelationType GetRelation	(u16 person_id);
	void				 SetRelation	(u16 person_id, ALife::ERelationType new_relation);
	void				 ClearRelations	();

protected:
	//////////////////////////////////////////////////////////////////////////
	// игровое представление 

	//игровое имя персонажа
	ref_str m_sGameName;

	//временно...(пока нет RGP компонент)
	ref_str m_sTeamName;
	ref_str m_sRank;


	//отношения которые мы имеем о других персонажей
	DEFINE_MAP(u16, ALife::ERelationType, RELATION_MAP, RELATION_MAP_IT);
	RELATION_MAP m_RelationMap;

	//////////////////////////////////////////////////////////////////////////
	// визуальное представление
	
	//имя используемой модели
	ref_str m_sVisualName;
	//положение большой икноки (для торговли и общения) в файле с иконками 
	int	m_iIconX, m_iIconY;	
	//положение мальнькой иконки (для карты)
	int	m_iMapIconX, m_iMapIconY;	
};