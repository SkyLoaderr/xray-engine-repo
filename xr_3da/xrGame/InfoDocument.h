///////////////////////////////////////////////////////////////
// InfoDocument.h
// InfoDocument - ��������, ���������� �������� ����������
///////////////////////////////////////////////////////////////


#pragma once

#include "inventory_item_object.h"

class CInfoDocument: public CInventoryItemObject {
private:
    typedef	CInventoryItemObject inherited;
public:
	CInfoDocument(void);
	virtual ~CInfoDocument(void);

	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();
	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

protected:
	//������ ����������, ������������ � ���������
	int m_iInfoIndex;
};
