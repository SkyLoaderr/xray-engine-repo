// LevelFogOfWar.h:  для карты уровня в одиночном режиме игры
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"


//клеточка, которыми покрывается уровень;
//для "тумана войны" над картой
typedef struct tagSFogOfWarCell 
{
	//если 1- то ячейка уже "увиденна" актером
    char opened:1;
	//вид клеточки - зависит от количества открытых
	//соседей и их расположения
	char shape:7;
} SFogOfWarCell;

//флаги, опряделяющие форму клеточки тумна
const static unsigned char FOG_OPEN_UP = 0x01;
const static unsigned char FOG_OPEN_DOWN = 0x02;
const static unsigned char FOG_OPEN_LEFT = 0x04;
const static unsigned char FOG_OPEN_RIGHT = 0x08;

//размеры одной клеточки тумана в метрах
const static float FOG_CELL_WIDTH = 20.f;
const static float FOG_CELL_HEIGHT = 20.f;
//радиус вокруг актрера, где убирается туман
const static float ACTOR_FOG_REMOVE_RADIUS = 25.f;

class CLevel;


class CFogOfWar
{
public:
		
	CFogOfWar();
	virtual ~CFogOfWar();

	//инициализация "тумана войны", вызывается при загрузке уровня
	void Init();
	//обновление тумана в зависимости от позиции персонажа
	//и дальности его видимости 
	void UpdateFog(const Fvector& world_pos, float view_radius);

	SFogOfWarCell& GetFogCell(int x, int y);
	SFogOfWarCell& GetFogCell(const Fvector2& world_pos,
		                      Ivector2& grid_pos);
	SFogOfWarCell& GetFogCell(const Fvector& world_pos,
		                      Ivector2& grid_pos);

	void GetFogCellWorldPos(int x, int y, Fvector& world_pos);


	DEFINE_VECTOR(SFogOfWarCell, FOG_VECTOR, FOG_VECTOR_IT);
	//клеточки, покрывающие карту
	FOG_VECTOR m_vFogCells; 

	//размеры (в клеточках) тумана над уровенем
	int m_iFogWidth;
	int m_iFogHeight;


protected:
	void OpenCell(float x, float y);

	//положение и размеры всей карты уровня
	float m_fMapWidth;
	float m_fMapHeight;
	float m_fMapLeft;
	float m_fMapTop;
};


