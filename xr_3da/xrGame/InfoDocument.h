///////////////////////////////////////////////////////////////
// InfoDocument.h
// InfoDocument - документ, содержащий сюжетную информацию
///////////////////////////////////////////////////////////////


#pragma once

#include "inventory.h"

class CInfoDocument: public CInventoryItem
{
private:
    typedef	CInventoryItem inherited;
public:
	CInfoDocument(void);
	virtual ~CInfoDocument(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();
	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

protected:
	//индекс информации, содержащейся в документе
	int m_iInfoIndex;
};
