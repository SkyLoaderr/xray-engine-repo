///////////////////////////////////////////////////////////////
// game_news.cpp
// реестр новостей: новости симул€ции + сюжетные
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_news.h"
#include "object_broker.h"


GAME_NEWS_DATA::GAME_NEWS_DATA():news_id(ALife::_NEWS_ID(-1)), news_text(NO_STRING) 
{
	texture_name = NULL;
	x1 = y1 = x2 = y2 = 0;
}

void GAME_NEWS_DATA::load (IReader& stream)
{
	load_data(news_id, stream);
	load_data(news_text, stream);
	load_data(receive_time, stream);

	texture_name = NULL;
	x1 = y1 = x2 = y2 = 0;
}

void GAME_NEWS_DATA::save (IWriter& stream)
{
	save_data(news_id, stream);
	save_data(news_text, stream);
	save_data(receive_time, stream);
}
