///////////////////////////////////////////////////////////////
// GameTask.h
// Класс игрового задания задания
///////////////////////////////////////////////////////////////

#pragma once

#include "ui/xrXMLParser.h"


class  CGameObject;

//состояние цели и задания
enum ETaskState {
	eTaskStateFail			= 0,
	eTaskStateInProgress	= 1,
	eTaskStateCompleted		= 2,
	eTaskStateMax			= u32(-1)
};


//подцель задания 
struct SGameTaskObjective 
{
	SGameTaskObjective() {state = eTaskStateMax;}

	//текстовое описание
	ref_str description;
	//скриптовая функция, которая возвращает ETaskState
	ref_str script_state;
	//то, что вернул скрипт, после просчета задания
	ETaskState state;
};


class CGameTask
{
public:
			CGameTask	();
	virtual ~CGameTask	();

	//загрузка из XML файла
	virtual void		Load			(CUIXml& ui_xml, XML_NODE* task_node);
	
	virtual u32			ObjectivesNum	()				{return	m_Objectives.size();}
	virtual ref_str		ObjectiveDesc	(u32 index)		{return m_Objectives[index].description;}
	virtual ETaskState	ObjectiveState  (u32 index)		{return m_Objectives[index].state;}

	virtual void 		CalcState		(CGameObject* task_accomplisher);

protected:
	virtual ETaskState	CalcObjectiveState  (u32 index,	 CGameObject* task_accomplisher);

	DEFINE_VECTOR(SGameTaskObjective, OBJECTIVE_VECTOR, OBJECTIVE_VECTOR_IT);
	OBJECTIVE_VECTOR m_Objectives;
};