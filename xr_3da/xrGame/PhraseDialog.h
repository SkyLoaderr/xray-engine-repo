///////////////////////////////////////////////////////////////
// PhraseDialog.h
// ����� ������� - ������� ��� ������ ���� 2� ���������� � ����
///////////////////////////////////////////////////////////////

#pragma once

#include "shared_data.h"
#include "phrase.h"
#include "graph_abstract.h"
#include "PhraseDialogDefs.h"
#include "xml_str_id_loader.h"

typedef CGraphAbstract<CPhrase*, float, u32, u32> CPhraseGraph;


//////////////////////////////////////////////////////////////////////////
// SPhraseDialogData: ������ ��� ������������� �������
//////////////////////////////////////////////////////////////////////////
struct SPhraseDialogData : CSharedResource
{
	SPhraseDialogData ();
	virtual ~SPhraseDialogData ();

	//��������� �������, ���� NULL, �� ����������� �� ��������� �����
	shared_str		m_sCaption;

	//��������������� ���� ����
	//��������� ��� ��������� �������� �������� �������
	CPhraseGraph	m_PhraseGraph;

	//������ ���������� ����������, ����������, ������� ����������
	//��� ������ �������
	CPhraseScript	m_PhraseScript;

	EDialogType		m_eDialogType;
	
	//������������ ����� - ��������� ������� (0 �� ���������), ����� ���� �������������
	//� ���� ������ � ������ ������� ����� ������������� �� ����� �������� �� �������� (�����) � �������� (������)
	int	m_iPriority;
};

DEFINE_VECTOR(CPhrase*, PHRASE_VECTOR, PHRASE_VECTOR_IT);

class CPhraseDialog;
class CPhraseDialogManager;

class CPhraseDialog	: public CSharedClass<SPhraseDialogData, PHRASE_DIALOG_INDEX>,
					  public CXML_IdToIndex<PHRASE_DIALOG_ID, PHRASE_DIALOG_INDEX, CPhraseDialog>
{
private:
	typedef CSharedClass<SPhraseDialogData, PHRASE_DIALOG_INDEX>					inherited_shared;
	typedef CXML_IdToIndex<PHRASE_DIALOG_ID, PHRASE_DIALOG_INDEX, CPhraseDialog>	id_to_index;

	friend id_to_index;
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
	virtual void Load	(PHRASE_DIALOG_ID dialog_id);
	virtual void Load	(PHRASE_DIALOG_INDEX dialog_index);

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
	virtual int			Priority		();


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
		
	EDialogType			GetDialogType	()	const {return data()->m_eDialogType;}
	void				SetDialogType	(EDialogType type)	{data()->m_eDialogType = type;}

protected:
	//������������� �������
	PHRASE_DIALOG_INDEX	m_DialogIndex;

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

	const SPhraseDialogData* data() const  { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	SPhraseDialogData* data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	//�������� ������� �� XML �����
	virtual void load_shared	(LPCSTR);
	
	//����������� ���������� ���� � ����
	virtual void AddPhrase	(XML_NODE* phrase_node, int node_id);

	//��������� ������ ��� ����������� �������
	CUIXml					uiXml;
	XML_NODE*				phrase_list_node;

	static void				InitXmlIdToIndex();
};
