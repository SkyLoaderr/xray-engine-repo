////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries_inline.h
//	Created 	: 05.01.2002
//  Modified 	: 04.05.2004
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeNewsRegistry
//////////////////////////////////////////////////////////////////////////

IC	void CSE_ALifeNewsRegistry::add						(const ALife::SGameNews &news)
{
	ALife::SGameNews				*_news = xr_new<ALife::SGameNews>(news);
	_news->m_news_id				= m_last_id++;
	m_news.insert					(std::make_pair(m_last_id,_news));
}

IC	void CSE_ALifeNewsRegistry::remove					(const ALife::_NEWS_ID &news_id)
{
	NEWS_REGISTRY::iterator			I = m_news.find(news_id);
	VERIFY							(news().end() != I);
	m_news.erase					(I);
}

IC	const CSE_ALifeNewsRegistry::NEWS_REGISTRY &CSE_ALifeNewsRegistry::news	() const
{
	return							(m_news);
}

IC	const ALife::SGameNews *CSE_ALifeNewsRegistry::news		(const ALife::_NEWS_ID &news_id) const
{
	NEWS_REGISTRY::const_iterator	I = news().find(news_id);
	if (news().end() == I)
		return						(0);
	return							((*I).second);
}
