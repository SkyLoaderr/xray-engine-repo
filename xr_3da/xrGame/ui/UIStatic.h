// UIStatic.h: класс статического элемента
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_STATIC_H_
#define _UI_STATIC_H_

#pragma once

#include "uilines.h"
#include "uiwindow.h"
#include "uistring.h"
#include "../uistaticitem.h"
#include "../script_export_space.h"
#include "../../gamefont.h"

#define RGB_ALPHA(a, r, g ,b)  ((u32) (((u8) (b) | ((u16) (g) << 8)) | (((u32) (u8) (r)) << 16)) | (((u32) (u8) (a)) << 24)) 

class CUIFrameWindow;
class CLAItem;
class CUIXml;

class CUIStatic : public CUIWindow, public CUISingleTextureOwner  
{
	friend class CUIXmlInit;
	friend class CUI3tButton;
private:
	typedef CUIWindow inherited;
	CLAItem*				m_lanim;
	float					m_lainm_start_time;
public:

					CUIStatic				();
	virtual			~CUIStatic				();

	// IUISimpleWindow
	virtual void	Init					(float x, float y, float width, float height);
	virtual void	SetWndRect				(float x, float y, float width, float height);
	virtual void	Draw					();
	virtual void	Update					();

	// IUISingleTextureOwner
	virtual void		CreateShader(const char* tex, const char* sh = "hud\\default");
	virtual ref_shader& GetShader();
	virtual void		SetTextureColor(u32 color);
	virtual u32			GetTextureColor() const;
	virtual void		SetOriginalRect(const Frect& r)							{m_UIStaticItem.SetOriginalRect(r);}
	virtual void		SetOriginalRectEx(const Frect& r)						{m_UIStaticItem.SetOriginalRectEx(r);}
	//
			void		SetOriginalRect(float x, float y, float width, float height)	{m_UIStaticItem.SetOriginalRect(x,y,width,height);}
			void		SetHeadingPivot(const Fvector2& p){m_UIStaticItem.SetHeadingPivot(p);}

	void			SetLightAnim			(LPCSTR lanim);
	virtual void	Init					(LPCSTR tex_name, float x, float y, float width, float height);	
			void	InitEx					(LPCSTR tex_name, LPCSTR sh_name, float x, float y, float width, float height);

	virtual void	DrawTexture				();
	virtual void	DrawText				();

	virtual void	OnFocusReceive			();
	virtual void	OnFocusLost				();


	static void		SetText					(LPCSTR str, STRING &arr);
	virtual void	SetText					(LPCSTR str);
	virtual void	SetLines				(LPCSTR str);
	LPCSTR			GetText					()								{return m_lines.GetText();}

	virtual void SetTextAlign				(CGameFont::EAligment align)	{m_eTextAlign = align; m_lines.SetTextAlignment(align);}
	CGameFont::EAligment GetTextAlign		()								{return m_eTextAlign;}

	void		SetTextAlign_script			(u32 align)						{ m_eTextAlign = (CGameFont::EAligment)align;	}
	u32			GetTextAlign_script			()								{ return static_cast<u32>(m_eTextAlign);		}

	virtual void SetColor					(u32 color)						{ m_UIStaticItem.SetColor(color);		}
	u32			GetColor					() const						{ return m_UIStaticItem.GetColor();		}
	// Получения цвета по референсу используется для анимации
	u32&		GetColorRef					()								{ return m_UIStaticItem.GetColorRef();	}
    
	//
	virtual void	InitTexture(LPCSTR tex_name);
//	virtual void
//	virtual void	InitSharedTexture(LPCSTR xml_file, LPCSTR texture);
//	virtual void	InitSharedTexture(LPCSTR xml_file, LPCSTR texture, bool owner = false);
	virtual void	InitTextureEx(LPCSTR tex_name, LPCSTR sh_name="hud\\default");
	CUIStaticItem*	GetStaticItem			()								{return &m_UIStaticItem;}

	virtual void ClipperOn					();
	virtual void ClipperOff					();
	virtual void ClipperOff					(CUIStaticItem& UIStaticItem);
	virtual bool GetClipperState			()								{return m_bClipper;}

	//отсечение части изображение, при его выходе за
	//пределы родительского окна
	void TextureClipper						(float offset_x = 0, float offset_y = 0,Frect* pClipRect = NULL);
	void TextureClipper						(float offset_x, float offset_y, Frect* pClipRect, CUIStaticItem& UIStaticItem);

	
	void			SetShader				(const ref_shader& sh);
	CUIStaticItem&	GetUIStaticItem			()						{return m_UIStaticItem;}

