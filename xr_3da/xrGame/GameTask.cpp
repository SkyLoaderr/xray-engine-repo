///////////////////////////////////////////////////////////////
// GameTask.cpp
// ����� �������� ������� �������
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameTask.h"
#include "gameobject.h"
#include "script_engine.h"
#include "ai_space.h"
#include "script_game_object.h"
#include "script_space.h"

#include "ui/xrXMLParser.h"

TASK_DATA::TASK_DATA():index(NO_TASK),receive_time(0),finish_time(0)
{
}

TASK_DATA::TASK_DATA(TASK_INDEX idx, ALife::_TIME_ID time):index(idx),receive_time(time),finish_time(0)
{
	if(index == NO_TASK) return;
	
	CGameTask task;
	task.Load(index);
	states.resize(task.ObjectivesNum());
	for(std::size_t i=0; i<states.size();i++)
		states[i] = eTaskStateInProgress;
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


SGameTaskData::SGameTaskData ()
{
}
SGameTaskData::~SGameTaskData ()
{}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CGameTask::CGameTask		()
{
}

CGameTask::~CGameTask		()
{
}

void CGameTask::Load	(TASK_ID str_id)
{
	Load	(id_to_index::IdToIndex(str_id));
}

void CGameTask::Load	(TASK_INDEX  index)
{
	m_TaskIndex = index;
	inherited_shared::load_shared(m_TaskIndex, NULL);
}


void CGameTask::load_shared	(LPCSTR)
{
	const id_to_index::ITEM_DATA& item_data = id_to_index::GetByIndex(m_TaskIndex);

	CUIXml uiXml;
	string128 xml_file_full;
	strconcat(xml_file_full, *ref_str(item_data.file_name), ".xml");

	bool xml_result = uiXml.Init("$game_data$", xml_file_full);
	R_ASSERT3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* task_node = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	R_ASSERT3(task_node, "game task id=", *ref_str(item_data.id));

	int tag_num = uiXml.GetNodesNum(task_node, "objective");

	data()->m_Objectives.clear		();
	data()->m_Objectives.reserve	(tag_num);

	SGameTaskObjective objective;
	for(int i=0; i<tag_num; i++)
	{
		LPCSTR tag_text = uiXml.Read(task_node, "objective:text", i, NULL);
		objective.description = tag_text;
		data()->m_Objectives.push_back(objective);
	}
}

void CGameTask::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "game_task";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("game_tasks", "files");
}

u32			CGameTask::ObjectivesNum	()
{
	return	data()->m_Objectives.size();
}
ref_str		CGameTask::ObjectiveDesc	(u32 index)
{
	return data()->m_Objectives[index].description;
}
ETaskState	CGameTask::ObjectiveState  (u32 index)
{
	VERIFY(index<m_ObjectiveStates.size());
	return m_ObjectiveStates[index];
}