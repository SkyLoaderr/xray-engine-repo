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

//////////////////////////////////////////////////////////////////////////

class CGameObject;

class CUIMapWnd: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUIMapWnd();
	virtual ~CUIMapWnd();

	virtual void		Init			();
	virtual void		Show			(bool status);
	virtual void		SendMessage		(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	virtual void		Draw			();
	virtual void		Update			();

	// Установить новую позицию на карте, по кторой она будет отцентрована
	void				SetActivePoint	(const Fvector &vNewPoint);

	// Режимы карты
	enum EMapModes
	{
		emmGlobal,
		emmLocal
	};
	// Переключить режим карты
	void				SwitchMapMode	(EMapModes mode);

protected:
	// Конвертация кординат карты из метров в пиксели на экране
	void				ConvertToLocal	(const Fvector& src, Ivector2& dest);
	void				AddObjectSpot	(CGameObject* pGameObject);
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
	// Имя локальной карты в нижнем правом углу
	CUIStatic			UILocalMapName;

	// Current map
	CUIMapBackground	*m_pCurrentMap;
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
		GMLptr					mapSpot;
		// Заданя на данной карте
		Objectives				obj;
		// Отдельный фрейм на кнопке переключения на глобальную карту
		CUIFrameWindow			*smallFrame;
		// Сама тектсура карты
		ref_shader				mapTexture;
		// И ее координаты
		Fvector4				mapDimentions;
		// Флаг видимости этой карты
		bool					visible;

		LocalMapDef()
			:	smallFrame		(NULL),
				mapTexture		(NULL),
				visible			(false)
		{
			mapDimentions.set(0.0f, 0.0f, 0.0f, 0.0f);
		}
	};

	// Ассоциативный массив описателей локальных карт (имя->данные)
	DEF_MAP				(LocalMaps, shared_str, LocalMapDef);
	LocalMaps			m_LocalMaps;

	// Инициализация карт. Выполняется 1 раз при загрузке игры
	void				InitGlobalMap			();
	ref_shader			m_GlobalMapShader;
	void				AddLocalMap				(const shared_str leveName, const Ivector4 &v, const Fvector4 &d, const shared_str &textureName);
	void				InitLocalMaps			();
	void				DeleteLocalMapsData		();
	// Инициализация заданий для отображения их на картах. Выполняется при каждом показе.
	void				InitGlobalMapObjectives	();
	void				InitLocalMapObjectives	();
	// Инициализация локальной карты.Выполняется при каждом показе.
	void				SetLocalMap				(const shared_str &levelName);
	// Применить фильтр на маплокейшины, для показа соответсвующих на нужной карте
	void				ApplyFilterToObjectives	(const shared_str &levelName);

private:
	// Размеры глобальной карты в условных единицах
	float				m_GlobalMapWidth;
	float				m_GlobalMapHeight;

	// Стандартное значение размеров прямоугольника на кнопке-переключателе на глобальную карту.
	// Мы сравниваем размеры прамоугольника показываемой карты, и если они равны, то данную карту
	// не показываем (т.к. для данного левела размеры карты не заданны)
	RECT				m_rDefault;
};
