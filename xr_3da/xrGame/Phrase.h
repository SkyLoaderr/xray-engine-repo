///////////////////////////////////////////////////////////////
// Phrase.h
// класс, описывающий фразу (элемент диалога)
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

	//уникальный индекс в списке фраз диалога
	int	m_iIndex;
	//текстовое представление фразы
	CUIString m_text;
};