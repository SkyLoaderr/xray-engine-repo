///////////////////////////////////////////////////////////////
// Phrase.h
// класс, описывающий фразу (элемент диалога)
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
	//уникальный индекс в списке фраз диалога
	int	m_iIndex;
	//текстовое представление фразы
	CUIString m_text;
	
	//минимальный уровень благосклоггости, необходимый для того
	//чтоб фразу можно было сказать
	int m_iGoodwillLevel;
	
	//для вызова скриптовых функций
	CPhraseScript m_PhraseScript;

};