///////////////////////////////////////////////////////////////
// encyclopedia_article_defs.h
///////////////////////////////////////////////////////////////

#pragma	once

#include "alife_space.h"

//статьи в энциклопедии
typedef int			ARTICLE_INDEX;
typedef ref_str		ARTICLE_STR_ID;
#define NO_ARTICLE ARTICLE_INDEX(-1)

struct ARTICLE_DATA
{
	ARTICLE_DATA ():index(NO_ARTICLE),receive_time(0),readed(false) {}
	ARTICLE_DATA (ARTICLE_INDEX idx, ALife::_TIME_ID time):index(idx),receive_time(time),readed(false) {}
	
	ALife::_TIME_ID receive_time;
	ARTICLE_INDEX index;
	//whether the article has been read
	bool readed;
};

DEFINE_VECTOR		(ARTICLE_INDEX, ARTICLE_INDEX_VECTOR, ARTICLE_INDEX_IT);
DEFINE_VECTOR		(ARTICLE_DATA, ARTICLE_VECTOR, ARTICLE_IT);