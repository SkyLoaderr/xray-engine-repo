///////////////////////////////////////////////////////////////
// Phrase.h
// класс, описывающий фразу (элемент диалога)
///////////////////////////////////////////////////////////////

#pragma once


#include "ui/uistring.h"
#include "ui/xrXMLParser.h"

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

	DEFINE_VECTOR(ref_str, ACTION_NAME_VECTOR, ACTION_NAME_VECTOR_IT);
	const ACTION_NAME_VECTOR&	ScriptActions () const	{return m_ScriptActions;}

	DEFINE_VECTOR(ref_str, PRECONDITION_VECTOR, PRECONDITION_VECTOR_IT);
	const ACTION_NAME_VECTOR&	Preconditions () const	{return m_Preconditions;}

protected:
	//уникальный индекс в списке фраз диалога
	int	m_iIndex;
	//текстовое представление фразы
	CUIString m_text;
	//минимальный уровень благосклоггости, необходимый для того
	//чтоб фразу можно было сказать
	int m_iGoodwillLevel;
	
	//скриптовые действия, которые активируется после того как 
	//говорится фраза
	ACTION_NAME_VECTOR m_ScriptActions;
	//список скриптовых предикатов, выполнение, которых необходимо
	//для того чтоб фраза стала доступной
	
	PRECONDITION_VECTOR m_Preconditions;
	//вызывает предикаты для фразы и возвращает результат
	bool IsPhraseAvailable (const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2) const;
};