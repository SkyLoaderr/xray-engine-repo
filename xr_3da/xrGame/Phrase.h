///////////////////////////////////////////////////////////////
// Phrase.h
// �����, ����������� ����� (������� �������)
///////////////////////////////////////////////////////////////

#pragma once


#include "ui/uistring.h"
#include "ui/xrXMLParser.h"

class CPhraseDialog;

class CPhrase
{
private:
	friend CPhraseDialog;
public:
	CPhrase(void);
	virtual ~CPhrase(void);

	void			SetText		(LPCSTR text)		{m_text.SetText(text);}
	LPCSTR			GetText		()	const			{return m_text.GetBuf();}

	void			SetIndex	(int index)			{m_iIndex = index;}
	int				GetIndex	()	const			{return m_iIndex;}

	int				GoodwillLevel()	const			{return m_iGoodwillLevel;}

	DEFINE_VECTOR(ref_str, ACTION_NAME_VECTOR, ACTION_NAME_VECTOR_IT);
	const ACTION_NAME_VECTOR&	ScriptActions ()	{return m_ScriptActions;}

protected:
	//���������� ������ � ������ ���� �������
	int	m_iIndex;
	//��������� ������������� �����
	CUIString m_text;
	//����������� ������� ���������������, ����������� ��� ����
	//���� ����� ����� ���� �������
	int m_iGoodwillLevel;
	
	//���������� ��������, ������� ������������ ����� ���� ��� 
	//��������� �����
	ACTION_NAME_VECTOR m_ScriptActions;
};