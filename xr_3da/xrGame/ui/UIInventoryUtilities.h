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

#define TRADE_ICONS_SCALE (4.f/5.f)

class CUIDragDropItem;
class CUIWpnDragDropItem;

DEFINE_VECTOR(CUIWpnDragDropItem*, DD_ITEMS_VECTOR, DD_ITEMS_VECTOR_IT);


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

void ClearDragDrop (DD_ITEMS_VECTOR& dd_item_vector);

// �������� �������� ������� � ��������� ����

// �������� ������������� �������� GetGameDateTimeAsString ��������: �� �����, �� �����, �� ������
enum ETimePrecision
{
	etpTimeToHours = 0,
	etpTimeToMinutes,
	etpTimeToSeconds,
	etpTimeToMilisecs,
	etpTimeNone
};

// �������� ������������� �������� GetGameDateTimeAsString ��������: �� ����, �� ������, �� ���
enum EDatePrecision
{
	edpDateToDay,
	edpDateToMonth,
	edpDateToYear,
	edpDateNone
};

const ref_str GetGameDateTimeAsString(EDatePrecision datePrec, ETimePrecision timePrec,
									  char dateSeparator = '/', char timeSeparator = ':', char dateTimeSeparatator = ' ');
};