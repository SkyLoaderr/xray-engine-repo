///////////////////////////////////////////////////////////////
// InfoPortion.h
// струтура, предстваляющая сюжетную информацию
///////////////////////////////////////////////////////////////


#pragma once


#include "ui/uistring.h"
#include "ui/xrXMLParser.h"
#include "map_location.h"


#define NO_INFO_INDEX (-1)




DEF_LIST (INFO_INDEX_LIST, int);

//вопрос 
typedef struct tagSInfoQuestion
{
	//порядковый номер вопроса в информации
	int num_in_info;
	//индекс информации которая содержит этот вопрос
	int info_portion_index;

	//список индексов кусков информации
	//которые мы можем получить в ответ на этот вопрос
	INFO_INDEX_LIST IndexList;

	//текстовое представление вопроса
	CUIString text;
	//заготовка на негативный ответ
	CUIString negative_answer_text;
		
} SInfoQuestion;


DEF_LIST (INFO_QUESTIONS_LIST, SInfoQuestion);

//квант  - порция информации
class CInfoPortion 
{
public:
	CInfoPortion(void);
	virtual ~CInfoPortion(void);

	int				GetIndex()			{return m_iIndex;}
	int				GetLocationsNum()	  {return m_MapLocationVector.size();}
	SMapLocation&	GetLocation(int index) {return	m_MapLocationVector[index];}
	LPCSTR			GetText()		{return m_text.GetBuf();}

	//загрузка структуры информацией из файла
	void			Load(int index);
	void			GetQuestion(SInfoQuestion& question, int question_num);
	//функция загрузки из XML
	void			LoadInfoPortionFromXml(CUIXml& uiXml, int num_in_file);

	//список вопросов, которые игрок может задавать после получения 
	//информации
	INFO_QUESTIONS_LIST m_QuestionsList;


	DEFINE_VECTOR(ref_str, DIALOG_NAME_VECTOR, DIALOG_NAME_VECTOR_IT);
	//массив с именами диалогов, которые могут быть инициированы
	//из этого InfoPortion
	DIALOG_NAME_VECTOR m_DialogNames;

protected:
	bool m_bLoaded;

	//уникальный индекс в списке всех возможных квантов информации
	int	m_iIndex;
	//список локаций на карте
	LOCATIONS_VECTOR	m_MapLocationVector;
	//текстовое представление информации
	CUIString m_text;
};