#ifndef PH_SKELETON_H
#define PH_SKELETON_H

#include "PHDefs.h"
class CPhysicsShellHolder;
class CSE_ALifePHSkeletonObject;
class CSE_Abstract;
class CPHSkeleton
{
	bool				b_removing;
	static u32			remove_time;
	u32					m_unsplit_time;
	PHSHELL_PAIR_VECTOR m_unsplited_shels;

	ref_str				m_startup_anim;
	flags8				m_flags;
private:
	//Creating

	void	Init				()																				;

	void	ClearUnsplited		()																				;
	//Splitting
	void	CopySpawnInit		()																				;
	void	PHSplit				()																				;

	void	SpawnCopy			()																				;
	//Autoremove
	bool	ReadyForRemove		()																				;
	void	RecursiveBonesCheck	(u16 id)																		;
	void	SetAutoRemove		()																				;

protected:
	void	SaveNetState		(NET_Packet& P)																	;
	void	LoadNetState		(NET_Packet& P)																	;
	void	RestoreNetState		(CSE_ALifePHSkeletonObject* po);
	void	UnsplitSingle		(CPHSkeleton* SO)														;

protected:
	virtual CPhysicsShellHolder*		PPhysicsShellHolder	()													=0;
	virtual	void	InitServerObject	(CSE_Abstract	*D)														;
			void	RespawnInit			()																		;
			bool	Spawn				(CSE_ALifePHSkeletonObject* po)											;
			void	Update				(u32 dt)																;
			void	Load				(LPCSTR section)														;
public:
	CPHSkeleton			()																						;
	virtual	~CPHSkeleton		()																				;
};

#endif