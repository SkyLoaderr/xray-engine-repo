// UIStatic.h: ����� ������������ ��������
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_STATIC_H_
#define _UI_STATIC_H_

#pragma once


#include "uiwindow.h"
#include "uistring.h"
#include "../uistaticitem.h"
#include "../script_export_space.h"
#include "../../gamefont.h"

#define RGB_ALPHA(a, r, g ,b)  ((u32) (((u8) (b) | ((u16) (g) << 8)) | (((u32) (u8) (r)) << 16)) | (((u32) (u8) (a)) << 24)) 

class CUIFrameWindow;

class CUIStatic : public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIStatic();
	virtual ~CUIStatic();


	virtual void	Init(LPCSTR tex_name, int x, int y, int width, int height);
	virtual void	Init(int x, int y, int width, int height);
	
	//���������� ����
	virtual void	Draw();
	//���������� ����� �����������
	virtual void	Update();
	virtual void	OnMouse(int x, int y, EUIMessages mouse_action);


	static void SetText(LPCSTR str, STRING &arr);
	virtual void SetText(LPCSTR str);
	LPCSTR GetText() {return m_str;}

	virtual void SetTextAlign(CGameFont::EAligment align) {m_eTextAlign = align;}
	CGameFont::EAligment GetTextAlign() {return m_eTextAlign;}

	void		SetTextAlign_script			(u32 align) { m_eTextAlign = (CGameFont::EAligment)align;	}
	u32			GetTextAlign_script			()			{ return static_cast<u32>(m_eTextAlign);		}

	void		SetColor					(u32 color) { m_UIStaticItem.SetColor(color);		}
	u32			GetColor					() const	{ return m_UIStaticItem.GetColor();		}
	// ��������� ����� �� ��������� ������������ ��� ��������
	u32&		GetColorRef					()			{ return m_UIStaticItem.GetColorRef();	}
	
	virtual void InitTexture(LPCSTR tex_name);
	void SetOriginalRect (int x, int y, int width, int height){m_UIStaticItem.SetOriginalRect(x,y,width,height);};
	CUIStaticItem* GetStaticItem() {return &m_UIStaticItem;}

	virtual void ClipperOn();
	virtual void ClipperOff();
	virtual void ClipperOff(CUIStaticItem& UIStaticItem);
	virtual bool GetClipperState() {return m_bClipper;}

	//��������� ����� �����������, ��� ��� ������ ��
	//������� ������������� ����
	void TextureClipper(int offset_x = 0, 
						int offset_y = 0,
						RECT* pClipRect = NULL);

	void TextureClipper(int offset_x, 
						int offset_y,
						RECT* pClipRect, 
						CUIStaticItem& UIStaticItem);

	virtual void	SetTextureScaleXY		(float new_scale_x, float new_scale_y);
	virtual float	GetTextureScaleX		();
	virtual float	GetTextureScaleY		();
	
	void	SetShader			(const ref_shader& sh);
	CUIStaticItem& GetUIStaticItem() {return m_UIStaticItem;}

	virtual void SetTextX(int text_x) {m_iTextOffsetX = text_x;}
	virtual void SetTextY(int text_y) {m_iTextOffsetY = text_y;}
	int GetTextX() {return m_iTextOffsetX;}
	int GetTextY() {return m_iTextOffsetY;}

	virtual void SetTextColor(u32 color)	{ m_dwFontColor = color; } 
			u32  GetTextColor() const		{ return m_dwFontColor; }
			u32  &GetTextColorRef()			{ return m_dwFontColor; }

	void SetStretchTexture(bool stretch_texture) {m_bStretchTexture = stretch_texture;}
	bool GetStretchTexture() {return m_bStretchTexture;}

	void SetClipRect(RECT r);
	Irect GetClipRect();

	// ������ � ������
	void SetMask(CUIFrameWindow *pMask);
	// C������� �������� ������
	void		SetTextureOffset(int x, int y) { m_iTexOffsetX = x; m_iTexOffsetY = y; }
	Ivector2	GetTextureOffeset() const { Ivector2 v; return v.set(m_iTexOffsetX, m_iTexOffsetY); }
	// ����������� ����� �� ������������ ��� �� ������ � �������� ������, � ���� ���, �� �������� 
	// "\n" ��������� ����� ������� wordwrap
	static void PreprocessText(STRING &str, u32 width, CGameFont *pFont);
	// ������� ������ ������
	void DrawString(const RECT &rect);
	// ����� ����� ������� �� ������ � ������, ��, ������, ��� ���������� �������� ��������� � ��������
	// �������. ��� ��� ����� � ������������� ��� �������
	enum EElipsisPosition
	{
		eepNone,
		eepBegin,
		eepEnd,
		eepCenter
	};

	void SetElipsis(EElipsisPosition pos, int indent);

	// �������/��������� ��������
	void TextureOn()					{ m_bTextureEnable = true; }
	void TextureOff()					{ m_bTextureEnable = false; }
	void TextureAvailable(bool value)	{ m_bAvailableTexture = value; }

	// performs text length limit :)
	void PerformTextLengthLimit(int limit = -1);
	// deletes last character in text field
	void DeleteLastCharacter();

protected:
	bool m_bClipper;
	//������������ �������� � ������� �������
	bool m_bStretchTexture;
	
	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	bool m_bAvailableTexture;
	bool m_bTextureEnable;
	CUIStaticItem m_UIStaticItem;

	//�����
	LPSTR m_str;
	STRING m_sEdit;

	//���� ������
	u32 m_dwFontColor;

	/////////////////////////////////////
	//��������������� ����� ������
	/////////////////////////////////////
	void WordOut(const RECT &rect);
	void AddLetter(char letter);
	u32 ReadColor(int pos, int& r, int& g, int& b);
	
	//��������� ������� �������
	int curretX;
	int curretY;
	//��������� �����
	int outX;
	int outY;

	//�������� ������,  � ����������� �� ����������
	//������ ���������
	int m_iTextOffsetX;
	int m_iTextOffsetY;

	//����� � ������� ������������ ��� ����������������� ������
	xr_vector<char> buf_str;
	u32 str_len;

	bool new_word;
	int word_length;
	
	int space_width;
	int word_width;

	CGameFont::EAligment m_eTextAlign;

    // ��� ������ �������� � ���������� �� ����� ���������� CUIFrameWindow
	CUIFrameWindow	*m_pMask;

	// ��� �������� ��������
	int m_iTexOffsetX, m_iTexOffsetY;

	// ������� �������
	EElipsisPosition	m_ElipsisPos;
	void Elipsis(const RECT &rect, EElipsisPosition elipsisPos);
	int	m_iElipsisIndent;

	// Clip rect
	RECT	m_ClipRect;

public:
	static void Elipsis(STRING &str, const RECT &rect, EElipsisPosition elipsisPos, CGameFont *pFont);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIStatic)
#undef script_type_list
#define script_type_list save_type_list(CUIStatic)

#endif // _UI_STATIC_H_
