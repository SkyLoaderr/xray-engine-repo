///////////////////////////////////////////////////////////////
// PhraseDialog.cpp
// Класс диалога - общения при помощи фраз 2х персонажей в игре
///////////////////////////////////////////////////////////////

/*
	структура XML-файла диалога
	---------------------------
	<dialog id="unique_name_id">
		<character_list>
        	//список персонажей, которые, могут учавствовать
			//в диалоге, задается отдельными именами или классами
			//персонажей
			<name></name>
			<name></name>
			....
		</character_list>
		<precondition>
			- ИМЯ скриптовой функции - 
			//предикат, который описывает, условия которые 
			//необходимы, чтоб диалог мог быть активирован
		</precondition>

		<phrase_list>
			<phrase></phrase>
			<phrase></phrase>
			....
		</phrase_list>
	</dialog>


	
	структура фразы диалога
	 -----------------------
	 <phrase id="уникальный в пределах диалога номер фразы">
		<text>текстовое представление фразы</text>
		<precondition>доп. условие для фразы, чтоб она могла стать доступной</precondition>
		<action>имя скриптовой функции, которая выполниться при вызове фразы</action>
		
		<phrase_next>
			//список фраз которые стают доступными после текущей
			<id></id>
			<id></id>
			....
		</phrase_next>
	</phrase>
*/


#include "stdafx.h"
#include "phrasedialog.h"
#include "phrasedialogmanager.h"

#define  DIALOGS_XML			"dialogs.xml"

#define  NO_PHRASE				-1
#define  START_PHRASE			0
#define  START_PHRASE_STR		"0"

//////////////////////////////////////////////////////////////////////////

SPhraseDialogData::SPhraseDialogData ()
{
	m_PhraseGraph.clear();
	m_sDialogID = NULL;
}

SPhraseDialogData::~SPhraseDialogData ()
{
	int a=0;
	a++;
}

//////////////////////////////////////////////////////////////////////////



CPhraseDialog::CPhraseDialog(void)
{
	m_iSaidPhraseID = NO_PHRASE;
	m_bFinished = false;

	m_pSpeakerFirst		= NULL;
	m_pSpeakerSecond	= NULL;

	m_eDialogType = eDialogTypeMax;
}

CPhraseDialog::~CPhraseDialog(void)
{
	Msg("CPhraseDialog destructor called");
}


//инициализация диалога
//если диалог с таким id раньше не использовался
//он будет загружен из файла
void CPhraseDialog::Load(ref_str dialog_id)
{
	m_sDialogID = dialog_id;
	m_eDialogType = eDialogTypeMax;

	inherited_shared::load_shared(dialog_id, DIALOGS_XML);
}

void CPhraseDialog::Init(CPhraseDialogManager* speaker_first, 
						 CPhraseDialogManager* speaker_second)
{
	m_pSpeakerFirst = speaker_first;
	m_pSpeakerSecond = speaker_second;

	m_iSaidPhraseID = NO_PHRASE;
	m_PhraseVector.clear();

	CPhraseGraph::CVertex* phrase_vertex = dialog_data()->m_PhraseGraph.vertex(START_PHRASE);
	VERIFY(phrase_vertex);
	m_PhraseVector.push_back(phrase_vertex->data());

	m_bFinished = false;
	m_bFirstIsSpeaking = true;
}

bool CPhraseDialog::SayPhrase (PHRASE_ID phrase_id)
{
	m_bFirstIsSpeaking = !m_bFirstIsSpeaking;
	
	m_iSaidPhraseID = phrase_id;
	
	CPhraseGraph::CVertex* phrase_vertex = dialog_data()->m_PhraseGraph.vertex(m_iSaidPhraseID);
	VERIFY(phrase_vertex);

	//больше нет фраз, чтоб говорить
	if(phrase_vertex->edges().empty()) 
	{
		m_bFinished = true;
		return false;
	}

	//обновить список фраз, которые можно сейчас говорить
	for(xr_vector<CPhraseGraph::CEdge>::const_iterator it = phrase_vertex->edges().begin();
		it != phrase_vertex->edges().end();
		it++)
	{
		const CPhraseGraph::CEdge& edge = *it;
		CPhraseGraph::CVertex* next_phrase_vertex = dialog_data()->m_PhraseGraph.vertex(edge.vertex_index());
		VERIFY(next_phrase_vertex);
		m_PhraseVector.push_back(next_phrase_vertex->data());
	}


	//сообщить CDialogManager, что сказана фраза
	//и ожидается ответ
	if(FirstIsSpeaking())
		FirstSpeaker()->ReceivePhrase((DIALOG_SHARED_PRT)this);
	else
		SecondSpeaker()->ReceivePhrase((DIALOG_SHARED_PRT)this);

	return true;
}


LPCSTR CPhraseDialog::GetPhraseText	(PHRASE_ID phrase_id)
{
	CPhraseGraph::CVertex* phrase_vertex = dialog_data()->m_PhraseGraph.vertex(phrase_id);
	VERIFY(phrase_vertex);
	return phrase_vertex->data()->GetText();
}

void CPhraseDialog::load_shared	(LPCSTR xml_file)
{
	bool xml_result = uiXml.Init("$game_data$", xml_file);
	R_ASSERT2(xml_result, "xml file not found");

	//loading from XML

	XML_NODE* dialog_node = uiXml.NavigateToNodeWithAttribute("dialog", "id", *m_sDialogID);
	R_ASSERT3(dialog_node, "dialog id=", *m_sDialogID);
	uiXml.SetLocalRoot(dialog_node);

	//заполнить граф диалога фразами
	dialog_data()->m_PhraseGraph.clear();

	phrase_list_node = uiXml.NavigateToNode(dialog_node, "phrase_list", 0);
	int phrase_num = uiXml.GetNodesNum(phrase_list_node, "phrase");
	R_ASSERT3(phrase_num, "dialog %s has no phrases at all", *m_sDialogID);

	uiXml.SetLocalRoot(phrase_list_node);
	//ищем стартовую фразу
	XML_NODE* phrase_node = uiXml.NavigateToNodeWithAttribute("phrase", "id", START_PHRASE_STR);
	VERIFY(phrase_node);
	AddPhrase(phrase_node, START_PHRASE);
}


void CPhraseDialog::AddPhrase	(XML_NODE* phrase_node, PHRASE_ID phrase_id)
{
	//проверить не добавлена ли вершина
	if(dialog_data()->m_PhraseGraph.vertex(phrase_id)) return;

	CPhrase* phrase = xr_new<CPhrase>(); VERIFY(phrase);
	phrase->SetIndex(phrase_id);
	//текстовое представление фразы
	phrase->SetText(uiXml.Read(phrase_node, "text", 0, ""));
	dialog_data()->m_PhraseGraph.add_vertex(phrase, phrase_id);

	//фразы которые собеседник может говорить после этой
	int next_num = uiXml.GetNodesNum(phrase_node, "next");
	for(int i=0; i<next_num; i++)
	{
		LPCSTR next_phrase_id_str = uiXml.Read(phrase_node, "next", i, "");
		XML_NODE* next_phrase_node = uiXml.NavigateToNodeWithAttribute("phrase", "id", next_phrase_id_str);
		VERIFY(next_phrase_node);
		int next_phrase_id = atoi(next_phrase_id_str);

		AddPhrase (next_phrase_node, next_phrase_id);
		dialog_data()->m_PhraseGraph.add_edge(phrase_id, next_phrase_id, 0.f);
	}
}