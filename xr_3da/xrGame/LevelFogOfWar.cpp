// LevelFogOfWar.cpp:  для карты уровня в одиночном режиме игры
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LevelFogOfWar.h"

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
	//Fbox level_box = Level().ObjectSpace.GetBoundingVolume();
	Fbox level_box;
	level_box.x2 = 359.843f;
	level_box.x1 = -280.157f;
	level_box.z2 = 253.36f;
	level_box.z1 = -386.64f;

	
	m_fMapWidth = level_box.x2 - level_box.x1;
	m_fMapHeight = level_box.z2 - level_box.z1;
	m_fMapLeft = level_box.x1;
	m_fMapTop = level_box.z1;

	m_iFogWidth = int(m_fMapWidth/FOG_CELL_WIDTH);
	if(m_fMapWidth - float(m_fMapWidth)*FOG_CELL_WIDTH>0.f) m_iFogWidth++;
	
	m_iFogHeight = int(m_fMapHeight/FOG_CELL_HEIGHT);
	if(m_fMapHeight - float(m_iFogHeight)*FOG_CELL_HEIGHT>0.f) m_iFogHeight++;

    
	m_vFogCells.clear();

	for(int i=0; i<m_iFogWidth*m_iFogHeight; i++)
	{
		SFogOfWarCell fog_cell;
		fog_cell.opened = 0;
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

	int x = int((world_pos.x - m_fMapLeft)/FOG_CELL_WIDTH);
	int y = int((world_pos.y - m_fMapTop)/FOG_CELL_HEIGHT);

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

	for(y=view_radius; y>=0; y -= FOG_CELL_HEIGHT)
		for(x=view_radius; x>=0; x -= FOG_CELL_WIDTH)
			OpenCell(world_pos.x + x, world_pos.z + y);

	for(y= -view_radius; y<=0; y += FOG_CELL_HEIGHT)
		for(x= -view_radius; x<=0; x += FOG_CELL_WIDTH)
			OpenCell(world_pos.x + x, world_pos.z + y);

	for(y=view_radius; y>=0; y -= FOG_CELL_HEIGHT)
		for(x= -view_radius; x<=0; x += FOG_CELL_WIDTH)
			OpenCell(world_pos.x + x, world_pos.z + y);

	for(y= -view_radius; y<=0; y += FOG_CELL_HEIGHT)
		for(x=view_radius; x>=0; x -= FOG_CELL_WIDTH)
		OpenCell(world_pos.x + x, world_pos.z + y);


}

void CFogOfWar::OpenCell(float x, float y)
{
	Fvector2 pos;
	Ivector2 grid_pos;
	pos.x = x;
	pos.y = y;
	
	if(GetFogCell(pos, grid_pos).opened) return;
	
	GetFogCell(grid_pos.x, grid_pos.y).opened = 1;

	//изменить вид клеточки тумана для соседей с открывшейся (чтоб было плавно)
	if(grid_pos.x>0) GetFogCell(grid_pos.x-1, grid_pos.y).shape |= FOG_OPEN_RIGHT;
	if(grid_pos.y>0) GetFogCell(grid_pos.x, grid_pos.y-1).shape |= FOG_OPEN_UP;
	if(grid_pos.x<m_iFogWidth-1) GetFogCell(grid_pos.x+1, grid_pos.y).shape |= FOG_OPEN_LEFT;
	if(grid_pos.y<m_iFogHeight-1)GetFogCell(grid_pos.x, grid_pos.y+1).shape |= FOG_OPEN_DOWN;
}

void CFogOfWar::GetFogCellWorldPos(int x, int y, Fvector& world_pos)
{
	world_pos.x = float(x)*FOG_CELL_WIDTH + m_fMapLeft;  
	world_pos.y = 0;
	world_pos.z = float(y)*FOG_CELL_HEIGHT + m_fMapTop + FOG_CELL_HEIGHT;
}