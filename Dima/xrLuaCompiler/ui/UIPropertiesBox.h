// UIPropertiesBox.h: 
//
// ����� � ��������� ��� ������ ��������, ���������� �� ������� ������ 
// ������� ����
//////////////////////////////////////////////////////////////////////

#pragma once


#include "uiframewindow.h"
#include "uilistwnd.h"


class CUIPropertiesBox: public CUIFrameWindow
{
private:
	typedef CUIFrameWindow inherited; 
public:
	////////////////////////////////////
	//�����������/����������
	CUIPropertiesBox();
	virtual ~CUIPropertiesBox();


	////////////////////////////////////
	//�������������
	virtual void Init(LPCSTR base_name, int x, int y, int width, int height);


	//���������, ������������ ������������� ����
	typedef enum{PROPERTY_CLICKED} E_MESSAGE;

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	bool AddItem(char*  str, void* pData = NULL, int value = 0);
	void RemoveItem(int index);
	void RemoveAll();

	void Show(int x, int y);
	void Hide();

	//��������� ������ �������� �������� ��� -1 ����
	//������ �������� �� ����
	int GetClickedIndex();
	//���������� ������� ������� ��� NULL ���� ��� ������
	CUIListItem* GetClickedItem();

	//�������������� ��������� ������ � ����������� �� ����������
	//���������
	void AutoUpdateHeight();

protected:
	CUIListWnd m_UIListWnd;

	int m_iClickedElement;
};