///////////////////////////////////////////////////////////////
// ArtifactMerger.h
// ArtifactMerger - устройство для получения новых артефактов 
// из имеющихся
///////////////////////////////////////////////////////////////


#pragma once


#include "inventory.h"


class CArtifactMerger : public CInventoryItem
{
typedef	CInventoryItem	inherited;
public:
	CArtifactMerger(void);
	virtual ~CArtifactMerger(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();



};
