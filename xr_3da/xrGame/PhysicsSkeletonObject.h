#ifndef PHYSICS_SKELETON_OBJECT_H
#define PHYSICS_SKELETON_OBJECT_H
#include "physicsshellholder.h"
#include "PHDefs.h"
#include "phnetstate.h"
class CSE_ALifePHSkeletonObject;
class CPhysicsSkeletonObject : public CPhysicsShellHolder
{
typedef CPhysicsShellHolder inherited;
	bool				b_removing;
	static u32			remove_time;
	u32					m_unsplit_time;
	PHSHELL_PAIR_VECTOR m_unsplited_shels;
	ref_str				m_startup_anim;
	flags8				m_flags;

private:
	//Creating
												;
	void	Init				()																				;
	void	RespawnInit			()																				;
	void	ClearUnsplited		()																				;
	//Splitting
	void	CopySpawnInit		()																				;
	void	PHSplit				()																				;
	
	void	SpawnCopy			()																				;
	//Autoremove
	bool	ReadyForRemove		()																				;
	void	RecursiveBonesCheck	(u16 id)																		;
	void	SetAutoRemove		()																				;
	void	SaveNetState		(NET_Packet& P)																	;
	void	LoadNetState		(NET_Packet& P)																	;
	void	RestoreNetState		(PHNETSTATE_VECTOR& saved_bones);
public:
	CPhysicsSkeletonObject(void);
	virtual ~CPhysicsSkeletonObject(void);


	virtual BOOL	net_Spawn			( LPVOID DC)																	;
	virtual void	net_Destroy			()																				;
	virtual	void	CreatePhysicsShell	(CSE_Abstract	*po)												;

	virtual void	Load				(LPCSTR section)																;
	//virtual void	UpdateCL			( )																				;// Called each frame, so no need for dt
	virtual void	shedule_Update		(u32 dt)																		;	//
	virtual void	net_Save			(NET_Packet& P)																	;
	//virtual	BOOL	net_SaveRelevant	()																				;
	//virtual BOOL	UsedAI_Locations	()																				;
protected:
			void	UnsplitSingle		(CPhysicsSkeletonObject* O)														;
	virtual	void	InitServerObject	(CSE_Abstract	*D)															;

};




#endif