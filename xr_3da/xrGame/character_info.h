//////////////////////////////////////////////////////////////////////////
// character_info.h			игровая информация для персонажей в игре
// 
//////////////////////////////////////////////////////////////////////////

#pragma		once
#include "alife_space.h"

#define DEFAULT_CHARACTER_FILE "npc_profile.xml"


class CInventoryOwner;

//структура, описывающая отношение одного персонажа к другому
struct SRelation
{
	SRelation();
	~SRelation();

	ALife::ERelationType	RelationType	() const;
	void					SetRelationType	(ALife::ERelationType relation);

	int						Goodwill		() const;
	void					SetGoodwill		(int new_goodwill);
private:
	//отношения (враг, нейтрал, друг)
	ALife::ERelationType m_eRelationType;
	//благосклонность
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


	//загрузка профиля персонажа из xml файла,
	//если профиль не найден - возвращаем false, иначе true
	bool Load(LPCSTR name_id, LPCSTR xml_file = DEFAULT_CHARACTER_FILE);


	int					 GetGoodwill			(u16 person_id) const ;
	void				 SetGoodwill			(u16 person_id, int goodwill);
	ALife::ERelationType GetRelationType		(u16 person_id) const ;
	void				 SetRelationType		(u16 person_id, ALife::ERelationType new_relation);
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
	DEFINE_MAP(u16, SRelation, RELATION_MAP, RELATION_MAP_IT);
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