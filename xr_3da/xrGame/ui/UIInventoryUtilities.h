// CUIInventoryUtilities.h:  ������� ������� ��� ������ �
// ���������� ������ ���������, �������� � �.�.
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../inventory_item.h"
#include "../UIStaticItem.h"

//������� ����� � �������� ���������
#define INV_GRID_WIDTH		50
#define INV_GRID_HEIGHT		50

//������� ����� � �������� ������ ����������
#define ICON_GRID_WIDTH		64
#define ICON_GRID_HEIGHT	64
//������ ������ ��������� ��� ��������� � ��������
#define CHAR_ICON_WIDTH		2
#define CHAR_ICON_HEIGHT	2	

//������ ������ ��������� � ������ ����
#define CHAR_ICON_FULL_WIDTH	2
#define CHAR_ICON_FULL_HEIGHT	8	




class CUIDragDropItem;


namespace InventoryUtilities
{

//���������� �������� �� ������������ ����������� ��� � �������
//��� ����������
bool GreaterRoomInRuck(PIItem item1, PIItem item2);
//��� �������� ���������� �����
bool FreeRoom(TIItemList item_list, int width, int height);

//��� �������� �� ������� � �������
void AmmoUpdateProc(CUIDragDropItem* pItem);
//��� �������� �� ������� � ����
void FoodUpdateProc(CUIDragDropItem* pItem);

//�������� shader �� ������ ���������
ref_shader& GetEquipmentIconsShader();
//shader �� ������ ����������
ref_shader& GetCharIconsShader();
// shader �� ������ ���������� � ������������
ref_shader&	GetMPCharIconsShader();

};