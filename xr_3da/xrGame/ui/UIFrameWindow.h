// UIFrameWindow.h: 
//
// ���� �������������� ����. ����� ����� CUIFrameRect
//////////////////////////////////////////////////////////////////////

#pragma once


#include "uiwindow.h"
#include "uistatic.h"

#include "../uiframerect.h"
#include "../uistaticitem.h"





class CUIFrameWindow: public CUIWindow
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
	virtual void Init(LPCSTR base_name, RECT* pRect);

	//��� ����������� ��������, ������������� ����
	void InitLeftTop(LPCSTR tex_name, int left_offset, int up_offset);
	void InitLeftBottom(LPCSTR tex_name, int left_offset, int up_offset);

	virtual void SetWidth(int width);
	virtual void SetHeight(int height);

	////////////////////////////////////
	//���������� ����
	virtual void Draw();
	

	//����� ���������
	CUIStatic UITitleText;

protected:

	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	
	//�������� ����� 
	CUIFrameRect m_UIWndFrame;
	
	//��������� ������ ����

	bool m_bOverLeftTop; 
	bool m_bOverLeftBottom;

	int m_iLeftTopOffset;
	int m_iUpTopOffset;

	int m_iLeftBottomOffset;
	int m_iUpBottomOffset;

	CUIStaticItem m_UIStaticOverLeftTop;
	CUIStaticItem m_UIStaticOverLeftBottom;
};