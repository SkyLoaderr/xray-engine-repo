///////////////////////////////////////////////////////////////
// InfoPortion.cpp
// струтура, предствал€юща€ сюжетную информацию
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InfoPortion.h"
#include "level.h"
#include "gameobject.h"
#include "encyclopedia_article.h"

//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: данные дл€ InfoProtion

SInfoPortionData::SInfoPortionData ()
{
	m_InfoIndex	= NO_INFO_INDEX;
	m_pGameTask = NULL;
}
SInfoPortionData::~SInfoPortionData ()
{
	xr_delete(m_pGameTask);
}


/////////////////////////////////////////
//	class CInfoPortion
//

CInfoPortion::CInfoPortion()
{
}

CInfoPortion::~CInfoPortion ()
{
}

void CInfoPortion::Load	(INFO_STR_ID info_str_id)
{
	Load	(id_to_index::IdToIndex(info_str_id));
}

void CInfoPortion::Load	(INFO_ID info_index)
{
	m_InfoIndex = info_index;
	inherited_shared::load_shared(m_InfoIndex, NULL);
}


void CInfoPortion::load_shared	(LPCSTR)
{
	const id_to_index::ITEM_DATA& item_data = id_to_index::GetByIndex(m_InfoIndex);

	CUIXml uiXml;
	string128 xml_file_full;
	strconcat(xml_file_full, *ref_str(item_data.file_name), ".xml");

	bool xml_result = uiXml.Init("$game_data$", xml_file_full);
	R_ASSERT3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* pNode = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	R_ASSERT3(pNode, "info_portion id=", *ref_str(item_data.id));

	//текст
	info_data()->m_text = uiXml.Read(pNode, "text", 0);

	//список названий диалогов
	int dialogs_num = uiXml.GetNodesNum(pNode, "dialog");
	info_data()->m_DialogNames.clear();
	for(int i=0; i<dialogs_num; ++i)
	{
		ref_str dialog_name = uiXml.Read(pNode, "dialog", i);
		info_data()->m_DialogNames.push_back(dialog_name);
	}


	//список названий порций информации, которые деактивируютс€,
	//после получени€ этой порции
	int disable_num = uiXml.GetNodesNum(pNode, "disable");
	info_data()->m_DisableInfo.clear();
	for(i=0; i<disable_num; ++i)
	{
		INFO_ID info_id = CInfoPortion::IdToIndex(uiXml.Read(pNode, "disable", NULL));
		info_data()->m_DisableInfo.push_back(info_id);
	}

	//имена скриптовых функций
	info_data()->m_PhraseScript.Load(uiXml, pNode);


	XML_NODE* pTaskNode = uiXml.NavigateToNode(pNode,"task",0);
	if(pTaskNode) 
	{
		info_data()->m_pGameTask = xr_new<CGameTask>();
		info_data()->m_pGameTask->Load(uiXml, pTaskNode);
	}
	//индексы статей
	info_data()->m_Articles.clear();
	int articles_num = uiXml.GetNodesNum(pNode, "article");
	for(i=0; i<articles_num; ++i)
	{
		LPCSTR article_str_id = uiXml.Read(pNode, "article", i, NULL);
		R_ASSERT(article_str_id);
		info_data()->m_Articles.push_back(CEncyclopediaArticle::IdToIndex(article_str_id));
	}
	
	//загрузить позиции на карте
	SMapLocation map_location;
	info_data()->m_MapLocations.clear();
	int location_num = uiXml.GetNodesNum(pNode, "location");

	for(i=0; i<location_num; ++i)
	{
		XML_NODE* pMapNode = uiXml.NavigateToNode(pNode,"location",i);

		if(pMapNode)
		{
			map_location.info_portion_id = m_InfoIndex;

			map_location.level_name = uiXml.Read(pMapNode,"level",0);
			map_location.x = (float)atof(uiXml.Read(pMapNode,"x",0));
			map_location.y = (float)atof(uiXml.Read(pMapNode,"y",0));

			map_location.name = uiXml.ReadAttrib(pMapNode, "icon", 0, "name");
			map_location.icon_x = uiXml.ReadAttribInt(pMapNode, "icon", 0, "x");
			map_location.icon_y = uiXml.ReadAttribInt(pMapNode, "icon", 0, "y");
			map_location.icon_width = uiXml.ReadAttribInt(pMapNode, "icon", 0, "width");
			map_location.icon_height = uiXml.ReadAttribInt(pMapNode, "icon", 0, "height");

			map_location.text = uiXml.Read(pMapNode, "text", 0);

			//присоединить к объекту на уровне, если тот задан
			if(uiXml.NavigateToNode(pMapNode,"object",0))
			{
				ref_str object_name = uiXml.Read(pMapNode, "object", 0, "name");
				CGameObject *pGameObject = NULL;
				if(*object_name)
					pGameObject = dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(*object_name));

				if (pGameObject)
				{
					map_location.attached_to_object = true;
					map_location.object_id = (u16)pGameObject->ID();
				}
				else
				{
					map_location.attached_to_object = false;
					map_location.object_id = 0xffff;
				}
			}
			info_data()->m_MapLocations.push_back(map_location);
		}
	}
}

LPCSTR  CInfoPortion::GetText () const
{
	return *info_data()->m_text;
}

void   CInfoPortion::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "info_portion";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("info_portions", "files");
}