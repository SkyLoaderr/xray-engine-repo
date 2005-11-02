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
	using CUIWindow::Draw;
	////////////////////////////////////
	//�����������/����������
	CUIFrameWindow();
	virtual ~CUIFrameWindow();

	////////////////////////////////////
	//�������������
	virtual void Init(LPCSTR base_name, float x, float y, float width, float height);
	virtual void Init(float x, float y, float width, float height);
	virtual void Init(LPCSTR base_name, Frect* pRect);

	virtual void InitTexture(const char* texture);
			void SetTextureColor(u32 color)										{m_UIWndFrame.SetTextureColor(color);}
//	virtual void SetRect2Item(int item, int x, int y, int width, int height);

	//��� ����������� ��������, ������������� ����
	void InitLeftTop(LPCSTR tex_name, float left_offset, float up_offset);
	void InitLeftBottom(LPCSTR tex_name, float left_offset, float up_offset);

	virtual void SetWidth(float width);
	virtual void SetHeight(float height);
	
	// ������������� ���� ����� ������
	void SetColor(u32 cl);

	////////////////////////////////////
	//���������� ����
	virtual void Draw();
	virtual void Update();
	
	//����� ���������
	CUIStatic UITitleText;
	CUIStatic*	GetTitleStatic(){return &UITitleText;};
	void		SetVisiblePart	(CUIFrameRect::EFramePart p, BOOL b)	{m_UIWndFrame.SetVisiblePart(p,b);};

protected:

	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	
	//�������� ����� 
	CUIFrameRect m_UIWndFrame;

	void		FrameClip(const Frect parentAbsR);
	
	//��������� ������ ����

	bool m_bOverLeftTop; 
	bool m_bOverLeftBottom;

	float m_iLeftTopOffset;
	float m_iUpTopOffset;

	float m_iLeftBottomOffset;
	float m_iUpBottomOffset;

	CUIStaticItem m_UIStaticOverLeftTop;
	CUIStaticItem m_UIStaticOverLeftBottom;

private:
	inline void ClampMax_Zero(Frect &r);

};
