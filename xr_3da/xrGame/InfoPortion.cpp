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

//статический вектор, дл€ хранени€ строк идентификаторов в по пор€дку
//дл€ получени€ чилсового ID
CInfoPortion::STR_ID_VECTOR* CInfoPortion::m_pStrToID = NULL;

CInfoPortion::STR_ID_VECTOR& CInfoPortion::StrIdVector ()
{
	if(!m_pStrToID)
	{
		m_pStrToID = xr_new<STR_ID_VECTOR >();

		CUIXml uiXml;
		bool xml_result = uiXml.Init("$game_data$", INFO_PORTION_XML);
		R_ASSERT2(xml_result, "xml file not found");

		//общий список info_portions
		int info_num = uiXml.GetNodesNum(uiXml.GetRoot(), "info_portion");
		for(int i=0; i<info_num; ++i)
		{
			LPCSTR info_name = uiXml.ReadAttrib(uiXml.GetRoot(), "info_portion", i, "id", NULL);

			string128 buf;
			sprintf(buf, "%d", i);
			R_ASSERT3(info_name, "id for info portion don't set. Info num:", buf);

			//проверетить ID на уникальность
			STR_ID_VECTOR_IT it = std::find(m_pStrToID->begin(), m_pStrToID->end(), info_name);
			R_ASSERT3(m_pStrToID->end() == it, "duplicate info id", info_name);
			
			m_pStrToID->push_back(info_name);
		}
	}

	return *m_pStrToID;
}

INFO_ID CInfoPortion::StrToID(const INFO_STR_ID& str_id)
{
	STR_ID_VECTOR_IT it = std::find(StrIdVector().begin(), StrIdVector().end(), str_id);
	R_ASSERT3(StrIdVector().end() != it, "info not found", *str_id);
	return (INFO_ID)(it-StrIdVector().begin());
}


INFO_STR_ID CInfoPortion::IDToStr(INFO_ID info_id)
{
	return StrIdVector()[info_id];
}

void CInfoPortion::DeleteStrToID	()
{
	xr_delete(m_pStrToID);
}


CInfoPortion::CInfoPortion()
{
}

CInfoPortion::~CInfoPortion ()
{
}

void CInfoPortion::Load	(INFO_STR_ID info_str_id)
{
	Load	(StrToID(info_str_id));
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
	XML_NODE* pNode = uiXml.NavigateToNodeWithAttribute("info_portion", "id", *IDToStr(m_InfoId));
	R_ASSERT3(pNode, "info_portion id=", *IDToStr(m_InfoId));

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
		INFO_ID info_id = (INFO_ID)uiXml.ReadInt(pNode, "disable", i);
		info_data()->m_DisableInfo.push_back(info_id);
	}

	//имена скриптовых функций
	info_data()->m_PhraseScript.Load(uiXml, pNode);

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