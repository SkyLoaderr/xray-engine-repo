///////////////////////////////////////////////////////////////
// Phrase.h
// �����, ����������� ����� (������� �������)
///////////////////////////////////////////////////////////////

#pragma once

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

	void			SetText			(LPCSTR text)		{m_text = text;}
	LPCSTR			GetText			()	const;
	LPCSTR			GetScriptText	(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2) const;

	void			SetIndex	(int index)			{m_iIndex = index;}
	int				GetIndex	()	const			{return m_iIndex;}

	int				GoodwillLevel()	const			{return m_iGoodwillLevel;}

	bool			IsDummy()		const;

	shared_str		Sound()			const			{return m_sound;}
	shared_str		Anim()			const			{return m_anim;}

protected:
	//���������� ������ � ������ ���� �������
	int	m_iIndex;
	//��������� ������������� �����
	std::string m_text;
	
	//����������� ������� ���������������, ����������� ��� ����
	//���� ����� ����� ���� �������
	int m_iGoodwillLevel;
	
	//��� ������ ���������� �������
	CPhraseScript m_PhraseScript;

	//��� �������� ����� NPC
	//�������� ����
	shared_str m_sound;
	shared_str m_anim;
};