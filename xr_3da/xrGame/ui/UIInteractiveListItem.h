//=============================================================================
//  Filename:   UIInteractiveListItem.h
//	---------------------------------------------------------------------------
//  Representation of list item with many interactive fields
//=============================================================================

#ifndef UI_INTERACTIVE_LIST_ITEM_H_
#define UI_INTERACTIVE_LIST_ITEM_H_

#include "UIListItem.h"

// #pragma once

//=============================================================================
//  ������������� ListItem
//=============================================================================

class CUIInteractiveListItem: public CUIListItem
{
	// ��������� ��������� ��� ������������� �������������� ��������
	typedef struct tagInteractiveItemData
	{
		tagInteractiveItemData(): ID(-1) {}
		// ������� ���������� 
		std::pair<int, int>	pairScreenCrd;
		// ����� �������������� ��������
		std::string			subStr;
		// ������������� �������������� �����������
		int					ID;
	} InteractiveItemData;
	// Inherited
	typedef CUIListItem inherited;
public:
	typedef xr_vector<InteractiveItemData> FIELDS_COORDS_VECTOR;
	typedef FIELDS_COORDS_VECTOR::iterator FIELDS_COORDS_VECTOR_it;
	typedef FIELDS_COORDS_VECTOR::const_iterator FIELDS_COORDS_VECTOR_c_it;
	// ���������
	typedef enum { INTERACTIVE_ITEM_CLICK = 7011 } E_MESSAGE;

	CUIInteractiveListItem();
	virtual ~CUIInteractiveListItem() {}
	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);
	virtual void Update();
	virtual void Draw();

	// �������������� ���� �� �������� ������� ������. ������ ��������� � �������
	// ����������� � �����-����������� �������� �������������. ��� ������� ��
	// ������ ��������������� ��������� �� ������� pIDArr ���������� �����e�������
	// ����
	virtual void Init(const char *str, const xr_vector<char *> &Data, xr_vector<int> &IDs, int height, const char StartShift = 0);
	//	// ������ ���� ������ ��������������� ����� ����
	//	virtual void SetStaticTextColor(u32 uColor) {}
	//	// ������ ���� ����� ������������� ����� ���������� ����
	//	virtual void SetInteractiveTextColor(u32 uColor) {}
	// �������������� ������� ��������� ���������� ���������.
	// ��� ����� ��� ��������� ������������� ���������
	virtual RECT GetAbsoluteSubRect();
	// ���/���� ������������� ���������
	bool	m_bInteractiveBahaviour;
	// ��������� ID ������� ��������
	void SetIItemID(const u32 uIndex, const int ID);
	// �������� ���������� ������������� ���������
	u32 GetIFieldsCount();
protected:
	// ���������� ������������� ����� � ������
	FIELDS_COORDS_VECTOR		vPositions;
	// �������� �� ���������� �������� ��������� �������������� ��������
	FIELDS_COORDS_VECTOR_c_it	itCurrIItem;
	// ������� ���������
	std::string	sub_str;
private:
	// �������� ������� ��� �������� ��������� ������� ���� � ��������
	struct mouse_hit: 
		public std::binary_function<FIELDS_COORDS_VECTOR::value_type, 
		std::pair<int, int>, 
		bool>
	{
		bool operator() (const FIELDS_COORDS_VECTOR::value_type &interactiveCrd,
			const std::pair<int, int> &mouseCrd) const
		{
			return interactiveCrd.pairScreenCrd.first <= mouseCrd.first && interactiveCrd.pairScreenCrd.second >= mouseCrd.first;
		}
	};
};

#endif