///////////////////////////////////////////////////////////////
// PhraseDialog.h
// ����� ������� - ������� ��� ������ ���� 2� ���������� � ����
///////////////////////////////////////////////////////////////

#pragma once

#include "shared_data.h"
#include "phrase.h"
#include "graph_abstract.h"
#include "PhraseDialogDefs.h"

typedef CGraphAbstract<CPhrase*, float, u32, u32> CPhraseGraph;


//////////////////////////////////////////////////////////////////////////
// SPhraseDialogData: ������ ��� ������������� �������
//////////////////////////////////////////////////////////////////////////
struct SPhraseDialogData : CSharedResource
{
	SPhraseDialogData ();
	virtual ~SPhraseDialogData ();

	//��������������� ���� ����
	//��������� ��� ��������� �������� �������� �������
	CPhraseGraph m_PhraseGraph;

	//���������� ������, ���������������� ������
	ref_str		m_sDialogID;
	//��������� �������, ���� NULL, �� ����������� �� ��������� �����
	ref_str		m_sCaption;

	//������ ���������� ����������, ����������, ������� ����������
	//��� ������ �������
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

	//�������������� copy constructor � �������� ==,
	//���� �� ������� ����������
    CPhraseDialog	(const CPhraseDialog& pharase_dialog) {*this = pharase_dialog;}
	CPhraseDialog&	operator = (const CPhraseDialog& pharase_dialog) {*this = pharase_dialog; return *this;}

	
	//������������� ������� �������
	//���� ������ � ����� id ������ �� �������������
	//�� ����� �������� �� �����
	virtual void Load	(ref_str dialog_id);
	//����� ������� ����� ����� DialogManager
	virtual void Init	(CPhraseDialogManager* speaker_first, 
						 CPhraseDialogManager* speaker_second);

	IC		bool IsInit () {return ((FirstSpeaker()!=NULL)&& (SecondSpeaker()!=NULL));}

	//��������������� �������
	virtual void Reset  ();

	//������ ���������� ������ �������
	virtual bool Precondition(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2);

	//������ ��������� � ������ ������ ����
	virtual const PHRASE_VECTOR& PhraseList() const			{return m_PhraseVector;}
	
	//������� ����� � ������� � ��������� ������ �������
	//���� ������� false, �� �������, ��� ������ ����������
	//(������� �����������, ��� ��� �� ������ ���������� ������� DIALOG_SHARED_PTR&,
	//� �� ������� ���������)
	static bool			SayPhrase		(DIALOG_SHARED_PTR& phrase_dialog, PHRASE_ID phrase_id);

	virtual LPCSTR		GetPhraseText		(PHRASE_ID phrase_id, bool current_speaking = true);
	virtual LPCSTR		GetLastPhraseText	() {return GetPhraseText(m_iSaidPhraseID, false);}
	virtual PHRASE_ID	GetLastPhraseID		() {return m_iSaidPhraseID;}

	//���������, �������, ���� �� �����, �� 0-� �����
	virtual LPCSTR		DialogCaption	();


	virtual bool		IsFinished		()	const {return m_bFinished;}
	
	IC	CPhraseDialogManager* FirstSpeaker	()	const {return m_pSpeakerFirst;}
	IC	CPhraseDialogManager* SecondSpeaker	()	const {return m_pSpeakerSecond;}
	   
		//��� ���������� �������� � ��� �������
		CPhraseDialogManager* CurrentSpeaker	()	const;
	    CPhraseDialogManager* OtherSpeaker	()	const;
		//��� ��������� ������ �����
		CPhraseDialogManager* LastSpeaker	()	const {return m_bFirstIsSpeaking?SecondSpeaker():FirstSpeaker();}

	IC bool				FirstIsSpeaking	()	const {return m_bFirstIsSpeaking;}
	IC bool				SecondIsSpeaking()	const {return !m_bFirstIsSpeaking;}

	IC bool				IsWeSpeaking	(CPhraseDialogManager* dialog_manager) const  {return (FirstSpeaker()==dialog_manager && FirstIsSpeaking()) ||
																							(SecondSpeaker()==dialog_manager && SecondIsSpeaking());}
	CPhraseDialogManager* OurPartner	(CPhraseDialogManager* dialog_manager) const;
		
	EDialogType			GetDialogType	()	const {return m_eDialogType;}
	void				SetDialogType	(EDialogType type)	{m_eDialogType = type;}

protected:
	//������������� �������
	ref_str		m_sDialogID;
	//��� �������
	EDialogType m_eDialogType;

	//ID ��������� ��������� ����� � �������, -1 ���� ����� �� ����
	PHRASE_ID	m_iSaidPhraseID;
	//������ ��������
	bool		m_bFinished;

	//������ ���������� �� ����� ��������� � ������ ������
	PHRASE_VECTOR m_PhraseVector;

	//��������� �� ������������ � �������
	CPhraseDialogManager* m_pSpeakerFirst;
	CPhraseDialogManager* m_pSpeakerSecond;
	//���� ����� ������� 1� ����� - true, ���� 2� - false
	bool				  m_bFirstIsSpeaking;

	SPhraseDialogData* dialog_data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	//�������� ������� �� XML �����
	virtual void load_shared	(LPCSTR xml_file);
	
	//����������� ���������� ���� � ����
	virtual void AddPhrase	(XML_NODE* phrase_node, int node_id);

	//��������� ������ ��� ����������� �������
	CUIXml					uiXml;
	XML_NODE*				phrase_list_node;

	//���������, ��� ��� ������� ����� ���������� ID
	static bool	m_bCheckUniqueness;
};
