///////////////////////////////////////////////////////////////
// game_news.h
// реестр новостей: новости симуляции + сюжетные
///////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "string_table_defs.h"
#include "object_interfaces.h"

#define NOT_SIMULATION_NEWS ALife::_NEWS_ID(-1)


//-----------------------------------------------------------------------------/
//	News related routines
//-----------------------------------------------------------------------------/




struct GAME_NEWS_DATA: public IPureSerializeObject
{	
	GAME_NEWS_DATA();

	virtual void load (IReader&);
	virtual void save (IWriter&);


	//id новости из симуляции ALife::_NEWS_ID(-1) то новость - сюжетная
	ALife::_NEWS_ID news_id;
	//текст для сюжетной новости
	STRING_INDEX news_text;

	//время получения новости
	ALife::_TIME_ID		receive_time;

	//положение иконки и имя текстуры (если задано)
	LPCSTR texture_name;
	int x1, y1, x2, y2;

	LPCSTR FullText();

private:
	//полный текст новостей, формируется при первом обращении
	std::string full_news_text;

	// Структурка описывающая ньюс
	typedef struct tagNewsTemplate
	{
		shared_str		str;
	} SNewsTemplate;

	// Array of news templates
	typedef std::map<u32, SNewsTemplate>	NewsTemplates;
	NewsTemplates							m_NewsTemplates;

	void LoadNewsTemplates();
};

DEFINE_VECTOR(GAME_NEWS_DATA, GAME_NEWS_VECTOR, GAME_NEWS_IT);