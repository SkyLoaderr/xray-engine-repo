///////////////////////////////////////////////////////////////
// PhraseDialog.h
// Класс диалога - общения при помощи фраз 2х персонажей в игре
///////////////////////////////////////////////////////////////

#pragma once

#include "shared_data.h"
#include "phrase.h"
#include "graph_abstract.h"
#include "PhraseDialogDefs.h"

typedef CGraphAbstract<CPhrase*, float, u32, u32> CPhraseGraph;


//////////////////////////////////////////////////////////////////////////
// SPhraseDialogData: данные для представления диалога
//////////////////////////////////////////////////////////////////////////
struct SPhraseDialogData : CSharedResource
{
	SPhraseDialogData ();
	virtual ~SPhraseDialogData ();

	//однонаправленый граф фраз
	//описывает все возможные варианты развития диалога
	CPhraseGraph m_PhraseGraph;

	//уникальная строка, идентифицирующая диалог в m_PhraseDialogMap
	ref_str		m_sDialogID;
};

DEFINE_VECTOR(CPhrase*, PHRASE_VECTOR, PHRASE_VECTOR_IT);


class CPhraseDialogManager;


class CPhraseDialog	: public CSharedClass<SPhraseDialogData, ref_str>
{
private:
	typedef CSharedClass<SPhraseDialogData, ref_str> inherited_shared;
public:
			 CPhraseDialog	(void);
	virtual ~CPhraseDialog	(void);

	//переобределяем copy constructor и оператор ==,
	//чтоб не ругался компилятор
    CPhraseDialog	(const CPhraseDialog& pharase_dialog) {*this = pharase_dialog;}
	CPhraseDialog&	operator = (const CPhraseDialog& pharase_dialog) {*this = pharase_dialog; return *this;}

	
	//инициализация диалога данными
	//если диалог с таким id раньше не использовался
	//он будет загружен из файла
	virtual void Load	(ref_str dialog_id);
	//связь диалога между двумя DialogManager
	virtual void Init	(CPhraseDialogManager* speaker_first, 
						 CPhraseDialogManager* speaker_second);

	virtual const PHRASE_VECTOR& PhraseList() const			{return m_PhraseVector;}
	
	//сказать фразу и перейти к следующей стадии диалога
	//если вернули false, то считаем, что диалог закончился
	virtual bool		SayPhrase		(PHRASE_ID phrase_id);
	virtual LPCSTR		GetPhraseText	(PHRASE_ID phrase_id);


	virtual bool		IsFinished		()	const {return m_bFinished;}
	
	CPhraseDialogManager* FirstSpeaker	()	const {return m_pSpeakerFirst;}
	CPhraseDialogManager* SecondSpeaker	()	const {return m_pSpeakerSecond;}

	bool				FirstIsSpeaking	()	const {return m_bFirstIsSpeaking;}
	bool				SecondIsSpeaking()	const {return !m_bFirstIsSpeaking;}

	bool				IsWeSpeaking	(CPhraseDialogManager* dialog_manager) const  {return (FirstSpeaker()==dialog_manager && FirstIsSpeaking()) ||
																							(SecondSpeaker()==dialog_manager && SecondIsSpeaking());}
	
	EDialogType			GetDialogType	()	const {return m_eDialogType;}
	void				SetDialogType	(EDialogType type)	{m_eDialogType = type;}

protected:
	//идентификатор диалога
	ref_str		m_sDialogID;
	//тип диалога
	EDialogType m_eDialogType;

	//ID последней сказанной фразы в диалоге, -1 если такой не было
	PHRASE_ID	m_iSaidPhraseID;
	//диалог закончен
	bool		m_bFinished;

	//список указателей на фразы доступные в данный момент
	PHRASE_VECTOR m_PhraseVector;

	//указатели на собеседников в диалоге
	CPhraseDialogManager* m_pSpeakerFirst;
	CPhraseDialogManager* m_pSpeakerSecond;
	//если фразу говорит 1й игрок - true, если 2й - false
	bool				  m_bFirstIsSpeaking;


	SPhraseDialogData* dialog_data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	//загрузка диалога из XML файла
	virtual void load_shared	(LPCSTR xml_file);
	
	//рекурсивное добавление фраз в граф
	virtual void AddPhrase	(XML_NODE* phrase_node, int node_id);

	//буфферные данные для рекурсивной функции
	CUIXml					uiXml;
	XML_NODE*				phrase_list_node;
};
