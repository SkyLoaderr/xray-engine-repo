////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_news.h
//	Created 	: 05.01.2003
//  Modified 	: 17.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife news class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_interfaces.h"

class CALifeNews : public IPureALifeLSObject {
public:
	ALife::_NEWS_ID			m_news_id;
	ALife::_TIME_ID			m_game_time;
	ALife::_GRAPH_ID		m_game_vertex_id;
	ALife::ENewsType		m_news_type;
	ALife::_OBJECT_ID		m_object_id[2];
	ALife::_CLASS_ID		m_class_id;

	virtual					~CALifeNews	();
	virtual void			load		(IReader &tFileStream);
	virtual void			save		(IWriter &tMemoryStream);
};

#include "alife_news_inline.h"