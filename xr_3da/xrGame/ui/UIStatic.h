// UIStatic.h: ����� ������������ ��������
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_STATIC_H_
#define _UI_STATIC_H_

#pragma once


#include "uiwindow.h"
#include "uistring.h"


#include "../uistaticitem.h"





class CUIStatic : public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIStatic();
	virtual ~ CUIStatic();


	virtual void Init(LPCSTR tex_name, int x, int y, int width, int height);
	virtual void Init(int x, int y, int width, int height);
	
	//���������� ����
	virtual void Draw();
	//���������� ����� �����������
	virtual void Update();


	void SetText(LPSTR str);
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

	//��������� ����� �����������, ��� ��� ������ ��
	//������� ������������� ����
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

protected:
	bool m_bClipper;	
	
	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	bool m_bAvailableTexture;
	CUIStaticItem m_UIStaticItem;


	//�����
	LPSTR m_str;
	STRING m_sEdit;


	/////////////////////////////////////
	//��������������� ����� ������
	/////////////////////////////////////
	void WordOut();
	void AddLetter(char letter);
	u32 ReadColor(int pos, int& r, int& g, int& b);
	
	//��������� ������� �������
	int curretX;
	int curretY;
	//��������� �����
	int outX;
	int outY;

	//����� � ������� ������������ ��� ����������������� ������
	xr_vector<char> buf_str;
	u32 str_len;

	bool new_word;
	int word_length;
	
	int space_width;
	int word_width;

	CGameFont::EAligment m_eTextAlign;
	
};

#endif // _UI_STATIC_H_
