// CUIMapBackground.h:  перемещаемая картинка (сам ландшафт)
//						интреактивной карты
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "UIMapSpot.h"



class CUIMapBackground: public CUIButton
{
private:
	typedef CUIButton inherited;
public:
	CUIMapBackground();
	virtual ~CUIMapBackground();

	virtual void Init(int x, int y, int width, int height);
	virtual void InitMapBackground();
	virtual void Draw();
	virtual void Update();



	void ConvertToLocal(const Fvector& src, Ivector2& dest);
	void ConvertFromLocalToMap(int x, int y, Fvector2& dest);
	void ConvertToTexture(const Fvector& src, Fvector2& dest);
	void ConvertToTexture(float x, float y, Fvector2& dest);

	//ширина и высота участка карты в метрах
	float m_fMapViewWidthMeters;
	float m_fMapViewHeightMeters;

	//ширина и высота карты в пикселях на экране
	int m_iMapViewWidthPixels;
	int m_iMapViewHeightPixels;

	//положение и размеры всей карты
	float m_fMapWidthMeters;
	float m_fMapHeightMeters;
	float m_fMapLeftMeters;
	float m_fMapTopMeters;
	float m_fMapBottomMeters;

	//текущее положение верхнего левого угла карты
	//в метрах от начала координат 
	//(0,0)- left top
	float m_fMapX;
	float m_fMapY;

	//вектор обозначений на карте
	DEFINE_VECTOR	(CUIMapSpot*, MAP_SPOT_VECTOR, MAP_SPOT_VECTOR_IT);
	MAP_SPOT_VECTOR m_vMapSpots;

protected:
	void DrawFogOfWar();
	void DrawFogOfWarCell(int x, int y);
	
	//обновления иконок на карте
	void UpdateMapSpots();
	void UpdateActorPos();
	//подложка для карты
	CUIStaticItem landscape;
	CUIStaticItem fog_of_war;

	CUIMapSpot m_fogOfWarCell;


	//для перетаскивания карты при помощи мыши
	void OnMouse(int x, int y, E_MOUSEACTION mouse_action);
	int m_iOldMouseX;
	int m_iOldMouseY;
	POINT m_previousPos;


};
