#pragma once
#include "gameobject.h"
#include "PHDefs.h"

class CSE_ALifeObjectPhysic;

class CPhysicsElement;
class CPhysicObject : public CGameObject {
	typedef CGameObject inherited;
	EPOType				m_type;
	float				m_mass;
	bool				b_removing;
	static u32			remove_time;
	u32					m_unsplit_time;
	PHSHELL_PAIR_VECTOR m_unsplited_shels;
	ref_str				m_startup_anim;
	flags8				m_flags;

private:
	//Creating
			void	CreateSkeleton  	(CSE_ALifeObjectPhysic	*po)													;
			void	CreateBody			(CSE_ALifeObjectPhysic	*po)													;
			void	AddElement			(CPhysicsElement* root_e, int id)												;
			void	Init				()																				;
			void	RespawnInit			()																				;
			void	ClearUnsplited		()																				;
	//Splitting
			void	CopySpawnInit		()																				;
			void	PHSplit				()																				;
			void	UnsplitSingle		(CPhysicObject* O)																;
			void	SpawnCopy			()																				;
	//Autoremove
			bool	ReadyForRemove		()																				;
			void	RecursiveBonesCheck	(u16 id)																		;
			void	SetAutoRemove		()																				;
			void	SaveNetState		(NET_Packet& P)																	;
			void	LoadNetState		(NET_Packet& P)																	;
public:
	CPhysicObject(void);
	virtual ~CPhysicObject(void);

	virtual	void	Hit					(float P, Fvector &dir, CObject* who,s16 element,
											Fvector p_in_object_space, float impulse, ALife::EHitType /**hit_type/**/)	;
	virtual BOOL	net_Spawn			( LPVOID DC)																	;
	virtual void	net_Destroy			()																				;
	virtual void	Load				(LPCSTR section)																;
	virtual void	UpdateCL			( )																				;// Called each frame, so no need for dt
	virtual void	shedule_Update		(u32 dt)																		;	//
	virtual void	OnEvent				(NET_Packet& P, u16 type)														;
	virtual void	net_Export			(NET_Packet& P)																	;
	virtual void	net_Import			(NET_Packet& P)																	;
	virtual BOOL	UsedAI_Locations	()																				;
	virtual void	OnH_A_Independent	()																				;


};
