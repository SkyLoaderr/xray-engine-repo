///////////////////////////////////////////////////////////////
// InfoPortion.h
// струтура, предствал€юща€ сюжетную информацию
///////////////////////////////////////////////////////////////


#pragma once


#include "ui/uistring.h"
#include "ui/xrXMLParser.h"



#define NO_INFO_INDEX (-1)

//структура дл€ локации на карте
struct SMapLocation
{
	SMapLocation() 
	{
		level_num = 0;
		x = y = 0;
		name.SetText("");
		text.SetText("");
		
		attached_to_object = false;
		object_id = 0xffff;
	}

	//номер уровн€
	int level_num;
	//координаты на уровне
	float x;
	float y;
	//им€ локации
	CUIString name;
	//текст описани€
	CUIString text;
	
	//присоединина ли локаци€ к объекту
	bool attached_to_object;
	//им€ объекта на уровне
	u16 object_id;

	//размеры и положение иконки
	int icon_x, icon_y, icon_width, icon_height;

};


DEF_LIST (INFO_INDEX_LIST, int);

//вопрос 
typedef struct tagSInfoQuestion
{
	//пор€дковый номер вопроса в информации
	int num_in_info;
	//индекс информации котора€ содержит этот вопрос
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
DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);

//квант  - порци€ информации
class CInfoPortion 
{
public:
	CInfoPortion(void);
	virtual ~CInfoPortion(void);

	int				GetIndex()			{return m_iIndex;}
	int				GetLocationsNum()	  {return m_MapLocationVector.size();}
	SMapLocation&	GetLocation(int index) {return	m_MapLocationVector[index];}
	char*			GetText()		{return m_text.GetBuf();}

	//загрузка структуры информацией из файла
	void			Load(int index);
	void			GetQuestion(SInfoQuestion& question, int question_num);
	//функци€ загрузки из XML
	void			LoadInfoPortionFromXml(CUIXml& uiXml, int num_in_file);

	//список вопросов, которые игрок может задавать после получени€ 
	//информации
	INFO_QUESTIONS_LIST m_QuestionsList;
	

protected:
	bool m_bLoaded;

	//уникальный индекс в списке всех возможных квантов информации
	int	m_iIndex;
	//список локаций на карте
	LOCATIONS_VECTOR	m_MapLocationVector;
	//текстовое представление информации
	CUIString m_text;
};