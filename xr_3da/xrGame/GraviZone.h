//////////////////////////////////////////////////////////////////////////
// GraviZone.h:		�������������� ��������
//					��� ������� ������� (�������� � �����)
//					��������� � ������ � ������ ��� �����-��
//					�����
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "customzone.h"
#include "ai/telekinesis.h"

class CGraviZone : public CCustomZone,
				   public CPHUpdateObject,
				   public CTelekinesis<CGraviZone>
{
private:
	typedef		CCustomZone					inherited;
	typedef		CTelekinesis<CGraviZone>	TTelekinesis;
public:
	CGraviZone(void);
	virtual ~CGraviZone(void);

	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	//����������� ����� �� ������
	virtual void	Affect(CObject* O);

	virtual void 	PhDataUpdate		(dReal step);
	virtual void 	PhTune				(dReal step) {}

	virtual void	shedule_Update		(u32 dt);
};
