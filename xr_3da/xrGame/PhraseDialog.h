///////////////////////////////////////////////////////////////
// PhraseDialog.h
// Класс диалога - общения при помощи фраз 2х персонажей в игре
///////////////////////////////////////////////////////////////

#pragma once


#include "phrase.h"
#include "graph_manager.h"


typedef CGraphAbstract<CPhrase*, float, u32, u32> CPhraseGraph;
typedef CGraphManagerAbstract<CPhrase*, float, u32, u32> CPhraseGraphManagerAbstract;

class CPhraseDialog	:public CPhraseGraphManagerAbstract
{
private:
	typedef CPhraseGraphManagerAbstract inherited;
public:
	CPhraseDialog(void);
	virtual ~CPhraseDialog(void);

	//загрузка диалога из XML файла
	virtual void Load(LPCSTR xml_file);

protected:
	//однонаправленый граф фраз
	//описывает все возможные варианты развития диалога
	CPhraseGraph m_PhraseGraph;
};