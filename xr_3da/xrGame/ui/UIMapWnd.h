// CUIMapWnd.h:  ������ ������������ �����
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"

#include "UIButton.h"
#include "UICheckButton.h"

#include "UIListWnd.h"
#include "UIFrameWindow.h"

#include "UIMapSpot.h"
#include "UIMapBackground.h"

#include "UICharacterInfo.h"

class CGameObject;

class CUIMapWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUIMapWnd();
	virtual ~CUIMapWnd();

	virtual void Init();
	virtual void InitMap();
	virtual void Show();

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Draw();
	virtual void Update();

	// ���������� ����� ������� �� �����, �� ������ ��� ����� ������������
	void SetActivePoint(const Fvector &vNewPoint);

protected:

	void DrawMap();
	void ConvertToLocal(const Fvector& src, Ivector2& dest);

	void RemoveAllSpots();
	void AddObjectSpot(CGameObject* pGameObject);

	//�������� ����������
//	CUIFrameWindow	UIMainMapFrame;

//	CUIStatic UIStaticTop;
//	CUIStatic UIStaticBottom;

	//�������� PDA ��� �������� ������������ ������
//	CUIStatic			UIPDAHeader;
	
	CUICheckButton UICheckButton1;
	CUICheckButton UICheckButton2;
	CUICheckButton UICheckButton3;
	CUICheckButton UICheckButton4;
		
	//������ � �����������
	CUIStatic UIStaticInfo;
	//���������� � ���������
	CUICharacterInfo UICharacterInfo;

	CUIMapBackground UIMapBackground;

	//�������� ��� �����
	CUIStaticItem	landscape;

	//������ � ������ ����� � �������� �� ������
	int m_iMapWidth;
	int m_iMapHeight;

	//���������� ������� �����
	float m_fWorldMapWidth;
	float m_fWorldMapHeight;
	float m_fWorldMapLeft;
	float m_fWorldMapTop;
};
