//////////////////////////////////////////////////////////////////////////
// UIPointerGage.h:			�������� ������
//////////////////////////////////////////////////////////////////////////


#pragma once


#include "UIStatic.h"

class CUIPointerGage: public CUIStatic
{
private:
	typedef CUIStatic inherited;
public:

	CUIPointerGage	();
	virtual ~CUIPointerGage	();


	virtual void Init		(LPCSTR tex_name, 
							int x, int y, int width, int height);

	virtual void InitPointer(LPCSTR arrow_tex_name,  int arrow_offset_x, int arrow_offset_y,
		                     float angle_min, float angle_max);
	
	virtual void Update		();
	virtual void Draw		();

	//value - �� 0 �� 1
	virtual void SetValue	(float value);

protected:
	float m_fAngleMin, m_fAngleMax;
	float m_fAngle;

	//�������� �������
    CUIStaticItem	m_ArrowPointer;
	int m_iArrowOffsetX, m_iArrowOffsetY;
};