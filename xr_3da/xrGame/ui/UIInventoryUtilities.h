// CUIInventoryUtilities.h:  ������� ������� ��� ������ �
// ���������� ������ ���������, �������� � �.�.
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../inventory_item.h"
#include "UIStatic.h"
#include "../character_info_defs.h"

//////////////////////////////////////////////////////////////////////////

//������� ����� � �������� ���������
#define INV_GRID_WIDTH			50
#define INV_GRID_HEIGHT			50

//������� ����� � �������� ������ ����������
#define ICON_GRID_WIDTH			64
#define ICON_GRID_HEIGHT		64
//������ ������ ��������� ��� ��������� � ��������
#define CHAR_ICON_WIDTH			2
#define CHAR_ICON_HEIGHT		2	

//������ ������ ��������� � ������ ����
#define CHAR_ICON_FULL_WIDTH	2
#define CHAR_ICON_FULL_HEIGHT	8	

#define TRADE_ICONS_SCALE		(4.f/5.f)

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
//shader �� ������ �����
ref_shader& GetMapIconsShader();
// shader �� ������ ���������� � ������������
ref_shader&	GetMPCharIconsShader();
//������� ��� �������
void DestroyShaders();

void ClearDragDrop (DD_ITEMS_VECTOR& dd_item_vector);

// �������� �������� ������� � ��������� ����

// �������� ������������� �������� GetGameDateTimeAsString ��������: �� �����, �� �����, �� ������
enum ETimePrecision
{
	etpTimeToHours = 0,
	etpTimeToMinutes,
	etpTimeToSeconds,
	etpTimeToMilisecs
};

// �������� ������������� �������� GetGameDateTimeAsString ��������: �� ����, �� ������, �� ���
enum EDatePrecision
{
	edpDateToDay,
	edpDateToMonth,
	edpDateToYear
};

const shared_str GetGameDateAsString(EDatePrecision datePrec, char dateSeparator = '/');
const shared_str GetGameTimeAsString(ETimePrecision timePrec, char timeSeparator = ':');
const shared_str GetDateAsString(ALife::_TIME_ID time, EDatePrecision datePrec, char dateSeparator = '/');
const shared_str GetTimeAsString(ALife::_TIME_ID time, ETimePrecision timePrec, char timeSeparator = ':');

// ���������� ���, ������� ����� �����
void UpdateWeight(CUIStatic &wnd, bool withPrefix = false);

// ������� ��������� ������-�������������� ����� � ��������� �� �� ��������� ��������������
LPCSTR GetRankAsText		(CHARACTER_RANK rankID);
LPCSTR GetReputationAsText	(CHARACTER_REPUTATION rankID);

// Internal namespace for some necessary helper functions
namespace Private
{
	typedef					std::pair<CHARACTER_RANK, shared_str>	CharInfoStringID;
	DEF_MAP					(CharInfoStrings, CHARACTER_RANK, shared_str);
	extern CharInfoStrings	*charInfoReputationStrings;
	extern CharInfoStrings	*charInfoRankStrings;

	void					InitCharacterInfoStrings	();
	void					ClearCharacterInfoStrings	();
	void					LoadStrings					(CharInfoStrings *container, LPCSTR section, LPCSTR field);
}

};