// UIMapSpot.h:  ����� �� �����, 
// ��� ������� ���� �������� ��������� ��������
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "UIButton.h"



class CUIMapSpot: public CUIStatic
{
private:
	typedef CUIStatic inherited;
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
};
