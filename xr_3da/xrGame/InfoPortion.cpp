///////////////////////////////////////////////////////////////
// InfoPortion.cpp
// струтура, предствал€юща€ сюжетную информацию
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InfoPortion.h"

///////////////////////////////////////////////////
//  глобальное хранилище дл€ всех порций информации
//


static bool g_bInfoPortionsInit = false;
//массив со всеми информационными сообщени€ми
DEFINE_MAP(int, CInfoPortion, INFO_PORTION_MAP, INFO_PORTION_PAIR_IT);
INFO_PORTION_MAP g_InfoPortionMap;





static void LoadAllToMemory()
{
	g_InfoPortionMap.clear();

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$","info_portions.xml");
	R_ASSERT2(xml_result, "xml file not found");


	//загрузить все части информации
	int info_num = uiXml.GetNodesNum(uiXml.GetRoot(), "info_portion");

	CInfoPortion info_portion;
	g_InfoPortionMap;


	//загрузка кусочка информации
	for(int i=0; i<info_num; ++i)
	{
		info_portion.LoadInfoPortionFromXml(uiXml, i);

		//добавить информацию в карту
		g_InfoPortionMap[info_portion.GetIndex()] = info_portion;
	}
}




/////////////////////////////////////////
//	class CInfoPortion
//

CInfoPortion::CInfoPortion()
{
	m_bLoaded = false;

	m_QuestionsList.clear();
}

CInfoPortion::~CInfoPortion ()
{
}


void CInfoPortion::LoadInfoPortionFromXml(CUIXml& uiXml, int num_in_file)
{
	XML_NODE* pNode = uiXml.NavigateToNode(uiXml.GetRoot(),
											"info_portion",
											   num_in_file);

	R_ASSERT2(pNode, "Info Portion with index does not exists");

	m_iIndex = uiXml.ReadAttribInt(pNode, "index", -1);
	
	R_ASSERT2(-1 != m_iIndex, "Wrong index");
										
	//текст
	m_text.SetText(uiXml.Read(pNode, "text", 0));


	//вопросы
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

	
	//загрузить позиции на карте
	SMapLocation map_location;
	m_MapLocationVector.clear();
	int location_num = uiXml.GetNodesNum(pNode, "location");

	for(i=0; i<location_num; ++i)
	{
		XML_NODE* pMapNode = uiXml.NavigateToNode(pNode,"location",i);

		if(pMapNode)
		{
			map_location.info_portion_index = m_iIndex;

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

			m_MapLocationVector.push_back(map_location);
		}
	}
}


//загрузка структуры информацией из пам€ти
void CInfoPortion::Load(int index)
{
	//загрузить всю информацию в пам€ть
	if(!g_bInfoPortionsInit) 
	{
		LoadAllToMemory();
		g_bInfoPortionsInit = true;
	}
	
	*this = g_InfoPortionMap[index];

	m_bLoaded = true;
}
/*
void CInfoPortion::Load(int index)
{
	m_iIndex = index;


	if(!g_bInfoPortionsInit)
	{
		g_InfoPortionsXml.Init("$game_data$","info_portions.xml");
		g_bInfoPortionsInit = true;
	}

	CUIXml& uiXml = g_InfoPortionsXml;
//	uiXml.Init("$game_data$","info_portions.xml");

	char index_str[255];
	sprintf(index_str, "%d", index);
	XML_NODE* pNode = uiXml.SearchForAttribute(uiXml.GetRoot(),
											   "info_portion",
											   "index",index_str);

	R_ASSERT2(pNode, "Info Portion with index does not exists");
										
	//текст
	m_text.SetText(uiXml.Read(pNode, "text", 0));

	//локаци€ на карте



	//вопросы
	int i,j;
	int questions_num = uiXml.GetNodesNum(pNode, "question");
	SInfoQuestion question;
	
	for(i=0; i<questions_num; ++i)
	{
		XML_NODE* pQuestionNode = uiXml.NavigateToNode(pNode,"question",i);
		
		question.num_in_info = i;
		question.info_portion_index = m_iIndex;

		question.text.SetText(uiXml.Read(pQuestionNode, "text", 0));
		question.negative_answer_text.SetText(uiXml.Read(pQuestionNode, 
													"negative_answer", 0));
	
		int info_num = uiXml.GetNodesNum(pQuestionNode, "to_info");
	
		for(j=0; j<info_num; ++j)
		{
			int val = uiXml.ReadInt(pQuestionNode,"to_info",j);
			question.IndexList.push_back(val);
		}
	
		m_QuestionsList.push_back(question);
	}

	m_bLoaded = true;
}*/


void  CInfoPortion::GetQuestion(SInfoQuestion& question, int question_num)
{
	R_ASSERT(m_bLoaded);

//	question = m_QuestionsList[question_num];

}