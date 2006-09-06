///////////////////////////////////////////////////////////////
// Phrase.h
// �����, ����������� ����� (������� �������)
///////////////////////////////////////////////////////////////

#pragma once

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

	void			SetText			(LPCSTR text)		{m_text = text;}
	LPCSTR			GetText			()	const;
//	LPCSTR			GetScriptText	(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2, LPCSTR dialog_id, int phrase_num) const;

	void			SetIndex		(int index)			{m_iIndex = index;}
	int				GetIndex		()	const			{return m_iIndex;}

	int				GoodwillLevel	()	const			{return m_iGoodwillLevel;}

	bool			IsDummy			()	const;
	CPhraseScript*	GetPhraseScript	()					{return &m_PhraseScript;};

protected:
	//���������� ������ � ������ ���� �������
	int				m_iIndex;
	//��������� ������������� �����
	xr_string		m_text;
	
	//����������� ������� ���������������, ����������� ��� ����
	//���� ����� ����� ���� �������
	int				m_iGoodwillLevel;
	
	//��� ������ ���������� �������
	CPhraseScript	m_PhraseScript;
};