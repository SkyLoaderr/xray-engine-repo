///////////////////////////////////////////////////////////////
// InfoPortion.h
// струтура, предствал€юща€ сюжетную информацию
///////////////////////////////////////////////////////////////


#pragma once

#include "shared_data.h"

#include "ui/xrXMLParser.h"
#include "map_location.h"
#include "PhraseScript.h"

//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: данные дл€ InfoProtion
//////////////////////////////////////////////////////////////////////////
struct SInfoPortionData : CSharedResource
{
	SInfoPortionData ();
	virtual ~SInfoPortionData ();

	//уникальный индекс порции информации
	INFO_ID		m_InfoId;

	//массив с именами диалогов, которые могут быть инициированы
	//из этого InfoPortion
	DEFINE_VECTOR(ref_str, DIALOG_NAME_VECTOR, DIALOG_NAME_VECTOR_IT);
	DIALOG_NAME_VECTOR m_DialogNames;

	//список локаций на карте
	LOCATIONS_VECTOR	m_MapLocations;

	//скриптовые действи€, которые активируетс€ после того как 
	//информацию получает персонаж
	CPhraseScript m_PhraseScript;

	//массив с индексами тех порций информации, которые
	//исчезнут, после получени€ этой info_portion
	DEFINE_VECTOR(INFO_ID, INFO_INDEX_VECTOR, INFO_INDEX_VECTOR_IT);
	INFO_INDEX_VECTOR m_DisableInfo;

	//текстовое представление информации	
	ref_str		m_text;
};




//квант  - порци€ информации
class CInfoPortion : public CSharedClass<SInfoPortionData, INFO_ID>
{
private:
	typedef CSharedClass<SInfoPortionData, INFO_ID> inherited_shared;
public:
				CInfoPortion(void);
	virtual		~CInfoPortion(void);

	//инициализаци€ info данными
	//если info с таким id раньше не использовалс€
	//он будет загружен из файла
	virtual void Load	(INFO_STR_ID info_str_id);
	virtual void Load	(INFO_ID info_id);

	const LOCATIONS_VECTOR&							MapLocations()	{return info_data()->m_MapLocations;}
	const SInfoPortionData::DIALOG_NAME_VECTOR&		DialogNames()	{return info_data()->m_DialogNames;}
	const SInfoPortionData::INFO_INDEX_VECTOR&		DisableInfos()	{return info_data()->m_DisableInfo;}
	
	//запуск присоединенных скриптовых функций	
	virtual void RunScriptActions   (const CGameObject* pOwner)	{info_data()->m_PhraseScript.Action(pOwner);}

	//текстовое представление информации
	virtual LPCSTR GetText ();


protected:
    INFO_ID	m_InfoId;

	void load_shared	(LPCSTR xml_file);
	SInfoPortionData* info_data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	//дл€ работы с ID (строковыми и числовыми)
public:
	static INFO_ID		StrToID			(const INFO_STR_ID& info_str_id);
	static INFO_STR_ID	IDToStr			(INFO_ID info_id);
	//удаление статичекого массива
	static void			DeleteStrToID	();

	DEFINE_VECTOR(ref_str, STR_ID_VECTOR, STR_ID_VECTOR_IT);
	static STR_ID_VECTOR& StrIdVector ();
protected:
	static xr_vector<ref_str>* m_pStrToID;
};