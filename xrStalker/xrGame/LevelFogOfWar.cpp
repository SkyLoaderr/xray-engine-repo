// LevelFogOfWar.cpp:  для карты уровня в одиночном режиме игры
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LevelFogOfWar.h"


float CFogOfWar::FOG_CELL_SIZE = 20.f;
float CFogOfWar::ACTOR_FOG_REMOVE_RADIUS = 25.f;



CFogOfWar::CFogOfWar()
{
	m_iFogWidth = 0;
	m_iFogHeight = 0;

	m_vFogCells.clear();
}

CFogOfWar::~CFogOfWar()
{
	m_vFogCells.clear();
}

//инициализация тумана, вызывается при загрузке уровня
void CFogOfWar::Init()
{
	//загрузить параметры
	FOG_CELL_SIZE = pSettings->r_float("game_map","fog_cell_size");
	ACTOR_FOG_REMOVE_RADIUS = pSettings->r_float("game_map","actor_fog_remove_radius");

	Fbox level_box;
	if (Level().pLevel->section_exist("level_map"))	
	{
		level_box.x1 = Level().pLevel->r_float("level_map","x1");
		level_box.z1 = Level().pLevel->r_float("level_map","z1");
		level_box.x2 = Level().pLevel->r_float("level_map","x2");
		level_box.z2 = Level().pLevel->r_float("level_map","z2");
	}
	else
	{
		level_box.x1 = level_box.z1 = level_box.x2 = level_box.z2 = 0.f;
	}
	
	m_fMapWidth = level_box.x2 - level_box.x1;
	m_fMapHeight = level_box.z2 - level_box.z1;
	m_fMapLeft = level_box.x1;
	m_fMapTop = level_box.z1;

	m_iFogWidth = int(m_fMapWidth/FOG_CELL_SIZE);
	if(m_fMapWidth - float(m_fMapWidth)*FOG_CELL_SIZE>0.f) ++m_iFogWidth;
	
	m_iFogHeight = int(m_fMapHeight/FOG_CELL_SIZE);
	if(m_fMapHeight - float(m_iFogHeight)*FOG_CELL_SIZE>0.f) ++m_iFogHeight;

    
	m_vFogCells.clear();

	//чтоб не было глюков, когда размер карты не задан
	if(m_iFogWidth<=0) m_iFogWidth = 1;
	if(m_iFogHeight<=0) m_iFogHeight = 1;

	for(int i=0; i<m_iFogWidth*m_iFogHeight; ++i)
	{
		SFogOfWarCell fog_cell;
		fog_cell.shape = 0;

		m_vFogCells.push_back(fog_cell);
	}

}

SFogOfWarCell& CFogOfWar::GetFogCell(int x, int y)
{
	R_ASSERT2(!m_vFogCells.empty(), "addressing FogOfWar array element before intialization");

	return m_vFogCells[x + m_iFogWidth*y];
}


//определить клеточку тумана по точке в пространстве уровня
SFogOfWarCell& CFogOfWar::GetFogCell(const Fvector2& world_pos,	Ivector2& grid_pos)
{
	R_ASSERT2(!m_vFogCells.empty(), "addressing FogOfWar array element before intialization");

	int x = int((world_pos.x - m_fMapLeft)/FOG_CELL_SIZE);
	int y = int((world_pos.y - m_fMapTop)/FOG_CELL_SIZE);

	grid_pos.x = x;
	grid_pos.y = y;

	if(grid_pos.x<0) grid_pos.x = 0;
	if(grid_pos.y<0) grid_pos.y = 0;
	if(grid_pos.x>m_iFogWidth-1) grid_pos.x = m_iFogWidth-1;
	if(grid_pos.y>m_iFogHeight-1) 
		grid_pos.y = m_iFogHeight-1;

	return m_vFogCells[grid_pos.x + m_iFogWidth*grid_pos.y];
}

SFogOfWarCell& CFogOfWar::GetFogCell(const Fvector& world_pos, Ivector2& grid_pos)
{
	Fvector2 pos;
	pos.x = world_pos.x;
	pos.y = world_pos.z;
	return GetFogCell(pos, grid_pos);
}

