///////////////////////////////////////////////////////////////
// PhraseDialog.h
//  ласс диалога - общени€ при помощи фраз 2х персонажей в игре
///////////////////////////////////////////////////////////////

#pragma once

#include "shared_data.h"
#include "phrase.h"
#include "graph_abstract.h"
#include "PhraseDialogDefs.h"

typedef CGraphAbstract<CPhrase*, float, u32, u32> CPhraseGraph;


//////////////////////////////////////////////////////////////////////////
// SPhraseDialogData: данные дл€ представлени€ диалога
//////////////////////////////////////////////////////////////////////////
struct SPhraseDialogData : CSharedResource
{
	SPhraseDialogData ();
	virtual ~SPhraseDialogData ();

	//однонаправленый граф фраз
	//описывает все возможные варианты развити€ диалога
	CPhraseGraph m_PhraseGraph;

	//уникальна€ строка, идентифицирующа€ диалог
	ref_str		m_sDialogID;
	//заголовок диалога, если NULL, то принимаетс€ за стартовую фразу
	ref_str		m_sCaption;

	//список скриптовых предикатов, выполнение, которых необходимо
	//дл€ начала диалога
	CPhraseScript m_PhraseScript;
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

	//переобредел€ем copy constructor и оператор ==,
	//чтоб не ругалс€ компил€тор
    CPhraseDialog	(const CPhraseDialog& pharase_dialog) {*this = pharase_dialog;}
	CPhraseDialog&	operator = (const CPhraseDialog& pharase_dialog) {*this = pharase_dialog; return *this;}

	
	//инициализаци€ диалога данными
	//если диалог с таким id раньше не использовалс€
	//он будет загружен из файла
	virtual void Load	(ref_str dialog_id);
	//св€зь диалога между двум€ DialogManager
	virtual void Init	(CPhraseDialogManager* speaker_first, 
						 CPhraseDialogManager* speaker_second);

	IC		bool IsInit () {return ((FirstSpeaker()!=NULL)&& (SecondSpeaker()!=NULL));}

	//реинициализаци€ диалога
	virtual void Reset  ();

	//список предикатов начала диалога
	virtual bool Precondition(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2);

	//список доступных в данный момент фраз
	virtual const PHRASE_VECTOR& PhraseList() const			{return m_PhraseVector;}
	
	//сказать фразу и перейти к следующей стадии диалога
	//если вернули false, то считаем, что диалог закончилс€
	//(сделано статическим, так как мы должны передавать имеенно DIALOG_SHARED_PTR&,
	//а не обычный указатель)
	static bool			SayPhrase		(DIALOG_SHARED_PTR& phrase_dialog, PHRASE_ID phrase_id);

	virtual LPCSTR		GetPhraseText		(PHRASE_ID phrase_id);
	virtual LPCSTR		GetLastPhraseText	() {return GetPhraseText(m_iSaidPhraseID);}
	virtual PHRASE_ID	GetLastPhraseID		() {return m_iSaidPhraseID;}

	//заголовок, диалога, если не задан, то 0-€ фраза
	virtual LPCSTR		DialogCaption	();


	virtual bool		IsFinished		()	const {return m_bFinished;}
	
	IC CPhraseDialogManager* FirstSpeaker	()	const {return m_pSpeakerFirst;}
	IC CPhraseDialogManager* SecondSpeaker	()	const {return m_pSpeakerSecond;}

	IC bool				FirstIsSpeaking	()	const {return m_bFirstIsSpeaking;}
	IC bool				SecondIsSpeaking()	const {return !m_bFirstIsSpeaking;}

	IC bool				IsWeSpeaking	(CPhraseDialogManager* dialog_manager) const  {return (FirstSpeaker()==dialog_manager && FirstIsSpeaking()) ||
																							(SecondSpeaker()==dialog_manager && SecondIsSpeaking());}
	CPhraseDialogManager* OurPartner	(CPhraseDialogManager* dialog_manager) const;
		
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

	//буфферные данные дл€ рекурсивной функции
	CUIXml					uiXml;
	XML_NODE*				phrase_list_node;
};
