/////////////////////////////////////////////////////
// ���������� ����: "���������"
// ��� ��������� ������ ������� � ���� ���������� 
// ������������� ������
// ���� ��������������� ����� ����� ������������ �����
// (����� m_dwPeriod ���������� � 0 �� m_fMaxPower)
//
/////////////////////////////////////////////////////
#pragma once
#include "customzone.h"

class CMincer :
	public CCustomZone
{
typedef	CCustomZone	inherited;
public:
	CMincer(void);
	virtual ~CMincer(void);

	virtual BOOL net_Spawn(LPVOID DC);

	//virtual void Update(u32 dt);
	virtual void UpdateCL();
	virtual void Affect(CObject* O);

	virtual void Postprocess(f32 val);
	virtual bool EnableEffector() {return true;}

protected:
	float Power(float dist, float mass);		
	float Impulse(float power, float mass);


	float m_fHitImpulseScale;

	u32 m_dwDeltaTime;


	//������� ����� ����
	float m_fCurrentPower;
	//����������� �����, ��� ������� ����� ��������� ������
	float m_fPowerMin;
	
	//�������� ������, ��������� ����� 
	float m_fSpecificDischarge;
	
	//���� ����������� �� ����-��
	bool m_bDischarging;
};
