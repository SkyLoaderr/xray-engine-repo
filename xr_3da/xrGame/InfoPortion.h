///////////////////////////////////////////////////////////////
// InfoPortion.h
// струтура, предствал€юща€ сюжетную информацию
///////////////////////////////////////////////////////////////


#pragma once

#include "shared_data.h"

#include "ui/xrXMLParser.h"
#include "map_location.h"
#include "PhraseScript.h"

#include "xml_str_id_loader.h"
#include "encyclopedia_article_defs.h"

#include "GameTaskDefs.h"
#include "PhraseDialogDefs.h"


//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: данные дл€ InfoProtion
//////////////////////////////////////////////////////////////////////////
struct SInfoPortionData : CSharedResource
{
	SInfoPortionData ();
	virtual ~SInfoPortionData ();

	//уникальный индекс порции информации
	INFO_INDEX	m_InfoIndex;

	//массив с именами диалогов, которые могут быть инициированы
	//из этого InfoPortion
	DIALOG_ID_VECTOR m_DialogNames;

	//список локаций на карте
	LOCATIONS_VECTOR	m_MapLocations;
	//список статей в энциклопедии, которые станов€тс€ известными 
	ARTICLE_INDEX_VECTOR m_Articles;
	//список статей в энциклопедии, которые станов€тс€ неизвестными (на тот случай если
	//нужно заменить одну статью другой)
	ARTICLE_INDEX_VECTOR m_ArticlesDisable;
    	
	//присоединенные задани€
	TASK_INDEX_VECTOR	m_GameTasks;

	//скриптовые действи€, которые активируетс€ после того как 
	//информацию получает персонаж
	CPhraseScript m_PhraseScript;

	//массив с индексами тех порций информации, которые
	//исчезнут, после получени€ этой info_portion
	DEFINE_VECTOR(INFO_INDEX, INFO_INDEX_VECTOR, INFO_INDEX_VECTOR_IT);
	INFO_INDEX_VECTOR m_DisableInfo;

	//текстовое представление информации	
	shared_str		m_text;
};


class CInfoPortion;

//квант  - порци€ информации
class CInfoPortion : public CSharedClass<SInfoPortionData, INFO_INDEX>,
					 public CXML_IdToIndex<INFO_STR_ID,	INFO_INDEX, CInfoPortion>
{
private:
	typedef CSharedClass<SInfoPortionData, INFO_INDEX> inherited_shared;
	typedef CXML_IdToIndex<INFO_STR_ID, INFO_INDEX, CInfoPortion>	id_to_index;

	friend id_to_index;
public:
				CInfoPortion	(void);
	virtual		~CInfoPortion	(void);

	//инициализаци€ info данными
	//если info с таким id раньше не использовалс€
	//он будет загружен из файла
	virtual void Load	(INFO_STR_ID info_str_id);
	virtual void Load	(INFO_INDEX info_index);

	const LOCATIONS_VECTOR&							MapLocations()	const {return info_data()->m_MapLocations;}
	const ARTICLE_INDEX_VECTOR&						Articles	()	const {return info_data()->m_Articles;}
	const ARTICLE_INDEX_VECTOR&						ArticlesDisable	()	const {return info_data()->m_ArticlesDisable;}
	const TASK_INDEX_VECTOR&						GameTasks	()	const {return info_data()->m_GameTasks;}
	const DIALOG_ID_VECTOR&							DialogNames	()	const {return info_data()->m_DialogNames;}
	const SInfoPortionData::INFO_INDEX_VECTOR&		DisableInfos()	const {return info_data()->m_DisableInfo;}
	
	//запуск присоединенных скриптовых функций	
	virtual void RunScriptActions   (const CGameObject* pOwner)	{info_data()->m_PhraseScript.Action(pOwner);}

	//текстовое представление информации
	virtual LPCSTR GetText () const ;


protected:
    INFO_INDEX	m_InfoIndex;

	void load_shared	(LPCSTR);
	SInfoPortionData* info_data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	const SInfoPortionData* info_data() const { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	static void InitXmlIdToIndex();
};