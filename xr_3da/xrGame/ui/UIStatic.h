// UIStatic.h: класс статического элемента
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_STATIC_H_
#define _UI_STATIC_H_

#pragma once


#include "uiwindow.h"
#include "uistring.h"


#include "..\uistaticitem.h"





class CUIStatic : public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIStatic();
	virtual ~ CUIStatic();


	virtual void Init(LPCSTR tex_name, int x, int y, int width, int height);
	virtual void Init(int x, int y, int width, int height);
	
	//прорисовка окна
	virtual void Draw();
	//обновление перед прорисовкой
	virtual void Update();


	void SetText(LPSTR str);
	LPCSTR GetText() {return m_str;}

	void SetTextAlign(CGameFont::EAligment align) {m_eTextAlign = align;}
	CGameFont::EAligment GetTextAlign() {return m_eTextAlign;}

	void SetColor(u32 color) {m_UIStaticItem.SetColor(color);}
	
	void InitTexture(LPCSTR tex_name);
	CUIStaticItem* GetStaticItem() {return &m_UIStaticItem;}

	void ClipperOn();
	void ClipperOff();
	bool GetClipperState() {return m_bClipper;}

	//отсечение части изображение, при его выходе за
	//пределы родительского окна
	void TextureClipper(int offset_x = 0, 
						int offset_y = 0,
						RECT* pClipRect = NULL);


	void SetTextureScale		(float new_scale);
	float GetTextureScale		();

protected:
	bool m_bClipper;	
	
	///////////////////////////////////////	
	//Графический интрефейс для рисования
	///////////////////////////////////////
	bool m_bAvailableTexture;
	CUIStaticItem m_UIStaticItem;


	//текст
	LPSTR m_str;
	STRING m_sEdit;


	/////////////////////////////////////
	//форматированный вывод текста
	/////////////////////////////////////
	void WordOut();
	void AddLetter(char letter);
	u32 ReadColor(int pos, int& r, int& g, int& b);
	
	//положение пишущей каретки
	int curretX;
	int curretY;
	//выводимый текст
	int outX;
	int outY;

	//буфер в который записывается уже отформатированная строка
	xr_vector<char> buf_str;
	u32 str_len;

	bool new_word;
	int word_length;
	
	int space_width;
	int word_width;

	CGameFont::EAligment m_eTextAlign;
	
};

#endif // _UI_STATIC_H_
