///////////////////////////////////////////////////////////////
// Phrase.h
// �����, ����������� ����� (������� �������)
///////////////////////////////////////////////////////////////

#pragma once


#include "ui/uistring.h"
#include "ui/xrXMLParser.h"



class CPhrase
{
public:
	CPhrase(void);
	virtual ~CPhrase(void);

/*	int				GetIndex()			{return m_iIndex;}
	LPCSTR			GetText()			{return m_text.GetBuf();}

	//�������� ��������� ����������� �� �����
	void			Load(int index);
	void			GetQuestion(SInfoQuestion& question, int question_num);
	//������� �������� �� XML
	void			LoadInfoPortionFromXml(CUIXml& uiXml, int num_in_file);


protected:
	bool m_bLoaded;

	//���������� ������ � ������ ���� ��������� ������� ����������
	int	m_iIndex;
	//��������� ������������� ����������
	CUIString m_text;

	DEFINE_MAP(ref_str,	CPhrase*, PHRASE_MAP, PHRASE_MAP_IT);
	//������ ����, ������� ���������� ���������� ���������
	static PHRASE_MAP m_PhrasesMap;
	*/
}