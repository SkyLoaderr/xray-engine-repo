///////////////////////////////////////////////////////////////
// encyclopedia_article.h
// структура, хранящая и загружающая статьи в энциклопедию
///////////////////////////////////////////////////////////////

#pragma once

#include "encyclopedia_article_defs.h"
#include "xml_str_id_loader.h"
#include "shared_data.h"

#include "ui\uistatic.h"

//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: данные для InfoProtion
//////////////////////////////////////////////////////////////////////////
struct SArticleData : CSharedResource
{
	shared_str name;
	shared_str group;
	
	//картинка
	CUIStatic image;

	//текст статьи
	shared_str	text;
	
	//секция ltx, откуда читать данные
	shared_str ltx;

	// Тип статьи
	ARTICLE_DATA::EArticleType articleType;
};

class CEncyclopediaArticle;

class CEncyclopediaArticle : public CSharedClass<SArticleData, ARTICLE_INDEX>,
							 public CXML_IdToIndex<ARTICLE_STR_ID,	ARTICLE_INDEX, CEncyclopediaArticle>
{
private:
	typedef CSharedClass<SArticleData, ARTICLE_INDEX> inherited_shared;
	typedef CXML_IdToIndex<ARTICLE_STR_ID, ARTICLE_INDEX, CEncyclopediaArticle>	id_to_index;

	friend id_to_index;
public:
	CEncyclopediaArticle			();
	virtual ~CEncyclopediaArticle	();

	virtual void Load	(ARTICLE_STR_ID str_id);
	virtual void Load	(ARTICLE_INDEX  index);


protected:
	ARTICLE_INDEX	m_ArticleIndex;
	void load_shared	(LPCSTR);
	static void InitXmlIdToIndex();
public:
	const ARTICLE_INDEX Index() {return m_ArticleIndex;}
	SArticleData* data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
};