// UIOutfitSlot.h:  ���� ������� � ������� ���������
// 
//////////////////////////////////////////////////////////////////////

#include "uidragdroplist.h"

#pragma once

class CUIOutfitSlot: public CUIDragDropList
{
private:
	typedef CUIDragDropList inherited;
public:
	CUIOutfitSlot();
	virtual ~CUIOutfitSlot();

	virtual void Init(int x, int y, int width, int height);
	virtual void OnMouse(int x, int y, EUIMessages mouse_action);

	virtual void AttachChild(CUIWindow *pChild);
	virtual void DetachChild(CUIWindow *pChild);

	virtual void DropAll();
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	virtual void Draw();
	virtual void Update();

	// ��������� ����������� ��������� ��� �����������
	void SetOriginalOutfit();

	// ����������� ��������� �� ������� ������. NULL ���� ���.
	CUIDragDropItem * GetCurrentOutfit();

	// ��� ������������ ��������� ���������� ���������� ������ ����������
	void	SetMPOutfit();

//	typedef enum{UNDRESS_OUTFIT = 6000, OUTFIT_RETURNED_BACK} E_OUTFIT_ACTION;

protected:
	//������ � ����������� �������� � ������ ����
	CUIStatic UIOutfitIcon;

	int m_iNoOutfitX;
	int m_iNoOutfitY;
};