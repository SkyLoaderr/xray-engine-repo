///////////////////////////////////////////////////////////////
// InfoPortion.h
// струтура, предстваляющая сюжетную информацию
///////////////////////////////////////////////////////////////


#pragma once


#include "ui\\uistring.h"
#include "ui\\xrXMLParser.h"


//структура для локации на карте
typedef struct tagSMapLocation
{
	//номер уровня
	int level_num;
	//координаты на уровне
	float x;
	float y;

} SMapLocation;


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

	int				GetIndex()		{return m_iIndex;}
	SMapLocation&	GetLocation()	{return m_sMapLocation;}
	char*			GetText()		{return m_text.GetBuf();}

	//загрузка структуры информацией из файла
	void			Load(int index);
	void			GetQuestion(SInfoQuestion& question, int question_num);
	//функция загрузки из XML
	void LoadInfoPortionFromXml(CUIXml& uiXml, int num_in_file);

	//список вопросов, которые игрок может задавать после получения 
	//информации
	INFO_QUESTIONS_LIST m_QuestionsList;


protected:
	bool m_bLoaded;

	//уникальный индекс в списке всех возможных квантов информации
	int	m_iIndex;

	//указывает локацию, если она задана
	bool m_bLocationSet;
	SMapLocation m_sMapLocation;

	//текстовое представление информации
	CUIString m_text;
};