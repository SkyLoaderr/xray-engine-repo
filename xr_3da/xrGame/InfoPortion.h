///////////////////////////////////////////////////////////////
// InfoPortion.h
// ��������, �������������� �������� ����������
///////////////////////////////////////////////////////////////


#pragma once


#include "ui/uistring.h"
#include "ui/xrXMLParser.h"


//��������� ��� ������� �� �����
typedef struct tagSMapLocation
{
	//����� ������
	int level_num;
	//���������� �� ������
	float x;
	float y;
	//��� �������
	CUIString name;

	//������� � ��������� ������
	int icon_x, icon_y, icon_width, icon_height;

} SMapLocation;


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

	int				GetIndex()		{return m_iIndex;}
	SMapLocation&	GetLocation()	{return m_sMapLocation;}
	bool			LocationExist() {return m_bLocationSet;}
	char*			GetText()		{return m_text.GetBuf();}

	//�������� ��������� ����������� �� �����
	void			Load(int index);
	void			GetQuestion(SInfoQuestion& question, int question_num);
	//������� �������� �� XML
	void LoadInfoPortionFromXml(CUIXml& uiXml, int num_in_file);

	//������ ��������, ������� ����� ����� �������� ����� ��������� 
	//����������
	INFO_QUESTIONS_LIST m_QuestionsList;


protected:
	bool m_bLoaded;

	//���������� ������ � ������ ���� ��������� ������� ����������
	int	m_iIndex;

	//��������� �������, ���� ��� ������
	bool m_bLocationSet;
	SMapLocation m_sMapLocation;

	//��������� ������������� ����������
	CUIString m_text;
};