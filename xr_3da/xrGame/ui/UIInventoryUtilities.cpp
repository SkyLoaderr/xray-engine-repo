// CUIInventoryUtilities.cpp:  ������� ������� ��� ������ �
// ���������� ������ ��������� 
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "UIInventoryUtilities.h"
#include "..\\WeaponAmmo.h"


//�������� ������ ��� ����������
static TIItemList ruck_list;


//��� �������� �� ������� � �������
void InventoryUtilities::AmmoUpdateProc(CUIDragDropItem* pItem)
{
	CWeaponAmmo* pAmmoItem = (CWeaponAmmo*)(pItem->GetData());
	RECT rect = pItem->GetAbsoluteRect();
	
	pItem->GetFont()->Out(float(rect.left), 
						float(rect.bottom - pItem->GetFont()->CurrentHeight()- 2),
						"%d",	pAmmoItem->m_boxCurr);
}

//��� �������� �� ������� � ����
void InventoryUtilities::FoodUpdateProc(CUIDragDropItem* pItem)
{
	CEatableItem* pEatableItem = (CEatableItem*)(pItem->GetData());
	RECT rect = pItem->GetAbsoluteRect();
	
	if(pEatableItem->m_iPortionsNum>0)
		pItem->GetFont()->Out(float(rect.left), 
							float(rect.bottom - pItem->GetFont()->CurrentHeight()- 2),
							"%d",	pEatableItem->m_iPortionsNum);
}

//���������� �������� �� ������������ ����������� ��� � �������
bool InventoryUtilities::GreaterRoomInRuck(PIItem item1, PIItem item2)
{
	int item1_room = item1->m_iGridWidth*item1->m_iGridHeight;
	int item2_room = item2->m_iGridWidth*item2->m_iGridHeight;

	if(item1_room > item2_room)
		return true;
	else if (item1_room == item2_room)
	{
		if(item1->m_iGridWidth >= item2->m_iGridWidth)
			return true;
	}
   	return false;
}


bool InventoryUtilities::FreeRoom(TIItemList item_list, int width, int height)
{
	BOOL* ruck_room = (BOOL*)xr_malloc(width*height);
	
	R_ASSERT(ruck_room);

	int i,j,k,m;
	int place_row = 0,  place_col = 0;
	bool found_place;
	bool can_place;


	for(i=0; i<height; i++)
		for(j=0; j<width; j++)
			ruck_room[i*width + j] = false;


	ruck_list = item_list;
	
	ruck_list.sort(GreaterRoomInRuck);
	
	found_place = true;

	for(PPIItem it = ruck_list.begin(); (it != ruck_list.end()) && found_place; it++) 
	{
		PIItem pItem = *it;

		//��������� ����� �� ���������� �������,
		//��������� ��������������� ������ ��������
		found_place = false;
	
		for(i=0; (i<height - pItem->m_iGridHeight+1) && !found_place; i++)
		{
			for(j=0; (j<width - pItem->m_iGridWidth +1) && !found_place; j++)
			{
				can_place = true;

				for(k=0; (k<pItem->m_iGridHeight) && can_place; k++)
				{
					for(m=0; (m<pItem->m_iGridWidth) && can_place; m++)
					{
						if(ruck_room[(i+k)*width + (j+m)])
								can_place =  false;
					}
				}
			
				if(can_place)
				{
					found_place=true;	
					place_row = i;
					place_col = j;
				}

			}
		}

		//���������� ������� �� ��������� �����
		if(found_place)
		{
			for(k=0; k<pItem->m_iGridHeight; k++)
			{
				for(m=0; m<pItem->m_iGridWidth; m++)
				{
					ruck_room[(place_row+k)*width + place_col+m] = true;
				}
			}
		}
	}


	xr_free(ruck_room);

	//��� ������-�� �������� ����� �� �������
	if(!found_place) return false;

	return true;
}