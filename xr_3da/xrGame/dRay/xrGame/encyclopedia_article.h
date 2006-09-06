///////////////////////////////////////////////////////////////
// encyclopedia_article.h
// ���������, �������� � ����������� ������ � ������������
///////////////////////////////////////////////////////////////

#pragma once

#include "encyclopedia_article_defs.h"
#include "xml_str_id_loader.h"
#include "shared_data.h"

#include "ui\uistatic.h"

//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: ������ ��� InfoProtion
//////////////////////////////////////////////////////////////////////////
struct SArticleData : CSharedResource
{
	shared_str name;
	shared_str group;
	
	//��������
	CUIStatic image;

	//����� ������
	xr_string	text;
	
	//������ ltx, ������ ������ ������
//	shared_str ltx;

	// ��� ������
	ARTICLE_DATA::EArticleType	articleType;
	shared_str					ui_template_name;
};

class CEncyclopediaArticle;

class CEncyclopediaArticle : public CSharedClass<SArticleData, ARTICLE_ID, false>
							,public CXML_IdToIndex<ARTICLE_ID,	int, CEncyclopediaArticle>
{
private:
	typedef CSharedClass<SArticleData, ARTICLE_ID, false>					inherited_shared;
	typedef CXML_IdToIndex<ARTICLE_ID, int, CEncyclopediaArticle>	id_to_index;

	friend id_to_index;
public:
	CEncyclopediaArticle			();
	virtual ~CEncyclopediaArticle	();

	virtual void Load	(ARTICLE_ID str_id);
//	virtual void Load	(ARTICLE_INDEX  index);


protected:
	ARTICLE_ID			m_ArticleId;
	virtual	void		load_shared					(LPCSTR);
	static  void		InitXmlIdToIndex			();
public:
	const ARTICLE_ID	Id							()						{return m_ArticleId;}
	SArticleData*		data						()						{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
};