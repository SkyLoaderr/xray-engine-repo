#pragma once
#include "inventory_item.h"

class CArtefact : public CInventoryItem
{
private:
	typedef	CInventoryItem	inherited;
public:
	CArtefact(void);
	virtual ~CArtefact(void);

	virtual void Load				(LPCSTR section);
	
	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();
	
	virtual void UpdateCL			();
	virtual void shedule_Update		(u32 dt);	
	
	virtual void			renderable_Render				();
	virtual BOOL			renderable_ShadowGenerate		()		{ return FALSE;	}
	virtual BOOL			renderable_ShadowReceive		()		{ return TRUE;	}

	float GetDetectionDist() {return m_fDetectionDist;}

	virtual void create_physic_shell();

protected:
	//рассто€ние обнаружени€ артифакта детектором
	float		m_fDetectionDist;
	
	//звуки
	ref_str		m_detectorSoundName;
	ref_sound	m_detectorSound;

	u16			m_CarringBoneID;
	
	static xr_set<CArtefact*> m_all;

	ref_str		m_sParticlesName;
	//////////////////////////////////////////////////////////////////////////
	//	Lights
	//////////////////////////////////////////////////////////////////////////
protected:
	//флаг, что подсветка может быть включена
	bool			m_bLightsEnabled;
	//подсветка во врем€ полета и работы двигател€
	IRender_Light*	m_pTrailLight;
	Fcolor			m_TrailLightColor;
	float			m_fTrailLightRange;
protected:
	virtual void	StartLights();
	virtual void	StopLights();
	virtual void	UpdateLights();
};
