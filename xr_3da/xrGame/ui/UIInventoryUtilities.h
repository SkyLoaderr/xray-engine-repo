// CUIInventoryUtilities.h:  ������� ������� ��� ������ �
// ���������� ������ ���������, �������� � �.�.
//////////////////////////////////////////////////////////////////////



#include "..\inventory.h"
#include "UIDragDropItem.h"


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


};