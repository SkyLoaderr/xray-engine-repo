///////////////////////////////////////////////////////////////
// InfoPortion.h
// ��������, �������������� �������� ����������
///////////////////////////////////////////////////////////////


#pragma once


#include "ui/uistring.h"
#include "ui/xrXMLParser.h"
#include "map_location.h"


#define NO_INFO_INDEX (-1)




DEF_LIST (INFO_INDEX_LIST, int);

//������ 
typedef struct tagSInfoQuestion
{
	//���������� ����� ������� � ����������
	int num_in_info;
	//������ ���������� ������� �������� ���� ������
	int info_portion_index;

	//������ �������� ������ ����������
	//������� �� ����� �������� � ����� �� ���� ������
	INFO_INDEX_LIST IndexList;

	//��������� ������������� �������
	CUIString text;
	//��������� �� ���������� �����
	CUIString negative_answer_text;
		
} SInfoQuestion;


DEF_LIST (INFO_QUESTIONS_LIST, SInfoQuestion);

//�����  - ������ ����������
class CInfoPortion 
{
public:
	CInfoPortion(void);
	virtual ~CInfoPortion(void);

	int				GetIndex()			{return m_iIndex;}
	int				GetLocationsNum()	  {return m_MapLocationVector.size();}
	SMapLocation&	GetLocation(int index) {return	m_MapLocationVector[index];}
	LPCSTR			GetText()		{return m_text.GetBuf();}

	//�������� ��������� ����������� �� �����
	void			Load(int index);
	void			GetQuestion(SInfoQuestion& question, int question_num);
	//������� �������� �� XML
	void			LoadInfoPortionFromXml(CUIXml& uiXml, int num_in_file);

	//������ ��������, ������� ����� ����� �������� ����� ��������� 
	//����������
	INFO_QUESTIONS_LIST m_QuestionsList;


	DEFINE_VECTOR(ref_str, DIALOG_NAME_VECTOR, DIALOG_NAME_VECTOR_IT);
	//������ � ������� ��������, ������� ����� ���� ������������
	//�� ����� InfoPortion
	DIALOG_NAME_VECTOR m_DialogNames;

protected:
	bool m_bLoaded;

	//���������� ������ � ������ ���� ��������� ������� ����������
	int	m_iIndex;
	//������ ������� �� �����
	LOCATIONS_VECTOR	m_MapLocationVector;
	//��������� ������������� ����������
	CUIString m_text;
};