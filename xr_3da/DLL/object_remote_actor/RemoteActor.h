// RemoteActor.h: interface for the CRemoteActor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTEACTOR_H__758FD0E0_8120_4BD5_8C84_C19B3674DD40__INCLUDED_)
#define AFX_REMOTEACTOR_H__758FD0E0_8120_4BD5_8C84_C19B3674DD40__INCLUDED_

#pragma once

#include "..\..\customactor.h"

class CRemoteActor : public CCustomActor  
{
private:
	typedef CCustomActor inherited;

	A_NET_export		imported;
	float				fTimeElapsed;
	Fvector				firePos,fireDir;
public:
						CRemoteActor	(void *params);
	virtual				~CRemoteActor	();

//	virtual void		Load			(CInifile* ini, const char* section);

	virtual void		GetFireParams	(Fvector &fire_pos, Fvector &fire_dir);

	virtual void		Die				( );
	virtual void		HitAnother		(CCustomActor* pActor, int Power );

	virtual BOOL		TakeItem		( DWORD CID );

	virtual void		NetworkExport	( );
	virtual void		OnNetworkMessage( );
	virtual void		DoMove			(EMoveCommand move_type, BOOL bSet = false);

	virtual void		OnMoveRelevant	();
};

#endif // !defined(AFX_REMOTEACTOR_H__758FD0E0_8120_4BD5_8C84_C19B3674DD40__INCLUDED_)
