// UIMapSpot.h:  ����� �� �����, 
// ��� ������� ���� �������� ��������� ��������
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "UIButton.h"

#include "../InfoPortion.h"

#define MAP_ICON_WIDTH  32
#define MAP_ICON_HEIGHT 32

extern const int			ARROW_DIMENTIONS;


class CUIMapSpot: public CUIButton
{
private:
	typedef CUIButton inherited;
public:
	CUIMapSpot();
	virtual ~CUIMapSpot();

	virtual void Draw();
	virtual void Update();
	
	//��������� �� ������, ������� ������������
	//�� �����,
	//���� ������ ��� �� NULL
	CObject* m_pObject;
	//������� ����� � ������� ������� ���������,
	//����� ���� m_pObject NULL
	Fvector m_vWorldPos;
	//��� �������
	CUIString m_sNameText;
	//���� ���� ����� � ��������� 
	CUIString m_sDescText;
	// ���/���� �������-���������
	bool m_bArrowEnabled;
	// ������� ��������� ���������
	bool m_bArrowVisible;
	
	//������������� ������ 
	enum EMapSpotAlign {eBottom, eCenter, eNone};
	EMapSpotAlign m_eAlign;
	
	//����������� ������
	bool	m_bHeading;
	float	m_fHeading;

	// C�������� - ���������
	CUIStaticItem m_Arrow;
};
