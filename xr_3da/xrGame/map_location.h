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
	SMapLocation(const SMapLocation&);

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
	// set this field to texture name you want to use instead of default
	shared_str shader_name;	
	
	//����� �� ���������� ������ (������� �� ���� �����)
	bool marker_show;
	//���������� ����������� ������� ������ ������ ����� (��������)
	bool big_icon;
	//���� ������� ������� � ������ �� ���������
	u32 icon_color;
	//���������� �� �� ���������� ����� ��� ������� (����� �������� �� �����)
	bool global_map_text;

	// ���������� ��� �� ���������� ������� ����� ������������ �� �����
	void	UpdateAnimation();
	// ������ �������� �������� ��� �����
	void	SetColorAnimation(const shared_str &animationName);

	ref_shader&	GetShader();

	// ������������ ���������� � ���������
	bool	dynamic_manifestation;

private:
	// �������� ���������� �� �����
	CUIColorAnimatorWrapper	animation;
	ref_shader	m_iconsShader;
};

DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);
DEFINE_VECTOR (SMapLocation*, LOCATIONS_PTR_VECTOR, LOCATIONS_PTR_VECTOR_IT);
