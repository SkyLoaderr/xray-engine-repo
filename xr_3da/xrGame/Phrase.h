///////////////////////////////////////////////////////////////
// Phrase.h
// �����, ����������� ����� (������� �������)
///////////////////////////////////////////////////////////////

#pragma once


#include "ui/uistring.h"
#include "ui/xrXMLParser.h"

#include "PhraseScript.h"

class CPhraseDialog;
class CGameObject;

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

protected:
	//���������� ������ � ������ ���� �������
	int	m_iIndex;
	//��������� ������������� �����
	CUIString m_text;
	
	//����������� ������� ���������������, ����������� ��� ����
	//���� ����� ����� ���� �������
	int m_iGoodwillLevel;
	
	//��� ������ ���������� �������
	CPhraseScript m_PhraseScript;

};