void CFogOfWar::UpdateFog(const Fvector& world_pos, float view_radius)
{
	float x,y;

	//открыть прямоугольник вокруг персонажа
	OpenCell(world_pos.x,world_pos.z);

	for(y=view_radius; y>=0; y -= FOG_CELL_SIZE)
		for(x=view_radius; x>=0; x -= FOG_CELL_SIZE)
			OpenCell(world_pos.x + x, world_pos.z + y);

	for(y= -view_radius; y<=0; y += FOG_CELL_SIZE)
		for(x= -view_radius; x<=0; x += FOG_CELL_SIZE)
			OpenCell(world_pos.x + x, world_pos.z + y);

	for(y=view_radius; y>=0; y -= FOG_CELL_SIZE)
		for(x= -view_radius; x<=0; x += FOG_CELL_SIZE)
			OpenCell(world_pos.x + x, world_pos.z + y);

	for(y= -view_radius; y<=0; y += FOG_CELL_SIZE)
		for(x=view_radius; x>=0; x -= FOG_CELL_SIZE)
		OpenCell(world_pos.x + x, world_pos.z + y);


}


void CFogOfWar::OpenCell(float x, float y)
{
	Fvector2 pos;
	Ivector2 grid_pos;
	pos.x = x;
	pos.y = y;

	GetFogCell(pos, grid_pos);
	OpenCell(grid_pos.x, grid_pos.y);
}
void CFogOfWar::OpenCell(int x, int y)
{
	Ivector2 grid_pos;
	grid_pos.x = x;
	grid_pos.y = y;
	
	if(GetFogCell(grid_pos.x, grid_pos.y).IsOpened()) return;
	
	GetFogCell(grid_pos.x, grid_pos.y).Open();

	
	//изменить вид клеточки тумана для соседей с открывшейся (чтоб было плавно)
	if(grid_pos.x>0) 
	{
		SFogOfWarCell& fog_cell = GetFogCell(grid_pos.x-1, grid_pos.y);
		fog_cell.shape |= FOG_OPEN_RIGHT;
		if(fog_cell.shape & FOG_OPEN_LEFT) OpenCell(grid_pos.x-1, grid_pos.y);
	}
	if(grid_pos.y>0)
	{
		SFogOfWarCell& fog_cell = GetFogCell(grid_pos.x, grid_pos.y-1);
		fog_cell.shape |= FOG_OPEN_UP;
		if(fog_cell.shape & FOG_OPEN_DOWN) OpenCell(grid_pos.x, grid_pos.y-1);
	}
	if(grid_pos.x<m_iFogWidth-1)
	{
		SFogOfWarCell& fog_cell = GetFogCell(grid_pos.x+1, grid_pos.y);
		fog_cell.shape |= FOG_OPEN_LEFT;
		if(fog_cell.shape & FOG_OPEN_RIGHT) OpenCell(grid_pos.x+1, grid_pos.y);
	}
	if(grid_pos.y<m_iFogHeight-1)
	{
		SFogOfWarCell& fog_cell = GetFogCell(grid_pos.x, grid_pos.y+1);
		fog_cell.shape |= FOG_OPEN_DOWN;
		if(fog_cell.shape & FOG_OPEN_UP) OpenCell(grid_pos.x, grid_pos.y+1);
	}

	if(grid_pos.x>0 && grid_pos.y>0) GetFogCell(grid_pos.x-1, grid_pos.y-1).shape |= FOG_OPEN_DOWN_RIGHT;
	if(grid_pos.x>0 && grid_pos.y<m_iFogHeight-1) GetFogCell(grid_pos.x-1, grid_pos.y+1).shape |= FOG_OPEN_UP_RIGHT;
	if(grid_pos.x<m_iFogWidth-1 && grid_pos.y>0) GetFogCell(grid_pos.x+1, grid_pos.y-1).shape |= FOG_OPEN_DOWN_LEFT;
	if(grid_pos.x<m_iFogWidth-1 && grid_pos.y<m_iFogHeight-1) GetFogCell(grid_pos.x+1, grid_pos.y+1).shape |= FOG_OPEN_UP_LEFT;

}

void CFogOfWar::GetFogCellWorldPos(int x, int y, Fvector& world_pos)
{
	world_pos.x = float(x)*FOG_CELL_SIZE + m_fMapLeft;  
	world_pos.y = 0;
	world_pos.z = float(y)*FOG_CELL_SIZE + m_fMapTop + FOG_CELL_SIZE;
}