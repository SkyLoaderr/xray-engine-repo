#pragma once
#include "inventory.h"

class CArtifact :
	public CInventoryItem
{
typedef	CInventoryItem	inherited;
public:
	CArtifact(void);
	virtual ~CArtifact(void);

	virtual void Load				(LPCSTR section);
	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();
	virtual void UpdateCL			();
	virtual void shedule_Update		(u32 dt);	
	virtual void renderable_Render	();
	virtual void net_Destroy		();

	void SoundCreate(ref_sound& dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(ref_sound& dest);

	float m_detectorDist;
	LPCSTR m_detectorSoundName;
	ref_sound m_detectorSound;
	float m_jumpHeight;

	static xr_set<CArtifact*> m_all;
};
