///////////////////////////////////////////////////////////////
// InfoPortion.cpp
// струтура, предствал€юща€ сюжетную информацию
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InfoPortion.h"
#include "level.h"
#include "gameobject.h"


#define  INFO_PORTION_XML			"info_portions.xml"


//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: данные дл€ InfoProtion

SInfoPortionData::SInfoPortionData ()
{
	m_InfoId = NO_INFO_INDEX;
}
SInfoPortionData::~SInfoPortionData ()
{
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



void CInfoPortion::Load	(INFO_ID info_id)
{
	m_InfoId = info_id;
	inherited_shared::load_shared(m_InfoId, INFO_PORTION_XML);
}

void CInfoPortion::load_shared	(LPCSTR xml_file)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", xml_file);
	R_ASSERT2(xml_result, "xml file not found");

	//loading from XML
	string128 buf_str;
	sprintf(buf_str, "%d", m_InfoId);
	XML_NODE* pNode = uiXml.NavigateToNodeWithAttribute("info_portion", "index", buf_str);
	R_ASSERT3(pNode, "info_portion index=", buf_str);

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

	//имена скриптовых функций
	int script_actions_num = uiXml.GetNodesNum(pNode, "action");
	info_data()->m_ScriptActions.clear();
	for(int i=0; i<script_actions_num; i++)
	{
		ref_str action_name = uiXml.Read(pNode, "action", i, NULL);
		info_data()->m_ScriptActions.push_back(action_name);
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
			map_location.info_portion_id = m_InfoId;

			map_location.level_num = uiXml.ReadInt(pMapNode,"level",0);
			map_location.x = (float)atof(uiXml.Read(pMapNode,"x",0));
			map_location.y = (float)atof(uiXml.Read(pMapNode,"y",0));

			map_location.name.SetText(uiXml.ReadAttrib(pMapNode, "icon", 0, "name"));
			map_location.icon_x = uiXml.ReadAttribInt(pMapNode, "icon", 0, "x");
			map_location.icon_y = uiXml.ReadAttribInt(pMapNode, "icon", 0, "y");
			map_location.icon_width = uiXml.ReadAttribInt(pMapNode, "icon", 0, "width");
			map_location.icon_height = uiXml.ReadAttribInt(pMapNode, "icon", 0, "height");

			map_location.text.SetText(uiXml.Read(pMapNode, "text", 0));

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

LPCSTR  CInfoPortion::GetText ()
{
	return *info_data()->m_text;
}