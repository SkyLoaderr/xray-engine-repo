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
#include "telewhirlwind.h"

class CMincer :	public CBaseGraviZone
{
private:
	typedef CBaseGraviZone inherited;
	CTeleWhirlwind m_telekinetics;
public:
	CMincer(void);
	virtual ~CMincer(void);
	virtual CTelekinesis& Telekinesis		(){return m_telekinetics;}
	virtual void SwitchZoneState			(EZoneState new_state);
	virtual void Load						(LPCSTR section);
	virtual void Affect						(CObject* O){};
	virtual BOOL net_Spawn					(LPVOID DC);

};