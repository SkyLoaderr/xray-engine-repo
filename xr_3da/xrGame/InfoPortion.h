///////////////////////////////////////////////////////////////
// InfoPortion.h
// ��������, �������������� �������� ����������
///////////////////////////////////////////////////////////////


#pragma once


#include "ui/uistring.h"
#include "ui/xrXMLParser.h"



#define NO_INFO_INDEX (-1)

//��������� ��� ������� �� �����
struct SMapLocation
{
	SMapLocation() 
	{
		level_num = 0;
		x = y = 0;
		name.SetText("");
		text.SetText("");
		
		attached_to_object = false;
		object_id = 0xffff;
	}

	//����� ������
	int level_num;
	//���������� �� ������
	float x;
	float y;
	//��� �������
	CUIString name;
	//����� ��������
	CUIString text;
	
	//������������ �� ������� � �������
	bool attached_to_object;
	//��� ������� �� ������
	u16 object_id;

	//������� � ��������� ������
	int icon_x, icon_y, icon_width, icon_height;

};


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
DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);

//�����  - ������ ����������
class CInfoPortion 
{
public:
	CInfoPortion(void);
	virtual ~CInfoPortion(void);

	int				GetIndex()			{return m_iIndex;}
	int				GetLocationsNum()	  {return m_MapLocationVector.size();}
	SMapLocation&	GetLocation(int index) {return	m_MapLocationVector[index];}
	char*			GetText()		{return m_text.GetBuf();}

	//�������� ��������� ����������� �� �����
	void			Load(int index);
	void			GetQuestion(SInfoQuestion& question, int question_num);
	//������� �������� �� XML
	void			LoadInfoPortionFromXml(CUIXml& uiXml, int num_in_file);

	//������ ��������, ������� ����� ����� �������� ����� ��������� 
	//����������
	INFO_QUESTIONS_LIST m_QuestionsList;
	

protected:
	bool m_bLoaded;

	//���������� ������ � ������ ���� ��������� ������� ����������
	int	m_iIndex;
	//������ ������� �� �����
	LOCATIONS_VECTOR	m_MapLocationVector;
	//��������� ������������� ����������
	CUIString m_text;
};