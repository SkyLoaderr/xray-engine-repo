///////////////////////////////////////////////////////////////
// InfoPortion.cpp
// ��������, �������������� �������� ����������
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InfoPortion.h"
#include "gameobject.h"
#include "encyclopedia_article.h"
#include "gametask.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_story_registry.h"
#include "xrServer_Objects_ALife.h"
#include "script_engine.h"
#include "ui\uixmlinit.h"
#include "object_broker.h"
//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: ������ ��� InfoProtion

void INFO_DATA::load (IReader& stream) 
{
	load_data(info_id, stream); 
	load_data(receive_time, stream);
}

void INFO_DATA::save (IWriter& stream) 
{
	save_data(info_id, stream); 
	save_data(receive_time, stream);
}


SInfoPortionData::SInfoPortionData ()
{
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
/*
void CInfoPortion::Load	(INFO_STR_ID info_str_id)
{
	Load	(id_to_index::IdToIndex(info_str_id));
}
*/
void CInfoPortion::Load	(INFO_ID info_id)
{
	m_InfoId = info_id;
	inherited_shared::load_shared(m_InfoId, NULL);
}


void CInfoPortion::load_shared	(LPCSTR)
{
//	const id_to_index::ITEM_DATA& item_data = *id_to_index::GetByIndex(m_InfoIndex);
	g_pStringContainer->verify();
	const id_to_index::ITEM_DATA& item_data = *id_to_index::GetById(m_InfoId);

	CUIXml uiXml;
	string_path xml_file_full;
	strconcat(xml_file_full, *item_data.file_name, ".xml");

	bool xml_result = uiXml.Init(CONFIG_PATH, GAME_PATH, xml_file_full);
	THROW3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* pNode = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	THROW3(pNode, "info_portion id=", *item_data.id);


	int defer_init = uiXml.ReadAttribInt(pNode, "defer_init", 0);
	if (1 == defer_init) 
		info_data()->m_bDeferInit = true;
	else
		info_data()->m_bDeferInit = false;


	//�����
	info_data()->m_text = uiXml.Read(pNode, "text", 0);

	//������ �������� ��������
	int dialogs_num = uiXml.GetNodesNum(pNode, "dialog");
	info_data()->m_DialogNames.clear();
	for(int i=0; i<dialogs_num; ++i)
	{
		shared_str dialog_name = uiXml.Read(pNode, "dialog", i);
		info_data()->m_DialogNames.push_back(dialog_name);
	}

	dialogs_num = uiXml.GetNodesNum(pNode, "actor_dialog");
	info_data()->m_ActorDialogNames.clear();
	for(int i=0; i<dialogs_num; ++i)
	{
		shared_str dialog_name = uiXml.Read(pNode, "actor_dialog", i);
		info_data()->m_ActorDialogNames.push_back(dialog_name);
	}
	
	//������ �������� ������ ����������, ������� ��������������,
	//����� ��������� ���� ������
	int disable_num = uiXml.GetNodesNum(pNode, "disable");
	info_data()->m_DisableInfo.clear();
	for(i=0; i<disable_num; ++i)
	{
		INFO_ID info_id = uiXml.Read(pNode, "disable", NULL);
		info_data()->m_DisableInfo.push_back(info_id);
	}

	//����� ���������� �������
	info_data()->m_PhraseScript.Load(uiXml, pNode);


	//������� ������
	info_data()->m_Articles.clear();
	int articles_num = uiXml.GetNodesNum(pNode, "article");
	for(i=0; i<articles_num; ++i)
	{
		LPCSTR article_str_id = uiXml.Read(pNode, "article", i, NULL);
		THROW(article_str_id);
		info_data()->m_Articles.push_back(article_str_id);
	}
	//������� ������, ������� �������� �� �������
	info_data()->m_ArticlesDisable.clear();
	articles_num = uiXml.GetNodesNum(pNode, "article_disable");
	for(i=0; i<articles_num; ++i)
	{
		LPCSTR article_str_id = uiXml.Read(pNode, "article_disable", i, NULL);
		THROW(article_str_id);
		info_data()->m_ArticlesDisable.push_back(article_str_id);
	}
	
	//������� ������, ������� �������� �� �������
	info_data()->m_GameTasks.clear();
	int task_num = uiXml.GetNodesNum(pNode, "task");
	for(i=0; i<task_num; ++i)
	{
		LPCSTR task_str_id = uiXml.Read(pNode, "task", i, NULL);
		THROW(task_str_id);
		info_data()->m_GameTasks.push_back(task_str_id);
	}
}

LPCSTR  CInfoPortion::GetText () const
{
	return *info_data()->m_text;
}

bool CInfoPortion::DeferInit () const
{
	return info_data()->m_bDeferInit;
}

void   CInfoPortion::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "info_portion";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("info_portions", "files");
}