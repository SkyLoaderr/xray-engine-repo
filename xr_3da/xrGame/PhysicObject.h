#pragma once
#include "gameobject.h"
#include "PHDefs.h"

class CSE_ALifeObjectPhysic;

class CPhysicsElement;
class CPhysicObject :
	public CGameObject
{
	typedef CGameObject inherited;
	EPOType				m_type;
	float				m_mass;
	bool				b_recalculate;
	bool				b_removing;
static u32				remove_time;
	u32					m_unsplit_time;
	PHSHELL_PAIR_VECTOR m_unsplited_shels;
	void			CreateSkeleton  (CSE_ALifeObjectPhysic	*po);
	void			CreateBody		(CSE_ALifeObjectPhysic	*po);
	void			CopySpawnInit	()							;
	bool			ReadyForRemove	()							;
	void			RecursiveBonesCheck(u16 id)					;
	void			SetAutoRemove	()							;
	void			AddElement		(CPhysicsElement* root_e, int id);
public:
	CPhysicObject(void);
	virtual ~CPhysicObject(void);

	virtual	void	Hit				(float P, Fvector &dir, CObject* who,s16 element,
										Fvector p_in_object_space, float impulse, ALife::EHitType /**hit_type/**/);
	virtual BOOL	net_Spawn		( LPVOID DC);
	virtual void	Load			(LPCSTR section);
	virtual void	UpdateCL		( );									// Called each frame, so no need for dt
	virtual void	shedule_Update	(u32 dt);								//
	virtual void	OnEvent			(NET_Packet& P, u16 type);
	void			PHSplit			();
	void			UnsplitSingle	(CGameObject* O);
	void			SpawnCopy		();
	virtual void	net_Export		(NET_Packet& P);
	virtual void	net_Import		(NET_Packet& P);
	virtual BOOL	UsedAI_Locations();
	virtual void	OnH_A_Independent();
};
