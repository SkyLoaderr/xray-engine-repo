///////////////////////////////////////////////////////////////
// map_location.h
// ��������� ��� ������� �� �����
///////////////////////////////////////////////////////////////

#pragma once

#include "ui/uistring.h"
#include "infoportiondefs.h"
#include "ui/UIColorAnimatorWrapper.h"

//////////////////////////////////////////////////////////////////////////

enum EMapLocationFlags
{
	eMapLocationPDAContact		= 0x01,
	eMapLocationInfoPortion		= 0x02,
	eMapLocationScript			= 0x04
};

//////////////////////////////////////////////////////////////////////////

struct SMapLocation
{
	SMapLocation();

	shared_str LevelName();

	//���� �������
	flags32 type_flags;

	//��� ������
	shared_str level_name;
	//���������� �� ������
	float x;
	float y;
	//��� �������
	shared_str name;
	//����� ��������
	shared_str text;

	//������������ �� ������� � �������
	bool attached_to_object;
	//id ������� �� ������
	u16 object_id;
	//������ ����� ����������
	INFO_INDEX info_portion_id;

	//������� � ��������� ������
	int icon_x, icon_y, icon_width, icon_height;
	
	//����� �� ���������� ������ (������� �� ���� �����)
	bool marker_show;
	//���� ������� ������� � ������ �� ���������
	u32 icon_color;

	// ���������� ��� �� ���������� ������� ����� ������������ �� �����
	void	UpdateAnimation();
	// ������ �������� �������� ��� �����
	void	SetColorAnimation(const shared_str &animationName);

	// ������������ ���������� � ���������
	bool	dynamic_manifestation;

private:
	// �������� ���������� �� �����
	CUIColorAnimatorWrapper	animation;
};

DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);