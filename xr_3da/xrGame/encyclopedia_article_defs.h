///////////////////////////////////////////////////////////////
// encyclopedia_article_defs.h
///////////////////////////////////////////////////////////////

#pragma	once

#include "alife_space.h"

//статьи в энциклопедии
//typedef int			ARTICLE_INDEX;
typedef shared_str	ARTICLE_ID;
//#define NO_ARTICLE	ARTICLE_INDEX(-1)

struct ARTICLE_DATA
{
	enum EArticleType {eEncyclopediaArticle, eDiaryArticle, eDummyArticle};

	ARTICLE_DATA			()
		:	article_id		(NULL),
			receive_time	(0),
			readed			(false),
			article_type	(EArticleType::eDummyArticle)
	{}

	ARTICLE_DATA			(ARTICLE_ID id, ALife::_TIME_ID time, EArticleType articleType)
		:	article_id		(id),
			receive_time	(time),
			readed			(false),
			article_type	(articleType)
	{}
	
	ALife::_TIME_ID receive_time;
	ARTICLE_ID article_id;
	//whether the article has been read
	bool readed;

	EArticleType article_type;
};

DEFINE_VECTOR		(ARTICLE_ID, ARTICLE_ID_VECTOR, ARTICLE_ID_IT);
DEFINE_VECTOR		(ARTICLE_DATA, ARTICLE_VECTOR, ARTICLE_IT);