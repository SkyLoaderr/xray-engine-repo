// LevelFogOfWar.h:  для карты уровня в одиночном режиме игры
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"


//клеточка, которыми покрывается уровень;
//для "тумана войны" над картой
typedef struct tagSFogOfWarCell 
{
	bool IsOpened() {return 0xFF == shape;}
	void Open()	{shape = 0xFF;}

	//вид клеточки - зависит от количества открытых
	//соседей и их расположения
	//0		- полностью закрыта
	//0xFF	- полность открыта
	unsigned char shape;
} SFogOfWarCell;

//флаги, опряделяющие форму клеточки тумна
const static unsigned char FOG_OPEN_UP			= 0x01;
const static unsigned char FOG_OPEN_DOWN		= 0x02;
const static unsigned char FOG_OPEN_LEFT		= 0x04;
const static unsigned char FOG_OPEN_RIGHT		= 0x08;
const static unsigned char FOG_OPEN_UP_LEFT		= 0x10;
const static unsigned char FOG_OPEN_DOWN_LEFT	= 0x20;
const static unsigned char FOG_OPEN_DOWN_RIGHT	= 0x40;
const static unsigned char FOG_OPEN_UP_RIGHT	= 0x80;




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


	//размеры одной клеточки тумана в метрах
	static float FOG_CELL_SIZE;
	//радиус вокруг актрера, где убирается туман
	static float ACTOR_FOG_REMOVE_RADIUS;
	
protected:
	void OpenCell(float x, float y);
	void OpenCell(int x, int y);

	//положение и размеры всей карты уровня
	float m_fMapWidth;
	float m_fMapHeight;
	float m_fMapLeft;
	float m_fMapTop;

};


