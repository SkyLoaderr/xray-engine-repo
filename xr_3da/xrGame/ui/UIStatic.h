// UIStatic.h: класс статического элемента
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_STATIC_H_
#define _UI_STATIC_H_

#pragma once


#include "uiwindow.h"
#include "uistring.h"


#include "../uistaticitem.h"


#define RGB_ALPHA(a, r, g ,b)  ((u32) (((u8) (b) | ((u16) (g) << 8)) | (((u32) (u8) (r)) << 16)) | (((u32) (u8) (a)) << 24)) 


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


	void SetText(LPCSTR str);
	LPCSTR GetText() {return m_str;}

	virtual void SetTextAlign(CGameFont::EAligment align) {m_eTextAlign = align;}
	CGameFont::EAligment GetTextAlign() {return m_eTextAlign;}

	void SetColor(u32 color) {m_UIStaticItem.SetColor(color);}
	
	void InitTexture(LPCSTR tex_name);
	CUIStaticItem* GetStaticItem() {return &m_UIStaticItem;}

	virtual void ClipperOn();
	virtual void ClipperOff();
	virtual void ClipperOff(CUIStaticItem& UIStaticItem);
	virtual bool GetClipperState() {return m_bClipper;}

	//отсечение части изображение, при его выходе за
	//пределы родительского окна
	void TextureClipper(int offset_x = 0, 
						int offset_y = 0,
						RECT* pClipRect = NULL);

	void TextureClipper(int offset_x, 
						int offset_y,
						RECT* pClipRect, 
						CUIStaticItem& UIStaticItem);

	virtual void	SetTextureScale		(float new_scale);
	virtual float	GetTextureScale		();
	
	void	SetShader			(const ref_shader& sh);
	CUIStaticItem& GetUIStaticItem() {return m_UIStaticItem;}

	void SetTextX(int text_x) {m_iTextOffsetX = text_x;}
	void SetTextY(int text_y) {m_iTextOffsetY = text_y;}
	int GetTextX() {return m_iTextOffsetX;}
	int GetTextY() {return m_iTextOffsetY;}

	void SetTextColor(u32 color) {m_dwFontColor = color;} 
	u32 GetTextColor() {return m_dwFontColor;}

	void SetStretchTexture(bool stretch_texture) {m_bStretchTexture = stretch_texture;}
	bool GetStretchTexture() {return m_bStretchTexture;}

	Irect GetClipRect();

protected:
	bool m_bClipper;
	//растягивание текстуры в видимую область
	bool m_bStretchTexture;
	
	///////////////////////////////////////	
	//Графический интрефейс для рисования
	///////////////////////////////////////
	bool m_bAvailableTexture;
	CUIStaticItem m_UIStaticItem;


	//текст
	LPSTR m_str;
	STRING m_sEdit;

	//цвет текста
	u32 m_dwFontColor;

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

	//смещение текста,  в зависимости от выбранного
	//метода центровки
	int m_iTextOffsetX;
	int m_iTextOffsetY;

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
