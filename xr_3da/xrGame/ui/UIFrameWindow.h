// UIFrameWindow.h: 
//
// ���� �������������� ����. ����� ����� CUIFrameRect
//////////////////////////////////////////////////////////////////////

#pragma once

#include "uiwindow.h"
#include "uistatic.h"

#include "../uiframerect.h"
#include "../uistaticitem.h"




class CUIFrameWindow: public CUIWindow,
					  public CUIMultiTextureOwner
{
private:
	typedef CUIWindow inherited;
public:
	////////////////////////////////////
	//�����������/����������
	CUIFrameWindow();
	virtual ~CUIFrameWindow();

	////////////////////////////////////
	//�������������
	virtual void Init(LPCSTR base_name, int x, int y, int width, int height);
	virtual void Init(int x, int y, int width, int height);
	virtual void Init(LPCSTR base_name, Irect* pRect);

	virtual void InitTexture(const char* texture);
			void SetTextureColor(u32 color)										{m_UIWndFrame.SetTextureColor(color);}
//	virtual void SetRect2Item(int item, int x, int y, int width, int height);

	//��� ����������� ��������, ������������� ����
	void InitLeftTop(LPCSTR tex_name, int left_offset, int up_offset);
	void InitLeftBottom(LPCSTR tex_name, int left_offset, int up_offset);

	virtual void SetWidth(int width);
	virtual void SetHeight(int height);
	
	// ������������� ���� ����� ������
	void SetColor(u32 cl);

	////////////////////////////////////
	//���������� ����
	virtual void Draw();
	virtual void Update();
	
	//����� ���������
	CUIStatic UITitleText;
	CUIStatic*	GetTitleStatic(){return &UITitleText;};

	bool		GetClipper()							{ return m_bClipper; }
	void		SetClipper(bool value, Irect clipRect)	{ m_bClipper = value; m_ClipRect = clipRect; }

protected:

	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	
	//�������� ����� 
	CUIFrameRect m_UIWndFrame;

	void		FrameClip(const Irect parentAbsR);
	
	//��������� ������ ����

	bool m_bOverLeftTop; 
	bool m_bOverLeftBottom;

	int m_iLeftTopOffset;
	int m_iUpTopOffset;

	int m_iLeftBottomOffset;
	int m_iUpBottomOffset;

	CUIStaticItem m_UIStaticOverLeftTop;
	CUIStaticItem m_UIStaticOverLeftBottom;

	// clipper
	bool	m_bClipper;
	Irect	m_ClipRect;

private:
	inline void ClampMax_Zero(Irect &r);

};
	
//	virtual void SetShader(const ref_shader& sh);
//	virtual void CreateShader(const char* tex, const char*sh = "hud\\default")	{m_UIWndFrame.CreateShader(tex,sh);}
//	virtual ref_shader& GetShader();
//	virtual u32  GetTextureColor()										const	{return m_UIWndFrame.GetTextureColor();}
//	virtual void SetOriginalRect(Irect& r)	{}