// CUIMapWnd.h:  диалог итерактивной карты
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
			bool	NeedShowPointer					(Irect r);
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
	// Размеры глобальной карты в условных единицах
	float				m_GlobalMapWidth;
	float				m_GlobalMapHeight;

	// Стандартное значение размеров прямоугольника на кнопке-переключателе на глобальную карту.
	// Мы сравниваем размеры прамоугольника показываемой карты, и если они равны, то данную карту
	// не показываем (т.к. для данного левела размеры карты не заданны)
	Irect				m_rDefault;
public:
	// Режимы карты
	enum EMapModes
	{
		emmGlobal,
		emmLocal
	};
protected:
	//global map
	ref_shader			m_GlobalMapShader;

	//элементы интерфейса
	CUIFrameWindow		UIMainMapFrame;
	//верхушка PDA
	CUIFrameLineWnd		UIPDAHeader;

	//окошко с информацией для локальной карты
	CUIStatic			UIStaticInfo;
	//информация о пресонаже
	CUICharacterInfo	UICharacterInfo;

	// Карта и скроллбары
	CUIMapBackground	UILocalMapBackground;
	CUIMapBackground	UIGlobalMapBackground;
	// Информация о локации на глобальной карте
	CUIStatic			UIMapName;
	CUIListWnd			UIMapGoals;
	// Имя локальной карты в нижнем правом углу
	CUIStatic			UILocalMapName;

	// Current map
	CUIMapBackground*	m_pCurrentMap;
	CUIScrollBar		UIMapBgndV,
		UIMapBgndH;
	//иконка актера
	CUIMapSpot*			m_pActorSpot;
	// Кнопка переключения глобальная/локальная карта
	CUIButton			UIMapTypeSwitch;
	// Текущий режим
	EMapModes			m_eCurrentMode;
	//подложка для карты
	CUIStaticItem		landscape;

	//ширина и высота карты в пикселях на экране
	int					m_iMapWidth;
	int					m_iMapHeight;

	//координаты мировой карты
	float				m_fWorldMapWidth;
	float				m_fWorldMapHeight;
	float				m_fWorldMapLeft;
	float				m_fWorldMapTop;

	// Список локальных карт для глобальной
	DEF_VECTOR			(Objectives, shared_str);

	// Описатель локальных карт на глобальной
	struct LocalMapDef
	{
		// Объект локальной карты на глобальной
		typedef boost::shared_ptr<CUIGlobalMapLocation>	GMLptr;
		GMLptr			mapSpot;
		// Заданя на данной карте
		Objectives		obj;
		// Отдельный фрейм на кнопке переключения на глобальную карту
		CUIFrameWindow*	smallFrame;
		// Сама тектсура карты
		ref_shader		mapTexture;
		// Флаг видимости этой карты
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
	// Ассоциативный массив описателей локальных карт (имя->данные)
	DEFINE_MAP			(shared_str, LocalMapDef, LocalMaps, LocalMapsPairIt);
	LocalMaps			m_LocalMaps;
protected:
	// Конвертация кординат карты из метров в пиксели на экране
	void				ConvertToLocal			(const Fvector& src, Ivector2& dest);
	void				AddObjectSpot			(CGameObject* pGameObject);

	// Инициализация карт. Выполняется 1 раз при загрузке игры
	void				InitMaps				();
	void				AddLocalMap				(const shared_str& leveName, const Frect &l, const Frect &g, shared_str textureName);
	void				DeleteLocalMapsData		();
	// Инициализация локальной карты.Выполняется при каждом показе.
	void				SetLocalMap				(const shared_str &levelName);
	// Применить фильтр на маплокейшины, для показа соответсвующих на нужной карте
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

	// Установить новую позицию на карте, по кторой она будет отцентрована
	void				SetActivePoint			(const Fvector &vNewPoint);

	// Переключить режим карты
	void				SwitchMapMode			(EMapModes mode);

	void				InitGlobalMapObjectives	();
	void				InitLocalMapObjectives	();
};
*/