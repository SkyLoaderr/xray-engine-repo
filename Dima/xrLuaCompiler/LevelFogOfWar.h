// LevelFogOfWar.h:  ��� ����� ������ � ��������� ������ ����
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"


//��������, �������� ����������� �������;
//��� "������ �����" ��� ������
typedef struct tagSFogOfWarCell 
{
	//���� 1- �� ������ ��� "��������" �������
    char opened:1;
	//��� �������� - ������� �� ���������� ��������
	//������� � �� ������������
	char shape:7;
} SFogOfWarCell;

//�����, ������������ ����� �������� �����
const static unsigned char FOG_OPEN_UP = 0x01;
const static unsigned char FOG_OPEN_DOWN = 0x02;
const static unsigned char FOG_OPEN_LEFT = 0x04;
const static unsigned char FOG_OPEN_RIGHT = 0x08;

//������� ����� �������� ������ � ������
const static float FOG_CELL_WIDTH = 20.f;
const static float FOG_CELL_HEIGHT = 20.f;
//������ ������ �������, ��� ��������� �����
const static float ACTOR_FOG_REMOVE_RADIUS = 25.f;

class CLevel;


class CFogOfWar
{
public:
		
	CFogOfWar();
	virtual ~CFogOfWar();

	//������������� "������ �����", ���������� ��� �������� ������
	void Init();
	//���������� ������ � ����������� �� ������� ���������
	//� ��������� ��� ��������� 
	void UpdateFog(const Fvector& world_pos, float view_radius);

	SFogOfWarCell& GetFogCell(int x, int y);
	SFogOfWarCell& GetFogCell(const Fvector2& world_pos,
		                      Ivector2& grid_pos);
	SFogOfWarCell& GetFogCell(const Fvector& world_pos,
		                      Ivector2& grid_pos);

	void GetFogCellWorldPos(int x, int y, Fvector& world_pos);


	DEFINE_VECTOR(SFogOfWarCell, FOG_VECTOR, FOG_VECTOR_IT);
	//��������, ����������� �����
	FOG_VECTOR m_vFogCells; 

	//������� (� ���������) ������ ��� ��������
	int m_iFogWidth;
	int m_iFogHeight;


protected:
	void OpenCell(float x, float y);

	//��������� � ������� ���� ����� ������
	float m_fMapWidth;
	float m_fMapHeight;
	float m_fMapLeft;
	float m_fMapTop;
};


