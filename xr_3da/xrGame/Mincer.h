/////////////////////////////////////////////////////
// ���������� ����: "���������"
// ��� ��������� ������ ������� � ���� ���������� 
// ������������� ������
// ���� ��������������� ����� ����� ������������ �����
// (����� m_dwPeriod ���������� � 0 �� m_fMaxPower)
//
/////////////////////////////////////////////////////
#pragma once
#include "gravizone.h"

class CMincer :	public CGraviZone
{
private:
	typedef CGraviZone inherited;
public:
	CMincer(void);
	virtual ~CMincer(void);

	virtual void Load (LPCSTR section);
};