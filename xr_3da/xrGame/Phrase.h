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

/*	int				GetIndex()			{return m_iIndex;}
	LPCSTR			GetText()			{return m_text.GetBuf();}

	//загрузка структуры информацией из файла
	void			Load(int index);
	void			GetQuestion(SInfoQuestion& question, int question_num);
	//функция загрузки из XML
	void			LoadInfoPortionFromXml(CUIXml& uiXml, int num_in_file);


protected:
	bool m_bLoaded;

	//уникальный индекс в списке всех возможных квантов информации
	int	m_iIndex;
	//текстовое представление информации
	CUIString m_text;

	DEFINE_MAP(ref_str,	CPhrase*, PHRASE_MAP, PHRASE_MAP_IT);
	//список фраз, которые становятся доступными персонажу
	static PHRASE_MAP m_PhrasesMap;
	*/
}