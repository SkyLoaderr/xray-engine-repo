///////////////////////////////////////////////////////////////
// GameTask.h
// ����� �������� ������� �������
///////////////////////////////////////////////////////////////

#pragma once

#include "ui/xrXMLParser.h"


class  CGameObject;

//��������� ���� � �������
enum ETaskState {
	eTaskStateFail			= 0,
	eTaskStateInProgress	= 1,
	eTaskStateCompleted		= 2,
	eTaskStateMax			= u32(-1)
};


//������� ������� 
struct SGameTaskObjective 
{
	SGameTaskObjective() {state = eTaskStateMax;}

	//��������� ��������
	ref_str description;
	//���������� �������, ������� ���������� ETaskState
	ref_str script_state;
	//��, ��� ������ ������, ����� �������� �������
	ETaskState state;
};


class CGameTask
{
public:
			CGameTask	();
	virtual ~CGameTask	();

	//�������� �� XML �����
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