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
#include "UIGlobalMapLocation.h"

#include "UICharacterInfo.h"
#include "UIWndCallback.h"

class CUIMapWnd;
class CUIGlobalMapSpot;

class CUICustomMap : public CUIStatic, public CUIWndCallback{
	
	shared_str		m_name;
	Frect			m_BoundRect;// real map size (meters)
	float			m_zoom_factor;
	
public:
					CUICustomMap					();
	virtual			~CUICustomMap					();
	virtual void	SetActivePoint					(const Fvector &vNewPoint);

	virtual void	Init							(shared_str name, CInifile& gameLtx);
	Ivector2		ConvertRealToLocal				(const Fvector2& src);// meters->pixels (relatively own left-top pos)
	Ivector2		ConvertRealToLocalNoTransform	(const Fvector2& src);// meters->pixels (relatively own left-top pos)

	bool			GetPointerTo					(const Ivector2& src, int item_radius, Ivector2& pos, float& heading);//position and heading for drawing pointer to src pos

	void			FitToWidth						(u32 width);
	void			FitToHeight						(u32 height);
	float			GetCurrentZoom					(){return m_zoom_factor;}
	const Frect&    BoundRect						()const					{return m_BoundRect;};
	virtual void	OptimalFit						(const Irect& r);
	virtual	void	MoveWndDelta					(const Ivector2& d);

	shared_str		MapName							() {return m_name;}
	virtual CUIGlobalMapSpot*	GlobalMapSpot		() {return NULL;}
	virtual LPCSTR	GetHint							(); 

	virtual void	SetZoomFactor					(float z);

	virtual void	Draw							();
	virtual void	Update							();

			bool	IsRectVisible					(Irect r);
protected:
	virtual void	UpdateSpots						() {};
};


class CUIGlobalMap: public CUICustomMap{
	typedef  CUICustomMap inherited;
public:
	enum EState{
		stNone,
		stMinimized,
		stNormal,
		stMaximized
	};
private:
	shared_str		m_prev_active_map;
	CUIButton		m_maximized_btn;
	CUIButton		m_minimized_btn;
	Ivector2		m_maximized_btn_offset;
	Ivector2		m_minimized_btn_offset;

	Ivector2		m_MinimizedSize;
	Ivector2		m_NormalSize;
	EState			m_State;
	void			OnBtnMinimizedClick		();
	void			OnBtnMaximizedClick		();
	CUIMapWnd*		m_mapWnd;
public:
	virtual void	SetActivePoint			(const Fvector &vNewPoint){};//do nothing
	void			SwitchTo				(EState new_state);

	virtual void	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);

					CUIGlobalMap			(CUIMapWnd*	pMapWnd);
	virtual			~CUIGlobalMap			();
	
	virtual void	Init					(shared_str name, CInifile& gameLtx);
	virtual void	Draw					();
	virtual void	Update					();

	CUIMapWnd*		MapWnd					() {return m_mapWnd;}
};

class CUILevelMap;
class CUIGlobalMapSpot: public CUIWindow
{
	typedef CUIWindow inherited;

	CUILevelMap*				m_owner_map;
public:
								CUIGlobalMapSpot	(CUICustomMap* m);
	virtual						~CUIGlobalMapSpot	();


	virtual void				Init				(u32 color);
	virtual void				OnMouse				(int x, int y, EUIMessages mouse_action);
	virtual void				Update				();
	virtual void				Draw				();
	virtual LPCSTR				GetHint				();
protected:
	CUIFrameWindow				UIBorder;
};

class CUILevelMap: public CUICustomMap{
	typedef  CUICustomMap inherited;

	Frect						m_GlobalRect;			// virtual map size (meters)
	CUIGlobalMapSpot			m_globalMapSpot;		//rect on the global map
public:
								CUILevelMap			();
	virtual						~CUILevelMap		();
	virtual void				Init				(shared_str name, CInifile& gameLtx);
	const Frect&				GlobalRect			() const								{return m_GlobalRect;}
	virtual CUIGlobalMapSpot*	GlobalMapSpot		()										{return &m_globalMapSpot;}
protected:
	virtual void				UpdateSpots			();

};

class CUIMiniMap: public CUICustomMap{
	typedef  CUICustomMap inherited;

public:
								CUIMiniMap			();
	virtual						~CUIMiniMap			();
	virtual void				Init				(shared_str name, CInifile& gameLtx);
	virtual	void				MoveWndDelta		(const Ivector2& d);
protected:
	virtual void				UpdateSpots			();

};


DEFINE_MAP(shared_str,CUICustomMap*,GameMaps,GameMapsPairIt);

class CUIMapWnd: public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	enum lmFlags{lmMouseHold = 1,};
	Flags32						m_flags;


	CUICustomMap*				m_activeLevelMap;

	CUIGlobalMap*				m_GlobalMap;
	GameMaps					m_GameMaps;

	CUIFrameWindow				m_UIMainFrame;
	CUIScrollBar				m_UIMainScrollV,	m_UIMainScrollH;
	CUIStatic					m_UILevelFrame;

	CUIFrameLineWnd				UIMainMapHeader;
	void						OnScrollV				();
	void						OnScrollH				();
	void						UpdateScroll			();
public:
								CUIMapWnd				();
	virtual						~CUIMapWnd				();

	virtual void				Init					();
	virtual void				Show					(bool status);
	virtual void				Draw					();
	virtual void				Update					();
			void				ShowHint				();

	virtual void				OnMouse					(int x, int y, EUIMessages mouse_action);
	virtual void				SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	void						InitGlobalMapObjectives	();
	void						InitLocalMapObjectives	();

	void						SetActivePoint			(const Fvector &vNewPoint);
	void						SetActiveMap			(shared_str level_name);
	CUICustomMap*				ActiveLevelMap			() {return m_activeLevelMap;};

	void						SetStatusInfo			(LPCSTR status_text);
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
	Irect				m_rDefault;
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