///////////////////////////////////////////////////////////////
// encyclopedia_article.cpp
// структура, хранящая и загружающая статьи в энциклопедию
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "encyclopedia_article.h"
#include "ui/xrXMLParser.h"
#include "ui/UIXmlInit.h"
#include "ui/UIInventoryUtilities.h"

using namespace InventoryUtilities;

CEncyclopediaArticle::CEncyclopediaArticle()
{
}

CEncyclopediaArticle::~CEncyclopediaArticle()
{
}

void CEncyclopediaArticle::Load	(ARTICLE_STR_ID str_id)
{
	Load	(id_to_index::IdToIndex(str_id));
}

void CEncyclopediaArticle::Load	(ARTICLE_INDEX  index)
{
	m_ArticleIndex = index;
	inherited_shared::load_shared(m_ArticleIndex, NULL);
}


void CEncyclopediaArticle::load_shared	(LPCSTR)
{
	const id_to_index::ITEM_DATA& item_data = *id_to_index::GetByIndex(m_ArticleIndex);

	CUIXml		uiXml;
	CUIXmlInit	xml_init;
	string_path xml_file_full;
	strconcat	(xml_file_full, *shared_str(item_data.file_name), ".xml");

	bool xml_result = uiXml.Init("$game_data$", xml_file_full);
	R_ASSERT3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* pNode = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	R_ASSERT3(pNode, "encyclopedia article id=", *shared_str(item_data.id));

	//текст
	data()->text = uiXml.Read(pNode, "text", 0);
	//имя
	data()->name = uiXml.ReadAttrib(pNode, "name", "");
	//группа
	data()->group = uiXml.ReadAttrib(pNode, "group", "");
	//секция ltx, откуда читать данные
	data()->ltx = uiXml.Read(pNode, "ltx", 0, NULL);

	if(*data()->ltx)
	{
		data()->image.SetShader(InventoryUtilities::GetEquipmentIconsShader());

		u32 x		= pSettings->r_u32(data()->ltx, "inv_grid_x") * INV_GRID_WIDTH;
		u32 y		= pSettings->r_u32(data()->ltx, "inv_grid_y") * INV_GRID_HEIGHT;
		u32 width	= pSettings->r_u32(data()->ltx, "inv_grid_width") * INV_GRID_WIDTH;
		u32 height	= pSettings->r_u32(data()->ltx, "inv_grid_height") * INV_GRID_HEIGHT;

		data()->image.GetUIStaticItem().SetOriginalRect(x, y, width, height);
		data()->image.ClipperOn();
		data()->image.TextureAvailable(true);
	}
	else 
	{
		uiXml.SetLocalRoot(pNode);
		xml_init.InitTexture(uiXml, "", 0, &data()->image);
		uiXml.SetLocalRoot(uiXml.GetRoot());
	}

	Irect r = data()->image.GetUIStaticItem().GetOriginalRect();

	const int minSize = 65;

	// Сначала устанавливаем если надо минимально допустимые размеры иконки
	if (r.width() < minSize)
	{
		int dx = minSize - r.width();
		r.x2 += dx;
		data()->image.SetTextureOffset(dx / 2, data()->image.GetTextureOffeset()[1]);
	}

	if (r.height() < minSize)
	{
		int dy = minSize - r.height();
		r.y2 += dy;
		data()->image.SetTextureOffset(data()->image.GetTextureOffeset()[0], dy / 2);
	}

	data()->image.SetWndRect(0, 0, r.width(), r.height());

	// Тип статьи
	if (uiXml.ReadAttribInt(pNode, "article_type", 0))
	{
		data()->articleType = ARTICLE_DATA::eDiaryArticle;
	}
	else
	{
		data()->articleType = ARTICLE_DATA::eEncyclopediaArticle;
	}
}

void CEncyclopediaArticle::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "article";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("encyclopedia", "files");
}
