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
#include "gameobject.h"



bool CPhraseDialog::m_bCheckUniqueness = false;


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
	VERIFY(!IsInit());

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

//обнуляем все связи
void CPhraseDialog::Reset ()
{
}

CPhraseDialogManager* CPhraseDialog::OurPartner	(CPhraseDialogManager* dialog_manager) const
{
	if(FirstSpeaker() == dialog_manager)
		return SecondSpeaker();
	else
		return FirstSpeaker();
}

//предикат для сортировки вектора фраз
static bool PhraseGoodwillPred(const CPhrase* phrase1, const CPhrase* phrase2)
{
	return phrase1->GoodwillLevel()>phrase2->GoodwillLevel();
}

bool CPhraseDialog::SayPhrase (DIALOG_SHARED_PTR& phrase_dialog, PHRASE_ID phrase_id)
{
	VERIFY(phrase_dialog->IsInit());

	phrase_dialog->m_iSaidPhraseID = phrase_id;

	bool first_is_speaking = phrase_dialog->FirstIsSpeaking();
	phrase_dialog->m_bFirstIsSpeaking = !phrase_dialog->m_bFirstIsSpeaking;

	const CGameObject*	pSpeakerGO1 = dynamic_cast<const CGameObject*>(phrase_dialog->FirstSpeaker());	VERIFY(pSpeakerGO1);
	const CGameObject*	pSpeakerGO2 = dynamic_cast<const CGameObject*>(phrase_dialog->SecondSpeaker());	VERIFY(pSpeakerGO2);
	if(!first_is_speaking) std::swap(pSpeakerGO1, pSpeakerGO2);

	CPhraseGraph::CVertex* phrase_vertex = phrase_dialog->dialog_data()->m_PhraseGraph.vertex(phrase_dialog->m_iSaidPhraseID);
	VERIFY(phrase_vertex);

	CPhrase* last_phrase = phrase_vertex->data();

	//больше нет фраз, чтоб говорить
	phrase_dialog->m_PhraseVector.clear();
	if(phrase_vertex->edges().empty())
	{
		phrase_dialog->m_bFinished = true;
	}
	else
	{
		//обновить список фраз, которые можно сейчас говорить
		for(xr_vector<CPhraseGraph::CEdge>::const_iterator it = phrase_vertex->edges().begin();
			it != phrase_vertex->edges().end();
			it++)
		{
			const CPhraseGraph::CEdge& edge = *it;
			CPhraseGraph::CVertex* next_phrase_vertex = phrase_dialog->dialog_data()->m_PhraseGraph.vertex(edge.vertex_id());
			VERIFY(next_phrase_vertex);

			if(next_phrase_vertex->data()->m_PhraseScript.Precondition(pSpeakerGO1, pSpeakerGO2))
				phrase_dialog->m_PhraseVector.push_back(next_phrase_vertex->data());
		}

		R_ASSERT3(!phrase_dialog->m_PhraseVector.empty(), "No available phrase to say.", *phrase_dialog->m_sDialogID);

		//упорядочить списко по убыванию благосклонности
		std::sort(phrase_dialog->m_PhraseVector.begin(),
				 phrase_dialog->m_PhraseVector.end(), PhraseGoodwillPred);
	}

	//вызвать скриптовую присоединенную функцию 
	//активируется после сказанной фразы
	//первый параметр - тот кто говорит фразу, второй - тот кто слушает
	last_phrase->m_PhraseScript.Action(pSpeakerGO1, pSpeakerGO2);

	//сообщить CDialogManager, что сказана фраза
	//и ожидается ответ
	if(first_is_speaking)
		phrase_dialog->SecondSpeaker()->ReceivePhrase(phrase_dialog);
	else
		phrase_dialog->FirstSpeaker()->ReceivePhrase(phrase_dialog);


	return !phrase_dialog->m_bFinished;
}

LPCSTR CPhraseDialog::GetPhraseText	(PHRASE_ID phrase_id)
{
	CPhraseGraph::CVertex* phrase_vertex = dialog_data()->m_PhraseGraph.vertex(phrase_id);
	VERIFY(phrase_vertex);
	return phrase_vertex->data()->GetText();
}

LPCSTR CPhraseDialog::DialogCaption()
{
	return *dialog_data()->m_sCaption?*dialog_data()->m_sCaption:GetPhraseText(START_PHRASE);
}

void CPhraseDialog::load_shared	(LPCSTR xml_file)
{
	bool xml_result = uiXml.Init("$game_data$", xml_file);
	R_ASSERT2(xml_result, "xml file not found");

	//проверить файл диалога на корректность
	if(!m_bCheckUniqueness)	
	{
		LPCSTR wrong_id = uiXml.CheckUniqueAttrib(uiXml.GetRoot(), "dialog", "id");
		R_ASSERT3(wrong_id == NULL, "dublicate dialog id", wrong_id);
		m_bCheckUniqueness = true;
	}

	//loading from XML

	XML_NODE* dialog_node = uiXml.NavigateToNodeWithAttribute("dialog", "id", *m_sDialogID);
	R_ASSERT3(dialog_node, "dialog id=", *m_sDialogID);
	uiXml.SetLocalRoot(dialog_node);

	//заголовок 
	dialog_data()->m_sCaption = uiXml.Read(dialog_node, "caption", 0, NULL);

	//предикаты начала диалога
	dialog_data()->m_PhraseScript.Load(uiXml, dialog_node);

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
	if(dialog_data()->m_PhraseGraph.vertex(phrase_id)) 
		return;

	CPhrase* phrase = xr_new<CPhrase>(); VERIFY(phrase);
	phrase->SetIndex(phrase_id);
	
	//текстовое представление фразы
	phrase->SetText(uiXml.Read(phrase_node, "text", 0, ""));
	//уровень благосклонности
	phrase->m_iGoodwillLevel = uiXml.ReadInt(phrase_node, "goodwill", 0, 0);
	
	//прочитать действия и предикаты
	phrase->m_PhraseScript.Load(uiXml, phrase_node);
	
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

bool  CPhraseDialog::Precondition(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2)
{
	return dialog_data()->m_PhraseScript.Precondition(pSpeaker1, pSpeaker2);
}