///////////////////////////////////////////////////////////////
// InfoPortion.cpp
// струтура, предствал€юща€ сюжетную информацию
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InfoPortion.h"

#include "ui\\xrXMLParser.h"



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
	uiXml.Init("$game_data$","info_portions.xml");

	//загрузить все части информации
	int info_num = uiXml.GetNodesNum(uiXml.GetRoot(), "info_portion");

	CInfoPortion info_portion;
	g_InfoPortionMap;


	//загрузка кусочка информации
	for(int i=0; i<info_num; i++)
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
	m_bLocationSet = false;
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
	
	R_ASSERT2(m_iIndex != -1, "Wrong index");
										
	//текст
	m_text.SetText(uiXml.Read(pNode, "text", 0));

	//локаци€ на карте



	//вопросы
	int i,j;
	int questions_num = uiXml.GetNodesNum(pNode, "question");
	SInfoQuestion question;

	m_QuestionsList.clear();
	
	for(i=0; i<questions_num; i++)
	{
		XML_NODE* pQuestionNode = uiXml.NavigateToNode(pNode,"question",i);
		
		question.num_in_info = i;
		question.info_portion_index = m_iIndex;

		question.text.SetText(uiXml.Read(pQuestionNode, "text", 0));
		question.negative_answer_text.SetText(uiXml.Read(pQuestionNode,
													"negative_answer", 0));
	
		int info_num = uiXml.GetNodesNum(pQuestionNode, "to_info");
		question.IndexList.clear();

		for(j=0; j<info_num; j++)
		{
			int val = uiXml.ReadInt(pQuestionNode,"to_info",j);
			question.IndexList.push_back(val);
		}
	
		m_QuestionsList.push_back(question);
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
	
	for(i=0; i<questions_num; i++)
	{
		XML_NODE* pQuestionNode = uiXml.NavigateToNode(pNode,"question",i);
		
		question.num_in_info = i;
		question.info_portion_index = m_iIndex;

		question.text.SetText(uiXml.Read(pQuestionNode, "text", 0));
		question.negative_answer_text.SetText(uiXml.Read(pQuestionNode, 
													"negative_answer", 0));
	
		int info_num = uiXml.GetNodesNum(pQuestionNode, "to_info");
	
		for(j=0; j<info_num; j++)
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