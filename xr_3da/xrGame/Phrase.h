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

	void			SetText		(LPCSTR text)		{m_text.SetText(text);}
	LPCSTR			GetText		()					{return m_text.GetBuf();}

	void			SetIndex	(int index)			{m_iIndex = index;}
	int				GetIndex	()					{return m_iIndex;}

protected:
	//���������� ������ � ������ ���� �������
	int	m_iIndex;
	//��������� ������������� �����
	CUIString m_text;
};