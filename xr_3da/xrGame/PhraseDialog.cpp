///////////////////////////////////////////////////////////////
// PhraseDialog.cpp
// Класс диалога - общения при помощи фраз 2х персонажей в игре
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "phrasedialog.h"
#include "phrasedialogmanager.h"
#include "gameobject.h"


//////////////////////////////////////////////////////////////////////////

SPhraseDialogData::SPhraseDialogData ()
{
	m_PhraseGraph.clear();
	m_eDialogType = eDialogTypeMax;
	m_iPriority = 0;
}

SPhraseDialogData::~SPhraseDialogData ()
{
}

//////////////////////////////////////////////////////////////////////////



CPhraseDialog::CPhraseDialog(void)
{
	m_iSaidPhraseID = NO_PHRASE;
	m_bFinished = false;

	m_pSpeakerFirst		= NULL;
	m_pSpeakerSecond	= NULL;

	m_DialogIndex = NO_PHRASE_DIALOG;
}

CPhraseDialog::~CPhraseDialog(void)
{
}


void CPhraseDialog::Init(CPhraseDialogManager* speaker_first, 
						 CPhraseDialogManager* speaker_second)
{
	VERIFY(!IsInit());

	m_pSpeakerFirst = speaker_first;
	m_pSpeakerSecond = speaker_second;

	m_iSaidPhraseID = NO_PHRASE;
	m_PhraseVector.clear();

	CPhraseGraph::CVertex* phrase_vertex = data()->m_PhraseGraph.vertex(START_PHRASE);
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


CPhraseDialogManager* CPhraseDialog::CurrentSpeaker()	const 
{
	return FirstIsSpeaking()?m_pSpeakerFirst:m_pSpeakerSecond;
}
CPhraseDialogManager* CPhraseDialog::OtherSpeaker	()	const 
{
	return (!FirstIsSpeaking())?m_pSpeakerFirst:m_pSpeakerSecond;
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

	CPhraseGraph::CVertex* phrase_vertex = phrase_dialog->data()->m_PhraseGraph.vertex(phrase_dialog->m_iSaidPhraseID);
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
		//обновить список фраз, которые сейчас сможет говорить собеседник
		for(xr_vector<CPhraseGraph::CEdge>::const_iterator it = phrase_vertex->edges().begin();
			it != phrase_vertex->edges().end();
			it++)
		{
			const CPhraseGraph::CEdge& edge = *it;
			CPhraseGraph::CVertex* next_phrase_vertex = phrase_dialog->data()->m_PhraseGraph.vertex(edge.vertex_id());
			VERIFY(next_phrase_vertex);

			if(next_phrase_vertex->data()->m_PhraseScript.Precondition(pSpeakerGO2, pSpeakerGO1))
				phrase_dialog->m_PhraseVector.push_back(next_phrase_vertex->data());
		}

		R_ASSERT3(!phrase_dialog->m_PhraseVector.empty(), "No available phrase to say.", *ref_str(CPhraseDialog::IndexToId(phrase_dialog->m_DialogIndex)));

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

LPCSTR CPhraseDialog::GetPhraseText	(PHRASE_ID phrase_id, bool current_speaking)
{
	
	CPhraseGraph::CVertex* phrase_vertex = data()->m_PhraseGraph.vertex(phrase_id);
	VERIFY(phrase_vertex);
	
	//если есть скриптовый текст, то он и будет задан
	const CGameObject*	pSpeakerGO1 = dynamic_cast<const CGameObject*>(CurrentSpeaker());
	const CGameObject*	pSpeakerGO2 = dynamic_cast<const CGameObject*>(OtherSpeaker());	

	if(!current_speaking) 
		std::swap(pSpeakerGO1, pSpeakerGO2);
	
	LPCSTR script_text = NULL;
	//если собеседники еще не заданы, то текст фразы запрашивается только для 
	//обычный но не скриптовый
	if(pSpeakerGO1 && pSpeakerGO2)
		script_text = phrase_vertex->data()->GetScriptText(pSpeakerGO1, pSpeakerGO2);
	return script_text?script_text:phrase_vertex->data()->GetText();
}

LPCSTR CPhraseDialog::DialogCaption()
{
	return *data()->m_sCaption?*data()->m_sCaption:GetPhraseText(START_PHRASE);
}


int	 CPhraseDialog::Priority()
{
	return data()->m_iPriority;
}


//инициализация диалога
//если диалог с таким id раньше не использовался
//он будет загружен из файла
void CPhraseDialog::Load(PHRASE_DIALOG_ID dialog_id)
{
	Load(id_to_index::IdToIndex(dialog_id));
}

void CPhraseDialog::Load(PHRASE_DIALOG_INDEX dialog_index)
{
	m_DialogIndex = dialog_index;
	inherited_shared::load_shared(m_DialogIndex, NULL);
}


void CPhraseDialog::load_shared	(LPCSTR)
{
	const id_to_index::ITEM_DATA& item_data = id_to_index::GetByIndex(m_DialogIndex);

	string128 xml_file_full;
	strconcat(xml_file_full, *ref_str(item_data.file_name), ".xml");

	bool xml_result = uiXml.Init("$game_data$", xml_file_full);
	R_ASSERT3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* dialog_node = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	R_ASSERT3(dialog_node, "dialog id=", *ref_str(item_data.id));

	uiXml.SetLocalRoot(dialog_node);

	int pda_dialog = uiXml.ReadAttribInt(dialog_node, "pda", 0);
	if (1 == pda_dialog) 
		data()->m_eDialogType = eDialogTypePDA;

	data()->m_iPriority = uiXml.ReadAttribInt(dialog_node, "priority", 0);

	//заголовок 
	data()->m_sCaption = uiXml.Read(dialog_node, "caption", 0, NULL);

	//предикаты начала диалога
	data()->m_PhraseScript.Load(uiXml, dialog_node);

	//заполнить граф диалога фразами
	data()->m_PhraseGraph.clear();

	phrase_list_node = uiXml.NavigateToNode(dialog_node, "phrase_list", 0);
	int phrase_num = uiXml.GetNodesNum(phrase_list_node, "phrase");
	R_ASSERT3(phrase_num, "dialog %s has no phrases at all", *ref_str(item_data.id));

	uiXml.SetLocalRoot(phrase_list_node);

	LPCSTR wrong_phrase_id = uiXml.CheckUniqueAttrib(phrase_list_node, "phrase", "id");
	R_ASSERT3(wrong_phrase_id == NULL, *ref_str(item_data.id), wrong_phrase_id);
	

	//ищем стартовую фразу
	XML_NODE* phrase_node = uiXml.NavigateToNodeWithAttribute("phrase", "id", START_PHRASE_STR);
	VERIFY(phrase_node);
	AddPhrase(phrase_node, START_PHRASE);
}


void CPhraseDialog::AddPhrase	(XML_NODE* phrase_node, PHRASE_ID phrase_id)
{
	//проверить не добавлена ли вершина
	if(data()->m_PhraseGraph.vertex(phrase_id)) 
		return;

	CPhrase* phrase = xr_new<CPhrase>(); VERIFY(phrase);
	phrase->SetIndex(phrase_id);
	
	//текстовое представление фразы
	phrase->SetText(uiXml.Read(phrase_node, "text", 0, ""));
	//уровень благосклонности
	phrase->m_iGoodwillLevel = uiXml.ReadInt(phrase_node, "goodwill", 0, 0);
	
	//прочитать действия и предикаты
	phrase->m_PhraseScript.Load(uiXml, phrase_node);
	
	data()->m_PhraseGraph.add_vertex(phrase, phrase_id);

	//фразы которые собеседник может говорить после этой
	int next_num = uiXml.GetNodesNum(phrase_node, "next");
	for(int i=0; i<next_num; i++)
	{
		LPCSTR next_phrase_id_str = uiXml.Read(phrase_node, "next", i, "");
		XML_NODE* next_phrase_node = uiXml.NavigateToNodeWithAttribute("phrase", "id", next_phrase_id_str);
		VERIFY(next_phrase_node);
		int next_phrase_id = atoi(next_phrase_id_str);

		AddPhrase (next_phrase_node, next_phrase_id);
		data()->m_PhraseGraph.add_edge(phrase_id, next_phrase_id, 0.f);
	}
}

bool  CPhraseDialog::Precondition(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2)
{
	return data()->m_PhraseScript.Precondition(pSpeaker1, pSpeaker2);
}

void   CPhraseDialog::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "dialog";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("dialogs", "files");
}