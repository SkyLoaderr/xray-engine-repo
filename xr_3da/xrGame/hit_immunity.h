// hit_immunity.h: ����� ��� ��� ��������, ������� ������������
//				   ������������ ���������� ��� ������ ����� �����
//////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"


//���� ���������� ���������� ������ �����
typedef	svector<float,ALife::eHitTypeMax> HitTypeSVec;

class CHitImmunity
{
public:
	CHitImmunity();
	virtual ~CHitImmunity();

	virtual void LoadImmunities (LPCSTR section);
	virtual void InitImmunities (LPCSTR section,CInifile* ini);

protected:
	//������������ �� ������� ����������� ���
	//��� ��������������� ���� �����������
	//(��� �������� �������� � ����������� ��������)
	HitTypeSVec m_HitTypeK;
};