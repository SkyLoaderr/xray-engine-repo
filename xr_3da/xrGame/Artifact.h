#pragma once
#include "inventory_item_object.h"
#include "hit_immunity.h"

class CArtefact : public CInventoryItemObject {
private:
	typedef	CInventoryItemObject	inherited;
public:
	CArtefact(void);
	virtual ~CArtefact(void);

	virtual void Load				(LPCSTR section);
	
	virtual BOOL net_Spawn			(CSE_Abstract* DC);
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

	//for smart_cast
	virtual CArtefact*		cast_artefact		()		{return this;}

protected:
	//рассто€ние обнаружени€ артифакта детектором
	float			m_fDetectionDist;
	
	//звуки
	shared_str		m_detectorSoundName;
	ref_sound		m_detectorSound;

	u16				m_CarringBoneID;
	
	static xr_set<CArtefact*> m_all;

	shared_str		m_sParticlesName;
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
	
public:
	//////////////////////////////////////////////////////////////////////////
	// свойства артефакта, когда он висит на по€се у актера
	//////////////////////////////////////////////////////////////////////////
	bool  m_bActorPropertiesEnabled;
	float m_fHealthRestoreSpeed;
	float m_fRadiationRestoreSpeed;
	float m_fSatietyRestoreSpeed;
	float m_fPowerRestoreSpeed;
	float m_fBleedingRestoreSpeed;
	CHitImmunity m_ArtefactHitImmunities;
};