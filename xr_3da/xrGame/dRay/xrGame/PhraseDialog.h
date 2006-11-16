#pragma once

#include "shared_data.h"
#include "phrase.h"
#include "graph_abstract.h"
#include "PhraseDialogDefs.h"
#include "xml_str_id_loader.h"

typedef CGraphAbstract<CPhrase*, float, int> CPhraseGraph;


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

	//������������ ����� - ��������� ������� (0 �� ���������), ����� ���� �������������
	//� ���� ������ � ������ ������� ����� ������������� �� ����� �������� �� �������� (�����) � �������� (������)
	int	m_iPriority;
};

DEFINE_VECTOR(CPhrase*, PHRASE_VECTOR, PHRASE_VECTOR_IT);

class CPhraseDialog;
class CPhraseDialogManager;

class CPhraseDialog	: public CSharedClass<SPhraseDialogData, shared_str, false>,
					  public CXML_IdToIndex<shared_str, int, CPhraseDialog>
{
private:
	typedef CSharedClass<SPhraseDialogData, shared_str, false>				inherited_shared;
	typedef CXML_IdToIndex<shared_str, int, CPhraseDialog>					id_to_index;

	friend id_to_index;
public:
			 CPhraseDialog	(void);
	virtual ~CPhraseDialog	(void);

	//�������������� copy constructor � �������� ==,
	//���� �� ������� ����������
			CPhraseDialog	(const CPhraseDialog& pharase_dialog) {*this = pharase_dialog;}
			CPhraseDialog&	operator = (const CPhraseDialog& pharase_dialog) {*this = pharase_dialog; return *this;}

	
	virtual void			Load				(shared_str dialog_id);

	//����� ������� ����� ����� DialogManager
	virtual void			Init				(CPhraseDialogManager* speaker_first, CPhraseDialogManager* speaker_second);

	IC		bool			IsInit				() {return ((FirstSpeaker()!=NULL)&& (SecondSpeaker()!=NULL));}

	//��������������� �������
	virtual void Reset  ();

	//������ ���������� ������ �������
	virtual bool			Precondition		(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2);

	//������ ��������� � ������ ������ ����
	virtual const PHRASE_VECTOR& PhraseList		() const			{return m_PhraseVector;}
			bool				allIsDummy		();
	//������� ����� � ������� � ��������� ������ �������
	//���� ������� false, �� �������, ��� ������ ����������
	//(������� �����������, ��� ��� �� ������ ���������� ������� DIALOG_SHARED_PTR&,
	//� �� ������� ���������)
	static bool				SayPhrase			(DIALOG_SHARED_PTR& phrase_dialog, int phrase_id);

	virtual LPCSTR			GetPhraseText		(int phrase_id, bool current_speaking = true);
	virtual LPCSTR			GetLastPhraseText	() {return GetPhraseText(m_iSaidPhraseID, false);}
	virtual int				GetLastPhraseID		() {return m_iSaidPhraseID;}

	//���������, �������, ���� �� �����, �� 0-� �����
	virtual LPCSTR			DialogCaption		();
	virtual int				Priority			();


	virtual bool			IsFinished			()	const {return m_bFinished;}
	
	IC	CPhraseDialogManager* FirstSpeaker		()	const {return m_pSpeakerFirst;}
	IC	CPhraseDialogManager* SecondSpeaker		()	const {return m_pSpeakerSecond;}
	   
		//��� ���������� �������� � ��� �������
		CPhraseDialogManager* CurrentSpeaker	()	const;
	    CPhraseDialogManager* OtherSpeaker		()	const;
		//��� ��������� ������ �����
		CPhraseDialogManager* LastSpeaker		()	const {return m_bFirstIsSpeaking?SecondSpeaker():FirstSpeaker();}

	IC bool					FirstIsSpeaking		()	const {return m_bFirstIsSpeaking;}
	IC bool					SecondIsSpeaking	()	const {return !m_bFirstIsSpeaking;}

	IC bool					IsWeSpeaking		(CPhraseDialogManager* dialog_manager) const  {return (FirstSpeaker()==dialog_manager && FirstIsSpeaking()) ||
																							(SecondSpeaker()==dialog_manager && SecondIsSpeaking());}
	CPhraseDialogManager*	OurPartner			(CPhraseDialogManager* dialog_manager) const;

protected:
	//������������� �������
	shared_str				m_DialogId;

	//ID ��������� ��������� ����� � �������, -1 ���� ����� �� ����
	int						m_iSaidPhraseID;
	//������ ��������
	bool					m_bFinished;

	//������ ���������� �� ����� ��������� � ������ ������
	PHRASE_VECTOR			m_PhraseVector;

	//��������� �� ������������ � �������
	CPhraseDialogManager*	m_pSpeakerFirst;
	CPhraseDialogManager*	m_pSpeakerSecond;
	bool					m_bFirstIsSpeaking;

	const SPhraseDialogData* data		() const	{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	SPhraseDialogData*		data		()			{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	//�������� ������� �� XML �����
	virtual void			load_shared	(LPCSTR);
	
	//����������� ���������� ���� � ����
	void					AddPhrase	(XML_NODE* phrase_node, int phrase_id, int prev_phrase_id);
public:
	CPhrase*				AddPhrase	(LPCSTR text, int phrase_id, int prev_phrase_id, int goodwil_level);
	void					SetCaption	(LPCSTR str);
	void					SetPriority	(int val);

protected:

	//��������� ������ ��� ����������� �������
	CUIXml					uiXml;
	XML_NODE*				phrase_list_node;

	static void				InitXmlIdToIndex();
};
