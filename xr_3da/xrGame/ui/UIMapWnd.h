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
#include "UIMapBackground.h"
#include "UIGlobalMapLocation.h"

#include "UICharacterInfo.h"

class CGameObject;

class CUIMapWnd: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUIMapWnd();
	virtual ~CUIMapWnd();

	virtual void Init();
	virtual void InitLocalMap();
	virtual void InitGlobalMap();
	virtual void InitMaps();
	virtual void Show(bool status);

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Draw();
	virtual void Update();

	// Установить новую позицию на карте, по кторой она будет отцентрована
	void SetActivePoint(const Fvector &vNewPoint);

	// Переключить режим карты
	enum EMapModes { emmGlobal, emmLocal };
	void SwitchMapMode(const EMapModes mode);
	void AddGlobalMapLocation(const ref_str mapName, const Ivector4 &v);

protected:

	void ConvertToLocal(const Fvector& src, Ivector2& dest);

	void AddObjectSpot(CGameObject* pGameObject);

	//элементы интерфейса
	CUIFrameWindow		UIMainMapFrame;

	//верхушка PDA
	CUIFrameLineWnd		UIPDAHeader;
	
	CUICheckButton		UICheckButton1;
	CUICheckButton		UICheckButton2;
	CUICheckButton		UICheckButton3;
	CUICheckButton		UICheckButton4;
		
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

	// Current map
	CUIMapBackground	*m_pCurrentMap;
	CUIScrollBar		UIMapBgndV, UIMapBgndH;
	//иконка актера
	CUIMapSpot*			m_pActorSpot;
	// Кнопка переключения глобальная/локальная карта
	CUIButton			UIMapTypeSwitch;
	// Текущий режим
	EMapModes			m_eCurrentMode;

	//подложка для карты
	CUIStaticItem		landscape;

	//ширина и высота карты в пикселях на экране
	int m_iMapWidth;
	int m_iMapHeight;

	//координаты мировой карты
	float m_fWorldMapWidth;
	float m_fWorldMapHeight;
	float m_fWorldMapLeft;
	float m_fWorldMapTop;

	// Список локальных карт для глобальной
	DEF_VECTOR(Objectives, ref_str);

	struct GlobalMapLocationDef
	{
		CUIGlobalMapLocation*	l;
		Objectives				obj;

		GlobalMapLocationDef() : l(NULL) {}
	};

	DEF_MAP(MapLocations, ref_str, GlobalMapLocationDef);
	MapLocations	m_MapLocations;

	// Global map locations frame name
	ref_str m_sGlobalMapLocFrameName;

	void InitGlobalMapLocationObjectives();
};
