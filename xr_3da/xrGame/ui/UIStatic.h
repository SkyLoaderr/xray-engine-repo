// UIStatic.h: класс статического элемента
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_STATIC_H_
#define _UI_STATIC_H_

#pragma once


#include "uiwindow.h"


#include "..\uistaticitem.h"



class CUIStatic : public CUIWindow  
{
public:
	CUIStatic();
	virtual ~ CUIStatic();


	virtual void Init(LPCSTR tex_name, int x, int y, int width, int height);

	//прорисовка окна
	virtual void Draw();
	//обновление перед прорисовкой
	virtual void Update();


	void SetText(LPSTR str) {m_str = str;}
	LPSTR GetText() {return m_str;}


protected:
	

	///////////////////////////////////////	
	//Графический интрефейс для рисования
	///////////////////////////////////////
	CUIStaticItem m_UIStaticItem;


	//текст
	LPSTR m_str;

	
};

#endif // _UI_STATIC_H_
