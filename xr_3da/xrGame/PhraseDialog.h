///////////////////////////////////////////////////////////////
// PhraseDialog.h
// ����� ������� - ������� ��� ������ ���� 2� ���������� � ����
///////////////////////////////////////////////////////////////

#pragma once


#include "phrase.h"
#include "graph_manager.h"


typedef CGraphAbstract<CPhrase*, float, u32, u32> CPhraseGraph;
typedef CGraphManagerAbstract<CPhrase*, float, u32, u32> CPhraseGraphManagerAbstract;


struct SPhraseDialogData
{
//////////////////////////////////////////////////////////////////////////
// smart pointer
//////////////////////////////////////////////////////////////////////////
public:
	u16					GetRefCount()   {return ref_count;}
	void				AddRef()		{ref_count++;}
	void				ReleaseRef()	{if(ref_count>0) ref_count--;}
private:
	//������� ������ �� ������
	u16					ref_count;


//////////////////////////////////////////////////////////////////////////
// dialog data: ������ ��� ������������� �������
//////////////////////////////////////////////////////////////////////////
public:
	//��������������� ���� ����
	//��������� ��� ��������� �������� �������� �������
	CPhraseGraph m_PhraseGraph;

	//���������� ������, ���������������� ������ � m_PhraseDialogMap
	ref_str		m_sDialogID;
};



DEFINE_MAP(ref_str,	SPhraseDialogData*, PHRASE_DIALOG_MAP, PHRASE_DIALOG_MAP_IT);



class CPhraseDialog	:public CPhraseGraphManagerAbstract
{
private:
	typedef CPhraseGraphManagerAbstract inherited;
public:
	CPhraseDialog(void);
	virtual ~CPhraseDialog(void);

	//������������� �������
	//���� ������ � ����� id ������ �� �������������
	//�� ����� �������� �� �����
	virtual void Init(ref_str dialog_id);

protected:
	//�������� ������� �� XML �����
	virtual void Load(LPCSTR xml_file);

	//���������� ��������� ��������, ������������ � ���� ������ ����
	//���������� ������������ ������ ��� ���������
	static PHRASE_DIALOG_MAP m_PhraseDialogMap;
};