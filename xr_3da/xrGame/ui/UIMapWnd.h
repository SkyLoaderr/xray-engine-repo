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
#include "UIFrameLineWnd.h"

#include "UIMapSpot.h"
#include "UIMapBackground.h"
#include "UIGlobalMapLocation.h"

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
	virtual void InitLocalMap();
	virtual void InitGlobalMap();
	virtual void InitMaps();
	virtual void Show();

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Draw();
	virtual void Update();

	// ���������� ����� ������� �� �����, �� ������ ��� ����� ������������
	void SetActivePoint(const Fvector &vNewPoint);

	// ����������� ����� �����
	enum EMapModes { emmGlobal, emmLocal };
	void SwitchMapMode(const EMapModes mode);
	void AddGlobalMapLocation(float x, float y, int width, int height);

protected:

	void ConvertToLocal(const Fvector& src, Ivector2& dest);

	void AddObjectSpot(CGameObject* pGameObject);

	//�������� ����������
	CUIFrameWindow		UIMainMapFrame;

	//�������� PDA
	CUIFrameLineWnd		UIPDAHeader;
	
	CUICheckButton		UICheckButton1;
	CUICheckButton		UICheckButton2;
	CUICheckButton		UICheckButton3;
	CUICheckButton		UICheckButton4;
		
	//������ � ����������� ��� ��������� �����
	CUIStatic			UIStaticInfo;
	//���������� � ���������
	CUICharacterInfo	UICharacterInfo;

	// ����� � ����������
	CUIMapBackground	UILocalMapBackground;
	CUIMapBackground	UIGlobalMapBackground;
	// ���������� � ������� �� ���������� �����
	CUIStatic			UIMapName;
	CUIListWnd			UIMapGoals;

	// Current map
	CUIMapBackground	*m_pCurrentMap;
	CUIScrollBar		UIMapBgndV, UIMapBgndH;
	//������ ������
	CUIMapSpot*			m_pActorSpot;
	// ������ ������������ ����������/��������� �����
	CUIButton			UIMapTypeSwitch;
	// ������� �����
	EMapModes			m_eCurrentMode;

	//�������� ��� �����
	CUIStaticItem		landscape;

	//������ � ������ ����� � �������� �� ������
	int m_iMapWidth;
	int m_iMapHeight;

	//���������� ������� �����
	float m_fWorldMapWidth;
	float m_fWorldMapHeight;
	float m_fWorldMapLeft;
	float m_fWorldMapTop;

	// ������ ��������� ���� ��� ����������
	DEF_VECTOR(MapLocations, CUIGlobalMapLocation*);
	MapLocations	m_MapLocations;
};
