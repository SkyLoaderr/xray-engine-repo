///////////////////////////////////////////////////////////////
// PhraseDialog.h
// Класс диалога - общения при помощи фраз 2х персонажей в игре
///////////////////////////////////////////////////////////////

#pragma once


#include "phrase.h"
#include "graph_manager.h"


typedef CGraphAbstract<CPhrase*, float, u32, u32> CPhraseGraph;
typedef CGraphManagerAbstract<CPhrase*, float, u32, u32> CPhraseGraphManagerAbstract;


struct SPhraseDialogData
{
//////////////////////////////////////////////////////////////////////////
// smart pointer
//////////////////////////////////////////////////////////////////////////
public:
	u16					GetRefCount()   {return ref_count;}
	void				AddRef()		{ref_count++;}
	void				ReleaseRef()	{if(ref_count>0) ref_count--;}
private:
	//счетчик ссылок на объект
	u16					ref_count;


//////////////////////////////////////////////////////////////////////////
// dialog data: данные для представления диалога
//////////////////////////////////////////////////////////////////////////
public:
	//однонаправленый граф фраз
	//описывает все возможные варианты развития диалога
	CPhraseGraph m_PhraseGraph;

	//уникальная строка, идентифицирующая диалог в m_PhraseDialogMap
	ref_str		m_sDialogID;
};



DEFINE_MAP(ref_str,	SPhraseDialogData*, PHRASE_DIALOG_MAP, PHRASE_DIALOG_MAP_IT);



class CPhraseDialog	:public CPhraseGraphManagerAbstract
{
private:
	typedef CPhraseGraphManagerAbstract inherited;
public:
	CPhraseDialog(void);
	virtual ~CPhraseDialog(void);

	//инициализация диалога
	//если диалог с таким id раньше не использовался
	//он будет загружен из файла
	virtual void Init(ref_str dialog_id);

protected:
	//загрузка диалога из XML файла
	virtual void Load(LPCSTR xml_file);

	//глобальное хранилище диалогов, представлено в виде графов фраз
	//информация подгружается только при обращении
	static PHRASE_DIALOG_MAP m_PhraseDialogMap;
};