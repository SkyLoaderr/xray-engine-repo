//////////////////////////////////////////////////////////////////////////
// GraviZone.h:		�������������� ��������
//////////////////////////////////////////////////////////////////////////
//					������� ��� �� �� 2� ���
//					���� ���������� ������, ������ �������� � 
//					��� ������� ������� (�������� � �����)
//					��������� � ������ � ������ ��� �����-��
//					�����
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "customzone.h"
#include "ai/telekinesis.h"

class CGraviZone : public CCustomZone,
				   public CTelekinesis
{
private:
	typedef		CCustomZone					inherited;
	typedef		CTelekinesis				TTelekinesis;
public:
					CGraviZone(void);
	virtual			~CGraviZone(void);

	virtual void	Load (LPCSTR section);

	virtual BOOL	net_Spawn(LPVOID DC);
	virtual void	net_Destroy();

	//����������� ����� �� ������
	virtual void	Affect(CObject* O);

	virtual void	shedule_Update		(u32 dt);

protected:
	//����� ���������� �������� � ����
	u32				m_dwThrowInTime;
	//���� �������� ���������� � ����
	float			m_fThrowInImpulse;
	
	//������
	float			m_fBlowtRadius;
};
