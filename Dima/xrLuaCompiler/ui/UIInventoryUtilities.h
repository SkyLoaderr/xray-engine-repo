// CUIInventoryUtilities.h:  ������� ������� ��� ������ �
// ���������� ������ ���������, �������� � �.�.
//////////////////////////////////////////////////////////////////////



#include "..\inventory.h"
#include "UIDragDropItem.h"
#include "..\\UIStaticItem.h"


#define INV_GRID_WIDTH	50
#define INV_GRID_HEIGHT	50

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


};