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
#include "UIScriptWnd.h"

//////////////////////////////////////////////////////////////////////////
class CUIGlobalMap: public CUIDialogWndEx{
	Frect			m_Box;
	Irect			m_MinimizedRect;
	Irect			m_NormalRect;
	enum EState{
		stNone,
		stMinimized,
		stNormal,
		stMaximized
	};
	EState			m_State;
	void			SwitchTo			(EState new_state);
	void			OnBtnMinimizedClick ();
	void			OnBtnMaximizedClick ();
public:
					CUIGlobalMap		();
	virtual			~CUIGlobalMap		();
	
	virtual void	Init				();
};

class CUILocalMap: public CUIStatic{
	Frect			m_LevelBox;
public:
					CUILocalMap			();
	virtual			~CUILocalMap		();
};
DEFINE_MAP(shared_str,CUILocalMap*,GameMaps,GameMapsPairIt);

class CUIMapWnd: public CUIDialogWndEx
{
	typedef CUIDialogWndEx inherited;

	CUIGlobalMap*	m_GlobalMap;
	GameMaps		m_GameMaps;
public:
					CUIMapWnd				();
	virtual			~CUIMapWnd				();

	virtual void	Init					();
	virtual void	Show					(bool status);

	void			InitGlobalMapObjectives	(){}
	void			InitLocalMapObjectives	(){}

	void			SetActivePoint			(const Fvector &vNewPoint){}
	virtual void	Draw					();
};

/*
class CGameObject;

class CUIMapWnd: public CUIWindow
{
private:
	typedef CUIWindow inherited;
private:
	// ������� ���������� ����� � �������� ��������
	float				m_GlobalMapWidth;
	float				m_GlobalMapHeight;

	// ����������� �������� �������� �������������� �� ������-������������� �� ���������� �����.
	// �� ���������� ������� �������������� ������������ �����, � ���� ��� �����, �� ������ �����
	// �� ���������� (�.�. ��� ������� ������ ������� ����� �� �������)
	RECT				m_rDefault;
public:
	// ������ �����
	enum EMapModes
	{
		emmGlobal,
		emmLocal
	};
protected:
	//global map
	ref_shader			m_GlobalMapShader;

	//�������� ����������
	CUIFrameWindow		UIMainMapFrame;
	//�������� PDA
	CUIFrameLineWnd		UIPDAHeader;

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
	// ��� ��������� ����� � ������ ������ ����
	CUIStatic			UILocalMapName;

	// Current map
	CUIMapBackground*	m_pCurrentMap;
	CUIScrollBar		UIMapBgndV,
		UIMapBgndH;
	//������ ������
	CUIMapSpot*			m_pActorSpot;
	// ������ ������������ ����������/��������� �����
	CUIButton			UIMapTypeSwitch;
	// ������� �����
	EMapModes			m_eCurrentMode;
	//�������� ��� �����
	CUIStaticItem		landscape;

	//������ � ������ ����� � �������� �� ������
	int					m_iMapWidth;
	int					m_iMapHeight;

	//���������� ������� �����
	float				m_fWorldMapWidth;
	float				m_fWorldMapHeight;
	float				m_fWorldMapLeft;
	float				m_fWorldMapTop;

	// ������ ��������� ���� ��� ����������
	DEF_VECTOR			(Objectives, shared_str);

	// ��������� ��������� ���� �� ����������
	struct LocalMapDef
	{
		// ������ ��������� ����� �� ����������
		typedef boost::shared_ptr<CUIGlobalMapLocation>	GMLptr;
		GMLptr			mapSpot;
		// ������ �� ������ �����
		Objectives		obj;
		// ��������� ����� �� ������ ������������ �� ���������� �����
		CUIFrameWindow*	smallFrame;
		// ���� �������� �����
		ref_shader		mapTexture;
		// ���� ��������� ���� �����
		bool			visible;
		// Real Level box
		Frect			level_box;

		LocalMapDef()
			:smallFrame	(NULL),
			mapTexture	(NULL),
			visible		(false)
		{
			level_box.set(0,0,0,0);
		}
	};
	// ������������� ������ ���������� ��������� ���� (���->������)
	DEFINE_MAP			(shared_str, LocalMapDef, LocalMaps, LocalMapsPairIt);
	LocalMaps			m_LocalMaps;
protected:
	// ����������� �������� ����� �� ������ � ������� �� ������
	void				ConvertToLocal			(const Fvector& src, Ivector2& dest);
	void				AddObjectSpot			(CGameObject* pGameObject);

	// ������������� ����. ����������� 1 ��� ��� �������� ����
	void				InitMaps				();
	void				AddLocalMap				(const shared_str& leveName, const Frect &l, const Frect &g, shared_str textureName);
	void				DeleteLocalMapsData		();
	// ������������� ��������� �����.����������� ��� ������ ������.
	void				SetLocalMap				(const shared_str &levelName);
	// ��������� ������ �� ������������, ��� ������ �������������� �� ������ �����
	void				ApplyFilterToObjectives	(const shared_str &levelName);
public:
						CUIMapWnd				();
	virtual				~CUIMapWnd				();

	virtual void		Init					();
	virtual void		Show					(bool status);
	virtual void		SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	virtual void		Draw					();
	virtual void		Update					();
	virtual void		OnMouseWheel			(int direction);

	// ���������� ����� ������� �� �����, �� ������ ��� ����� ������������
	void				SetActivePoint			(const Fvector &vNewPoint);

	// ����������� ����� �����
	void				SwitchMapMode			(EMapModes mode);

	void				InitGlobalMapObjectives	();
	void				InitLocalMapObjectives	();
};
*/