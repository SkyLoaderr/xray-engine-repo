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

	LPCSTR			GetText()			{return m_text.GetBuf();}
	int				GetIndex()			{return m_iIndex;}

protected:
	bool m_bLoaded;

	//���������� ������ � ������ ���� �������
	int	m_iIndex;
	//��������� ������������� �����
	CUIString m_text;
};