	virtual	void SetTextX					(float text_x)			{m_iTextOffsetX = text_x;}
	virtual	void SetTextY					(float text_y)			{m_iTextOffsetY = text_y;}
	virtual	void SetTextPos					(float x, float y)		{SetTextX(x); SetTextY(y);}
			float GetTextX					()						{return m_iTextOffsetX;}
			float GetTextY					()						{return m_iTextOffsetY;}

	virtual void SetTextColor				(u32 color)				{ m_dwFontColor = color; m_lines.SetTextColor(color);} 
			void SetTextColor_script	(int a, int r, int g, int b){SetTextColor(color_argb(a,r,g,b));}
			u32  GetTextColor				() const				{ return m_dwFontColor; }
			u32  &GetTextColorRef			()						{ return m_dwFontColor; }
	virtual void SetFont					(CGameFont* pFont);

	void		SetStretchTexture			(bool stretch_texture)	{m_bStretchTexture = stretch_texture;}
	bool		GetStretchTexture			()						{return m_bStretchTexture;}

	void		SetClipRect					(Frect r);
	Frect		GetSelfClipRect				();
	Frect		GetClipperRect				();

	// Работа с маской
	void SetMask							(CUIFrameWindow *pMask);
	// Cмещение текстуры кнопки
	virtual void SetTextureOffset			(float x, float y)		{ m_iTexOffsetX = x; m_iTexOffsetY = y; }
	Fvector2	GetTextureOffeset			() const				{ Fvector2 v; return v.set(m_iTexOffsetX, m_iTexOffsetY); }
	// Анализируем текст на помещаемость его по длинне в заданную ширину, и если нет, то всталяем 
	// "\n" реализуем таким образом wordwrap
	static void PreprocessText				(STRING &str, float width, CGameFont *pFont);
	// Функция вывода текста
	void DrawString							(const Frect &rect);
	// Когда текст надписи не влазит в статик, то, иногда, нам необходимо показать троеточие и обрезать
	// надпись. Вот для этого и предназначена эта функция
	enum EElipsisPosition
	{
		eepNone,
		eepBegin,
		eepEnd,
		eepCenter
	};

	void SetElipsis							(EElipsisPosition pos, int indent);

	// Включть/выключить текстуру
	void TextureOn							()						{ m_bTextureEnable = true; }
	void TextureOff							()						{ m_bTextureEnable = false; }
	void TextureAvailable					(bool value)			{ m_bAvailableTexture = value; }

	// performs text length limit :)
	void PerformTextLengthLimit				(int limit = -1);
	// deletes last character in text field
	void DeleteLastCharacter				();
	
	void	SetHeading						(float f)				{m_fHeading = f;};
	float	GetHeading						()						{return m_fHeading;}
	bool	Heading							()						{return m_bHeading;}
	void	EnableHeading					(bool b)				{m_bHeading = b;}

	// will be need by CUI3tButton
	// Don't change order!!!!!
	typedef enum {
		E, // enabled
		D, // disabled
		T, // touched
		H  // highlighted
	} E4States;

	void SetTextColor(u32 color, E4States state);
	CUILines	m_lines;
protected:
	//цвет текста
	u32 m_dwFontColor;
	// this array of color will be useful in CUI3tButton class
	// but we really need to declare it directly there because it must be initialized in CUIXmlInit::InitStatic
	u32  m_dwTextColor[4];
	bool m_bUseTextColor[4]; // note: 0 index will be ignored

	bool m_bClipper;
	bool m_bStretchTexture;

	
	///////////////////////////////////////	
	//Графический интрефейс для рисования
	///////////////////////////////////////
	bool m_bAvailableTexture;
	bool m_bTextureEnable;
	CUIStaticItem m_UIStaticItem;


	/////////////////////////////////////
	//форматированный вывод текста
	/////////////////////////////////////
	void WordOut(const Frect &rect);
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
	float m_iTextOffsetX;
	float m_iTextOffsetY;

	//буфер в который записывается уже отформатированная строка
	xr_vector<char> buf_str;
	u32 str_len;

	bool new_word;
	int word_length;
	
	int space_width;
	int word_width;

	bool			m_bHeading;
	float			m_fHeading;

	CGameFont::EAligment m_eTextAlign;

    // Для вывода текстуры с обрезанием по маске используем CUIFrameWindow
	CUIFrameWindow	*m_pMask;

	// для смещения текстуры
	float m_iTexOffsetX, m_iTexOffsetY;

	// Обрезка надписи
	EElipsisPosition	m_ElipsisPos;
	void Elipsis(const Frect &rect, EElipsisPosition elipsisPos);
	int	m_iElipsisIndent;

	// Clip rect
	Frect	m_ClipRect;

public:
	static void Elipsis(STRING &str, const Frect &rect, EElipsisPosition elipsisPos, CGameFont *pFont);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIStatic)
#undef script_type_list
#define script_type_list save_type_list(CUIStatic)

#endif // _UI_STATIC_H_
