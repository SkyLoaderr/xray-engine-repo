//////////////////////////////////////////////////////////////////////////
// character_info.h			игровая информация для персонажей в игре
// 
//////////////////////////////////////////////////////////////////////////

#pragma		once

#define DEFAULT_CHARACTER_FILE "npc_profile.xml"

class CCharacterInfo
{
public:
	CCharacterInfo();
	~CCharacterInfo();

	virtual LPCSTR Name();
	virtual LPCSTR Rank();
	virtual LPCSTR Community();

	//загрузка профиля персонажа из xml файла,
	//если профиль не найден - возвращаем false, иначе true
	bool Load(LPCSTR name_id, LPCSTR xml_file = DEFAULT_CHARACTER_FILE);
protected:
	//////////////////////////////////////////////////////////////////////////
	// игровое представление 

	//игровое имя персонажа
	ref_str m_sGameName;

	//временно...(пока нет RGP компонент)
	ref_str m_sTeamName;
	ref_str m_sRank;

	//////////////////////////////////////////////////////////////////////////
	// визуальное представление
	
	//имя используемой модели
	ref_str m_sVisualName;
	//положение большой икноки (для торговли и общения) в файле с иконками 
	int	m_iIconX, m_iIconY;	
	//положение мальнькой иконки (для карты)
	int	m_iMapIconX, m_iMapIconY;	
};