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



class CBaseGraviZone : public CCustomZone
{
private:
	typedef		CCustomZone					inherited;
	//typedef		CTelekinesis				TTelekinesis;
	
public:
					CBaseGraviZone(void);
	virtual			~CBaseGraviZone(void);

	virtual void	Load (LPCSTR section);

	virtual BOOL	net_Spawn(LPVOID DC);
	virtual void	net_Destroy();

	//����������� ����� �� ������
	virtual void	Affect(CObject* O);

	virtual void	shedule_Update		(u32 dt);
	virtual bool	BlowoutState();
	virtual bool	IdleState();

	virtual float	RelativePower(float dist);
protected:
	virtual CTelekinesis& Telekinesis()						=0;
protected:
	//���� �������� ���������� � ���� (��� ���� 100 ��)
	float			m_fThrowInImpulse;
	//���� �������� ���������� � ���� ��� ����� �������
	float			m_fThrowInImpulseAlive;
	//�����. ����������� (��� ������, ��� ������� ����������)
	float			m_fThrowInAtten;
	//������ �������� ������� (� ��������� �� �����)
	float			m_fBlowoutRadiusPercent;

	//��������� ����������	
	float			m_fTeleHeight;
	u32				m_dwTimeToTele;
	u32				m_dwTelePause;
	u32				m_dwTeleTime;
	
	//��� ��������� ����������
	void			PlayTeleParticles(CGameObject* pObject);
	void			StopTeleParticles(CGameObject* pObject);

	ref_str			m_sTeleParticlesBig;
	ref_str			m_sTeleParticlesSmall;
};

class CGraviZone	: public CBaseGraviZone
{
	typedef		CBaseGraviZone				inherited;
	CTelekinesis m_telekinesis;
protected:
	virtual CTelekinesis& Telekinesis()						{return m_telekinesis;}
public:
							CGraviZone		(void)			{}
				virtual		~CGraviZone		(void)			{}
};