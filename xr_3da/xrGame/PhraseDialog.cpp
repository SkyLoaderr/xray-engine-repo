///////////////////////////////////////////////////////////////
// PhraseDialog.cpp
// ����� ������� - ������� ��� ������ ���� 2� ���������� � ����
///////////////////////////////////////////////////////////////

/*
	��������� XML-����� �������
	---------------------------
	<dialog id="unique_name_id">
		<character_list>
        	//������ ����������, �������, ����� ������������
			//� �������, �������� ���������� ������� ��� ��������
			//����������
			<name></name>
			<name></name>
			....
		</character_list>
		<precondition>
			- ��� ���������� ������� - 
			//��������, ������� ���������, ������� ������� 
			//����������, ���� ������ ��� ���� �����������
		</precondition>

		<phrase_list>
			<phrase></phrase>
			<phrase></phrase>
			....
		</phrase_list>
	</dialog>


	
	��������� ����� �������
	 -----------------------
	 <phrase id="���������� � �������� ������� ����� �����">
		<text>��������� ������������� �����</text>
		<precondition>���. ������� ��� �����, ���� ��� ����� ����� ���������</precondition>
		<action>��� ���������� �������, ������� ����������� ��� ������ �����</action>
		
		<phrase_next>
			//������ ���� ������� ����� ���������� ����� �������
			<id></id>
			<id></id>
			....
		</phrase_next>
	</phrase>
*/


#include "stdafx.h"
#include "phrasedialog.h"


CPhraseDialog::CPhraseDialog(void)
{
}
CPhraseDialog::~CPhraseDialog(void)
{
}


//������������� �������
//���� ������ � ����� id ������ �� �������������
//�� ����� �������� �� �����
void CPhraseDialog::Init(ref_str dialog_id)
{
}

void CPhraseDialog::Load(LPCSTR xml_file)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", xml_file);
	R_ASSERT2(xml_result, "xml file not found");

	//��������� ��� ����� ����������
	//int info_num = uiXml.GetNodesNum(uiXml.GetRoot(), "info_portion");
}



/*

static void LoadAllToMemory()
{
g_InfoPortionMap.clear();

CUIXml uiXml;
bool xml_result = uiXml.Init("$game_data$","info_portions.xml");
R_ASSERT2(xml_result, "xml file not found");


//��������� ��� ����� ����������
int info_num = uiXml.GetNodesNum(uiXml.GetRoot(), "info_portion");

CInfoPortion info_portion;
g_InfoPortionMap;


//�������� ������� ����������
for(int i=0; i<info_num; ++i)
{
info_portion.LoadInfoPortionFromXml(uiXml, i);

//�������� ���������� � �����
g_InfoPortionMap[info_portion.GetIndex()] = info_portion;
}
}




void CInfoPortion::LoadInfoPortionFromXml(CUIXml& uiXml, int num_in_file)
{
	XML_NODE* pNode = uiXml.NavigateToNode(uiXml.GetRoot(),
											"info_portion",
											   num_in_file);

	R_ASSERT2(pNode, "Info Portion with index does not exists");

	m_iIndex = uiXml.ReadAttribInt(pNode, "index", -1);
	
	R_ASSERT2(-1 != m_iIndex, "Wrong index");
										
	//�����
	m_text.SetText(uiXml.Read(pNode, "text", 0));


	//�������
	int i,j;
	int questions_num = uiXml.GetNodesNum(pNode, "question");
	SInfoQuestion question;

	m_QuestionsList.clear();
	
	for(i=0; i<questions_num; ++i)
	{
		XML_NODE* pQuestionNode = uiXml.NavigateToNode(pNode,"question",i);
		
		question.num_in_info = i;
		question.info_portion_index = m_iIndex;

		question.text.SetText(uiXml.Read(pQuestionNode, "text", 0));
		question.negative_answer_text.SetText(uiXml.Read(pQuestionNode,
													"negative_answer", 0));
	
		int info_num = uiXml.GetNodesNum(pQuestionNode, "to_info");
		question.IndexList.clear();

		for(j=0; j<info_num; ++j)
		{
			int val = uiXml.ReadInt(pQuestionNode,"to_info",j);
			question.IndexList.push_back(val);
		}
	
		m_QuestionsList.push_back(question);
	}

	
	//��������� ������� �� �����
	SMapLocation map_location;
	m_MapLocationVector.clear();
	int location_num = uiXml.GetNodesNum(pNode, "location");

	for(i=0; i<location_num; ++i)
	{
		XML_NODE* pMapNode = uiXml.NavigateToNode(pNode,"location",i);

		if(pMapNode)
		{
			map_location.level_num = uiXml.ReadInt(pMapNode,"level",0);
			map_location.x = (float)atof(uiXml.Read(pMapNode,"x",0));
			map_location.y = (float)atof(uiXml.Read(pMapNode,"y",0));

			map_location.name.SetText(uiXml.ReadAttrib(pMapNode, "icon", 0, "name"));
			map_location.icon_x = uiXml.ReadAttribInt(pMapNode, "icon", 0, "x");
			map_location.icon_y = uiXml.ReadAttribInt(pMapNode, "icon", 0, "y");
			map_location.icon_width = uiXml.ReadAttribInt(pMapNode, "icon", 0, "width");
			map_location.icon_height = uiXml.ReadAttribInt(pMapNode, "icon", 0, "height");

			map_location.text.SetText(uiXml.Read(pMapNode, "text", 0));

			//������������ � ������� �� ������, ���� ��� �����
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

			m_MapLocationVector.push_back(map_location);
		}
	}
}
*/